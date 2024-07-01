#pragma once
#ifndef __UNION_RAW_MEMORY_H__
#define __UNION_RAW_MEMORY_H__

#include <Windows.h>
#include <stack>
#include "Memory.h"

namespace Union {
  class RawMemory {
    struct MemoryFrame {
      byte* Memory;
      size_t Length;
    };

    DWORD SavedProtection;
    MemoryFrame MasterFrame;
    std::stack<MemoryFrame> SavedFrames;

    RawMemory( void* start, void* end ) {
      SavedProtection = PAGE_EXECUTE_READWRITE;
      MasterFrame.Memory = reinterpret_cast<byte*>(start);
      MasterFrame.Length = reinterpret_cast<size_t>(end) - reinterpret_cast<size_t>(start);
      VirtualProtect( MasterFrame.Memory, MasterFrame.Length, SavedProtection, &SavedProtection );
      Push();
    }

  public:
    static std::unique_ptr<RawMemory> GetAccess( void* start, void* end ) {
      return std::unique_ptr<RawMemory>{ new RawMemory( start, end ) };
    }

    static std::unique_ptr<RawMemory> GetAccess( void* start, void* end, void(*procedure)(RawMemory& memory) ) {
      auto rawMemory = GetAccess( start, end );
      procedure( *rawMemory.get() );
      return rawMemory;
    }

    byte& operator [] ( size_t index ) {
      return MasterFrame.Memory[index];
    }

    template<typename T>
    T& Get( size_t index ) {
      return *reinterpret_cast<T*>(&MasterFrame.Memory[index]);
    }

    void Push() {
      auto& frame = SavedFrames.emplace();
      frame.Memory = new byte[MasterFrame.Length];
      frame.Length = MasterFrame.Length;
      memcpy( frame.Memory, MasterFrame.Memory, frame.Length );
    }

    void Pop() {
      auto& frame = SavedFrames.top();
      memcpy( MasterFrame.Memory, frame.Memory, frame.Length );
      delete[] frame.Memory;
      SavedFrames.pop();
    }

    virtual ~RawMemory() {
      VirtualProtect( MasterFrame.Memory, MasterFrame.Length, SavedProtection, &SavedProtection );
      while( !SavedFrames.empty() ) {
        auto& frame = SavedFrames.top();
        delete[] frame.Memory;
        SavedFrames.pop();
      }
    }
  };
}

#endif // __UNION_RAW_MEMORY_H__
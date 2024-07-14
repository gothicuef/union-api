#pragma once
#ifndef __UNION_HOOK_DETOURS_H__
#define __UNION_HOOK_DETOURS_H__

#ifdef _UNION_API_LIB
#define DETOURS_INTERNAL
#endif
#include <Windows.h>
#include "Array.h"
#include "Memory.h"
#include "HookProto.h"

#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
#include "Detours.h"
#endif

namespace Union {
  class UNION_API HookProviderDetours : public HookProvider {
    void* OriginalPtr;    // Hook from
    void* DestinationPtr; // Hook to
    void* DetoursPtr;     // Where to return (special detours)
    HookProviderDetours* Prev;
    HookProviderDetours* Next;

    static Array<HookProviderDetours*> GetHookList();
    static HookProviderDetours* GetHookTree( void* ptr );
    HookProviderDetours* GetFirst();
    HookProviderDetours* GetLast();
  public:
    HookProviderDetours();
    virtual bool IsEnabled();
    virtual bool Enable( void* originPtr, void* destPtr );
    virtual bool Enable();
    virtual bool Disable();
    virtual void* GetReturnAddress();
  };


#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
  inline HookProviderDetours::HookProviderDetours() {
    OriginalPtr = nullptr;
    DestinationPtr = nullptr;
    DetoursPtr = nullptr;
    Prev = nullptr;
    Next = nullptr;
  }


  inline HookProviderDetours* HookProviderDetours::GetFirst() {
    return Prev ? Prev->GetFirst() : this;
  }


  inline HookProviderDetours* HookProviderDetours::GetLast() {
    return Next ? Next->GetLast() : this;
  }


  inline HookProviderDetours* HookProviderDetours::GetHookTree( void* ptr ) {
    auto hooks = GetHookList();
    for( auto Hook : hooks )
      if( Hook->OriginalPtr == ptr )
        return Hook;
    
    return nullptr;
  }


  inline Array<HookProviderDetours*> HookProviderDetours::GetHookList() {
    static Array<HookProviderDetours*>* hooks = 
      (Array<HookProviderDetours*>*)CreateSharedSingleton( "DetoursHookList", []() -> void* { return new Array<HookProviderDetours*>(); } );
    return hooks->Share();
  }


  inline void DetourAttachOnce( void** pointer, void* detour ) {
    DetourTransactionBegin();
    DetourUpdateThread( GetCurrentThread() );
    DetourAttach( pointer, detour );
    DetourTransactionCommit();
  }


  inline void DetourDetachOnce( void** pointer, void* detour ) {
    DetourTransactionBegin();
    DetourUpdateThread( GetCurrentThread() );
    DetourDetach( pointer, detour );
    DetourTransactionCommit();
  }


  inline bool HookProviderDetours::IsEnabled() {
    return DetoursPtr != nullptr;
  }


  inline bool HookProviderDetours::Enable( void* from, void* to ) {
    if( !CanHookThisSpace() )
      return false;

    if( from == nullptr || to == nullptr )
      return false;

    if( IsEnabled() )
      return true;

    OriginalPtr = from;
    DestinationPtr = to;
    DetoursPtr = from;

    HookProviderDetours* tree = GetHookTree( from );
    if( tree ) {
      tree = tree->GetLast();
      tree->Next = this;
      this->Prev = tree;
      DetoursPtr = tree->DestinationPtr;
    }
    else
      GetHookList().Insert( this );

    DetourAttachOnce( &DetoursPtr, DestinationPtr );
    return true;
  }


  inline bool HookProviderDetours::Enable() {
    return Enable( OriginalPtr, DestinationPtr );
  }


  inline bool HookProviderDetours::Disable() {
    if( !IsEnabled() )
      return false;

    // Detach this object
    DetourDetachOnce( &DetoursPtr, DestinationPtr );

    // Remove this hook from list
    if( Prev )
      Prev->Next = Next;
    else
      GetHookList().Remove( this );

    // Reattach the next object
    if( Next ) {
      Next->Prev = Prev;
      DetourDetachOnce( &Next->DetoursPtr, Next->DestinationPtr );
      Next->DetoursPtr = nullptr;
      Next->Enable( DetoursPtr, Next->DestinationPtr );
    }

    // Clear a hook information
    DetoursPtr = nullptr;
    Prev = nullptr;
    Next = nullptr;
    return true;
  }

  inline void* HookProviderDetours::GetReturnAddress() {
    return DetoursPtr;
  }
#endif
}

#endif // __UNION_HOOK_DETOURS_H__
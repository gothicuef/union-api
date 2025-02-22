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
  protected:
    void* OriginalPtr;    // Hook from
    void* DestinationPtr; // Hook to
    void* DetoursPtr;     // Where to return (special detours)
    HookProviderDetours* Prev;
    HookProviderDetours* Next;
    bool Enabled;

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
    Enabled = false;
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
    return Enabled;
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
    Enabled = true;
    return true;
  }


  inline bool HookProviderDetours::Enable() {
    return Enable( OriginalPtr, DestinationPtr );
  }


  inline bool HookProviderDetours::Disable() {
    if( !IsEnabled() )
      return false;

    // Now object can free a holded function to 
    // return prologue to the source state
    DetourDetachOnce( &DetoursPtr, DestinationPtr );

    if( Prev ) {
      Prev->Next = Next;
      DetoursPtr = Prev->DestinationPtr;
    }
    // If this object is root node of the hook
    // tree it should be removed from list
    else {
      GetHookList().Remove( this );
      DetoursPtr = OriginalPtr;
    }

    if( Next ) {
      // Now we should disable the next object in this tree and
      // connect to the Prev or the Origin object from this list
      DetourDetachOnce( &Next->DetoursPtr, Next->DestinationPtr );
      Next->Prev = Prev;

      // Set the detours function for the next object, after
      // that get hooked value. Now disabled provider may exit
      // from the destination function if it was disabled from there
      DetourAttachOnce( &DetoursPtr, Next->DestinationPtr );
      Next->DetoursPtr = DetoursPtr;

      if( Prev == nullptr )
        GetHookList().Insert( Next );

      // Triggering vfunc after manual update
      Next->Enable();
    }

    Prev = nullptr;
    Next = nullptr;

    Enabled = false;
    return true;
  }

  inline void* HookProviderDetours::GetReturnAddress() {
    return DetoursPtr;
  }
#endif
}

#endif // __UNION_HOOK_DETOURS_H__
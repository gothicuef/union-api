#pragma once
#ifndef __UNION_HOOK_H__
#define __UNION_HOOK_H__

#include "HookProto.h"
#include "HookDetours.h"
#include "HookPatch.h"
#include "HookPartial.h"
#include "Signature.h"

namespace Union {
  template<typename EntryType>
  inline Hook<EntryType> CreateHook( void* originPtr, EntryType destPtr, HookType type = HookType::Hook_Auto ) {
    if( !HookProvider::CanHookThisSpace() )
      return Hook<EntryType>( nullptr );
    
    HookProvider* provider = nullptr;

    if( originPtr && destPtr ) {
      if( provider == nullptr && type == HookType::Hook_Auto || type == HookType::Hook_CallPatch ) {
        provider = new HookProviderPatch();
        if( !provider->Enable( originPtr, *(void**)&destPtr ) ) {
          delete provider;
          provider = nullptr;
        }
      }

      if( provider == nullptr && type == HookType::Hook_Auto || type == HookType::Hook_Detours ) {
        provider = new HookProviderDetours();
        if( !provider->Enable( originPtr, *(void**)&destPtr ) ) {
          delete provider;
          provider = nullptr;
        }
      }
    }

    return Hook<EntryType>( provider );
  }


  template<typename EntryType>
  inline Hook<EntryType> CreateHook( Signature originSig, EntryType destPtr, HookType type = HookType::Hook_Auto ) {
    if( !HookProvider::CanHookThisSpace() )
      return Hook<EntryType>( nullptr );
    
    auto similarSignature = SignatureFile::FindSimilarSignatureInThisHookspace( &originSig );
    if( similarSignature == null )
      return Hook<EntryType>( nullptr );

    return CreateHook( similarSignature->GetAddress(), destPtr, type );
  }


  inline Hook<PartialHookProc> CreatePartialHook( void* whereFrom, PartialHookProc whereTo ) {
    HookProviderPartial* provider = nullptr;
    if( whereFrom && whereTo ) {
      provider = new HookProviderPartial();
      provider->Enable( whereFrom, whereTo );
    }

    return Hook<PartialHookProc>( provider );
  }


  inline Hook<PartialHookProc> CreatePartialHook( void* whereFrom, PartialHookNoRegsProc whereTo ) {
    return CreatePartialHook( whereFrom, (PartialHookProc)whereTo );
  }
}

#endif // __UNION_HOOK_H__
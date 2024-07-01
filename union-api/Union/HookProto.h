#pragma once
#ifndef __UNION_HOOK_PROTO_H__
#define __UNION_HOOK_PROTO_H__
#include "Types.h"
#include "Signature.h"

namespace Union {
  struct AnyPtr {
    void* VoidPtr;

    template<typename T>
    AnyPtr( T ptr ) {
      VoidPtr = *(void*)&ptr;
    }

    operator void* () {
      return VoidPtr;
    }
  };


  enum class HookType {
    Hook_Auto,
    Hook_Detours,
    Hook_CallPatch
  };


  class HookSpace {
    HookSpace();
  public:
    bool(*Condition)();
    HookSpace( bool(*condition)() );
    HookSpace( bool(*condition)(), const String& fileName );
    HookSpace( bool(*condition)(), int resourceID, const char* type );
    bool IsActive();
    static HookSpace& GetCurrentInstance();
  };


#define HOOKSPACE(nameSpace, condition) \
 static Union::HookSpace HookSpace_##nameSpace( []() -> bool { return (condition); } )

#define HOOKSPACE_WITH_SG_FILE(nameSpace, condition, fileName) \
  static Union::HookSpace HookSpace_##nameSpace( []() -> bool { return (condition); }, fileName )

#define HOOKSPACE_WITH_SG_RC(nameSpace, condition, id, type) \
  static Union::HookSpace HookSpace_##nameSpace( []() -> bool { return (condition); }, id, type )

#define STD_ENGINE_CHECKING GetGameVersion() == ENGINE
#define STD_SG_RC_TYPE "TXT"


  class UNION_API HookProvider {
  public:
    static bool CanHookThisSpace();
    virtual bool IsEnabled() = 0;
    virtual bool Enable( void* originPtr, void* destPtr ) = 0;
    virtual bool Enable() = 0;
    virtual bool Disable() = 0;
    virtual void* GetReturnAddress() = 0;
  };


  template<typename EntryType>
  class Hook {
    HookProvider* Provider;
    Hook() { }
  public:
    Hook( HookProvider* provider );
    Hook( const Hook& other );
    bool Enable( void* originPtr, void* destPtr );
    bool Enable();
    bool Disable();
    operator EntryType();
  };


  inline HookSpace::HookSpace() {
    Condition = nullptr;
  }


  inline HookSpace::HookSpace( bool(*condition)() ) {
    Condition = condition;
    GetCurrentInstance().Condition = condition;
  }


  inline HookSpace::HookSpace( bool(*condition)(), const String& fileName ) {
    Condition = condition;
    GetCurrentInstance().Condition = condition;
    SignatureFile::SwitchCurrentSignatureFile( fileName );
  }


  inline HookSpace::HookSpace( bool(*condition)(), int resourceID, const char* type ) {
    Condition = condition;
    GetCurrentInstance().Condition = condition;
    SignatureFile::SwitchCurrentSignatureFile( resourceID, type );
  }


  inline bool HookSpace::IsActive() {
    return Condition ? Condition() : true;
  }


  inline HookSpace& HookSpace::GetCurrentInstance() {
    static HookSpace* instance =
      (HookSpace*)CreateSharedSingleton( "HookSpace", []() -> void* { return new HookSpace(); } );
    return *instance;
  }


  inline bool HookProvider::CanHookThisSpace() {
    return HookSpace::GetCurrentInstance().IsActive();
  }


  template<typename EntryType>
  Hook<EntryType>::Hook( HookProvider* provider ) {
    Provider = provider;
  }


  template<typename EntryType>
  Hook<EntryType>::Hook( const Hook& other ) {
    Provider = other.Provider;
  }


  template<typename EntryType>
  bool Hook<EntryType>::Enable( void* originPtr, void* destPtr ) {
    return Provider->Enable( originPtr, destPtr );
  }


  template<typename EntryType>
  bool Hook<EntryType>::Enable() {
    return Provider->Enable();
  }


  template<typename EntryType>
  bool Hook<EntryType>::Disable() {
    return Provider->Disable();
  }


  template<typename EntryType>
  Hook<EntryType>::operator EntryType() {
    // Hack for the x64 addresses on x86 arc
    uint64 ptr = (uint64)Provider->GetReturnAddress();
    return *(EntryType*)&ptr;
  }
}

#endif // __UNION_HOOK_PROTO_H__
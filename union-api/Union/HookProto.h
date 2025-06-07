#pragma once
#ifndef __UNION_HOOK_PROTO_H__
#define __UNION_HOOK_PROTO_H__
#include "Types.h"
#include "Signature.h"

#include <type_traits>
#if __cplusplus >= 202002L
#include <bit>
#define UNION_NO_UNIQUE_ADDRESS [[no_unique_address]]
#define UNION_BIT_CAST std::bit_cast
#else
#define UNION_NO_UNIQUE_ADDRESS
#define UNION_BIT_CAST std::_Bit_cast
#endif


namespace Union {
  struct AnyPtr {
    void* VoidPtr;

    template<typename T>
    AnyPtr( T ptr ) {
      VoidPtr = *(void**)&ptr;
    }

    operator void* () {
      return VoidPtr;
    }
  };

  template<typename To, typename From>
  constexpr To bit_cast( const From& from ) noexcept
  {
    return UNION_BIT_CAST<To>( from );
  }

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

#define STD_ENGINE_CHECKING (GetGameVersion() == ENGINE)
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

  using FunctionAddress = void*;

  template<typename EntryType>
  constexpr bool non_trivial_member_function_pointer = (std::is_member_function_pointer_v<EntryType>) && (sizeof( EntryType ) > sizeof( FunctionAddress ));

  template<typename EntryType, bool = non_trivial_member_function_pointer<EntryType>>
  struct FunctionMetadata : std::false_type{};

  template<typename EntryType>
  struct FunctionMetadata<EntryType, true> : std::true_type {
    std::byte Rawdata[sizeof( EntryType ) - sizeof( FunctionAddress )];
  };

  template<typename EntryType, bool = non_trivial_member_function_pointer<EntryType>>
  struct alignas( EntryType ) PointerWrapper {
    void* Address;
  };

  template<typename EntryType>
  struct alignas( EntryType ) PointerWrapper<EntryType, true> {
    void* Address;
    FunctionMetadata<EntryType> Metadata;
  };

  template<typename T>
  using HookMetadata = FunctionMetadata<T>;

  template<typename EntryType>
  class Hook {
    HookProvider* Provider{};
    UNION_NO_UNIQUE_ADDRESS const HookMetadata<EntryType> Metadata{};
    Hook() { }
  public:
    Hook( HookProvider* provider, const HookMetadata<EntryType>* metadata = nullptr );
    Hook( const Hook& other );
    bool Enable( void* originPtr, void* destPtr );
    bool Enable();
    bool Disable();
    operator EntryType() const;
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
  Hook<EntryType>::Hook( HookProvider* provider, const HookMetadata<EntryType>* metadata )
      : Provider{ provider }, Metadata{ metadata ? *metadata : HookMetadata<EntryType>{} } {
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
  Hook<EntryType>::operator EntryType() const {
    if constexpr(!non_trivial_member_function_pointer<EntryType>) {
      return bit_cast<EntryType>( Provider->GetReturnAddress() );
    }
    else { 
      const PointerWrapper<EntryType> wrapper{
       .Address = Provider->GetReturnAddress(),
       .Metadata = this->Metadata
      };
      return bit_cast<EntryType>( wrapper );
    }
  }
}

#endif // __UNION_HOOK_PROTO_H__
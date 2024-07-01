#pragma once
#ifndef __UNION_HOOK_PARTIAL_H__
#define __UNION_HOOK_PARTIAL_H__

#ifdef _UNION_API_LIB
#define DETOURS_INTERNAL
#endif
#include <Windows.h>
#include "Memory.h"
#include "Array.h"
#include "Memory.h"
#include "HookProto.h"

#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
#include "Detours.h"
#endif

namespace Union {
#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
  inline void x86_set_jump_offset( byte* from, byte* to ) {
    *(DWORD*)from = to - from - 4;
  }

  inline void x86_set_jump( byte* from, byte* to ) {
    from[0] = 0xE9;
    x86_set_jump_offset( &from[1], to );
  }

  inline byte* x86_get_jump_address( byte* from ) {
    return &from[0] + *(DWORD*)&from[1] + 5;
  }

  inline void x86_set_jump_dword_ptr( byte* from, DWORD to ) {
    from[0] = 0xFF;
    from[1] = 0x25;
    *(DWORD*)&from[2] = to;
  }

  inline void x86_set_call( byte* from, byte* to ) {
    from[0] = 0xE8;
    x86_set_jump_offset( &from[1], to );
  }

  inline void x86_set_call_dword_ptr( byte* from, DWORD to ) {
    from[0] = 0xFF;
    from[1] = 0x15;
    *(DWORD*)&from[2] = to;
  }

  inline void x86_fill_pack_unpack_registers( byte code, byte* ptr, DWORD* dwordPtrs ) {
#pragma pack(push, 1)
    struct Instruction {
      byte Code;
      byte Register;
      DWORD DwordPtr;
    };
#pragma pack(pop)
    Instruction* instructions = (Instruction*)ptr;
    for( int i = 0; i < 8; i++ ) {
      auto& instruction = instructions[i];
      instruction.Code = code;
      instruction.Register = 0x05 + 8 * i;
      instruction.DwordPtr = (DWORD)&dwordPtrs[i];
    }
  }

  inline void x86_set_pack_registers( byte* ptr, DWORD* dwordPtrs ) {
    x86_fill_pack_unpack_registers( 0x89, ptr, dwordPtrs );
  }

  inline void x86_set_unpack_registers( byte* ptr, DWORD* dwordPtrs ) {
    x86_fill_pack_unpack_registers( 0x8B, ptr, dwordPtrs );
  }

  inline void x86_set_push_dword( byte* ptr, DWORD dword ) {
    ptr[0] = 0x68;
    *(DWORD*)&ptr[1] = dword;
  }

  inline void x86_set_reg_dword( byte* ptr, int reg, DWORD dword ) {
    ptr[0] = 0xB8 + reg;
    *(DWORD*)&ptr[1] = dword;
  }

  inline void x86_set_mov_dword_to_ptr( byte* ptr, DWORD* dwordPtrs, DWORD dword ) {
    ptr[0] = 0xC7;
    ptr[1] = 0x05;
    *(DWORD*)&ptr[2] = (DWORD)dwordPtrs;
    *(DWORD*)&ptr[6] = dword;
  }
#endif


  struct Registers {
    DWORD eax;
    DWORD ecx;
    DWORD edx;
    DWORD ebx;
    DWORD esp;
    DWORD ebp;
    DWORD esi;
    DWORD edi;
    DWORD eip;
  };

  typedef void( __fastcall* PartialHookProc )(Registers&);
  typedef void( __fastcall* PartialHookNoRegsProc )();


  class UNION_API PartialHookTrampolineNode {
    friend class PartialHookTrampoline;
    friend class HookProviderPartial;
    struct {
      byte PushRegisters[5];
      byte CallForProcedure[5];
      byte JumpBack[5];
      ulong Protection;
      operator byte* () { return (byte*)this; }
    } Opcode;
  public:

    PartialHookTrampolineNode();
    void SetProcedure( PartialHookProc where );
    void SetReturn( const void* where );
    byte* GetReturn();
    void SetRegisters( const void* where );
    ~PartialHookTrampolineNode();
  };


  class UNION_API PartialHookTrampoline {
    friend class HookProviderPartial;
    struct {
      byte PushRegisters[6*8];
      byte PushF;
      byte UpdateEIP[10];
      byte JumpToFirstNode[5];
      byte PopF;
      byte PopRegisters[6*8];
      byte JumpBack[6];
      byte OriginalCode[15];
      byte JumpBackDefault[6];
      ulong Protection;
      operator byte* () { return (byte*)this; }
    } Opcode;

    byte* ReturnAddress;
    Registers Registers;
    byte* SavedCodePosition;
    byte SavedCode[15];
    uint SavedCodeLength;
  public:
    PartialHookTrampoline();
    void Initialize( void* whereFrom );
    void SetBaseNode( PartialHookTrampolineNode* node );
    void Restore();
    ~PartialHookTrampoline();
  };


  class UNION_API HookProviderPartial : public HookProvider {
    void* BaseAddress;  // Hook from
    void* Function;     // Hook to

    HookProviderPartial* Prev;
    HookProviderPartial* Next;
    PartialHookTrampolineNode* TrampolineNode;
    PartialHookTrampoline* Trampoline;

    static Array<HookProviderPartial*> GetHookList();
    static HookProviderPartial* GetHookTree( void* ptr );
    HookProviderPartial* GetFirst();
    HookProviderPartial* GetLast();

  public:
    HookProviderPartial();
    virtual bool IsEnabled();
    virtual bool Enable( void* originPtr, void* destPtr );
    virtual bool Enable();
    virtual bool Disable();
    virtual void* GetReturnAddress();
    void SetReturnAddress( void* address );
    void ResetReturnAddress();
  };


#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
  inline PartialHookTrampolineNode::PartialHookTrampolineNode() {
    memset( &Opcode, 0x90, sizeof( Opcode ) );
    x86_set_jump( Opcode.CallForProcedure, Opcode.CallForProcedure );
    x86_set_jump( Opcode.JumpBack, Opcode.JumpBack );
    Opcode.Protection = PAGE_EXECUTE_READWRITE;
    VirtualProtect( &Opcode, sizeof( Opcode ), Opcode.Protection, &Opcode.Protection );
  }


  inline void PartialHookTrampolineNode::SetProcedure( PartialHookProc where ) {
    x86_set_call( Opcode.CallForProcedure, (byte*)where );
  }


  inline void PartialHookTrampolineNode::SetReturn( const void* where ) {
    x86_set_jump( Opcode.JumpBack, (byte*)where );
  }


  inline byte* PartialHookTrampolineNode::GetReturn() {
    return x86_get_jump_address( Opcode.JumpBack );
  }


  inline void PartialHookTrampolineNode::SetRegisters( const void* where ) {
    // x86_set_push_dword( Opcode.PushRegisters, (DWORD)where );
    x86_set_reg_dword( Opcode.PushRegisters, 1/*ecx*/, (DWORD)where );
  }


  inline PartialHookTrampolineNode::~PartialHookTrampolineNode() {
    VirtualProtect( &Opcode, sizeof( Opcode ), Opcode.Protection, &Opcode.Protection );
  }


  inline PartialHookTrampoline::PartialHookTrampoline() {
    memset( &Opcode, 0x90, sizeof( Opcode ) );
    Opcode.PushF = 0x9C;
    Opcode.PopF = 0x9D;
    x86_set_pack_registers( Opcode.PushRegisters, &Registers.eax );
    x86_set_unpack_registers( Opcode.PopRegisters,  &Registers.eax );
    x86_set_jump( Opcode.JumpToFirstNode, Opcode.JumpToFirstNode );
    x86_set_jump( Opcode.JumpBack, Opcode.JumpBack );
    Opcode.Protection = PAGE_EXECUTE_READWRITE;
    VirtualProtect( &Opcode, sizeof( Opcode ), Opcode.Protection, &Opcode.Protection );
  }


  inline void PartialHookTrampoline::Initialize( void* whereFrom ) {
    byte* it = (byte*)whereFrom;
    byte* nearEnd = it + 5;
    byte* farEnd = it;
    byte* whereTo = Opcode.OriginalCode;

    long extra = 0;
    while( it < nearEnd ) {
      farEnd = (byte*)DetourCopyInstruction( whereTo, nullptr, it, nullptr, &extra );
      long length = farEnd - it;
      whereTo += length + extra;
      it = farEnd;
    }

    SavedCodePosition = (byte*)whereFrom;
    SavedCodeLength = farEnd - (byte*)whereFrom;
    memcpy( SavedCode, whereFrom, SavedCodeLength );
    
    ulong protection = PAGE_EXECUTE_READWRITE;
    VirtualProtect( whereFrom, SavedCodeLength, protection, &protection );
    x86_set_jump( (byte*)whereFrom, Opcode );
    VirtualProtect( whereFrom, SavedCodeLength, protection, &protection );
    x86_set_jump_dword_ptr( Opcode.JumpBack, (DWORD)&Registers.eip);
    x86_set_jump_dword_ptr( Opcode.JumpBackDefault, (DWORD)&ReturnAddress );
    Registers.eip = (DWORD)Opcode.OriginalCode;
    ReturnAddress = farEnd;
    x86_set_mov_dword_to_ptr( Opcode.UpdateEIP, &Registers.eip, Registers.eip );
  }


  inline void PartialHookTrampoline::SetBaseNode( PartialHookTrampolineNode* node ) {
    x86_set_jump( Opcode.JumpToFirstNode, node ? node->Opcode : Opcode.JumpToFirstNode );
  }


  inline void PartialHookTrampoline::Restore() {
    if( SavedCodePosition == nullptr )
      return;

    ulong protection = PAGE_EXECUTE_READWRITE;
    VirtualProtect( SavedCodePosition, SavedCodeLength, protection, &protection );
    memcpy( SavedCodePosition, SavedCode, SavedCodeLength );
    VirtualProtect( SavedCodePosition, SavedCodeLength, protection, &protection );
    SavedCodePosition = nullptr;
  }


  inline PartialHookTrampoline::~PartialHookTrampoline() {
    Restore();
    VirtualProtect( &Opcode, sizeof( Opcode ), Opcode.Protection, &Opcode.Protection );
  }


  inline HookProviderPartial::HookProviderPartial() {
    BaseAddress = nullptr;
    Function = nullptr;
    TrampolineNode = nullptr;
    Trampoline = nullptr;
    Prev = nullptr;
    Next = nullptr;
  }


  inline Array<HookProviderPartial*> HookProviderPartial::GetHookList() {
    static Array<HookProviderPartial*>* hooks =
      (Array<HookProviderPartial*>*)CreateSharedSingleton( "PartialHookList", []() -> void* { return new Array<HookProviderPartial*>(); } );
    return hooks->Share();
  }


  inline HookProviderPartial* HookProviderPartial::GetHookTree( void* ptr ) {
    auto hooks = GetHookList();
    for( auto Hook : hooks ) {
      int diff = (int)Hook->BaseAddress - (int)ptr;
      if( diff >= -5 && diff <= 5 )
        return Hook;
    }

    return nullptr;
  }


  inline HookProviderPartial* HookProviderPartial::GetFirst() {
    return Prev ? Prev->GetFirst() : this;
  }


  inline HookProviderPartial* HookProviderPartial::GetLast() {
    return Next ? Next->GetLast() : this;
  }


  inline bool HookProviderPartial::IsEnabled() {
    return TrampolineNode != nullptr;
  }


  inline bool HookProviderPartial::Enable( void* baseAddress, void* function ) {
    if( !CanHookThisSpace() )
      return false;

    if( !baseAddress || !function )
      return false;

    if( IsEnabled() )
      return true;

    BaseAddress = baseAddress;
    Function = function;

    HookProviderPartial* tree = GetHookTree( baseAddress );
    if( tree ) {
      BaseAddress = tree->BaseAddress;
      Trampoline = tree->Trampoline;
      tree = tree->GetLast();
      tree->Next = this;
      this->Prev = tree;
      TrampolineNode = new PartialHookTrampolineNode();
      tree->TrampolineNode->SetReturn( TrampolineNode->Opcode );
    }
    else {
      Trampoline = new PartialHookTrampoline();
      TrampolineNode = new PartialHookTrampolineNode();
      Trampoline->Initialize( baseAddress );
      Trampoline->SetBaseNode( TrampolineNode );
      TrampolineNode->SetReturn( Trampoline->Opcode.PopRegisters );
      GetHookList().Insert( this );
    }

    TrampolineNode->SetProcedure( (PartialHookProc)function );
    TrampolineNode->SetReturn( Trampoline->Opcode.PopRegisters );
    TrampolineNode->SetRegisters( &Trampoline->Registers.eax );
    return true;
  }


  inline bool HookProviderPartial::Enable() {
    if( !BaseAddress || !Function )
      return false;

    return Enable( BaseAddress, Function );
  }


  inline bool HookProviderPartial::Disable() {
    if( !TrampolineNode )
      return true;

    if( !Prev ) {
      GetHookList().Remove( this );
      if( Next ) {
        GetHookList().Insert( Next );
        Trampoline->SetBaseNode( Next->TrampolineNode );
        if( Trampoline->Registers.eip != (DWORD)Trampoline->ReturnAddress )
          Trampoline->Registers.eip = (DWORD)Trampoline->ReturnAddress;
      }
      else
        delete Trampoline;
    }
    else {
      Prev->TrampolineNode->SetReturn( TrampolineNode->GetReturn() );
      Prev->Next = Next;
    }

    if( Next )
      Next->Prev = Prev;

    delete TrampolineNode;
    TrampolineNode = nullptr;
    Trampoline = nullptr;
    Prev = nullptr;
    Next = nullptr;
    return true;
  }


  inline void* HookProviderPartial::GetReturnAddress() {
    return nullptr;
  }


  inline void HookProviderPartial::SetReturnAddress( void* address ) {
    if( TrampolineNode )
      TrampolineNode->SetReturn( address );
  }


  inline void HookProviderPartial::ResetReturnAddress() {
    if( !IsEnabled() )
      return;

    TrampolineNode->SetReturn( Next ? Next->TrampolineNode->Opcode : Trampoline->Opcode.PopRegisters );
  }
#endif
}

#endif // __UNION_HOOK_PARTIAL_H__
#pragma once
#ifndef __UNION_LIBRARY_H__
#define __UNION_LIBRARY_H__
#pragma warning(push)
#pragma warning(disable:4251)

#include "String.h"
#include <dbghelp.h>
#include <Psapi.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

namespace Union {
  class UNION_API Dll {
  protected:
    HANDLE Handle;
    StringANSI Name;
    StringANSI FullName;

    Dll();
    virtual ~Dll();
  public:
    virtual HANDLE GetHandle() const;
    virtual StringANSI GetName() const;
    virtual void* GetProcedureAddress( const char* procName ) const;
    virtual void* GetResource( LPCTSTR resourceName, LPCTSTR resourceType ) const;
    virtual void* GetResourceEx( LPCTSTR resourceName, LPCTSTR resourceType, WORD language ) const;
    virtual const void* GetResourceData( void* resource ) const;
    virtual void FreeResourceData( const void* resourceData ) const;
    virtual size_t GetResourceLength( void* resource ) const;
    virtual IMAGE_NT_HEADERS* GetNTHeaders() const;
    virtual bool GetRange( void*& imageBase, size_t& imageLength ) const;
    virtual void Acquire();
    virtual void Release();
    virtual void Forget();

    static Dll* Load( const char* dllName, bool asResource = false );
    static Dll* Load( const wchar_t* dllName, bool asResource = false );
    static Dll* Find( const StringANSI& name );
    static Dll* Find( HANDLE module );
    static HANDLE FindNearestModule( void* where = &CreateSharedSingleton );
  protected:
    static Array<Dll*>& GetDllList();
  };


#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
  inline Dll::Dll() : Handle( nullptr ) {
    GetDllList().Insert( this );
  }


  inline HANDLE Dll::GetHandle() const {
    return Handle;
  }


  inline StringANSI Dll::GetName() const {
    return Name;
  }


  inline void* Dll::GetProcedureAddress( const char* procName ) const {
    return ::GetProcAddress( (HMODULE)Handle, procName );
  }


  inline void* Dll::GetResource( LPCTSTR resourceName, LPCTSTR resourceType ) const {
    return ::FindResource( (HMODULE)Handle, resourceName, resourceType );
  }


  inline void* Dll::GetResourceEx( LPCTSTR resourceName, LPCTSTR resourceType, WORD language ) const {
    return ::FindResourceEx( (HMODULE)Handle, resourceName, resourceType, language );
  }


  inline const void* Dll::GetResourceData( void* resource ) const {
    return ::LoadResource( (HMODULE)Handle, (HRSRC)resource );
  }


  inline void Dll::FreeResourceData( const void* resourceData ) const {
    ::FreeResource( (HGLOBAL)resourceData );
  }


  inline size_t Dll::GetResourceLength( void* resource ) const {
    return ::SizeofResource( (HMODULE)Handle, (HRSRC)resource );
  }


  inline IMAGE_NT_HEADERS* Dll::GetNTHeaders() const {
    return ImageNtHeader( (HMODULE)Handle );
  }


  inline bool Dll::GetRange( void*& imageBase, size_t& imageLength ) const {
    MODULEINFO sysModuleInfo;
    memset( &sysModuleInfo, 0, sizeof( sysModuleInfo ) );
    if( GetModuleInformation( GetCurrentProcess(), (HMODULE)Handle, &sysModuleInfo, sizeof( sysModuleInfo ) ) == 0 )
      return false;

    imageBase = sysModuleInfo.lpBaseOfDll;
    imageLength = sysModuleInfo.SizeOfImage;
    return true;
  }


  inline void Dll::Acquire() {
    ::LoadLibraryA( FullName );
  }


  inline void Dll::Release() {
    ::FreeLibrary( (HMODULE)Handle );
  }


  inline void Dll::Forget() {
    delete this;
  }


  inline Dll::~Dll() {
    GetDllList().Remove( this );
  }


  inline Dll* Dll::Load( const char* dllName, bool asResource ) {
    StringANSI fullName = dllName;
    StringANSI shortName = fullName.GetFileName();

    Dll* dll = Find( shortName );
    if( dll ) {
      dll->Acquire();
      return dll;
    }

    void* module = ::LoadLibraryExA( fullName, nullptr,
      asResource ? LOAD_LIBRARY_AS_IMAGE_RESOURCE : 0 );

    if( !module )
      return nullptr;

    dll = new Dll();
    dll->Handle = module;
    dll->Name = shortName;
    return dll;
  }


  inline Dll* Dll::Load( const wchar_t* dllName, bool asResource ) {
    StringANSI fullName;
    StringConverter::UTF16ToANSI( dllName, fullName );
    StringANSI shortName = fullName.GetFileName();

    Dll* dll = Find( shortName );
    if( dll ) {
      dll->Acquire();
      return dll;
    }

    void* module = ::LoadLibraryExA( fullName, nullptr,
      asResource ? LOAD_LIBRARY_AS_IMAGE_RESOURCE : 0 );

    if( !module )
      return nullptr;

    dll = new Dll();
    dll->Handle = module;
    dll->Name = shortName;
    return dll;
  }


  inline Dll* Dll::Find( const StringANSI& name ) {
    auto& libraries = GetDllList();
    StringANSI shortName = name.GetFileName();
    for( auto&& it : libraries )
      if( it->Name == shortName )
        return it;

    return nullptr;
  }


  inline Dll* Dll::Find( HANDLE handle ) {
    auto& libraries = GetDllList();
    for( auto&& it : libraries )
      if( it->Handle == handle )
        return it;

    char buffer[1024];
    memset( buffer, 0, sizeof( buffer ) );
    if( GetModuleFileNameA( (HMODULE)handle, buffer, sizeof( buffer ) - 1 ) ) {
      Dll* dll = new Dll();
      dll->Handle = handle;
      dll->FullName = buffer;
      dll->Name = dll->FullName.GetFileName();
      libraries.Insert( dll );
      return dll;
    }

    return nullptr;
  }


  inline HANDLE Dll::FindNearestModule( void* where ) {
    ulong nearest = 0;
    auto& libraries = GetDllList();
    for( auto&& it : libraries ) {
      ulong address = (ulong)it->Handle;
      if( address <= (ulong)where && address > nearest )
        nearest = address;
    }

    ulong address;
    if( GetModuleHandleEx(
      GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
      (LPCTSTR)where,
      (HMODULE*)&address ) ) {
      if( address <= (ulong)where && address > nearest )
        nearest = address;
    }

    return (HANDLE)nearest;
  }


  inline Array<Dll*>& Dll::GetDllList() {
    static Array<Dll*>* list =
      (Array<Dll*>*)CreateSharedSingleton( "DllList", []() -> void* { return new Array<Dll*>(); } );
    return *list;
  }
#endif
}

#pragma warning(pop)
#endif // __UNION_LIBRARY_H__
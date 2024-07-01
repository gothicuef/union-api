#include <Windows.h>
#include <malloc.h>
#include <psapi.h>
#include "LDR.h"
#include "Types.h"
#include "Memory.h"
#include "Dll.h"
#include "Thread.h"
#include "HookPatch.h"
#pragma comment(lib, "psapi.lib")

namespace Union {
  typedef void* (*MallocFunction)(size_t size);
  typedef void* (*CallocFunction)(size_t count, size_t size);
  typedef void* (*ReallocFunction)(void* memory, size_t size);
  typedef void (*FreeFunction)(void* memory);
  typedef size_t( *MsizeFunction )(void* memory);


  struct SharedSingleton {
    char* Name;
    void* Address;
    int Shares;
    SharedSingleton( const char* name, void* address );
    void* operator new(size_t size);
    void operator delete(void* memory);
    ~SharedSingleton();
  };


  struct SharedMemory {
    SharedSingleton** Singletons;
    int SingletonsCount;
    Dll* Dll;

    MallocFunction Malloc;
    CallocFunction Calloc;
    ReallocFunction Realloc;
    FreeFunction Free;
    MsizeFunction Msize;

    SharedMemory();
    void InitializeLDR();
    virtual int SearchSingleton( const char* singletonName );
    virtual void Insert( const char* singletonName, void* address );
    virtual void* Share( const char* singletonName );
    virtual void* Release( const char* singletonName );
    void* operator new(size_t size);
    void operator delete(void* memory);
    static void InitializeInstance();
    static void InitializeDll();
    static void __stdcall DllLoadCallback( ulong notificationReason, PLDR_DLL_NOTIFICATION_DATA notificationData, void* context );
    static SharedMemory& GetInstance();
  };


  SharedMemory::SharedMemory() {
    Singletons = nullptr;
    SingletonsCount = 0;
    Dll = nullptr;

    Malloc  = &malloc;
    Calloc  = &calloc;
    Realloc = &realloc;
    Free    = &free;
    Msize   = &_msize;

    HMODULE module = GetModuleHandleA( "shw32.dll" );
    if( module /*&& module != Dll::FindNearestModule()*/ ) {
      void* shi_functions[] = {
        GetProcAddress( module, "shi_malloc" ),
        GetProcAddress( module, "shi_calloc" ),
        GetProcAddress( module, "shi_realloc" ),
        GetProcAddress( module, "shi_free" ),
        GetProcAddress( module, "shi_msize" ),
        GetProcAddress( module, "shi_MemInitDefaultPool" )
      };

      for( auto&& it : shi_functions )
        if( it == nullptr )
          return;
      
      Malloc  = (MallocFunction)  shi_functions[0];
      Calloc  = (CallocFunction)  shi_functions[1];
      Realloc = (ReallocFunction) shi_functions[2];
      Free    = (FreeFunction)    shi_functions[3];
      Msize   = (MsizeFunction)   shi_functions[4];

      ((int(*)())shi_functions[5])();
    }
  }


  SharedSingleton::SharedSingleton( const char* name, void* address ) {
    size_t nameLength = strlen( name ) + 1;
    Name = (char*)::malloc(nameLength);
    memcpy( Name, name, nameLength );
    Address = address;
    Shares = 1;
  }


  SharedSingleton::~SharedSingleton() {
    ::free( Name );
  }


  void* SharedSingleton::operator new(size_t size) {
    return ::malloc( size );
  }


  void SharedSingleton::operator delete(void* memory) {
    ::free( memory );
  }


  int SharedMemory::SearchSingleton( const char* singletonName ) {
    for( int i = 0; i < SingletonsCount; i++ )
      if( strcmp( Singletons[i]->Name, singletonName ) == 0 )
        return i;
    
    return -1;
  }


  void SharedMemory::Insert( const char* singletonName, void* address ) {
    int index = SearchSingleton( singletonName );
    if( index == -1 ) {
      index = SingletonsCount++;
      Singletons = (SharedSingleton**)::realloc( Singletons, SingletonsCount * sizeof( SharedSingleton* ) );
      Singletons[index] = new SharedSingleton( singletonName, address );
    }
    else
      Singletons[index]->Shares++;
  }


  void* SharedMemory::Share( const char* singletonName ) {
    int index = SearchSingleton( singletonName );
    if( index == -1 )
      return nullptr;

    Singletons[index]->Shares++;
    return Singletons[index]->Address;
  }


  void* SharedMemory::Release( const char* singletonName ) {
    int index = SearchSingleton( singletonName );
    if( index == -1 )
      return nullptr;

    if( --Singletons[index]->Shares == 0 ) {
      void* address = Singletons[index]->Address;
      delete Singletons[index];
      memcpy( &Singletons[index], &Singletons[--SingletonsCount], sizeof( SharedSingleton* ) );
      Singletons = (SharedSingleton**)::realloc( Singletons, SingletonsCount * sizeof( SharedSingleton* ) );
      return address;
    }

    return nullptr;
  }


  void* SharedMemory::operator new(size_t size) {
    return ::malloc( size );
  }


  void SharedMemory::operator delete(void* memory) {
    ::free( memory );
  }


  _export_c_ SharedMemory* UnionSharedMemoryInstance = nullptr;


  void SharedMemory::InitializeInstance() {
    DWORD processId = GetCurrentProcessId();
    HMODULE modules[1024];
    HANDLE process;
    DWORD cbNeeded;

    process = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );
    if( process != nullptr ) {
      if( EnumProcessModules( process, modules, sizeof( modules ), &cbNeeded ) ) {
        DWORD length = cbNeeded / sizeof( HMODULE );
        for( DWORD i = 0; i < length; i++ ) {
          auto address = GetProcAddress( modules[i], "UnionSharedMemoryInstance");
          if( address ) {
            auto object = *(SharedMemory**)address;
            if( object ) {
              CloseHandle( process );
              UnionSharedMemoryInstance = object;
              return;
            }
          }
        }
      }

      CloseHandle( process );
    }
    
    UnionSharedMemoryInstance = new SharedMemory();
    StringANSI::Format( "New shared memory was initialized: {0}",
      ToHEX( UnionSharedMemoryInstance ) ).StdPrintLine();

    UnionSharedMemoryInstance->InitializeLDR();
  }


  void SharedMemory::InitializeLDR() {
    PVOID registrationHandle;
    NTSTATUS status = LdrRegisterDllNotification( 0, DllLoadCallback, NULL, &registrationHandle );
  }


  void SharedMemory::DllLoadCallback( ulong notificationReason, PLDR_DLL_NOTIFICATION_DATA notificationData, void* context ) {
    static StringUTF16 gameDirectory = [] {
      wchar_t moduleFileName[2048];
      GetModuleFileNameW( GetModuleHandleW( nullptr ), moduleFileName, sizeof( moduleFileName ) / sizeof( wchar_t ) - 1 );
      return StringUTF16( moduleFileName ).GetDirectory().GetDirectory();
      }();

      switch( notificationReason ) {
      case LDR_DLL_NOTIFICATION_REASON_LOADED:
      {
        StringUTF16 dllPath = notificationData->Loaded.FullDllName->Buffer;
        if( dllPath.StartsWith( gameDirectory, StringBase::IgnoreCase ) ) {
          StringUTF16::Format( L"[+] {0}", dllPath ).StdPrintLine();
          auto dll = Dll::Find( notificationData->Loaded.DllBase );
          if( !ProcessImm32Collection::GetInstance().IsInCollection( dll ) ) {
            ProcessImm32Collection::GetInstance().AnalizeModule( dll );
            HookProviderPatch::UpdateInRange( dll );
          }
        }
        break;
      }
      case LDR_DLL_NOTIFICATION_REASON_UNLOADED:
      {
        StringUTF16 dllPath = notificationData->Unloaded.FullDllName->Buffer;
        if( dllPath.StartsWith( gameDirectory, StringBase::IgnoreCase ) ) {
          StringUTF16::Format( L"[-] {0}", dllPath ).StdPrintLine();
          auto dll = Dll::Find( notificationData->Unloaded.DllBase );
          if( ProcessImm32Collection::GetInstance().IsInCollection( dll ) ) {
            HookProviderPatch::ReleaseInRange( dll );
            ProcessImm32Collection::GetInstance().ReleaseModule( dll );
          }
          dll->Forget();
        }
        break;
      }
      default:
        StringUTF16::Format( L"[?]" ).StdPrintLine();
        break;
      }
  }


  void SharedMemory::InitializeDll() {
    HANDLE handle = Dll::FindNearestModule();
    if( handle ) {
      UnionSharedMemoryInstance->Dll = Dll::Find( handle );
      ProcessImm32Collection::GetInstance().AnalizeModule( UnionSharedMemoryInstance->Dll );
      HookProviderPatch::UpdateInRange( UnionSharedMemoryInstance->Dll );
    }
  }


  SharedMemory& SharedMemory::GetInstance() {
    if( UnionSharedMemoryInstance == nullptr ) {
      InitializeInstance();
#ifndef _UNION_API_BUILD
      InitializeDll();
#endif
    }
    return *UnionSharedMemoryInstance;
  }


// #if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
  /* UNION_API */ void* MemAlloc( size_t size ) {
    /*static*/ auto proc = SharedMemory::GetInstance().Malloc;
    return proc( size );
  }


  /* UNION_API */ void* MemCalloc( size_t count, size_t size ) {
    /*static*/ auto proc = SharedMemory::GetInstance().Calloc;
    return proc( count, size );
  }


  /* UNION_API */ void* MemRealloc( void* memory, size_t size ) {
    /*static*/ auto proc = SharedMemory::GetInstance().Realloc;
    return proc( memory, size );
  }


  /* UNION_API */ void MemFree( void* memory ) {
    /*static*/ auto proc = SharedMemory::GetInstance().Free;
    return proc( memory );
  }


  /* UNION_API */ void MemDelete( void* memory ) {
    /*static*/ auto proc = SharedMemory::GetInstance().Free;
    return proc( memory );
  }


  /* UNION_API */ size_t MemSize( void* memory ) {
    /*static*/ auto proc = SharedMemory::GetInstance().Msize;
    return proc( memory );
  }


  /* UNION_API */ void* CreateSharedSingleton( const char* globalName, void* (*allocation)() ) {
    auto& memory = SharedMemory::GetInstance();
    int index = memory.SearchSingleton( globalName );
    if( index != -1 )
      return memory.Share( globalName );

    void* address = allocation();
    if( address )
      memory.Insert( globalName, address );

    return address;
  }


  /* UNION_API */ void FreeSharedSingleton( const char* globalName, void(*destructor)(void*) ) {
    auto& memory = SharedMemory::GetInstance();
    int index = memory.SearchSingleton( globalName );
    if( index == -1 )
      return;
    
    void* address = memory.Release( globalName );
    if( address )
      destructor( address );
  }
// #endif
}
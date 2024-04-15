#pragma once

#include <Windows.h>

#define LDR_DLL_NOTIFICATION_REASON_LOADED     1
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED   2

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
#ifdef MIDL_PASS
  [size_is( MaximumLength / 2 ), length_is( (Length) / 2 )] USHORT* Buffer;
#else // MIDL_PASS
  _Field_size_bytes_part_opt_( MaximumLength, Length ) PWCH   Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING* PUNICODE_STRING;
typedef const UNICODE_STRING* PCUNICODE_STRING;

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
  ULONG Flags;                    //Reserved.
  PCUNICODE_STRING FullDllName;   //The full path name of the DLL module.
  PCUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
  PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
  ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, * PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
  ULONG Flags;                    //Reserved.
  PCUNICODE_STRING FullDllName;   //The full path name of the DLL module.
  PCUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
  PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
  ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, * PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
  LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
  LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, * PLDR_DLL_NOTIFICATION_DATA;

typedef VOID( CALLBACK* PLDR_DLL_NOTIFICATION_FUNCTION )(
  ULONG NotificationReason,
  PLDR_DLL_NOTIFICATION_DATA NotificationData,
  PVOID Context
  );

inline NTSTATUS LdrDisableThreadCalloutsForDll( PVOID DllHandle ) {
  static auto func = reinterpret_cast<decltype(LdrDisableThreadCalloutsForDll)*>(GetProcAddress( LoadLibrary( "ntdll.dll" ), "LdrDisableThreadCalloutsForDll" ));
  return func( DllHandle );
}

inline VOID NTAPI LdrFastFailInLoaderCallout() {
  static auto func = reinterpret_cast<decltype(LdrFastFailInLoaderCallout)*>(GetProcAddress( LoadLibrary( "ntdll.dll" ), "LdrFastFailInLoaderCallout" ));
  return func();
}

inline NTSTATUS NTAPI LdrRegisterDllNotification( ULONG Flags, PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction, PVOID Context, PVOID Cookie ) {
  static auto func = reinterpret_cast<decltype(LdrRegisterDllNotification)*>(GetProcAddress( LoadLibrary( "ntdll.dll" ), "LdrRegisterDllNotification" ));
  return func( Flags, NotificationFunction, Context, Cookie );
}

inline NTSTATUS NTAPI LdrUnregisterDllNotification( PVOID Cookie ) {
  static auto func = reinterpret_cast<decltype(LdrUnregisterDllNotification)*>(GetProcAddress( LoadLibrary( "ntdll.dll" ), "LdrUnregisterDllNotification" ));
  return func( Cookie );
}
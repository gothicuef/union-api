#pragma once
#ifndef __UNION_THREAD_H__
#define __UNION_THREAD_H__

#include <windows.h>
#include "Types.h"

namespace Union {
  class UNION_API Thread {
    void* Function;
    DWORD ID;
    HANDLE Handle;
  public:
    Thread();
    Thread( void* function );
    void SetFunction( void* function );
    bool Start( void* argument, bool suspended = false );
    void Stop();
    void Suspend();
    void Resume();
    void SetPriotity( int priority );
    bool WaitForEnd( DWORD ms = INFINITE );
    void* GetFunction();
    int GetPriority();
    DWORD GetID();
    HANDLE GetHandle();
  };


  class UNION_API Mutex {
    CRITICAL_SECTION Handle;
  public:
    Mutex();
    void Enter();
    void Leave();
    ~Mutex();
  };


  class UNION_API ThreadGuard {
    Mutex* BaseMutex;
  public:
    ThreadGuard( Mutex& mutex );
    ~ThreadGuard();
  };


  class UNION_API Event {
    HANDLE Handle;
  public:
    Event( bool opened = false );
    bool Open();
    bool Close();
    bool WaitForClose( DWORD ms = INFINITE );
    bool IsOpen() const;
    HANDLE GetHandle();
    ~Event();
  };


  class UNION_API EventSwitch {
    Event EventOn;
    Event EventOff;
  public:
    EventSwitch( bool on = false );
    void On();
    void Off();
    void Toggle();
    bool GetState();
    void WaitForOn();
    void WaitForOff();
  };


  class UNION_API Semaphore {
    HANDLE Handle;
  public:
    Semaphore( long maximumThreads );
    bool Enter();
    bool Leave();
    ~Semaphore();
  };


#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
#pragma region thread
  inline Thread::Thread() {
    Function = nullptr;
    ID = 0;
    Handle = nullptr;
  }


  inline Thread::Thread( void* function ) {
    Function = function;
    ID = 0;
    Handle = nullptr;
  }


  inline void Thread::SetFunction( void* function ) {
    Function = function;
  }


  inline bool Thread::Start( void* argument, bool suspended ) {
    Handle = CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE)Function, argument, suspended ? CREATE_SUSPENDED : 0x0, &ID );
    return Handle != null;
  }


  inline void Thread::Stop() {
    TerminateThread( Handle, 0 );
  }


  inline void Thread::Suspend() {
    SuspendThread( Handle );
  }


  inline void Thread::Resume() {
    while( ResumeThread( Handle ) > 0 );
  }


  inline void Thread::SetPriotity( int priority ) {
    SetThreadPriority( Handle, priority );
  }


  inline bool Thread::WaitForEnd( DWORD ms ) {
    return WaitForSingleObject( Handle, ms ) == 0;
  }


  inline void* Thread::GetFunction() {
    return Function;
  }


  inline int Thread::GetPriority() {
    return GetThreadPriority( Handle );
  }


  inline DWORD Thread::GetID() {
    return ID;
  }


  inline HANDLE Thread::GetHandle() {
    return Handle;
  }
#pragma endregion


#pragma region mutex
  inline Mutex::Mutex() {
    InitializeCriticalSection( &Handle );
  }


  inline void Mutex::Enter() {
    EnterCriticalSection( &Handle );
  }


  inline void Mutex::Leave() {
    LeaveCriticalSection( &Handle );
  }


  inline Mutex::~Mutex() {
    DeleteCriticalSection( &Handle );
  }
#pragma endregion


#pragma region thread_guard
  inline ThreadGuard::ThreadGuard( Mutex& mutex ) {
    BaseMutex = &mutex;
    BaseMutex->Enter();
  }


  inline ThreadGuard::~ThreadGuard() {
    BaseMutex->Leave();
  }
#pragma endregion


#pragma region event
  inline Event::Event( bool opened ) {
    Handle = CreateEvent( nullptr, 1, opened ? 0 : 1, nullptr );
  }


  inline bool Event::Open() {
    return ResetEvent( Handle ) != 0;
  }


  inline bool Event::Close() {
    return SetEvent( Handle ) != 0;
  }


  inline bool Event::WaitForClose( DWORD ms ) {
    return WaitForSingleObject( Handle, ms ) == 0;
  }


  inline bool Event::IsOpen() const {
    return WaitForSingleObject( Handle, 0 ) == WAIT_TIMEOUT;
  }


  inline HANDLE Event::GetHandle() {
    return Handle;
  }

  inline Event::~Event() {
    CloseHandle( Handle );
  }
#pragma endregion


#pragma region event_switch
  inline EventSwitch::EventSwitch( bool on ) : EventOn( !on ), EventOff( on ) {
  }


  inline void EventSwitch::On() {
    EventOff.Open();
    EventOn.Close();
  }


  inline void EventSwitch::Off() {
    EventOn.Open();
    EventOff.Close();
  }


  inline void EventSwitch::Toggle() {
    if( EventOn.IsOpen() )
      On();
    else
      Off();
  }


  inline bool EventSwitch::GetState() {
    return !EventOn.IsOpen();
  }


  inline void EventSwitch::WaitForOn() {
    EventOn.WaitForClose();
  }


  inline void EventSwitch::WaitForOff() {
    EventOff.WaitForClose();
  }
#pragma endregion


#pragma region semaphore
  inline Semaphore::Semaphore( long maximumThreads ) {
    Handle = CreateSemaphore( nullptr, maximumThreads, maximumThreads, nullptr );
  }


  inline bool Semaphore::Enter() {
    return WaitForSingleObject( Handle, INFINITE ) != 0;
  }


  inline bool Semaphore::Leave() {
    return ReleaseSemaphore( Handle, 1, 0 ) != 0;
  }


  inline Semaphore::~Semaphore() {
    CloseHandle( Handle );
  }
#pragma endregion
#endif
}

#endif // __UNION_THREAD_H__
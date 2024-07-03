#pragma once
#ifndef __UNION_MEMORY_H__
#define __UNION_MEMORY_H__
#pragma warning(push)
#pragma warning(disable:4595)

#include "Types.h"
#include <iostream>

namespace Union {
  /* UNION_API */ void* CreateSharedSingleton( const char* globalName, void* (*allocation)() );
  /* UNION_API */ void FreeSharedSingleton( const char* globalName, void* (*allocation)() );

  /* UNION_API */ void* MemAlloc( size_t size );
  /* UNION_API */ void* MemCalloc( size_t count, size_t size );
  /* UNION_API */ void* MemRealloc( void* memory, size_t size );
  /* UNION_API */ void MemFree( void* memory );
  /* UNION_API */ void MemDelete( void* memory );
  /* UNION_API */ size_t MemSize( void* memory );
}


#if 1
inline void* operator new (size_t size) {
  return Union::MemAlloc( size );
}


inline void* operator new []( size_t size ) {
  return Union::MemAlloc( size );
}


inline void operator delete (void* memory) {
  Union::MemFree( memory );
}


inline void operator delete []( void* memory ) {
  Union::MemFree( memory );
}
#pragma warning(pop)
#endif
#endif // __UNION_MEMORY_H__
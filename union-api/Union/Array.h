#pragma once
#ifndef __UNION_ARRAY_H__
#define __UNION_ARRAY_H__
#pragma warning(push)
#pragma warning(disable:6308)
#pragma warning(disable:4521)
#pragma warning(disable:4522)

#include "Types.h"
#include "Memory.h"
#include <stack>

#define _Array_xType_ template<class xType>
#define _Array_SortionFunc_ template<auto* _Compare_>
#define _Array_SortionFunc_xType_ template<auto* _Compare_, class xType>

namespace Union {
  template<class T>
  class Array {
    struct SharedMemory {
      T* Data;
      uint UsedCount;
      uint AllocatedCount;
      uint AllocateMultiplier;
      uint ShareCounter;

      SharedMemory();
      bool Allocate();
      bool Reallocate();
      bool Free();
      void Release();
      ~SharedMemory();
    }
    *Memory;

  public:
    Array();
    Array( Array<T>& array );
    Array( const Array<T>& array );
#ifdef _INITIALIZER_LIST_
    typedef std::initializer_list<T> initializer;
    Array( initializer initializer );
    Array& operator = ( initializer initializer );
#endif
    Array& operator = ( Array& array );
    Array& operator = ( const Array& array );
    const T* GetMemory() const;
    uint GetCount() const;
    bool IsEmpty() const;
    void SetMemoryReservationMultiplier( uint mult = 2 );
    T& operator [] ( uint i );
    const T& operator [] ( uint i ) const;
    T& Get( uint i );
    const T& Get( uint i ) const;
    T& GetEnd( uint i );
    const T& GetEnd( uint i ) const;
    uint Insert( const T& object );
  _Array_SortionFunc_
    uint Insert( const T& object );
    uint InsertAt( const T& object, uint index );
    T& Create();
    T* Allocate( uint count );
  _Array_xType_
    T& Create( const xType& object );
  _Array_SortionFunc_xType_
    T& Create( const xType& object );
  _Array_xType_ 
    uint Remove( const xType& object, bool fast = false );
  _Array_SortionFunc_ 
    uint Remove( const T& object, bool fast = false );
    uint RemoveAt( uint index, bool fast = false );
    uint RemoveFirst( bool fast = false );
    uint RemoveLast();
  _Array_xType_ 
    uint Delete( const xType& object, bool fast = false );
  _Array_SortionFunc_ 
    uint Delete( const T& object, bool fast = false );
    uint DeleteAt( uint index, bool fast = false );
    uint DeleteFirst( bool fast = false );
    uint DeleteLast();
  _Array_xType_
    uint IndexOf( const xType& object ) const;
  _Array_SortionFunc_xType_
    uint IndexOf( const xType& object ) const;
  _Array_xType_
    uint IsIn( const xType& object ) const;
  _Array_SortionFunc_xType_
    uint IsIn( const xType& object ) const;
    void Clear();
    void DeleteData();
    T& GetFirst();
    const T& GetFirst() const;
    T& GetLast();
    const T& GetLast() const;
    Array& Share();
    void Swap( Array<T>& other );
  _Array_SortionFunc_xType_ 
    uint GetSortedIndexFor( const xType& object ) const;
  _Array_SortionFunc_
    void QuickSort();
    T* begin();
    const T* begin() const;
    T* end();
    const T* end() const;
    bool IsLocatedHere( void* memptr );
    ~Array();
  };


  template<class T>
  Array<T>::SharedMemory::SharedMemory() {
    Data = nullptr;
    UsedCount = 0;
    AllocatedCount = 0;
    AllocateMultiplier = 2;
    ShareCounter = 1;
  }


  template<class T>
  bool Array<T>::SharedMemory::Allocate() {
    if( UsedCount > AllocatedCount ) {
      AllocatedCount = UsedCount < 1024 ?
        UsedCount + 1024 :
        UsedCount * AllocateMultiplier;

      if( AllocatedCount < 16 )
        AllocatedCount = 16;

      Data = (T*)MemRealloc( Data, AllocatedCount * sizeof( T ) );
      return true;
    }

    return false;
  }


  template<class T>
  bool Array<T>::SharedMemory::Reallocate() {
    return Free() || Allocate();
  }


  template<class T>
  bool Array<T>::SharedMemory::Free() {
    if( UsedCount < (AllocatedCount >> 1) ) {
      AllocatedCount >>= 1;
      if( AllocatedCount < 16 )
        AllocatedCount = 16;

      Data = (T*)MemRealloc( Data, AllocatedCount * sizeof( T ) );
      return true;
    }

    return false;
  }


  template<class T>
  void Array<T>::SharedMemory::Release() {
    if( --ShareCounter == 0 )
      delete this;
  }


  template<class T>
  Array<T>::SharedMemory::~SharedMemory() {
    if( Data ) {
      for( uint i = 0; i < UsedCount; i++ )
        Data[i].~T();
      MemFree( Data );
    }
  }


  template<class T>
  Array<T>::Array() {
    Memory = new SharedMemory();
  }


  template<class T>
  Array<T>::Array( Array<T>& array ) {
    // if( array.Memory->ShareCounter > 1 ) {
    //   Memory = array.Memory;
    //   return;
    // }
    // 
    // Memory = new SharedMemory();
    // Memory->UsedCount = array.Memory->UsedCount;
    // Memory->Allocate();
    // for( uint i = 0; i < Memory->UsedCount; i++ )
    //   new(&Memory->Data[i]) T(array.Memory->Data[i]);

    operator = ( array );
  }


  template<class T>
  Array<T>::Array( const Array<T>& array ) {
    // if( array.Memory->ShareCounter > 1 ) {
    //   Memory = array.Memory;
    //   return;
    // }
    // 
    // Memory = new SharedMemory();
    // Memory->UsedCount = array.Memory->UsedCount;
    // Memory->Allocate();
    // for( uint i = 0; i < Memory->UsedCount; i++ )
    //   new(&Memory->Data[i]) T( array.Memory->Data[i] );

    operator = ( array );
  }


#ifdef _INITIALIZER_LIST_
  template<class T>
  Array<T>::Array( initializer initializer ) {
    Memory = new SharedMemory();
    Memory->UsedCount = initializer.size();
    Memory->Allocate();
    const T* source = initializer.begin();
    const T* source_end = initializer.end();
          T* destination = Memory->Data;
    
    while( source < source_end )
      ::new(destination++) T( *(source++) );
  }


  template<class T>
  Array<T>& Array<T>::operator = ( initializer initializer ) {
    for( uint i = 0; i < Memory->UsedCount; i++ )
      Memory->Data[i].~T();

    Memory->UsedCount = initializer.size();
    Memory->Reallocate();
    const T* source = initializer.begin();
    const T* source_end = initializer.end();
          T* destination = Memory->Data;
    
    while( source < source_end )
      ::new(destination++) T( *(source++) );

    return *this;
  }
#endif


  template<class T>
  Array<T>& Array<T>::operator = ( Array<T>& array ) {
    if( array.Memory->ShareCounter > 1 ) {
      Memory = array.Memory;
      return *this;
    }

    return operator = ( (const Array&)array );
  }


  template<class T>
  Array<T>& Array<T>::operator = ( const Array<T>& array ) {
    Memory = new SharedMemory();
    Memory->UsedCount = array.Memory->UsedCount;
    Memory->Allocate();
    for( uint i = 0; i < Memory->UsedCount; i++ )
      ::new(&Memory->Data[i]) T( array.Memory->Data[i] );

    return *this;
  }


  template<class T>
  const T* Array<T>::GetMemory() const {
    return Memory->Data;
  }


  template<class T>
  uint Array<T>::GetCount() const {
    return Memory->UsedCount;
  }


  template<class T>
  bool Array<T>::IsEmpty() const {
    return Memory->UsedCount == 0;
  }


  template<class T>
  void Array<T>::SetMemoryReservationMultiplier( uint mult ) {
    Memory->AllocateMultiplier = mult;
  }


  template<class T>
  T& Array<T>::operator [] ( uint i ) {
    return Memory->Data[i];
  }


  template<class T>
  const T& Array<T>::operator [] ( uint i ) const {
    return Memory->Data[i];
  }


  template<class T>
  T& Array<T>::Get( uint i ) {
    return Memory->Data[i];
  }


  template<class T>
  const T& Array<T>::Get( uint i ) const {
    return Memory->Data[i];
  }


  template<class T>
  T& Array<T>::GetEnd( uint i ) {
    return Memory->Data[GetCount() - 1 - i];
  }


  template<class T>
  const T& Array<T>::GetEnd( uint i ) const {
    return Memory->Data[GetCount() - 1 - i];
  }


  template<class T>
  uint Array<T>::Insert( const T& object ) {
    uint inserted_object_index = Memory->UsedCount;
    Memory->UsedCount++;
    Memory->Allocate();
    ::new( &Memory->Data[inserted_object_index] ) T( object );
    return inserted_object_index;
  }


  template<class T>
  _Array_SortionFunc_ uint Array<T>::Insert( const T& object ) {
    uint index_for_new_object = GetSortedIndexFor<_Compare_>( object );
    return InsertAt( object, index_for_new_object );
  }


  template<class T>
  uint Array<T>::InsertAt( const T& object, uint index ) {
    if( index >= Memory->UsedCount )
      return Insert( object );

    uint objects_count_to_rshift = Memory->UsedCount - index;
    Memory->UsedCount++;
    Memory->Allocate();
    memmove( &Memory->Data[index + 1], &Memory->Data[index], objects_count_to_rshift * sizeof( T ) );
    ::new(&Memory->Data[index]) T( object );
    return index;
  }


  template<class T>
  T& Array<T>::Create() {
    uint created_object_index = Insert( T() );
    return Memory->Data[created_object_index];
  }


  template<class T>
  T* Array<T>::Allocate( uint count ) {
    uint allocated_memory_position = Memory->UsedCount;
    Memory->UsedCount += count;
    Memory->Allocate();
    return Memory->Data + allocated_memory_position;
  }


  template<class T>
  _Array_xType_ T& Array<T>::Create( const xType& object ) {
    uint created_object_index = Insert( T( object ) );
    return Memory->Data[created_object_index];
  }


  template<class T>
  _Array_SortionFunc_xType_
  T& Array<T>::Create( const xType& object ) {
    uint created_object_index = Insert<_Compare_>( T( object ) );
    return Memory->Data[created_object_index];
  }


  template <class T>
  _Array_xType_ uint Array<T>::Remove( const xType& object, bool fast ) {
    for( uint i = 0; i < Memory->UsedCount; i++ )
      if( Memory->Data[i] == object )
        return RemoveAt( i, fast );

    return static_cast<uint>( -1 );
  }


  template <class T>
  _Array_SortionFunc_ uint Array<T>::Remove( const T& object, bool fast ) {
    uint removable_object_index = GetSortedIndexFor<_Compare_>( object );
    if( removable_object_index < Memory->UsedCount )
      if( _Compare_( Memory->Data[removable_object_index], object ) == 0 )
        return RemoveAt( removable_object_index, fast );

    return -1;
  }


  template <class T>
  uint Array<T>::RemoveAt( uint index, bool fast ) {
    Memory->Data[index].~T();

    if( index < --Memory->UsedCount ) {
      if( !fast ) {
        uint objects_count_to_lshift = Memory->UsedCount - index;
        memcpy( &Memory->Data[index], &Memory->Data[index + 1], objects_count_to_lshift * sizeof( T ) );
      }
      else {
        memcpy( &Memory->Data[index], &Memory->Data[Memory->UsedCount], sizeof( T ) );
      }
    }

    Memory->Free();
    return index;
  }


  template <class T>
  uint Array<T>::RemoveFirst( bool fast ) {
    return RemoveAt( 0, fast );
  }


  template <class T>
  uint Array<T>::RemoveLast() {
    return RemoveAt( GetCount() - 1 );
  }


  template <class T>
  _Array_xType_ uint Array<T>::Delete( const xType& object, bool fast ) {
    for( uint i = 0; i < Memory->UsedCount; i++ )
      if( Memory->Data[i] == object )
        return DeleteAt( i, fast );

    return static_cast<uint>( -1 );
  }


  template <class T>
  _Array_SortionFunc_ uint Array<T>::Delete( const T& object, bool fast ) {
    uint removable_object_index = GetSortedIndexFor<_Compare_>( object );
    if( removable_object_index < Memory->UsedCount )
      if( _Compare_( Memory->Data[removable_object_index], object ) == 0 )
      return DeleteAt( removable_object_index, fast );

    return -1;
  }


  template <class T>
  uint Array<T>::DeleteAt( uint index, bool fast ) {
    ::delete Memory->Data[index];

    if( index < --Memory->UsedCount ) {
      if( !fast ) {
        uint objects_count_to_lshift = Memory->UsedCount - index;
        memcpy( &Memory->Data[index], &Memory->Data[index + 1], objects_count_to_lshift * sizeof( T ) );
      }
      else {
        memcpy( &Memory->Data[index], &Memory->Data[Memory->UsedCount], sizeof( T ) );
      }
    }

    Memory->Free();
    return index;
  }


  template <class T>
  uint Array<T>::DeleteFirst( bool fast ) {
    return DeleteAt( 0, fast );
  }


  template <class T>
  uint Array<T>::DeleteLast() {
    return DeleteAt( GetCount() - 1 );
  }


  template<class T>
  _Array_xType_ uint Array<T>::IndexOf( const xType& object ) const {
    for( uint i = 0; i < Memory->UsedCount; i++ )
      if( Memory->Data[i] == object )
        return i;

    return -1;
  }


  template<class T>
  _Array_SortionFunc_xType_ uint Array<T>::IndexOf( const xType& object ) const {
    uint object_index = GetSortedIndexFor<_Compare_>( object );
    if( object_index < Memory->UsedCount )
      if( _Compare_( Memory->Data[object_index], object ) == 0 )
        return object_index;

    return static_cast<uint>( -1 );
  }


  template<class T>
  _Array_xType_ uint Array<T>::IsIn( const xType& object ) const {
    return IndexOf( object ) != -1;
  }


  template<class T>
  _Array_SortionFunc_xType_ uint Array<T>::IsIn( const xType& object ) const {
    return IndexOf<_Compare_>( object ) != -1;
  }


  template <class T>
  void Array<T>::Clear() {
    for( uint i = 0; i < Memory->UsedCount; i++ )
      Memory->Data[i].~T();

    Memory->UsedCount = 0;
    Memory->Free();
  }


  template <class T>
  void Array<T>::DeleteData() {
    for( uint i = 0; i < Memory->UsedCount; i++ )
      ::delete Memory->Data[i];
    Memory->UsedCount = 0;
    Memory->Free();
  }


  template <class T>
  T& Array<T>::GetFirst() {
    return Memory->Data[0];
  }


  template <class T>
  const T& Array<T>::GetFirst() const {
    return Memory->Data[0];
  }


  template <class T>
  T& Array<T>::GetLast() {
    return Memory->Data[Memory->UsedCount - 1];
  }


  template <class T>
  const T& Array<T>::GetLast() const {
    return Memory->Data[Memory->UsedCount - 1];
  }


  template <class T>
  Array<T>& Array<T>::Share() {
    Memory->ShareCounter++;
    return *this;
  }


  template <class T>
  void Array<T>::Swap( Array<T>& other ) {
    auto tmp = other.Memory;
    other.Memory = Memory;
    Memory = tmp;
  }


  template <class T>
  _Array_SortionFunc_xType_ uint Array<T>::GetSortedIndexFor( const xType& object ) const {
    if( Memory->UsedCount == 0 )
      return 0;

    T* array = Memory->Data;
    int l = 0, r = Memory->UsedCount;
    while( true ) {
      int pivot = (l + r) >> 1;
      int cmp = _Compare_( array[pivot], object );
      if( cmp == 0 )
        return pivot;

      if( r - l <= 1 )
        return cmp < 0 ? r : l;

      if( cmp < 0 )
        l = pivot;
      else
        r = pivot;
    }
  }


  template <class T>
  _Array_SortionFunc_ void Array<T>::QuickSort() {
    if( Memory->UsedCount == 0 )
      return;

    std::stack<int> stack;
    T* array = Memory->Data;
    byte pivot[sizeof( T )];
    int l = 0, r = 0, m = 0;
    int i = 0, j = 0;

    stack.push( Memory->UsedCount - 1 );
    stack.push( 0 );
    do {
      l = stack.top(); stack.pop();
      r = stack.top(); stack.pop();
      do {
        m = (l + r) >> 1;
        memcpy( pivot, array + m, sizeof( T ) );
        i = l;
        j = r;
        do {
          while( _Compare_( *(T*)pivot, array[i] ) > 0 )
            i++;
          while( _Compare_( array[j], *(T*)pivot ) > 0 )
            j--;
          if( i <= j )
            std::swap( array[i++], array[j--] );
        }
        while( i <= j );

        if( i < m ) {
          if( i < r ) {
            stack.push( r );
            stack.push( i );
          }
          r = j;
        }
        else {
          if( j > l ) {
            stack.push( j );
            stack.push( l );
          }
          l = i;
        }
      }
      while( l < r );
    }
    while( !stack.empty() );
  }


  template <class T>
  T* Array<T>::begin() {
    return Memory->Data;
  }


  template <class T>
  T* Array<T>::end() {
    return Memory->Data + Memory->UsedCount;
  }


  template <class T>
  const T* Array<T>::begin() const {
    return Memory->Data;
  }


  template <class T>
  const T* Array<T>::end() const {
    return Memory->Data + Memory->UsedCount;
  }


  template <class T>
  bool Array<T>::IsLocatedHere( void* memptr ) {
    return (T*)memptr >= Memory->Data && memptr <= (T*)(Memory->Data + Memory->UsedCount);
  }


  template <class T>
  Array<T>::~Array() {
    Memory->Release();
  }
}

#pragma warning(pop)
#endif // __UNION_ARRAY_H__
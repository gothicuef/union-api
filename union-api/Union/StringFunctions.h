#pragma once
#ifndef __UNION_STRING_FUNCTIONS_H__
#define __UNION_STRING_FUNCTIONS_H__
#pragma warning(push)
#pragma warning(disable:6308)

#include <Windows.h>
#include "Types.h"
#include "Memory.h"
#include "StringBase.h"
#include <string>
#include <cctype>
#include <cwctype>

namespace Union {
  inline size_t str_get_length( const char* str ) {
    return str ? std::strlen( str ) : 0;
  }


  inline size_t str_get_length( const wchar* str ) {
    return str ? std::wcslen( str ) : 0;
  }


  inline void int_to_str( int value, char* buffer, size_t buffer_count, int radix ) {
    _itoa_s( value, buffer, buffer_count, radix );
  }


  inline void int_to_str( int value, wchar* buffer, size_t buffer_count, int radix ) {
    _itow_s( value, buffer, buffer_count, radix );
  }


  inline void uint_to_str( uint value, char* buffer, size_t buffer_count, int radix ) {
    _ultoa_s( value, buffer, buffer_count, radix );
  }


  inline void uint_to_str( uint value, wchar* buffer, size_t buffer_count, int radix ) {
    _ultow_s( value, buffer, buffer_count, radix );
  }


  inline void long_to_str( long value, char* buffer, size_t buffer_count, int radix ) {
    _ltoa_s( value, buffer, buffer_count, radix );
  }


  inline void long_to_str( long value, wchar* buffer, size_t buffer_count, int radix ) {
    _ltow_s( value, buffer, buffer_count, radix );
  }


  inline void ulong_to_str( ulong value, char* buffer, size_t buffer_count, int radix ) {
    _ultoa_s( value, buffer, buffer_count, radix );
  }


  inline void ulong_to_str( ulong value, wchar* buffer, size_t buffer_count, int radix ) {
    _ultow_s( value, buffer, buffer_count, radix );
  }


  inline void int64_to_str( int64 value, char* buffer, size_t buffer_count, int radix ) {
    _i64toa_s( value, buffer, buffer_count, radix );
  }


  inline void int64_to_str( int64 value, wchar* buffer, size_t buffer_count, int radix ) {
    _i64tow_s( value, buffer, buffer_count, radix );
  }


  inline void uint64_to_str( uint64 value, char* buffer, size_t buffer_count, int radix ) {
    _ui64toa_s( value, buffer, buffer_count, radix );
  }


  inline void uint64_to_str( uint64 value, wchar* buffer, size_t buffer_count, int radix ) {
    _ui64tow_s( value, buffer, buffer_count, radix );
  }


  inline void single_to_str( single value, char* buffer, size_t buffer_count, int decimal ) {
    char formated_string[8];
    sprintf_s( formated_string, "%%.%if", decimal );
    sprintf_s( buffer, buffer_count, formated_string, value );
  }


  inline void single_to_str( single value, wchar* buffer, size_t buffer_count, int decimal ) {
    wchar formated_string[8];
    swprintf_s( formated_string, L"%%.%if", decimal );
    swprintf_s( buffer, buffer_count, formated_string, value );
  }


  inline void double_to_str( double value, char* buffer, size_t buffer_count, int decimal ) {
    char formated_string[8];
    sprintf_s( formated_string, "%%.%if", decimal );
    sprintf_s( buffer, buffer_count, formated_string, value );
  }


  inline void double_to_str( double value, wchar* buffer, size_t buffer_count, int decimal ) {
    wchar formated_string[8];
    swprintf_s( formated_string, L"%%.%if", decimal );
    swprintf_s( buffer, buffer_count, formated_string, value );
  }


  inline char char_to_upper( char sym ) {
    const auto result = std::toupper( static_cast<unsigned char>(sym) );
    return static_cast<char>( result );
  }


  inline wchar char_to_upper( wchar sym ) {
    return std::towupper( sym );
  }


  inline char char_to_lower( char sym ) {
    const auto result = std::tolower( sym );
    return static_cast<char>( result );
  }


  inline wchar char_to_lower( wchar sym ) {
    return std::towlower( sym );
  }


  inline int str_compare( const char* left, const char* right ) {
    return strcmp( left, right );
  }


  inline int str_compare( const wchar* left, const wchar* right ) {
    return wcscmp( left, right );
  }


  inline int str_compare_ignore_case( const char* left, const char* right ) {
    return _strcmpi( left, right );
  }


  inline int str_compare_ignore_case( const wchar* left, const wchar* right ) {
    return _wcsicmp( left, right );
  }


  inline int str_to_int( const char* str, int radix ) {
    return strtol( str, nullptr, radix );
  }


  inline int str_to_int( const wchar* str, int radix ) {
    return wcstol( str, nullptr, radix );
  }


  inline uint str_to_uint( const char* str, int radix ) {
    return strtoul( str, nullptr, radix );
  }


  inline uint str_to_uint( const wchar* str, int radix ) {
    return wcstoul( str, nullptr, radix );
  }


  inline int64 str_to_int64( const char* str, int radix ) {
    return _strtoi64( str, nullptr, radix );
  }


  inline int64 str_to_int64( const wchar* str, int radix ) {
    return _wcstoi64( str, nullptr, radix );
  }


  inline uint64 str_to_uint64( const char* str, int radix ) {
    return _strtoui64( str, nullptr, radix );
  }


  inline uint64 str_to_uint64( const wchar* str, int radix ) {
    return _wcstoui64( str, nullptr, radix );
  }


  inline single str_to_single( const char* str ) {
    return (single)atof( str );
  }


  inline single str_to_single( const wchar* str ) {
    return (single)_wtof( str );
  }


  inline double str_to_double( const char* str ) {
    return atof( str );
  }


  inline double str_to_double( const wchar* str ) {
    return _wtof( str );
  }


  inline bool char_is_digit( int c ) {
    if( c >= '0' && c <= '9' )
      return true;

    return
      c == '.' ||
      c == '-' ||
      c == '+' ||
      c == 'e' ||
      c == 'E' ||
      c == 'f' ||
      c == 'F';
  }


  inline bool char_is_hexadecimal_digit( int c ) {
    if( c >= '0' && c <= '9' ) return true;
    if( c >= 'a' && c <= 'f' ) return true;
    if( c >= 'A' && c <= 'F' ) return true;
    if( c == 'x' || c == 'X' ) return true;
    if( c == 'h' || c == 'H' ) return true;
    return false;
  }


  inline void str_malloc( char*& ptr, size_t sz ) {
    ptr = reinterpret_cast<char*>( MemAlloc( ++sz ) );
  }


  inline void str_malloc( wchar*& ptr, size_t sz ) {
    ptr = reinterpret_cast<wchar*>(MemAlloc( ++sz * sizeof( wchar ) ) );
  }


  inline void str_realloc( char*& ptr, size_t sz ) {
    ptr = reinterpret_cast<char*>( MemRealloc( ptr, ++sz ) );
  }


  inline void str_realloc( wchar*& ptr, size_t sz ) {
    ptr = reinterpret_cast<wchar*>(MemRealloc( ptr, ++sz * sizeof( wchar ) ) );
  }

  
  inline void str_copy( char* ptr, const void* src, size_t sz ) {
    ::memcpy( ptr, src, sz );
  }


  inline void str_copy( wchar* ptr, const void* src, size_t sz ) {
    ::memcpy( ptr, src, sz * sizeof( wchar ) );
  }
  
  
  inline void str_free( void* ptr ) {
    MemFree( ptr );
  }

  
  inline void str_move( char*& ptr, size_t to, size_t from, size_t len ) {
    memmove( &ptr[to], &ptr[from], len );
  }


  inline void str_move( wchar*& ptr, size_t to, size_t from, size_t len ) {
    memmove( &ptr[to], &ptr[from], len * sizeof( wchar ) );
  }


  inline bool str_to_boolean( const char* str ) {
    return
      str_compare( str, "1" ) == 0 ||
      str_compare_ignore_case( str, "true" ) == 0;
  }


  inline bool str_to_boolean( const wchar* str ) {
    return
      str_compare( str, L"1" ) == 0 ||
      str_compare_ignore_case( str, L"true" ) == 0;
  }


  inline bool str_is_digit( char* str, _Out_ bool* is_real ) {
    if( is_real )
      *is_real = false;

    do {
      if( !char_is_digit( *str ) )
        return false;

      if( is_real && *str == '.' )
        *is_real = true;

    } while( *(++str) != 0 );
    return true;
  }


  inline bool str_is_digit( wchar* str, _Out_ bool* is_real ) {
    if( is_real )
      *is_real = false;

    do {
      if( !char_is_digit( *str ) )
        return false;

      if( is_real && *str == '.' )
        *is_real = true;

    } while( *(++str) != 0 );
    return true;
  }


  inline bool str_is_hexadecimal_digit( char* str ) {
    char* i = str;
    if( *i != '0' )
      return false;

    do {
      if( !char_is_hexadecimal_digit( *i ) )
        return false;
    } while( *(++i) != 0 );

    return
      *(str + 1) == 'x' ||
      *(str + 1) == 'X' ||
      *(i - 1) == 'h' ||
      *(i - 1) == 'H';
  }


  inline bool str_is_hexadecimal_digit( wchar* str ) {
    wchar* i = str;
    if( *i != '0' )
      return false;

    do {
      if( !char_is_hexadecimal_digit( *i ) )
        return false;
    } while( *(++i) != 0 );

    return
      *(str + 1) == 'x' ||
      *(str + 1) == 'X' ||
      *(i - 1) == 'h' ||
      *(i - 1) == 'H';
  }


  inline bool str_is_integer_digit( char* str ) {
    do {
      if( !char_is_digit( *str ) )
        return false;

      if( *str == '.' )
        return false;

    } while( *(++str) != 0 );

    return true;
  }


  inline bool str_is_integer_digit( wchar* str ) {
    do {
      if( !char_is_digit( *str ) )
        return false;

      if( *str == '.' )
        return false;

    } while( *(++str) != 0 );

    return true;
  }


  inline bool str_is_real_digit( char* str ) {
    bool isReal = false;
    do {
      if( !char_is_digit( *str ) )
        return false;

      if( *str == '.' || *str == 'e' || *str == 'E' )
        isReal = true;

    } while( *(++str) != 0 );
    
    return isReal;
  }


  inline bool str_is_real_digit( wchar* str ) {
    bool isReal = false;
    do {
      if( !char_is_digit( *str ) )
        return false;

      if( *str == '.' || *str == 'e' || *str == 'E' )
        isReal = true;

    } while( *(++str) != 0 );
    
    return isReal;
  }


  inline bool str_is_boolean( const char* str ) {
    return
      str_compare( str, "0" ) == 0 ||
      str_compare( str, "1" ) == 0 ||
      str_compare_ignore_case( str, "false" ) == 0 ||
      str_compare_ignore_case( str, "true" ) == 0;
  }


  inline bool str_is_boolean( const wchar* str ) {
    return
      str_compare( str, L"0" ) == 0 ||
      str_compare( str, L"1" ) == 0 ||
      str_compare_ignore_case( str, L"false" ) == 0 ||
      str_compare_ignore_case( str, L"true" ) == 0;
  }


  inline bool str_compare_byte_order_mark( void* bom, const void* ptr, size_t size ) {
    return memcmp( bom, ptr, size ) == 0;
  }


  inline bool str_compare_byte_order_mark( void* bom, const char* ptr ) {
    return str_compare_byte_order_mark( bom, ptr, strlen( ptr ) );
  }

  inline unsigned long get_current_directory( char* buffer, unsigned long length ) {
    return GetCurrentDirectoryA( length, buffer );
  }

  inline unsigned long get_current_directory( wchar_t* buffer, unsigned long length ) {
    return GetCurrentDirectoryW( length, buffer );
  }
  
  template<typename T>
  inline T* str_search( T* str, int str_length, int offset, const T* substr, int substr_length ) {
    T* search_start = offset != -1 ?
      str + offset :
      str;

    T* search_end = str_length != -1 ?
      str + str_length :
      0;

    const T* substr_end = substr_length != -1 ?
      substr + substr_length :
      0;
          T* i = search_start;
          T* l = nullptr;
    const T* r = nullptr;
    do {
      l = i;
      r = substr;
      while( true ) {
        if( *r == 0 || r == substr_end )
          return i;

        if( *l == 0 || l == search_end )
          return nullptr;

        if( *(l++) != *(r++) )
          break;
      }
    } while( *(++i) );
    return nullptr;
  }


  template<typename T>
  inline T* str_search_ignore_case( T* str, int str_length, int offset, const T* substr, int substr_length ) {
    T* search_start = offset != -1 ?
      str + offset :
      str;

    T* search_end = str_length != -1 ?
      str + str_length :
      0;

    const T* substr_end = substr_length != -1 ?
      substr + substr_length :
      0;
          T* i = search_start;
          T* l = nullptr;
    const T* r = nullptr;
    do {
      l = i;
      r = substr;
      while( true ) {
        if( *r == 0 || r == substr_end )
          return i;

        if( *l == 0 || l == search_end )
          return nullptr;

        if( char_to_upper( *(l++) ) != char_to_upper( *(r++) ) )
          break;
      }
    } while( *(++i) );
    return nullptr;
  }


  template<typename T>
  inline T* str_search_in_reverse( T* str, int str_length, int offset, const T* substr, int substr_length ) {
    T* search_start = offset != -1 ?
      str + offset :
      str + str_get_length( str ) - 1;

    T* search_end = str_length != -1 ?
      str + str_length :
      0;

    const T* substr_end = substr_length != -1 ?
      substr + substr_length :
      0;
          T* i = search_start;
          T* l = nullptr;
    const T* r = nullptr;
    do {
      l = i;
      r = substr;
      while( true ) {
        if( *r == 0 || r == substr_end )
          return i;

        if( *l == 0 || l == search_end )
          return nullptr;

        if( *(l++) != *(r++) )
          break;
      }
    } while( --i  >= str );
    return nullptr;
  }


  template<typename T>
  inline T* str_search_in_reverse_ignore_case( T* str, int str_length, int offset, const T* substr, int substr_length ) {
    T* search_start = offset != -1 ?
      str + offset :
      str + str_get_length( str ) - 1;

    T* search_end = str_length != -1 ?
      str + str_length :
      0;

    const T* substr_end = substr_length != -1 ?
      substr + substr_length :
      0;
          T* i = search_start;
          T* l = nullptr;
    const T* r = nullptr;
    do {
      l = i;
      r = substr;
      while( true ) {
        if( *r == 0 || r == substr_end )
          return i;

        if( *l == 0 || l == search_end )
          return nullptr;

        if( char_to_upper( *(l++) ) != char_to_upper( *(r++) ) )
          break;
      }
    } while( --i  >= str );
    return nullptr;
  }


  template<typename T>
  inline bool char_is_matches_to_range( T symbol, const T* range, bool ignore_case ) {
    if( ignore_case ) {
      for( int i = 0; range[i] != 0; i++ )
        if( char_to_upper( symbol ) == char_to_upper( range[i] ) )
          return true;
    }
    else for( int i = 0; range[i] != 0; i++ )
      if( symbol == range[i] )
        return true;

    return false;
  }


  template<typename T>
  inline bool is_unicode_type() {
    return sizeof( T ) == 2;
  }


  template<typename T>
  inline bool is_ansi_type() {
    return sizeof( T ) == 1;
  }


#ifdef _WINUSER_
  inline int str_msgbox( const char* message, const char* title = "", UINT flags = 0 ) {
    return MessageBoxA( 0, message, title, flags );
  }


  inline int str_msgbox( const char* message, int flags ) {
    return str_msgbox( message, "", flags );
  }


  inline int str_msgbox( const wchar* message, const wchar* title = L"", int flags = 0 ) {
    return MessageBoxW( 0, message, title, flags );
  }


  inline int str_msgbox( const wchar* message, int flags ) {
    return str_msgbox( message, L"", flags );
  }
#endif
}

#pragma warning(pop)
#endif // __UNION_STRING_FUNCTIONS_H__
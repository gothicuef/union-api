#ifndef __UNION_STRING_H__
#define __UNION_STRING_H__

#include "StringBase.h"
#include "StringFunctions.h"
#include "Stream.h"
#include "Array.h"
#include <regex>
#include <type_traits>
#include <io.h>
#include <iostream>

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#undef CreateDirectory
#define clamp(_value, _min, _max) std::min( std::max( _value, _min ), _max )

namespace Union {
  template<typename T>
  struct UnionStringRange {
    const T* Data;
    const T* Begin;
    const T* End;

    UnionStringRange();
    uint GetIndex() const;
    uint GetLength() const;
  };


  template<typename T>
  class UnionString : public StringBase {
    friend class UnionString<char>;
    friend class UnionString<wchar>;
  protected:
    uint Range;
    T* Data;
    uint Length;
    uint Reserved;

    void Allocate();
    void Free();
  public:
#pragma region constructors
    UnionString();
    UnionString( const UnionStringRange<T>& range );
    UnionString( const UnionString& a_str );
    explicit UnionString( bool boolean );
    UnionString( T character );
    UnionString( const T* c_str );
    UnionString( const T* c_str0, const T* c_str1 );
    UnionString( const T* c_str, uint length );
    UnionString( byte character );
    UnionString( void* digit );
    UnionString( int digit, uint radix = 10 );
    UnionString( uint digit, uint radix = 10 );
    UnionString( long digit, uint radix = 10 );
    UnionString( ulong digit, uint radix = 10 );
    UnionString( int64 digit, uint radix = 10 );
    UnionString( uint64 digit, uint radix = 10 );
    UnionString( single digit, uint decimal = 3 );
    UnionString( double digit, uint decimal = 3 );
    UnionString<T> GetCopy() const;
    virtual ~UnionString();
#pragma endregion

#pragma region operators
    UnionString operator + ( const UnionString& a_str ) const;
    UnionString operator + ( const T* c_str ) const;
    UnionString operator + ( T c_str ) const;
    UnionString& operator += ( const UnionString& a_str );
    UnionString& operator += ( const T* c_str );
    UnionString& operator += ( T c_str );
    UnionString& operator = ( const UnionString& a_str );
    UnionString& operator = ( const T* c_str );
    UnionString operator - ( const UnionString& a_str );
    bool operator == ( const UnionString& a_str ) const;
    bool operator == ( const T* c_str ) const;
    bool operator != ( const UnionString& a_str ) const;
    bool operator != ( const T* c_str ) const;
    T& operator [] ( int index );
    const T& operator [] ( int index ) const;
    operator const T* () const;
#pragma endregion

#pragma region convert
    void* ConvertToVoidPtr( int sourceRadix = 16 ) const;
    int ConvertToInt( int sourceRadix = 10 ) const;
    uint ConvertToUInt( int sourceRadix = 10 ) const;
    int64 ConvertToInt64( int sourceRadix = 10 ) const;
    uint64 ConvertToUInt64( int sourceRadix = 10 ) const;
    single ConvertToSingle() const;
    double ConvertToDouble() const;
    bool ConvertToBoolean() const;
    bool IsDigit() const;
    bool IsDigit( OUT bool& isReal ) const;
    bool IsHexadecimalDigit() const;
    bool IsIntegerDigit() const;
    bool IsRealDigit() const;
    bool IsBoolean() const;
#pragma endregion

#pragma region helpers
    void Swap( UnionString& a_str );
    T& First();
    const T& First() const;
    T& Last();
    const T& Last() const;
    const T* ToChar() const;
    T* ToChar();
    T* begin();
    const T* begin() const;
    T* end();
    const T* end() const;
    int ShowMessage( const T* title = _lpStrT( "" ), int flags = 0 ) const;
    int ShowMessage( int flags ) const;
    int StdPrint() const;
    int StdRead();
    int StdPrint( int messageLevel ) const;
    int StdPrintLine() const;
    int StdPrintLine( int messageLevel ) const;
    static void StdSetCodepage( int codepage );
    static void SetMessageLevel( int messageLevel );
    static int& GetMessageLevel();
#pragma endregion

#pragma region service_simple
    uint GetLength() const;
    void SetLength( size_t length );
    void ShrinkToFit();
    uint GetAllocatedLength() const;
    bool IsEmpty() const;
    bool IsBlank() const;
    bool StartsWith( const T* c_str, Flags flags = Flags::Default ) const;
    bool EndsWith( const T* c_str, Flags flags = Flags::Default ) const;
    bool IsSame( const T* c_str, Flags flags = Flags::Default ) const;
    int GetDifference( const T* c_str, Flags flags = Flags::Default ) const;
    bool IsMatchesMask( const T* c_str, Flags flags = Flags::Default ) const;
    bool IsMatchesCharacters( const T* c_str, Flags flags = Flags::Default ) const;
    uint Search( const T* c_str, Flags flags = Flags::Default ) const;
    uint Search( const T* c_str, uint pos, Flags flags = Flags::Default ) const;
    bool Contains( const T* c_str, Flags flags = Flags::Default ) const;
    uint GetContainsCount( const T* c_str, Flags flags = Flags::Default ) const;
    template<typename _Lambda>
    void WalkThroughLines( _Lambda proc/*void(*)( const UnionString<T>& )*/ );
#pragma endregion

#pragma region service_advanced
  protected:
    UnionString GetWordInReverse( const T* separators, int index ) const;
    UnionString GetWordBetweenInReverse( const T* lword, const T* rword, int index = 1 ) const;
  public:
    UnionString& PadLeft( uint length, T sym = ' ' );
    UnionString& PadRight( uint length, T sym = ' ' );
    UnionString GetWord( int index ) const;
    UnionString GetWord( const T* separators, int index = 1 ) const;
    UnionString GetWordBetween( const T* lword, const T* rword, int index = 1 ) const;
    Array<UnionString> Split( const T* separators = _lpStrT( " \t\r\n\f\v" ), bool ignoreEmpty = true ) const;
    Array<UnionString> Split( T separator, bool ignoreEmpty = true ) const;
    Array<UnionString> SplitSmart( Flags flags = Flags::Default ) const;
    UnionString GetWordSmart( int index, Flags flags = Flags::Default ) const;
    UnionString GetWordSmart( int index, IN OUT int& pos, Flags flags = Flags::Default ) const;
    UnionString GetWordSmart( int index, IN OUT int& pos, OUT WordGroups& group, Flags flags = Flags::Default ) const;
    UnionString GetFlipped() const;
    UnionString& Clear();
    UnionString SubString( uint pos, uint length ) const;
    UnionString& Remove( uint length );
    UnionString& Remove( uint pos, uint length );
    UnionString& Insert( const T* c_str );
    UnionString& Insert( const T* c_str, int index );
    UnionString& Insert( T c_str );
    UnionString& Insert( T c_str, int index );
    UnionString& Overlay( const T* c_str, int index );
    UnionString& Replace( const T* c_src, const T* c_dst, Flags flags = Flags::Default );
    UnionString& Trim( const T* characters = _lpStrT( " \t\r\n\f\v" ) );
    UnionString& TrimLeft( const T* characters = _lpStrT( " \t\r\n\f\v" ) );
    UnionString& TrimRight( const T* characters = _lpStrT( " \t\r\n\f\v" ) );
    UnionString& MakeUpper();
    UnionString& MakeLower();
    UnionString& MakeLowerFirstUpper();
    UnionString& FillWith( int value );
#pragma endregion

#pragma region io
    UnionString& MakeFullPath();
    UnionString GetDirectory() const;
    UnionString GetFileName() const;
    UnionString GetFileNameNoExtension() const;
    UnionString GetFileExtension() const;
    UnionString CreateDirectory() const;
    bool IsExistsAsFile() const;
    bool IsExistsAsDirectory() const;
    void DeleteAsFile() const;
    void DeleteAsDirectory() const;
#pragma endregion

#pragma region cast
    virtual bool IsAnsiString() const;
    virtual bool IsUnicodeString() const;
    UnionString<wchar> ToUnicode( int codepage = (-1) ) const;
    UnionString<char> ToAnsi( int codepage = (-1) ) const;
    template<typename NewT>
    UnionString<NewT> ConvertTo();
    template<typename NewT>
    UnionString<NewT>& ReinterpretTo();
#pragma endregion

#pragma region regex
    
    struct Match {
      Array<UnionString<T>> Items;
      Match() { }
      Match( Match& match ) : Items( match.Items.Share() ) { }
      operator const UnionString<T>& () const { return Items.GetFirst(); }
      operator bool() const { return !Items.IsEmpty(); }
      const UnionString<T>& operator []( int index ) const { return Items[index]; }
      uint GetLength() const { return Items.GetCount(); }
    };

    UnionStringRange<T> RegexSearch( const UnionString& mask, uint pos = 0 ) const;
    Array<UnionStringRange<T>> RegexSearchAll( const UnionString& mask ) const;
    Match RegexMatch( const UnionString& mask ) const;
    bool RegexIsMatchesMask( const UnionString& mask ) const;
    UnionString<T>& RegexReplace( const UnionString& mask, const UnionString& to );
#pragma region

#pragma region io
  protected:
    int ReadFileEncoding( Stream* file, int defaultEncoding = Encodings::ANSI );
  public:
    void SetEOL( char* eol );
    bool ReadFromFile( const char* fileName, int defaultEncoding = Encodings::ANSI );
    bool ReadFromFile( const wchar* fileName, int defaultEncoding = Encodings::ANSI );
    bool ReadFromVDFS( const char* fileName, int defaultEncoding = Encodings::ANSI );
    bool ReadFromVDFS( const wchar* fileName, int defaultEncoding = Encodings::ANSI );
    bool ReadFromFile( Stream* file, int defaultEncoding = Encodings::ANSI );
    bool WriteToFile( const char* fileName, int encoding = (-1) );
    bool WriteToFile( const wchar* fileName, int encoding = (-1) );
    bool WriteToFile( Stream* file, int encoding = (-1) );
#pragma endregion

#pragma region statics
    static UnionString FormatRaw( const T* format, ... );
    template<typename... Args>
    static UnionString Format( const T* format, Args... args );
    static UnionString Format( const T* format );

    static UnionString MakeHexadecimal( void* address );
    static UnionString MakeHexadecimal( int digit );
    static UnionString MakeHexadecimal( uint digit );
    static UnionString MakeHexadecimal( int64 digit );
    static UnionString MakeHexadecimal( uint64 digit );
#pragma endregion

    static UnionString GetEmpty() { return UnionString(); }
  };


  typedef UnionString<char>       UnionStringA, StringANSI, StringUTF8;
  typedef UnionString<wchar>      UnionStringW, StringUTF16;
  typedef UnionStringRange<char>  StringRangeA;
  typedef UnionStringRange<wchar> StringRangeW;
#ifdef _UNICODE
  typedef UnionString<wchar> String;
  typedef StringRangeW StringRange;
#else
  typedef UnionString<char> String;
  typedef StringRangeA StringRange;
#endif

  class UNION_API StringConverter {
  public:
    static bool UTF8ToANSI( const StringUTF8& input, StringANSI& output );
    static bool UTF16ToANSI( const StringUTF16& input, StringANSI& output );
    static bool ANSIToUTF8( const StringANSI& input, StringUTF8& output, int codepage = (-1) );
    static bool UTF16ToUTF8( const StringUTF16& input, StringUTF8& output );
    static bool ANSIToUTF16( const StringANSI& input, StringUTF16& output, int codepage = (-1) );
    static bool UTF8ToUTF16( const StringUTF8& input, StringUTF16& output );
  };
}

#include "Locale.h"

namespace Union {
  template<typename T>
  UnionStringRange<T>::UnionStringRange() : Data( nullptr ), Begin( nullptr ), End( nullptr ) {
  }
  
  template<typename T>
  uint UnionStringRange<T>::GetIndex() const {
    return Begin - Data;
  }
  
  template<typename T>
  uint UnionStringRange<T>::GetLength() const {
    return End - Begin;
  }


  inline uint get_next_string_length( uint length ) {
    if( length > 512 )
      return length + 512;

    if( length < 16 )
      return 16;

    return length * 2;
  }


  template<typename T>
  void UnionString<T>::Allocate() {
    Length = clamp( Length, 0u, Range );
    if( Length + 1 > Reserved ) {
      Reserved = clamp( get_next_string_length( Length + 1u ), 16u, Range );
      str_realloc( Data, Reserved );
    }
    Data[Length] = 0;
  }


  template<typename T>
  void UnionString<T>::Free() {
    Length = clamp( Length, 0u, Range );
    if( Length + 1 < Reserved / 2 ) {
      Reserved = clamp( Reserved / 2u, 16u, Range );
      str_realloc( Data, Reserved );
    }
    Data[Length] = 0;
  }


  template<typename T>
  void UnionString<T>::Swap( UnionString& other_string ) {
    std::swap( Data, other_string.Data );
    std::swap( Length, other_string.Length );
    std::swap( Reserved, other_string.Reserved );
  }


#pragma region constructors
  template<typename T>
  UnionString<T>::UnionString() {
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = 0;
    Allocate();
  }


  template<typename T>
  UnionString<T>::UnionString( const UnionStringRange<T>& range ) {
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = range.GetLength();
    Allocate();
    str_copy( Data, range.Begin, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( const UnionString<T>& other_string ) {
    Range = other_string.Range;
    Data = nullptr;
    Reserved = 0;
    Length = other_string.Length;
    Allocate();
    str_copy( Data, other_string.Data, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( bool boolean ) {
    const T* c_str = boolean ? 
      _lpStrT( "true" ) : 
      _lpStrT( "false" );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = (uint)str_get_length( c_str );
    Allocate();
    str_copy( Data, c_str, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( T character ) {
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = 1;
    Allocate();
    Data[0] = character;
  }


  template<typename T>
  UnionString<T>::UnionString( const T* c_str ) {
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( c_str );
    Allocate();
    str_copy( Data, c_str, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( const T* c_str0, const T* c_str1 ) {
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = (uint)c_str1 - (uint)c_str0;
    Allocate();
    str_copy( Data, c_str0, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( const T* c_str, uint length ) {
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = length;
    Allocate();
    str_copy( Data, c_str, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( byte character ) {
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = 4;
    Allocate();

    T buffer[3];
    int_to_str( character, buffer, 3, 16 );
    uint bufferLength = str_get_length( buffer );
    for( uint i = 0; buffer[i]; i++ )
      buffer[i] = char_to_upper( buffer[i] );
    
    str_copy( Data, (const T*)_lpStrT( "0x00" ), Length );
    str_copy( Data + (Length - bufferLength), buffer, bufferLength );
  }


  template<typename T>
  UnionString<T>::UnionString( void* value ) {
#ifdef _M_X64
    T buffer[65];
    int64_to_str( (int64)value, buffer, 65, 16 );
    for( int i = 0; buffer[i]; i++ )
      buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
#else
    T buffer[33];
    int_to_str( (int)value, buffer, 33, 16 );
    for( int i = 0; buffer[i]; i++ )
      buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
#endif
  }


  template<typename T>
  UnionString<T>::UnionString( int digit, uint radix ) {
    T buffer[33];
    int_to_str( digit, buffer, 33, radix );
    if( radix > 10 )
      for( int i = 0; buffer[i]; i++ )
        buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( uint digit, uint radix ) {
    T buffer[33];
    uint_to_str( digit, buffer, 33, radix );
    if( radix > 10 )
      for( int i = 0; buffer[i]; i++ )
        buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( long digit, uint radix ) {
    T buffer[33];
    long_to_str( digit, buffer, 33, radix );
    if( radix > 10 )
      for( int i = 0; buffer[i]; i++ )
        buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( ulong digit, uint radix ) {
    T buffer[33];
    ulong_to_str( digit, buffer, 33, radix );
    if( radix > 10 )
      for( int i = 0; buffer[i]; i++ )
        buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( int64 digit, uint radix ) {
    T buffer[65];
    int64_to_str( digit, buffer, 65, radix );
    if( radix > 10 )
      for( int i = 0; buffer[i]; i++ )
        buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( uint64 digit, uint radix ) {
    T buffer[65];
    uint64_to_str( digit, buffer, 65, radix );
    if( radix > 10 )
      for( int i = 0; buffer[i]; i++ )
        buffer[i] = char_to_upper( buffer[i] );

    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T>::UnionString( single digit, uint mant ) {
    T buffer[100];
    single_to_str( digit, buffer, 100, mant );
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetCopy() const {
    return *this;
  }


  template<typename T>
  UnionString<T>::UnionString( double digit, uint mant ) {
    T buffer[100];
    double_to_str( digit, buffer, 100, mant );
    Range = UINT_MAX;
    Data = nullptr;
    Reserved = 0;
    Length = str_get_length( buffer );
    Allocate();
    str_copy( Data, buffer, Length );
  }


  template<typename T>
  UnionString<T>::~UnionString() {
    if( Data )
      str_free( Data );
  }
#pragma endregion

#pragma region operators
  template<typename T>
  UnionString<T> UnionString<T>::operator + ( const UnionString& other_string ) const {
    return UnionString( *this ) += other_string;
  }


  template<typename T>
  UnionString<T> UnionString<T>::operator + ( const T* c_str ) const {
    return UnionString( *this ) += c_str;
  }


  template<typename T>
  UnionString<T> UnionString<T>::operator + ( T c ) const {
    return UnionString( *this ) += c;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::operator += ( const UnionString& other_string ) {
    uint index = Length;
    Length += other_string.Length;
    Allocate();
    str_copy( Data + index, other_string.Data, other_string.Length );
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::operator += ( const T* c_str ) {
    uint str_length = str_get_length( c_str );
    uint index = Length;
    Length += str_length;
    Allocate();
    str_copy( Data + index, c_str, str_length );
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::operator += ( T c ) {
    uint index = Length;
    Length += 1;
    Allocate();
    Data[index] = c;
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::operator = ( const UnionString& other_string ) {
    Range = other_string.Range;
    Reserved = 0;
    Length = other_string.Length;
    Allocate();
    str_copy( Data, other_string.Data, Length );
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::operator = ( const T* c_str ) {
    Reserved = 0;
    Length = str_get_length( c_str );
    Allocate();
    str_copy( Data, c_str, Length );
    return *this;
  }


  template<typename T>
  UnionString<T> UnionString<T>::operator - ( const UnionString& a_str ) {
    uint index = Search( a_str );
    if( index != -1 ) {
      return GetCopy().Remove(index, a_str.Length);
    }

    if( a_str.Search( *this ) != -1 )
      return GetEmpty();

    return *this;
  }


  template<typename T>
  bool UnionString<T>::operator == ( const UnionString& other_string ) const {
    return str_compare_ignore_case( Data, other_string ) == 0;
  }


  template<typename T>
  bool UnionString<T>::operator == ( const T* c_str ) const {
    return str_compare_ignore_case( Data, c_str ) == 0;
  }


  template<typename T>
  bool UnionString<T>::operator != ( const UnionString& other_string ) const {
    return str_compare_ignore_case( Data, other_string ) != 0;
  }


  template<typename T>
  bool UnionString<T>::operator != ( const T* c_str ) const {
    return str_compare_ignore_case( Data, c_str ) != 0;
  }


  template<typename T>
  T& UnionString<T>::operator [] ( int index ) {
    return index >= 0 ?
      Data[index] :
      Data[Length - index - 1];
  }


  template<typename T>
  const T& UnionString<T>::operator [] ( int index ) const {
    return index >= 0 ?
      Data[index] :
      Data[Length - index - 1];
  }


  template<typename T>
  UnionString<T>::operator const T* () const {
    return Data;
  }
#pragma endregion

#pragma region convert
  template<typename T>
  void* UnionString<T>::ConvertToVoidPtr( int sourceRadix ) const {
#ifdef _M_X64
    uint64 pointer = str_to_uint64( Data, sourceRadix );
#else
    uint pointer = str_to_uint( Data, sourceRadix );
#endif
    return (void*)pointer;
  }


  template<typename T>
  int UnionString<T>::ConvertToInt( int sourceRadix ) const {
    return str_to_int( Data, sourceRadix );
  }


  template<typename T>
  uint UnionString<T>::ConvertToUInt( int sourceRadix ) const {
    return str_to_uint( Data, sourceRadix );
  }


  template<typename T>
  int64 UnionString<T>::ConvertToInt64( int sourceRadix ) const {
    return str_to_int64( Data, sourceRadix );
  }


  template<typename T>
  uint64 UnionString<T>::ConvertToUInt64( int sourceRadix ) const {
    return str_to_uint64( Data, sourceRadix );
  }


  template<typename T>
  single UnionString<T>::ConvertToSingle() const {
    return str_to_single( Data );
  }


  template<typename T>
  double UnionString<T>::ConvertToDouble() const {
    return str_to_double( Data );
  }


  template<typename T>
  bool UnionString<T>::ConvertToBoolean() const {
    return str_to_boolean( Data );
  }


  template<typename T>
  bool UnionString<T>::IsDigit() const {
    return str_is_digit( Data, null );
  }


  template<typename T>
  bool UnionString<T>::IsDigit( OUT bool& isReal ) const {
    return str_is_digit( Data, &isReal );
  }


  template<typename T>
  bool UnionString<T>::IsHexadecimalDigit() const {
    return str_is_hexadecimal_digit( Data );
  }


  template<typename T>
  bool UnionString<T>::IsIntegerDigit() const {
    return str_is_integer_digit( Data );
  }


  template<typename T>
  bool UnionString<T>::IsRealDigit() const {
    return str_is_real_digit( Data );
  }


  template<typename T>
  bool UnionString<T>::IsBoolean() const {
    return str_is_boolean( Data );
  }
#pragma endregion

#pragma region helpers
  template<typename T>
  T& UnionString<T>::First() {
    return Data[0];
  }


  template<typename T>
  const T& UnionString<T>::First() const {
    return Data[0];
  }


  template<typename T>
  T& UnionString<T>::Last() {
    return Data[Length - 1];
  }


  template<typename T>
  const T& UnionString<T>::Last() const {
    return Data[Length - 1];
  }


  template<typename T>
  const T* UnionString<T>::ToChar() const {
    return Data;
  }


  template<typename T>
  T* UnionString<T>::ToChar() {
    return Data;
  }


  template<typename T>
  T* UnionString<T>::begin() {
    return Data;
  }


  template<typename T>
  const T* UnionString<T>::begin() const {
    return Data;
  }


  template<typename T>
  T* UnionString<T>::end() {
    return Data + Length;
  }


  template<typename T>
  const T* UnionString<T>::end() const {
    return Data + Length;
  }


#ifdef _WINUSER_
  template<typename T>
  int UnionString<T>::ShowMessage( const T* title, int flags ) const {
    return str_msgbox( ToChar(), title, flags );
  }


  template<typename T>
  int UnionString<T>::ShowMessage( int flags ) const {
    return ShowMessage( _lpStrT( "" ), flags );
  }


  template<>
  inline int UnionString<char>::StdPrint() const {
    SetConsoleOutputCP( (uint)Locale::GetUserLocale().Codepage );
    DWORD dw;
    WriteConsoleA( GetStdHandle( STD_OUTPUT_HANDLE ), ToChar(), GetLength(), &dw, nullptr );
    return dw;
  }


  template<>
  inline int UnionString<char>::StdRead() {
    SetConsoleOutputCP( (uint)Locale::GetUserLocale().Codepage );
    DWORD readBytes;
    char buffer[65536];
    size_t length = sizeof( buffer ) - 1;
    ReadConsoleA( GetStdHandle( STD_INPUT_HANDLE ), buffer, length, &readBytes, nullptr );
    SetLength( readBytes );
    memcpy( ToChar(), buffer, readBytes );
    return readBytes;
  }


  template<>
  inline int UnionString<char>::StdPrint( int messageLevel ) const {
    return messageLevel <= GetMessageLevel() ?
      StdPrint() :
      0;
  }


  template<>
  inline int UnionString<wchar>::StdPrint() const {
    DWORD dw;
    WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), ToChar(), GetLength(), &dw, nullptr );
    return dw;
  }


  template<>
  inline int UnionString<wchar>::StdPrint( int messageLevel ) const {
    return messageLevel <= GetMessageLevel() ?
      StdPrint() :
      0;
  }


  template<>
  inline int UnionString<char>::StdPrintLine() const {
    DWORD dw;
    WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), ToChar(), GetLength(), &dw, nullptr );
    WriteConsole( GetStdHandle( STD_OUTPUT_HANDLE ), "\n", 1, &dw, nullptr );
    return dw;
  }


  template<>
  inline int UnionString<char>::StdPrintLine( int messageLevel ) const {
    return messageLevel <= GetMessageLevel() ?
      StdPrintLine() :
      0;
  }


  template<>
  inline int UnionString<wchar>::StdPrintLine() const {
    DWORD dw;
    WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), ToChar(), GetLength(), &dw, nullptr );
    WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), "\n", 1, &dw, nullptr );
    return dw;
  }


  template<>
  inline int UnionString<wchar>::StdPrintLine( int messageLevel ) const {
    return messageLevel <= GetMessageLevel() ?
      StdPrintLine() :
      0;
  }


  template<typename T>
  void UnionString<T>::StdSetCodepage( int codepage ) {
    SetConsoleOutputCP( codepage );
    SetConsoleCP( codepage );
  }


  template<typename T>
  void UnionString<T>::SetMessageLevel( int messageLevel ) {
    GetMessageLevel() = messageLevel;
  }


  template<typename T>
  int& UnionString<T>::GetMessageLevel() {
    static int* level = (int*)CreateSharedSingleton( "MessageLevel", []() -> void* { return new int( 0 ); } );
    return *level;
  }
#endif
#pragma endregion

#pragma region service_simple
  template<typename T>
  uint UnionString<T>::GetLength() const {
    return Length;
  }


  template<typename T>
  void UnionString<T>::SetLength( size_t length ) {
    if( Length > length ) {
      Length = length;
      Free();
    }
    else if( Length < length ) {
      Length = length;
      Allocate();
    }
  }


  template<typename T>
  void UnionString<T>::ShrinkToFit() {
    size_t length = str_get_length( Data );
    SetLength( length );
  }


  template<typename T>
  uint UnionString<T>::GetAllocatedLength() const {
    return Reserved;
  }


  template<typename T>
  bool UnionString<T>::IsEmpty() const {
    return Length == 0;
  }


  template<typename T>
  bool UnionString<T>::IsBlank() const {
    for( size_t i = 0; i < Length; i++ ) {
      bool isBlank =
        Data[i] == ' ' ||
        Data[i] == '\t' ||
        Data[i] == '\r' ||
        Data[i] == '\n' ||
        Data[i] == '\0';

      if( !isBlank )
        return false;
    }

    return true;
  }


  template<typename T>
  bool UnionString<T>::StartsWith( const T* c_str, Flags flags ) const {
    uint length = str_get_length( c_str );
    if( Length < length )
      return false;

          T* i = Data;
    const T* j = c_str;
    if( check_flag( flags, Flags::IgnoreCase ) ) {
      while( *j != 0 )
        if( char_to_upper( *(j++) ) != char_to_upper( *(i++) ) )
          return false;
    }
    else {
      while( *j != 0 )
        if( *(j++) != *(i++) )
          return false;
    }
    return true;
  }


  template<typename T>
  bool UnionString<T>::EndsWith( const T* c_str, Flags flags ) const {
    uint length = str_get_length( c_str );
    if( Length < length )
      return false;

          T* i = Data + Length - length;
    const T* j = c_str;
    if( check_flag( flags, Flags::IgnoreCase ) ) {
      while( *j != 0 )
        if( char_to_upper( *(j++) ) != char_to_upper( *(i++) ) )
          return false;
    }
    else {
      while( *j != 0 )
        if( *(j++) != *(i++) )
          return false;
    }
    return true;
  }


  template<typename T>
  bool UnionString<T>::IsSame( const T* c_str, Flags flags ) const {
    return check_flag( flags, Flags::IgnoreCase ) ?
      str_compare_ignore_case( Data, c_str ) == 0 :
      str_compare( Data, c_str ) == 0;
  }


  template<typename T>
  int UnionString<T>::GetDifference( const T* c_str, Flags flags ) const {
    return check_flag( flags, Flags::IgnoreCase ) ?
      str_compare_ignore_case( Data, c_str ) :
      str_compare( Data, c_str );
  }


  template<typename T>
  bool UnionString<T>::IsMatchesMask( const T* c_str, Flags flags ) const {
          T* l = Data;
    const T* r = c_str;
    const T* i = r;
    bool ignoreCase = check_flag( flags, Flags::IgnoreCase );
    bool firstMatch = true;
    do {
      if( *i == '*' || *i == 0 ) {
        int len = i - r;
        T* found = ignoreCase ?
          str_search_ignore_case( l, -1, 0, r, len ) :
          str_search( l, -1, 0, r, len );

        if( found == nullptr )
          return false;

        // The first part of the mask should 
        // starts from first index of the data.
        if( firstMatch ) {
          firstMatch = false;
          if( len > 0 && found != Data )
            return false;
        }

        if( *i == 0 ) {
          // The last part of the mask should
          // ends at the last index of the data.
          if( len > 0 && found != Data + (Length - len) )
            return false;

          break;
        }

        l = found + len;
        r = i + 1;
      }
    } while( *(i++) != 0 );

    return true;
  }


  template<typename T>
  bool UnionString<T>::IsMatchesCharacters( const T* c_str, Flags flags ) const {
    bool ignoreCase = check_flag( flags, Flags::IgnoreCase );
    for( uint i = 0; i < Length; i++ )
      if( !char_is_matches_to_range( Data[i], c_str, ignoreCase ) )
        return false;

    return true;
  }


  template<typename T>
  uint UnionString<T>::Search( const T* c_str, Flags flags ) const {
    return Search( c_str, -1, flags );
  }


  template<typename T>
  uint UnionString<T>::Search( const T* c_str, uint pos, Flags flags ) const {
    T* found = nullptr;
    switch( flags ) {
      case Flags::Default:                             
        found = str_search( Data, Length, pos, c_str, -1 );
        break;
      case Flags::IgnoreCase:
        found = str_search_ignore_case( Data, Length, pos, c_str, -1 );
        break;
      case Flags::SearchInReverse:                     
        found = str_search_in_reverse( Data, Length, pos, c_str, Length );
        break;
      case Flags::SearchInReverse + Flags::IgnoreCase: 
        found = str_search_in_reverse_ignore_case( Data, Length, pos, c_str, Length );
        break;
    }

    return found != nullptr ?
      (uint)(found - Data) :
      -1;
  }


  template<typename T>
  bool UnionString<T>::Contains( const T* c_str, Flags flags ) const {
    return Search( c_str, flags ) != -1;
  }


  template<typename T>
  uint UnionString<T>::GetContainsCount( const T* c_str, Flags flags ) const {
    uint length = str_get_length( c_str );
    if( length == 0 )
      return -1;

    uint count = 0;
    uint start = 0;
    while( start != -1 ) {
      start = Search( c_str, start, flags );
      if( start != -1 ) {
        start += length;
        count++;
      }
    }

    return count;
  }


  template<typename T>
  template<typename _Lambda>
  void UnionString<T>::WalkThroughLines( _Lambda proc /*void(*proc)(const UnionString<T>&)*/ ) {
    const T* start = Data;
    const T* end = Data;
    while( true ) {
      if( *end == '\r' || *end == '\n' || *end == '\0' ) {
        proc( UnionString<T>( start, end ) );
        if( *end == '\0' )
          return;

        while( true ) {
          if( *end != '\r' && *end != '\n' ) {
            start = end;
            if( *end == '\0' )
              return;

            break;
          }

          end++;
        }
      }
      
      end++;
    }
  }
#pragma endregion

#pragma region service_advanced
  template<typename T>
  UnionString<T>& UnionString<T>::PadLeft( uint length, T sym ) {
    if( Length >= length )
      return *this;

    uint addleft = length - Length;
    uint movelen = Length;
    Length = length;
    Allocate();

    str_move( Data, addleft, 0, movelen );
    for( uint i = 0; i < addleft; i++ )
      Data[i] = sym;

    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::PadRight( uint length, T sym ) {
    if( Length >= length )
      return *this;

    uint addright = length - Length;
    uint fillstart = Length;
    Length = length;
    Allocate();

    for( uint i = fillstart; i < Length; i++ )
      Data[i] = sym;

    return *this;
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetWord( int index ) const {
    return GetWord( _lpStrT( " \t\r\n\f\v" ), index );
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetWord( const T* separators, int index ) const {
    if( index < 0 )
      return GetWordInReverse( separators, -index );

    T* start = Data;
    while( *start != 0 && char_is_matches_to_range( *(++start), separators, false ) );

    T* i = start;
    int counter = 0;
    do {
      if( *i == 0 || char_is_matches_to_range( *i, separators, false ) ) {
        if( ++counter >= index ) {
          uint length = i - start;
          return UnionString( start, length );
        }
        else if( *i != 0 ) {
          start = i;
          while( *start != 0 && char_is_matches_to_range( *(++start), separators, false ) );
          i = start;
        }
        else
          break;
      }
    } while( *(i++) );
    return UnionString::GetEmpty();
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetWordInReverse( const T* separators, int index ) const {
    T* start = Data + Length - 1;
    while( start > Data && char_is_matches_to_range( *(--start), separators, false ) );

    T* i = start;
    int counter = 0;
    do {
      if( i < Data || char_is_matches_to_range( *i, separators, false ) ) {
        if( ++counter >= index ) {
          uint length = start - i;
          return UnionString( start - length + 1, length );
        }
        else if( i >= Data ) {
          start = i;
          while( start > Data && char_is_matches_to_range( *(--start), separators, false ) );
          i = start;
        }
        else
          break;
      }
    } while( i-- >= Data );
    return UnionString::GetEmpty();
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetWordBetween( const T* lword, const T* rword, int index ) const {
    if( lword == nullptr && rword == nullptr )
      return *this;

    if( index < 0 )
      return GetWordBetweenInReverse( lword, rword, -index );

    int counter = 0;
    int offset = -1;
    T* lFound = Data;
    T* rFound = Data + Length - 1;
    while( true ) {
      if( lword != nullptr ) {
        lFound = str_search( Data, Length, offset + 1, lword, -1 );
        if( lFound == nullptr )
          return UnionString::GetEmpty();
        offset = lFound - Data;
      }

      if( rword != nullptr ) {
        rFound = str_search( Data, Length, offset + 1, rword, -1 );
        if( rFound == nullptr )
          return UnionString::GetEmpty();
        offset = rFound - Data;
      }

      if( ++counter < index )
        continue;

      lFound += lword != nullptr ? str_get_length( lword ) : 0;
      return UnionString( lFound, rFound - lFound );
    }

    return UnionString::GetEmpty();
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetWordBetweenInReverse( const T* lword, const T* rword, int index ) const {
    int counter = 0;
    int offset = 0;
    T* lFound = Data;
    T* rFound = Data + Length - 1;
    while( true ) {
      if( rword != nullptr ) {
        rFound = str_search_in_reverse( Data, Length, offset - 1, rword, -1 );
        if( rFound == nullptr )
          return UnionString::GetEmpty();
        offset = rFound - Data;
      }

      if( lword != nullptr ) {
        lFound = str_search_in_reverse( Data, Length, offset - 1, lword, -1 );
        if( lFound == nullptr )
          return UnionString::GetEmpty();
        offset = lFound - Data;
      }

      if( ++counter < index )
        continue;

      lFound += lword != nullptr ? str_get_length( lword ) : 0;
      return UnionString( lFound, rFound - lFound );
    }

    return UnionString::GetEmpty();
  }


  template<typename T>
  Array<UnionString<T>> UnionString<T>::Split( const T* separators, bool ignoreEmpty ) const {
    Array<UnionString> words;
    if( Length >= 0 ) {
      T* i = Data;
      T* j = i;
      if( !ignoreEmpty )
        goto forceBegin;
      do {
        if( !char_is_matches_to_range( *i, separators, false ) ) {
          j = i + 1;
        forceBegin:
          do {
            if( *j == 0 || char_is_matches_to_range( *j, separators, false ) ) {
              words.Insert( UnionString( i, j - i ) );
              if( *j == 0 )
                return words.Share();

              if( ignoreEmpty ) {
                i = j;
                break;
              }
              i = j + 1;
            }
          } while( *(j++) != 0 );
        }
      } while( *(++i) != 0 );
    }

    return words.Share();
  }


  template<typename T>
  Array<UnionString<T>> UnionString<T>::Split( T separator, bool ignoreEmpty ) const {
    T separators[2] = { separator, 0 };
    return Split( separators, ignoreEmpty );
  }


  template<typename T>
  Array<UnionString<T>> UnionString<T>::SplitSmart( Flags flags ) const {
    Array<UnionString<T>> array;
    int start = 0;
    while( true ) {
      UnionString word = GetWordSmart( 1, start, flags );
      if( word.IsEmpty() )
        break;

      start += word.GetLength();
      array.Insert( word );
    }

    return array.Share();
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetWordSmart( int index, Flags flags ) const {
    int pos = 0;
    return GetWordSmart( index, pos, flags );
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetWordSmart( int index, IN OUT int& pos, Flags flags ) const {
    WordGroups group;
    return GetWordSmart( index, pos, group, flags );
  }
  

#pragma region
  template<typename T>
  const T* _GetSymbolsGroup( StringBase::WordGroups index ) {
    switch( index ) {
      case 0: return _lpStrT( " \t\r\n\f\v" );
      case 1: return _lpStrT( ".:;[]()+-~!&*/%<>=^|,'@\\\"" );
      case 2: return _lpStrT( "0123456789.abcdefxABCDEFXh" );
      case 3: return _lpStrT( "_#'/0123456789" );
    }
    return nullptr;
  }

  inline bool _IsLatinLetter( int symbol ) {
    return
      symbol >= 'a' && symbol <= 'z' ||
      symbol >= 'A' && symbol <= 'Z';
  }

  template<typename T>
  static StringBase::WordGroups StartSymbolGroup( T symbol ) {
    if( _IsLatinLetter( symbol ) )                                                                return StringBase::Letters;
    if( char_is_matches_to_range( symbol, _GetSymbolsGroup<T>( StringBase::Spaces ),    false ) ) return StringBase::Spaces;
    if( char_is_matches_to_range( symbol, _GetSymbolsGroup<T>( StringBase::Operators ), false ) ) return StringBase::Operators;
    if( char_is_matches_to_range( symbol, _GetSymbolsGroup<T>( StringBase::Digits ),    false ) ) return StringBase::Digits;
    if( char_is_matches_to_range( symbol, _GetSymbolsGroup<T>( StringBase::Letters ),   false ) ) return StringBase::Letters;
                                                                                                  return StringBase::Other;
  }

  template<typename T>
  bool _IsMatchesSymbolGroup( T symbol, StringBase::WordGroups group ) {
    if( group == StringBase::Other || group == StringBase::Strings )
      return false;

    if( group == StringBase::Letters && _IsLatinLetter( symbol ) )
      return true;

    return char_is_matches_to_range( symbol, _GetSymbolsGroup<T>( group ), false );
  }

  template<typename T>
  StringBase::WordGroups NextSymbolGroup( T symbol, StringBase::WordGroups group ) {
    if( group > StringBase::Spaces && _IsMatchesSymbolGroup( symbol, group ) )
      return group;

    return StartSymbolGroup( symbol );
  }
#pragma endregion



  template<typename T>
  UnionString<T> UnionString<T>::GetWordSmart( int index, IN OUT int& pos, OUT WordGroups& group, Flags flags ) const {
    if( Length == 0 || index <= 0 ) {
      pos = -1;
      return UnionString::GetEmpty();
    }

    bool splitOperators  = check_flag( flags, Flags::SplitOperators );
    bool detectStrings   = check_flag( flags, Flags::DetectStrings );
    bool skipCppComments = check_flag( flags, Flags::SkipCppComments );
    bool skipAsmComments = check_flag( flags, Flags::SkipAsmComments );

    int counter = 0;
    T* i = Data + pos;
    T* left = i;
    group = Spaces;

    do {
      WordGroups nextGroup = NextSymbolGroup( *i, group );
      bool toSplit =
        (nextGroup != group) ||
        (splitOperators && nextGroup == Operators);

      if( toSplit ) {
        if( group != Spaces ) {
          if( ++counter >= index ) {
            pos = left - Data;
            return UnionString( left, i - left );
          }
        }

        left = i;
        if( nextGroup == Operators ) {
          // This block combines words included
          // to the '..' or ".." string constructions.
          if( detectStrings ) {
            T quot = *i;
            if( quot == '\'' || quot == '\"' ) {
              group = Strings;
              while( *(i + 1) != 0 )
                if( *(i++) != '\\' && *i == quot )
                  break;
              continue;
            }
          }
          // Block skips comments in the C or Cpp style
          // and returns result without /*..*/ and //..
          if( skipCppComments ) {
            if( *i == '/' ) {
              if( *(i + 1) == '/' ) {
                group = Spaces;
                while( *(++i) != 0 && *i != '\r' && *i != '\n' );
                continue;
              }
              else if( *(i + 1) == '*' ) {
                group = Spaces;
                i = str_search<T>( i, -1, 0, _lpStrT("*/"), -1);
                if( i == nullptr )
                  break;
                i += 2;
                continue;
              }
            }
          }
          // Block skips comments in the Assembler
          // style and returns result without ;..
          if( skipAsmComments ) {
            if( *i == ';' ) {
              group = Spaces;
              while( *(++i) != 0 && *i != '\r' && *i != '\n' );
              continue;
            }
          }
        }

        group = nextGroup;
      }
    } while( *(i++) != 0 );

    pos = -1;
    return UnionString::GetEmpty();
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetFlipped() const {
    UnionString str;
    str.Length = Length;
    str.Allocate();

    T* i = str.Data;
    T* j = Data + Length - 1;
    do {
      *(i++) = *(j--);
    } while( *i != 0 );

    return str;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Clear() {
    Length = 0;
    Free();
    return *this;
  }


  template<typename T>
  UnionString<T> UnionString<T>::SubString( uint pos, uint length ) const {
    return UnionString( Data + pos, length );
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Remove( uint length ) {
    Length -= length;
    Free();
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Remove( uint pos, uint length ) {
    if( pos == 0 && length >= Length )
      return Clear();

    uint cutMax = Length - pos;
    if( length > cutMax )
      length = cutMax;

    uint from = pos + length;
    str_move( Data, pos, from, Length - length );
    Length -= length;
    Free();
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Insert( const T* c_str ) {
    int length = str_get_length( c_str );
    int index = Length;
    Length += length;
    Allocate();
    str_copy( Data + index, c_str, length );
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Insert( const T* c_str, int index ) {
    int length = str_get_length( c_str );
    int oldLength = Length;
    Length += length;
    Allocate();
    str_move( Data, index + length, index, oldLength - index);
    str_copy( Data + index, c_str, length );
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Insert( T c ) {
    uint index = Length;
    Length += 1;
    Allocate();
    Data[index] = c;
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Insert( T c, int index ) {
    int oldLength = Length;
    Length += 1;
    Allocate();
    str_move( Data, index + 1, index, oldLength - index );
    Data[index] = c;
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Overlay( const T* c_str, int index ) {
    int length = str_get_length( c_str );
    int totalLength = index + length;
    if( totalLength > Length ) {
      Length = totalLength;
      Allocate();
    }
    str_copy( Data + index, c_str, length );
    return *this;
  }


  // Search and replace source words to the destination.
  // If the length of the source word is not equal
  // todestination, than methods will move memory
  // segments part by part (not a full string).
  template<typename T>
  UnionString<T>& UnionString<T>::Replace( const T* c_src, const T* c_dst, Flags flags ) {
    int srclen = str_get_length( c_src );
    int dstlen = str_get_length( c_dst );

    // Simple replace words if the source word length
    // is equal to destination word length.
    if( srclen == dstlen ) {
      if( str_compare( c_src, c_dst ) == 0 )
        return *this;

      uint start = 0;
      while( true ) {
        start = Search( c_src, start, flags );
        if( start == -1 )
          break;

        T* found = Data + start;
        start += srclen;
        str_copy( found, c_dst, dstlen );
      }
    }

    // Replace words if the source word length
    // more than destination word length. Move
    // all parts of the string to the left.
    else if( srclen > dstlen ) {
      uint subLength = srclen - dstlen;
      uint newDataEnd = 0;
      uint moveStart = 0;
      uint newLength = Length;

      while( true ) {
        uint found = Search( c_src, moveStart, flags );
        uint moveEnd = found == -1 ? Length : found;
        uint moveLength = moveEnd - moveStart;
        str_copy( Data + newDataEnd, Data + moveStart, moveLength );
        newDataEnd += moveLength;
        str_copy( Data + newDataEnd, c_dst, dstlen );

        if( found == -1 ) {
          Length = newLength;
          Free();
          break;
        }

        newDataEnd += dstlen;
        newLength -= subLength;
        moveStart = moveEnd + srclen;
      }
    }

    // Replace words if the source word length
    // less than destination word length. Move
    // all parts of the string to the right.
    else {
      uint addLength = dstlen - srclen;
      uint replaceNum = GetContainsCount( c_src );
      uint oldLength = Length;
      Length += addLength * replaceNum;
      Allocate();

      uint moveEnd = oldLength;
      uint newDataEnd = Length;
      while( true ) {
        uint found = Search( c_src, moveEnd - 1, flags | Flags::SearchInReverse );
        uint moveStart = found != -1 ? found + srclen : 0;
        uint moveLength = moveEnd - moveStart;

        str_copy( Data + newDataEnd - moveLength, Data + moveStart, moveLength );
        if( found == -1 )
          break;

        newDataEnd -= moveLength + dstlen;
        str_copy( Data + newDataEnd, c_dst, dstlen );
        moveEnd = moveStart - srclen;
      }
    }

    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::Trim( const T* characters ) {
    TrimLeft( characters );
    return TrimRight( characters );
  }


  template<typename T>
  UnionString<T>& UnionString<T>::TrimLeft( const T* characters ) {
    int cutLength = 0;
    T* i = Data;
    while( *i != 0 && char_is_matches_to_range( *(i++), characters, false ) )
      cutLength++;

    return Remove( 0, cutLength );
  }


  template<typename T>
  UnionString<T>& UnionString<T>::TrimRight( const T* characters ) {
    int cutLength = 0;
    T* i = Data + Length - 1;
    while( i >= Data && char_is_matches_to_range( *(i--), characters, false ) )
      cutLength++;

    return Remove( cutLength );
  }


  template<typename T>
  UnionString<T>& UnionString<T>::MakeUpper() {
    for( uint i = 0; i < Length; i++ )
      Data[i] = char_to_upper( Data[i] );
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::MakeLower() {
    for( uint i = 0; i < Length; i++ )
      Data[i] = char_to_lower( Data[i] );
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::MakeLowerFirstUpper() {
    if( Length > 0 ) {
      Data[0] = char_to_upper( Data[0] );
      for( uint i = 1; i < Length; i++ )
        Data[i] = char_to_lower( Data[i] );
    }
    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::FillWith( int value ) {
    for( int i = 0; i < Length; i++ )
      Data[i] = value;

    return *this;
  }


  template<typename T>
  UnionString<T>& UnionString<T>::MakeFullPath() {
    bool isFull = Length >= 2 && (Data[1] == ':' || (Data[0] == '\\' && Data[1] == '\\'));
    if( isFull == false ) {
      UnionString currentDirectory;
      ulong pathLength = get_current_directory( (T*)nullptr, 0 );
      currentDirectory.SetLength( pathLength );
      get_current_directory( currentDirectory.ToChar(), pathLength );
      currentDirectory[pathLength - 1] = '\\';
      *this = currentDirectory + *this;
    }

    Array<UnionString> pathPartsSrc = Split( '\\', false );
    Array<UnionString> pathPartsDst;
    for( auto& part : pathPartsSrc ) {
      if( part == _lpStrT( "." ) )
        continue;

      if( part == _lpStrT( ".." ) ) {
        pathPartsDst.RemoveLast();
        continue;
      }

      pathPartsDst.Insert( part );
    }

    Clear();
    for( uint i = 0; i < pathPartsDst.GetCount(); i++ ) {
      if( i > 0 ) Insert( (T)'\\');
      Insert( pathPartsDst[i] );
    }
    return this->Replace( _lpStrT( "/" ), _lpStrT( "\\" ) );
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetDirectory() const {
    uint i = GetLength() - 2;
    while( i > 0 ) {
      if( Data[i] == (T)'\\' || Data[i] == (T)'/' )
        return SubString( 0, i + 1 );
      i--;
    }
    return GetEmpty();
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetFileName() const {
    uint length = Length;
    if( Last() == '\\' || Last() == '/' )
      length--;
    uint it = length - 1;
    while( it > 0 ) {
      if( Data[it] == '\\' || Data[it] == '/' )
        return SubString( ++it, length - it );
      it--;
    }
    return *this;
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetFileNameNoExtension() const {
    UnionString fileName = GetFileName();
    uint index = fileName.GetLength() - 1;
    while( index != UINT_MAX ) {
      if( fileName[index] == '.' )
        return fileName.SubString( 0, index );
      index--;
    }
    return fileName;
  }


  template<typename T>
  UnionString<T> UnionString<T>::GetFileExtension() const {
    UnionString fileName = GetFileName();
    uint index = fileName.GetLength() - 1;
    while( index != UINT_MAX ) {
      if( fileName[index] == '.' )
        return fileName.SubString( index, GetLength() - index );
      index--;
    }
    return GetEmpty();
  }


  template<>
  inline UnionString<char> UnionString<char>::CreateDirectory() const {
    UnionString root = GetDirectory();
    if( !root.IsEmpty() )
      root.CreateDirectory();
    ::CreateDirectoryA( (char*)ToChar(), 0);
    return *this;
  }


  template<>
  inline UnionString<wchar_t> UnionString<wchar_t>::CreateDirectory() const {
    UnionString root = GetDirectory();
    if( !root.IsEmpty() )
      root.CreateDirectory();
    ::CreateDirectoryW( (wchar_t*)ToChar(), 0);
    return *this;
  }


  template<>
  inline bool UnionString<char>::IsExistsAsFile() const {
    return _access( (char*)ToChar(), 0) == 0;
  }


  template<>
  inline bool UnionString<wchar_t>::IsExistsAsFile() const {
    return _waccess( (wchar_t*)ToChar(), 0 ) == 0;
  }


  template<>
  inline bool UnionString<char>::IsExistsAsDirectory() const {
    return _access( (char*)ToChar(), 0 ) == 0;
  }


  template<>
  inline bool UnionString<wchar_t>::IsExistsAsDirectory() const {
    return _waccess( (wchar_t*)ToChar(), 0 ) == 0;
  }


  template<>
  inline void UnionString<char>::DeleteAsFile() const {
    DeleteFileA( (char*)ToChar() );
  }


  template<>
  inline void UnionString<wchar_t>::DeleteAsFile() const {
    DeleteFileW( (wchar_t*)ToChar() );
  }


  template<>
  inline void UnionString<char>::DeleteAsDirectory() const {
    RemoveDirectoryA( (char*)ToChar() );
  }


  template<>
  inline void UnionString<wchar_t>::DeleteAsDirectory() const {
    RemoveDirectoryW( (wchar_t*)ToChar() );
  }
#pragma endregion

#pragma region cast
  template<>
  inline bool UnionString<char>::IsAnsiString() const {
    return true;
  }


  template<>
  inline bool UnionString<char>::IsUnicodeString() const {
    return false;
  }


  template<>
  inline bool UnionString<wchar>::IsAnsiString() const {
    return false;
  }


  template<>
  inline bool UnionString<wchar>::IsUnicodeString() const {
    return true;
  }


  template<>
  inline UnionString<char> UnionString<char>::ToAnsi( int codepage ) const {
    return *this;
  }


  template<>
  inline UnionString<char> UnionString<wchar>::ToAnsi( int codepage ) const {
    if( codepage == -1 )
      codepage = (int)Locale::GetUserLocale().Codepage;

    UnionString<char> ansi;
    StringConverter::UTF16ToANSI( *this, ansi );
    return ansi;
  }


  template<>
  inline UnionString<wchar> UnionString<wchar>::ToUnicode( int codepage ) const {
    return *this;
  }


  template<>
  inline UnionString<wchar> UnionString<char>::ToUnicode( int codepage ) const {
    if( codepage == -1 )
      codepage = (int)Locale::GetUserLocale().Codepage;

    UnionString<wchar> unicode;
    StringConverter::ANSIToUTF16( *this, unicode, codepage );
    return unicode;
  }


  template<typename T>
  template<typename NewT>
  UnionString<NewT> UnionString<T>::ConvertTo() {
    // Is this Ansi -> convert to Unicode.
    if( is_ansi_type<T>() && is_unicode_type<NewT>() )
      return ToUnicode().ReinterpretTo<NewT>();
    
    // Is this Unicode -> convert to Ansi.
    if( is_unicode_type<T>() && is_ansi_type<NewT>() )
      return ToAnsi().ReinterpretTo<NewT>();
  
    // Types are identical.
    return ReinterpretTo<NewT>();
  }


  template<typename T>
  template<typename NewT>
  UnionString<NewT>& UnionString<T>::ReinterpretTo() {
    return *reinterpret_cast<UnionString<NewT>*>( this );
  }
#pragma endregion

#pragma region regular_expression

#define _basic_string(T) std::basic_string<T, std::char_traits<T>, std::allocator<T>>
#define _basic_match(T) std::match_results<_basic_string(T)::const_iterator>
#define _basic_regex(T) std::basic_regex<T>

  template<typename T>
  UnionStringRange<T> UnionString<T>::RegexSearch( const UnionString<T>& mask, uint pos ) const {
    UnionStringRange<T> range;
    try {
      _basic_string(T) next( Data + pos );
      _basic_match(T) match;
      _basic_regex(T) regex( mask.begin() );
      if( std::regex_search( next, match, regex ) ) {
        range.Data = Data;
        range.Begin = Data + pos + match.position();
        range.End = range.Begin + match.length();

        // match.
      }
    }
    catch( std::regex_error e ) {}
    return range;
  }

  template<typename T>
  Array<UnionStringRange<T>> UnionString<T>::RegexSearchAll( const UnionString<T>& mask ) const {
    Array<UnionStringRange<T>> ranges;
    uint pos = 0;
    while( true ) {
      UnionStringRange<T> range = RegexSearch( mask, pos );
      if( range.Begin == nullptr )
        break;

      pos = range.GetIndex() + range.GetLength();
      ranges.Insert( range );
    }

    return ranges.Share();
  }

  template<typename T>
  typename UnionString<T>::Match UnionString<T>::RegexMatch( const UnionString& pattern ) const {
    // try {
    //   _basic_string(T) next( begin() );
    //   _basic_match(T) match;
    //   _basic_regex(T) regex( mask.begin() );
    //   if( std::regex_match( next, match, regex ) )
    //     return match.str().c_str();
    // }
    // catch( std::regex_error e ) { }
    // return UnionString::GetEmpty();
    

    Match result;
    try {
      // std::basic_regex<T> regexPattern( pattern.ToChar() );
      _basic_regex( T ) regexPattern( pattern.begin() );
      // std::match_results<const T*> regexMatch;
      _basic_match( T ) regexMatch;
      // std::basic_string<T, std::char_traits<T>, std::allocator<T>> input( begin() );
      _basic_string( T ) next( begin() );
      if( std::regex_search( next, regexMatch, regexPattern ) )
        for( auto&& it : regexMatch )
          result.Items.Insert( it.str().c_str());
    }
    catch( const std::regex_error& e ) {
      std::cerr << "Regular expression error: " << e.what() << std::endl;
    }

    return result;
  }


  template<typename T>
  bool UnionString<T>::RegexIsMatchesMask( const UnionString& mask ) const {
    return !RegexMatch( mask ).IsEmpty();
  }


  template<typename T>
  UnionString<T>& UnionString<T>::RegexReplace( const UnionString& mask, const UnionString& to ) {
    try {
      _basic_string(T) src( begin() );
      _basic_string(T) dst( to.begin() );
      _basic_regex(T) regex( mask.begin() );
      auto result = std::regex_replace( src, regex, dst );
      (*this) = result.c_str();
    }
    catch( std::regex_error e ) { }
    return *this;
  }

#undef _basic_string
#undef _basic_match
#undef _basic_regex

#pragma endregion

#pragma region io
  template<typename T>
  void UnionString<T>::SetEOL( char* eol ) {
    // TODO: 
  }


  template<typename T>
  bool UnionString<T>::ReadFromFile( const char* fileName, int defaultEncoding ) {
    FileReader fileReader( fileName );
    if( !fileReader.IsOpened() )
      return false;

    return ReadFromFile( &fileReader, defaultEncoding );
  }


  template<typename T>
  bool UnionString<T>::ReadFromFile( const wchar* fileName, int defaultEncoding ) {
    FileReader fileReader( fileName );
    if( !fileReader.IsOpened() )
      return false;

    return ReadFromFile( &fileReader, defaultEncoding );
  }


  inline size_t file_get_size_to_read( Stream* file ) {
    return file->GetSize() - file->GetPosition();
  }


  template<typename T>
  int UnionString<T>::ReadFileEncoding( Stream* file, int defaultEncoding ) {
    byte bom[3] = { 0, 0, 0 };
    size_t bomLength = file->Read( bom, 3 );
    if( str_compare_byte_order_mark( bom, BOM_UTF8 ) ) {
      file->SetPosition( 3, SEEK_SET );
      return Encodings::UTF8;
    }
    else if( str_compare_byte_order_mark( bom, BOM_UTF16_LE ) ) {
      file->SetPosition( 2, SEEK_SET );
      return Encodings::UTF16;
    }

    file->SetPosition( 0, SEEK_SET );
    return defaultEncoding;
  }


  template<>
  inline bool UnionString<char>::ReadFromFile( Stream* file, int defaultEncoding ) {
    if( file == nullptr )
      return false;

    Clear();
    // _setmode( _fileno( file ), 0x8000 ); // TODO ?
    int encoding = ReadFileEncoding( file, defaultEncoding );
    size_t fileLength = file_get_size_to_read( file );

    switch( encoding ) {
      case -1:
      case Encodings::ANSI:
      {
        SetLength( fileLength );
        file->Read( ToChar(), fileLength );
        break;
      }
      case Encodings::UTF8:
      {
        StringUTF8 utf8;
        utf8.SetLength( fileLength );
        file->Read( utf8.ToChar(), fileLength );
        StringConverter::UTF8ToANSI( utf8, *this );
        break;
      }
      case Encodings::UTF16: {
        StringUTF16 utf16;
        utf16.SetLength( fileLength / 2 );
        file->Read( utf16.ToChar(), fileLength );
        StringConverter::UTF16ToANSI( utf16, *this );
        break;
      }
      default: return false;
    }
    
    return true;
  }


  template<>
  inline bool UnionString<wchar>::ReadFromFile( Stream* file, int defaultEncoding ) {
    if( file == nullptr )
      return false;

    Clear();
    // _setmode( _fileno( file ), 0x8000 ); // TODO ?
    int encoding = ReadFileEncoding( file, defaultEncoding );
    size_t fileLength = file_get_size_to_read( file );

    switch( encoding ) {
      case -1:
      case Encodings::ANSI:
      {
        StringANSI ansi;
        ansi.SetLength( fileLength );
        file->Read( ansi.ToChar(), fileLength );
        StringConverter::ANSIToUTF16( ansi, *this, (int)Locale::GetUserLocale().Codepage );
        break;
      }
      case Encodings::UTF8:
      {
        StringUTF8 utf8;
        utf8.SetLength( fileLength );
        file->Read( utf8.ToChar(), fileLength );
        StringConverter::UTF8ToUTF16( utf8, *this );
        break;
      }
      case Encodings::UTF16:
      {
        SetLength( fileLength / 2 );
        file->Read( ToChar(), fileLength );
        break;
      }
      default: return false;
    }

    return true;
  }


  template<typename T>
  bool UnionString<T>::WriteToFile( const char* fileName, int encoding ) {
    FileWriter writer( fileName );
    if( !writer.IsOpened() )
      return false;

    return WriteToFile( &writer, encoding );
  }


  template<typename T>
  bool UnionString<T>::WriteToFile( const wchar* fileName, int encoding ) {
    FileWriter writer( fileName );
    if( !writer.IsOpened() )
      return false;

    return WriteToFile( &writer, encoding );
  }


  template<>
  inline bool UnionString<char>::WriteToFile( Stream* file, int encoding ) {
    if( !file )
      return false;

    switch( encoding ) {
      case Encodings::UTF8: {
        StringUTF8 utf8;
        StringConverter::ANSIToUTF8( *this, utf8 );
        file->Write( (void*)"\xEF\xBB\xBF", 3 );
        file->Write( utf8.ToChar(), utf8.GetLength() );
        break;
      }
      case Encodings::UTF16: {
        StringUTF16 utf16;
        StringConverter::ANSIToUTF16( *this, utf16 );
        file->Write( (void*)"\xFF\xFE", 2 );
        file->Write( utf16.ToChar(), utf16.GetLength() * 2 );
        break;
      }
      case Encodings::ANSI: {
        file->Write( ToChar(), GetLength() );
        break;
      }
      default: return false;
    }
    return true;
  }


  template<>
  inline bool UnionString<wchar>::WriteToFile( Stream* file, int encoding ) {
    if( !file )
      return false;

    switch( encoding ) {
      case Encodings::UTF8: {
        StringUTF8 utf8;
        StringConverter::UTF16ToUTF8( *this, utf8 );
        file->Write( (void*)"\xEF\xBB\xBF", 3 );
        file->Write( utf8.ToChar(), utf8.GetLength() );
        break;
      }
      case Encodings::UTF16: {
        file->Write( (void*)"\xFF\xFE", 2 );
        file->Write( ToChar(), GetLength() * 2 );
        break;
      }
      case Encodings::ANSI: {
        StringANSI ansi;
        StringConverter::UTF16ToANSI( *this, ansi );
        file->Write( ansi.ToChar(), ansi.GetLength() );
        break;
      }
      default: return false;
    }
    return true;
  }
#pragma endregion

#pragma region statics
  template<typename T>
  UnionString<T> UnionString<T>::FormatRaw( const T* format, ... ) {
    UnionString result;
    va_list va;
    va_start( va, format );
    for( const T* i = format; *i != 0; i++ ) {
      if( *i != '%' ) {
        result += *i;
        continue;
      }

      switch( *(++i) ) {
        case VaInt: {
          int vaInt = va_arg( va, int );
          result += UnionString( vaInt );
          break;
        }
        case VaInt64: {
          int64 vaInt64 = va_arg( va, int64 );
          result += UnionString( vaInt64 );
          break;
        }
        case VaUInt: {
          uint vaUInt = va_arg( va, uint );
          result += UnionString( vaUInt );
          break;
        }
        case VaUInt64: {
          uint64 vaUInt64 = va_arg( va, uint64 );
          result += UnionString( vaUInt64 );
          break;
        }
        case VaHex: {
          uint vaUInt = va_arg( va, uint );
          result += UnionString::MakeHexadecimal( vaUInt );
          break;
        }
        case VaHex64: {
          uint64 vaUInt64 = va_arg( va, uint64 );
          result += UnionString::MakeHexadecimal( vaUInt64 );
          break;
        }
        case VaSingle: {
          double vaDouble = va_arg( va, double );
          int mantissa = 3;

          // Find and apply a custom mantissa
          // length for this real value.
          const T* flt = i + 1;
          if( *flt == '.' ) {
            int _mantissa = 0;
            while( *(++flt) != 0 ) {
              if( *flt >= '0' && *flt <= '9' ) {
                _mantissa = _mantissa * 10 + *flt - '0';
                continue;
              }
              if( *flt == 'f' ) {
                mantissa = _mantissa;
                i = flt;
              }
              break;
            }
          }

          result += UnionString( vaDouble, mantissa );
          break;
        }
        case VaDouble: {
          double vaDouble = va_arg( va, double );
          int mantissa = 3;

          // Find and apply a custom mantissa
          // length for this real value.
          const T* flt = i + 1;
          if( *flt == '.' ) {
            int _mantissa = 0;
            while( *(++flt) != 0 ) {
              if( *flt >= '0' && *flt <= '9' ) {
                _mantissa = _mantissa * 10 + *flt - '0';
                continue;
              }
              if( *flt == 'f' ) {
                mantissa = _mantissa;
                i = flt;
              }
              break;
            }
          }

          result += UnionString( vaDouble, mantissa );
          break;
        }
        case VaBoolean: {
          int vaInt = va_arg( va, int );
          result += UnionString( vaInt ? true : false );
          break;
        }
        case VaChar: {
          char vaChar = va_arg( va, char );
          result += vaChar;
          break;
        }
        case VaUChar: {
          byte vaByte = va_arg( va, byte );
          result += UnionString( vaByte );
          break;
        }
        case VaLPSTR: {
          char* vaLPSTR = va_arg( va, char* );
          result += UnionString<char>( vaLPSTR ).ConvertTo<T>();
          break;
        }
        case VaLPWSTR: {
          wchar* vaLPWSTR = va_arg( va, wchar* );
          result += UnionString<wchar>( vaLPWSTR ).ConvertTo<T>();
          break;
        }
        case VaUnionString: {
          UnionString& vaStringSource = va_arg( va, UnionString<T> );
          result += vaStringSource.IsUnicodeString() ?
            vaStringSource.ReinterpretTo<wchar>().ConvertTo<T>() :
            vaStringSource.ReinterpretTo<char>() .ConvertTo<T>();
          break;
        }
        case VaGothicString: {
          UnionString<char>& vaString = va_arg( va, UnionString<char> );
          result += vaString.ConvertTo<T>();
          break;
        }
        case VaLine: {
          result += '\n';
          break;
        }
        case VaPercent: {
          result += '%';
          break;
        }
        case 0: {
          break;
        }
        default: {
          result += '%';
          result += *i;
          break;
        }
      }
    }
    va_end( va );
    return result;
  }


  template<class T>
  template<typename... Args>
  static UnionString<T> UnionString<T>::Format( const T* format, Args... args ) {
    UnionString result;
    UnionString<T> arguments[] = { args... };

    auto iterator = format;
    while( *iterator ) {
      if( *iterator == '{' ) {
        if( *(++iterator) == '{' ) {
          result.Insert( *(iterator++) );
          continue;
        }

        auto start = iterator;
        while( *(++iterator) && *iterator != '}' );
        auto id = UnionString( start, iterator ).ConvertToUInt();
        result.Insert( arguments[id] );
      }
      else
        result.Insert( *iterator );

      iterator++;
    }

    return result;
  }


  template<class T>
  UnionString<T> UnionString<T>::Format( const T* format ) {
    return format;
  }

  
  template<class T>
  UnionString<T> UnionString<T>::MakeHexadecimal( void* address ) {
    return UnionString( (const T*)_lpStrT( "0x" ) ) + UnionString( (uint)address, 16 ).PadLeft( 8, '0' );
  }


  template<class T>
  UnionString<T> UnionString<T>::MakeHexadecimal( int digit ) {
    return UnionString( (const T*)_lpStrT( "0x" ) ) + UnionString( digit, 16 ).PadLeft( 8, '0' );
  }


  template<class T>
  UnionString<T> UnionString<T>::MakeHexadecimal( uint digit ) {
    return UnionString( (const T*)_lpStrT("0x")) + UnionString(digit, 16).PadLeft(8, '0');
  }


  template<class T>
  UnionString<T> UnionString<T>::MakeHexadecimal( int64 digit ) {
    return UnionString( (const T*)_lpStrT( "0x" ) ) + UnionString( digit, 16 ).PadLeft( 16, '0' );
  }


  template<class T>
  UnionString<T> UnionString<T>::MakeHexadecimal( uint64 digit ) {
    return UnionString( (const T*)_lpStrT( "0x" ) ) + UnionString( digit, 16 ).PadLeft( 16, '0' );
  }
#pragma endregion


#pragma region conversion
  inline bool StringConverter::UTF8ToANSI( const StringUTF8& input, StringANSI& output ) {
    int size = MultiByteToWideChar( CP_UTF8, 0, input.ToChar(), -1, NULL, 0 );
    if( size == 0 ) {
      return false;
    }
    StringUTF16 utf16String;
    utf16String.SetLength( size );
    if( MultiByteToWideChar( CP_UTF8, 0, input.ToChar(), -1, utf16String.ToChar(), size) == 0 ) {
      return false;
    }
    int ansiSize = WideCharToMultiByte( CP_ACP, 0, utf16String.ToChar(), -1, NULL, 0, NULL, NULL );
    if( ansiSize == 0 ) {
      return false;
    }
    output.SetLength( ansiSize );
    if( WideCharToMultiByte( CP_ACP, 0, utf16String.ToChar(), -1, output.ToChar(), ansiSize, NULL, NULL) == 0 ) {
      return false;
    }
    return true;
  }


  inline bool StringConverter::UTF16ToANSI( const StringUTF16& input, StringANSI& output ) {
    int size = WideCharToMultiByte( CP_ACP, 0, input.ToChar(), -1, NULL, 0, NULL, NULL );
    if( size == 0 ) {
      return false;
    }
    output.SetLength( size );
    if( WideCharToMultiByte( CP_ACP, 0, input.ToChar(), -1, output.ToChar(), size, NULL, NULL) == 0 ) {
      return false;
    }
    return true;
  }


  inline bool StringConverter::ANSIToUTF8( const StringANSI& input, StringUTF8& output, int codepage ) {
    if( codepage == -1 )
      codepage = (int)Locale::GetUserLocale().Codepage;

    int size = MultiByteToWideChar( codepage, 0, input.ToChar(), -1, NULL, 0 );
    if( size == 0 ) {
      return false;
    }
    StringUTF16 wideString;
    wideString.SetLength( size );
    if( MultiByteToWideChar( codepage, 0, input.ToChar(), -1, wideString.ToChar(), size ) == 0 ) {
      return false;
    }
    int utf8Size = WideCharToMultiByte( CP_UTF8, 0, wideString.ToChar(), -1, NULL, 0, NULL, NULL );
    if( utf8Size == 0 ) {
      return false;
    }
    output.SetLength( utf8Size );
    if( WideCharToMultiByte( CP_UTF8, 0, wideString.ToChar(), -1, output.ToChar(), utf8Size, NULL, NULL) == 0 ) {
      return false;
    }
    return true;
  }


  inline bool StringConverter::UTF16ToUTF8( const StringUTF16& input, StringUTF8& output ) {
    int size = WideCharToMultiByte( CP_UTF8, 0, input.ToChar(), -1, NULL, 0, NULL, NULL );
    if( size == 0 ) {
      return false;
    }
    output.SetLength( size );
    if( WideCharToMultiByte( CP_UTF8, 0, input.ToChar(), -1, output.ToChar(), size, NULL, NULL) == 0 ) {
      return false;
    }
    return true;
  }


  inline bool StringConverter::ANSIToUTF16( const StringANSI& input, StringUTF16& output, int codepage ) {
    if( codepage == -1 )
      codepage = (int)Locale::GetUserLocale().Codepage;

    int size = MultiByteToWideChar( codepage, 0, input.ToChar(), -1, NULL, 0 );
    if( size == 0 ) {
      return false;
    }
    output.SetLength( size );
    if( MultiByteToWideChar( codepage, 0, input.ToChar(), -1, output.ToChar(), size) == 0 ) {
      return false;
    }
    return true;
  }


  inline bool StringConverter::UTF8ToUTF16( const StringUTF8& input, StringUTF16& output ) {
    int size = MultiByteToWideChar( CP_UTF8, 0, input.ToChar(), -1, NULL, 0 );
    if( size == 0 ) {
      return false;
    }
    output.SetLength( size );
    if( MultiByteToWideChar( CP_UTF8, 0, input.ToChar(), -1, output.ToChar(), size) == 0 ) {
      return false;
    }
    return true;
  }
#pragma endregion


  template<typename T>
  inline StringANSI ToHEX_ANSI( T* value ) {
    return StringANSI::MakeHexadecimal( value );
  }

  template<typename T>
  inline StringANSI ToHEX_ANSI( T value ) {
    return StringANSI::MakeHexadecimal( value );
  }

  template<typename T>
  inline StringUTF16 ToHEX_UTF16( T* value ) {
    return StringUTF16::MakeHexadecimal( value );
  }

  template<typename T>
  inline StringUTF16 ToHEX_UTF16( T value ) {
    return StringUTF16::MakeHexadecimal( value );
  }

#ifdef _UNICODE
#define ToHEX ToHEX_UTF16
#else
#define ToHEX ToHEX_ANSI
#endif
}
#pragma pop_macro("max")
#pragma pop_macro("min")

#undef clamp
#endif // __UNION_STRING_H__


#if defined(__UNION_VDFS_H__) && defined(__UNION_CLASS_VDFS__)
#ifndef __UNION_STRING_VDFS_
#define __UNION_STRING_VDFS_
namespace Union {
  template<typename T>
  bool UnionString<T>::ReadFromVDFS( const char* fileName, int defaultEncoding ) {
    int systems = VDF_PHYSICAL | VDF_VIRTUAL;
    auto file = VDFS::GetDefaultInstance().GetFile( fileName, systems );
    if( !file )
      return false;

    auto stream = file->Open();
    auto result = ReadFromFile( stream, defaultEncoding );
    stream->Close();
    return result;
  }


  template<typename T>
  bool UnionString<T>::ReadFromVDFS( const wchar* fileName, int defaultEncoding ) {
    StringANSI ansi;
    StringConverter::UTF16ToANSI( fileName, ansi );
    return ReadFromVDFS( ansi, defaultEncoding );
  }
}
#endif // __UNION_VDFS_
#endif // defined(__UNION_VDFS_H__) && defined(__UNION_CLASS_VDFS__)
#pragma once
#ifndef __UNION_STREAM_H__
#define __UNION_STREAM_H__

#include <memory>
#include <stdio.h>
#include "Types.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#define clamp(_value, _min, _max) std::min( std::max( _value, _min ), _max )

namespace Union {
  class UNION_API Stream {
  public:
    virtual bool IsOpened() const = 0;
    virtual size_t GetSize() const = 0;
    virtual size_t Read( void* where, size_t length ) = 0;
    virtual size_t Write( void* where, size_t length ) = 0;
    virtual void SetPosition( size_t position, int origin = SEEK_SET ) = 0;
    virtual size_t GetPosition() const = 0;
    virtual void Flush() = 0;
    virtual void Close() = 0;
    virtual Stream* OpenCopy() = 0;
    virtual HANDLE GetHandle() = 0;
    virtual ~Stream();
  };


  class UNION_API FileReader : public Stream {
    FILE* Handle;
    size_t Size;
    char* FileName;
    wchar* FileNameUnicode;
    bool Exclusize;
  public:
    FileReader( const char* fileName, bool exclusive = false );
    FileReader( const wchar* fileName, bool exclusive = false );
    virtual bool IsOpened() const;
    virtual size_t GetSize() const;
    virtual size_t Read( void* where, size_t length );
    virtual size_t ReadLine( void* where, int eol );
    virtual size_t Write( void* where, size_t length );
    virtual void SetPosition( size_t position, int origin = SEEK_SET );
    virtual size_t GetPosition() const;
    virtual void Flush();
    virtual void Close();
    virtual Stream* OpenCopy();
    virtual HANDLE GetHandle();
    virtual ~FileReader();
  };


  class UNION_API FileWriter : public Stream {
    FILE* Handle;
    size_t Size;
    char* FileName;
    wchar* FileNameUnicode;
    bool Exclusize;
  public:
    FileWriter( const char* fileName, bool append = false );
    FileWriter( const wchar* fileName, bool append = false );
    virtual bool IsOpened() const;
    virtual size_t GetSize() const;
    virtual size_t Read( void* where, size_t length );
    virtual size_t Write( void* where, size_t length );
    virtual void SetPosition( size_t position, int origin = SEEK_SET );
    virtual size_t GetPosition() const;
    virtual void Flush();
    virtual void Close();
    virtual Stream* OpenCopy();
    virtual HANDLE GetHandle();
    virtual ~FileWriter();
  };


#if 0 // TODO
  class UNION_API MemoryStream : public Stream {
    void** MemoryPtr;
    size_t Size;
    size_t Position;
  public:
    MemoryStream();
    MemoryStream( void*& memory );
    virtual bool IsOpened() const;
    virtual int GetSize() const;
    virtual size_t Read( void* where, size_t length );
    virtual size_t Write( void* where, size_t length );
    virtual void SetPosition( size_t position, int origin = SEEK_SET );
    virtual size_t GetPosition() const;
    virtual void Flush();
    virtual void Close();
    byte* GetMemory();
    const byte* GetMemory() const;
    virtual HANDLE GetHandle();
    virtual ~MemoryStream();
  };
#endif


  inline Stream::~Stream() {
  }

#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
#pragma region file_reader
  inline FileReader::FileReader( const char* fileName, bool exclusive ) {
    Exclusize = exclusive;
    size_t fileNameLen = strlen( fileName );
    FileName = new char[fileNameLen + 1];
    strcpy_s( FileName, fileNameLen + 1, fileName );
    FileNameUnicode = nullptr;

    fopen_s( &Handle, fileName, exclusive ? "rb+" : "rb" );
    if( Handle != nullptr ) {
      size_t position = GetPosition();
      SetPosition( 0, SEEK_END );
      Size = GetPosition();
      SetPosition( position, SEEK_SET );
    }
    else
      Size = 0;
  }


  inline FileReader::FileReader( const wchar* fileName, bool exclusive ) {
    Exclusize = exclusive;
    int fileNameLen = lstrlenW( fileName );
    FileNameUnicode = new wchar[fileNameLen + 1];
    lstrcpyW( FileNameUnicode, fileName );
    FileName = nullptr;

    _wfopen_s( &Handle, fileName, exclusive ? L"rb+" : L"rb" );
    if( Handle != nullptr ) {
      size_t position = GetPosition();
      SetPosition( 0, SEEK_END );
      Size = GetPosition();
      SetPosition( position, SEEK_SET );
    }
    else
      Size = 0;
  }


  inline bool FileReader::IsOpened() const {
    return Handle != nullptr;
  }


  inline size_t FileReader::GetSize() const {
    return Size;
  }


  inline size_t FileReader::Read( void* where, size_t length ) {
    return fread( where, 1, length, Handle );
  }


  inline size_t FileReader::ReadLine( void* where, int eol ) {
    byte* buffer = (byte*)where;
    while( Read( buffer, 1 ) > 0 ) {
      if( *(buffer++) == eol )
        break;
    }
    return static_cast<size_t>(buffer - (byte*)where);
  }


  inline size_t FileReader::Write( [[maybe_unused]] void* where, [[maybe_unused]] size_t length ) {
    return 0; // pass
  }


  inline void FileReader::SetPosition( size_t position, int origin ) {
    _fseeki64( Handle, static_cast<__int64>( position ), origin );
  }


  inline size_t FileReader::GetPosition() const {
    return static_cast<size_t>( _ftelli64( Handle ) );
  }


  inline void FileReader::Flush() {
    // pass
  }


  inline void FileReader::Close() {
    if( Handle ) {
      fclose( Handle );
      Handle = nullptr;
      Size = 0;
    }
  }


  inline Stream* FileReader::OpenCopy() {
    if( FileName )
      return new FileReader( FileName, Exclusize );
    else
      return new FileReader( FileNameUnicode, Exclusize );
  }


  inline HANDLE FileReader::GetHandle() {
    return Handle;
  }


  inline FileReader::~FileReader() {
    Close();
    if( FileName )
      delete[] FileName;
    if( FileNameUnicode )
      delete[] FileNameUnicode;
  }
#pragma endregion


#pragma region file_writer
  inline FileWriter::FileWriter( const char* fileName, bool append ) {
    size_t fileNameLen = strlen( fileName );
    FileName = new char[fileNameLen + 1];
    strcpy_s( FileName, fileNameLen + 1, fileName );
    FileNameUnicode = nullptr;

    fopen_s( &Handle, fileName, append ? "ab+" : "wb+" );
    if( Handle != nullptr ) {
      size_t position = GetPosition();
      SetPosition( 0, SEEK_END );
      Size = GetPosition();
      SetPosition( position, SEEK_SET );
    }
    else
      Size = 0;
  }

  
  inline FileWriter::FileWriter( const wchar* fileName, bool append ) {
    const int fileNameLen = lstrlenW( fileName );
    FileNameUnicode = new wchar[fileNameLen + 1];
    lstrcpyW( FileNameUnicode, fileName );
    FileName = nullptr;

    _wfopen_s( &Handle, fileName, append ? L"ab+" : L"wb+" );
    if( Handle != nullptr ) {
      size_t position = GetPosition();
      SetPosition( 0, SEEK_END );
      Size = GetPosition();
      SetPosition( position, SEEK_SET );
    }
    else
      Size = 0;
  }


  inline bool FileWriter::IsOpened() const {
    return Handle != nullptr;
  }


  inline size_t FileWriter::GetSize() const {
    return Size;
  }


  inline size_t FileWriter::Read( [[maybe_unused]] void* where, [[maybe_unused]] size_t length ) {
    return 0; // pass
  }


  inline size_t FileWriter::Write( void* where, size_t length ) {
    size_t writed = fwrite( where, 1, length, Handle );
    Size += writed;
    return writed;
  }


  inline void FileWriter::SetPosition( size_t position, int origin ) {
    _fseeki64( Handle, static_cast<__int64>( position ), origin );
  }


  inline size_t FileWriter::GetPosition() const {
    return static_cast<size_t>( _ftelli64( Handle ) );
  }


  inline void FileWriter::Flush() {
    fflush( Handle );
  }


  inline void FileWriter::Close() {
    if( Handle ) {
      fclose( Handle );
      Handle = nullptr;
      Size = 0;
    }
  }


  inline Stream* FileWriter::OpenCopy() {
    if( FileName )
      return new FileWriter( FileName, Exclusize );
    else
      return new FileWriter( FileNameUnicode, Exclusize );
  }


  inline HANDLE FileWriter::GetHandle() {
    return Handle;
  }


  inline FileWriter::~FileWriter() {
    Close();
    if( FileName )
      delete[] FileName;
    if( FileNameUnicode )
      delete[] FileNameUnicode;
  }
#pragma endregion


#if 0 // TODO
#pragma region memory_stream
  MemoryStream::MemoryStream() {
    *MemoryPtr = shi_malloc( 0 );
    Size = 0;
    Position = 0;
  }


  MemoryStream::MemoryStream( void*& memory ) {
    if( !memory )
      memory = shi_malloc( 0 );

    MemoryPtr = &memory;
    Size = shi_msize( memory );
    Position = 0;
  }


  bool MemoryStream::IsOpened() const {
    return GetMemory() != nullptr;
  }


  int MemoryStream::GetSize() {
    return Size;
  }


  size_t MemoryStream::Read( void* where, size_t length ) {
    size_t canRead = Size - Position;
    size_t toRead = std::min( canRead, length );
    memcpy( where, GetMemory() + Position, toRead);
    Position += toRead;
    return toRead;
  }


  size_t MemoryStream::Write( void* where, size_t length ) {
    size_t farPosition = std::max( Size, Position );
    size_t newSize = farPosition + length;
    *MemoryPtr = shi_realloc( *MemoryPtr, newSize );


    if( Size > Position ) {
      size_t moveLength = Size - Position;
      memmove( GetMemory() + Position + length, GetMemory() + Position, length );
    }
    else {

    }
  }


  void MemoryStream::SetPosition( size_t position, int origin ) {
    fseek( Handle, position, origin );
  }


  size_t MemoryStream::GetPosition() const {
    return ftell( Handle );
  }


  void MemoryStream::Flush() {
    // pass
  }


  void MemoryStream::Close() {
    if( Handle ) {
      fclose( Handle );
      Handle = nullptr;
      Size = 0;
    }
  }


  byte* MemoryStream::GetMemory() {
    return (byte*)*MemoryPtr;
  }


  const byte* MemoryStream::GetMemory() const {
    return (byte*)*MemoryPtr;
  }


  MemoryStream::~MemoryStream() {
    // pass
  }
#pragma endregion
#endif
#endif
}
#undef clamp
#pragma pop_macro("max")
#pragma pop_macro("min")

#endif // __UNION_STREAM_H__

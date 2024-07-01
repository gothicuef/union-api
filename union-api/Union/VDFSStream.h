#pragma once
#ifndef __UNION_VDFS_STREAM_H__
#define __UNION_VDFS_STREAM_H__
#pragma warning(push)
#pragma warning(disable:4251)

#include "Types.h"
#include "Stream.h"
#include "Thread.h"
#include "vorbis/vorbisfile.h"
#include "zlib.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

namespace Union {
  class UNION_API StreamFilter : public Stream {
  protected:
    Stream* BaseStream;
    size_t StartPosition;
    size_t Size;
  public:
    StreamFilter( Stream* baseStream );
    virtual void SetPosition( size_t position, int origin = SEEK_SET );
    virtual size_t GetPosition() const;
    virtual int GetSize() const;
    virtual void SetStartPosition( size_t position );
    virtual void SetSize( size_t size );
    virtual Stream* const GetBaseStream() const;
  };

  class UNION_API StreamFilterCached : public StreamFilter {
    byte Cache[8192];
    size_t CachePosition;
    size_t CacheSize;
    void UpdateCache();
    size_t ReadFromCache( void* where, size_t length );
  public:
    StreamFilterCached( Stream* baseStream );
    StreamFilterCached( Stream* baseStream, size_t position, size_t size );
    virtual bool IsOpened() const;
    virtual size_t Read( void* where, size_t length );
    virtual size_t Write( void* where, size_t length );
    virtual void SetPosition( size_t position, int origin = SEEK_SET );
    virtual size_t GetPosition() const;
    virtual void Flush();
    virtual void Close();
    virtual Stream* OpenCopy();
    virtual HANDLE GetHandle();
    virtual ~StreamFilterCached();
  };


  class UNION_API Decompressor {
    friend struct Block;
    struct Block {
      Decompressor* Decompressor;
      byte* InputData;
      ulong InputDataLength;
      byte* OutputData;
      ulong OutputDataLength;
      EventSwitch Switch;
      Thread Thread;
      Semaphore* Semaphore;
      bool DestroyInput;

      Block();
      bool IsBusy();
      void WaitForEnd();
      void Decompress( void* input, size_t inputLength, void* output, size_t outputLength, bool destroyInput );
      void Decompress();
      static void DecompressionAsyncProcess( Block* block );
    };

    Semaphore Semaphore;
    Block Blocks[32];
    int CurrentBlock;
    Event ThreadAvailabilityState;

    Decompressor();
    ulong GetProcessorsCount();

  public:
    bool MultithreadingIsAvailable();
    bool IsBusy();
    void WaitForEnd();
    void Decompress( void* input, size_t inputLength, void* output, size_t outputLength, bool destroyInput = false );
    static Decompressor& GetInstance();
    static void ThreadAvailabilityNotifier( Event& event );
  };


  class UNION_API StreamFilterZIP : public StreamFilter {
    uint DecompressedSize;
    uint SizePerBlock;
    uint BlocksCount;
    byte* Cache;
    uint CachePosition;

  public:
    StreamFilterZIP( Stream* baseStream );
    StreamFilterZIP( Stream* baseStream, size_t position, size_t size );
    virtual bool IsOpened() const;
    virtual int GetSize() const;
    virtual size_t Read( void* where, size_t length );
    virtual size_t Write( void* where, size_t length );
    virtual void SetPosition( size_t position, int origin = SEEK_SET );
    virtual size_t GetPosition() const;
    virtual void Flush();
    virtual void Close();
    virtual Stream* OpenCopy();
    virtual HANDLE GetHandle();
    virtual ~StreamFilterZIP();
  };


  class UNION_API StreamFilterOGG : public StreamFilter {
#pragma pack(push, 1)
    struct WaveFileHeader {
      byte RIFF_SIG[4];
      ulong RiffSize;
      byte WAVE_SIG[4];
    };

    struct RiffChunk {
      byte CHUNK_NAME[4];
      ulong ChunkSize;
    };

    struct WaveFormatEx {
      ushort FormatTag;
      ushort Channels;
      ushort SamplesPerSec;
      ushort AvgBytesPerSec;
      ushort BlockAlign;
      ushort BitsPerSample;
    };

    struct WaveFile {
      WaveFileHeader Header;
      RiffChunk FormatChunk;
      WaveFormatEx Format;
      RiffChunk DataChunk;
    };
#pragma pack(pop)

    struct {
      OggVorbis_File Vorbis;
      ov_callbacks Callbacks;
      WaveFile Wave;
      uint Position;
      bool IsVorbis;
    }
    ogg;
  public:
    StreamFilterOGG( Stream* baseStream );
    virtual bool IsOpened() const;
    virtual int GetSize() const;
    virtual size_t Read( void* where, size_t length );
    virtual size_t Write( void* where, size_t length );
    virtual void SetPosition( size_t position, int origin = SEEK_SET );
    virtual size_t GetPosition() const;
    virtual void Flush();
    virtual void Close();
    virtual Stream* OpenCopy();
    virtual HANDLE GetHandle();
    virtual void SetStartPosition( size_t position );
    virtual void SetSize( size_t size );
    virtual ~StreamFilterOGG();

  protected:
    bool InitializeOggFormat();
    static uint VorbisRead( void* buffer, uint size, uint length, void* streamHandle );
    static int VorbisSeek( void* streamHandle, int64 position, int origin );
    static long VorbisTell( void* streamHandle );
    static int VorbisClose( void* streamHandle );
  };


#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
#pragma region stream_filter
  inline StreamFilter::StreamFilter( Stream* baseStream ) {
    BaseStream = baseStream;
    StartPosition = 0;
    Size = baseStream ? baseStream->GetSize() : 0;

    auto baseFilter = dynamic_cast<StreamFilter*>(baseStream);
    if( baseFilter )
      StartPosition = baseFilter->StartPosition;
  }


  inline void StreamFilter::SetStartPosition( size_t position ) {
    StartPosition = position;
    BaseStream->SetPosition( position );
  }


  inline void StreamFilter::SetSize( size_t size ) {
    StreamFilter::Size = size;
  }


  inline Stream* const StreamFilter::GetBaseStream() const {
    return BaseStream;
  }


  inline void StreamFilter::SetPosition( size_t position, int origin ) {
    switch( origin ) {
      case SEEK_SET: return BaseStream->SetPosition( StartPosition + position, SEEK_SET );
      case SEEK_CUR: return BaseStream->SetPosition( position, SEEK_CUR );
      case SEEK_END: return BaseStream->SetPosition( StartPosition + Size - position, SEEK_SET );
    }
  }


  inline size_t StreamFilter::GetPosition() const {
    return BaseStream->GetPosition() - StartPosition;
  }


  inline int StreamFilter::GetSize() const {
    return StreamFilter::Size;
  }
#pragma endregion


#pragma region stream_filter_cached
  inline StreamFilterCached::StreamFilterCached( Stream* baseStream ) : StreamFilter( baseStream ) {
    CachePosition = -1;
  }


  inline StreamFilterCached::StreamFilterCached( Stream* baseStream, size_t position, size_t size ) : StreamFilter( baseStream ) {
    CachePosition = -1;
    SetStartPosition( position );
    SetSize( size );
  }


  inline bool StreamFilterCached::IsOpened() const {
    return BaseStream->IsOpened();
  }


  inline void StreamFilterCached::UpdateCache() {
    size_t savedPosition = BaseStream->GetPosition();
    CacheSize = std::min( Size - GetPosition(), sizeof( Cache ) );
    BaseStream->Read( Cache, CacheSize );
    BaseStream->SetPosition( savedPosition );
    CachePosition = 0;
  }


  inline size_t StreamFilterCached::ReadFromCache( void* where, size_t length ) {
    if( CachePosition >= sizeof( Cache ) )
      UpdateCache();

    size_t maxToRead = CacheSize - CachePosition;
    size_t toRead = std::min( length, maxToRead );
    memcpy( where, Cache + CachePosition, toRead );
    CachePosition += toRead;
    SetPosition( toRead, SEEK_CUR );
    if( length > toRead )
      toRead += ReadFromCache( (byte*)where + toRead, length );

    return toRead;
  }


  inline size_t StreamFilterCached::Read( void* where, size_t length ) {
    size_t maxToRead = Size - GetPosition();
    size_t toRead = std::min( length, maxToRead );

    if( toRead > sizeof( Cache ) ) {
      CachePosition = -1;
      return BaseStream->Read( where, toRead );
    }
    
    return ReadFromCache( where, toRead );
  }


  inline size_t StreamFilterCached::Write( void* where, size_t length ) {
    return BaseStream->Write( where, length );
  }


  inline void StreamFilterCached::SetPosition( size_t position, int origin ) {
    StreamFilter::SetPosition( position, origin );
    CachePosition = -1;
  }


  inline size_t StreamFilterCached::GetPosition() const {
    return BaseStream->GetPosition() - StreamFilter::StartPosition;
  }


  inline void StreamFilterCached::Flush() {
    BaseStream->Flush();
  }


  inline void StreamFilterCached::Close() {
    if( BaseStream ) {
      BaseStream->Close();
      delete BaseStream;
      BaseStream = nullptr;
    }
  }


  inline Stream* StreamFilterCached::OpenCopy() {
    Stream* baseStream = BaseStream->OpenCopy();
    StreamFilterCached* streamFilter = new StreamFilterCached( baseStream );
    streamFilter->StartPosition = StartPosition;
    streamFilter->Size = Size;
    return streamFilter;
  }


  inline HANDLE StreamFilterCached::GetHandle() {
    return BaseStream->GetHandle();
  }


  inline StreamFilterCached::~StreamFilterCached() {
    Close();
  }
#pragma endregion


#pragma region decompressor
  inline Decompressor::Block::Block() : Switch( false ), Thread( &DecompressionAsyncProcess ) {
  }


  inline bool Decompressor::Block::IsBusy() {
    return Switch.GetState();
  }


  inline void Decompressor::Block::WaitForEnd() {
    return Switch.WaitForOff();
  }


  inline void Decompressor::Block::Decompress( void* input, size_t inputLength, void* output, size_t outputLength, bool destroyInput ) {
    InputData = (byte*)input;
    InputDataLength = inputLength;
    OutputData = (byte*)output;
    OutputDataLength = outputLength;
    DestroyInput = destroyInput;
    Switch.On();
  }


  inline Decompressor::Decompressor() : Semaphore( GetProcessorsCount() ), ThreadAvailabilityState( false ) {
    CurrentBlock = 0;
    for( int i = 0; i < 30; i++ ) {
      Block& block = Blocks[i];
      block.Decompressor = this;
      block.Semaphore = &Semaphore;
      block.Thread.Start( &block );
    }

    Thread notifierThread( &ThreadAvailabilityNotifier );
    notifierThread.Start( &ThreadAvailabilityState );
  }


  inline ulong Decompressor::GetProcessorsCount() {
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    return std::min( 1ul, sysinfo.dwNumberOfProcessors );
  }


  inline void Decompressor::Block::Decompress() {
    uncompress( OutputData, &OutputDataLength, InputData, InputDataLength );
    if( DestroyInput )
      delete[] InputData;
  }


  inline void Decompressor::Block::DecompressionAsyncProcess( Block* block ) {
    while( true ) {
      block->Switch.WaitForOn();
      block->Semaphore->Enter();
      block->Decompress();
      block->Semaphore->Leave();
      block->Switch.Off();
    }
  }


  inline bool Decompressor::MultithreadingIsAvailable() {
    return ThreadAvailabilityState.IsOpen();
  }


  inline bool Decompressor::IsBusy() {
    for( int i = 0; i < 30; i++ )
      if( Blocks[i].IsBusy() )
        return true;
    
    return false;
  }


  inline void Decompressor::WaitForEnd() {
    for( int i = 0; i < 30; i++ )
      Blocks[i].WaitForEnd();
  }


  inline void Decompressor::Decompress( void* input, size_t inputLength, void* output, size_t outputLength, bool destroyInput ) {
    if( !MultithreadingIsAvailable() ) {
      uncompress( (Bytef*)output, (uLongf*)&outputLength, (Bytef*)input, (uLongf)inputLength);
      if( destroyInput )
        delete[] input;
      return;
    }

    Block& block = Blocks[CurrentBlock];
    if( ++CurrentBlock > 30 )
      CurrentBlock = 0;

    block.WaitForEnd();
    block.Decompress( input, inputLength, output, outputLength, destroyInput );
  }


  inline Decompressor& Decompressor::GetInstance() {
    static Decompressor* instance =
      (Decompressor*)CreateSharedSingleton( "Decompressor", []() -> void* { return new Decompressor(); } );
    return *instance;
  }


  inline void Decompressor::ThreadAvailabilityNotifier( Event& event ) {
    event.Open();
  }
#pragma endregion


#pragma region stream_filter_zipped
  inline StreamFilterZIP::StreamFilterZIP( Stream* baseStream ) : StreamFilterZIP( baseStream, 0, 0 ) {
  }


  inline StreamFilterZIP::StreamFilterZIP( Stream* baseStream, size_t position, size_t size ) : StreamFilter( baseStream ) {
    SetStartPosition( position );
    SetSize( size );

    BaseStream->Read( &DecompressedSize, 4 );
    BaseStream->Read( &SizePerBlock, 4 );
    BaseStream->Read( &BlocksCount, 4 );
    Cache = new byte[DecompressedSize];

    byte* where = Cache;
    for( uint i = 0; i < BlocksCount; i++ ) {
      uint sourceLength;
      uint compressedLength;
      uint blockSize;
      BaseStream->Read( &sourceLength, 4 );
      BaseStream->Read( &compressedLength, 4 );
      BaseStream->Read( &blockSize, 4 );

      byte* compressedData = new byte[compressedLength];
      size_t readed = BaseStream->Read( compressedData, compressedLength );
      Decompressor::GetInstance().Decompress( compressedData, compressedLength, where, sourceLength, true );
      where += sourceLength;
    }

    Decompressor::GetInstance().WaitForEnd();
    CachePosition = 0;
  }


  inline bool StreamFilterZIP::IsOpened() const {
    return BaseStream->IsOpened();
  }


  inline int StreamFilterZIP::GetSize() const {
    return DecompressedSize;
  }


  inline size_t StreamFilterZIP::Read( void* where, size_t length ) {
    size_t maxToRead = std::min( length, DecompressedSize - CachePosition );
    memcpy( where, Cache + CachePosition, length );
    return maxToRead;
  }


  inline size_t StreamFilterZIP::Write( void* where, size_t length ) {
    return 0;
  }


  inline void StreamFilterZIP::SetPosition( size_t position, int origin ) {
    switch( origin ) {
      case SEEK_SET: CachePosition = position; break;
      case SEEK_CUR: CachePosition += position; break;
      case SEEK_END: CachePosition = DecompressedSize - position; break;
    }
  }


  inline size_t StreamFilterZIP::GetPosition() const {
    return CachePosition;
  }


  inline void StreamFilterZIP::Flush() {
  }


  inline void StreamFilterZIP::Close() {
    if( BaseStream ) {
      BaseStream->Close();
      delete BaseStream;
      BaseStream = null;
    }

    if( Cache ) {
      delete[] Cache;
      Cache = nullptr;
    }
  }


  inline Stream* StreamFilterZIP::OpenCopy() {
    Stream* baseStream = BaseStream->OpenCopy();
    StreamFilterZIP* streamFilter = new StreamFilterZIP( baseStream );
    streamFilter->StartPosition = StartPosition;
    streamFilter->Size = Size;
    return streamFilter;
  }


  inline HANDLE StreamFilterZIP::GetHandle() {
    return BaseStream->GetHandle();
  }


  inline StreamFilterZIP::~StreamFilterZIP() {
    Close();
  }
#pragma endregion


#pragma region stream_filter_ogg
  inline StreamFilterOGG::StreamFilterOGG( Stream* baseStream ) : StreamFilter( baseStream ) {
    ogg.IsVorbis = false;
    memset( &ogg.Vorbis, 0, sizeof( OggVorbis_File ) );
    memset( &ogg.Wave, 0, sizeof( WaveFile ) );

    memcpy( &ogg.Wave.Header.RIFF_SIG, "RIFF", 4 );
    memcpy( &ogg.Wave.Header.WAVE_SIG, "WAVE", 4 );
    memcpy( &ogg.Wave.FormatChunk.CHUNK_NAME, "fmt ", 4 );
    memcpy( &ogg.Wave.DataChunk.CHUNK_NAME, "data", 4 );

    ogg.Wave.FormatChunk.ChunkSize = sizeof( WaveFormatEx );
    ogg.Wave.Format.FormatTag = 1;
    ogg.Wave.Format.BitsPerSample = 16;

    ogg.Callbacks.read_func = VorbisRead;
    ogg.Callbacks.seek_func = VorbisSeek;
    ogg.Callbacks.tell_func = VorbisTell;
    ogg.Callbacks.close_func = VorbisClose;

    char tag[4];
    BaseStream->Read( tag, sizeof(tag) );
    BaseStream->SetPosition( 0 );
    if( memcmp( tag, "Ogg", 3 ) == 0 )
      ogg.IsVorbis = InitializeOggFormat();
    
    BaseStream->SetPosition( 0 );
  }


  inline bool StreamFilterOGG::InitializeOggFormat() {
    bool IsOpenCallbacks = !ov_open_callbacks( (void*)this, &ogg.Vorbis, nullptr, 0, ogg.Callbacks );
    if( !IsOpenCallbacks )
      return false;

    vorbis_info* info = ov_info( &ogg.Vorbis, -1 );
    size_t pcm = (size_t)ov_pcm_total( &ogg.Vorbis, -1 );

    ogg.Wave.Format.Channels = info->channels;
    ogg.Wave.Format.SamplesPerSec = (ushort)info->rate;
    ogg.Wave.Format.BlockAlign = ogg.Wave.Format.Channels * ogg.Wave.Format.BitsPerSample / 8;
    ogg.Wave.Format.AvgBytesPerSec = ogg.Wave.Format.SamplesPerSec * ogg.Wave.Format.BlockAlign;
    ogg.Wave.DataChunk.ChunkSize = pcm * ogg.Wave.Format.BlockAlign;
    ogg.Wave.Header.RiffSize = ogg.Wave.DataChunk.ChunkSize + sizeof( WaveFile ) - 8;
    return true;
  }


  inline bool StreamFilterOGG::IsOpened() const {
    return BaseStream->IsOpened();
  }


  inline int StreamFilterOGG::GetSize() const {
    if( !ogg.IsVorbis )
      return BaseStream->GetSize();

    return ogg.Wave.Header.RiffSize + 8;
  }


  inline size_t StreamFilterOGG::Read( void* where, size_t length ) {
    if( !ogg.IsVorbis )
      return BaseStream->Read( where, length );

    size_t leftToRead = length;

    // Read the WAV header of the OGG file
    if( ogg.Position < sizeof( WaveFile ) ) {
      size_t maxToRead = std::min( leftToRead, sizeof( WaveFile ) - ogg.Position );
      memcpy( where, &ogg.Wave, maxToRead);
      ogg.Position += maxToRead;
      (char*&)where += maxToRead;
      leftToRead -= maxToRead;
    }

    if( leftToRead == 0 )
      return length;

    int bitstream = -1;
    while( leftToRead > 0 ) {
      long readed = ov_read( &ogg.Vorbis, (char*)where, leftToRead, 0, 2, 1, &bitstream );
      if( readed <= 0 )
        break;
      
      (char*&)where += readed;
      leftToRead -= readed;
    }

    return length - leftToRead;
  }


  inline size_t StreamFilterOGG::Write( void* where, size_t length ) {
    return 0;
  }


  inline void StreamFilterOGG::SetPosition( size_t position, int origin ) {
    ov_pcm_seek( &ogg.Vorbis, position );
  }


  inline size_t StreamFilterOGG::GetPosition() const {
    return (size_t)ov_pcm_tell( (OggVorbis_File*)&ogg.Vorbis);
  }


  inline void StreamFilterOGG::Flush() {
    // pass
  }


  inline void StreamFilterOGG::Close() {
    if( BaseStream ) {
      if( ogg.IsVorbis )
        ov_clear( &ogg.Vorbis );

      memset( &ogg.Vorbis, 0, sizeof( OggVorbis_File ) );
      BaseStream->Close();
      delete BaseStream;
      BaseStream = nullptr;
    }
  }


  inline Stream* StreamFilterOGG::OpenCopy() {
    Stream* baseStream = BaseStream->OpenCopy();
    StreamFilterOGG* streamFilter = new StreamFilterOGG( baseStream );
    return streamFilter;
  }


  inline HANDLE StreamFilterOGG::GetHandle() {
    return BaseStream->GetHandle();
  }


  inline void StreamFilterOGG::SetStartPosition( size_t position ) {
    StreamFilter::SetStartPosition( position );
  }


  inline void StreamFilterOGG::SetSize( size_t size ) {
    StreamFilter::SetSize( size );
  }


  inline StreamFilterOGG::~StreamFilterOGG() {
    Close();
  }


  inline uint StreamFilterOGG::VorbisRead( void* where, uint size, uint length, void* streamHandle ) {
    return ((StreamFilterOGG*)streamHandle)->BaseStream->Read( where, length );
  }


  inline int StreamFilterOGG::VorbisSeek( void* streamHandle, int64 position, int origin ) {
    ((StreamFilterOGG*)streamHandle)->BaseStream->SetPosition( (size_t)position, origin );
    return ((StreamFilterOGG*)streamHandle)->BaseStream->GetPosition();
  }


  inline long StreamFilterOGG::VorbisTell( void* streamHandle ) {
    return ((StreamFilterOGG*)streamHandle)->BaseStream->GetPosition();
  }


  inline int StreamFilterOGG::VorbisClose( void* streamHandle ) {
    return 0;
  }
#pragma endregion
#endif
}
#pragma pop_macro("max")
#pragma pop_macro("min")

#pragma warning(pop)
#endif // __UNION_VDFS_STREAM_H__
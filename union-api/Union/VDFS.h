
#ifndef __UNION_VDFS_H__
#define __UNION_VDFS_H__
#pragma warning(push)
#pragma warning(disable:4251)

#define VDF_VIRTUAL 1
#define VDF_PHYSICAL 2
#define VDF_PHYSICALFIRST 4

#ifndef OUT
#define OUT
#endif
#ifndef IN
#define IN
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#include "Memory.h"
#include "String.h"
#include "VDFSStream.h"

#define ZIPPED_VOLUME_FLAG 160

namespace Union {
  /**
   * Compact virtual file system class
   */
  class UNION_API VDFS {
  public:
    /**
     * The last error from the vdfs methods
     */
    StringANSI LastError;
    StringANSI InitialDirectory;

    class UNION_API File {
      friend class VDFS;
      Stream* BaseStream;
      bool Zipped;
      StringANSI FullName;
      StringANSI FullNameVirtual;
      StringANSI Name;
      size_t Position;
      size_t Size;
      uint Timestamp;
      int ReferenceCount;
      File();
      StreamFilter* OpenWithFilter( Stream* baseStream ) const;
    public:
      /**
       * @brief Opens a new stream to read data from this file
       * @return A new stream reader
       */
      Stream* Open() const;
      /**
       * @brief Returns the name of the current file
       * @return The name of the file
       */
      const StringANSI& GetName() const;
      /**
       * @brief Returns the full name of the current file
       * @return The full name of the file
       */
      const StringANSI& GetFullName() const;
      /**
       * @brief Returns the size of the current file
       * @return The size of the file
       */
      size_t GetSize() const;
      /**
       * @brief Returns the timestamp of the current file
       * @return The timestamp of the file
       */
      uint GetTimestamp() const;
      /**
       * @brief Returns system where the file is placed: VDF_VIRTUAL or VDF_PHYSICAL
       * @return A system where the file is placed: VDF_VIRTUAL or VDF_PHYSICAL
       */
      int GetSystem() const;
      /**
       * @brief Increases the count of the references for this file
       * @return A new count of references
       */
      int Acquire();
      /**
       * @brief Reduces the count of the references for this file
       * @return A new count of references. If value is Zero, object was deleted
       */
      int Release();
      /**
       * @brief Sortion function for the files using the full names
       * @param l The left file
       * @param r The right file
       * @return A dirrefence between two files
       */
      static int Sortion_ByFullName( File* const& l, File* const& r );
      static int Sortion_ByFullNameStringed( File* const& l, StringANSI const& r );
      /**
       * @brief Sortion function for the files using the short names
       * @param l The left file
       * @param r The right file
       * @return A dirrefence between two files
       */
      static int Sortion_ByName( File* const& l, File* const& r );
      static int Sortion_ByNameStringed( File* const& l, StringANSI const& r );
    };

    class UNION_API Volume {
      friend class VDFS;
      Stream* BaseStream;
      StringANSI FullName;
      StringANSI Name;
      uint Timestamp;
      uint DataPosition;
      uint Flags;
      Array<File*> Files_ByName;
      Array<File*> Files_ByFullName;
      bool ReadNextEntry( const StringANSI& dir );
    public:
      /**
       * @brief Opens this volume from the file
       * @param fullName Full path to the volume file
       * @param tryExclusive Tries to open this file as exclusive (to write data for example)
       * @return True if file was opened, False if not
       */
      bool Open( const StringANSI& fullName, bool tryExclusive = false );
      /**
       * @brief Closes this volume stream
       * @return True if file was closed, False if not
       */
      bool Close();
      /**
       * @brief Returns list of the files from this volume
       * @return The array with files
       */
      const Array<const File*>& GetFileList() const;
      /**
       * @brief Returns the name of this volume
       * @return The string name
       */
      const StringANSI& GetName() const;
      /**
       * @brief Returns the full name of this volume
       * @return The string name
       */
      const StringANSI& GetFullName() const;
      /**
       * @brief Returns the raw timestamp of this volume
       * @return The raw timestamp
       */
      const uint GetTimestamp() const;
    };

  protected:
    Array<Volume*> Volumes;
    struct {
      Array<File*> Files_ByName;
      Array<File*> Files_ByFullName;
    } Virtual, Physical;

    const File* GetFile_ByFullName( const StringANSI& name, IN OUT int& system ) const;
    Array<const File*> GetFileVariants_ByFullName( const StringANSI& name, IN OUT int& systems ) const;
    void LoadDirectoryVDF( const StringANSI& rootDirectory );
    void LoadDirectoryFiles( const StringANSI& rootDirectory, const StringANSI& localDirectory = "" );
    void InitDefault();

  public:
    /**
     * @brief Loads a new volume or returns existing one
     * @param name Local or full name to the volume
     * @return The pointer to the volume, or null if it is not found
     */
    const Volume* LoadVolume( const StringANSI& name );
    /**
     * @brief Returns file from the volume
     * @param name Short (without specified directory) or local (starts from the root directory) name of the file
     * @param systems In what systems the file can be found: VDF_VIRTUAL, VDF_PHYSICAL, VDF_PHYSICAL_FIRST
     * @return The pointer to the file, or null if it is not found
     */
    const File* GetFile( const StringANSI& name, IN OUT int& systems ) const;
    /**
     * @brief Returns an file list from any volumes and physical directories
     * @param name Short (without specified directory) or local (starts from the root directory) name of the file
     * @param systems In what systems the file can be found: VDF_VIRTUAL, VDF_PHYSICAL, VDF_PHYSICAL_FIRST
     * @return The list of pointer to the found files
     */
    Array<const File*> GetFileVariants( const StringANSI& name, IN int systems ) const;
    /**
     * @brief Returns file list from the vdf volumes
     * @return The array with files 
     */
    const Array<const File*>& GetVirtualFileList() const;
    /**
     * @brief Returns file list from the game directory
     * @return The array with files
     */
    const Array<const File*>& GetPhysicalFileList() const;
    /**
     * @brief Returns a loaded volume list
     * @return The array with volumes
     */
    const Array<const Volume*>& GetVolumes() const;
    /**
     * @brief Returns initial directory
     * @return The full path to the initial directory
     */
    StringANSI GetInitialDirectory() const;
    /**
     * @brief Loads files from specified firectory
     */
    void PostLoadDirectoryFiles( const StringANSI& directory );
    /**
     * @brief Loads VDF volumes from specified firectory
     */
    void PostLoadDirectoryVDF( const StringANSI& directory );
    /**
     * @brief Returns the common file system
     * @return The pointer to the system
     */
    static VDFS& GetDefaultInstance();
  };


  /**
   * Represents the shell of a VDFS file
   */
  class UNION_API VirtualFile {
    const char* Name;
    int Handle;
    int System;
  public:
    /**
     * @brief Creates a new virtual file
     * @param handle Full (relative to the root directory) or short name of the file
     * @param systems In what systems the file can be found: VDF_VIRTUAL, VDF_PHYSICAL, VDF_PHYSICAL_FIRST
     */
    VirtualFile( const char* name, int systems = VDF_VIRTUAL | VDF_PHYSICAL );
    /**
     * @brief Returns name of the file
     * @return Name of the file
     */
    const char* GetName() const;
    /**
     * @brief Returns the existence of the file
     * @return True - exists, False - inexists
     */
    bool IsExists() const;
    /**
     * @brief Returns the file size
     * @return The file size
     */
    int GetSize() const;
    /**
     * @brief Reads content from the file
     * @param where Memory to which data will be copied
     * @param length Count in bytes to read
     * @return Cout of readed bytes. This calue can be less than 'length'
     */
    int Read( void* where, int length );
    /**
     * @brief Reads all bytes from the file
     * @param where Memory to which data will be copied
     * @return Cout of readed bytes
     */
    int ReadToEnd( void* where );
    /**
     * @brief Returns the caret position
     * @return Caret position
     */
    int GetPosition() const;
    /**
     * @brief Changes the caret in the file (position where is the reading comes from)
     * @param position New caret position
     * @param origin Origin to change: SEEK_SET, SEEK_CUR, SEEK_END
     * @return Nothing to return
     */
    void SetPosition( int position, int origin = SEEK_SET );
    /**
     * @brief Returns the system containing the file
     * @return VDF_VIRTUAL, VDF_PHYSICAL or Zero (file inexists)
     */
    int GetSysetm() const;
    /**
     * @brief Returns the file handle
     * @return Valid handle or (-1) (file inexists)
     */
    int GetHandle() const;
    /**
     * @brief Closes the file
     * @return Nothing to return
     */
    void Close();
    /**
     * @brief Closes the file and destructs this object
     */
    ~VirtualFile();
  };


#pragma region vdfs_api
  /**
   * @brief Opens a virtual file to read
   * @param file_name Full (relative to the root directory) or short name of the file
   * @param systems In what systems the file can be found: VDF_VIRTUAL, VDF_PHYSICAL, VDF_PHYSICAL_FIRST
   * @return Opened file handle or (-1) for error
   */
  static int vdf_fopen( const char* file_name, int systems ) {
    StringANSI fileName = StringANSI( file_name ).MakeUpper().TrimLeft( "\\" );
    const VDFS::File* file = VDFS::GetDefaultInstance().GetFile( fileName, systems );
    if( file == nullptr ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_open: file not found -> '{0}'", fileName );
      return -1;
    }

    Stream* stream = file->Open();
    return reinterpret_cast<int&>(stream);
  }
  /**
   * @brief Closes an opened file handle
   * @param handle Handle of the opened file
   * @return Non-zero if successful. Zero if stream handle is invalid.
   */
  static int vdf_fclose( int handle ) {
    if( handle == -1 ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_fclose: stream has invalid handle" );
      return 0;
    }

    Stream* stream = reinterpret_cast<Stream*>(handle);
    stream->Close();
    return 1;
  }
  /**
   * @brief Reads content from the opened file
   * @param handle Handle of the opened file
   * @param buffer Memory to which data will be copied
   * @param length Count in bytes to read
   * @return Cout of readed bytes. This calue can be less than 'length'
   */
  static int vdf_fread( int handle, void* buffer, int length ) {
    if( handle == -1 ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_fread: stream has invalid handle" );
      return 0;
    }

    Stream* stream = reinterpret_cast<Stream*>(handle);
    return stream->Read( buffer, length );
  }
  /**
   * @brief Changes the caret in the file (position where is the reading comes from)
   * @param handle Handle of the opened file
   * @param position New caret position
   * @return Non-zero if successful. Zero if stream handle is invalid.
   */
  static int vdf_fseek( int handle, int position ) {
    if( handle == -1 ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_fseek: stream has invalid handle" );
      return 0;
    }

    Stream* stream = reinterpret_cast<Stream*>(handle);
    stream->SetPosition( position );
    return 1;
  }
  /**
   * @brief Returns the caret position
   * @param handle Handle of the opened file
   * @return Caret position
   */
  static int vdf_ftell( int handle ) {
    if( handle == -1 ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_ftell: stream has invalid handle" );
      return 0;
    }

    Stream* stream = reinterpret_cast<Stream*>(handle);
    return stream->GetPosition();
  }
  /**
   * @brief Returns the systems where the file was found
   * @param file_name Full (relative to the root directory) or short name of the file
   * @param systems In what systems the file can be found: VDF_VIRTUAL, VDF_PHYSICAL, VDF_PHYSICAL_FIRST
   * @return VDF_VIRTUAL, VDF_PHYSICAL or Zero (not found)
   */
  static int vdf_fexists( const char* file_name, int systems ) {
    StringANSI fileName = StringANSI( file_name ).MakeUpper().TrimLeft( "\\" );
    VDFS::GetDefaultInstance().GetFile( fileName, systems );
    if( systems == 0 ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_fexists: file not found -> '{0}'", fileName );
    }
    return systems;
  }
  /**
   * @brief Returns the found list of full names
   * @param file_name Short name of the file to be found
   * @param full_file_names Buffer to be copied full file names (sepparated by '|')
   * @return Found files count
   */
  static int vdf_searchfile( const char* file_name, OUT char* full_file_names ) {
    StringANSI fileName = StringANSI( file_name ).MakeUpper().TrimLeft( "\\" );
    int systems = VDF_VIRTUAL | VDF_PHYSICAL;
    const VDFS::File* file = VDFS::GetDefaultInstance().GetFile( fileName, systems );
    if( file == nullptr ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_searchfile: file not found -> '{0}'", fileName );
      return 0;
    }

    StringANSI fullFileName = file->GetFullName();
    strcpy_s( full_file_names, fullFileName.GetLength() + 1, fullFileName );
    return 1;
  }
  /**
   * @brief Returns the file size
   * @param handle Handle of the opened file
   * @return The file size
   */
  static int vdf_ffilesize( int handle ) {
    if( handle == -1 ) {
      VDFS::GetDefaultInstance().LastError =
        StringANSI::Format( "vdf_ftell: stream has invalid handle" );
      return 0;
    }

    Stream* stream = reinterpret_cast<Stream*>(handle);
    return stream->GetSize();
  }
  /**
   * @brief Returns the last internal error of the VDFS
   * @param handle Buffer to be copied the error text
   * @return Text length
   */
  static int vdf_getlasterror( OUT char* error ) {
    StringANSI& lastError = VDFS::GetDefaultInstance().LastError;
    strcpy_s( error, lastError.GetLength() + 1, lastError );
    return lastError.GetLength();
  }
#pragma endregion


#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
#pragma region vdfs_file
  inline VDFS::File::File() {
    BaseStream = nullptr;
    Zipped = false;
    Position = 0;
    Size = 0;
    Timestamp = 0xFFFFFFFF;
    ReferenceCount = 1;
  }


  inline StreamFilter* VDFS::File::OpenWithFilter( Stream* baseStream ) const {
    if( Name.EndsWith( ".WAV" ) || Name.EndsWith( ".OGG" ) )
      return new StreamFilterOGG( baseStream );

    if( Zipped )
      return new StreamFilterZIP( baseStream, Position, Size );

    return new StreamFilterCached( baseStream, Position, Size );
  }


  inline Stream* VDFS::File::Open() const {
    if( BaseStream ) {
      Stream* copiedStream = BaseStream->OpenCopy();
      StreamFilter* filter = OpenWithFilter( copiedStream );
      return filter;
    }
    else {
      FileReader* reader = new FileReader( FullName );
      const_cast<uint&>( Size ) = reader->GetSize(); // Emergency update the file size
      return OpenWithFilter( reader );
    }
  }


  inline const StringANSI& VDFS::File::GetName() const {
    return Name;
  }


  inline const StringANSI& VDFS::File::GetFullName() const {
    return FullNameVirtual;
  }


  inline size_t VDFS::File::GetSize() const {
    return Size;
  }


  inline uint VDFS::File::GetTimestamp() const {
    return Timestamp;
  }


  inline int VDFS::File::GetSystem() const {
    return BaseStream ? VDF_VIRTUAL : VDF_PHYSICAL;
  }


  inline int VDFS::File::Acquire() {
    return ++ReferenceCount;
  }


  inline int VDFS::File::Release() {
    if( --ReferenceCount <= 0 ) {
      delete this;
      return 0;
    }
    return ReferenceCount;
  }


  inline int VDFS::File::Sortion_ByFullName( File* const& l, File* const& r ) {
    return l->FullNameVirtual.GetDifference( r->FullNameVirtual );
  }


  inline int VDFS::File::Sortion_ByFullNameStringed( File* const& l, StringANSI const& r ) {
    return l->FullNameVirtual.GetDifference( r );
  }


  inline int VDFS::File::Sortion_ByName( File* const& l, File* const& r ) {
    return l->Name.GetDifference( r->Name );
  }


  inline int VDFS::File::Sortion_ByNameStringed( File* const& l, StringANSI const& r ) {
    return l->Name.GetDifference( r );
  }
#pragma endregion


#pragma region vdfs_volume
  inline bool VDFS::Volume::ReadNextEntry( const StringANSI& dir ) {
    struct {
      char Name[64];
      uint Position;
      uint Size;
      uint Flags;
      uint Type;
    } entry;

    while( BaseStream->Read( &entry, sizeof( entry ) ) ) {
      StringANSI name = StringANSI( entry.Name, sizeof( entry.Name ) ).TrimRight();
      if( entry.Position < DataPosition ) {
        size_t savPos = BaseStream->GetPosition();
        size_t nextPos = 296 + 80 * entry.Position;
        BaseStream->SetPosition( nextPos );
        ReadNextEntry( StringANSI::Format( "{0}{1}\\", dir, name ) );
        BaseStream->SetPosition( savPos );
      }
      else {
        File* file = new File();
        file->BaseStream = BaseStream;
        file->Zipped = Flags == ZIPPED_VOLUME_FLAG;
        file->FullName = StringANSI::Format( "{0}{1}", dir, name );
        file->FullNameVirtual = file->FullName;
        file->Name = name;
        file->Position = entry.Position;
        file->Size = entry.Size;
        file->Timestamp = Timestamp;
        Files_ByFullName.Insert<File::Sortion_ByFullName>( file );
        Files_ByName.Insert<File::Sortion_ByName>( file );
      }

      if( entry.Flags & 0x40000000 )
        return true;
    }

    return false;
  }


  inline bool VDFS::Volume::Open( const StringANSI& fullName, bool tryExclusive ) {
    BaseStream = new FileReader( fullName, tryExclusive );
    if( !BaseStream->IsOpened() ) {
      delete BaseStream;
      BaseStream = nullptr;
      if( tryExclusive )
        return Open( fullName, false );
      
      return false;
    }

    if( BaseStream->GetSize() < 296 ) {
      delete BaseStream;
      BaseStream = nullptr;
      return false;
    }

    int entriesCount;
    BaseStream->SetPosition( 272 );
    BaseStream->Read( &entriesCount, sizeof( entriesCount ) );
    if( entriesCount == 0 ) {
      delete BaseStream;
      BaseStream = nullptr;
      return false;
    }

    BaseStream->SetPosition( 280 );
    BaseStream->Read( &Timestamp, sizeof( Timestamp ) );
    BaseStream->SetPosition( 288 );
    BaseStream->Read( &DataPosition, sizeof( DataPosition ) );
    BaseStream->Read( &Flags, sizeof( Flags ) );
    ReadNextEntry( StringANSI::GetEmpty() );
    return true;
  }


  inline bool VDFS::Volume::Close() {
    if( BaseStream ) {
      for( auto file : Files_ByFullName )
        file->Release();
      delete BaseStream;
      BaseStream = nullptr;
      return true;
    }

    return false;
  }


  inline const Array<const VDFS::File*>& VDFS::Volume::GetFileList() const {
    return (Array<const VDFS::File*>&)Files_ByFullName;
  }



  inline const StringANSI& VDFS::Volume::GetName() const {
    return Name;
  }



  inline const StringANSI& VDFS::Volume::GetFullName() const {
    return FullName;
  }



  inline const uint VDFS::Volume::GetTimestamp() const {
    return Timestamp;
  }
#pragma endregion


#pragma region vdfs
  inline const VDFS::Volume* VDFS::LoadVolume( const StringANSI& name ) {
    StringANSI fullName( name );
    fullName.MakeFullPath();
    for( auto volume : Volumes ) {
      if( volume->FullName == fullName )
        return volume;
    }
    Volume* volume = new Volume();
    if( !volume->Open( fullName ) ) {
      delete volume;
      return nullptr;
    }

    for( auto file : volume->Files_ByFullName ) {
      uint index = Virtual.Files_ByFullName.IndexOf<File::Sortion_ByFullName>( file );
      if( index != -1 && file->Timestamp > Virtual.Files_ByFullName[index]->Timestamp ) {
        Virtual.Files_ByFullName[index]->Release();
        Virtual.Files_ByFullName[index] = file;
        file->Acquire();
      }
      else {
        Virtual.Files_ByFullName.Insert<File::Sortion_ByFullName>( file );
        file->Acquire();
      }

      index = Virtual.Files_ByName.IndexOf<File::Sortion_ByName>( file );
      if( index != -1 && file->Timestamp > Virtual.Files_ByName[index]->Timestamp ) {
        Virtual.Files_ByName[index]->Release();
        Virtual.Files_ByName[index] = file;
        file->Acquire();
      }
      else {
        Virtual.Files_ByName.Insert<File::Sortion_ByName>( file );
        file->Acquire();
      }
    }

    Volumes.Insert( volume );
    return volume;
  }


  inline const VDFS::File* VDFS::GetFile_ByFullName( const StringANSI& name, IN OUT int& system ) const {
    File tmpFile;
    tmpFile.FullNameVirtual = name;
    tmpFile.FullNameVirtual.MakeUpper();
    bool physicalFirst = (system & VDF_PHYSICALFIRST) == VDF_PHYSICALFIRST;

    if( physicalFirst ) {
      uint index = Physical.Files_ByFullName.IndexOf<File::Sortion_ByFullName>( &tmpFile );
      if( index != -1 ) {
        system = VDF_PHYSICAL;
        // Physical.Files_ByFullName[index]->Acquire();
        return Physical.Files_ByFullName[index];
      }
    }

    if( system & VDF_VIRTUAL ) {
      uint index = Virtual.Files_ByFullName.IndexOf<File::Sortion_ByFullName>( &tmpFile );
      if( index != -1 ) {
        system = VDF_VIRTUAL;
        // Virtual.Files_ByFullName[index]->Acquire();
        return Virtual.Files_ByFullName[index];
      }
    }

    if( !physicalFirst && system & VDF_PHYSICAL ) {
      uint index = Physical.Files_ByFullName.IndexOf<File::Sortion_ByFullName>( &tmpFile );
      if( index != -1 ) {
        system = VDF_PHYSICAL;
        // Physical.Files_ByFullName[index]->Acquire();
        return Physical.Files_ByFullName[index];
      }
    }

    system = 0;
    return nullptr;
  }


  inline const VDFS::File* VDFS::GetFile( const StringANSI& name, IN OUT int& system ) const {
    if( name.Contains( "\\" ) )
      return GetFile_ByFullName( name , system );

    File tmpFile;
    tmpFile.Name = name;
    tmpFile.Name.MakeUpper();
    bool physicalFirst = (system & VDF_PHYSICALFIRST) == VDF_PHYSICALFIRST;

    if( physicalFirst ) {
      uint index = Physical.Files_ByName.IndexOf<File::Sortion_ByName>( &tmpFile );
      if( index != -1 ) {
        system = VDF_PHYSICAL;
        // Physical.Files_ByName[index]->Acquire();
        return Physical.Files_ByName[index];
      }
    }

    if( system & VDF_VIRTUAL ) {
      uint index = Virtual.Files_ByName.IndexOf<File::Sortion_ByName>( &tmpFile );
      if( index != -1 ) {
        system = VDF_VIRTUAL;
        // Virtual.Files_ByName[index]->Acquire();
        return Virtual.Files_ByName[index];
      }
    }

    if( !physicalFirst && system & VDF_PHYSICAL ) {
      uint index = Physical.Files_ByName.IndexOf<File::Sortion_ByName>( &tmpFile );
      if( index != -1 ) {
        system = VDF_PHYSICAL;
        // Physical.Files_ByName[index]->Acquire();
        return Physical.Files_ByName[index];
      }
    }

    system = 0;
    return nullptr;
  }


  inline Array<const VDFS::File*> VDFS::GetFileVariants_ByFullName( const StringANSI& name, IN OUT int& systems ) const {
    Array<const VDFS::File*> fileList;

    if( systems & VDF_PHYSICAL ) {
      uint index = Physical.Files_ByFullName.IndexOf<File::Sortion_ByFullNameStringed>( name );
      if( index != -1 ) {
        auto file = Physical.Files_ByFullName[index];
        fileList.Insert( file );
      }
    }

    if( systems & VDF_VIRTUAL ) {
      for( auto&& v : Volumes ) {
        uint index = v->Files_ByFullName.IndexOf<File::Sortion_ByFullNameStringed>( name );
        if( index != -1 ) {
          auto file = v->Files_ByFullName[index];
          fileList.Insert( file );
        }
      }
    }

    return fileList.Share();
  }


  inline Array<const VDFS::File*> VDFS::GetFileVariants( const StringANSI& name, IN int systems ) const {
    if( name.Contains( "\\" ) )
      return GetFileVariants_ByFullName( name, systems ).Share();

    Array<const VDFS::File*> fileList;
    if( systems & VDF_PHYSICAL ) {
      uint index = Physical.Files_ByName.IndexOf<File::Sortion_ByNameStringed>( name );
      if( index != -1 ) {
        auto file = Physical.Files_ByName[index];
        fileList.Insert( file );
      }
    }

    if( systems & VDF_VIRTUAL ) {
      for( auto&& v : Volumes ) {
        uint index = v->Files_ByName.IndexOf<File::Sortion_ByNameStringed>( name );
        if( index != -1 ) {
          auto file = v->Files_ByName[index];
          fileList.Insert( file );
        }
      }
    }

    return fileList.Share();
  }


  inline const Array<const VDFS::File*>& VDFS::GetVirtualFileList() const {
    return (Array<const VDFS::File*>&)Virtual.Files_ByFullName;
  }


  inline const Array<const VDFS::File*>& VDFS::GetPhysicalFileList() const {
    return (Array<const VDFS::File*>&)Physical.Files_ByFullName;
  }


  inline const Array<const VDFS::Volume*>& VDFS::GetVolumes() const {
    return (Array<const VDFS::Volume*>&)Volumes;
  }


  inline StringANSI VDFS::GetInitialDirectory() const {
    return InitialDirectory;
  }


  inline void VDFS::LoadDirectoryVDF( const StringANSI& rootDirectory ) {
    if( !rootDirectory.IsExistsAsDirectory() )
      return;

    WIN32_FIND_DATAA findData;
    HANDLE findHandle = ::FindFirstFileA( rootDirectory + "\\*.*", &findData );
    if( findHandle != INVALID_HANDLE_VALUE ) {
      do {
        StringANSI fileName = StringANSI( findData.cFileName ).MakeUpper();
        if( fileName.EndsWith( ".VDF" ) || fileName.EndsWith( ".MOD" ) )
          LoadVolume( StringANSI::Format( "{0}\\{1}", rootDirectory, fileName ) );
      }
      while( ::FindNextFileA( findHandle, &findData ) );
      FindClose( findHandle );
    }
  }


  inline void VDFS::LoadDirectoryFiles( const StringANSI& rootDirectory, const StringANSI& localDirectory ) {
    if( !StringANSI::Format( "{0}{1}", rootDirectory, localDirectory ).IsExistsAsDirectory() )
      return;

    WIN32_FIND_DATAA findData;
    StringANSI searchFilter = StringANSI::Format( "{0}{1}*.*", rootDirectory, localDirectory );
    HANDLE findHandle = ::FindFirstFileA( searchFilter, &findData );
    if( findHandle != INVALID_HANDLE_VALUE ) {
      do {
        StringANSI fileName = StringANSI( findData.cFileName ).MakeUpper();
        if( fileName == "." || fileName == ".." )
          continue;

        if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
          LoadDirectoryFiles( rootDirectory, StringANSI::Format( "{0}{1}\\", localDirectory, fileName ) );
        }
        else {
          File* file = new File();
          file->FullNameVirtual = (localDirectory + fileName).GetCopy().MakeUpper();
          file->FullName = InitialDirectory + file->FullNameVirtual;
          file->Name = fileName.GetCopy().MakeUpper();
          
          Physical.Files_ByFullName.Insert<File::Sortion_ByFullName>( file );
          size_t index = Physical.Files_ByName.IndexOf<File::Sortion_ByName>( file );
          if( index != -1 ) {
            Physical.Files_ByName[index]->Release();
            Physical.Files_ByName[index] = file;
            file->Acquire();
          }
          else {
            Physical.Files_ByName.Insert<File::Sortion_ByName>( file );
            file->Acquire();
          }
        }
      } while( ::FindNextFileA( findHandle, &findData ) );
      FindClose( findHandle );
    }
  }


  inline void VDFS::PostLoadDirectoryFiles( const StringANSI& directory ) {
    LoadDirectoryFiles( directory, "" );
  }


  inline void VDFS::PostLoadDirectoryVDF( const StringANSI& directory ) {
    LoadDirectoryVDF( directory );
  }


  inline void VDFS::InitDefault() {
    StringANSI localSystemDirectory = "SYSTEM\\";
    StringANSI localSavesDirectory = "SAVES\\";

    // Get current root directory and system directory name
    HMODULE mainModule = GetModuleHandleA( nullptr );
    StringANSI moduleFileName;
    moduleFileName.SetLength( 1024 );
    memset( moduleFileName.ToChar(), 0, 1024 );
    GetModuleFileNameA( mainModule, moduleFileName.ToChar(), moduleFileName.GetLength() );
    StringANSI fileDirectory = moduleFileName.GetCopy().GetDirectory();
    InitialDirectory = fileDirectory.GetCopy().GetDirectory();
    StringANSI::Format( "Start VDFS initialization in {0}", InitialDirectory ).StdPrintLine();

    localSystemDirectory = (fileDirectory - InitialDirectory).MakeUpper();
    StringANSI::Format( "Work System directory: {0}", localSystemDirectory ).StdPrintLine();

    // Get current saves directory from ini name
    StringANSI commandLine = GetCommandLineA();
    uint gameId = commandLine.Search( "-GAME:", String::Flags::IgnoreCase );
    if( gameId != -1 ) {
      int start = gameId + 6;
      StringANSI gameName = commandLine.GetWordSmart( 1, start );
      if( gameName != "GothicGame" )
        localSavesDirectory = StringANSI::Format( "SAVES_{0}\\", gameName );
    }

    StringANSI::Format( "Saves directory: {0}", localSavesDirectory ).StdPrintLine();

    // Load vdf and mod volumes
    LoadDirectoryVDF( InitialDirectory + "DATA\\" );
    LoadDirectoryVDF( InitialDirectory + "DATA\\PATCHES\\" );
    LoadDirectoryVDF( InitialDirectory + "DATA\\PLUGINS\\" );

    // Load physical file list
    LoadDirectoryFiles( InitialDirectory, localSystemDirectory );
    LoadDirectoryFiles( InitialDirectory, localSavesDirectory );
    LoadDirectoryFiles( InitialDirectory, "_WORK\\" );

    StringANSI::Format( "Loaded volumes count:       {0}", Volumes.GetCount() ).StdPrintLine();
    StringANSI::Format( "Total virtual files count:  {0}", Virtual.Files_ByFullName.GetCount() ).StdPrintLine();
    StringANSI::Format( "Total physical files count: {0}", Physical.Files_ByFullName.GetCount() ).StdPrintLine();

    // If current system name is not 'system', virtual names
    // for all files should be replaced to the 'system'.
    if( localSystemDirectory != "SYSTEM\\" ) {
      for( auto file : Physical.Files_ByFullName )
        if( file->FullNameVirtual.StartsWith( localSystemDirectory ) )
          file->FullNameVirtual.Replace( localSystemDirectory, "SYSTEM\\" );

      Physical.Files_ByFullName.QuickSort<File::Sortion_ByFullName>();
    }
  }


  inline VDFS& VDFS::GetDefaultInstance() {
    static VDFS* instance = (VDFS*)CreateSharedSingleton( "VirtualDiskFileSystem", []() -> void* { auto vdfs = new VDFS(); vdfs->InitDefault(); return vdfs; } );
    return *instance;
  }
#pragma endregion


#pragma region virtual_file
  inline VirtualFile::VirtualFile( const char* name, int systems ) {
    Handle = -1;
    System = vdf_fexists( name, systems );
    if( System == 0 )
      return;

    Handle = vdf_fopen( name, systems );
  }


  inline const char* VirtualFile::GetName() const {
    return Name;
  }


  inline bool VirtualFile::IsExists() const {
    return Handle != -1;
  }


  inline int VirtualFile::GetSize() const {
    return vdf_ffilesize( Handle );
  }


  inline int VirtualFile::Read( void* where, int length ) {
    return vdf_fread( Handle, where, length );
  }


  inline int VirtualFile::ReadToEnd( void* where ) {
    SetPosition( 0 );
    return vdf_fread( Handle, where, GetSize() );
  }


  inline int VirtualFile::GetPosition() const {
    return vdf_ftell( Handle );
  }


  inline void VirtualFile::SetPosition( int position, int origin ) {
    switch( origin ) {
    case SEEK_SET:
      vdf_fseek( Handle, position );
      break;
    case SEEK_CUR:
      vdf_fseek( Handle, GetPosition() + position );
      break;
    case SEEK_END:
      vdf_fseek( Handle, GetSize() );
      break;
    }
  }


  inline int VirtualFile::GetSysetm() const {
    return System;
  }


  inline int VirtualFile::GetHandle() const {
    return Handle;
  }


  inline void VirtualFile::Close() {
    if( Handle != -1 ) {
      vdf_fclose( Handle );
      Handle = -1;
    }
  }


  inline VirtualFile::~VirtualFile() {
    Close();
  }
#pragma endregion
#endif // !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
}
#pragma warning(pop)

#define __UNION_CLASS_VDFS__
#endif // __UNION_VDFS_H__
#pragma once
#ifndef __UNION_UPDATER_H__
#define __UNION_UPDATER_H__

#include <Windows.h>
#include <Wininet.h>
#include <fstream>
#pragma comment(lib,"wininet.lib")
#include "Array.h"
#include "String.h"
#include "zlib.h"

namespace Union {
  class Updater {
  protected:
    Updater() { }
  public:
    static StringANSI HttpRequest( const StringANSI& url, const StringANSI& host );
    int DownloadPackage( const StringANSI& url, Stream& outStream, void(*callback)(int, int) );
    bool ExtractPackage( const StringANSI& packageFileName, const StringANSI& outputDirectory, Array<StringANSI>* extractedFileList );
  };


  class UpdaterGitHub : public Updater {
    StringANSI GitOwner;
    StringANSI RepoName;
  public:
    UpdaterGitHub( const StringANSI& gitOwner, const StringANSI& repoName );
    StringANSI GetLatestVersion();
    bool QueryLatestVersion( StringANSI& downloadUrl, StringANSI& version, StringANSI& releaseId, StringANSI& fileName );
    int DownloadRelease( const StringANSI& version, const StringANSI& inFileName, Stream& outStream, void(*callback)(int, int) = nullptr );
    int DownloadRelease( const StringANSI& version, const StringANSI& inFileName, const StringANSI& outFileName, void(*callback)(int, int) = nullptr );
    int DownloadLatestRelease( Stream& outStream, void(*callback)(int, int) = nullptr );
    int DownloadLatestRelease( const StringANSI& outFileName, void(*callback)(int, int) = nullptr );
  };


  class UpdaterGitLab : public Updater {
  public:
  };


#pragma region updater
  inline StringANSI Updater::HttpRequest( const StringANSI& url, const StringANSI& host ) {
    const DWORD flags = INTERNET_FLAG_SECURE | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE;
    HINTERNET inet = InternetOpenA( "WinINet", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0 );
    HINTERNET icon = InternetConnectA( inet, host, INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0 );
    HINTERNET ireq = HttpOpenRequestA( icon, "GET", url, nullptr , nullptr, nullptr, flags, 0 );
    HttpSendRequestA( ireq, nullptr, 0, nullptr, 0 );
    char buffer[1024] = { 0 };
    DWORD readed = 0;
    StringANSI result;
    while( InternetReadFile( ireq, buffer, sizeof( buffer ) - 1, &readed ) && readed ) {
      buffer[readed] = 0;
      result += buffer;
    }
    InternetCloseHandle( ireq );
    InternetCloseHandle( icon );
    InternetCloseHandle( inet );
    return result;
  }


  inline int Updater::DownloadPackage( const StringANSI& url, Stream& outStream, void(*callback)(int, int) ) {
    HINTERNET inet = InternetOpenA( "WinINet", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
    if( !inet )
      return -1;

    HINTERNET iurl = InternetOpenUrlA( inet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
    if( !iurl ) {
      InternetCloseHandle( inet );
      return -1;
    }

    DWORD sizeToDownload = 0;
    DWORD sizeToDownloadSz = sizeof( sizeToDownload );
    if( !HttpQueryInfoA( iurl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &sizeToDownload, &sizeToDownloadSz, NULL ) ) {
      InternetCloseHandle( iurl );
      InternetCloseHandle( inet );
      return -1;
    }

    const DWORD bufferSize = 4096;
    DWORD written = 0;
    DWORD readed = 0;
    DWORD readedTotal = 0;
    char buffer[bufferSize];
    while( InternetReadFile( iurl, buffer, bufferSize, &readed ) && readed > 0 ) {
      if( !outStream.Write( buffer, readed ) ) {
        InternetCloseHandle( iurl );
        InternetCloseHandle( inet );
        return -1;
      }

      readedTotal += readed;
      if( callback )
        callback( readedTotal, sizeToDownload );
    }

    InternetCloseHandle( iurl );
    InternetCloseHandle( inet );
    return readedTotal;
  }


  inline bool Updater::ExtractPackage( const StringANSI& packageFileName, const StringANSI& outputDirectory, Array<StringANSI>* extractedFileList ) {
    FileReader reader( packageFileName );
    if( !reader.IsOpened() ) {
      StringANSI::Format( "Cannot open archive: {0}", packageFileName ).StdPrintLine();
      return false;
    }

#pragma pack(push, 1)
    struct {
      byte signature[4];
      ushort version;
      ushort flags;
      ushort method;
      uint timestamp;
      uint crc32;
      uint compressedSize;
      uint uncompressedSize;
      ushort nameSize;
      ushort extraFieldsSize;
    } header;
#pragma pack(pop)

    StringANSI fileName;
    char* uncompressedData;
    char* compressedData;

    while( true ) {
      if( reader.Read( &header, sizeof( header ) ) < sizeof( header ) )
        break;

      // Central Directory - stop extraction
      if( header.signature[0] != 'P' || header.signature[1] != 'K' || header.signature[2] != 1 || header.signature[3] != 2 )
        break;

      // Normal file signature
      if( header.signature[0] != 'P' || header.signature[1] != 'K' || header.signature[2] != 3 || header.signature[3] != 4 ) {
        StringANSI::FormatRaw( "Unsupported zip format. Signature: %c %c %B %B",
          header.signature[0], header.signature[1], header.signature[2], header.signature[3] ).StdPrintLine();
        return false;
      }

      fileName.SetLength( header.nameSize );
      reader.Read( fileName.ToChar(), header.nameSize );

      // Skip pure directory
      if( fileName.Last() == '/' || fileName.Last() == '\\' )
        continue;

      // Skip extra fields data
      reader.SetPosition( header.extraFieldsSize, SEEK_CUR );

      compressedData = new char[header.compressedSize];
      reader.Read( compressedData, header.compressedSize );

      if( header.method > 0 ) {
        uncompressedData = new char[header.uncompressedSize];
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        stream.avail_in = header.compressedSize;
        stream.next_in = reinterpret_cast<Bytef*>(compressedData);
        stream.avail_out = header.uncompressedSize;
        stream.next_out = reinterpret_cast<Bytef*>(uncompressedData);

        if( inflateInit2( &stream, -MAX_WBITS ) != Z_OK ) {
          StringANSI( "Failed to initialize zlib." ).StdPrintLine();
          return false;
        }

        int status = inflate( &stream, Z_FINISH );
        inflateEnd( &stream );
        if( status != Z_STREAM_END ) {
          StringANSI::Format( "Failed to decompress file: {0}", fileName ).StdPrintLine();
          return false;
        }
      }
      else {
        uncompressedData = compressedData;
        compressedData = nullptr;
      }

      StringANSI outputFileName = StringANSI::Format( "{0}\\{1}", outputDirectory, fileName ).Replace( "/", "\\" );
      outputFileName.GetDirectory().CreateDirectory();

      FileWriter writer( outputFileName );
      if( !writer.IsOpened() ) {
        StringANSI::Format( "Cannot open file to write: {0}", outputFileName ).StdPrintLine();
        delete[] uncompressedData;
        if( compressedData )
          delete[] compressedData;
        return false;
      }

      writer.Write( uncompressedData, header.uncompressedSize );
      delete[] uncompressedData;
      if( compressedData )
        delete[] compressedData;

      if( extractedFileList )
        extractedFileList->Insert( outputFileName );
    }

    return true;
  }
#pragma endregion


#pragma region updater_github
  inline UpdaterGitHub::UpdaterGitHub( const StringANSI& gitOwner, const StringANSI& repoName ) {
    GitOwner = gitOwner;
    RepoName = repoName;
  }


  inline StringANSI UpdaterGitHub::GetLatestVersion() {
    StringANSI url = StringANSI::Format( "/repos/{0}/{1}/{2}", GitOwner, RepoName, "releases/latest" );
    StringANSI response = HttpRequest( url, "api.github.com" );
    if( response.IsEmpty() )
      return response;

    return response.GetWordBetween( "/tag/", "\"," );
  }


  inline bool UpdaterGitHub::QueryLatestVersion( StringANSI& downloadUrl, StringANSI& version, StringANSI& releaseId, StringANSI& fileName ) {
    StringANSI url = StringANSI::Format( "/repos/{0}/{1}/{2}", GitOwner, RepoName, "releases/latest" );
    StringANSI response = HttpRequest( url, "api.github.com" );
    if( response.IsEmpty() )
      return false;

    downloadUrl = response.GetWordBetween( "\"browser_download_url\":\"", "\"}]" );
    version = response.GetWordBetween( "/tag/", "\"," );
    releaseId = response.GetWordBetween( "/releases/", "\"," );
    fileName = downloadUrl.GetFileName();
    return true;
  }


  inline int UpdaterGitHub::DownloadRelease( const StringANSI& version, const StringANSI& inFileName, Stream& outStream, void(*callback)(int, int) ) {
    StringANSI downloadUrl = StringANSI::Format( "https://github.com/{0}/{1}/releases/download/{2}/{3}", GitOwner, RepoName, version, inFileName );
    return DownloadPackage( downloadUrl, outStream, callback );
  }


  inline int UpdaterGitHub::DownloadRelease( const StringANSI& version, const StringANSI& inFileName, const StringANSI& outFileName, void(*callback)(int, int) ) {
    outFileName.GetDirectory().CreateDirectory();
    FileWriter writer( outFileName );
    if( !writer.IsOpened() )
      return -1;

    return DownloadRelease( version, inFileName, writer, callback );
  }


  inline int UpdaterGitHub::DownloadLatestRelease( const StringANSI& outFileName, void(*callback)(int, int) ) {
    StringANSI url;
    StringANSI version;
    StringANSI releaseId;
    StringANSI fileName;
    QueryLatestVersion( url, version, releaseId, fileName );
    return DownloadRelease( version, fileName, outFileName, callback );
  }


  inline int UpdaterGitHub::DownloadLatestRelease( Stream& outStream, void(*callback)(int, int) ) {
    StringANSI url;
    StringANSI version;
    StringANSI releaseId;
    StringANSI fileName;
    QueryLatestVersion( url, version, releaseId, fileName );
    return DownloadRelease( version, fileName, outStream, callback );
  }
#pragma endregion
}

#endif // __UNION_UPDATER_H__
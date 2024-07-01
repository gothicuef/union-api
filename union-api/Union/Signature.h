#pragma once
#ifndef __UNION_SIGNATURE_H__
#define __UNION_SIGNATURE_H__

#include "Memory.h"
#include "Array.h"
#include "VDFS.h"
#include "Regex.h"
#include "Dll.h"

#pragma warning(push)
#pragma warning(disable:4018)

namespace Union {
#define SIGNATURE_OF(what) Signature(what, #what)

  class UNION_API Signature {
    friend class SignatureFile;
    StringANSI Address;
    StringANSI TypeName;
    StringANSI CallingConventionName;
    StringANSI ClassName;
    StringANSI FunctionName;
    bool Constancy;
    Array<StringANSI> ArgumentsTypeNames;

    Signature() { }
  public:

    template<typename T>
    Signature( T detailsPtr, const StringANSI& baseStorageClass );
    bool IsMethod() const;
    bool IsSimilarWith( Signature* sig, bool checkNames = true ) const;
    bool IsCompatibleWith( Signature* sig ) const;
    bool TryCastToMethod();
    void* GetAddress() const;
    Signature* Clone() const;
    StringANSI ToString( bool withAddress = false ) const;
  };


  class SignatureFile {
    StringANSI FileName;
    uint ResourceID;
    Array<Signature*> Signatures;

    void ReadFromFile( const StringANSI& fileName );
    void ReadFromResource( int ID, const char* type );
  public:

    Signature* FindSimilarSignature( Signature* sig ) const;
    static SignatureFile* GetFromFile( const StringANSI& fileName );
    static SignatureFile* GetFromResource( int ID, const char* type );
    static Array<SignatureFile*> GetLocalSignatureFileList();
    static Array<SignatureFile*> GetGlobalSignatureFileList();
    static Signature* FindSimilarSignatureInThisHookspace( Signature* sig );

  protected:
    friend class HookSpace;
    static SignatureFile*& GetCurrentSignatureFile();
    static SignatureFile* SwitchCurrentSignatureFile( const StringANSI& fileName );
    static SignatureFile* SwitchCurrentSignatureFile( int ID, const char* type );
  };



  template<typename T>
  inline Signature::Signature( T detailsPtr, const StringANSI& baseStorageClass ) : Constancy(false) {
    static StringANSI patternRemoveKeywords    = "\\b(enum)|(struct)|(class)\\s";
    static StringANSI patternRemoveExtraSpaces = "\\s(>|&|\\*)";
    static StringANSI patternParseSignature    = "(.+) \\((\\w+)(( (.+)::\\*)|(()\\*))\\)\\((.+)\\)(.*)";
    static StringANSI patternParseStorageClass = "&*((.+)::)*(.+)";
    static StringANSI patternRemoveNamespaces = "Gothic_I\\w+\\:\\:";
    static StringANSI inputTypeID              = typeid(T).name();

    {
      inputTypeID = RegexANSI( inputTypeID, patternRemoveKeywords ).Replace( StringANSI::GetEmpty() );
      inputTypeID = RegexANSI( inputTypeID, patternRemoveNamespaces ).Replace( StringANSI::GetEmpty() );
      inputTypeID = RegexANSI( inputTypeID, patternRemoveExtraSpaces ).Replace( "$1" );

      RegexANSI regex( inputTypeID, patternParseSignature );
      if( regex.Next() ) {
        TypeName              =  regex[1];
        CallingConventionName =  regex[2];
        ClassName             =  regex[5];
        Constancy             = !regex[9].IsEmpty();
      
        auto augumentsList = regex[8].Split( "," );
        for( auto&& it : augumentsList )
          if( it.Trim() != "void" )
            ArgumentsTypeNames.Insert( it );
      }
    }

    {
      RegexANSI regex( baseStorageClass, patternParseStorageClass );
      if( regex.Next() ) {
        FunctionName = regex[3];
        if( regex[2] != ClassName )
          String::Format( "Warning: class '{0}' != '{1}' in the signature", regex[2], ClassName ).StdPrintLine();
      }
    }
  }


  inline bool Signature::IsMethod() const {
    return !ClassName.IsEmpty();
  }


  inline bool Signature::IsSimilarWith( Signature* sig, bool checkNames ) const {
    if( TypeName != sig->TypeName )
      return false;

    if( CallingConventionName != sig->CallingConventionName )
      return false;

    if( ClassName != sig->ClassName )
      return false;

    if( checkNames && FunctionName != sig->FunctionName )
      return false;

    if( ArgumentsTypeNames.GetCount() != sig->ArgumentsTypeNames.GetCount() )
      return false;

    for( auto i = 0; i < ArgumentsTypeNames.GetCount(); i++ )
      if( ArgumentsTypeNames[i] != sig->ArgumentsTypeNames[i] )
        return false;

    return true;
  }


  inline bool Signature::IsCompatibleWith( Signature* sig ) const {
    if( IsSimilarWith( sig, false ) )
      return true;

    Signature clone1( *this );
    Signature clone2( *sig );
    clone1.TryCastToMethod();
    clone2.TryCastToMethod();

    if( clone1.TypeName != clone2.TypeName )
      return false;

    if( clone1.CallingConventionName != clone2.CallingConventionName )
      return false;

    if( clone1.ClassName != clone2.ClassName )
      return false;

    if( clone1.ArgumentsTypeNames.GetCount() != clone2.ArgumentsTypeNames.GetCount() )
      return false;

    for( auto i = 0; i < ArgumentsTypeNames.GetCount(); i++ )
      if( clone1.ArgumentsTypeNames[i] != clone2.ArgumentsTypeNames[i] )
        return false;

    return true;
  }


  inline bool Signature::TryCastToMethod() {
    if( !ClassName.IsEmpty() )
      return true;

    if( CallingConventionName != "__fastcall" )
      return false;

    if( ArgumentsTypeNames.GetCount() < 2 )
      return false;

    if( ArgumentsTypeNames[0].Last() != '*' )
      return false;

    if( ArgumentsTypeNames[1] != "void*" )
      return false;

    CallingConventionName = "__thiscall";
    ClassName = ArgumentsTypeNames[0].TrimRight( "*" );
    ArgumentsTypeNames.RemoveFirst(); // Remove hidden this argument
    ArgumentsTypeNames.RemoveFirst(); // Remove edx argument
    return true;
  }


  inline void* Signature::GetAddress() const {
    return Address.ConvertToVoidPtr();
  }


  inline Signature* Signature::Clone() const {
    return new Signature( *this );
  }


  inline StringANSI Signature::ToString( bool withAddress ) const {
    StringANSI result;
    if( withAddress )
      result.Insert( StringANSI::Format( "{0} ", Address ) );

    result.Insert( StringANSI::Format( "{0} {1} ", TypeName, CallingConventionName ));

    if( !ClassName.IsEmpty() )
      result.Insert( StringANSI::Format( "{0}::", ClassName ) );

    result.Insert( StringANSI::Format( "{0}(", FunctionName ) );
    
    StringANSI arguments;
    for( auto&& arg : ArgumentsTypeNames ) {
      if( !arguments.IsEmpty() )
        arguments.Insert( ", " );
      arguments.Insert( arg );
    }

    result.Insert( StringANSI::Format( "{0})", arguments ));
    if( Constancy )
      result.Insert( " const" );

    return result;
  }


  inline void SignatureFile::ReadFromFile( const StringANSI& fileName ) {
    FileName = fileName;

    StringANSI fileData;
    if( !fileData.ReadFromVDFS( fileName ) )
      return;

    fileData.WalkThroughLines( [this]( const StringANSI& line ) {
      auto tokens = line.Split( '\t', false );
      auto sig = Signatures.Create() = new Signature();

      sig->Address               = tokens[0];
      sig->TypeName              = tokens[1];
      sig->CallingConventionName = tokens[2];
      sig->ClassName             = tokens[3];
      sig->FunctionName          = tokens[4];
      for( auto i = 5; i < tokens.GetCount(); i++ )
        sig->ArgumentsTypeNames.Insert( tokens[i] );

      } );
  }


  inline void SignatureFile::ReadFromResource( int ID, const char* type ) {
    ResourceID = ID;

    HANDLE module = Dll::FindNearestModule();
    if( !module )
      return;

    Dll* dll = Dll::Find( module );
    if( !dll )
      return;

    HANDLE resource = dll->GetResource( MAKEINTRESOURCEA( ID ), type );
    if( !resource )
      return;

    const char* resourceData = (const char*)dll->GetResourceData( resource );
    if( !resourceData )
      return;

    size_t resourceDataLength = dll->GetResourceLength( resource );
    StringANSI fileData( resourceData, resourceDataLength );
    dll->FreeResourceData( resourceData );

    fileData.WalkThroughLines( [this]( const StringANSI& line ) {
      auto tokens = line.Split( '\t', false );
      auto sig = Signatures.Create() = new Signature();

      sig->Address               = tokens[0].Trim();
      sig->TypeName              = tokens[1].Trim();
      sig->CallingConventionName = tokens[2].Trim();
      sig->ClassName             = tokens[3].Trim();
      sig->FunctionName          = tokens[4].Trim();
      for( auto i = 5; i < tokens.GetCount(); i++ )
        sig->ArgumentsTypeNames.Insert( tokens[i] );

      } );
  }


  inline Signature* SignatureFile::FindSimilarSignature( Signature* left ) const {
    for( auto&& right : Signatures )
      if( right->IsSimilarWith( left ) )
        return right;
    
    return null;
  }


  inline SignatureFile* SignatureFile::GetFromFile( const StringANSI& fileName ) {
    auto list = GetGlobalSignatureFileList();
    for( auto&& it : list )
      if( it->FileName == fileName )
        return it;

    auto sigFile = list.Create() = new SignatureFile();
    sigFile->ReadFromFile( fileName );
    return sigFile;
  }


  inline SignatureFile* SignatureFile::GetFromResource( int ID, const char* type ) {
    auto list = GetLocalSignatureFileList();
    for( auto&& it : list )
      if( it->ResourceID == ID )
        return it;

    auto sigFile = list.Create() = new SignatureFile();
    sigFile->ReadFromResource( ID, type );
    return sigFile;
  }


  inline Array<SignatureFile*> SignatureFile::GetLocalSignatureFileList() {
    static Array<SignatureFile*> list;
    return list.Share();
  }


  inline Array<SignatureFile*> SignatureFile::GetGlobalSignatureFileList() {
    static Array<SignatureFile*>* list =
      (Array<SignatureFile*>*)CreateSharedSingleton( "SignatureFileList", []() -> void* { return new Array<SignatureFile*>(); } );
    return list->Share();
  }


  inline Signature* SignatureFile::FindSimilarSignatureInThisHookspace( Signature* sig ) {
    auto signatureFile = GetCurrentSignatureFile();
    if( !signatureFile )
      return nullptr;

    return signatureFile->FindSimilarSignature( sig );
  }


  inline SignatureFile*& SignatureFile::GetCurrentSignatureFile() {
    static SignatureFile* instance = null;
    return instance;
  }


  inline SignatureFile* SignatureFile::SwitchCurrentSignatureFile( const StringANSI& fileName ) {
    auto signatureFile = GetFromFile( fileName );
    GetCurrentSignatureFile() = signatureFile;
    return signatureFile;
  }


  inline SignatureFile* SignatureFile::SwitchCurrentSignatureFile( int ID, const char* type ) {
    auto signatureFile = GetFromResource( ID, type );
    GetCurrentSignatureFile() = signatureFile;
    return signatureFile;
  }




#if 0
  /**
   * The class stores the information about the function in the signature context
   */
  class UNION_API Signature {
    friend class SignatureFile;
    StringANSI Address;
    StringANSI Type;
    StringANSI CallingConvention;
    StringANSI ClassName;
    StringANSI FunctionName;
    Array<StringANSI> ArgumentsTypes;

    static void ClearTypeName( StringANSI& typeName );
  public:
    Signature();
    /**
     * @brief Creates signature using a pattern
     * @param detailsPtr The base funciton for which the signature will be created
     * @param baseStorageClass Defines the final storage class of the detailed function
     */
    template<typename T>
    Signature( T detailsPtr, const StringANSI& baseStorageClass );
    /**
     * @brief Returns address of the associated with this signature function
     * @return Address of the associated function
     */
    void* GetAddress() const;
    /**
     * @brief Returns readable instance of this signature
     * @return ANSI String
     */
    StringANSI ToString() const;
    /**
     * @brief Returns found compatible signature from the specified file
     * @param signature Base signature for which to search
     * @param fileName In which file to search
     * @return The pointer to compatible signature or nullptr if not found
     */
    static Signature* GetCompatibleSignature( Signature* signature, const String& fileName );
    /**
     * @brief Returns the sortion direction based on the comparison of the two signature pointers
     * @param l The left signature
     * @param r The right signature
     * @return The sortion direction
     */
    static int SortionFunc( Signature* const& l, Signature* const& r );
  };


  /**
   * The class containts list of the functions information in the signature context
   */
  class UNION_API SignatureFile {
    String FileName;
    Array<Signature*> Signatures;

    bool LoadFromFile( const String& fileName );
    static Array<SignatureFile*>& GetSignatureFiles();
  public:
    /**
     * @brief Returns the file of signatures
     * @param fileName Name of the file
     * @return The file of signatures
     */
    static const Array<Signature*>& GetFileSignatures( const String& fileName );
  };


#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
  inline int Signature::SortionFunc( Signature* const& l, Signature* const& r ) {
    // int diff = l->Type.GetDifference( r->Type );
    // if( diff != 0 )
    //   return diff;

    diff = l->ClassName.GetDifference( r->ClassName );
    if( diff != 0 )
      return diff;

    diff = l->FunctionName.GetDifference( r->FunctionName );
    if( diff != 0 )
      return diff;

    diff = l->ArgumentsTypes.GetCount() - r->ArgumentsTypes.GetCount();
    if( diff != 0 )
      return diff;

    for( uint i = 0; i < l->ArgumentsTypes.GetCount(); i++ ) {
      diff = l->ArgumentsTypes[i].GetDifference( r->ArgumentsTypes[i] );
      if( diff != 0 )
        return diff;
    }
   
    return 0;
  }


  inline Signature* Signature::GetCompatibleSignature( Signature* signature, const String& fileName ) {
    Array<Signature*> signatures = SignatureFile::GetFileSignatures( fileName );
    uint index = signatures.IndexOf<Signature::SortionFunc>( signature );
    if( index == -1 )
      return nullptr;

    Signature* found = signatures[index];
    if( found->CallingConvention != signature->CallingConvention ) {
      StringANSI::Format( "Incompatible calling conventions.\n  Engine function: %s\n  Plugin function: %s",
        found->ToString(), signature->ToString() ).StdPrintLine();
      return nullptr;
    }

    return found;
  }


  inline void Signature::ClearTypeName( StringANSI& typeName ) {
    typeName.Replace( " &", "& " );
    typeName.Replace( " *", "* " );
    typeName.Replace( "class ", "" );
    typeName.Replace( "struct ", "" );
    typeName.Replace( "enum \\w+", "enum" );
    typeName.RegexReplace( "\\w+\\:\\:", "" );
    typeName.Trim( " \t()" );
  }


  template<typename T>
  inline Signature::Signature( T detailsPtr, const StringANSI& baseStorageClass ) {
    StringANSI typeId = typeid(T).name();
    Type = typeId.RegexSearch( "^.+?\\(" );
    ClearTypeName( Type );
    StringANSI body = typeId.RegexSearch( "\\(__.+?\\*\\)" );
    CallingConvention = body.RegexSearch( "__\\w+" );
    ClassName = body.RegexSearch( "\\w+\\:\\:\\*" );
    ClassName.Trim( ":*" );
    StringANSI arguments = typeId.RegexSearch( "\\)\\(.+?\\)" );
    ArgumentsTypes = arguments.Split( ',' );
    for( auto& argument : ArgumentsTypes )
      ClearTypeName( argument );

    FunctionName = baseStorageClass.RegexSearch( "\\w+$" );
    FunctionName.Trim( " &:" );

    StringANSI className = baseStorageClass.RegexSearch( "\\w+\\:\\:" );
    className.Trim( " &:" );
    
    if( ClassName.IsEmpty() && !className.IsEmpty() ) {
      ClassName = className;

      // Check ECX argument
      if( ArgumentsTypes.GetCount() > 0 && ArgumentsTypes[0] == StringANSI::Format( "%s*", className ) ) {
        ArgumentsTypes.RemoveAt( 0 );

        // Check __fastcall is __thiscall
        if( ArgumentsTypes.GetCount() > 0 && ArgumentsTypes[0] == "void*" ) {
          if( CallingConvention == "__fastcall" ) {
            CallingConvention = "__thiscall";
            ArgumentsTypes.RemoveAt( 0 );
          }
        }
      }
    }
    
    if( ClassName != className )
      String::Format( "Incompatible classes in the signature: %s, %s", ClassName, className ).StdPrintLine();
  }
  
  
  inline Signature::Signature() {

  }


  void* Signature::GetAddress() const {
    return !Address.IsEmpty() ? Address.ConvertToVoidPtr() : nullptr;
  }


  StringANSI Signature::ToString() const {
    StringANSI out;
    out.Insert( Type ).Insert( " " );
    out.Insert( CallingConvention ).Insert( " " );
    if( !ClassName.IsEmpty() )
      out.Insert( ClassName ).Insert( "::" );
    out.Insert( FunctionName ).Insert( "(" );
    for( uint i = 0; i < ArgumentsTypes.GetCount(); i++ ) {
      const StringANSI& arg = ArgumentsTypes[i];
      if( i > 0 ) out.Insert( ", " );
      out.Insert( arg );
    }
    out.Insert( ")" );
    return out;
  }


  inline Array<SignatureFile*>& SignatureFile::GetSignatureFiles() {
    Array<SignatureFile*>* signatureFiles =
      (Array<SignatureFile*>*)CreateSharedSingleton( "SignatureFiles", []() -> void* { return new Array<SignatureFile*>(); } );
    return *signatureFiles;
  }


  inline bool SignatureFile::LoadFromFile( const String& fileName ) {
    FileName = fileName;
    StringANSI fileData;
    if( !fileData.ReadFromVDFS( fileName ) || fileData.IsEmpty() ) // TODO VDFS
      return false;

    Array<StringANSI> fileRows = fileData.Split( "\r\n" );
    if( fileRows.IsEmpty() )
      return false;

    for( auto& row : fileRows ) {
      Array<StringANSI> cells = row.Split( '\t', false );
      if( cells.GetCount() < 5 )
        continue;

      Signature* signature = new Signature();
      signature->Address = cells[0];
      signature->Type = cells[1];
      signature->CallingConvention = cells[2];
      signature->ClassName = cells[3];
      signature->FunctionName = cells[4];
      for( uint i = 5; i < cells.GetCount(); i++ )
        signature->ArgumentsTypes.Insert( cells[i] );

      Signatures.Insert( signature );
    }

    Signatures.QuickSort<Signature::SortionFunc>();
    return !Signatures.IsEmpty();
  }


  inline const Array<Signature*>& SignatureFile::GetFileSignatures( const String& fileName ) {
    static Array<SignatureFile*>& signatureFiles = GetSignatureFiles();
    for( auto file : signatureFiles )
      if( file->FileName == fileName )
        return file->Signatures;

    SignatureFile* file = new SignatureFile();
    auto ok = file->LoadFromFile( fileName );
    if( !ok )
      String::Format( "Signature file '%s' cannot be loaded", fileName ).StdPrintLine();

    signatureFiles.Insert( file );
    return file->Signatures;
  }
#endif
#endif
}

#pragma warning(pop)
#endif // __UNION_SIGNATURE_H__
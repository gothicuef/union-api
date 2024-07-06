#pragma once
#ifndef __UNION_STRING_BASE_H__
#define __UNION_STRING_BASE_H__
#include <Windows.h>
#include "Types.h"
#include "Memory.h"

#define BOM_UTF8     "\xEF\xBB\xBF"
#define BOM_UTF16_LE "\xFF\xFE"
#define EOL_CR       "\r"
#define EOL_LF       "\n"
#define EOL_CRLF     "\r\n"
#define STR_SPACES   " \t\r\n\f\v"

namespace Union {
  enum class TerminalColorUID {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    YELLOW,
    GRAY,
    LGRAY,
    LBLUE,
    LGREEN,
    LCYAN,
    LRED,
    LPURPLE,
    LYELLOW,
    WHITE,
    MAX
  };


  class StringBase {
  protected:
  public:
    enum Flags {
      Default         = 0,
      IgnoreCase      = 1,
      SearchWordEnd   = 2,
      SearchInReverse = 4,
      SplitOperators  = 8,
      DetectStrings   = 16,
      SkipCppComments = 32,
      SkipAsmComments = 64,
      ReplaceLooped   = 128
    };

    enum Encodings {
      ANSI    = 1, // ANSI
      UTF8    = 2, // UTF-8
      UTF16   = 3  // UTF-16
    };

    enum LineBreaks {
      CR   = 1, // \r
      LF   = 2, // \r
      CRLF = 3  // \r\n
    };

    enum FormatParameters {
      VaInt          = 'i',
      VaInt64        = 'I',
      VaUInt         = 'u',
      VaUInt64       = 'U',
      VaHex          = 'x',
      VaHex64        = 'X',
      VaSingle       = 'f',
      VaDouble       = 'F',
      VaBoolean      = 'b',
      VaChar         = 'c',
      VaUChar        = 'B',
      VaLPSTR        = 't',
      VaLPWSTR       = 'T',
      VaUnionString  = 's',
      VaGothicString = 'z',
      VaLine         = 'r',
      VaPercent      = '%'
    };

    enum WordGroups {
      Strings   = -2,
      Other     = -1,
      Spaces    =  0,
      Operators =  1,
      Digits    =  2,
      Letters   =  3
    };

    virtual bool IsAnsiString() const = 0;
    virtual bool IsUnicodeString() const = 0;

    static void SaveTerminalColorFG() {
      GetSavedTerminalColorFG() = GetTerminalColorFG();
    }

    static void SaveTerminalColorBG() {
      GetSavedTerminalColorBG() = GetTerminalColorBG();
    }

    static void SaveTerminalColors() {
      SaveTerminalColorFG();
      SaveTerminalColorBG();
    }

    static void LoadTerminalColorFG() {
      SetTerminalColorFG( GetSavedTerminalColorFG() );
    }

    static void LoadTerminalColorBG() {
      SetTerminalColorBG( GetSavedTerminalColorBG() );
    }

    static void LoadTerminalColors() {
      LoadTerminalColorFG();
      LoadTerminalColorBG();
    }

    static void SetTerminalColorFG( TerminalColorUID color ) {
      GetTerminalColorFG() = color;
      UpdateTerminalColors();
    }

    static void SetTerminalColorBG( TerminalColorUID color ) {
      GetTerminalColorBG() = color;
      UpdateTerminalColors();
    }

  private:
    static TerminalColorUID& GetTerminalColorFG() {
      static TerminalColorUID* instance =
        (TerminalColorUID*)CreateSharedSingleton( "TerminalColorFG", []() -> void* { return new TerminalColorUID( TerminalColorUID::GRAY ); } );
      return *instance;
    }

    static TerminalColorUID& GetTerminalColorBG() {
      static TerminalColorUID* instance =
        (TerminalColorUID*)CreateSharedSingleton( "TerminalColorBG", []() -> void* { return new TerminalColorUID( TerminalColorUID::BLACK ); } );
      return *instance;
    }

    static TerminalColorUID& GetSavedTerminalColorFG() {
      static TerminalColorUID* instance =
        (TerminalColorUID*)CreateSharedSingleton( "SavedTerminalColorFG", []() -> void* { return new TerminalColorUID( TerminalColorUID::GRAY ); } );
      return *instance;
    }

    static TerminalColorUID& GetSavedTerminalColorBG() {
      static TerminalColorUID* instance =
        (TerminalColorUID*)CreateSharedSingleton( "SavedTerminalColorBG", []() -> void* { return new TerminalColorUID( TerminalColorUID::BLACK ); } );
      return *instance;
    }

    static void UpdateTerminalColors() {
      auto colorFG = GetTerminalColorFG();
      auto colorBG = GetTerminalColorBG();
      HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
      SetConsoleTextAttribute( consoleHandle, (DWORD)colorFG + ((DWORD)colorBG << 4) );

    }
  };

  struct __lpStringT {
    const char* Ansi;
    const wchar* Unicode;
    __lpStringT( const char* ansi, const wchar* unicode ) :
      Ansi( ansi ), Unicode( unicode ) {
    }
    operator const char* () {
      return Ansi;
    }
    operator const wchar* () {
      return Unicode;
    }
  };

#define _lpStrT(s) __lpStringT( s, L##s )

  inline StringBase::Flags operator | ( StringBase::Flags a, StringBase::Flags b ) {
    return (StringBase::Flags)((int)a | (int)b);
  }
}
#endif // __UNION_STRING_BASE_H__
#pragma once
#ifndef __UNION_LOCALE_H__
#define __UNION_LOCALE_H__
#include "Types.h"
#include "Memory.h"
#include <locale>
#include <Windows.h>

namespace Union {
  enum class CODEPAGE {
    CENTRALOREASTERN_EUROPEAN = 1250,
    CZECH                     = 1250,
    HUNGARIAN                 = 1250,
    POLISH                    = 1250,
    ROMANIAN                  = 1250,
    CROATIAN                  = 1250,
    SLOVAK                    = 1250,
    ALBANIAN                  = 1250,
    SLOVENIAN                 = 1250,
    SERBIANLATIN              = 1250,
    BOSNIAN                   = 1250,
    CYRILLIC                  = 1251,
    BULGARIAN                 = 1251,
    RUSSIAN                   = 1251,
    UKRAINIAN                 = 1251,
    BELARUSIAN                = 1251,
    MACEDONIAN                = 1251,
    SERBIAN                   = 1251,
    NORTHORWESTERN_EUROPEAN   = 1252,
    CELTIC                    = 1252,
    ICELANDIC                 = 1252,
    DANISH                    = 1252,
    GERMAN                    = 1252,
    ENGLISH                   = 1252,
    SPANISH                   = 1252,
    FINNISH                   = 1252,
    FRENCH                    = 1252,
    ITALIAN                   = 1252,
    DUTCH                     = 1252,
    NORWEGIAN                 = 1252,
    PORTUGUESE                = 1252,
    SWEDISH                   = 1252,
    AFRIKAANS                 = 1252,
    SWAHILI                   = 1252,
    GALICIAN                  = 1252,
    WELSH                     = 1252,
    BRETON                    = 1252,
    IRISH                     = 1252,
    GREEK                     = 1253,
    TURKISH                   = 1254,
    ESPERANTO                 = 1254,
    KURDISH                   = 1254,
    HEBREW                    = 1255,
    ARABIC                    = 1256,
    BALTIC                    = 1257,
    ESTONIAN                  = 1257,
    LATVIAN                   = 1257,
    LITHUANIAN                = 1257,
    CHINESE                   = 936,
    OTHER                     = ENGLISH,
  };
  
  enum class LANGUAGE {
    UNKNOWN,
    RUSSIAN,     
    ENGLISH,
    GERMAN,
    POLISH,
    ROMANIAN,
    ITALIAN,
    CZECH,
    SPANISH,
    HUNGARIAN,
    CROATIAN,
    SLOVAK,
    ALBANIAN,
    SLOVENIAN,
    BOSNIAN,
    BULGARIAN,
    UKRAINIAN,
    BELARUSIAN,
    MACEDONIAN,
    SERBIAN,
    ICELANDIC,
    DANISH,
    FINNISH,
    FRENCH,
    DUTCH,
    NORWEGIAN,
    PORTUGUESE,
    SWEDISH,
    AFRIKAANS,
    SWAHILI,
    GALICIAN,
    WELSH,
    BRETON,
    IRISH,
    GREEK,
    TURKISH,
    ESPERANTO,
    KURDISH,
    HEBREW,
    ARABIC,
    ESTONIAN,
    LATVIAN,
    LITHUANIAN,
    CHINESE,
    LANGUAGE_MAX,
    OTHER = ENGLISH
  };

  struct UNION_API Locale {
    LCID LanguageCodeID;
    LANGUAGE Language;
    CODEPAGE Codepage;
    char LanguageName[20];

    Locale();
    void operator =( Locale locale );
    void SetLocale( LCID langId );
    void SetLocale( LANGUAGE lang );
    static Locale GetLocale();
    static CODEPAGE GetCodepage( LANGUAGE lang );
    static LANGUAGE GetLanguage( LCID langId );
    static LCID GetLCID( LANGUAGE lang );
    static LANGUAGE GetLanguageFromName( const char* codename );
    static const Locale& GetDefaultLocale();
    static Locale& GetUserLocale();

#ifdef __UNION_STRING_H__
    template<class T>
    static T Localize( const char* format, T start ... ) {
      const Locale& locale = GetUserLocale();
      const T* list = &start;
      Array<LANGUAGE> itemLangs;

      Array<StringANSI> items = StringANSI( format ).Split( " \t%,;" );
      for( uint i = 0; i < items.GetCount(); i++ ) {
        StringANSI& item = items[i].Trim();
        LANGUAGE lang = GetLanguageFromName( item );
        itemLangs.Insert( lang );
      }

      for( uint i = 0; i < itemLangs.GetCount(); i++ )
        if( itemLangs[i] == locale.Language )
          return list[i];

      // Alternative language for CIS
      if( locale.Language == LANGUAGE::UKRAINIAN || locale.Language == LANGUAGE::BELARUSIAN )
        for( uint i = 0; i < itemLangs.GetCount(); i++ )
          if( itemLangs[i] == LANGUAGE::RUSSIAN )
            return list[i];

      // International
      for( uint i = 0; i < itemLangs.GetCount(); i++ )
        if( itemLangs[i] == LANGUAGE::ENGLISH )
          return list[i];

      return list[0];
    }
#endif // __UNION_STRING_H__
  };

#if !defined(_UNION_API_DLL) || defined(_UNION_API_BUILD)
  template<size_t size>
  static void _getLanguageName( char( &buffer )[size], LCID lcid ) {
    GetLocaleInfoA( lcid, LOCALE_SENGLANGUAGE, buffer, sizeof( buffer ) );
  }

  template<size_t size>
  static void _getLanguageName( wchar_t( &buffer )[size], LCID lcid ) {
    GetLocaleInfoW( lcid, LOCALE_SENGLANGUAGE, buffer, sizeof( buffer ) );
  }

  inline Locale::Locale() {
    memset( this, 0, sizeof( Locale ) );
  }

  inline void Locale::operator =( Locale locale ) {
    LanguageCodeID = locale.LanguageCodeID;
    Language = locale.Language;
    Codepage = locale.Codepage;
    memcpy( LanguageName, locale.LanguageName, sizeof( LanguageName ) );
    SetConsoleOutputCP( (uint)Codepage );
  }

  inline Locale Locale::GetLocale() {
    Locale locale;
    locale.LanguageCodeID = GetSystemDefaultLangID();
    locale.Language = GetLanguage( locale.LanguageCodeID );
    locale.Codepage = GetCodepage( locale.Language );
    _getLanguageName( locale.LanguageName, locale.LanguageCodeID );
    return locale;
  }

  inline void Locale::SetLocale( LCID langId ) {
    LanguageCodeID = langId;
    Language = GetLanguage( langId );
    Codepage = GetCodepage( Language );
    _getLanguageName( LanguageName, LanguageCodeID );
  }

  inline void Locale::SetLocale( LANGUAGE lang ) {
    LanguageCodeID = GetLCID( lang );
    Language = lang;
    Codepage = GetCodepage( Language );
    _getLanguageName( LanguageName, LanguageCodeID );
  }

  inline CODEPAGE Locale::GetCodepage( LANGUAGE lang ) {
    switch( lang ) {
    case LANGUAGE::POLISH:
    case LANGUAGE::CZECH:
    case LANGUAGE::ROMANIAN:
    case LANGUAGE::HUNGARIAN:
    case LANGUAGE::CROATIAN:
    case LANGUAGE::SLOVAK:
    case LANGUAGE::ALBANIAN:
    case LANGUAGE::SLOVENIAN:
    case LANGUAGE::BOSNIAN:
      return CODEPAGE::CENTRALOREASTERN_EUROPEAN;
    case LANGUAGE::RUSSIAN:
    case LANGUAGE::BULGARIAN:
    case LANGUAGE::UKRAINIAN:
    case LANGUAGE::BELARUSIAN:
    case LANGUAGE::MACEDONIAN:
    case LANGUAGE::SERBIAN:
      return CODEPAGE::CYRILLIC;
    case LANGUAGE::ENGLISH:
    case LANGUAGE::GERMAN:
    case LANGUAGE::ITALIAN:
    case LANGUAGE::SPANISH:
    case LANGUAGE::ICELANDIC:
    case LANGUAGE::DANISH:
    case LANGUAGE::FINNISH:
    case LANGUAGE::FRENCH:
    case LANGUAGE::DUTCH:
    case LANGUAGE::NORWEGIAN:
    case LANGUAGE::PORTUGUESE:
    case LANGUAGE::SWEDISH:
    case LANGUAGE::AFRIKAANS:
    case LANGUAGE::SWAHILI:
    case LANGUAGE::GALICIAN:
    case LANGUAGE::WELSH:
    case LANGUAGE::BRETON:
    case LANGUAGE::IRISH:
      return CODEPAGE::NORTHORWESTERN_EUROPEAN;
    case LANGUAGE::GREEK:
      return CODEPAGE::GREEK;
    case LANGUAGE::TURKISH:
    case LANGUAGE::ESPERANTO:
    case LANGUAGE::KURDISH:
      return CODEPAGE::TURKISH;
    case LANGUAGE::HEBREW:
      return CODEPAGE::HEBREW;
    case LANGUAGE::ARABIC:
      return CODEPAGE::ARABIC;
    case LANGUAGE::ESTONIAN:
    case LANGUAGE::LATVIAN:
    case LANGUAGE::LITHUANIAN:
      return CODEPAGE::CHINESE;
    case LANGUAGE::CHINESE:
      return CODEPAGE::CHINESE;
    default:
      return CODEPAGE::ENGLISH;
    }
  }

  inline LANGUAGE Locale::GetLanguage( LCID langId ) {
    switch( langId ) {
    case 1049: return LANGUAGE::RUSSIAN;
    case 1033:
    case 2057:
    case 4105:
    case 3081: return LANGUAGE::ENGLISH;
    case 1031:
    case 3079:
    case 5127:
    case 4103: return LANGUAGE::GERMAN;
    case 1045: return LANGUAGE::POLISH;
    case 1048: return LANGUAGE::ROMANIAN;
    case 1040: return LANGUAGE::ITALIAN;
    case 1029: return LANGUAGE::CZECH;
    case 1034: return LANGUAGE::SPANISH;
    case 1038: return LANGUAGE::HUNGARIAN;
    case 1050: return LANGUAGE::CROATIAN;
    case 1051: return LANGUAGE::SLOVAK;
    case 1052: return LANGUAGE::ALBANIAN;
    case 1060: return LANGUAGE::SLOVENIAN;
    case 1026: return LANGUAGE::BULGARIAN;
    case 1058: return LANGUAGE::UKRAINIAN;
    case 1059: return LANGUAGE::BELARUSIAN;
    case 1071: return LANGUAGE::MACEDONIAN;
    case 3098: return LANGUAGE::SERBIAN;
    case 1039: return LANGUAGE::ICELANDIC;
    case 1030: return LANGUAGE::DANISH;
    case 1035: return LANGUAGE::FINNISH;
    case 1036: return LANGUAGE::FRENCH;
    case 1043: return LANGUAGE::DUTCH;
    case 1044: return LANGUAGE::NORWEGIAN;
    case 1046:
    case 2070: return LANGUAGE::PORTUGUESE;
    case 1053: return LANGUAGE::SWEDISH;
    case 1078: return LANGUAGE::AFRIKAANS;
    case 1032: return LANGUAGE::GREEK;
    case 1055: return LANGUAGE::TURKISH;
    case 1037: return LANGUAGE::HEBREW;
    case 1025: return LANGUAGE::ARABIC;
    case 1061: return LANGUAGE::ESTONIAN;
    case 1062: return LANGUAGE::LATVIAN;
    case 1063: return LANGUAGE::LITHUANIAN;
    case 1028:
    case 3076:
    case 2052:
    case 4100: return LANGUAGE::CHINESE;
    default:   return LANGUAGE::OTHER;
    }
  }

  inline LCID Locale::GetLCID( LANGUAGE lang ) {
    switch( lang ) {
    case LANGUAGE::RUSSIAN:    return 1049;
    case LANGUAGE::ENGLISH:    return 3081;
    case LANGUAGE::GERMAN:     return 4103;
    case LANGUAGE::POLISH:     return 1045;
    case LANGUAGE::ROMANIAN:   return 1048;
    case LANGUAGE::ITALIAN:    return 1040;
    case LANGUAGE::CZECH:      return 1029;
    case LANGUAGE::SPANISH:    return 1034;
    case LANGUAGE::HUNGARIAN:  return 1038;
    case LANGUAGE::CROATIAN:   return 1050;
    case LANGUAGE::SLOVAK:     return 1051;
    case LANGUAGE::ALBANIAN:   return 1052;
    case LANGUAGE::SLOVENIAN:  return 1060;
    case LANGUAGE::BULGARIAN:  return 1026;
    case LANGUAGE::UKRAINIAN:  return 1058;
    case LANGUAGE::BELARUSIAN: return 1059;
    case LANGUAGE::MACEDONIAN: return 1071;
    case LANGUAGE::SERBIAN:    return 3098;
    case LANGUAGE::ICELANDIC:  return 1039;
    case LANGUAGE::DANISH:     return 1030;
    case LANGUAGE::FINNISH:    return 1035;
    case LANGUAGE::FRENCH:     return 1036;
    case LANGUAGE::DUTCH:      return 1043;
    case LANGUAGE::NORWEGIAN:  return 1044;
    case LANGUAGE::PORTUGUESE: return 2070;
    case LANGUAGE::SWEDISH:    return 1053;
    case LANGUAGE::AFRIKAANS:  return 1078;
    case LANGUAGE::GREEK:      return 1032;
    case LANGUAGE::TURKISH:    return 1055;
    case LANGUAGE::HEBREW:     return 1037;
    case LANGUAGE::ARABIC:     return 1025;
    case LANGUAGE::ESTONIAN:   return 1061;
    case LANGUAGE::LATVIAN:    return 1062;
    case LANGUAGE::LITHUANIAN: return 1063;
    case LANGUAGE::CHINESE:    return 4100;
    default:                   return 3081;
    }
  }

  inline LANGUAGE Locale::GetLanguageFromName( const char* codename ) {

#define CHECK_LANGUAGE_3(codename, l1, l2, l3, ret) \
    if( _strcmpi( codename, l1 ) == 0 || \
        _strcmpi( codename, l2 ) == 0 || \
        _strcmpi( codename, l3 ) == 0 )  \
        return ret;

#define CHECK_LANGUAGE_4(codename, l1, l2, l3, l4, ret) \
    if( _strcmpi( codename, l1 ) == 0 || \
        _strcmpi( codename, l2 ) == 0 || \
        _strcmpi( codename, l3 ) == 0 || \
        _strcmpi( codename, l4 ) == 0 )  \
        return ret;

    CHECK_LANGUAGE_3( codename, "ru", "rus",        "russian",    LANGUAGE::RUSSIAN );
    CHECK_LANGUAGE_3( codename, "en", "eng",        "english",    LANGUAGE::ENGLISH );
    CHECK_LANGUAGE_4( codename, "de", "ger", "deu", "german",     LANGUAGE::GERMAN );
    CHECK_LANGUAGE_3( codename, "pl", "pol",        "polish",     LANGUAGE::POLISH );
    CHECK_LANGUAGE_4( codename, "ro", "rom", "ron", "romanian",   LANGUAGE::ROMANIAN );
    CHECK_LANGUAGE_3( codename, "it", "ita",        "italian",    LANGUAGE::ITALIAN );
    CHECK_LANGUAGE_4( codename, "cs", "cze", "ces", "czech",      LANGUAGE::CZECH );
    CHECK_LANGUAGE_3( codename, "es", "spa",        "spanish",    LANGUAGE::SPANISH );
    CHECK_LANGUAGE_3( codename, "hu", "hun",        "hungarian",  LANGUAGE::HUNGARIAN );
    CHECK_LANGUAGE_3( codename, "hr", "hrv",        "croatian",   LANGUAGE::CROATIAN );
    CHECK_LANGUAGE_4( codename, "sk", "slo", "slk", "slovak",     LANGUAGE::SLOVAK );
    CHECK_LANGUAGE_4( codename, "sq", "alb", "sqi", "albanian",   LANGUAGE::ALBANIAN );
    CHECK_LANGUAGE_3( codename, "sl", "slv",        "slovenian",  LANGUAGE::SLOVENIAN );
    CHECK_LANGUAGE_3( codename, "bg", "bul",        "bulgarian",  LANGUAGE::BULGARIAN );
    CHECK_LANGUAGE_3( codename, "uk", "ukr",        "ukrainian",  LANGUAGE::UKRAINIAN );
    CHECK_LANGUAGE_3( codename, "be", "bel",        "belarusian", LANGUAGE::BELARUSIAN );
    CHECK_LANGUAGE_4( codename, "mk", "mac", "mkd", "macedonian", LANGUAGE::MACEDONIAN );
    CHECK_LANGUAGE_3( codename, "sr", "srp",        "serbian",    LANGUAGE::SERBIAN );
    CHECK_LANGUAGE_4( codename, "is", "ice", "isl", "icelandic",  LANGUAGE::ICELANDIC );
    CHECK_LANGUAGE_3( codename, "da", "dan",        "danish",     LANGUAGE::DANISH );
    CHECK_LANGUAGE_3( codename, "fi", "fin",        "finnish",    LANGUAGE::FINNISH );
    CHECK_LANGUAGE_4( codename, "fr", "fre", "fra", "french",     LANGUAGE::FRENCH );
    CHECK_LANGUAGE_4( codename, "nl", "dut", "nld", "dutch",      LANGUAGE::DUTCH );
    CHECK_LANGUAGE_3( codename, "no", "nor",        "norwegian",  LANGUAGE::NORWEGIAN );
    CHECK_LANGUAGE_3( codename, "pt", "por",        "portuguese", LANGUAGE::PORTUGUESE );
    CHECK_LANGUAGE_3( codename, "sv", "swe",        "swedish",    LANGUAGE::SWEDISH );
    CHECK_LANGUAGE_3( codename, "af", "afr",        "afrikaans",  LANGUAGE::AFRIKAANS );
    CHECK_LANGUAGE_4( codename, "el", "gre", "ell", "greek",      LANGUAGE::GREEK );
    CHECK_LANGUAGE_3( codename, "tr", "tur",        "turkish",    LANGUAGE::TURKISH );
    CHECK_LANGUAGE_3( codename, "he", "heb",        "hebrew",     LANGUAGE::HEBREW );
    CHECK_LANGUAGE_3( codename, "ar", "ara",        "arabic",     LANGUAGE::ARABIC );
    CHECK_LANGUAGE_3( codename, "et", "est",        "estonian",   LANGUAGE::ESTONIAN );
    CHECK_LANGUAGE_3( codename, "lv", "lav",        "latvian",    LANGUAGE::LATVIAN );
    CHECK_LANGUAGE_3( codename, "lt", "lit",        "lithuanian", LANGUAGE::LITHUANIAN );
    CHECK_LANGUAGE_4( codename, "zh", "chi", "zho", "chinese",    LANGUAGE::CHINESE );

#undef CHECK_LANGUAGE_3
#undef CHECK_LANGUAGE_4

    return LANGUAGE::UNKNOWN;
  }

  inline const Locale& Locale::GetDefaultLocale() {
    static Locale locale = GetLocale();
    return locale;
  }

  inline Locale& Locale::GetUserLocale() {
    static Locale* locale = (Locale*)CreateSharedSingleton( "UserLocale", []() -> void* { return new Locale( GetLocale() ); } );
    return *locale;
  }
#endif
}
#endif // __UNION_LOCALE_H__
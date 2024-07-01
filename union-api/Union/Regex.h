#pragma once
#ifndef __UNION_REGEX_H__
#define __UNION_REGEX_H__

#include "String.h"
#include "Array.h"
#include <string>
#include <regex>

#pragma warning(push)
#pragma warning(disable:4018)

#define xString(T) std::basic_string<T, std::char_traits<T>, std::allocator<T>>
#define xRegex(T) std::basic_regex<T>
#define xRegexIterator(T) std::regex_iterator<xString(T)::const_iterator>


#pragma pack(push, _CRT_PACKING)
namespace Union {
  class UnionRegexA {
    std::regex Regex;
    std::string Input;
    std::sregex_iterator Iterator;
    std::sregex_iterator End;
    Array<StringANSI> Match;
  public:

    UnionRegexA( const StringANSI& input, const StringANSI& pattern );
    bool Next();
    StringANSI Replace( const StringANSI& output );
    const StringANSI operator [] ( int index ) const;
    const StringANSI GetValue() const;
    uint GetLength() const;
    const StringANSI* begin() const;
    const StringANSI* end() const;
  };


  class UnionRegexW {
    std::wregex Regex;
    std::wstring Input;
    std::wsregex_iterator Iterator;
    std::wsregex_iterator End;
    Array<StringUTF16> Match;
  public:

    UnionRegexW( const StringUTF16& input, const StringUTF16& pattern );
    bool Next();
    StringUTF16 Replace( const StringUTF16& output );
    const StringUTF16 operator [] ( int index ) const;
    const StringUTF16 GetValue() const;
    uint GetLength() const;
    const StringUTF16* begin() const;
    const StringUTF16* end() const;
  };


  inline UnionRegexA::UnionRegexA( const StringANSI& input, const StringANSI& pattern )
    : Regex( pattern ), Input( input ), Iterator( Input.begin(), Input.end(), Regex ) { }


  inline bool UnionRegexA::Next() {
    try {
      Match.Clear();
      if( Iterator != End ) {
        std::smatch match = *Iterator;
        for( const auto& m : match )
          Match.Insert( m.str().c_str() );

        ++Iterator;
        return true;
      }
    }
    catch( const std::regex_error& e ) {
      std::cerr << "Regular expression error: " << e.what() << std::endl;
    }

    return false;
  }


  inline StringANSI UnionRegexA::Replace( const StringANSI& output ) {
    auto result = std::regex_replace( Input, Regex, output.ToChar() );
    return result.c_str();
  }


  inline const StringANSI UnionRegexA::operator [] ( int index ) const {
    return index < Match.GetCount() ? Match[index] : StringANSI::GetEmpty();
  }


  inline const StringANSI UnionRegexA::GetValue() const {
    return Match.GetFirst();
  }


  inline uint UnionRegexA::GetLength() const {
    return Match.GetCount();
  }


  inline const StringANSI* UnionRegexA::begin() const {
    return Match.begin();
  }


  inline const StringANSI* UnionRegexA::end() const {
    return Match.end();
  }


  inline UnionRegexW::UnionRegexW( const StringUTF16& input, const StringUTF16& pattern )
    : Regex( pattern ), Input( input ), Iterator( Input.begin(), Input.end(), Regex ) { }


  inline bool UnionRegexW::Next() {
    try {
      Match.Clear();
      if( Iterator != End ) {
        std::wsmatch match = *Iterator;
        for( const auto& m : match )
          Match.Insert( m.str().c_str() );

        ++Iterator;
        return true;
      }
    }
    catch( const std::regex_error& e ) {
      std::cerr << "Regular expression error: " << e.what() << std::endl;
    }

    return false;
  }


  inline StringUTF16 UnionRegexW::Replace( const StringUTF16& output ) {
    auto result = std::regex_replace( Input, Regex, output.ToChar() );
    return result.c_str();
  }


  inline const StringUTF16 UnionRegexW::operator [] ( int index ) const {
    return index < Match.GetCount() ? Match[index] : StringUTF16::GetEmpty();
  }


  inline const StringUTF16 UnionRegexW::GetValue() const {
    return Match.GetFirst();
  }


  inline uint UnionRegexW::GetLength() const {
    return Match.GetCount();
  }


  inline const StringUTF16* UnionRegexW::begin() const {
    return Match.begin();
  }


  inline const StringUTF16* UnionRegexW::end() const {
    return Match.end();
  }


  typedef UnionRegexA RegexANSI;
  typedef UnionRegexW RegexUTF16;

#ifdef _UNICODE
  typedef UnionRegexW Regex;
#else
  typedef UnionRegexA Regex;
#endif
}

#pragma pack(pop)
#pragma warning(pop)
#endif // __UNION_REGEX_H__
/***************************************************************************
                          string_util.h  -  description
                             -------------------
    begin                : Sat Apr 26 2003
    copyright            : (C) 2003 by Blake Madden
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License.                                *
 *                                                                         *
 ***************************************************************************/

#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <cwctype>
#include <cctype>
#include <cstring>
#include <cwchar>
#include <cstddef>
#include <string>
#include <functional>
#include <cassert>
#include <clocale>
#include <stdexcept>
#include "meta.h"
#include "utilities.h"

namespace string_util
    {
    template<typename Tchar_type>
    inline bool is_one_of(const Tchar_type character, const Tchar_type* char_string)
        {
        while(*char_string)
            {
            if (character == char_string[0])
                {
                return true;
                }
            ++char_string;
            }
        return false;
        }

    ///lowercases any Western European alphabetic characters
    inline char tolower_western(const char c)
        {
        return ((c >= 'A') && (c <= 'Z')) ||
            ((c >= assign_character(char, 0xC0)) && (c <= assign_character(char, 0xD6))) ||
            ((c >= assign_character(char, 0xD8)) && (c <= assign_character(char, 0xDE))) 
                ? (c + 32) : c;
        }
    inline wchar_t tolower_western(const wchar_t c)
        {
        return ((c >= L'A') && (c <= L'Z')) ||
            ((c >= 0xC0) && (c <= 0xD6)) ||
            ((c >= 0xD8) && (c <= 0xDE)) 
                ? (c + 32) : c;
        }

    ///uppercases any Western European alphabetic characters
    inline char toupper_western(const char c)
        {
        return ((c >= 'a') && (c <= 'z')) ||
            ((c >= assign_character(char, 0xE0)) && (c <= assign_character(char, 0xF6))) ||
            ((c >= assign_character(char, 0xF8)) && (c <= assign_character(char, 0xFE)))  
                ? (c - 32) : c;
        }
    inline wchar_t toupper_western(const wchar_t c)
        {
        return ((c >= L'a') && (c <= L'z')) ||
            ((c >= 0xE0) && (c <= 0xF6)) ||
            ((c >= 0xF8) && (c <= 0xFE))   
                ? (c - 32) : c;
        }

    inline bool isupper_western(const char c)
        {
        return ((c >= 0x41) && (c <= 0x5A)) ||
            ((c >= assign_character(char, 0xC0)) && (c <= assign_character(char, 0xD6))) ||
            ((c >= assign_character(char, 0xD8)) && (c <= assign_character(char, 0xDE)));
        }
    inline bool isupper_western(const wchar_t c)
        {
        return ((c >= 0x41) && (c <= 0x5A)) ||
            ((c >= 0xC0) && (c <= 0xD6)) ||
            ((c >= 0xD8) && (c <= 0xDE));
        }
    inline bool islower_western(const char c)
        {
        return ((c >= 0x61) && (c <= 0x7A)) ||
            ((c >= assign_character(char, 0xE0)) && (c <= assign_character(char, 0xF6))) ||
            ((c >= assign_character(char, 0xF8)) && (c <= assign_character(char, 0xFE)));
        }
    inline bool islower_western(const wchar_t c)
        {
        return ((c >= 0x61) && (c <= 0x7A)) ||
            ((c >= 0xE0) && (c <= 0xF6)) ||
            ((c >= 0xF8) && (c <= 0xFE));
        }
    ///Returns true is character is a Western alphabetic letter
    inline bool isalpha_western(const char c)
        {
        return (((c >= 0x41) && (c <= 0x5A)) ||
            ((c >= 0x61) && (c <= 0x7A)) ||
            ((c >= assign_character(char, 0xC0)) && (c <= assign_character(char, 0xD6)))||
            ((c >= assign_character(char, 0xD8)) && (c <= assign_character(char, 0xF6))) ||
            ((c >= assign_character(char, 0xF8)) && (c <= assign_character(char, 0xFF))));
        }
    inline bool isalpha_western(const wchar_t c)
        {
        return (((c >= 0x41) && (c <= 0x5A)) ||
            ((c >= 0x61) && (c <= 0x7A)) ||
            ((c >= 0xC0) && (c <= 0xD6))||
            ((c >= 0xD8) && (c <= 0xF6)) ||
            ((c >= 0xF8) && (c <= 0xFF)));
        }

    ///ANSI C decorators
    ///strtol
    inline double strtol(const char* str, char** strend, int radix)
        { return std::strtol(str, strend, radix); }
    inline double strtol(const wchar_t* str, wchar_t** strend, int radix)
        { return std::wcstol(str, strend, radix); }
    ///strtod
    inline double strtod(const char* str, char** strend)
        { return std::strtod(str, strend); }
    inline double strtod(const wchar_t* str, wchar_t** strend)
        { return std::wcstod(str, strend); }
    ///atoi
    inline int atoi(const char* str)
        {
        if (str == NULL)
            { return 0; }
        return std::atoi(str);
        }
    inline int atoi(const wchar_t* str)
        {
        if (str == NULL)
            { return 0; }
        wchar_t* dummy = NULL;
        return static_cast<int>(std::wcstol(str, &dummy, 10));
        }
    ///atol
    inline int atol(const char* str)
        {
        if (str == NULL)
            { return 0; }
        return std::atoi(str);
        }
    inline int atol(const wchar_t* str)
        {
        if (str == NULL)
            { return 0; }
        wchar_t* dummy = NULL;
        return std::wcstol(str, &dummy, 10);
        }
    /**ispunct (template interface included to make it easier to use as a predicate
    for STL functions).*/
    template<typename T>
    inline int ispunct(T ch)
        {
        return std::ispunct(static_cast<unsigned char>(ch));
        }
    ///partial specialized version of ispunct
    inline int ispunct(int ch)
        {
        return std::ispunct(static_cast<unsigned char>(ch));
        }
    inline int ispunct(wchar_t ch)
        {
        return std::iswpunct(ch);
        }
    /**ispunct for ISO 8891-1 and MS 1252.
    This is more locale-independent and also handles surrogates (e.g., the Euro
    symbol under 1252) that the regular ispunct would miss.*/
    template<typename T>
    inline bool is_western_punct(T ch)
        {
        //see if it is either a space, control character, or alphanumeric and negate that
        return !(((ch >= 0x30/*'0'*/) && (ch <= 0x39/*'9'*/)) ||
            ((ch >= 0x41/*'A'*/) && (ch <= 0x5A/*'Z'*/)) ||
            ((ch >= 0x61/*'a'*/) && (ch <= 0x7A/*'z'*/)) ||
            //uppercase extended ASCII set
            ((ch >= assign_character(T, 0xC0)) && (ch <= assign_character(T, 0xD6))) ||
            ((ch >= assign_character(T, 0xD8)) && (ch <= assign_character(T, 0xDF))) ||
            //lowercase extended ASCII set
            ((ch >= assign_character(T, 0xE0)) && (ch <= assign_character(T, 0xF6))) ||
            ((ch >= assign_character(T, 0xF8)) && (ch <= assign_character(T, 0xFF))) ||
            //spaces and control characters
            ((ch >= 0x00) && (ch <= 0x20)) );
        }
    ///isdigit
    inline int isdigit(int ch)
        {
        return std::isdigit(static_cast<unsigned char>(ch));
        }
    inline int isdigit(wchar_t ch)
        {
        return std::iswdigit(ch);
        }
    ///isspace
    inline int isspace(char c)
        {
        return std::isspace(static_cast<unsigned char>(c));
        }
    inline int isspace(wchar_t c)
        {
        return std::iswspace(c);
        }
    ///tolower
    inline int tolower(char c)
        {
        return std::tolower(static_cast<unsigned char>(c));
        }
    inline wchar_t tolower(wchar_t c)
        {
        return std::towlower(c);
        }
    ///toupper
    inline int toupper(char c)
        {
        return std::toupper(static_cast<unsigned char>(c));
        }
    inline wchar_t toupper(wchar_t c)
        {
        return std::towupper(c);
        }
    ///memset
    template<typename T>
    inline T* memset(T* dest, int c, size_t count)
        {
        return static_cast<T*>(std::memset(dest, c, count));
        }
    //partial specialized versions of memset
    inline char* memset(char* dest, int c, size_t count)
        {
        return static_cast<char*>(std::memset(dest, c, count));
        }
    inline wchar_t* memset(wchar_t* dest, int c, size_t count)
        {
        return std::wmemset(dest, static_cast<wchar_t>(c), count);
        }
    ///strchr
    inline const char* strchr(const char* s, int ch)
        {
        return std::strchr(s, ch);
        }
    inline const wchar_t* strchr(const wchar_t* s, wchar_t ch)
        {
        return std::wcschr(s, ch);
        }
    ///strstr
    inline const char* strstr(const char* s1, const char* s2)
        {
        return std::strstr(s1, s2);
        }
    inline const wchar_t* strstr(const wchar_t* s1, const wchar_t* s2)
        {
        return std::wcsstr(s1, s2);
        }
    ///strcspn
    inline size_t strcspn(const char* string1,const char* string2 )
        {
        return std::strcspn(string1, string2);
        }
    inline size_t strcspn(const wchar_t* string1,const wchar_t* string2 )
        {
        return std::wcscspn(string1, string2);
        }
    ///strncat
    inline char* strncat(char* strDest, const char* strSource, size_t count)
        {
        return std::strncat(strDest, strSource, count);
        }
    inline wchar_t* strncat(wchar_t* strDest, const wchar_t* strSource, size_t count)
        {
        return std::wcsncat(strDest, strSource, count);
        }
    ///wctomb
    inline int wctomb(wchar_t* s, wchar_t wc)
        {
        s[0] = wc;
        return -1;
        }
    inline int wctomb(char* s, wchar_t wc)
        {
        return std::wctomb(s, wc);
        }
    //strlen
    inline size_t strlen(const char* text) { return std::strlen(text); }
    inline size_t strlen(const wchar_t* text)
        { return std::wcslen(text); }
    ///strcmp
    inline int strcmp(const char* string1, const char* string2)
        {
        return std::strcmp(string1, string2);
        }
    inline int strcmp(const wchar_t* string1, const wchar_t* string2)
        {
        return std::wcscmp(string1, string2);
        }
    ///strncmp
    inline int strncmp(const char* string1, const char* string2, size_t count)
        {
        return std::strncmp(string1, string2, count);
        }
    inline int strncmp(const wchar_t* string1, const wchar_t* string2, size_t count)
        {
        return std::wcsncmp(string1, string2, count);
        }
    ///strncpy
    inline char* strncpy(char* strDest, const char* strSource, size_t count)
        {
        return std::strncpy(strDest, strSource, count);
        }
    inline wchar_t* strncpy(wchar_t* strDest, const wchar_t* strSource, size_t count)
        {
        return std::wcsncpy(strDest, strSource, count);
        }

    ///functions not available in ANSI C
    /**converts string in hex format to int. Default figures out how much of the string
    is a valid hex string, but passing a value to the second parameter overrides this
    and allows you to indicate how much of the string to try to convert.*/
    template<typename T>
    int axtoi(const T* hexStg, size_t length = 0)
        {
        if (!hexStg)
            { return 0; }
        //skip over leading Ox
        if (hexStg[0] == 0x30/*0*/ && (hexStg[1] == 0x78/*x*/ || hexStg[1] == 0x58/*X*/))
            {
            hexStg += 2;
            //if they specified a length to read then take into account the Ox that we just skipped over
            if (length != 0)
                { length -= 2; }
            }
        if (length == 0)
            {
            const T* currentPos = hexStg;
            do
                {
                if (!is_hex_digit(currentPos[0]) || currentPos[0] == 0)
                    { break; }
                }
            while (currentPos++);

            length = currentPos-hexStg;
            //if no valid hex digits then fail and return zero
            if (length == 0)
                { return 0; }
            }
        size_t n = 0;      // position in string
        size_t m = 0;      // position in digit[] to shift
        int intValue = 0;  // integer value of hex string
        int* digit = new int[length+1];      // hold values to convert
        std::memset(digit, 0, length+1);
        while (n < length)
            {
            if (hexStg[n] == 0)
                break;
            if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
                digit[n] = hexStg[n] & 0x0f;            //convert to int
            else if (hexStg[n] >=0x61/*'a'*/ && hexStg[n] <= 0x66/*'f'*/) //if a to f
                digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
            else if (hexStg[n] >=0x41/*'A'*/ && hexStg[n] <= 0x46/*'F'*/) //if A to F
                digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
            else break;
            ++n;
            }
        size_t count = n;
        m = n - 1;
        n = 0;
        while(n < count)
            {
            // digit[n] is value of hex digit at position n
            // (m << 2) is the number of positions to shift
            // OR the bits into return value
            intValue = intValue | (digit[n] << (m << 2));
            --m;   // adjust the position to set
            ++n;   // next digit to process
            }
        delete [] digit;
        return intValue;
        }

    template<typename T>
    inline bool is_hex_digit(const T ch)
        {
        return (string_util::isdigit(ch) ||
                ((ch >= 0x61/*'a'*/ && ch <= 0x66/*'f'*/) ||
                (ch >= 0x41/*'A'*/ && ch <= 0x46/*'F'*/)) );
        }

    ///search for substring in string (case-insensitive)
    template<typename T>
    inline T* stristr(const T* string, const T* strSearch)
        {
        if (!string || !strSearch)
            { return NULL; }
        size_t substring_len = string_util::strlen(strSearch);
        while (string)
            {
            //compare the characters one at a time
            size_t i = 0;
            for (i = 0; i < substring_len; ++i)
                {
                if (string[i] == 0)
                    { return NULL; }
                if (string_util::tolower(strSearch[i]) != string_util::tolower(string[i]) )
                    {
                    ++string;
                    break;
                    }
                }
            //if the substring loop completed then the substring was found
            if (i == substring_len)
                { return const_cast<T*>(string); }
            }
        return NULL;
        }

    ///search for substring in a substring of a larger string (case-insensitive)
    template<typename T>
    inline T* strnistr(const T* string, const T* strSearch, const size_t string_len)
        {
        if (!string || !strSearch || string_len == 0)
            { return NULL; }
        size_t substring_len = string_util::strlen(strSearch);
        if (substring_len == 0 ||
            string_len < substring_len)
            { return NULL; }
        for (size_t i = 0;
            (i <= string_len-substring_len) && (string+i);
            ++i)
            {
            //compare the characters one at a time
            size_t j = 0;
            for (j = 0; j < substring_len; ++j)
                {
                if (string_util::tolower(strSearch[j]) != string_util::tolower(string[i+j]) )
                    { break; }
                if (string[i+j] == 0)
                    { return NULL; }
                }
            //if the substring loop completed then the substring was found
            if (j == substring_len)
                { return const_cast<T*>(string+i); }
            }
        return NULL;
        }

    /**search for substring in string (case-insensitive), but make sure that
    what you are searching for is not in quotes.*/
    template<typename T>
    inline T* stristr_not_quoted(const T* string, const T* strSearch,
                                 const T quoteSymbol = 0x22/*standard quotation mark*/)
        {
        if (!string || !strSearch)
            { return NULL; }
        size_t substring_len = string_util::strlen(strSearch);
        size_t quotesInSearchString = 0;
        //first see how many quotes are in the search string--we don't want to count these
        for (size_t i = 0; i < substring_len; ++i)
            {
            if (strSearch[i] == quoteSymbol)
                { ++quotesInSearchString; }
            }
        
        const T* original_string = string;
        size_t quote_count = 0;
        while (string)
            {
            //compare the characters one at a time
            size_t i = 0;
            for (i = 0; i < substring_len; ++i)
                {
                if (string[i] == 0)
                    { return NULL; }
                if (string[i] == quoteSymbol)
                    {
                    /*logic for weird scenario where the substring begins with a quote.
                    If this is a quote and this index has not already been counted from a previous loop,
                    then it may increment the quote count.*/
                    if (strSearch[0] == quoteSymbol)
                        {
                        if (i > 0 || string == original_string)
                            {++quote_count; }
                        }
                    else
                        { ++quote_count; }
                    }
                if (string_util::tolower(strSearch[i]) != string_util::tolower(string[i]) )
                    {
                    ++string;
                    break;
                    }
                }
            //if the substring loop completed then the substring was found.            
            if (i == substring_len)
                {
                /*make sure there are an even number of quotes in front of this;
                otherwise, it is inside of quotes and we need to skip it.*/
                if (is_even(quote_count-quotesInSearchString))
                    { return const_cast<T*>(string); }
                else
                    { string += substring_len; }
                }
            }
        return NULL;
        }

    /**Search string in reverse for substring.
    "offset" is how far we are in the source string already and how far
    to go back*/
    template<typename T>
    inline const T* strrstr(const T* string,
                    const T* search,
                    size_t offset)
        {
        if (!string || !search)
            { return NULL; }
        size_t len = string_util::strlen(search);
        if (len > offset) return NULL;
        string -= len;
        offset -= len;
        bool fnd = false;
        while (!fnd && offset > 0)
            {
            fnd = true;
            for (size_t i=0; i < len; ++i)
                {
                if (string[i] != search[i])
                    {
                    fnd = false;
                    break;
                    }
                }
            if (fnd) return string;
            --string;
            --offset;
            }
        return NULL;
        }

    //Case-insensitive comparison by character count
    template<typename T>
    inline int strnicmp(const T* first, const T* last, size_t count)
        {
        //first check if either of the strings are NULL
        if (!first && !last)
            { return 0; }
        else if (!first && last)
            { return -1; }
        else if (first && !last)
            { return 1; }

        int f,l;
        int result = 0;

        if (count)
            {
            do
                {
                f = string_util::tolower(*(first++) );
                l = string_util::tolower(*(last++) );
                } while ( (--count) && f && (f == l) );
            result = static_cast<int>(f - l);
            }
        return result;
        }

    ///Case-insensitve comparison
    template<typename T>
    inline int stricmp(const T* first, const T* last)
        {
        //first check if either of the strings are NULL
        if (!first && !last)
            { return 0; }
        else if (!first && last)
            { return -1; }
        else if (first && !last)
            { return 1; }

        int f,l;
        do
            {
            f = string_util::tolower(*(first++) );
            l = string_util::tolower(*(last++) );
            } while (f && (f == l) );

        return static_cast<int>(f - l);
        }

    /**Natural order comparison functions.
    Compare, recognizing numeric strings*/
    template<typename T>
    inline int strnatordcmp(const T* first_string, const T* second_string, bool case_insensitive = false)
        {
        //first check if either of the strings are NULL
        if (!first_string && !second_string)
            { return 0; }
        else if (!first_string && second_string)
            { return -1; }
        else if (first_string && !second_string)
            { return 1; }

        size_t first_string_index = 0, second_string_index = 0;
        T ch1, ch2;
 
        while (true)
            {
            ch1 = first_string[first_string_index];
            ch2 = second_string[second_string_index];

            //skip leading spaces
            while (string_util::isspace(ch1))
                { ch1 = first_string[++first_string_index]; }

            while (string_util::isspace(ch2))
                { ch2 = second_string[++second_string_index]; }

            //process run of digits
            if (string_util::isdigit(ch1) && string_util::isdigit(ch2))
                {
                T *firstEnd, *secondEnd;
                double firstDouble = string_util::strtod(first_string+first_string_index, &firstEnd);
                double secondDouble = string_util::strtod(second_string+second_string_index, &secondEnd);

                if (firstDouble < secondDouble)
                    { return -1; }
                else if (firstDouble > secondDouble)
                    { return 1; }
                else //numbers are equal
                    {
                    //if this was the end of both strings then they are equal
                    if (*firstEnd == 0 && *secondEnd == 0)
                        { return 0; }
                    /*the first string is done, but there is more to the second string
                    after the number, so first is smaller*/
                    else if (*firstEnd == 0)
                        { return -1; }
                    /*the second string is done, but there is more to the first string
                    after the number, so first is bigger*/
                    else if (*secondEnd == 0)
                        { return 1; }
                    //there is more to both of them, so move the counter and move on
                    else
                        {
                        first_string_index = (firstEnd - first_string);
                        second_string_index = (secondEnd - second_string);
                        continue;
                        }
                    }
                }

            //if we are at the end of the strings then they are the same
            if (ch1 == 0 && ch2 == 0)
                { return 0; }

            if (case_insensitive)
                {
                ch1 = string_util::tolower(ch1);
                ch2 = string_util::tolower(ch2);
                }

            if (ch1 < ch2)
                { return -1; }
            else if (ch1 > ch2)
                { return 1; }

            ++first_string_index;
            ++second_string_index;
            }
        }

    ///Compare, recognizing numeric strings and ignoring case
    template<typename T>
    inline int strnatordncasecmp(const T* a, const T* b)
        {
        return strnatordcmp(a, b, true);
        }

    /**search for a single character in a string, but making sure that
    it is not inside of a pair of double quotes.*/
    template<typename T>
    inline const T* strchr_not_quoted(const T* string, const T ch,
                                      const T quoteSymbol = 0x22/*standard quotation mark*/)
        {
        if (!string)
            { return NULL; }
        int ch_quote_count = ((ch==quoteSymbol) ? 1 : 0);
        size_t quote_count = 0;
        while (string)
            {
            if (string[0] == 0)
                { return NULL; }
            if (string[0] == quoteSymbol)
                { ++quote_count; }
                if (string[0] == ch && is_even(quote_count-ch_quote_count))
                { return string; }
            ++string;
            }
        return NULL;
        }

    /**search for a single character in a string for
    n number of characters. Size argument is assumed to be less than
    the length of the string being searched.*/
    template<typename T>
    inline const T* strnchr(const T* string, const T ch, size_t size)
        {
        if (!string)
            { return NULL; }
        size_t i = 0;
        for (i = 0; i < size; ++i)
            {
            /*throw boundary error if string being searched is shorter
            than the size argument*/
            if (string[i] == 0)
                { throw std::out_of_range(""); }
            if (string[i] == ch)
                { return string+i; }
            }
        return NULL;
        }

    /**search for a single character from a sequence in a string and
    return a pointer if found.*/
    template<typename T>
    inline const T* strcspn_pointer(const T* string1, const T* string2)
        {
        if (!string1 || !string2)
            { return NULL; }
        size_t i = 0;
        size_t string2Length = string_util::strlen(string2);
        while (string1)
            {
            if (string1[0] == 0)
                { return NULL; }
            for (i = 0; i < string2Length; ++i)
                {
                if (string1[0] == string2[i])
                    { return string1; }
                }
            ++string1;
            }
        return NULL;
        }

    /**search for a single character from a sequence in a string for
    n number of characters.*/
    template<typename T>
    inline size_t strncspn(const T* string1, const T* string2, size_t size)
        {
        if (!string1 || !string2 || size == 0)
            { return size; }
        size_t i = 0, j = 0;
        size_t string2Length = string_util::strlen(string2);
        for (i = 0; i < size; ++i)
            {
            /*throw boundary error if string being searched is shorter
            than the size argument*/
            if (string1[i] == 0)
                { throw std::out_of_range(""); }
            for (j = 0; j < string2Length; ++j)
                {
                if (string1[i] == string2[j])
                    { return i; }
                }
            }
        return i;
        }

 //   /**search for a single character from a sequence in a string in reverse.*/
 //   template<typename T>
    //inline const T* find_last_of(const T* string,
    //                const T* search,
    //                size_t offset)
    //    {
 //       if (!string || !search)
    //        { return NULL; }
    //    size_t len = string_util::strlen(search);
    //    while (offset >= 0)
    //        {
    //        for (size_t i=0; i < len; ++i)
    //            {
    //            if (string[offset] == search[i])
    //                { return (string+offset); }
    //            }
    //        --offset;
    //        }
    //    return NULL;
    //    }

    /**search for a single character not from a sequence in a string in reverse.*/
    template<typename T>
    inline size_t find_last_not_of(const T* string,
                    const T* search,
                    size_t offset = -1)
        {
        if (!string || !search)
            { return -1; }
        if (offset == -1)
            { offset = string_util::strlen(string)-1; }
        size_t len = string_util::strlen(search);

        size_t i=0;
        while (offset != -1)
            {
            for (i = 0; i < len; ++i)
                {
                if (string[offset] == search[i])
                    { break; }
                }
            if (i < len)
                { --offset; }
            else
                { return offset; }
            }
        return -1;
        }

    /*search for the first occurance that is not a character from a sequence in
    a string for n number of characters and returns zero-based index if found.
    Return value will be the same as size if not found.*/
    template<typename T>
    inline size_t find_first_not_of(const T* string1, const T* string2, size_t size)
        {
        if (!string1 || !string2 || size == 0)
            { return 0; }
        size_t i = 0, j = 0;
        size_t string2Length = string_util::strlen(string2);
        for (i = 0; i < size; ++i)
            {
            /*throw boundary error if string being searched is shorter
            than the size argument*/
            if (string1[i] == 0)
                { throw std::out_of_range(""); }
            for (j = 0; j < string2Length; ++j)
                {
                //stop if a character matching anything in the sequence
                if (string1[i] == string2[j])
                    { break; }
                }
            //if we went through the whole sequence then nothing matched
            if (j == string2Length)
                { return i; }
            }
        return i;
        }

    //utility classes
    //equal functors
    template<typename T>
    class equal_string_i_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::stricmp(a_, b_) == 0);
            }
        };

    //less functors
    template<typename T>
    class less_string_n_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        less_string_n_compare(size_t comparison_size) : m_comparison_size(comparison_size) {}
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strncmp(a_, b_, m_comparison_size) < 0);
            }
    private:
        size_t m_comparison_size;
        };

    template<typename T>
    class less_string_ni_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        less_string_ni_compare(size_t comparison_size) : m_comparison_size(comparison_size) {}
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strnicmp(a_, b_, m_comparison_size) < 0);
            }
    private:
        size_t m_comparison_size;
        };

    template<typename T>
    class less_string_i_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::stricmp(a_, b_) < 0);
            }
        };

    template<typename T>
    class less_string_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strcmp(a_, b_) < 0);
            }
        };

    template<typename T>
    class less_string_natural_order_i_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strnatordncasecmp(a_, b_) < 0);
            }
        };

    ///trims whitespace from around a string
    template<typename char_typeT>
    class string_trim
        {
    public:
        const char_typeT* operator()(const char_typeT* value,
                                     size_t length = std::basic_string<char_typeT>::npos)
            {
            m_trimmed_string_length = 0;
            if (value == NULL)
                { return NULL; }
            if (length == 0)
                { return value; }
            if (length == std::basic_string<char_typeT>::npos)
                { length = string_util::strlen(value); }
            const char_typeT* start = value;
            //end is last valid character in the string, not the one after it
            const char_typeT* end = value+(length-1);
            while (start && (start <= end))
                {
                if (string_util::isspace(start[0]))
                    { ++start; }
                else
                    { break; }
                }
            while (end > start)
                {
                if (string_util::isspace(end[0]))
                    { --end; }
                else
                    { break; }
                }
            //if start overran end then this string was all spaces.
            m_trimmed_string_length = (start > end) ? 0 : (end-start)+1;
            return start;
            }
        size_t get_trimmed_string_length() const
            { return m_trimmed_string_length; }
    private:
        size_t m_trimmed_string_length;
        };

    ///helper functions for stemmers
    template<typename Tchar_type, typename T>
    inline void replace_all(T& text, Tchar_type text_to_replace, Tchar_type replacement_text)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(text_to_replace, start);
            if (start == T::npos)
                {
                return;
                }
            text[start++] = replacement_text;
            }
        }

    template<typename T, typename Tchar_type>
    inline void replace_all(T& text, const Tchar_type* text_to_replace, const Tchar_type* replacement_text)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(text_to_replace, start);
            if (start == T::npos)
                {
                return;
                }
            text.replace(start, string_util::strlen(text_to_replace), replacement_text);
            }
        }

    template<typename T>
    inline void replace_all(T& text, const T& text_to_replace, const T& replacement_text)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(text_to_replace, start);
            if (start == T::npos)
                {
                return;
                }
            text.replace(start, text_to_replace.length(), replacement_text);
            }
        }
    }

#endif //__STRING_UTIL_H__

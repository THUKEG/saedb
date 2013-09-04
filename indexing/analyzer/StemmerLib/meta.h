/***************************************************************************
                          meta.h  -  description
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

#ifndef __COMPILE_TIME_META_H__
#define __COMPILE_TIME_META_H__

#include <cwchar>

//converts constants to unsigned/unsigned char or wchar_t at compile time.
template<typename T, int N>
class __character
    {
public:
    static const T val = (N < 128) ? N : (N - 256);
    };

//specialized for signed char
template<unsigned N>
class __character<char, N>
    {
public:
    static const char val = (N < 128) ? N : static_cast<char>((N - 256));
    };

//specialized for unsigned char
template<unsigned N>
class __character<unsigned char, N>
    {
public:
    static const unsigned char val = (N < 256) ? N : 255;
    };

//specialized for wchar_t
template<unsigned N>
class __character<wchar_t, N>
    {
public:
    static const wchar_t val = N;
    };
#define assign_character(t, n) __character<t, n>::val

#endif //__COMPILE_TIME_META_H__

/***************************************************************************
                          utilities.h  -  description
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

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <algorithm>
#include <functional>
#include <cmath>
#include <cassert>

#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795
#endif

//returns the byte size of an array
#define size_of_array(x) (sizeof(x)/sizeof(x[0]))

inline double within_range(size_t start, size_t end, double value)
    {
    return ( (value >= start) && (value <= end) ) ? value :
            (value < start) ? start :
            (value > end) ? end : /*never reaches this branch*/ value;
    }

inline float degrees_to_radians(const float degrees)
    { return degrees * (M_PI/static_cast<float>(180)); }

//Opposite side is the side going upwards of a right triangle
inline float calc_opposite_side(const float hypontenuse, const float angleInDegrees)
    {
    return (sin(degrees_to_radians(angleInDegrees)) * hypontenuse);
    }

//Adjacent side is the side at the bottom of a right triangle
inline float calc_adjacent_side(const float hypontenuse, const float angleInDegrees)
    {
    return (cos(degrees_to_radians(angleInDegrees)) * hypontenuse);
    }

//Function to see if a number is even
template<typename T>
inline bool is_even(T value)
    { return (value%2) == 0; }
//specialized version of is_even for floating point value types that need to be "floored" first
inline bool is_even(double value)
    { return (static_cast<long>(std::floor(std::abs(value)))%2) == 0; }
inline bool is_even(float value)
    { return (static_cast<long>(std::floor(std::abs(value)))%2) == 0; }

///integer rounding function
template<typename T>
inline double round(T x)
    { return (std::floor(x+0.5f)); }

//class that remembers its original value from constuction
template <typename T>
class backup_variable
    {
public:
    backup_variable(const T& value) : m_originalValue(value), m_value(value)
        {}
    void operator=(const T& value)
        { m_value = value; }
    bool operator==(const T& value) const
        { return m_value == value; }
    bool operator<(const T& value) const
        { return m_value < value; }
    bool operator<=(const T& value) const
        { return m_value <= value; }
    bool operator>(const T& value) const
        { return m_value > value; }
    bool operator>=(const T& value) const
        { return m_value >= value; }
    void operator+(const T& value)
        { m_value + value; }
    void operator+=(const T& value)
        { m_value += value; }
    void operator-(const T& value)
        { m_value - value; }
    void operator-=(const T& value)
        { m_value -= value; }
    operator const T() const
        { return m_value; }
    T* operator&()
        { return &m_value; }
    T get_value() const
        { return m_value; }
    bool has_changed() const
        { return m_value != m_originalValue; }
private:
    T m_originalValue;
    T m_value;
    };

template<typename T>
inline bool is_either(T value, T first, T second)
    {
    return (value == first || value == second);
    }

template<typename T>
inline bool is_neither(T value, T first, T second)
    {
    assert(first != second);
    return (value != first && value != second);
    }

template<typename T>
inline bool is_within(T value, T first, T second)
    {
    assert(first <= second);
    return (value >= first && value <= second);
    }

/*calls a member function of elements in a container for each
elelement in another container*/
template<typename inT, typename outT, typename member_extract_functorT>
inline outT copy_member(inT begin, inT end, outT dest, member_extract_functorT get_value)
    {
    for (; begin != end; ++dest, ++begin)
        *dest = get_value(*begin);
    return (dest);
    }

template<typename inT, typename outT,
         typename _Pr,
         typename member_extract_functorT>
inline outT copy_member_if(inT begin, inT end, outT dest,
                           _Pr meets_criteria,
                           member_extract_functorT get_value)
    {
    for (; begin != end; ++begin)
        {
        if (meets_criteria(*begin))
            {
            *dest = get_value(*begin);
            ++dest;
            }
        }
    return (dest);
    }

template<typename _InIt, typename _Pr, typename member_extract_functorT>
inline typename std::iterator_traits<_InIt>::difference_type
    count_member_if(_InIt _First, _InIt _Last,
                    _Pr _Pred, member_extract_functorT get_value)
    {
    //count elements satisfying _Pred
    typename std::iterator_traits<_InIt>::difference_type _Count = 0;

    for (; _First != _Last; ++_First)
        if (_Pred(get_value(*_First)) )
            ++_Count;
    return (_Count);
    }

//determines if number is even
template <typename T>
class even : public std::unary_function<T, bool>
    {
public:
    inline bool operator()(const T& val) const
        { return val%2==0; }
    };

//determines if a value is within a given range
template<typename T>
class within : public std::unary_function<T, bool>
    {
public:
    within(T range_begin, T range_end)
        : m_range_begin(range_begin), m_range_end(range_end)
        {}
    inline bool operator()(T value) const
        { return (value >= m_range_begin && value <= m_range_end); }
private:
    T m_range_begin;
    T m_range_end;
    };

#endif //__UTILITIES_H__

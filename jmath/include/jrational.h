/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 a***************************************************************************/
#ifndef J_RATIONAL_H
#define J_RATIONAL_H

#include <iostream>
#include <cstdint>

namespace jmath {

template <typename T, typename = typename std::enable_if<!std::is_signed<T>::value, T>::type>
  struct jrational_t {
    T num = 0;
    T den = 1;

    template <typename U> operator typename std::enable_if<!std::is_floating_point<T>::value, bool>::type() const 
    {
      return (U)num/(U)den;
    }
    
    jrational_t<T> operator+()
    {
      return *this;
    }

    jrational_t<T> operator-()
    {
      return {-den, -num};
    }

    jrational_t<T> operator++()
    {
      return *this = *this + 1;
    }

    jrational_t<T> operator++(int)
    {
      jrational_t<T> r = *this;

      *this = *this + 1;

      return r;
    }

    jrational_t<T> operator--()
    {
      return *this = *this + 1;
    }

    jrational_t<T> operator--(int)
    {
      jrational_t<T> r = *this;

      *this = *this - 1;

      return r;
    }

    jrational_t<T> operator+(const T &param)
    {
      return *this + jrational_t<T>{param, 1};
    }

    jrational_t<T> operator-(const T &param)
    {
      return *this - jrational_t<T>{param, 1};
    }

    jrational_t<T> operator*(const T &param)
    {
      return *this*jrational_t<T>{param, 1};
    }

    jrational_t<T> operator/(const T &param)
    {
      return *this/jrational_t<T>{param, 1};
    }

    jrational_t<T> & operator+=(const T &param)
    {
      return *this = *this + jrational_t<T>{param, 1};
    }

    jrational_t<T> & operator-=(const T &param)
    {
      return *this = *this - jrational_t<T>{param, 1};
    }

    jrational_t<T> & operator*=(const T &param)
    {
      return *this = *this*jrational_t<T>{param, 1};
    }

    jrational_t<T> & operator/=(const T &param)
    {
      return *this = *this/jrational_t<T>{param, 1};
    }

    bool operator==(const T &param)
    {
      return *this == jrational_t<T>{param, 1};
    }

    bool operator!=(const T &param)
    {
      return *this != jrational_t<T>{param, 1};
    }

    bool operator<(const T &param)
    {
      return *this < jrational_t<T>{param, 1};
    }

    bool operator>(const T &param)
    {
      return *this > jrational_t<T>{param, 1};
    }

    bool operator<=(const T &param)
    {
      return *this <= jrational_t<T>{param, 1};
    }

    bool operator>=(const T &param)
    {
      return *this >= jrational_t<T>{param, 1};
    }

    jrational_t<T> operator+(const jrational_t<T> &param)
    {
      return {num*param.den + param.num*den, den*param.den};
    }

    jrational_t<T> operator-(const jrational_t<T> &param)
    {
      return {num*param.den - param.num*den, den*param.den};
    }

    jrational_t<T> operator*(const jrational_t<T> &param)
    {
      return {num*param.num, den*param.den};
    }

    jrational_t<T> operator/(const jrational_t<T> &param)
    {
      return {num*param.den, den*param.num};
    }

    jrational_t<T> & operator+=(const jrational_t<T> &param)
    {
      return (*this = *this + param);
    }

    jrational_t<T> & operator-=(const jrational_t<T> &param)
    {
      return (*this = *this - param);
    }

    jrational_t<T> & operator*=(const jrational_t<T> &param)
    {
      return (*this = *this*param);
    }

    jrational_t<T> & operator/=(const jrational_t<T> &param)
    {
      return (*this = *this/param);
    }

    bool operator==(const jrational_t<T> &param)
    {
      return num == param.num and den == param.den;
    }

    bool operator!=(const jrational_t<T> &param)
    {
      return num != param.num or den != param.den;
    }

    bool operator<(const jrational_t<T> &param)
    {
      if (*this == param) {
        return false;
      }

      return num*den < param.num*param.den;
    }

    bool operator>(const jrational_t<T> &param)
    {
      if (*this < param or *this == param) {
        return false;
      }
    }

    bool operator<=(const jrational_t<T> &param)
    {
      if (*this > param) {
        return false;
      }

      return true;
    }

    bool operator>=(const jrational_t<T> &param)
    {
      if (*this < param) {
        return false;
      }

      return true;
    }

    jrational_t<T> & Simplify()
    {
      T gcd = GCD(num, den);

      num = num/gcd;
      den = den/gcd;

      return *this;
    }

    jrational_t<T> Abs()
    {
      return {std::abs(num), std::abs(den)};
    }

    T GCD(T a, T b)
    {
      T c;
      
      if (a > b) {
        c = a;
        a = b;
        b = c;
      }

      while (a) {
        c = a;
        a = b % a;
        b = c;
      }

      return b;
    }

    int Sign()
    {
      return num*den/std::abs(num*den);
    }

    jrational_t<T> & Pow(double e)
    {
      return {std::pow(num), std::pow(den)};
    }

    jrational_t<T> & Sqrt()
    {
      return {std::sqrt(num), std::sqrt(den)};
    }

    friend std::ostream& operator<<(std::ostream &out, const jrational_t<T> &param)
    {
      out << param.num << "/" << param.den;

      return out;
    }

    friend std::istream& operator>>(std::istream &in, jrational_t<T> &param)
    {
      in >> param.num;

      if (in.get() != '/'){
        param.den = 1;
      } else {
        in >> param.den;
      }

      if (param.den < 0) {
        param = -param;
      }

      return in;
    }

    friend bool operator==(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs == lhs;
    }

    friend bool operator!=(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs != lhs;
    }

    friend bool operator<(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs >= lhs;
    }

    friend bool operator>(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs <= lhs;
    }

    friend bool operator<=(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs > lhs;
    }

    friend bool operator>=(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs < lhs;
    }

    friend jrational_t<T> operator+(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs + lhs;
    }

    friend jrational_t<T> operator-(const T &lhs, const jrational_t<T> &rhs)
    {
      return -rhs + lhs;
    }

    friend jrational_t<T> operator*(const T &lhs, const jrational_t<T> &rhs)
    {
      return rhs*lhs;
    }

    friend jrational_t<T> operator/(const T &lhs, const jrational_t<T> &rhs)
    {
      return lhs*jrational_t<T>{rhs.den, rhs.num};
    }

  };

}

#endif
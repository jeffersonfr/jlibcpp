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
#ifndef J_VECTOR_H
#define J_VECTOR_H

#include "jexception/joutofboundsexception.h"
#include "jexception/jnullpointerexception.h"
#include "jexception/jinvalidargumentexception.h"

#include <iostream>
#include <fstream>
#include <complex>
#include <algorithm>
#include <optional>

namespace jmath {

template<size_t N, typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jvector_t {
    T data[N];

    template<typename U> operator jvector_t<N, U>()
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (U)data[i];
      }

      return v;
    }

    T & operator[](size_t n) 
    {
      if (n >= N) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[n];
    }

    bool operator==(T param)
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(T param)
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param) {
          return true;
        }
      }

      return false;
    }

    bool operator==(jvector_t<N, T> param)
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param.data[i]) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(jvector_t<N, T> param)
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param.data[i]) {
          return true;
        }
      }

      return false;
    }

    jvector_t<N, T> & operator-()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = -data[i];
      }

      return *this;
    }
 
    template<typename U> jvector_t<N, T> & operator=(U param)
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)param;
      }

      return *this;
    }

    template<typename U> jvector_t<N, T> operator+(U param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] + param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator-(U param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] - param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator*(U param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]*param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator/(U param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]/param);
      }

      return v;
    }

    template<typename U> jvector_t<N, T> & operator=(jvector_t<N, U> param)
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)param.data[i];
      }

      return *this;
    }

    template<typename U> jvector_t<N, T> operator+(jvector_t<N, U> param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] + param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator-(jvector_t<N, U> param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] - param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator*(jvector_t<N, U> param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]*param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator/(jvector_t<N, U> param)
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]/param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> & operator+=(jvector_t<N, U> param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jvector_t<N, T> & operator-=(jvector_t<N, U> param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jvector_t<N, T> & operator*=(jvector_t<N, U> param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jvector_t<N, T> & operator/=(jvector_t<N, U> param)
    {
      return (*this = *this/param);
    }
    
    template<typename U> T Scalar(jvector_t<N, U> param)
    {
      T n = 0;

      for (size_t i=0; i<N; i++) {
        n = n + (T)(data[i]*param.data[i]);
      }

      return n;
    }
    
    template<size_t M> jvector_t<M, T> SubVector(size_t n, size_t m)
    {
      if (n >= N or m > N) {
        throw jexception::OutOfBoundsException("Sub vector indexes are out of bounds");
      }

      jvector_t<M, T> v;

      for (size_t i=n; i<m; i++) {
        v.data[i - n] = data[i];
      }

      return v;
    }

    jvector_t<N, T> Pow(double e)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::pow(data[i], e);
      }

      return v;
    }

    jvector_t<N, T> Sqrt()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::sqrt(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Abs()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::abs(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Sin()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::sin(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Cos()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::cos(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Tan()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::tan(data[i]);
      }

      return v;
    }

    jvector_t<N, T> SinH()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::sinh(data[i]);
      }

      return v;
    }

    jvector_t<N, T> CosH()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::cosh(data[i]);
      }

      return v;
    }

    jvector_t<N, T> TanH()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::tanh(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Exp()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::exp(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Log()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::log(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Log2()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::log2(data[i]);
      }

      return v;
    }

    jvector_t<N, T> Log10()
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)std::log10(data[i]);
      }

      return v;
    }

    size_t Size()
    {
      return N;
    }

    T Sum()
    {
      T sum = 0;

      for (size_t i=0; i<N; i++) {
        sum = sum + data[i];
      }

      return sum;
    }

    T Mul()
    {
      T mul = 1;

      for (size_t i=0; i<N; i++) {
        mul = mul*data[i];
      }

      return mul;
    }

    T Mul(jvector_t<N, T> &param)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = data[i]*param.data[i];
      }

      return v;
    }

    T Div(jvector_t<N, T> &param)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = data[i]/param.data[i];
      }

      return v;
    }

    T Norm1()
    {
      return Abs().Sum();
    }

    T Norm()
    {
      T norm = 0.0f;

      for (size_t i=0; i<N; i++) {
        norm = norm + data[i]*data[i];
      }

      return norm;
    }

    double EuclidianNorm()
    {
      return std::sqrt(Norm());
    }

    T NormInf()
    {
      return Abs().Min();
    }

    jvector_t<N, double> Normalize()
    {
      return jvector_t<N, double>(*this)/EuclidianNorm();
    }
 
    std::optional<T &> Find(const T &param)
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] == param) {
          return data[i];
        }

        return std::nullopt;
      }

      return *std::min_element(data, data + N);
    }

    T Min()
    {
      return *std::min_element(data, data + N);
    }

    T Max()
    {
      return *std::max_element(data, data + N);
    }

    friend jvector_t<N, T> operator+(T param, jvector_t<N, T> thiz)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(param + thiz.data[i]);
      }

      return v;
    }
    
    friend jvector_t<N, T> operator-(T param, jvector_t<N, T> thiz)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(param - thiz.data[i]);
      }

      return v;
    }
    
    friend jvector_t<N, T> operator*(T param, jvector_t<N, T> thiz)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(param*thiz.data[i]);
      }

      return v;
    }
    
    friend jvector_t<N, T> operator/(T param, jvector_t<N, T> thiz)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(param/thiz.data[i]);
      }

      return v;
    }

    friend std::ostream & operator<<(std::ostream& out, const jvector_t<N, T> &param)
    {
      for (size_t i=0; i<N; i++) {
        out << param.data[i];

        if (i != (N - 1)) {
          out << ", ";
        }
      }

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jvector_t<N, T> &param) 
    {
      for (size_t i=0; i<N; i++) {
        is >> param(i);
      }

      return is;
    }

  };

}

#endif

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
#include <random>

namespace jmath {

template<class T> 
  struct is_complex : std::false_type {
  };

template<class T> 
  struct is_complex<std::complex<T>> : std::true_type {
  };

template<size_t N, typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value || is_complex<T>::value, T>::type>
  struct jvector_t {
    T data[N];

    static jvector_t<N, T> Random(double lo = 0.0, double hi = 1.0)
    {
      jvector_t<N, T> v;

      std::random_device rd;  //Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
      std::uniform_real_distribution<> distribution(lo, hi);

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)distribution(gen);
      }

      return v;
    }

    template<typename U> operator jvector_t<N, U>() const
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

    const T & operator[](size_t n) const
    {
      if (n >= N) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[n];
    }

    bool operator==(const T &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(const T &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param) {
          return true;
        }
      }

      return false;
    }

    bool operator==(const jvector_t<N, T> &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param.data[i]) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(const jvector_t<N, T> &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] != param.data[i]) {
          return true;
        }
      }

      return false;
    }

    jvector_t<N, T> operator-() const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = -data[i];
      }

      return v;
    }
 
    template<typename U> jvector_t<N, T> & operator=(const U &param)
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)param;
      }

      return *this;
    }

    template<typename U> jvector_t<N, T> operator+(const U &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] + param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator-(const U &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] - param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator*(const U &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]*param);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator/(const U &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]/param);
      }

      return v;
    }

    template<typename U> jvector_t<N, T> & operator=(const jvector_t<N, U> &param)
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)param.data[i];
      }

      return *this;
    }

    template<typename U> jvector_t<N, T> operator+(const jvector_t<N, U> &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] + param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator-(const jvector_t<N, U> &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i] - param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator*(const jvector_t<N, U> &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]*param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> operator/(const jvector_t<N, U> &param) const
    {
      jvector_t<N, U> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(data[i]/param.data[i]);
      }

      return v;
    }
    
    template<typename U> jvector_t<N, T> & operator+=(const jvector_t<N, U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jvector_t<N, T> & operator-=(const jvector_t<N, U> &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jvector_t<N, T> & operator*=(const jvector_t<N, U> &param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jvector_t<N, T> & operator/=(const jvector_t<N, U> &param)
    {
      return (*this = *this/param);
    }
    
    template<typename U> T Scalar(const jvector_t<N, U> &param) const
    {
      T n = 0;

      for (size_t i=0; i<N; i++) {
        n = n + (T)(data[i]*param.data[i]);
      }

      return n;
    }
    
    template<size_t M> jvector_t<M, T> SubVector(size_t n, size_t m) const
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

    jvector_t<N, T> & Pow(double e)
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::pow(data[i], e);
      }

      return *this;
    }

    jvector_t<N, T> & Sqrt()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::sqrt(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Abs()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::abs(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Sin()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::sin(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Cos()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::cos(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Tan()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::tan(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & SinH()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::sinh(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & CosH()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::cosh(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & TanH()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::tanh(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Exp()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::exp(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Log()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::log(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Log2()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::log2(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Log10()
    {
      for (size_t i=0; i<N; i++) {
        data[i] = (T)std::log10(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> & Sort()
    {
      std::sort(data, data + N);

      return *this;
    }

    jvector_t<N, T> & Reverse(size_t lo = 0, size_t hi = N - 1)
    {
      if (lo < 0 or hi < 0 or lo >= N or hi >= N or lo > hi) {
        throw jexception::OutOfBoundsException("Range index is out of bounds");
      }

      size_t length = hi - lo;

      for (size_t i=0; i<=length/2; i++) {
        std::swap(data[lo + i], data[hi - i]);
      }

      return *this;
    }

    jvector_t<N, T> & MoveLeft(size_t n)
    {
      jvector_t<N, T> v = *this;

      n = n%N;

      for (size_t i=0; i<N - n; i++) {
        data[i] = v.data[n + i];
      }

      for (size_t i=0; i<n; i++) {
        data[N - n + i] = v.data[i];
      }

      return *this;
    }

    jvector_t<N, T> & MoveRight(size_t n)
    {
      jvector_t<N, T> v = *this;

      n = n%N;

      for (size_t i=0; i<n; i++) {
        data[i] = v.data[N - n + i];
      }

      for (size_t i=0; i<N - n; i++) {
        data[n + i] = v.data[i];
      }

      return *this;
    }

    size_t Size() const
    {
      return N;
    }

    T Sum() const
    {
      T sum = 0;

      for (size_t i=0; i<N; i++) {
        sum = sum + data[i];
      }

      return sum;
    }

    T Mul() const
    {
      T mul = 1;

      for (size_t i=0; i<N; i++) {
        mul = mul*data[i];
      }

      return mul;
    }

    T Mul(jvector_t<N, T> &param) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = data[i]*param.data[i];
      }

      return v;
    }

    T Div(jvector_t<N, T> &param) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = data[i]/param.data[i];
      }

      return v;
    }

    T Norm1() const
    {
      jvector_t<N, T> v = *this;

      return v.Abs().Sum();
    }

    T Norm() const
    {
      T norm {0};

      for (size_t i=0; i<N; i++) {
        norm = norm + data[i]*data[i];
      }

      return norm;
    }

    double EuclidianNorm() const
    {
      return std::sqrt(Norm());
    }

    T NormInf() const
    {
      jvector_t<N, T> v = *this;

      return v.Abs().Min();
    }

    jvector_t<N, double> Normalize() const
    {
      return jvector_t<N, double>(*this)/EuclidianNorm();
    }
 
    jvector_t<N, double> Equalize(T lo, T hi) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (data[i] - lo)/(hi - lo);
      }

      return v;
    }
 
    jvector_t<N, double> Clip(T lo, T hi) const
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        T e = data[i];

        v.data[i] = (e < lo)?lo:(e > hi)?hi:e;
      }

      return v;
    }
 
    std::optional<T &> Find(const T &param) const
    {
      for (size_t i=0; i<N; i++) {
        if (data[i] == param) {
          return data[i];
        }

        return std::nullopt;
      }

      return *std::min_element(data, data + N);
    }

    T Min() const
    {
      return *std::min_element(data, data + N);
    }

    T Max() const
    {
      return *std::max_element(data, data + N);
    }

    jvector_t<N, T> & Conjugate()
    {
      static_assert(is_complex<T>::value, "T != std::complex<U>");

      for (size_t i=0; i<N; i++) {
        data[i] = std::conj(data[i]);
      }

      return *this;
    }

    jvector_t<N, T> Component(const jvector_t<N, T> &param)
    {
      return Scalar(param)/param.EuclidianNorm();
    }

    friend jvector_t<N, T> operator+(const T &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(param + thiz.data[i]);
      }

      return v;
    }
    
    friend jvector_t<N, T> operator-(const T &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(param - thiz.data[i]);
      }

      return v;
    }
    
    friend jvector_t<N, T> operator*(const T &param, const jvector_t<N, T> &thiz)
    {
      jvector_t<N, T> v;

      for (size_t i=0; i<N; i++) {
        v.data[i] = (T)(param*thiz.data[i]);
      }

      return v;
    }
    
    friend jvector_t<N, T> operator/(const T &param, const jvector_t<N, T> &thiz)
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

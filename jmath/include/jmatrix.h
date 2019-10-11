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
#ifndef J_MATRIX_H
#define J_MATRIX_H

#include "jmath/jvector.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jnullpointerexception.h"
#include "jexception/jinvalidargumentexception.h"

#include <iostream>
#include <fstream>
#include <complex>
#include <algorithm>
#include <random>

namespace jmath {

template<size_t R, size_t C, typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value || is_complex<T>::value, T>::type>
  struct jmatrix_t {
    typedef T value_type;

    jvector_t<C, T> data[R];

    static jmatrix_t<R, C, T> Identity()
		{
      jmatrix_t<R, C, T> m;

			m = 0;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if (j == i) {
          	m.data[j][i] = 1;
					}
        }
      }

      return m;
		}

    static jmatrix_t<R, C, T> GaussianKernel(T sigma = 1)
    {
      static_assert(R == C, "Kernel needs a square matrix");

      jmatrix_t<R, C, T> m;

      // intialising standard deviation to 1.0
      double s = 2.0*sigma*sigma, sum = 0.0;
      int offset = R/2;

      // generating kernel
      for (int y=-offset; y<=offset; y++) {
        for (int x=-offset; x<=offset; x++) {
          double 
            r = std::sqrt(x*x + y*y),
            e = exp(-(r*r)/s)/(M_PI*s);
          
          m.data[y + offset][x + offset] = e;

          sum = sum + e;
        }
      }

      // normalising the kernel
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<R; i++) {
          m.data[j][i] = m.data[j][i]/sum;
        }
      }

      return m;
    }

    static jmatrix_t<R, C, T> UniformRandom(double lo = 0.0, double hi = 1.0)
    {
      jmatrix_t<R, C, T> m;

      std::random_device rd;  //Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
      std::uniform_real_distribution<> distribution(lo, hi);

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<R; i++) {
          m.data[j][i] = (T)distribution(gen);
        }
      }

      return m;
    }

    template<typename U> operator jvector_t<R*C, U>() const
    {
      jvector_t<R*C, U> v;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          v.data[j*C + i] = (U)data[j][i];
        }
      }

      return v;
    }

    template<typename U> operator jmatrix_t<R, C, U>() const
    {
      jmatrix_t<R, C, U> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (U)data[j][i];
        }
      }

      return m;
    }

    template<typename U> jmatrix_t<R, C, U> Real() const
    {
      jmatrix_t<R, C, U> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (U)std::real(data[j][i]);
        }
      }

      return m;
    }

    template<typename U> jmatrix_t<R, C, U> Imag() const
    {
      jmatrix_t<R, C, U> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (U)std::imag(data[j][i]);
        }
      }

      return m;
    }

    T & operator[](size_t n)
    {
      if (n >= R*C) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[n/C][n%C];
    }

    const T & operator[](size_t n) const
    {
      if (n >= R*C) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[n/C][n%C];
    }

    T & operator()(size_t n) 
    {
      if (n >= R*C) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[n/C][n%C];
    }

    T & operator()(size_t row, size_t col) 
    {
      if (row >= R or col >= C) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[row][col];
    }

    const T & operator()(size_t n) const
    {
      if (n >= R*C) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[n/C][n%C];
    }

    const T & operator()(size_t row, size_t col) const
    {
      if (row >= R or col >= C) {
        throw jexception::OutOfBoundsException("Element index is out of bounds");
      }
      
      return data[row][col];
    }

    bool operator==(const T &param) const
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          if (data[j][i] != param) {
            return false;
          }
        }
      }

      return true;
    }

    bool operator!=(const T &param) const
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          if (data[j][i] != param) {
            return true;
          }
        }
      }

      return false;
    }

    bool operator==(const jmatrix_t<R, C, T> &param) const
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          if (data[j][i] != param.data[j][i]) {
            return false;
          }
        }
      }

      return true;
    }

    bool operator!=(const jmatrix_t<R, C, T> &param) const
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          if (data[j][i] != param.data[j][i]) {
            return true;
          }
        }
      }

      return false;
    }

    jmatrix_t<R, C, T> operator-() const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = -data[j][i];
        }
      }

      return m;
    }
 
    template<typename U> jmatrix_t<R, C, T> & operator=(const U &param)
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)param;
        }
      }

      return *this;
    }

    template<typename U> jmatrix_t<R, C, typename std::common_type<T, U>::type> operator+(const U &param) const
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (data[j][i] + param);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, typename std::common_type<T, U>::type> operator-(const U &param) const
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (data[j][i] - param);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, typename std::common_type<T, U>::type> operator*(const U &param) const
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (data[j][i]*param);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, typename std::common_type<T, U>::type> operator/(const U &param) const
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (data[j][i]/param);
        }
      }

      return m;
    }
 
    template<typename U> jmatrix_t<R, C, T> & operator=(const jmatrix_t<R, C, U> &param)
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)param.data[j][i];
        }
      }

      return *this;
    }

    template<typename U> jmatrix_t<R, C, typename std::common_type<T, U>::type> operator+(const jmatrix_t<R, C, U> &param) const
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (data[j][i] + param.data[j][i]);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, typename std::common_type<T, U>::type> operator-(const jmatrix_t<R, C, U> &param) const
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (data[j][i] - param.data[j][i]);
        }
      }

      return m;
    }
    
    template<size_t R1 = C, size_t C1, typename U> jmatrix_t<R, C1, typename std::common_type<T, U>::type> operator*(const jmatrix_t<R1, C1, U> &param) const
    {
      jmatrix_t<R, C1, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C1; i++) {
          m.data[j][i] = Row(j).Scalar(param.Column(i));
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, typename std::common_type<T, U>::type> operator/(const jmatrix_t<R, C, U> &param) const
    {
			return *this*param.Inverse();
    }
    
    template<typename U> jmatrix_t<R, C, T> & operator+=(const jmatrix_t<R, C, U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jmatrix_t<R, C, T> & operator-=(const jmatrix_t<R, C, U> &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jmatrix_t<R, C, T> & operator*=(const jmatrix_t<R, C, U> &param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jmatrix_t<R, C, T> & operator/=(const jmatrix_t<R, C, U> &param)
    {
      return (*this = *this/param);
    }
    
    constexpr size_t Size() noexcept
    {
      return R*C;
    }

    constexpr size_t Rows() noexcept
    {
      return R;
    }

    constexpr size_t Cols() noexcept
    {
      return C;
    }

    jvector_t<C, T> Row(size_t n) const
    {
      if (n >= R) {
        throw jexception::OutOfBoundsException("Row index is out of bounds");
      }

      return data[n];
    }

    jvector_t<R, T> Column(size_t n) const
    {
      if (n >= C) {
        throw jexception::OutOfBoundsException("Column index is out of bounds");
      }

      jvector_t<R, T> v;

      for (size_t j=0; j<R; j++) {
        v.data[j] = data[j][n];
      }

      return v;
    }

    jmatrix_t<R, C, T> & SwapRow(size_t i, size_t j)
    {
      if (i >= R or j >= R) {
        throw jexception::OutOfBoundsException("Row index is out of bounds");
      }

      if (i != j) {
        jvector_t<C, T> v = Row(i);

        Row(i, Row(j));
        Row(j, v);
      }
        
      return *this;
    }

    jmatrix_t<R, C, T> & SwapColumn(size_t i, size_t j)
    {
      if (i >= C or j >= C) {
        throw jexception::OutOfBoundsException("Column index is out of bounds");
      }

      if (i != j) {
        jvector_t<R, T> v = Column(i);

        Column(i, Column(j));
        Column(j, v);
      }
        
      return *this;
    }

    jmatrix_t<R, C, T> & Row(size_t n, const jmath::jvector_t<C, T> &v)
    {
      data[n] = v;

      return *this;
    }

    jvector_t<R, T> Column(size_t n, const jmath::jvector_t<R, T> &v)
    {
      for (size_t j=0; j<R; j++) {
        data[j][n] = v[j];
      }

      return *this;
    }

    template<size_t R1, size_t C1> jmatrix_t<R1, C1, T> Format() const
    {
      static_assert(R*C == R1*C1, "Numbers of elements must be equal");

      jmatrix_t<R1, C1, T> m;

      for (size_t i=0; i<R*C; i++) {
        m.data[i/C1][i%C1] = data[i/C][i%C];
      }

      return m;
    }

    template<size_t R1, size_t C1> jmatrix_t<R1, C1, T> SubMatrix(size_t r, size_t c) const
    {
      if (r >= R or (r + R1) > R or c >= C or (c + C1) > C) {
        throw jexception::OutOfBoundsException("Sub matrix indexes are out of bounds");
      }

      jmatrix_t<R1, C1, T> m;

      for (size_t j=0; j<R1; j++) {
        for (size_t i=0; i<C1; i++) {
          m.data[j][i] = data[j + r][i + c];
        }
      }

      return m;
    }

    template<size_t R1 = R - 1, size_t C1 = C - 1> double Cofactor(size_t r, size_t c) const
    {
      jmatrix_t<R1, C1, T> m;

      for (size_t j=0; j<R1; j++) {
        for (size_t i=0; i<C1; i++) {
          m.data[j][i] = data[j + ((j >= r)?1:0)][i + ((i >= c)?1:0)];
        }
      }

      return std::pow(-1, (r + c)&0x01)*m.Determinant();
    }

    jmatrix_t<R, C, T> RowEchelonForm()
    {
      size_t lead = 0;

      for (size_t row=0; row<R; row++) {
        if (lead > (C - 1)) {
          return *this;
        }

        size_t i = row;

        while (data[i][lead] == 0) {
          i = i + 1;

          if (i > (R - 1)) {
            i = row;
            lead = lead + 1;

            if (lead > (C - 1)) {
              return *this;
            }
          }
        }

        SwapRow(i, row);
        Row(row, data[row]/data[row][lead]);

        for (i=0; i<R; i++) {
          if (i != row) {
            Row(i, Row(i) - Row(row)*data[i][lead]);
          }
        }
      }

      return *this;
    }

    T Mean() const
		{
      T count = 0;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          count = count + data[j][i];
        }
      }

      return count/(R*C);
		}

    bool IsSquare() const
    {
      return R == C;
    }

    bool IsNormal() const
    {
      jmatrix_t<R, C, T> 
        m = *this,
        h = this->Hermitian();

      return (m*h == h*m);
    }

    bool IsSingular() const
    {
      return Determinant() == 0;
    }

    bool IsInvertible()
    {
      return IsSingular() == false and IsSquare() == true;
    }

    bool IsDiagonal() const
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if (j != i and data[j][i] != 0) {
            return false;
					}
        }
      }

      return true;
    }

    bool IsIdentity() const
    {
      if (IsDiagonal() == false) {
        return false;
      }

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if (j == i and data[j][i] != 1) {
            return false;
					}
        }
      }

      return true;
    }

    bool IsScalar() const
    {
      T ref = data[0][0];

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if ((j == i and data[j][i] != ref) or (j != i and data[j][i] != 0)) {
            return false;
					}
        }
      }

      return true;
    }

    bool IsNull() const
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if (data[j][i] != 0) {
            return false;
					}
        }
      }

      return true;
    }

    double LUDecomposition(double m[R][C]) const
    {
			if (R < 1 or C < 1) {
				return 0.0;
			}

      double 
				det = 1.0;
      int 
				ri[R];

      for (size_t i=0; i<R; i++) {
        ri[i] = i;
      }

      // LU factorization.
      for (size_t p=1; p<=R - 1; p++) {
        // Find pivot element.
        for (size_t i=p + 1; i<=R; i++) {
          if (abs(m[ri[i - 1]][p - 1]) > abs(m[ri[p - 1]][p - 1])) {
            // Switch the index for the p-1 pivot row if necessary.
            std::swap(ri[p - 1], ri[i - 1]);

            det = -det;
          }
        }

        if (m[ri[p - 1]][p - 1] == 0) {
          return 0.0;
        }

        det = det*m[ri[p - 1]][p - 1];

        for (size_t i=p + 1; i<=R; i++) {
          m[ri[i - 1]][p - 1] /= m[ri[p - 1]][p - 1];

          for (size_t j=p + 1; j<=R; j++)
            m[ri[i - 1]][j - 1] -= m[ri[i - 1]][p - 1]*m[ri[p - 1]][j - 1];
        }
      }

      return det*m[ri[R - 1]][R - 1];
    }

    double Determinant() const
    {
      static_assert(R == C, "Determinant needs a square matrix");

      double m[R][C];

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m[j][i] = (double)data[j][i];
        }
      }

      return LUDecomposition(m);
    }

    jmatrix_t<C, R, T> Transpose()
    {
      jmatrix_t<C, R, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[i][j] = data[j][i];
        }
      }

      return m;
    }

    T Trace() const
		{
      T sum = 0;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if (j == i) {
          	sum = sum + data[j][i];
					}
        }
      }

      return sum;
		}

    jmatrix_t<R, C, T> Adjoint() const
    {
      static_assert(R == C, "Adjoint needs a square matrix");

      jmatrix_t<R, C, T> m;

			for (size_t j=0; j<R; j++) {
				for (size_t i=0; i<C; i++) {
					m.data[j][i] = Cofactor(j, i);
				}
			}

			return m.Transpose();
    }

    jmatrix_t<R, C, T> Inverse() const
    {
      double d = Determinant();

      if (d == 0.0) {
        throw jexception::RuntimeException("Matrix in not inversible");
      }

      return Adjoint()/d;
    }

    std::optional<T &> Find(const T &param) const
    {
			for (size_t j=0; j<R; j++) {
        std::optional<T &>
          opt = data[j].Find(param);

        if (opt != std::nullopt) {
          return opt;
        }
			}

      return std::nullopt;
    }

    T Min() const
    {
      return jvector_t<R*C, T>(*this).Min();
    }

    T Max() const
    {
      return jvector_t<R*C, T>(*this).Max();
    }

    T Sum() const
    {
      T sum = 0;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          sum = sum + data[j][i];
        }
      }

      return sum;
    }

    T Mul() const
    {
      T mul = 1;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          mul = mul*data[j][i];
        }
      }

      return mul;
    }

    T Mul(const jmatrix_t<R, C, T> &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = data[j][i]*param.data[j][i];
        }
      }

      return m;
    }

    T Div(const jmatrix_t<R, C, T> &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = data[j][i]/param.data[j][i];
        }
      }

      return m;
    }

    T Norm1() const
    {
      jmatrix_t<R, C, T> m = *this;

      return m.Abs().Sum();
    }

    T Norm() const
    {
      T norm {0};

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          norm = norm + data[j][i]*data[j][i];
        }
      }

      return norm;
    }

    double EuclidianNorm() const
    {
      return std::sqrt(Norm());
    }

    T NormInf() const
    {
      jmatrix_t<R, C, T> m = *this;

      return m.Abs().Min();
    }

    jmatrix_t<R, C, double> Normalize() const
    {
      return jmatrix_t<R, C, double>(*this)/EuclidianNorm();
    }
 
    jmatrix_t<R, C, double> Equalize(T lo, T hi) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        m.data[j] = data[j].Normalize(lo, hi);
      }

      return m;
    }
 
    jmatrix_t<R, C, double> Clip(T lo, T hi) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        m.data[j] = data[j].Clip(lo, hi);
      }

      return m;
    }
 
    jmatrix_t<R, C, T> Pow(double e)
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::pow(data[j][i], e);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> Sqrt()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::sqrt(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> Abs()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::abs(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Sin()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::sin(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Cos()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::cos(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Tan()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::tan(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> SinH()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::sinh(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> CosH()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::cosh(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> TanH()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::tanh(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Exp()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::exp(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Log()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::log(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Log2()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::log2(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Log10()
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)std::log10(data[j][i]);
        }
      }

      return m;
    }

    jmatrix_t<R, C, T> Conjugate()
    {
      static_assert(is_complex<T>::value, "T must be std::complex<U>");

      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        m.data[j] = data[j].Conjugate();
      }

      return m;
    }

    jmatrix_t<R, C, T> Hermitian()
    {
      return Transpose().Conjugate();
    }

    template<typename U> friend jmatrix_t<R, C, typename std::common_type<T, U>::type> operator+(const U &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (param + thiz.data[j][i]);
        }
      }

      return m;
    }
    
    template<typename U> friend jmatrix_t<R, C, typename std::common_type<T, U>::type> operator-(const U &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (param - thiz.data[j][i]);
        }
      }

      return m;
    }
    
    template<typename U> friend jmatrix_t<R, C, typename std::common_type<T, U>::type> operator*(const U &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (param*thiz.data[j][i]);
        }
      }

      return m;
    }
    
    template<typename U> friend jmatrix_t<R, C, typename std::common_type<T, U>::type> operator/(const U &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, typename std::common_type<T, U>::type> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (param/thiz.data[j][i]);
        }
      }

      return m;
    }
    
    friend std::ostream & operator<<(std::ostream& out, const jmatrix_t<R, C, T> &param)
    {
      for (size_t j=0; j<R; j++) {
        out << param.data[j];

        if (j != (R - 1)) {
          out << "\n";
        }
      }

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jmatrix_t<R, C, T> &param) 
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          is >> param(j, i);
        }
      }

      return is;
    }

  };

}

#endif

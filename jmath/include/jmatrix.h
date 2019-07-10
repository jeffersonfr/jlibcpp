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

namespace jmath {

template<size_t R, size_t C, typename T = float, typename = typename std::enable_if<std::is_arithmetic<T>::value || is_complex<T>::value, T>::type>
  struct jmatrix_t {
    jvector_t<C, T> data[R];

    static jmatrix_t<R, C, T> Identity()
		{
      jmatrix_t<R, C, T> m;

			m = 0;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if (j == i) {
          	m.data[i][j] = 1;
					}
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

    template<typename U> jmatrix_t<R, C, T> operator+(const U &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(data[j][i] + param);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, T> operator-(const U &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(data[j][i] - param);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, T> operator*(const U &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(data[j][i]*param);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, T> operator/(const U &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(data[j][i]/param);
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

    template<typename U> jmatrix_t<R, C, T> operator+(const jmatrix_t<R, C, U> &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(data[j][i] + param.data[j][i]);
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, T> operator-(const jmatrix_t<R, C, U> &param) const
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(data[j][i] - param.data[j][i]);
        }
      }

      return m;
    }
    
    template<size_t R1 = C, size_t C1, typename U> jmatrix_t<R, C1, T> operator*(const jmatrix_t<R1, C1, U> &param) const
    {
      jmatrix_t<R, C1, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C1; i++) {
          m.data[j][i] = Row(j).Scalar(param.Col(i));
        }
      }

      return m;
    }
    
    template<typename U> jmatrix_t<R, C, T> operator/(const jmatrix_t<R, C, U> &param) const
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
    
    size_t Size() const
    {
      return R*C;
    }

    jvector_t<C, T> Row(size_t n) const
    {
      if (n >= R) {
        throw jexception::OutOfBoundsException("Row index is out of bounds");
      }

      return data[n];
    }

    jvector_t<R, T> Col(size_t n) const
    {
      if (n >= C) {
        throw jexception::OutOfBoundsException("Row index is out of bounds");
      }

      jvector_t<R, T> v;

      for (size_t j=0; j<R; j++) {
        v.data[j] = data[j][n];
      }

      return v;
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

    T Mean() const
		{
      T count = 0;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          count = count + data[i][j];
        }
      }

      return count/(R*C);
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
      return Determinant() == 0.0f;
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
				return 0.0f;
			}

      double 
				det = 1.0f;
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
          return 0.0f;
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

    jmatrix_t<C, R, T> & Transpose()
    {
      for (size_t j=0; j<R/2; j++) {
        for (size_t i=0; i<C/2; i++) {
          data[i][j] = data[j][i];
        }
      }

      return *this;
    }

    T Trace() const
		{
      T sum = 0;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
					if (j == i) {
          	sum = sum + data[i][j];
					}
        }
      }

      return sum;
		}

    jmatrix_t<R, C, T> Adjoint() const
    {
      static_assert(R == C, "Determinant needs a square matrix");

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

      if (d == 0.0f) {
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
      T norm = 0.0f;

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
 
    jmatrix_t<R, C, T> & Pow(double e)
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::pow(data[j][i], e);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Sqrt()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::sqrt(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Abs()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::abs(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Sin()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::sin(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Cos()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::cos(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Tan()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::tan(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & SinH()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::sinh(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & CosH()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::cosh(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & TanH()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::tanh(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Exp()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::exp(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Log()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::log(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Log2()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::log2(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Log10()
    {
      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          data[j][i] = (T)std::log10(data[j][i]);
        }
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Conjugate()
    {
      static_assert(is_complex<T>::value, "T != std::complex<U>");

      for (size_t j=0; j<R; j++) {
        data[j] = data[j].Conjugate();
      }

      return *this;
    }

    jmatrix_t<R, C, T> & Hermitian()
    {
      static_assert(is_complex<T>::value, "T != std::complex<U>");

      return Transpose().Conjugate();
    }

    friend jmatrix_t<R, C, T> operator+(const T &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(param + thiz.data[j][i]);
        }
      }

      return m;
    }
    
    friend jmatrix_t<R, C, T> operator-(const T &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(param - thiz.data[j][i]);
        }
      }

      return m;
    }
    
    friend jmatrix_t<R, C, T> operator*(const T &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(param*thiz.data[j][i]);
        }
      }

      return m;
    }
    
    friend jmatrix_t<R, C, T> operator/(const T &param, const jmatrix_t<R, C, T> &thiz)
    {
      jmatrix_t<R, C, T> m;

      for (size_t j=0; j<R; j++) {
        for (size_t i=0; i<C; i++) {
          m.data[j][i] = (T)(param/thiz.data[j][i]);
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

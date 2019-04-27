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

#include <stdlib.h>

namespace jmath {

template<class T> class Matrix : public virtual jcommon::Object {

  public:
    T *_m;
    T **_v;
    int _clo;
    int _chi;
    int _ncol;
    int _rlo;
    int _rhi;
    int _nrow;
    int _nelem;

    /** 
     * \brief Construct new  Matrix of dimension 0 
     *
     */
    Matrix():
      jcommon::Object() 
    {
      jcommon::Object::SetClassName("jmath::Matrix");

      _v = nullptr;
      _m = nullptr;
      _clo = 0;
      _chi = 0;
      _rlo = 0;
      _rhi = 0;
      _nrow = 0;
      _ncol = 0;
    }

    /**
     * \brief Copy constructor - copy by value
     *
     */
    Matrix(const Matrix &V):
      jcommon::Object() 
    {
      jcommon::Object::SetClassName("jmath::Matrix");

      _v = nullptr;
      _m = nullptr;
      _clo = 0;
      _chi = 0;
      _rlo = 0;
      _rhi = 0;
      _nrow = 0;
      _ncol = 0;
      (*this) = V;
    }

    /** 
     * \brief Construct new  matrix without initialization
     *
     */
    Matrix(int rlo, int rhi, int clo, int chi):
      jcommon::Object() 
    {
      jcommon::Object::SetClassName("jmath::Matrix");

      NewMatrix(rlo, rhi, clo, chi);
    }

    /** 
     * \brief Construct new  Matrix without initialization 
     *
     */
    Matrix(int rhi, int chi):
      jcommon::Object() 
    {
      jcommon::Object::SetClassName("jmath::Matrix");

      // TODO:: assume matrix indices begin at 1
      NewMatrix(1, rhi, 1, chi);
    }

    /** 
     * \brief Construct new  matrix with elements 'a'
     *
     */
    Matrix(int rlo, int rhi, int clo, int chi, T a):
      jcommon::Object() 
    {
      jcommon::Object::SetClassName("jmath::Matrix");

      NewMatrix(rlo, rhi, clo, chi);

      *this = a;
    }

    virtual ~Matrix() 
    {
      delete [] _m;
      delete [] (_v + _rlo);
    }

    void NewMatrix(int rlo, int rhi, int clo, int chi) 
    {
      if ( (rlo > rhi) || (clo > chi) )
        throw jexception::OutOfBoundsException("Matrix: Attempt to create Matrix with low index > high index");

      this->_rlo = rlo;
      this->_rhi = rhi;
      this->_clo = clo;
      this->_chi = chi;
      _nrow = rhi-rlo+1; 
      _ncol = chi-clo+1;
      _nelem = _nrow*_ncol;
      _m = new T[_nelem];
      _v = new T *[_nrow];
      for (int i=0; i<_nrow; i++) 
        _v[i] = _m + i*_ncol - clo;
      _v -= rlo;
    }

    void SetSize(int rlo, int rhi, int clo, int chi) 
    {
      delete [] _m;
      delete [] (_v+this->_rlo);
    
      NewMatrix(rlo, rhi, clo, chi);
    }

    void SetSize(int rhi, int chi) 
    {
      // TODO:: assume matrix indices start at 1
      delete [] _m;
      delete [] (_v+this->_rlo);

      NewMatrix(1, rhi, 1, chi);
    }

    void SetSize(const Matrix<T> &M) 
    {
      SetSize(M._rlo, M._rhi, M._clo, M._chi);
    }

    /**
     * \brief returns a new Matrix with same values as *this, but with type of "other" matrix.
     *
     */
    template<class T2> Matrix<T2> Convert(Matrix<T2> & other) 
    {
      Matrix<T2> new_matrix(_rlo, _rhi, _clo, _chi);

      for (int i = _rlo; i <= _rhi; i++)
        for (int j = _clo; j <= _chi; j++)
          new_matrix(i, j) = _v[i][j];

      return new_matrix;
    }

    void NewOffset(int newrlo, int newclo) 
    {
      _v -= (newrlo - _rlo);
      _rlo = newrlo;
      _rhi = newrlo + _nrow - 1;
    
      for (int i=_rlo; i<=_rhi; i++) 
        _v[i] -= (newclo - _clo);
      
      _clo = newclo;
      _chi = newclo + _ncol - 1;
    }

    int LowColumn() const 
    {
      return _clo;
    }

    int HighColumn() const 
    {
      return _chi;
    }

    int GetColumnSize() const 
    {
      return _ncol;
    }

    int LowRow() const 
    {
      return _rlo;
    }

    int HighRow() const 
    {
      return _rhi;
    }

    int GetRowSize() const 
    {
      return _nrow;
    }

    int GetSize() const 
    {
      return _nelem;
    }

    /** 
     * \brief Reference element  (i,j) 
     *
     */
    T &operator()(int i, int j) 
    {
      if ((i < _rlo) || (i > _rhi) || (j < _clo) || (j > _chi))
        throw jexception::OutOfBoundsException("Matrix::operator(): Element reference out of bounds");

      return _v[i][j];
    }

    /** 
     * \brief Reference element  (i,j) 
     *
     */ 
    const T &operator()(int i, int j) const 
    {
      if ((i < _rlo) || (i > _rhi) || (j < _clo) || (j > _chi))
        throw jexception::OutOfBoundsException("Matrix::operator(): Element reference out of bounds");

      return _v[i][j];
    } 

    /** 
     * \brief Return new  Matrix with range ( subrlo,  subrhi, subclo,  subchi) 
     *
     */
    Matrix<T> SubMatrix(int subrlo, int subrhi, int subclo, int subchi) 
    {
      if ( (subrlo < _rlo) || (subrhi > _rhi) || (subclo < _clo) || (subchi > _chi) )
        throw jexception::OutOfBoundsException("Matrix::submatrix: Invalid range selected");

      Matrix<T> sub;
      int row_range = subrhi-subrlo;
      int col_range = subchi-subclo;
      sub.NewMatrix(_rlo, _rlo+row_range, _clo, _clo+col_range);

      for (int i=sub._rlo; i<=sub._rhi; i++) 
        for (int j=sub._clo; j<=sub._chi; j++) 
          sub(i,j) = _v[i+(subrlo-_rlo)][j+(subclo-_clo)];
      
      return sub;
    }

    Vector<T> Row(int r) 
    {
      if ( (r < _rlo) || (r > _rhi) )
        throw jexception::OutOfBoundsException("Matrix::row: Invalid row selected");

      Vector<T> sub;

      sub.NewVector(_clo, _chi);
      
      for (int i=sub._vlo; i<=sub._vhi; i++)
        sub[i] = _v[r][i];
      
      return sub;
    }

    Vector<T> Column(int c) 
    {
      if ( (c < _clo) || (c > _chi) )
        throw jexception::OutOfBoundsException("Matrix::col: Invalid column selected");

      Vector<T> sub;

      sub.NewVector(_rlo, _rhi);
    
      for (int i=sub._vlo; i<=sub._vhi; i++)
        sub[i] = _v[i][c];
      
      return sub;
    } 

    Vector<T> Pack() 
    {
      Vector<T> packed;

      packed.NewVector(1, _nelem);
      
      T *v = packed._v;
    
      for (int i=0; i<_nelem; i++) 
        v[i+1] = _m[i];
      
      return packed;
    }

    T ** Raw() const 
    {
      return (_v+_rlo);
    }

    /** 
     * \brief Assignment by value to scalar 
     *
     */
    Matrix<T> &operator=(T a) 
    {
      for (int i=0; i<_nelem; i++) 
        _m[i] = a;
    
      return *this;
    }

    /** 
     * \brief Assignment by value to Matrix 
     *
     */
    Matrix<T> &operator=(const Matrix<T> &M) 
    {
      delete [] _m;
      delete [] (_v+_rlo);
    
      NewMatrix(M._rlo, M._rhi, M._clo, M._chi);
      
      for (int i=0; i<M._nelem; i++) 
        _m[i] = M._m[i];
      
      return *this;
    }

    /** 
     * \brief Assignment by value to 1DP matrix 
     *
     */
    Matrix<T> &operator=(T *m) 
    {
      if (this->_m == nullptr) 
        throw jexception::NullPointerException("Matrix::operator=(T *m):  Attempt to assign by value to a C array, when no dimensions have been allocated for the Matrix");
      
      for (int i=0; i<_nelem; i++) 
        this->_m[i] = m[i];

      return *this;
    }

    /** 
     * \brief Assignment by value to 2DP matrix 
     *
     */
    Matrix<T> &operator=(T **m) 
    {
      if (this->_m == nullptr) 
        throw jexception::NullPointerException("Matrix::operator=(T **m):  Attempt to assign by value to a C array, when no dimensions have been allocated for the Matrix");

      for (int i=0; i<_nelem; i++) 
        this->_m[i] = *(*m+i);

      for (int i=_rlo; i<_rlo+_nrow; i++) 
        this->_v[i] = this->_m + (i-_rlo)*_ncol - _clo;
      
      return *this;
    }

    /** 
     * \brief Matrix addition. 
     *
     */
    Matrix<T> &operator+=(const Matrix<T> &M) 
    {
      if (M._nelem != _nelem)
        throw jexception::InvalidArgumentException("Matrix::operator+=:  Attempt to add nonconformant Matrix");

      T *m2 = M._m;

      for (int i=0; i<_nelem; i++) 
        _m[i] += m2[i];
      
      return *this;
    }

    /** 
     * \brief Matrix subtraction 
     *
     */
    Matrix<T> &operator-=(const Matrix<T> &M) 
    {
      if (M._nelem != _nelem)
        throw jexception::InvalidArgumentException("Matrix::operator-=:  Attempt to subtract nonconformant Matrix");

      T *m2 = M._m;
      
      for (int i=0; i<_nelem; i++) 
        _m[i] -= m2[i];
      
      return *this;
    }

    /** 
     * \brief Addition of scalar 
     *
     */
    Matrix<T> &operator+=(const T &e) 
    {
      for (int i=0; i<_nelem; i++) 
        _m[i] += e;

      return *this;
    }

    /**  
     * \brief Subtraction of scalar 
     *
     */
    Matrix<T> &operator-=(const T &e) {
      for (int i=0; i<_nelem; i++) 
        _m[i] += e;

      return *this;
    }

    /** 
     * \brief Multiplication by scalar
     *
     */
    Matrix<T> &operator*=(const T &e) 
    {
      for (int i=0; i<_nelem; i++) 
        _m[i] *= e;

      return *this;
    }

    /** 
     * \brief Division by scalar 
     *
     */
    Matrix<T> &operator/=(const T &e) 
    {
      for (int i=0; i<_nelem; i++) 
        _m[i] /= e;

      return *this;
    }
};

template<class T> Matrix<T> operator-(const Matrix<T> &M) 
{
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m,
    *m = M._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = -m[i];
  
  return M2;
}

template<class T> Matrix<T> operator+(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("operator+(Matrix, Matrix):  Attempt to add nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;
  Matrix<T> M(M1._rlo, M1._rhi, M1._clo, M1._chi);
  T *m = M._m;

  for (int i=0; i<M1._nelem; i++) 
    m[i] = m1[i] + m2[i];
  
  return M;
}

template<class T> Matrix<T> operator+(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i] + e;
  
  return M2;
}

template<class T> Matrix<T> operator+(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = e + m[i];
  
  return M2;
}

template<class T> Matrix<T> operator-(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("operator-(Matrix, Matrix):  Attempt to subtract nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;
  Matrix<T> M(M1._rlo, M1._rhi, M1._clo, M1._chi);
  T *m = M._m;

  for (int i=0; i<M1._nelem; i++) 
    m[i] = m1[i] - m2[i];
  
  return M;
}

template<class T> Matrix<T> operator-(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i] - e;
  
  return M2;
}

template<class T> Matrix<T> operator-(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = e - m[i];
  
  return M2;
}

template<class T> Matrix<T> operator*(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i] * e;
  
  return M2;
}

template<class T> Matrix<T> operator*(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = e * m[i];
  
  return M2;
}

template<class T> Matrix<T> operator/(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i] / e;
  
  return M2;
}

template<class T> Matrix<T> operator/(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = e / m[i];
  
  return M2;
}

template<class T> Matrix<T> operator/(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  return DivideMatrix(M1, M2);
}

template<class T> Matrix<T> AddMatrix(const Matrix<T> &M, const T &e) 
{
  return (M + e);
}

template<class T> Matrix<T> AddMatrix(const T &e, const Matrix<T> &M) 
{
  return (e + M);
}

template<class T> Matrix<T> AddMatrix(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  return (M1 + M2);
}

template<class T> Matrix<T> SubMatrix(const Matrix<T> &M, const T &e) 
{
  return (M - e);
}

template<class T> Matrix<T> SubMatrix(const T &e, const Matrix<T> &M) 
{
  return (e - M);
}

template<class T> Matrix<T> SubMatrix(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  return (M1 - M2);
}

template<class T> Matrix<T> MultiplyMatrix(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("elem_mul(Matrix, Matrix):  Attempt to element multiply nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;
  Matrix<T> M(M1._rlo, M1._rhi, M1._clo, M1._chi);
  T *m = M._m;

  for (int i=0; i<M1._nelem; i++) 
    m[i] = m1[i] * m2[i];
  
  return M;
}

template<class T> Matrix<T> MultiplyMatrix(const Matrix<T> &M, const T &e) 
{
  return (M * e);
}

template<class T> Matrix<T> MultiplyMatrix(const T &e, const Matrix<T> &M) 
{
  return (e * M);
}

template<class T> Matrix<T> DivideMatrix(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("elem_div(Matrix, Matrix):  Attempt to element divide nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;
  Matrix<T> M(M1._rlo, M1._rhi, M1._clo, M1._chi);
  T *m = M._m;

  for (int i=0; i<M1._nelem; i++) 
    m[i] = m1[i] / m2[i];
  
  return M;
}

template<class T> Matrix<T> DivideMatrix(const Matrix<T> &M, const T &e) 
{
  return (M / e);
}

template<class T> Matrix<T> DivideMatrix(const T &e, const Matrix<T> &M) 
{
  return (e / M);
}

template<class T> T Min(const Matrix<T> &M) 
{
  T *m = M._m,
    low = m[0];

  for (int i=1; i<M._nelem; i++) 
    if (low > m[i]) 
      low = m[i];
  
  return low;
}

template<class T> Matrix<T> Min(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("min(Matrix, Matrix):  Attempt to extract minimum from nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;
  Matrix<T> M(M1._rlo, M1._rhi, M1._clo, M1._chi);
  T *m = M._m;

  for (int i=0; i<M1._nelem; i++) 
    if (m1[i] <= m2[i]) 
      m[i] = m1[i];
    else 
      m[i] = m2[i];
  
  return M;
}

template<class T> Matrix<T> Min(const T &e, const Matrix<T> &M2) 
{
  T *m2 = M2._m;
  Matrix<T> M(M2._rlo, M2._rhi, M2._clo, M2._chi);
  T *m = M._m;

  for (int i=0; i<M2._nelem; i++) 
    if (e <= m2[i]) 
      m[i] = e;
    else 
      m[i] = m2[i];
  
  return M;
}

template<class T> Matrix<T> Min(const Matrix<T> &M2, const T &e) 
{
  T *m2 = M2._m;
  Matrix<T> M(M2._rlo, M2._rhi, M2._clo, M2._chi);
  T *m = M._m;
  
  for (int i=0; i<M2._nelem; i++) 
    if (m2[i] <= e) 
      m[i] = m2[i];
    else 
      m[i] = e;

  return M;
}

template<class T> T Max(const Matrix<T> &M) 
{
  T *m = M._m,
    hi = m[0];

  for (int i=1; i<M._nelem; i++)
    if (hi < m[i]) 
      hi = m[i];
  
  return hi;
}

template<class T> Matrix<T> Max(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("min(Matrix, Matrix):  Attempt to extract minimum from nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;
  Matrix<T> M(M1._rlo, M1._rhi, M1._clo, M1._chi);
  T *m = M._m;
  
  for (int i=0; i<M1._nelem; i++) 
    if (m1[i] >= m2[i]) 
      m[i] = m1[i];
    else 
      m[i] = m2[i];
  
  return M;
}

template<class T> Matrix<T> Max(const T &e, const Matrix<T> &M2) 
{
  T *m2 = M2._m;
  Matrix<T> M(M2._rlo, M2._rhi, M2._clo, M2._chi);
  T *m = M._m;

  for (int i=0; i<M2._nelem; i++) 
    if (e >= m2[i]) 
      m[i] = e;
    else 
      m[i] = m2[i];
  
  return M;
}

template<class T> Matrix<T> Max(const Matrix<T> &M2, const T &e) 
{
  T *m2 = M2._m;
  Matrix<T> M(M2._rlo, M2._rhi, M2._clo, M2._chi);
  T *m = M._m;

  for (int i=0; i<M2._nelem; i++) 
    if (m2[i] >= e) 
      m[i] = m2[i];
    else 
      m[i] = e;

  return M;
}

template<class T> bool operator==(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;

  for (int i=0; i<M1._nelem; i++)
    if (m1[i] != m2[i]) 
      return false;
  
  return true;
}

template<class T> bool operator==(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (m[i] != e) 
      return false;
  
  return true;
}

template<class T> bool operator==(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (e != m[i]) 
      return false;
  
  return true;
}

template<class T> bool operator<(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;
  
  for (int i=0; i<M1._nelem; i++)
    if (m1[i] >= m2[i]) 
      return false;
  
  return true;
}

template<class T> bool operator<(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (m[i] >= e) 
      return false;
  
  return true;
}

template<class T> bool operator<(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (e >= m[i]) 
      return false;
  
  return true;
}

template<class T> bool operator<=(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;

  for (int i=0; i<M1._nelem; i++)
    if (m1[i] > m2[i]) 
      return false;
  
  return true;
}

template<class T> bool operator<=(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (m[i] > e) 
      return false;
  
  return true;
}

template<class T> bool operator<=(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (e > m[i]) 
      return false;
  
  return true;
}

template<class T> bool operator>=(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    jexception::InvalidArgumentException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;

  for (int i=0; i<M1._nelem; i++)
    if (m1[i] < m2[i]) 
      return false;
  
  return true;
}

template<class T> bool operator>=(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (m[i] < e) 
      return false;
  
  return true;
}

template<class T> bool operator>=(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (e < m[i]) 
      return false;
  
  return true;
}

template<class T> bool operator>(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._nelem != M2._nelem)
    throw jexception::InvalidArgumentException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

  T *m1 = M1._m,
    *m2 = M2._m;

  for (int i=0; i<M1._nelem; i++)
    if (m1[i] <= m2[i]) 
      return false;
  
  return true;
}

template<class T> bool operator>(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (m[i] <= e) 
      return false;
  
  return true;
}

template<class T> bool operator>(const T &e, const Matrix<T> &M) 
{
  T *m = M._m;

  for (int i=0; i<M._nelem; i++)
    if (e <= m[i]) 
      return false;
  
  return true;
}

template<class T> Matrix<T> Transpose(const Matrix<T> &M) 
{
  Matrix<T> M2(M._clo, M._chi, M._rlo, M._rhi);
  T **v2 = M2._v,
    **v = M._v;
  int clo = M._rlo,
      chi = M._rhi,
      rlo = M._clo,
      rhi = M._chi;

  for (int i=rlo; i<=rhi; i++) 
    for (int j=clo; j<=chi; j++) 
      v2[i][j] = v[j][i];
  
  return M2;
}

// Decomposes input Matrix to LU equivalent, replacing
// the input Matrix by both L and U.  In order to be useful in
// solving linear equations, this function implements partial pivoting.
// The Vector 'exchanges' contains the row exchange information, to
// be used in conjunction with solve_lu_inplace().
//
// TODO: error checking
template<class T> void LUDecompositionInplace(Matrix<T> & M, Vector<int> & exchanges) 
{
  // the number of rows/cols.
  int n = M._nrow;
  // reset M's offset for easy management.
  int rlo_orig = M._rlo;
  int clo_orig = M._clo;
  M.NewOffset(1, 1);
  exchanges.SetSize(n);
  int vlo_orig = exchanges._vlo;
  exchanges.NewOffset(1);

  // implement implicit pivoting using a scaling factor for each row.  Each row 
  // will be scaled to the largest value in each column of U.
  Vector<T> scale(1, n);

  // populate the scale Vector.
  for (int i = 1; i <= n; i++) {
    T largest = 0;
  
    for (int j = 1; j <= n; j++) {
      T mag = abs(M(i, j));
      if (mag > largest) 
        largest = mag;
    }

    if (largest == 0) {
      throw jexception::InvalidArgumentException("Attempt to factor a singular matrix");
    }

    scale[i] = 1.0 / largest;
  }

  // do the L-U factorization.  This is Crout's method with partial pivoting.
  int max_index = 1;
  for (int j = 1; j <= n; j++) {
    // populate the U part of the result for this column.
    for (int i = 1; i < j; i++) {
      T sum = M(i, j);
      
      for (int k = 1; k < i; k++) 
        sum -= M(i, k) * M(k, j);
      
      M(i, j) = sum;
    }

    // populate the L part fo the result for this column.
    T largest = 0;
    for (int i = j; i <= n; i++) {
      T sum = M(i, j);
      
      for (int k = 1; k < j; k++) 
        sum -= M(i, k) * M(k, j);
      
      M(i, j) = sum;

      // is this a new potential pivot element?
      T mag = abs(sum) * scale[i];
      if (mag >= largest) {
        largest = mag;
        max_index = i;
      }
    }

    // Interchange rows if necessary, to get the big pivot in  the diagonal.
    if (j != max_index) {

      // exchange rows j and max_index.  This will put 'largest' on the diagonal.
      for (int k = 1; k <= n; k++) {
        T tmp = M(max_index, k);
        M(max_index, k) = M(j, k);
        M(j, k) = tmp;
      }

      // don't forget to exchange the rows in the scale Vector, too!
      scale[max_index] = scale[j];
    }

    // test for singular matrix.
    if (M(j,j) == 0) 
      throw jexception::InvalidArgumentException("Attempt to factor a singular matrix");

    // record the exchange. ("No exchange" is noted by having exchanges[i] == i.)
    exchanges[j] = max_index;

    // now we can divide by the pivot to get the final values for this column.
    if (j != n) {
      T tmp = 1.0/M(j, j);
  
      for (int i = j+1; i <= n; i++) 
        M(i, j) *= tmp;
    }
  }

  // Restore M and exchanges to their original index offsets.
  M.NewOffset(rlo_orig, clo_orig);
  exchanges.NewOffset(vlo_orig);
}

// A nicer version of lu_decomp that doesn't destroy the user's
// original Matrix.
//
// Returns 3 Matrices: L, U, and a permutation matrix P such that
// PM = LU.
//
// The user doesn't have to provide Matrix objects of any particular
// size or content ... this function will size and populate the return
// objects completely.
template<class T> void LUDecomposition(Matrix<T> M, Matrix<int> & P, Matrix<T> & L, Matrix<T> & U) 
{
  // standardize the row and column offsets. remember input offsets, so output can match.
  int rlo_orig = M._rlo;
  int clo_orig = M._clo;
  M.NewOffset(1, 1);

  // call lu_decomp_inplace.
  Vector<int> exchanges(1, M._nrow);
  LUDecompositionInplace(M, exchanges);

  // build return "U" matrix.
  int n = M._nrow;
  U.NewMatrix(1, n, 1, n);
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) {
      if (j >= i) 
        U(i, j) = M(i, j);
      else 
        U(i, j) = 0;
    }
  }
  
  U.NewOffset(rlo_orig, clo_orig);

  // build return "L" matrix.
  L.NewMatrix(1, n, 1, n);
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) {
      if (j > i) 
        L(i, j) = 0;
      else if (j == i) 
        L(i, j) = 1;
      else 
        L(i, j) = M(i, j);
    }
  }

  L.NewOffset(rlo_orig, clo_orig);

  // build return "P" matrix.
  P.NewMatrix(1, n, 1, n);
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) {
      if (i == j) 
        P(i, j) = 1;
      else 
        P(i, j) = 0;
    }
  }

  for (int i = 1; i <= n; i++) {
    int ex = exchanges(i);
    if (i != ex) {
      // exchange rows i and exchanges(i).
      for (int k = 1; k <= n; k++) {
        int tmp = P(ex, k);
        
        P(ex, k) = P(i, k);
        P(i, k) = tmp;
      }
    }
  }

  P.NewOffset(rlo_orig, clo_orig);
}

// Solves the given linear equations using L-U decomposition.
// This function requires an input Matrix in the form produced by
// lu_decomp_inplace(), along with the associated row-exchange Vector.
//
// This function overwrites the input Vector 'b' with the solution.
//
// Note that this function takes advantage of zero elements within 'b', so
// it is appropriate for use with computing the inverse, for example.
//
// TODO: error checking
template<class T> void LUInplaceSolve(Matrix<T> & LU, Vector<int> & exchanges, Vector<T> & b)
{
  int n = LU._nrow;
  // reset the offsets to be 1-based, for convenience during the algorithm.
  int rlo_orig = LU._rlo;
  int clo_orig = LU._clo;
  int vlo_orig = exchanges._vlo;
  int b_vlo_orig = b._vlo;
  LU.NewOffset(1, 1);
  exchanges.NewOffset(1);
  b.NewOffset(1);

  // 'ii' is used to detect zero elements in 'b'.
  int ii = 0;

  // do the forward substitution part.
  for (int i = 1; i <= n; i++) {
    int ip = exchanges[i];
    
    T sum = b[ip];
    b[ip] = b[i];
    
    if (ii) {
      for (int j = ii; j <= i-1; j++) 
        sum -= LU(i, j)*b[j];
    } else if (sum) {
      ii = i;
    }

    b[i] = sum;
  }

  // do the backward substitution part.
  for (int i = n; i >= 1; i--) {
    T sum = b[i];
    
    for (int j = i+1; j <= n; j++) 
      sum -= LU(i, j) * b[j];
    
    b[i] = sum / LU(i, i);
  }

  // restore the offsets.
  LU.NewOffset(rlo_orig, clo_orig);
  exchanges.NewOffset(vlo_orig);
  b.NewOffset(b_vlo_orig);
}

// Solves the given system of equations and returns the
// solution Vector.  This function just defers to solve_lu_inplace(), but
// doesn't overwrite any input objects.
template<class T> Vector<T> Solve(Matrix<T> M, Vector<T> b) 
{
  // Call solve_lu_inplace().
  Vector<int> exchanges(1, M._nrow);
  LUDecompositionInplace(M, exchanges);
  LUInplaceSolve(M, exchanges, b);

  // Return resulting vector.
  return b;
}

// computes the inverse of the input matrix using LU-decomposition.
//
// TODO: error checking
template<class T> Matrix<T> Inverse(Matrix<T> M) 
{
  // remember offsets so the output can match the input.
  int rlo_orig = M._rlo;
  int clo_orig = M._clo;
  int n = M._nrow;
  M.NewOffset(1, 1);
  Vector<int> exchanges(1, n);
  LUDecompositionInplace(M, exchanges);

  // Prepare (an empty) resultant matrix.
  Matrix<T> result(M._nrow, M._ncol);

  // Now call solve_lu_inplace() for each column of M^-1.
  for (int j = 1; j <=n; j++) {
    // build the identity vector for this column.
    Vector<T> id(1, n);
    
    for (int i = 1; i <= n; i++) 
      id[i] = 0.0;

    id[j] = 1.0;

    // solve the linear equation.
    LUInplaceSolve(M, exchanges, id);

    // copy the resultant column into the result matrix.
    for (int i = 1; i <= n; i++) 
      result(i, j) = id[i];
  }

  // return the inverse.  Make the offset match the input Matrix.
  result.NewOffset(rlo_orig, clo_orig);

  return result;
}

// \brief finds the determinant of a non-singular matrix. The
// determinant is the product of the diagonal elements of the U-Matrix
// returned by LU-decomp.
//
// Here we use lu_decomp_inplace as an efficient mechanism for 
// computing the determinant.
template<class T> T Determinant(Matrix<T> M) 
{
  M.NewOffset(1, 1);
  int n = M._nrow;
  Vector<int> exchanges(1, n);
  
  try {
    LUDecompositionInplace(M, exchanges);

    // Now get the product of the U-matrix main diagonal.
    T result = 1.0;
    for (int i = 1; i <= n; i++) {
      result *= M(i, i);
    }

    // Now we just need to figure out the sign of the determinant.
    T factor = 1.0;
    for (int i = 1; i <= n; i++) {
      if (exchanges[i] != i) 
        factor *= -1.0;
    }

    return result * factor;
  } catch (const char* error) {
  }

  // if lu_decomp() discovers a singular matrix, det = 0.
  return 0;
}

template<class T> Matrix<T> operator*(const Matrix<T> &M1, const Matrix<T> &M2) 
{
  if (M1._ncol != M2._nrow)
    throw jexception::InvalidArgumentException("operator*(Matrix, Matrix):  Attempt to multiply non-conformant matrices.");
  if ( (M1._rlo != M2._rlo) || (M1._clo != M2._clo) ) 
    throw jexception::InvalidArgumentException("operator*(Matrix, Matrix):  Attempt to multiply matrices with different starting indices.");

  Matrix<T> M3(M1._rlo, M1._rhi, M2._clo, M2._chi);
  T **v1 = M1._v;
  T **v2 = M2._v;
  T **v3 = M3._v;
  int imin = M1._rlo;
  int imax = M1._rhi;
  int jmin = M2._clo;
  int jmax = M2._chi;
  int kmin = M1._clo;
  int kmax = M1._chi;

  for (int i=imin; i<=imax; i++) {
    for (int j=jmin; j<=jmax; j++) {
      v3[i][j] = 0;
      for (int k=kmin; k<=kmax; k++) 
        v3[i][j] += v1[i][k] * v2[k][j];
    }
  }

  return M3;
}

template<class T> Vector<T> operator*(const Matrix<T> &M, const Vector<T> &V) 
{
  if (M._ncol != V._nelem)
    throw jexception::InvalidArgumentException("operator*(Matrix, Vector):  Attempt to multiply non-conformant matrix and vector.");
  if (M._clo != V._vlo) 
    throw jexception::InvalidArgumentException("operator*(Matrix, Vector):  Attempt to multiply matrix and vector with different starting indices.");

  Vector<T> V2(M._rlo, M._rhi);
  T **mv = M._v;
  T *vv = V._v;
  T *v2 = V2._v;
  int imin = M._rlo;
  int imax = M._rhi;
  int jmin = M._clo;
  int jmax = M._chi;

  for (int i=imin; i<=imax; i++) {
    v2[i] = 0;
  
    for (int j=jmin; j<=jmax; j++)  
      v2[i] += mv[i][j] * vv[j];
  }
  
  return V2;
}

template<class T> Vector<T> operator*(const Vector<T> &V, const Matrix<T> &M) 
{
  if (M._nrow != V._nelem)
    throw jexception::InvalidArgumentException("operator*(Vector, Matrix):  Attempt to multiply non-conformant vector and matrix.");
  if (M._rlo != V._vlo) 
    throw jexception::InvalidArgumentException("operator*(Vector, Matrix):  Attempt to multiply vector and matrix with different starting indices.");

  Vector<T> V2(M._clo, M._chi);
  T **mv = M._v;
  T *vv = V._v;
  T *v2 = V2._v;
  int imin = M._clo;
  int imax = M._chi;
  int jmin = M._rlo;
  int jmax = M._rhi;

  for (int i=imin; i<=imax; i++) {
    v2[i] = 0;
  
    for (int j=jmin; j<=jmax; j++)  
      v2[i] += vv[j] * mv[j][i];
  }
  
  return V2;
}

template<class T> T Trace(const Matrix<T> &M) 
{
  if (M._ncol != M._nrow)
    throw jexception::InvalidArgumentException("trace(Matrix):  Attempt to take trace of non-square matrix");

  T val = 0;
  T *m  = M._m;
  int inc = M._nrow;
  int imax = M._nelem;

  for (int i=0; i<imax; i+=inc+1) 
    val += m[i];
  
  return val;
}

template<class T> T Sum(const Matrix<T> &M) 
{
  T val = 0;
  T *m = M._m;

  for (int i=0; i<M._nelem; i++) 
    val += m[i];
  
  return val;
}

template<class T> Matrix<T> Pow(const Matrix<T> &M, const T &e) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = pow(m[i], e);
  
  return M2;
}

template<class T> Matrix<T> Pow(const Matrix<T> &M, int e) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = pow(m[i], e);
  
  return M2;
}

template<class T> Matrix<T> Abs(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = abs(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Sqrt(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = sqrt(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Sqr(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i]*m[i];
  
  return M2;
}

template<class T> Matrix<T> AbsSqr(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = abs(m[i])*abs(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Cube(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i]*m[i]*m[i];
  
  return M2;
}

template<class T> Matrix<T> Sin(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = sin(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Cos(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = cos(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Tan(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = tan(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Sinh(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = sinh(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Cosh(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = cosh(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Tanh(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;
  
  for (int i=0; i<M._nelem; i++) 
    m2[i] = tanh(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Exp(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = exp(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Ln(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = log(m[i]);
  
  return M2;
}

template<class T> Matrix<T> Log10(const Matrix<T> &M) 
{
  T *m = M._m;
  Matrix<T> M2(M._rlo, M._rhi, M._clo, M._chi);
  T *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = log10(m[i]);
  
  return M2;
}

template<class T> T Norm(const Matrix<T> &M) 
{
  return normFro(M);
}

template<class T> T NormFro(const Matrix<T> &M) 
{
  T val = 0;
  T *m  = M._m;

  for (int i=0; i<M._nelem; i++) 
    val += abs(m[i])*abs(m[i]);
  
  return sqrt(val);
}

template<class T> T NormInf(const Matrix<T> &M) 
{
  int rlo = M._rlo;
  int rhi = M._rhi;
  int clo = M._clo;
  int chi = M._chi;
  T **v = M._v;
  T max = 0;

  for (int i=rlo; i<=rhi; i++) 
    for (int j=clo; j<=chi; j++) 
      if (abs(v[i][j]) > max) 
        max = abs(v[i][j]);
  
  return max;
} 

template<class T> T Norm1(const Matrix<T> &M) 
{
  int rlo = M._rlo;
  int rhi = M._rhi;
  int clo = M._clo;
  int chi = M._chi;
  T **v = M._v;
  T max = 0;
  T tmp;

  for (int i=clo; i<=chi; i++) {
    tmp = 0;
    for (int j=rlo; j<=rhi; j++) 
      tmp += abs(v[j][i]);
    if (tmp > max) 
      max = tmp;
  }
  
  return max;
}  

template<class T> Matrix< std::complex<T> > Conj(const Matrix< std::complex<T> > &M) 
{
  std::complex<T> *m = M._m;
  Matrix< std::complex<T> > M2(M._rlo, M._rhi, M._clo, M._chi);
  std::complex<T> *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = std::complex<T>(real(m[i]),-imag(m[i]));
  
  return M2;
}

template<class T> Matrix< std::complex<T> > Real(const Matrix< std::complex<T> > &M) 
{
  std::complex<T> *m = M._m;
  Matrix< std::complex<T> > M2(M._rlo, M._rhi, M._clo, M._chi);
  std::complex<T> *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i].real();
  
  return M2;
}

template<class T> Matrix< std::complex<T> > Imag(const Matrix< std::complex<T> > &M) 
{
  std::complex<T> *m = M._m;
  Matrix< std::complex<T> > M2(M._rlo, M._rhi, M._clo, M._chi);
  std::complex<T> *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = m[i].imag();
  
  return M2;
}

template<class T> Matrix< std::complex<T> > Arg(const Matrix< std::complex<T> > &M) 
{
  std::complex<T> *m = M._m;
  Matrix< std::complex<T> > M2(M._rlo, M._rhi, M._clo, M._chi);
  std::complex<T> *m2 = M2._m;
  
  for (int i=0; i<M._nelem; i++) {
    m2[i] = atan(imag(m[i])/real(m[i]));
  
    if (real(m[i]) < 0) 
      m2[i] = m2[i] + 3.1415926535897932;
  }
  
  return M2;
}

template<class T> Matrix< std::complex<T> > Pow(const Matrix< std::complex<T> > &M, const T &e) 
{
  std::complex<T> *m = M._m;
  Matrix< std::complex<T> > M2(M._rlo, M._rhi, M._clo, M._chi);
  std::complex<T> *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = pow(m[i], e);
  
  return M2;
}

template<class T> Matrix< std::complex<T> > Pow(const T &e, const Matrix< std::complex<T> > &M) 
{
  std::complex<T> *m = M._m;
  Matrix< std::complex<T> > M2(M._rlo, M._rhi, M._clo, M._chi);
  std::complex<T> *m2 = M2._m;

  for (int i=0; i<M._nelem; i++) 
    m2[i] = pow(e, m[i]);
  
  return M2;
}

template<class T> T Norm(const Matrix< std::complex<T> > &M) 
{
  return normFro(M);
}

template<class T> T NormFro(const Matrix< std::complex<T> > &M) 
{
  T val = 0;
  std::complex<T> *m  = M._m;

  for (int i=0; i<M._nelem; i++) 
    val += abs(m[i])*abs(m[i]);
  
  return sqrt(val);
}

template<class T> T NormInf(const Matrix< std::complex<T> > &M) 
{
  int rlo = M._rlo;
  int rhi = M._rhi;
  int clo = M._clo;
  int chi = M._chi;
  std::complex<T> **v = M._v;
  T max = 0;

  for (int i=rlo; i<=rhi; i++) 
    for (int j=clo; j<=chi; j++)  
      if (max < abs(v[i][j])) 
        max = abs(v[i][j]);
  
  return max;
} 

template<class T> T Norm1(const Matrix< std::complex<T> > &M) 
{
  int rlo = M._rlo;
  int rhi = M._rhi;
  int clo = M._clo;
  int chi = M._chi;
  std::complex<T> **v = M._v;
  T max = 0;
  T tmp;

  for (int i=clo; i<=chi; i++) {
    tmp = 0;
    for (int j=rlo; j<=rhi; j++) 
      tmp += abs(v[j][i]);
    if (tmp > max) 
      max = tmp;
  }

  return max;
}  

template<class T> Matrix< std::complex<T> > Dag(const Matrix< std::complex<T> > &M) 
{
  Matrix< std::complex<T> > M2(M._clo, M._chi, M._rlo, M._rhi);
  std::complex<T> **v2 = M2._v;
  std::complex<T> **v = M._v;
  int clo = M._rlo;
  int chi = M._rhi;
  int rlo = M._clo;
  int rhi = M._chi;

  for (int i=rlo; i<=rhi; i++) 
    for (int j=clo; j<=chi; j++)
      v2[i][j] = std::complex<T>(real(v[j][i]), -imag(v[j][i]));

  return M2;
}

template<class T> std::ostream& operator<<(std::ostream& os, const Vector<T> &V) 
{
  for (int i=V._vlo; i<=V._vhi; i++) 
    os << V(i) << " ";
  
  os << std::endl;
  
  return os;
}

template<class T> std::istream& operator>>(std::istream& os, Vector<T> &V) 
{
  for (int i=V._vlo; i<=V._vhi; i++) 
    os >> V(i);

  return os;
} 

template<class T> std::ostream& operator<<(std::ostream& os, const Matrix<T> &M) 
{
  for (int i=M._rlo; i<=M._rhi; i++) {
    for (int j=M._clo; j<=M._chi; j++)
      os << M(i,j) << " ";
    os << std::endl;
  }

  return os;
}

template<class T> std::istream& operator>>(std::istream& os, Matrix<T> &M) 
{
  for (int i=M._rlo; i<=M._rhi; i++) 
    for (int j=M._clo; j<=M._chi; j++)
      os >> M(i,j);

  return os;
}

}

#endif

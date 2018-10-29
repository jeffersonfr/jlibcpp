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

#include "jmath/jmath.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jnullpointerexception.h"
#include "jexception/jinvalidargumentexception.h"

#include <iostream>
#include <fstream>
#include <complex>

#include <stdlib.h>

namespace jmath {

template<class T> class Vector : public virtual jcommon::Object {

	public:
		T *_v;
		int _vlo;
		int _vhi;
		int _nelem;

		/** 
		 * \brief Construct new Vector of dimension 0 
		 *
		 */
		Vector():
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Vector");

			_v = nullptr;
			_vlo = 0;
			_vhi = 0;
			_nelem = 0;
		}

		/** 
		 * \brief Copy constructor - copy by value 
		 *
		 */
		Vector(const Vector &V):
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Vector");

			_v = nullptr;
			_vlo = 0;
			_vhi = 0;
			_nelem = 0;
			*this = V;
		}

		/** 
		 * \brief Construct new vector without initialization 
		 *
		 */
		Vector(int vlo, int vhi):
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Vector");

			NewVector(vlo, vhi);
		}

		/** 
		 * \brief Construct new Vector without initialization 
		 *
		 */
		Vector(int vhi):
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Vector");

			if (vhi < 1) {
				throw jexception::OutOfBoundsException("Vector index is out of bounds");
			}

			NewVector(1, vhi);
		}

		/** 
		 * \brief Construct new vector with elements a 
		 *
		 */
		Vector(int vlo, int vhi, T a):
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Vector");

			NewVector(vlo, vhi);
			*this = a;
		}

		/**
		 * \brief Destructor
		 *
		 */
		virtual ~Vector() 
		{
			delete [] (_v+_vlo);
		}

		void NewVector(int vlo, int vhi) 
		{
			if (vlo > vhi)
				throw jexception::InvalidArgumentException("Attempt to create Vector with low index > high index.");
			this->_vlo = vlo;
			this->_vhi = vhi;
			_nelem = vhi-vlo+1;
			(_v = new T[_nelem]) -= vlo;
		}

		void SetSize(int vlo, int vhi) 
		{
			delete [] (_v+this->_vlo);
			NewVector(vlo, vhi);
		}

		void SetSize(const Vector<T> &V) 
		{
			SetSize(V._vlo, V._vhi);
		}

		void SetSize(int vhi) 
		{
			if (vhi < 1) {
				throw jexception::InvalidArgumentException("Size of vector cannot be nullptr");
			}

			delete [] (_v+this->_vlo);
			
			NewVector(1, vhi);
		}

		void NewOffset(int newlo) 
		{
			_v -= (newlo - _vlo);
			_vlo = newlo;
			_vhi = newlo + _nelem - 1;
		}

		/**
		 * \brief returns a new Vector with same values as *this, but with type of "other" Vector.
		 *
		 */
		template<class T2> Vector<T2> Convert(Vector<T2> & other) 
		{
				Vector<T2> new_vector(_vlo, _vhi);

				for (int i = _vlo; i <= _vhi; i++)
					new_vector(i) = _v[i];
				
				return new_vector;
			}

		int Low() const 
		{
			return _vlo;
		}

		int High() const 
		{
			return _vhi;
		}

		int GetSize() const 
		{
			return _nelem;
		}

		/** 
		 * \brief Reference element n 
		 *
		 */ 
		inline T &operator()(int n) 
		{
			if ((n < _vlo) || (n > _vhi))
				throw jexception::OutOfBoundsException("Vector::operator(): Element reference out of bounds");
			
			return _v[n];
		} 

		/** 
		 * \brief Reference element n 
		 *
		 */
		inline T &operator()(int n) const {
			if ((n < _vlo) || (n > _vhi))
				throw jexception::OutOfBoundsException("Vector::operator(): Element reference out of bounds");
			
			return _v[n];
		}

		/** 
		 * \brief Reference element n 
		 *
		 */
		inline T &operator[](int n) 
		{
			return _v[n];
		}

		/** 
		 * \brief Reference element n 
		 *
		 */
		inline const T &operator[](int n) const 
		{
			return _v[n];
		}

		/** 
		 * 
		 * \brief Return new Vector with range (lo, hi) 
		 */
		Vector<T> SubVector(int lo, int hi) 
		{
			if ( (lo < _vlo) || (hi > _vhi) )
				throw jexception::OutOfBoundsException("Vector::subvector: Invalid range selected");
			
			Vector<T> sub;
			int range = hi-lo+1;
			
			sub.NewVector(_vlo, _vlo+range-1);
			
			T *v = this->_v + lo,
				*subv = sub._v + _vlo;  
			
			for (int i=0; i<range; i++) 
				subv[i] = v[i];
			
			return sub;
		}

		T * Raw() const 
		{
			return (_v+_vlo);
		}

		/** 
		 * \brief Assignment by value to scalar 
		 *
		 */
		Vector<T> &operator=(T a) 
		{
			for (int i=_vlo; i<=_vhi; i++) 
				_v[i] = a;

			return *this;
		}

		/**
		 * \brief Assignment by value to Vector 
		 *
		 */
		Vector<T> &operator=(const Vector<T> &V) 
		{
			delete [] (_v+_vlo);
			
			NewVector(V._vlo,V._vhi);
			
			for (int i=_vlo; i<_vlo+_nelem; i++) 
				_v[i] = V._v[i];
			
			return *this;
		}

		/** 
		 * \brief Assignment by value a 1DP vector 
		 *
		 */
		Vector<T> &operator=(T *v2) 
		{
			T *v = this->_v + _vlo;
			
			for (int i=0; i<_nelem; i++) 
				v[i] = v2[i];
			
			return *this;
		}

		/** 
		 * \brief Vector addition. 
		 *
		 */
		Vector<T> &operator+=(const Vector<T> &V) 
		{
			if (V._nelem != _nelem)
				throw jexception::InvalidArgumentException("Vector::operator+=(): Attempt to add nonconformant Vector");
			
			T *v = this->_v+_vlo,
				*v2 = V._v + V._vlo;
			
			for (int i=0; i<_nelem; i++) 
				v[i] += v2[i];
			
			return *this;
		}

		/** 
		 * \brief Vector subtraction 
		 *
		 */
		Vector<T> &operator-=(const Vector<T> &V) 
		{
			if (V._nelem != _nelem)
				throw jexception::InvalidArgumentException("Vector::operator-=(): Attempt to subtract nonconformant Vector");

			T *v = this->_v+_vlo;
			T *v2 = V._v + V._vlo;
			for (int i=0; i<_nelem; i++) 
				v[i] -= v2[i];
			return *this;
		}

		/** 
		 * \breif Addition of scalar 
		 *
		 */
		Vector<T> &operator+=(const T &e) 
		{
			T *v = this->_v+_vlo;
			for (int i=0; i<_nelem; i++) 
				v[i] += e;
			return *this;
		}

		/** 
		 * \brief Subtraction of scalar 
		 *
		 * */
		Vector<T> &operator-=(const T &e) 
		{
			T *v = this->_v+_vlo;
			for (int i=0; i<_nelem; i++) 
				v[i] += e;
			return *this;
		}

		/** 
		 * \brief Multiplication by scalar 
		 *
		 */
		Vector<T> &operator*=(const T &e) 
		{
			T *v = this->_v+_vlo;
			for (int i=0; i<_nelem; i++) 
				v[i] *= e;
			return *this;
		}

		/** 
		 * \brief Division by scalar 
		 *
		 */
		Vector<T> &operator/=(const T &e) 
		{
			T *v = this->_v+_vlo;
			for (int i=0; i<_nelem; i++) 
				v[i] /= e;
			return *this;
		}

};

template<class T> Vector<T> operator-(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = -v[i];

	return V2;
}

template<class T> Vector<T> operator+(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("operator+(Vector, Vector):  Attempt to add nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;
	Vector<T> V(V1._vlo, V1._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V1._nelem; i++) 
		v[i] = v1[i] + v2[i];

	return V;
}

template<class T> Vector<T> operator+(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i] + e;

	return V2;
}

template<class T> Vector<T> operator+(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = e + v[i];

	return V2;
}

template<class T> Vector<T> operator-(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("operator-(Vector, Vector): Attempt to subtract nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;
	Vector<T> V(V1._vlo, V1._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V1._nelem; i++) 
		v[i] = v1[i] - v2[i];

	return V;
}

template<class T> Vector<T> operator-(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i] - e;

	return V2;
}

template<class T> Vector<T> operator-(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = e - v[i];

	return V2;
}

template<class T> Vector<T> operator*(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i] * e;

	return V2;
}

template<class T> Vector<T> operator*(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = e * v[i];

	return V2;
}

template<class T> Vector<T> operator/(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i] / e;

	return V2;
}

template<class T> Vector<T> operator/(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = e / v[i];

	return V2;
}

template<class T> Vector<T> operator/(const Vector<T> &V1, const Vector<T> &V2) 
{
	return DivideVector(V1, V2);
}

template<class T> Vector<T> SumVector(const Vector<T> &V, const T &e) 
{
	return (V + e);
}

template<class T> Vector<T> SumVector(const T &e, const Vector<T> &V) 
{
	return (e + V);
}

template<class T> Vector<T> SumVector(const Vector<T> &V1, const Vector<T> &V2) 
{
	return (V1 + V2);
}

template<class T> Vector<T> SubVector(const Vector<T> &V, const T &e) 
{
	return (V - e);
}

template<class T> Vector<T> SubVector(const T &e, const Vector<T> &V) 
{
	return (e - V);
}

template<class T> Vector<T> SubVector(const Vector<T> &V1, const Vector<T> &V2) 
{
	return (V1 - V2);
}

template<class T> Vector<T> MultiplyVector(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("elem_mul(Vector, Vector): Attempt to element multiply nonconformant Vectors");

	T *v1 = V1._v + V1._vlo;
	T *v2 = V2._v + V2._vlo;
	Vector<T> V(V1._vlo, V1._vhi);
	T *v = V._v + V._vlo;
	for (int i=0; i<V1._nelem; i++) 
		v[i] = v1[i] * v2[i];
	return V;
}

template<class T> Vector<T> MultiplyVector(const Vector<T> &V, const T &e) 
{
	return (V * e);
}

template<class T> Vector<T> MultiplyVector(const T &e, const Vector<T> &V) 
{
	return (e * V);
}

template<class T> Vector<T> DivideVector(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("elem_div(Vector, Vector): Attempt to element divide nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;
	Vector<T> V(V1._vlo, V1._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V1._nelem; i++) 
		v[i] = v1[i] / v2[i];

	return V;
}

template<class T> Vector<T> DivideVector(const Vector<T> &V, const T &e) 
{
	return (V / e);
}

template<class T> Vector<T> DivideVector(const T &e, const Vector<T> &V) 
{
	return (e / V);
}

template<class T> T Min(const Vector<T> &V) 
{
	T *v = V._v + V._vlo,
		low = v[0];

	for (int i=1; i<V._nelem; i++) 
		if (low > v[i]) low = v[i];

	return low;
}

template<class T> Vector<T> Min(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("min(Vector, Vector): Attempt to extract minimum from nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;
	Vector<T> V(V1._vlo, V1._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V1._nelem; i++) 
		if (v1[i] <= v2[i]) 
			v[i] = v1[i];
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Min(const T &e, const Vector<T> &V2) 
{
	T *v2 = V2._v + V2._vlo;
	Vector<T> V(V2._vlo, V2._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V2._nelem; i++) 
		if (e <= v2[i]) 
			v[i] = e;
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Min(const Vector<T> &V2, const T &e) 
{
	T *v2 = V2._v + V2._vlo;
	Vector<T> V(V2._vlo, V2._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V2._nelem; i++) 
		if (v2[i] <= e) 
			v[i] = v2[i];
		else 
			v[i] = e;

	return V;
}

template<class T> T Max(const Vector<T> &V) 
{
	T *v = V._v + V._vlo,
		hi = v[0];

	for (int i=1; i<V._nelem; i++)
		if (hi < v[i]) 
			hi = v[i];

	return hi;
}

template<class T> Vector<T> Max(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("min(Vector, Vector): Attempt to extract minimum from nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;
	Vector<T> V(V1._vlo, V1._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V1._nelem; i++) 
		if (v1[i] >= v2[i]) 
			v[i] = v1[i];
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Max(const T &e, const Vector<T> &V2) 
{
	T *v2 = V2._v + V2._vlo;
	Vector<T> V(V2._vlo, V2._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V2._nelem; i++) 
		if (e >= v2[i]) 
			v[i] = e;
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Max(const Vector<T> &V2, const T &e) 
{
	T *v2 = V2._v + V2._vlo;
	Vector<T> V(V2._vlo, V2._vhi);
	T *v = V._v + V._vlo;

	for (int i=0; i<V2._nelem; i++) 
		if (v2[i] >= e) 
			v[i] = v2[i];
		else 
			v[i] = e;

	return V;
}

template<class T> bool operator==(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;

	for (int i=0; i<V1._nelem; i++)
		if (v1[i] != v2[i]) 
			return false;

	return true;
}

template<class T> bool operator==(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (v[i] != e) 
			return false;

	return true;
}

template<class T> bool operator==(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (e != v[i]) 
			return false;

	return true;
}

template<class T> bool operator<(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;

	for (int i=0; i<V1._nelem; i++)
		if (v1[i] >= v2[i]) 
			return false;

	return true;
}

template<class T> bool operator<(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (v[i] >= e) 
			return false;

	return true;
}

template<class T> bool operator<(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (e >= v[i]) 
			return false;

	return true;
}

template<class T> bool operator<=(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;

	for (int i=0; i<V1._nelem; i++)
		if (v1[i] > v2[i]) 
			return false;

	return true;
}

template<class T> bool operator<=(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (v[i] > e) 
			return false;

	return true;
}

template<class T> bool operator<=(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (e > v[i]) 
			return false;

	return true;
}

template<class T> bool operator>=(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;

	for (int i=0; i<V1._nelem; i++)
		if (v1[i] < v2[i]) 
			return false;

	return true;
}

template<class T> bool operator>=(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (v[i] < e) 
			return false;

	return true;
}

template<class T> bool operator>=(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (e < v[i]) 
			return false;

	return true;
}

template<class T> bool operator>(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem)
		throw jexception::InvalidArgumentException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;

	for (int i=0; i<V1._nelem; i++)
		if (v1[i] <= v2[i]) 
			return false;

	return true;
}

template<class T> bool operator>(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (v[i] <= e) 
			return false;

	return true;
}

template<class T> bool operator>(const T &e, const Vector<T> &V) 
{
	T *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++)
		if (e <= v[i]) 
			return false;

	return true;
}

template<class T> T Dot(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1._nelem != V2._nelem) 
		throw jexception::InvalidArgumentException("dot(Vector, Vector): Nonconformant vectors");

	T val = 0,
		*v1 = V1._v + V1._vlo,
		*v2 = V2._v + V2._vlo;

	for (int i=0; i<V1._nelem; i++) 
		val += v1[i] * v2[i];

	return val;
}

template<class T> T operator*(const Vector<T> &V1, const Vector<T> &V2) 
{
	return dot(V1, V2);
}

template<class T> T Sum(const Vector<T> &V) 
{
	T val = 0,
		*v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) 
		val += v[i];

	return val;
}

template<class T> T Multiply(const Vector<T> &V) 
{
	T val = 1,
		*v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) 
		val *= v[i];

	return val;
}

template<class T> Vector<T> Pow(const Vector<T> &V, const T &e) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = pow(v[i], e);

	return V2;
}

template<class T> Vector<T> Pow(const Vector<T> &V, int e) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = pow(v[i], e);

	return V2;
}

template<class T> Vector<T> Abs(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = abs(v[i]);

	return V2;
}

template<class T> Vector<T> Sqrt(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = sqrt(v[i]);

	return V2;
}

template<class T> Vector<T> Sqr(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i]*v[i];

	return V2;
}

template<class T> Vector<T> AbsSqr(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = abs(v[i])*abs(v[i]);

	return V2;
}

template<class T> Vector<T> Cube(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i]*v[i]*v[i];

	return V2;
}

template<class T> Vector<T> Sin(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = sin(v[i]);

	return V2;
}

template<class T> Vector<T> Cos(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = cos(v[i]);

	return V2;
}

template<class T> Vector<T> Tan(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = tan(v[i]);

	return V2;
}

template<class T> Vector<T> Sinh(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = sinh(v[i]);

	return V2;
}

template<class T> Vector<T> Cosh(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = cosh(v[i]);

	return V2;
}

template<class T> Vector<T> Tanh(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = tanh(v[i]);

	return V2;
}

template<class T> Vector<T> Exp(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = exp(v[i]);

	return V2;
}

template<class T> Vector<T> Ln(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = log(v[i]);

	return V2;
}

template<class T> Vector<T> Log10(const Vector<T> &V) 
{
	T *v = V._v + V._vlo;
	Vector<T> V2(V._vlo, V._vhi);
	T *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = log10(v[i]);

	return V2;
}

template<class T> T Norm(const Vector<T> &V) 
{
	return norm2(V);
}

template<class T> T Norm1(const Vector<T> &V) 
{
	T val = 0,
		*v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) 
		val += abs(v[i]);

	return val;
}

template<class T> T NormInf(const Vector<T> &V) 
{
	T val = 0,
		*v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) {
		if (abs(v[i]) > val) 
			val = abs(v[i]);
	}

	return val;
}

template<class T> T Norm2(const Vector<T> &V) 
{
	T val = 0,
		*v  = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) 
		val += abs(v[i])*abs(v[i]);

	return sqrt(val);
}

template<class T> Vector< std::complex<T> > Conj(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V._v + V._vlo;
	Vector< std::complex<T> > V2(V._vlo, V._vhi);
	std::complex<T> *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = std::complex<T>(real(v[i]), -imag(v[i]));

	return V2;
}

template<class T> Vector< std::complex<T> > Real(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V._v + V._vlo;
	Vector< std::complex<T> > V2(V._vlo, V._vhi);
	std::complex<T> *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i].real();

	return V2;
}

template<class T> Vector< std::complex<T> > Imag(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V._v + V._vlo;
	Vector< std::complex<T> > V2(V._vlo, V._vhi);
	std::complex<T> *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = v[i].imag();

	return V2;
}

template<class T> Vector< std::complex<T> > Arg(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V._v + V._vlo;
	Vector< std::complex<T> > V2(V._vlo, V._vhi);
	std::complex<T> *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) {
		v2[i] = atan(imag(v[i])/real(v[i]));

		if (real(v[i]) < 0) 
			v2[i] = v2[i] + 3.1415926535897932;
	}

	return V2;
}

template<class T> Vector< std::complex<T> > Pow(const Vector< std::complex<T> > &V, const T &e) 
{
	std::complex<T> *v = V._v = V._vlo;
	Vector< std::complex<T> > V2(V._vlo, V._vhi);
	std::complex<T> *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = pow(v[i], e);

	return V2;
}

template<class T> Vector< std::complex<T> > Pow(const T &e, const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V._v = V._vlo;
	Vector< std::complex<T> > V2(V._vlo, V._vhi);
	std::complex<T> *v2 = V2._v + V2._vlo;

	for (int i=0; i<V._nelem; i++) 
		v2[i] = pow(e, v[i]);

	return V2;
}

template<class T> T Norm(const Vector< std::complex<T> > &V) 
{
	return Norm2(V);
}

template<class T> T Norm1(const Vector< std::complex<T> > &V) 
{
	T val = 0;
	std::complex<T> *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) 
		val += abs(v[i]);

	return val;
}

template<class T> T NormInf(const Vector< std::complex<T> > &V) 
{
	T val = 0;
	std::complex<T> *v = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) {
		if (abs(v[i]) > val) 
			val = abs(v[i]);
	}

	return val;
}

template<class T> T Norm2(const Vector< std::complex<T> > &V) 
{
	T val = 0;
	std::complex<T> *v  = V._v + V._vlo;

	for (int i=0; i<V._nelem; i++) 
		val += abs(v[i])*abs(v[i]);

	return ::sqrt(val);
}

}

#endif

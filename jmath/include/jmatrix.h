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

#include "jobject.h"
#include "jruntimeexception.h"
#include "jmath.h"

#include <iostream>
#include <fstream>
#include <complex>

#include <stdlib.h>

namespace jmath {

/**
 * \brief
 * 
 * \author Brian Thorndyke <thorndyb@phys.ufl.edu>
 * \author Robert Thorndyke <rthorndy@camosun.bc.ca>
 *
 */
template<class T> bool operator<(const std::complex<T> &e1, const std::complex<T> &e2) {
	if (abs(e1) < abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> bool operator<(const std::complex<T> &e1, const T &e2) {
	if (abs(e1) < e2) 
		return true;
	else 
		return false;
}

template<class T> bool operator<(const T &e1, const std::complex<T> &e2) {
	if (e1 < abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> bool operator<=(const std::complex<T> &e1, const std::complex<T> &e2) {
	if (abs(e1) <= abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> bool operator<=(const std::complex<T> &e1, const T &e2) {
	if (abs(e1) <= e2) 
		return true;
	else 
		return false;
}

template<class T> bool operator<=(const T &e1, const std::complex<T> &e2) {
	if (e1 <= abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> bool operator>(const std::complex<T> &e1, const std::complex<T> &e2) {
	if (abs(e1) > abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> bool operator>(const std::complex<T> &e1, const T &e2) {
	if (abs(e1) > e2) 
		return true;
	else 
		return false;
}

template<class T> bool operator>(const T &e1, const std::complex<T> &e2) {
	if (e1 > abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> bool operator>=(const std::complex<T> &e1, const std::complex<T> &e2) {
	if (abs(e1) >= abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> bool operator>=(const std::complex<T> &e1, const T &e2) {
	if (abs(e1) >= e2) 
		return true;
	else 
		return false;
}

template<class T> bool operator>=(const T &e1, const std::complex<T> &e2) {
	if (e1 >= abs(e2)) 
		return true;
	else 
		return false;
}

template<class T> class Vector : public virtual jcommon::Object{

	public:
		T *v;
		int vlo, 
			vhi, 
			nelem;

		/** 
		 * \brief Construct new Vector of dimension 0 
		 *
		 */
		Vector():
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Vector");

			v = NULL;
			vlo = 0;
			vhi = 0;
			nelem = 0;
		}

		/** 
		 * \brief Copy constructor - copy by value 
		 *
		 */
		Vector(const Vector &V):
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Vector");

			v = NULL;
			vlo = 0;
			vhi = 0;
			nelem = 0;
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
				throw jcommon::RuntimeException("Vector index is out of range");
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
			delete [] (v+vlo);
		}

		void NewVector(int vlo, int vhi) 
		{
			if (vlo > vhi)
				throw jcommon::RuntimeException("Attempt to create Vector with low index > high index.");
			this->vlo = vlo;
			this->vhi = vhi;
			nelem = vhi-vlo+1;
			(v = new T[nelem]) -= vlo;
		}

		void SetSize(int vlo, int vhi) 
		{
			delete [] (v+this->vlo);
			NewVector(vlo, vhi);
		}

		void SetSize(const Vector<T> &V) 
		{
			SetSize(V.vlo, V.vhi);
		}

		void SetSize(int vhi) 
		{
			if (vhi < 1) {
				throw jcommon::RuntimeException("Size of vector cannot be NULL");
			}

			delete [] (v+this->vlo);
			
			NewVector(1, vhi);
		}

		void NewOffset(int newlo) 
		{
			v -= (newlo - vlo);
			vlo = newlo;
			vhi = newlo + nelem - 1;
		}

		/**
		 * \brief returns a new Vector with same values as *this, but with type of "other" Vector.
		 *
		 */
		template<class T2> Vector<T2> Convert(Vector<T2> & other) 
		{
				Vector<T2> new_vector(vlo, vhi);

				for (int i = vlo; i <= vhi; i++)
					new_vector(i) = v[i];
				
				return new_vector;
			}

		int Low() const 
		{
			return vlo;
		}

		int High() const 
		{
			return vhi;
		}

		int GetSize() const 
		{
			return nelem;
		}

		/** 
		 * \brief Reference element n 
		 *
		 */ 
		inline T &operator()(int n) 
		{
			if ((n < vlo) || (n > vhi))
				throw jcommon::RuntimeException("Vector::operator():  Element reference out of bounds");
			
			return v[n];
		} 

		/** 
		 * \brief Reference element n 
		 *
		 */
		inline T &operator()(int n) const {
			if ((n < vlo) || (n > vhi))
				throw jcommon::RuntimeException("Vector::operator(): Element reference out of bounds");
			
			return v[n];
		}

		/** 
		 * \brief Reference element n 
		 *
		 */
		inline T &operator[](int n) 
		{
			return v[n];
		}

		/** 
		 * \brief Reference element n 
		 *
		 */
		inline const T &operator[](int n) const 
		{
			return v[n];
		}

		/** 
		 * 
		 * \brief Return new Vector with range (lo, hi) 
		 */
		Vector<T> SubVector(int lo, int hi) 
		{
			if ( (lo < vlo) || (hi > vhi) )
				throw jcommon::RuntimeException("Vector::subvector: Invalid range selected");
			
			Vector<T> sub;
			int range = hi-lo+1;
			
			sub.NewVector(vlo, vlo+range-1);
			
			T *v = this->v + lo,
				*subv = sub.v + vlo;  
			
			for (int i=0; i<range; i++) 
				subv[i] = v[i];
			
			return sub;
		}

		T * Raw() const 
		{
			return (v+vlo);
		}

		/** 
		 * \brief Assignment by value to scalar 
		 *
		 */
		Vector<T> &operator=(T a) 
		{
			for (int i=vlo; i<=vhi; i++) 
				v[i] = a;

			return *this;
		}

		/**
		 * \brief Assignment by value to Vector 
		 *
		 */
		Vector<T> &operator=(const Vector<T> &V) 
		{
			delete [] (v+vlo);
			
			NewVector(V.vlo,V.vhi);
			
			for (int i=vlo; i<vlo+nelem; i++) 
				v[i] = V.v[i];
			
			return *this;
		}

		/** 
		 * \brief Assignment by value a 1DP vector 
		 *
		 */
		Vector<T> &operator=(T *v2) 
		{
			T *v = this->v + vlo;
			
			for (int i=0; i<nelem; i++) 
				v[i] = v2[i];
			
			return *this;
		}

		/** 
		 * \brief Vector addition. 
		 *
		 */
		Vector<T> &operator+=(const Vector<T> &V) 
		{
			if (V.nelem != nelem)
				throw jcommon::RuntimeException("Vector::operator+=(): Attempt to add nonconformant Vector");
			
			T *v = this->v+vlo,
				*v2 = V.v + V.vlo;
			
			for (int i=0; i<nelem; i++) 
				v[i] += v2[i];
			
			return *this;
		}

		/** 
		 * \brief Vector subtraction 
		 *
		 */
		Vector<T> &operator-=(const Vector<T> &V) 
		{
			if (V.nelem != nelem)
				throw jcommon::RuntimeException("Vector::operator-=(): Attempt to subtract nonconformant Vector");

			T *v = this->v+vlo;
			T *v2 = V.v + V.vlo;
			for (int i=0; i<nelem; i++) 
				v[i] -= v2[i];
			return *this;
		}

		/** 
		 * \breif Addition of scalar 
		 *
		 */
		Vector<T> &operator+=(const T &e) 
		{
			T *v = this->v+vlo;
			for (int i=0; i<nelem; i++) 
				v[i] += e;
			return *this;
		}

		/** 
		 * \brief Subtraction of scalar 
		 *
		 * */
		Vector<T> &operator-=(const T &e) 
		{
			T *v = this->v+vlo;
			for (int i=0; i<nelem; i++) 
				v[i] += e;
			return *this;
		}

		/** 
		 * \brief Multiplication by scalar 
		 *
		 */
		Vector<T> &operator*=(const T &e) 
		{
			T *v = this->v+vlo;
			for (int i=0; i<nelem; i++) 
				v[i] *= e;
			return *this;
		}

		/** 
		 * \brief Division by scalar 
		 *
		 */
		Vector<T> &operator/=(const T &e) 
		{
			T *v = this->v+vlo;
			for (int i=0; i<nelem; i++) v[i] /= e;
			return *this;
		}

};

template<class T> Vector<T> operator-(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = -v[i];

	return V2;
}

template<class T> Vector<T> operator+(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("operator+(Vector, Vector):  Attempt to add nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;
	Vector<T> V(V1.vlo, V1.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V1.nelem; i++) 
		v[i] = v1[i] + v2[i];

	return V;
}

template<class T> Vector<T> operator+(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i] + e;

	return V2;
}

template<class T> Vector<T> operator+(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = e + v[i];

	return V2;
}

template<class T> Vector<T> operator-(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("operator-(Vector, Vector): Attempt to subtract nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;
	Vector<T> V(V1.vlo, V1.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V1.nelem; i++) 
		v[i] = v1[i] - v2[i];

	return V;
}

template<class T> Vector<T> operator-(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i] - e;

	return V2;
}

template<class T> Vector<T> operator-(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = e - v[i];

	return V2;
}

template<class T> Vector<T> operator*(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i] * e;

	return V2;
}

template<class T> Vector<T> operator*(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = e * v[i];

	return V2;
}

template<class T> Vector<T> operator/(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i] / e;

	return V2;
}

template<class T> Vector<T> operator/(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
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
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("elem_mul(Vector, Vector): Attempt to element multiply nonconformant Vectors");

	T *v1 = V1.v + V1.vlo;
	T *v2 = V2.v + V2.vlo;
	Vector<T> V(V1.vlo, V1.vhi);
	T *v = V.v + V.vlo;
	for (int i=0; i<V1.nelem; i++) 
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
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("elem_div(Vector, Vector): Attempt to element divide nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;
	Vector<T> V(V1.vlo, V1.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V1.nelem; i++) 
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
	T *v = V.v + V.vlo,
		low = v[0];

	for (int i=1; i<V.nelem; i++) 
		if (low > v[i]) low = v[i];

	return low;
}

template<class T> Vector<T> Min(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("min(Vector, Vector): Attempt to extract minimum from nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;
	Vector<T> V(V1.vlo, V1.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V1.nelem; i++) 
		if (v1[i] <= v2[i]) 
			v[i] = v1[i];
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Min(const T &e, const Vector<T> &V2) 
{
	T *v2 = V2.v + V2.vlo;
	Vector<T> V(V2.vlo, V2.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V2.nelem; i++) 
		if (e <= v2[i]) 
			v[i] = e;
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Min(const Vector<T> &V2, const T &e) 
{
	T *v2 = V2.v + V2.vlo;
	Vector<T> V(V2.vlo, V2.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V2.nelem; i++) 
		if (v2[i] <= e) 
			v[i] = v2[i];
		else 
			v[i] = e;

	return V;
}

template<class T> T Max(const Vector<T> &V) 
{
	T *v = V.v + V.vlo,
		hi = v[0];

	for (int i=1; i<V.nelem; i++)
		if (hi < v[i]) 
			hi = v[i];

	return hi;
}

template<class T> Vector<T> Max(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("min(Vector, Vector): Attempt to extract minimum from nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;
	Vector<T> V(V1.vlo, V1.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V1.nelem; i++) 
		if (v1[i] >= v2[i]) 
			v[i] = v1[i];
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Max(const T &e, const Vector<T> &V2) 
{
	T *v2 = V2.v + V2.vlo;
	Vector<T> V(V2.vlo, V2.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V2.nelem; i++) 
		if (e >= v2[i]) 
			v[i] = e;
		else 
			v[i] = v2[i];

	return V;
}

template<class T> Vector<T> Max(const Vector<T> &V2, const T &e) 
{
	T *v2 = V2.v + V2.vlo;
	Vector<T> V(V2.vlo, V2.vhi);
	T *v = V.v + V.vlo;

	for (int i=0; i<V2.nelem; i++) 
		if (v2[i] >= e) 
			v[i] = v2[i];
		else 
			v[i] = e;

	return V;
}

template<class T> bool operator==(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;

	for (int i=0; i<V1.nelem; i++)
		if (v1[i] != v2[i]) 
			return false;

	return true;
}

template<class T> bool operator==(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (v[i] != e) 
			return false;

	return true;
}

template<class T> bool operator==(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (e != v[i]) 
			return false;

	return true;
}

template<class T> bool operator<(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;

	for (int i=0; i<V1.nelem; i++)
		if (v1[i] >= v2[i]) 
			return false;

	return true;
}

template<class T> bool operator<(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (v[i] >= e) 
			return false;

	return true;
}

template<class T> bool operator<(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (e >= v[i]) 
			return false;

	return true;
}

template<class T> bool operator<=(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;

	for (int i=0; i<V1.nelem; i++)
		if (v1[i] > v2[i]) 
			return false;

	return true;
}

template<class T> bool operator<=(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (v[i] > e) 
			return false;

	return true;
}

template<class T> bool operator<=(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (e > v[i]) 
			return false;

	return true;
}

template<class T> bool operator>=(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;

	for (int i=0; i<V1.nelem; i++)
		if (v1[i] < v2[i]) 
			return false;

	return true;
}

template<class T> bool operator>=(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (v[i] < e) 
			return false;

	return true;
}

template<class T> bool operator>=(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (e < v[i]) 
			return false;

	return true;
}

template<class T> bool operator>(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem)
		throw jcommon::RuntimeException("Vector::operator==(): Attempt to compare nonconformant Vectors");

	T *v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;

	for (int i=0; i<V1.nelem; i++)
		if (v1[i] <= v2[i]) 
			return false;

	return true;
}

template<class T> bool operator>(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (v[i] <= e) 
			return false;

	return true;
}

template<class T> bool operator>(const T &e, const Vector<T> &V) 
{
	T *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++)
		if (e <= v[i]) 
			return false;

	return true;
}

template<class T> T Dot(const Vector<T> &V1, const Vector<T> &V2) 
{
	if (V1.nelem != V2.nelem) 
		throw jcommon::RuntimeException("dot(Vector, Vector): Nonconformant vectors");

	T val = 0,
		*v1 = V1.v + V1.vlo,
		*v2 = V2.v + V2.vlo;

	for (int i=0; i<V1.nelem; i++) 
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
		*v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) 
		val += v[i];

	return val;
}

template<class T> T Multiply(const Vector<T> &V) 
{
	T val = 1,
		*v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) 
		val *= v[i];

	return val;
}

template<class T> Vector<T> Pow(const Vector<T> &V, const T &e) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = pow(v[i], e);

	return V2;
}

template<class T> Vector<T> Pow(const Vector<T> &V, int e) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = pow(v[i], e);

	return V2;
}

template<class T> Vector<T> Abs(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = abs(v[i]);

	return V2;
}

template<class T> Vector<T> Sqrt(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = sqrt(v[i]);

	return V2;
}

template<class T> Vector<T> Sqr(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i]*v[i];

	return V2;
}

template<class T> Vector<T> AbsSqr(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = abs(v[i])*abs(v[i]);

	return V2;
}

template<class T> Vector<T> Cube(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i]*v[i]*v[i];

	return V2;
}

template<class T> Vector<T> Sin(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = sin(v[i]);

	return V2;
}

template<class T> Vector<T> Cos(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = cos(v[i]);

	return V2;
}

template<class T> Vector<T> Tan(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = tan(v[i]);

	return V2;
}

template<class T> Vector<T> Sinh(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = sinh(v[i]);

	return V2;
}

template<class T> Vector<T> Cosh(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = cosh(v[i]);

	return V2;
}

template<class T> Vector<T> Tanh(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = tanh(v[i]);

	return V2;
}

template<class T> Vector<T> Exp(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = exp(v[i]);

	return V2;
}

template<class T> Vector<T> Ln(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = log(v[i]);

	return V2;
}

template<class T> Vector<T> Log10(const Vector<T> &V) 
{
	T *v = V.v + V.vlo;
	Vector<T> V2(V.vlo, V.vhi);
	T *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
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
		*v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) 
		val += abs(v[i]);

	return val;
}

template<class T> T NormInf(const Vector<T> &V) 
{
	T val = 0,
		*v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) {
		if (abs(v[i]) > val) 
			val = abs(v[i]);
	}

	return val;
}

template<class T> T Norm2(const Vector<T> &V) 
{
	T val = 0,
		*v  = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) 
		val += abs(v[i])*abs(v[i]);

	return sqrt(val);
}

template<class T> Vector< std::complex<T> > Conj(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V.v + V.vlo;
	Vector< std::complex<T> > V2(V.vlo, V.vhi);
	std::complex<T> *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = std::complex<T>(real(v[i]), -imag(v[i]));

	return V2;
}

template<class T> Vector< std::complex<T> > Real(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V.v + V.vlo;
	Vector< std::complex<T> > V2(V.vlo, V.vhi);
	std::complex<T> *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i].real();

	return V2;
}

template<class T> Vector< std::complex<T> > Imag(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V.v + V.vlo;
	Vector< std::complex<T> > V2(V.vlo, V.vhi);
	std::complex<T> *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = v[i].imag();

	return V2;
}

template<class T> Vector< std::complex<T> > Arg(const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V.v + V.vlo;
	Vector< std::complex<T> > V2(V.vlo, V.vhi);
	std::complex<T> *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) {
		v2[i] = atan(imag(v[i])/real(v[i]));

		if (real(v[i]) < 0) 
			v2[i] = v2[i] + 3.1415926535897932;
	}

	return V2;
}

template<class T> Vector< std::complex<T> > Pow(const Vector< std::complex<T> > &V, const T &e) 
{
	std::complex<T> *v = V.v = V.vlo;
	Vector< std::complex<T> > V2(V.vlo, V.vhi);
	std::complex<T> *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
		v2[i] = pow(v[i], e);

	return V2;
}

template<class T> Vector< std::complex<T> > Pow(const T &e, const Vector< std::complex<T> > &V) 
{
	std::complex<T> *v = V.v = V.vlo;
	Vector< std::complex<T> > V2(V.vlo, V.vhi);
	std::complex<T> *v2 = V2.v + V2.vlo;

	for (int i=0; i<V.nelem; i++) 
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
	std::complex<T> *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) 
		val += abs(v[i]);

	return val;
}

template<class T> T NormInf(const Vector< std::complex<T> > &V) 
{
	T val = 0;
	std::complex<T> *v = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) {
		if (abs(v[i]) > val) 
			val = abs(v[i]);
	}

	return val;
}

template<class T> T Norm2(const Vector< std::complex<T> > &V) 
{
	T val = 0;
	std::complex<T> *v  = V.v + V.vlo;

	for (int i=0; i<V.nelem; i++) 
		val += abs(v[i])*abs(v[i]);

	return ::sqrt(val);
}

template<class T> class Matrix : public virtual jcommon::Object{

	public:
		T *m, 
			**v;
		int clo, 
			chi, 
			ncol, 
			rlo, 
			rhi, 
			nrow, 
			nelem;

		/** 
		 * \brief Construct new  Matrix of dimension 0 
		 *
		 */
		Matrix():
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Matrix");

			v = NULL;
			m = NULL;
			clo = 0;
			chi = 0;
			rlo = 0;
			rhi = 0;
			nrow = 0;
			ncol = 0;
		}

		/**
		 * \brief Copy constructor - copy by value
		 *
		 */
		Matrix(const Matrix &V):
			jcommon::Object() 
		{
			jcommon::Object::SetClassName("jmath::Matrix");

			v = NULL;
			m = NULL;
			clo = 0;
			chi = 0;
			rlo = 0;
			rhi = 0;
			nrow = 0;
			ncol = 0;
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
			delete [] m;
			delete [] (v + rlo);
		}

		void NewMatrix(int rlo, int rhi, int clo, int chi) 
		{
			if ( (rlo > rhi) || (clo > chi) )
				throw jcommon::RuntimeException("Matrix: Attempt to create Matrix with low index > high index");

			this->rlo = rlo;
			this->rhi = rhi;
			this->clo = clo;
			this->chi = chi;
			nrow = rhi-rlo+1; 
			ncol = chi-clo+1;
			nelem = nrow*ncol;
			m = new T[nelem];
			v = new T *[nrow];
			for (int i=0; i<nrow; i++) 
				v[i] = m + i*ncol - clo;
			v -= rlo;
		}

		void SetSize(int rlo, int rhi, int clo, int chi) 
		{
			delete [] m;
			delete [] (v+this->rlo);
		
			NewMatrix(rlo, rhi, clo, chi);
		}

		void SetSize(int rhi, int chi) 
		{
			// TODO:: assume matrix indices start at 1
			delete [] m;
			delete [] (v+this->rlo);

			NewMatrix(1, rhi, 1, chi);
		}

		void SetSize(const Matrix<T> &M) 
		{
			SetSize(M.rlo, M.rhi, M.clo, M.chi);
		}

		/**
		 * \brief returns a new Matrix with same values as *this, but with type of "other" matrix.
		 *
		 */
		template<class T2> Matrix<T2> Convert(Matrix<T2> & other) 
		{
			Matrix<T2> new_matrix(rlo, rhi, clo, chi);

			for (int i = rlo; i <= rhi; i++)
				for (int j = clo; j <= chi; j++)
					new_matrix(i, j) = v[i][j];

			return new_matrix;
		}

		void NewOffset(int newrlo, int newclo) 
		{
			v -= (newrlo - rlo);
			rlo = newrlo;
			rhi = newrlo + nrow - 1;
		
			for (int i=rlo; i<=rhi; i++) 
				v[i] -= (newclo - clo);
			
			clo = newclo;
			chi = newclo + ncol - 1;
		}

		int LowColumn() const 
		{
			return clo;
		}

		int HighColumn() const 
		{
			return chi;
		}

		int GetColumnSize() const 
		{
			return ncol;
		}

		int LowRow() const 
		{
			return rlo;
		}

		int HighRow() const 
		{
			return rhi;
		}

		int GetRowSize() const 
		{
			return nrow;
		}

		int GetSize() const 
		{
			return nelem;
		}

		/** 
		 * \brief Reference element  (i,j) 
		 *
		 */
		T &operator()(int i, int j) 
		{
			if ((i < rlo) || (i > rhi) || (j < clo) || (j > chi))
				throw jcommon::RuntimeException("Matrix::operator(): Element reference out of bounds");

			return v[i][j];
		}

		/** 
		 * \brief Reference element  (i,j) 
		 *
		 */ 
		const T &operator()(int i, int j) const 
		{
			if ((i < rlo) || (i > rhi) || (j < clo) || (j > chi))
				throw jcommon::RuntimeException("Matrix::operator(): Element reference out of bounds");

			return v[i][j];
		} 

		/** 
		 * \brief Return new  Matrix with range ( subrlo,  subrhi, subclo,  subchi) 
		 *
		 */
		Matrix<T> SubMatrix(int subrlo, int subrhi, int subclo, int subchi) 
		{
			if ( (subrlo < rlo) || (subrhi > rhi) || (subclo < clo) || (subchi > chi) )
				throw jcommon::RuntimeException("Matrix::submatrix: Invalid range selected");

			Matrix<T> sub;
			int row_range = subrhi-subrlo;
			int col_range = subchi-subclo;
			sub.NewMatrix(rlo, rlo+row_range, clo, clo+col_range);

			for (int i=sub.rlo; i<=sub.rhi; i++) 
				for (int j=sub.clo; j<=sub.chi; j++) 
					sub(i,j) = v[i+(subrlo-rlo)][j+(subclo-clo)];
			
			return sub;
		}

		Vector<T> Row(int r) 
		{
			if ( (r < rlo) || (r > rhi) )
				throw jcommon::RuntimeException("Matrix::row: Invalid row selected");

			Vector<T> sub;

			sub.NewVector(clo, chi);
			
			for (int i=sub.vlo; i<=sub.vhi; i++)
				sub[i] = v[r][i];
			
			return sub;
		}

		Vector<T> Column(int c) 
		{
			if ( (c < clo) || (c > chi) )
				throw jcommon::RuntimeException("Matrix::col: Invalid column selected");

			Vector<T> sub;

			sub.NewVector(rlo, rhi);
		
			for (int i=sub.vlo; i<=sub.vhi; i++)
				sub[i] = v[i][c];
			
			return sub;
		} 

		Vector<T> Pack() 
		{
			Vector<T> packed;

			packed.NewVector(1, nelem);
			
			T *v = packed.v;
		
			for (int i=0; i<nelem; i++) 
				v[i+1] = m[i];
			
			return packed;
		}

		T ** Raw() const 
		{
			return (v+rlo);
		}

		/** 
		 * \brief Assignment by value to scalar 
		 *
		 */
		Matrix<T> &operator=(T a) 
		{
			for (int i=0; i<nelem; i++) 
				m[i] = a;
		
			return *this;
		}

		/** 
		 * \brief Assignment by value to Matrix 
		 *
		 */
		Matrix<T> &operator=(const Matrix<T> &M) 
		{
			delete [] m;
			delete [] (v+rlo);
		
			NewMatrix(M.rlo, M.rhi, M.clo, M.chi);
			
			for (int i=0; i<M.nelem; i++) 
				m[i] = M.m[i];
			
			return *this;
		}

		/** 
		 * \brief Assignment by value to 1DP matrix 
		 *
		 */
		Matrix<T> &operator=(T *m) 
		{
			if (this->m == NULL) 
				throw jcommon::RuntimeException("Matrix::operator=(T *m):  Attempt to assign by value to a C array, when no dimensions have been allocated for the Matrix");
			
			for (int i=0; i<nelem; i++) 
				this->m[i] = m[i];

			return *this;
		}

		/** 
		 * \brief Assignment by value to 2DP matrix 
		 *
		 */
		Matrix<T> &operator=(T **m) 
		{
			if (this->m == NULL) 
				throw jcommon::RuntimeException("Matrix::operator=(T **m):  Attempt to assign by value to a C array, when no dimensions have been allocated for the Matrix");

			for (int i=0; i<nelem; i++) 
				this->m[i] = *(*m+i);

			for (int i=rlo; i<rlo+nrow; i++) 
				this->v[i] = this->m + (i-rlo)*ncol - clo;
			
			return *this;
		}

		/** 
		 * \brief Matrix addition. 
		 *
		 */
		Matrix<T> &operator+=(const Matrix<T> &M) 
		{
			if (M.nelem != nelem)
				throw jcommon::RuntimeException("Matrix::operator+=:  Attempt to add nonconformant Matrix");

			T *m2 = M.m;

			for (int i=0; i<nelem; i++) 
				m[i] += m2[i];
			
			return *this;
		}

		/** 
		 * \brief Matrix subtraction 
		 *
		 */
		Matrix<T> &operator-=(const Matrix<T> &M) 
		{
			if (M.nelem != nelem)
				throw jcommon::RuntimeException("Matrix::operator-=:  Attempt to subtract nonconformant Matrix");

			T *m2 = M.m;
			
			for (int i=0; i<nelem; i++) 
				m[i] -= m2[i];
			
			return *this;
		}

		/** 
		 * \brief Addition of scalar 
		 *
		 */
		Matrix<T> &operator+=(const T &e) 
		{
			for (int i=0; i<nelem; i++) 
				m[i] += e;

			return *this;
		}

		/**  
		 * \brief Subtraction of scalar 
		 *
		 */
		Matrix<T> &operator-=(const T &e) {
			for (int i=0; i<nelem; i++) 
				m[i] += e;

			return *this;
		}

		/** 
		 * \brief Multiplication by scalar
		 *
		 */
		Matrix<T> &operator*=(const T &e) 
		{
			for (int i=0; i<nelem; i++) 
				m[i] *= e;

			return *this;
		}

		/** 
		 * \brief Division by scalar 
		 *
		 */
		Matrix<T> &operator/=(const T &e) 
		{
			for (int i=0; i<nelem; i++) 
				m[i] /= e;

			return *this;
		}
};

template<class T> Matrix<T> operator-(const Matrix<T> &M) 
{
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m,
		*m = M.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = -m[i];
	
	return M2;
}

template<class T> Matrix<T> operator+(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("operator+(Matrix, Matrix):  Attempt to add nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;
	Matrix<T> M(M1.rlo, M1.rhi, M1.clo, M1.chi);
	T *m = M.m;

	for (int i=0; i<M1.nelem; i++) 
		m[i] = m1[i] + m2[i];
	
	return M;
}

template<class T> Matrix<T> operator+(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i] + e;
	
	return M2;
}

template<class T> Matrix<T> operator+(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = e + m[i];
	
	return M2;
}

template<class T> Matrix<T> operator-(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("operator-(Matrix, Matrix):  Attempt to subtract nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;
	Matrix<T> M(M1.rlo, M1.rhi, M1.clo, M1.chi);
	T *m = M.m;

	for (int i=0; i<M1.nelem; i++) 
		m[i] = m1[i] - m2[i];
	
	return M;
}

template<class T> Matrix<T> operator-(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i] - e;
	
	return M2;
}

template<class T> Matrix<T> operator-(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = e - m[i];
	
	return M2;
}

template<class T> Matrix<T> operator*(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i] * e;
	
	return M2;
}

template<class T> Matrix<T> operator*(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = e * m[i];
	
	return M2;
}

template<class T> Matrix<T> operator/(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i] / e;
	
	return M2;
}

template<class T> Matrix<T> operator/(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
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
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("elem_mul(Matrix, Matrix):  Attempt to element multiply nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;
	Matrix<T> M(M1.rlo, M1.rhi, M1.clo, M1.chi);
	T *m = M.m;

	for (int i=0; i<M1.nelem; i++) 
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
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("elem_div(Matrix, Matrix):  Attempt to element divide nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;
	Matrix<T> M(M1.rlo, M1.rhi, M1.clo, M1.chi);
	T *m = M.m;

	for (int i=0; i<M1.nelem; i++) 
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
	T *m = M.m,
		low = m[0];

	for (int i=1; i<M.nelem; i++) 
		if (low > m[i]) 
			low = m[i];
	
	return low;
}

template<class T> Matrix<T> Min(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("min(Matrix, Matrix):  Attempt to extract minimum from nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;
	Matrix<T> M(M1.rlo, M1.rhi, M1.clo, M1.chi);
	T *m = M.m;

	for (int i=0; i<M1.nelem; i++) 
		if (m1[i] <= m2[i]) 
			m[i] = m1[i];
		else 
			m[i] = m2[i];
	
	return M;
}

template<class T> Matrix<T> Min(const T &e, const Matrix<T> &M2) 
{
	T *m2 = M2.m;
	Matrix<T> M(M2.rlo, M2.rhi, M2.clo, M2.chi);
	T *m = M.m;

	for (int i=0; i<M2.nelem; i++) 
		if (e <= m2[i]) 
			m[i] = e;
		else 
			m[i] = m2[i];
	
	return M;
}

template<class T> Matrix<T> Min(const Matrix<T> &M2, const T &e) 
{
	T *m2 = M2.m;
	Matrix<T> M(M2.rlo, M2.rhi, M2.clo, M2.chi);
	T *m = M.m;
	
	for (int i=0; i<M2.nelem; i++) 
		if (m2[i] <= e) 
			m[i] = m2[i];
		else 
			m[i] = e;

	return M;
}

template<class T> T Max(const Matrix<T> &M) 
{
	T *m = M.m,
		hi = m[0];

	for (int i=1; i<M.nelem; i++)
		if (hi < m[i]) 
			hi = m[i];
	
	return hi;
}

template<class T> Matrix<T> Max(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("min(Matrix, Matrix):  Attempt to extract minimum from nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;
	Matrix<T> M(M1.rlo, M1.rhi, M1.clo, M1.chi);
	T *m = M.m;
	
	for (int i=0; i<M1.nelem; i++) 
		if (m1[i] >= m2[i]) 
			m[i] = m1[i];
		else 
			m[i] = m2[i];
	
	return M;
}

template<class T> Matrix<T> Max(const T &e, const Matrix<T> &M2) 
{
	T *m2 = M2.m;
	Matrix<T> M(M2.rlo, M2.rhi, M2.clo, M2.chi);
	T *m = M.m;

	for (int i=0; i<M2.nelem; i++) 
		if (e >= m2[i]) 
			m[i] = e;
		else 
			m[i] = m2[i];
	
	return M;
}

template<class T> Matrix<T> Max(const Matrix<T> &M2, const T &e) 
{
	T *m2 = M2.m;
	Matrix<T> M(M2.rlo, M2.rhi, M2.clo, M2.chi);
	T *m = M.m;

	for (int i=0; i<M2.nelem; i++) 
		if (m2[i] >= e) 
			m[i] = m2[i];
		else 
			m[i] = e;

	return M;
}

template<class T> bool operator==(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;

	for (int i=0; i<M1.nelem; i++)
		if (m1[i] != m2[i]) 
			return false;
	
	return true;
}

template<class T> bool operator==(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (m[i] != e) 
			return false;
	
	return true;
}

template<class T> bool operator==(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (e != m[i]) 
			return false;
	
	return true;
}

template<class T> bool operator<(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;
	
	for (int i=0; i<M1.nelem; i++)
		if (m1[i] >= m2[i]) 
			return false;
	
	return true;
}

template<class T> bool operator<(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (m[i] >= e) 
			return false;
	
	return true;
}

template<class T> bool operator<(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (e >= m[i]) 
			return false;
	
	return true;
}

template<class T> bool operator<=(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;

	for (int i=0; i<M1.nelem; i++)
		if (m1[i] > m2[i]) 
			return false;
	
	return true;
}

template<class T> bool operator<=(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (m[i] > e) 
			return false;
	
	return true;
}

template<class T> bool operator<=(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (e > m[i]) 
			return false;
	
	return true;
}

template<class T> bool operator>=(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		jcommon::RuntimeException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;

	for (int i=0; i<M1.nelem; i++)
		if (m1[i] < m2[i]) 
			return false;
	
	return true;
}

template<class T> bool operator>=(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (m[i] < e) 
			return false;
	
	return true;
}

template<class T> bool operator>=(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (e < m[i]) 
			return false;
	
	return true;
}

template<class T> bool operator>(const Matrix<T> &M1, const Matrix<T> &M2) 
{
	if (M1.nelem != M2.nelem)
		throw jcommon::RuntimeException("Matrix::operator==:  Attempt to compare nonconformant Matrices");

	T *m1 = M1.m,
		*m2 = M2.m;

	for (int i=0; i<M1.nelem; i++)
		if (m1[i] <= m2[i]) 
			return false;
	
	return true;
}

template<class T> bool operator>(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (m[i] <= e) 
			return false;
	
	return true;
}

template<class T> bool operator>(const T &e, const Matrix<T> &M) 
{
	T *m = M.m;

	for (int i=0; i<M.nelem; i++)
		if (e <= m[i]) 
			return false;
	
	return true;
}

template<class T> Matrix<T> Transpose(const Matrix<T> &M) 
{
	Matrix<T> M2(M.clo, M.chi, M.rlo, M.rhi);
	T **v2 = M2.v,
		**v = M.v;
	int clo = M.rlo,
			chi = M.rhi,
			rlo = M.clo,
			rhi = M.chi;

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
	int n = M.nrow;
	// reset M's offset for easy management.
	int rlo_orig = M.rlo;
	int clo_orig = M.clo;
	M.NewOffset(1, 1);
	exchanges.SetSize(n);
	int vlo_orig = exchanges.vlo;
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
			throw jcommon::RuntimeException("Attempt to factor a singular matrix");
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
			throw jcommon::RuntimeException("Attempt to factor a singular matrix");

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
	int rlo_orig = M.rlo;
	int clo_orig = M.clo;
	M.NewOffset(1, 1);

	// call lu_decomp_inplace.
	Vector<int> exchanges(1, M.nrow);
	LUDecompositionInplace(M, exchanges);

	// build return "U" matrix.
	int n = M.nrow;
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
	int n = LU.nrow;
	// reset the offsets to be 1-based, for convenience during the algorithm.
	int rlo_orig = LU.rlo;
	int clo_orig = LU.clo;
	int vlo_orig = exchanges.vlo;
	int b_vlo_orig = b.vlo;
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
	Vector<int> exchanges(1, M.nrow);
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
	int rlo_orig = M.rlo;
	int clo_orig = M.clo;
	int n = M.nrow;
	M.NewOffset(1, 1);
	Vector<int> exchanges(1, n);
	LUDecompositionInplace(M, exchanges);

	// Prepare (an empty) resultant matrix.
	Matrix<T> result(M.nrow, M.ncol);

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
	int n = M.nrow;
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
	if (M1.ncol != M2.nrow)
		throw jcommon::RuntimeException("operator*(Matrix, Matrix):  Attempt to multiply non-conformant matrices.");
	if ( (M1.rlo != M2.rlo) || (M1.clo != M2.clo) ) 
		throw jcommon::RuntimeException("operator*(Matrix, Matrix):  Attempt to multiply matrices with different starting indices.");

	Matrix<T> M3(M1.rlo, M1.rhi, M2.clo, M2.chi);
	T **v1 = M1.v;
	T **v2 = M2.v;
	T **v3 = M3.v;
	int imin = M1.rlo;
	int imax = M1.rhi;
	int jmin = M2.clo;
	int jmax = M2.chi;
	int kmin = M1.clo;
	int kmax = M1.chi;

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
	if (M.ncol != V.nelem)
		throw jcommon::RuntimeException("operator*(Matrix, Vector):  Attempt to multiply non-conformant matrix and vector.");
	if (M.clo != V.vlo) 
		throw jcommon::RuntimeException("operator*(Matrix, Vector):  Attempt to multiply matrix and vector with different starting indices.");

	Vector<T> V2(M.rlo, M.rhi);
	T **mv = M.v;
	T *vv = V.v;
	T *v2 = V2.v;
	int imin = M.rlo;
	int imax = M.rhi;
	int jmin = M.clo;
	int jmax = M.chi;

	for (int i=imin; i<=imax; i++) {
		v2[i] = 0;
	
		for (int j=jmin; j<=jmax; j++)  
			v2[i] += mv[i][j] * vv[j];
	}
	
	return V2;
}

template<class T> Vector<T> operator*(const Vector<T> &V, const Matrix<T> &M) 
{
	if (M.nrow != V.nelem)
		throw jcommon::RuntimeException("operator*(Vector, Matrix):  Attempt to multiply non-conformant vector and matrix.");
	if (M.rlo != V.vlo) 
		throw jcommon::RuntimeException("operator*(Vector, Matrix):  Attempt to multiply vector and matrix with different starting indices.");

	Vector<T> V2(M.clo, M.chi);
	T **mv = M.v;
	T *vv = V.v;
	T *v2 = V2.v;
	int imin = M.clo;
	int imax = M.chi;
	int jmin = M.rlo;
	int jmax = M.rhi;

	for (int i=imin; i<=imax; i++) {
		v2[i] = 0;
	
		for (int j=jmin; j<=jmax; j++)  
			v2[i] += vv[j] * mv[j][i];
	}
	
	return V2;
}

template<class T> T Trace(const Matrix<T> &M) 
{
	if (M.ncol != M.nrow)
		throw jcommon::RuntimeException("trace(Matrix):  Attempt to take trace of non-square matrix");

	T val = 0;
	T *m  = M.m;
	int inc = M.nrow;
	int imax = M.nelem;

	for (int i=0; i<imax; i+=inc+1) 
		val += m[i];
	
	return val;
}

template<class T> T Sum(const Matrix<T> &M) 
{
	T val = 0;
	T *m = M.m;

	for (int i=0; i<M.nelem; i++) 
		val += m[i];
	
	return val;
}

template<class T> Matrix<T> Pow(const Matrix<T> &M, const T &e) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = pow(m[i], e);
	
	return M2;
}

template<class T> Matrix<T> Pow(const Matrix<T> &M, int e) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = pow(m[i], e);
	
	return M2;
}

template<class T> Matrix<T> Abs(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = abs(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Sqrt(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = sqrt(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Sqr(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i]*m[i];
	
	return M2;
}

template<class T> Matrix<T> AbsSqr(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = abs(m[i])*abs(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Cube(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i]*m[i]*m[i];
	
	return M2;
}

template<class T> Matrix<T> Sin(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = sin(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Cos(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = cos(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Tan(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = tan(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Sinh(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = sinh(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Cosh(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = cosh(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Tanh(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;
	
	for (int i=0; i<M.nelem; i++) 
		m2[i] = tanh(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Exp(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = exp(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Ln(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = log(m[i]);
	
	return M2;
}

template<class T> Matrix<T> Log10(const Matrix<T> &M) 
{
	T *m = M.m;
	Matrix<T> M2(M.rlo, M.rhi, M.clo, M.chi);
	T *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
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
	T *m  = M.m;

	for (int i=0; i<M.nelem; i++) 
		val += abs(m[i])*abs(m[i]);
	
	return sqrt(val);
}

template<class T> T NormInf(const Matrix<T> &M) 
{
	int rlo = M.rlo;
	int rhi = M.rhi;
	int clo = M.clo;
	int chi = M.chi;
	T **v = M.v;
	T max = 0;

	for (int i=rlo; i<=rhi; i++) 
		for (int j=clo; j<=chi; j++) 
			if (abs(v[i][j]) > max) 
				max = abs(v[i][j]);
	
	return max;
} 

template<class T> T Norm1(const Matrix<T> &M) 
{
	int rlo = M.rlo;
	int rhi = M.rhi;
	int clo = M.clo;
	int chi = M.chi;
	T **v = M.v;
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
	std::complex<T> *m = M.m;
	Matrix< std::complex<T> > M2(M.rlo, M.rhi, M.clo, M.chi);
	std::complex<T> *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = std::complex<T>(real(m[i]),-imag(m[i]));
	
	return M2;
}

template<class T> Matrix< std::complex<T> > Real(const Matrix< std::complex<T> > &M) 
{
	std::complex<T> *m = M.m;
	Matrix< std::complex<T> > M2(M.rlo, M.rhi, M.clo, M.chi);
	std::complex<T> *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i].real();
	
	return M2;
}

template<class T> Matrix< std::complex<T> > Imag(const Matrix< std::complex<T> > &M) 
{
	std::complex<T> *m = M.m;
	Matrix< std::complex<T> > M2(M.rlo, M.rhi, M.clo, M.chi);
	std::complex<T> *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = m[i].imag();
	
	return M2;
}

template<class T> Matrix< std::complex<T> > Arg(const Matrix< std::complex<T> > &M) 
{
	std::complex<T> *m = M.m;
	Matrix< std::complex<T> > M2(M.rlo, M.rhi, M.clo, M.chi);
	std::complex<T> *m2 = M2.m;
	
	for (int i=0; i<M.nelem; i++) {
		m2[i] = atan(imag(m[i])/real(m[i]));
	
		if (real(m[i]) < 0) 
			m2[i] = m2[i] + 3.1415926535897932;
	}
	
	return M2;
}

template<class T> Matrix< std::complex<T> > Pow(const Matrix< std::complex<T> > &M, const T &e) 
{
	std::complex<T> *m = M.m;
	Matrix< std::complex<T> > M2(M.rlo, M.rhi, M.clo, M.chi);
	std::complex<T> *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
		m2[i] = pow(m[i], e);
	
	return M2;
}

template<class T> Matrix< std::complex<T> > Pow(const T &e, const Matrix< std::complex<T> > &M) 
{
	std::complex<T> *m = M.m;
	Matrix< std::complex<T> > M2(M.rlo, M.rhi, M.clo, M.chi);
	std::complex<T> *m2 = M2.m;

	for (int i=0; i<M.nelem; i++) 
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
	std::complex<T> *m  = M.m;

	for (int i=0; i<M.nelem; i++) 
		val += abs(m[i])*abs(m[i]);
	
	return sqrt(val);
}

template<class T> T NormInf(const Matrix< std::complex<T> > &M) 
{
	int rlo = M.rlo;
	int rhi = M.rhi;
	int clo = M.clo;
	int chi = M.chi;
	std::complex<T> **v = M.v;
	T max = 0;

	for (int i=rlo; i<=rhi; i++) 
		for (int j=clo; j<=chi; j++)  
			if (max < abs(v[i][j])) 
				max = abs(v[i][j]);
	
	return max;
} 

template<class T> T Norm1(const Matrix< std::complex<T> > &M) 
{
	int rlo = M.rlo;
	int rhi = M.rhi;
	int clo = M.clo;
	int chi = M.chi;
	std::complex<T> **v = M.v;
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
	Matrix< std::complex<T> > M2(M.clo, M.chi, M.rlo, M.rhi);
	std::complex<T> **v2 = M2.v;
	std::complex<T> **v = M.v;
	int clo = M.rlo;
	int chi = M.rhi;
	int rlo = M.clo;
	int rhi = M.chi;

	for (int i=rlo; i<=rhi; i++) 
		for (int j=clo; j<=chi; j++)
			v2[i][j] = std::complex<T>(real(v[j][i]), -imag(v[j][i]));

	return M2;
}

template<class T> std::ostream& operator<<(std::ostream& os, const Vector<T> &V) 
{
	for (int i=V.vlo; i<=V.vhi; i++) 
		os << V(i) << " ";
	
	os << std::endl;
	
	return os;
}

template<class T> std::istream& operator>>(std::istream& os, Vector<T> &V) 
{
	for (int i=V.vlo; i<=V.vhi; i++) 
		os >> V(i);

	return os;
} 

template<class T> std::ostream& operator<<(std::ostream& os, const Matrix<T> &M) 
{
	for (int i=M.rlo; i<=M.rhi; i++) {
		for (int j=M.clo; j<=M.chi; j++)
			os << M(i,j) << " ";
		os << std::endl;
	}

	return os;
}

template<class T> std::istream& operator>>(std::istream& os, Matrix<T> &M) 
{
	for (int i=M.rlo; i<=M.rhi; i++) 
		for (int j=M.clo; j<=M.chi; j++)
			os >> M(i,j);

	return os;
}

}

#endif

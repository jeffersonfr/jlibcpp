/***************************************************************************
 *   Copyright (C) 2005 <still unknown>                                    *
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
 ***************************************************************************/
#ifndef J_GC_H
#define J_GC_H

#include "jruntimeexception.h"

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#if _MSC_VER >= 1000
#pragma once
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef _SMGC_POOR_STL
#include <cstdlib>
#include <memory>
#include <climits>
#include <cstring>
#include <limits>
#else
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <string.h>
#endif

#include <set>
#include <map>
#include <vector>
#include <deque>
#include <utility>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <new>

#ifndef _SMGC_POOR_STL
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#include <stddef.h>
#endif

#ifndef _SMGC_POOR_STL
#include <cstdlib>
#include <cstddef>
#include <climits>
#include <cassert>
#else
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>
#endif

#if defined(_MSC_VER) && (_MSVC <= 1200)
#define _SMGC_NO_FRIEND_TEMPLATES
#define _SMGC_NO_ARRAY_NEW
#define _SMGC_POOR_TEMPLATES
#endif

#if defined(_SMGC_POOR_TEMPLATES) && !defined(_SMGC_NO_FRIEND_TEMPLATES)
#define _SMGC_NO_FRIEND_TEMPLATES
#endif

#define ASSERT_VALID if(0){}else
#define ASSERT_SUPER_VALID if(0){}else
#define ASSERT_PTR_VALID(_p_)  if(0){}else
#ifndef NDEBUG
# ifdef DEBUG_COMPILER
#  undef ASSERT_VALID
#  undef ASSERT_SUPER_VALID
#  undef ASSERT_PTR_VALID
#  define ASSERT_VALID assert(!destroyed)
#  define ASSERT_SUPER_VALID assert(!super::destroyed)
#  define ASSERT_PTR_VALID(_p_) assert(!(_p_).destroyed)
# endif
#endif

namespace jcommon {

template<typename T> class gc_ptr;
template<typename T> class const_gc_ptr;
template<typename T> class gc_arr;
template<typename T> class const_gc_arr;
template<typename T> class wk_ptr;
template<typename T> class const_wk_ptr;
template<typename T> class wk_arr;
template<typename T> class const_wk_arr;

void collect();
void set_threshold(size_t bytes);
size_t get_threshold();
size_t get_dynamic_threshold();

extern const size_t no_threshold;
extern const size_t default_threshold;

struct gc_t {
};

struct gc_null_t {
	operator void* () const
	{
		return 0;
	}
};

template <typename T>struct destructor {
	static void destroy(void* obj, void* base, size_t)
	{
		T* ptr = static_cast<T*>(obj);
		ptr->~T();
		::operator delete(base);
	}

	static void destroy_array(void* obj, void* base, size_t size)
	{
		T* arr = static_cast<T*>(obj);
		for(size_t i = 0; i < size/sizeof(T); ++i)
		{
			arr[i].~T();
		}
		::operator delete[](base);
	}
};

struct node_base {
};

struct pointer_base {
	int pos;
#ifndef NDEBUG
	bool destroyed;
#ifdef DEBUG_COMPILER
	char *trash;
	//void assert_valid()
	//{ ASSERT_VALID; }
#endif
#endif

	protected:
	pointer_base();
	pointer_base(const pointer_base& pb);
	virtual ~pointer_base();

	void operator= (const pointer_base& pb) 
	{
	}

	void reset_node(const void* obj, void (*destroy)(void*, void*, size_t));

	void swap(pointer_base &pb)
	{ 
		ASSERT_VALID; 
	}
};

struct weak_base {
	int pos;
#ifndef NDEBUG
	bool destroyed;
#ifdef DEBUG_COMPILER
	char *trash;
#endif
#endif

	protected:
	weak_base();
	weak_base(const weak_base& pb);
	virtual ~weak_base();

	void operator= (const weak_base& pb) 
	{
	}
};

void reg(void *obj, size_t size, void (*destroy)(void*, void*, size_t));

struct dynamic_cast_t {
};

struct static_cast_t {
};

struct const_cast_t {
};

struct light_cons_t {
};

// Pointer registation function, allocated memory must be freeable using supplied destroy function
template<typename T>T* reg(T *ptr, void (*destroy)(void*, void*, size_t))
{
	reg(static_cast<void*>(ptr), sizeof(T), destroy);
	return ptr;
}

// Array pointer registation function, allocated memory must be freeable using supplied destroy function
template<typename T>T* reg(T arr[], size_t len, void (*destroy)(void*, void*, size_t))
{
	reg(static_cast<void*>(arr), sizeof(T)*len, destroy);
	return arr;
}

/* Implicit cast operation */
template<typename T, typename U> T implicit_cast(U u) 
{
   	return u; 
}

// Managed pointer class template 
template<typename T>class const_gc_ptr: public pointer_base {
#ifdef _SMGC_NO_FRIEND_TEMPLATES
	public:
#else
	protected:
		template<typename U> friend class const_gc_ptr;
		template<typename U> friend class gc_ptr;
		template<typename U> friend class const_wk_ptr;
		template<typename U> friend class wk_ptr;
		friend struct std::less<const_gc_ptr<T> >;
		friend struct std::less<gc_ptr<T> >;
#endif
		friend void collect();

		T* ptr;

	public:
		typedef T element_type;

	private:
		typedef pointer_base super;

	protected:
		template <typename U>void reset_node(const U* obj)
		{
			super::reset_node(obj, destructor<U>::destroy);
		}

		const_gc_ptr(const light_cons_t&) 
		{
		}

	public:
		const_gc_ptr(): ptr(0) 
		{
		}

		const_gc_ptr(const gc_null_t&): ptr(0) 
		{
		}

		template<typename U>explicit const_gc_ptr(const U *p)//: ptr(0)
		{
			reset(p);
		}

		template<typename U>const_gc_ptr(const U *p, const dynamic_cast_t*)//: ptr(0)
		{
			reset(dynamic_cast<T*>(p));
		}

		template<typename U>const_gc_ptr(const U *p, const static_cast_t*)//: ptr(0)
		{
			reset(static_cast<T*>(p));
		}

		const_gc_ptr(const T *p)//: ptr(0)
		{
			reset(p);
		}

		template<typename U>const_gc_ptr(const const_gc_ptr<U> &p): super(p), ptr(implicit_cast<T*>(p.ptr)) 
		{
		}

		template<typename U>const_gc_ptr(const const_gc_ptr<U> &p, const dynamic_cast_t*):
				super(p), ptr(dynamic_cast<T*>(p.ptr)) 
		{
		}

		template<typename U>const_gc_ptr(const const_gc_ptr<U> &p, const static_cast_t*):
				super(p), ptr(static_cast<T*>(p.ptr)) 
		{
		}

#ifndef NDEBUG
		virtual ~const_gc_ptr()
		{
			ptr=0;
		}
#endif

		const_gc_ptr<T>& operator = (const gc_null_t&)
		{
			ASSERT_VALID;
			ptr = 0;
			return *this;
		}

		template<typename U>const_gc_ptr<T>& operator = (const U *p)
		{
			ASSERT_VALID;
			reset(p);
			return *this;
		}

		template<typename U>const_gc_ptr<T>& operator = (const const_gc_ptr<U> &p)
		{
			ASSERT_VALID;
			ptr = p.ptr;
			return *this;
		}

		const T& operator * () const
		{
			ASSERT_VALID;
			return *ptr;
		}

		const T* operator -> () const
		{
			ASSERT_VALID;
			return ptr;
		}

		const T* get() const
		{
			ASSERT_VALID;
			return ptr;
		}

		bool operator ! () const
		{
			ASSERT_VALID;
			return ptr==0;
		}

		operator bool () const
		{
			ASSERT_VALID;
			return ptr!=0;
		}

		bool operator ==(const gc_null_t&) const
		{
			ASSERT_VALID;
			return ptr == 0;
		}

		bool operator !=(const gc_null_t&) const
		{
			ASSERT_VALID;
			return ptr != 0;
		}

		template<typename U>bool operator ==(const const_gc_ptr<U> &p) const
		{
			ASSERT_VALID;
			return ptr == p.ptr;
		}

		template<typename U>bool operator !=(const const_gc_ptr<U> &p) const
		{
			ASSERT_VALID;
			return ptr != p.ptr;
		}

		template<typename U>bool operator <=(const const_gc_ptr<U> &p) const
		{
			ASSERT_VALID;
			return ptr <= p.ptr;
		}

		template<typename U>bool operator >=(const const_gc_ptr<U> &p) const
		{
			ASSERT_VALID;
			return ptr >= p.ptr;
		}

		template<typename U>bool operator <(const const_gc_ptr<U> &p) const
		{
			ASSERT_VALID;
			return ptr < p.ptr;
		}

		template<typename U>bool operator >(const const_gc_ptr<U> &p) const
		{
			ASSERT_VALID;
			return ptr > p.ptr;
		}


		template<typename U>bool operator ==(const U *p) const
		{
			ASSERT_VALID;
			return ptr == p;
		}

		template<typename U>bool operator !=(const U *p) const
		{
			ASSERT_VALID;
			return ptr != p;
		}

#ifdef _SMGC_PTR_COMPARABLE
		template<typename U>bool operator <=(const U *p) const
		{
			ASSERT_VALID;
			return ptr <= p;
		}

		template<typename U>bool operator >=(const U *p) const
		{
			ASSERT_VALID;
			return ptr >= p;
		}

		template<typename U>bool operator <(const U *p) const
		{
			ASSERT_VALID;
			return ptr < p;
		}

		template<typename U>bool operator >(const U *p) const
		{
			ASSERT_VALID;
			return ptr > p;
		}
#endif

		void reset(const T* p=0)
		{
			ASSERT_VALID;
			reset_node(p);
			ptr = const_cast<T*>(p);
		}

		void swap(const_gc_ptr &p)
		{
			ASSERT_VALID;
			T* tmp = ptr;
			ptr = p.ptr;
			p.ptr = tmp;
			super::swap(p);
		}
};


// Managed pointer class template 
template<typename T>class gc_ptr: public const_gc_ptr<T> {
	public:
		typedef T element_type;

	private:
		typedef const_gc_ptr<T> super;

	public:
		gc_ptr() 
		{
		}

		gc_ptr(const gc_null_t&) 
		{
		}

		template<typename U>explicit gc_ptr(U *p): super(light_cons_t())
		{
			reset(p);
		}

		template<typename U>gc_ptr(U *p, const dynamic_cast_t*): super(light_cons_t())
		{
			reset(dynamic_cast<T*>(p));
		}

		template<typename U>gc_ptr(U *p, const static_cast_t*): super(light_cons_t())
		{
			reset(static_cast<T*>(p));
		}

		template<typename U>gc_ptr(const U *p, const const_cast_t*): super(light_cons_t())
		{
			reset(const_cast<T*>(p));
		}

		gc_ptr(T *p): super(light_cons_t())
		{
			reset(p);
		}

		template<typename U>gc_ptr(const gc_ptr<U> &p): super(p) 
		{
		}

		template<typename U>gc_ptr(const gc_ptr<U> &p, const dynamic_cast_t*):
				super(p, static_cast<dynamic_cast_t*>(0)) 
		{
		}

		template<typename U>gc_ptr(const gc_ptr<U> &p, const static_cast_t*):
				super(p, static_cast<static_cast_t*>(0)) 
		{
		}

		template<typename U>gc_ptr(const const_gc_ptr<U> &p, const const_cast_t*):
				super(p) 
		{
		}

#ifndef NDEBUG
		virtual ~gc_ptr()
		{
			ASSERT_SUPER_VALID;
			super::ptr=0;
		}
#endif

		gc_ptr<T>& operator = (const gc_null_t&)
		{
			ASSERT_SUPER_VALID;
			super::ptr = 0;
			return *this;
		}

		template<typename U>gc_ptr<T>& operator = (U *p)
		{
			ASSERT_SUPER_VALID;
			reset(p);
			return *this;
		}

		template<typename U>gc_ptr<T>& operator = (const gc_ptr<U> &p)
		{
			ASSERT_SUPER_VALID;
			super::ptr = const_cast<U*>(p.ptr);
			return *this;
		}

		T& operator * () const
		{
			ASSERT_SUPER_VALID;
			return *super::ptr;
		}

		T* operator -> () const
		{
			ASSERT_SUPER_VALID;
			return super::ptr;
		}

		T* get() const
		{
			ASSERT_SUPER_VALID;
			return super::ptr;
		}

		void reset(T* p=0)
		{
			ASSERT_SUPER_VALID;
			reset_node(p);
			super::ptr = p;
		}

		void swap(gc_ptr &p)
		{
			ASSERT_SUPER_VALID;
			T* tmp = super::ptr;
			super::ptr = p.ptr;
			p.ptr = tmp;
			super::swap(p);
		}
};

template<typename U>bool operator ==(const gc_null_t &l, const const_gc_ptr<U> &p)
{
	return p.operator ==(l);
}

template<typename U>bool operator !=(const gc_null_t &l, const const_gc_ptr<U> &p)
{
	return p.operator !=(l);
}

template<typename T, typename U>bool operator ==(const T *l, const const_gc_ptr<U> &p)
{
	return p.operator ==(l);
}

template<typename T, typename U>bool operator !=(const T *l, const const_gc_ptr<U> &p)
{
	return p.operator !=(l);
}

#ifdef _SMGC_PTR_COMPARABLE
template<typename T, typename U>bool operator <=(const T *l, const const_gc_ptr<U> &p)
{
	return p.operator >=(l);
}

template<typename T, typename U>bool operator >=(const T *l, const const_gc_ptr<U> &p)
{
	return p.operator <=(l);
}

template<typename T, typename U>bool operator <(const T *l, const const_gc_ptr<U> &p)
{
	return p.operator >(l);
}

template<typename T, typename U>bool operator >(const T *l, const const_gc_ptr<U> &p)
{
	return p.operator <(l);
}
#endif

/* Smartpointer cast operations */
template<typename T, typename U>gc_ptr<T> dynamic_cast_gc_ptr(U *u)
{
	return gc_ptr<T>(u, static_cast<dynamic_cast_t*>(0));
}

template<typename T, typename U>const_gc_ptr<T> dynamic_cast_gc_ptr(const U *u)
{
	return const_gc_ptr<T>(u, static_cast<dynamic_cast_t*>(0));
}

template<typename T, typename U>gc_ptr<T> dynamic_cast_gc_ptr(const gc_ptr<U> &u)
{
	return gc_ptr<T>(u, static_cast<dynamic_cast_t*>(0));
}

template<typename T, typename U>const_gc_ptr<T> dynamic_cast_gc_ptr(const const_gc_ptr<U> &u)
{
	return const_gc_ptr<T>(u, static_cast<dynamic_cast_t*>(0));
}

template<typename T, typename U>gc_ptr<T> static_cast_gc_ptr(U *u)
{
	return gc_ptr<T>(u, static_cast<static_cast_t*>(0));
}

template<typename T, typename U>const_gc_ptr<T> static_cast_gc_ptr(const U *u)
{
	return const_gc_ptr<T>(u, static_cast<static_cast_t*>(0));
}

template<typename T, typename U>gc_ptr<T> static_cast_gc_ptr(const gc_ptr<U> &u)
{
	return gc_ptr<T>(u, static_cast<static_cast_t*>(0));
}

template<typename T, typename U>const_gc_ptr<T> static_cast_gc_ptr(const const_gc_ptr<U> &u)
{
	return const_gc_ptr<T>(u, static_cast<static_cast_t*>(0));
}

template<typename T, typename U>gc_ptr<T> const_cast_gc_ptr(const U *u)
{
	return gc_ptr<T>(u, static_cast<const_cast_t*>(0));
}

template<typename T, typename U>gc_ptr<T> const_cast_gc_ptr(const const_gc_ptr<U> &u)
{
	return gc_ptr<T>(u, static_cast<const_cast_t*>(0));
}

/* Managed const array class template */
template<typename T>class const_gc_arr: public pointer_base
{
	protected:
		friend class gc_arr<T>;
		friend class const_wk_arr<T>;
		friend class wk_arr<T>;
#ifndef _SMGC_POOR_TEMPLATES
		friend struct std::less<const_gc_arr<T> >;
		friend struct std::less<gc_arr<T> >;
#endif
		T* ptr;

	public:
		typedef T element_type;

	private:
		typedef pointer_base super;

	protected:
		void reset_node(const T* obj)
		{
			super::reset_node(obj, destructor<T>::destroy_array);
		}

		const_gc_arr(const light_cons_t&) 
		{
		}

	public:
		const_gc_arr(): ptr(0)
		{
		}

		const_gc_arr(const gc_null_t&): ptr(0)
		{
		}

		/*explicit*/ const_gc_arr(const T p[])//: ptr(0)
		{
			reset(p);
		}

#ifndef NDEBUG
		virtual ~const_gc_arr()
		{
			ptr = 0;
		}
#endif

		const_gc_arr& operator = (const gc_null_t&)
		{
			ptr = 0;
			return *this;
		}

		const_gc_arr& operator = (const T p[])
		{
			reset(p);
			return *this;
		}

		const T& operator [] (size_t i) const
		{
			return ptr[i];
		}

		const T* get() const
		{
			return ptr;
		}

		bool operator ! () const
		{
			return ptr == 0;
		}

		operator bool () const
		{
			return ptr != 0;
		}

		bool operator ==(const gc_null_t&) const
		{
			return ptr == 0;
		}

		bool operator !=(const gc_null_t&) const
		{
			return ptr != 0;
		}

		template<typename U>bool operator ==(const const_gc_arr<U> &p) const
		{
			return ptr == p.ptr;
		}

		template<typename U>bool operator !=(const const_gc_arr<U> &p) const
		{
			return ptr != p.ptr;
		}

		template<typename U>bool operator <=(const const_gc_arr<U> &p) const
		{
			return ptr <= p.ptr;
		}

		template<typename U>bool operator >=(const const_gc_arr<U> &p) const
		{
			return ptr >= p.ptr;
		}

		template<typename U>bool operator <(const const_gc_arr<U> &p) const
		{
			return ptr < p.ptr;
		}

		template<typename U>bool operator >(const const_gc_arr<U> &p) const
		{
			return ptr > p.ptr;
		}

		template<typename U>bool operator ==(const U *p) const
		{
			return ptr == p;
		}

		template<typename U>bool operator !=(const U *p) const
		{
			return ptr != p;
		}

		template<typename U>bool operator <=(const U *p) const
		{
			return ptr <= p;
		}

		template<typename U>bool operator >=(const U *p) const
		{
			return ptr >= p;
		}

		template<typename U>bool operator <(const U *p) const
		{
			return ptr < p;
		}

		template<typename U>bool operator >(const U *p) const
		{
			return ptr > p;
		}

		const_gc_arr& operator += (ptrdiff_t d)
		{
			ptr += d;
			return *this;
		}

		const_gc_arr& operator -= (ptrdiff_t d)
		{
			ptr -= d;
			return *this;
		}

		const_gc_arr& operator ++ ()
		{
			++ptr;
			return *this;
		}

		const_gc_arr& operator -- ()
		{
			--ptr;
			return *this;
		}

		const_gc_arr operator ++ (int)
		{
			const_gc_arr<T> old(*this);
			++ptr;
			return old;
		}

		const_gc_arr operator -- (int)
		{
			const_gc_arr<T> old(*this);
			--ptr;
			return old;
		}

		const_gc_arr operator + (ptrdiff_t d) const
		{
			const_gc_arr<T> res(*this);
			res += d;
			return res;
		}

		const_gc_arr operator - (ptrdiff_t d) const
		{
			const_gc_arr<T> res(*this);
			res -= d;
			return res;
		}

		void reset(const T* p=0)
		{
			reset_node(p);
			ptr = const_cast<T*>(p);
		}

		void swap(const_gc_arr &p)
		{
			T* tmp = ptr;
			ptr = p.ptr;
			p.ptr = tmp;
			super::swap(p);
		}
};

/* Managed array class template */
template<typename T>class gc_arr: public const_gc_arr<T>
{
	public:
		typedef T element_type;

	private:
		typedef const_gc_arr<T> super;

	public:
		gc_arr()
		{
		}

		gc_arr(const gc_null_t&)
		{
		}

		/*explicit*/ gc_arr(T p[]): super(light_cons_t())
		{
			reset(p);
		}

#ifndef NDEBUG
		virtual ~gc_arr()
		{
			super::ptr = 0;
		}
#endif

		gc_arr& operator = (const gc_null_t&)
		{
			super::ptr = 0;
			return *this;
		}

		gc_arr& operator = (T p[])
		{
			reset(p);
			return *this;
		}

		T& operator [] (size_t i) const
		{
			return super::ptr[i];
		}

		T* get() const
		{
			return super::ptr;
		}

		gc_arr& operator += (ptrdiff_t d)
		{
			super::ptr += d;
			return *this;
		}

		gc_arr& operator -= (ptrdiff_t d)
		{
			super::ptr -= d;
			return *this;
		}

		gc_arr& operator ++ ()
		{
			++super::ptr;
			return *this;
		}

		gc_arr& operator -- ()
		{
			--super::ptr;
			return *this;
		}

		gc_arr operator ++ (int)
		{
			gc_arr<T> old(*this);
			++super::ptr;
			return old;
		}

		gc_arr operator -- (int)
		{
			gc_arr<T> old(*this);
			--super::ptr;
			return old;
		}

		gc_arr operator + (ptrdiff_t d) const
		{
			gc_arr<T> res(*this);
			res += d;
			return res;
		}

		gc_arr operator - (ptrdiff_t d) const
		{
			gc_arr<T> res(*this);
			res -= d;
			return res;
		}

		void reset(T* p=0)
		{
			reset_node(p);
			super::ptr = /*const_cast<T*>*/(p);
		}

		void swap(gc_arr &p)
		{
			T* tmp = super::ptr;
			super::ptr = p.ptr;
			p.ptr = tmp;
			super::swap(p);
		}
};

template<typename U>bool operator ==(const gc_null_t &l, const const_gc_arr<U> &p)
{
	return p.operator ==(l);
}

template<typename U>
bool operator !=(const gc_null_t &l, const const_gc_arr<U> &p)
{
	return p.operator !=(l);
}

template<typename T, typename U>bool operator ==(const T *l, const const_gc_arr<U> &p)
{
	return p.operator ==(l);
}

template<typename T, typename U>bool operator !=(const T *l, const const_gc_arr<U> &p)
{
	return p.operator !=(l);
}

template<typename T, typename U>bool operator <=(const T *l, const const_gc_arr<U> &p)
{
	return p.operator >=(l);
}

template<typename T, typename U>bool operator >=(const T *l, const const_gc_arr<U> &p)
{
	return p.operator <=(l);
}

template<typename T, typename U>bool operator <(const T *l, const const_gc_arr<U> &p)
{
	return p.operator >(l);
}

template<typename T, typename U>bool operator >(const T *l, const const_gc_arr<U> &p)
{
	return p.operator <(l);
}

template<typename T>const_gc_arr<T> operator +(ptrdiff_t l, const const_gc_arr<T> &p)
{
	return p+l;
}

template<typename T>gc_arr<T> operator +(ptrdiff_t l, const gc_arr<T> &p)
{
	return p+l;
}

/* Const weak pointer template */
template<typename T>class const_wk_ptr: public weak_base
{
#ifdef _SMGC_NO_FRIEND_TEMPLATES
	public:
#else
	protected:
		template<typename U> friend class const_wk_ptr;
		template<typename U> friend class wk_ptr;
#endif
		friend void collect();

		T *ptr;

	public:
		const_wk_ptr(): ptr(0) 
		{
		}

		const_wk_ptr(const gc_null_t&): ptr(0) 
		{
		}

		template<typename U>const_wk_ptr(const const_wk_ptr<U> &p): ptr(implicit_cast<T*>(p.ptr)) 
		{
		}

		template<typename U>const_wk_ptr(const const_gc_ptr<U> &p): ptr(implicit_cast<T*>(p.ptr)) 
		{
		}

		template<typename U>const_wk_ptr(const const_gc_ptr<U> &p, const dynamic_cast_t*):
				ptr(dynamic_cast<T*>(p.ptr)) 
		{
		}

		template<typename U>const_wk_ptr(const const_gc_ptr<U> &p, const static_cast_t*):
				ptr(static_cast<T*>(p.ptr)) 
		{
		}

#ifndef NDEBUG
		virtual ~const_wk_ptr()
		{
			ptr=0;
		}
#endif

		const_wk_ptr<T>& operator = (const gc_null_t&)
		{
			ptr = 0;
			return *this;
		}

		template<typename U>const_wk_ptr<T>& operator = (const const_gc_ptr<U> &p)
		{
			ptr = const_cast<U*>(p.ptr);
			return *this;
		}

		const_gc_ptr<T> get() const
		{
			return const_gc_ptr<T>(ptr);
		}
};

template<typename T>class wk_ptr: public const_wk_ptr<T>
{
	typedef const_wk_ptr<T> super;

	public:
	wk_ptr(const gc_null_t&) 
	{
	}

	template<typename U>
		wk_ptr(const wk_ptr<U> &p): super(p) 
	{
	}

	template<typename U>
		wk_ptr(const gc_ptr<U> &p): super(p) 
	{
	}

	template<typename U>
		wk_ptr(const gc_ptr<U> &p, const dynamic_cast_t*):
			super(p, static_cast<dynamic_cast_t*>(0)) 
	{
	}

	template<typename U>
		wk_ptr(const gc_ptr<U> &p, const static_cast_t*):
			super(p, static_cast<static_cast_t*>(0)) 
	{
	}

	template<typename U>
		wk_ptr(const const_gc_ptr<U> &p, const const_cast_t*):
			super(p) 
	{
	}

	wk_ptr<T>& operator = (const gc_null_t &nt)
	{
		return super::operator=(nt);
	}

	template<typename U>wk_ptr<T>& operator = (const gc_ptr<U> &p)
	{
		super::ptr = const_cast<U*>(p.ptr);
		return *this;
	}

	//using super::get;

	gc_ptr<T> get() const
	{
		return gc_ptr<T>(super::ptr);
	}
};

/* Const weak array template */
template<typename T>class const_wk_arr: public weak_base
{
	protected:
		friend class wk_arr<T>;
		friend void collect();

		T *ptr;

	public:
		const_wk_arr(): ptr(0) 
		{
		}

		const_wk_arr(const gc_null_t&): ptr(0) 
		{
		}

		const_wk_arr(const const_gc_arr<T> &p): ptr(p.ptr) 
		{
		}

#ifndef NDEBUG
		virtual ~const_wk_arr()
		{
			ptr=0;
		}
#endif

		const_wk_arr<T>& operator = (const gc_null_t&)
		{
			ptr = 0;
			return *this;
		}

		const_wk_arr<T>& operator = (const const_gc_arr<T> &p)
		{
			ptr = const_cast<T*>(p.ptr);
			return *this;
		}

		const_gc_arr<T> get() const
		{
			return const_gc_arr<T>(ptr);
		}
};

template<typename T>class wk_arr: public const_wk_arr<T>
{
	typedef const_wk_arr<T> super;

	public:
		wk_arr(const gc_null_t&) 
		{
		}

		template<typename U>wk_arr(const wk_arr<U> &p): super(p) 
		{
		}

		template<typename U>wk_arr(const gc_arr<U> &p): super(p) 
		{
		}

		template<typename U>wk_arr(const const_gc_arr<U> &p, const const_cast_t*):
				super(p) 
		{
		}

		wk_arr<T>& operator = (const gc_null_t &nt)
		{
			return super::operator=(nt);
		}

		wk_arr<T>& operator = (const gc_arr<T> &p)
		{
			super::ptr = const_cast<T*>(p.ptr);
			return *this;
		}

		//using super::get;

		gc_arr<T> get() const
		{
			return gc_arr<T>(super::ptr);
		}
};

gc_null_t NIL;
gc_t GC;
gc_null_t GC_NIL;

template<typename P>struct ptr_rev {
	P *ptr;
	ptr_rev(P *p=0): ptr(p) {}

	ptr_rev& operator=(P *p)
	{
		ptr = p;
		return *this;
	}

	P& operator*() 
	{
	   	return *ptr; 
	}
	
	const P& operator*() const 
	{
	   	return *ptr; 
	}
	
	P* operator->() 
	{
	   	return ptr; 
	}
	
	const P* operator->() const
   	{ 
		return ptr; 
	}

	ptr_rev& operator--() 
	{
	   	++ptr; return *this; 
	}
	
	ptr_rev& operator++() 
	{
	   	--ptr; return *this; 
	}

	ptr_rev operator--(int) 
	{
	   	ptr_rev<P> p=*this; ++ptr; return p; 
	}
	
	ptr_rev operator++(int) 
	{
	   	ptr_rev<P> p=*this; --ptr; return p; 
	}

	ptr_rev& operator-=(ptrdiff_t d) 
	{
	   	ptr+=d; return *this; 
	}
	
	ptr_rev& operator+=(ptrdiff_t d) 
	{
	   	ptr-=d; return *this; 
	}
	
	ptrdiff_t operator-(ptr_rev& p) 
	{
	   	return p.ptr-ptr; 
	}

	bool operator<(const ptr_rev& p) 
	{
	   	return p.ptr < ptr; 
	}
	
	bool operator<=(const ptr_rev& p) 
	{
	   	return p.ptr <= ptr; 
	}
	
	bool operator>(const ptr_rev& p) 
	{
	   	return p.ptr > ptr; 
	}
	
	bool operator>=(const ptr_rev& p) 
	{
	   	return p.ptr >= ptr; 
	}
	
	bool operator==(const ptr_rev& p) 
	{
	   	return p.ptr == ptr; 
	}
	
	bool operator!=(const ptr_rev& p) 
	{
	   	return p.ptr != ptr; 
	}
	
	bool operator==(P *p) 
	{
	   	return p == ptr; 
	}
	
	bool operator!=(P *p) 
	{
	   	return p != ptr; 
	}

};

template<typename P>inline ptr_rev<P> operator+(const ptr_rev<P> &p, ptrdiff_t d)
{
	ptr_rev<P> r = p.ptr-d;
	return r;
}

template<typename P>inline ptr_rev<P> operator+(ptrdiff_t d, const ptr_rev<P> &p)
{
	ptr_rev<P> r = d-p.ptr;
	return r;
}

template<typename P>inline ptr_rev<P> operator-(const ptr_rev<P> &p, ptrdiff_t d)
{
	ptr_rev<P> r = p.ptr+d;
	return r;
}

template<typename P>inline ptr_rev<P> operator-(ptrdiff_t d, const ptr_rev<P> &p)
{
	ptr_rev<P> r = d+p.ptr;
	return r;
}

template<typename P>inline bool operator==(P *l, const ptr_rev<P> &p)
{
	return p.ptr == l;
}

template<typename P>inline bool operator!=(P *l, const ptr_rev<P> &p)
{
	return p.ptr != l;
}


template<typename T>class fastvec {
	size_t len;
	size_t reserved;
	T* data;

	public:
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef ptr_rev<T> reverse_iterator;
		typedef ptr_rev<const T> const_reverse_iterator;


		fastvec(): 
			len(0)
		{
			reserved = 64;
			data = (static_cast<T*>(malloc(sizeof(T)*(reserved+1))))+1;
		}

		fastvec(size_t r): reserved(r+1024), len(r)
		{
			data = (static_cast<T*>(malloc(sizeof(T)*(reserved+1))))+1;
		}

		fastvec(size_t r, size_t res): reserved(res), len(r)
		{
			data = (static_cast<T*>(malloc(sizeof(T)*(reserved+1))))+1;
		}

		fastvec(const fastvec &f): reserved(f.reserved), len(f.len)
		{
			data = (static_cast<T*>(malloc(sizeof(T)*(reserved+1))))+1;
			memcpy(data, f.data, sizeof(T)*len);
		}

		virtual ~fastvec() 
		{
			free(data-1); 
		}

		fastvec& operator =(const fastvec &f)
		{
			reserved = f.reserved;
			len = f.len;
			data = (static_cast<T*>(realloc(data-1, sizeof(T)*(reserved+1))))+1;
			memcpy(data, f.data, sizeof(T)*(reserved-1));
			return *this;
		}

		void reserve(size_t r)
		{
			reserved = r;
			data = (static_cast<T*>(realloc(data-1, sizeof(T)*(reserved+1))))+1;
		}

		void erase(iterator w)
		{
			iterator e = data+len;//-1;
			if(w == e/*+1*/)
			{
				return;
			}
			//else if(w == e)
			//{
			//  len--;
			//  return;
			//}
			for(;w!=e; ++w)
			{
				*w = *(w+1);
			}
			if(len*4+4096 < reserved)
				reserve(len*2+2048);
			len--;
		}

		void erase(iterator f, iterator l)
		{
			if(f == l)
				return;
			ptrdiff_t d = l-f;
			iterator e = data+len;
			for(;l!=e; ++l)
			{
				*f++ = *l;
			}
			len -= d;
			if(len*4+4096 < reserved)
				reserve(len*2+2048);
		}

		void push_back(const_reference val)
		{
			if(len >= reserved)
				reserve(len*2+2048);
			data[len++] = val;
		}

		void pop_back()
		{
			if(len*4+4096 < reserved)
				reserve(len*2+2048);
			--len;
		}

		void resize(size_t r)
		{
			len = r;
			if(len >= reserved)
				reserve(len*2+2048);
		}

		void clear() 
		{
		   	len = 0; 
		}

		operator pointer() 
		{
		   	return data; 
		}
		
		operator const_pointer() const 
		{
		   	return data; 
		}

		size_t size() const 
		{
		   	return len; 
		}
		
		size_t capacity() const 
		{
		   	return reserved; 
		}
		
		bool empty() const 
		{
		   	return len == 0; 
		}

		reference front() 
		{
		   	return *data; 
		}
		
		const_reference front() const 
		{
		   	return *data; 
		}
		
		reference back() 
		{
		   	return data[len-1]; 
		}
		
		const_reference back() const 
		{
		   	return data[len-1]; 
		}

		iterator begin() 
		{
		   	return data; 
		}
		
		const_iterator begin() const 
		{
		   	return data; 
		}
		
		reverse_iterator rbegin() 
		{
		   	return data+len-1; 
		}
		
		const_reverse_iterator rbegin() const 
		{
		   	return data+len-1; 
		}

		iterator end() 
		{
		   	return data+len; 
		}
		
		const_iterator end() const 
		{
		   	return data+len; 
		}
	
		reverse_iterator rend() 
		{
		   	return data-1; 
		}
	
		const_reverse_iterator rend() const 
		{
		   	return data-1; 
		}

};

struct node_t: public node_base {
	node_t(const void* obj, size_t n, bool intern);

	virtual ~node_t() 
	{
	}

	bool contains(const void* obj);

	const void* base;
	size_t size;
	bool mark;
	bool internal;
	void* object;
	void (*destroy)(void*, void*, size_t);

	static void kill(node_t *node);
	static void* operator new(size_t s);
	static void* operator new(size_t s, void *place);
	static void operator delete(void *n);
	static void operator delete(void *n, void *place) {}
};

struct node_less {
	bool operator()(const node_t* x, const node_t* y) const
	{
		//return (static_cast<const char*>(x->base) + x->size) <= static_cast<const char*>(y->base);
		static const std::less<const void*> cmp = std::less<const void*>();
		return !cmp(static_cast<const char*>(y->base), static_cast<const char*>(x->base) + x->size);
	}
} node_cmp;

typedef std::pair<pointer_base*, node_t*> ptr_pair;
typedef std::pair<weak_base*, node_t*> weak_pair;
typedef fastvec<ptr_pair> ptr_vector;
typedef fastvec<node_t*> node_vector;
typedef fastvec<weak_pair> weak_vector;
typedef std::deque<ptr_vector::iterator> ptr_queue;

struct data_t {
	data_t(): 
		threshold(default_threshold), 
		dynamic_threshold(default_threshold), 
		allocated(0), 
		collecting(false), 
		current_mark(false)
	{ 
		// InitializeCriticalSection(&cs);
		// cs=false;
	}

	virtual ~data_t()
	{
		// If there is something to collect, then we do collections until
		// nothing could be freed anymore
		size_t pre = old_nodes.size() + new_nodes.size() + born_nodes.size();
		if(pre)
		{
			size_t post = pre;
			do
			{
				pre = post;
				collect();
				post = old_nodes.size() + new_nodes.size() + born_nodes.size();
			} while(post < pre && post != 0);
			pre = post;
		}

		// After final collection we clean up remaining undeleted nodes,
		// but do not call destructors of objects belonging to them
		if(pre) {
			node_vector::iterator end = old_nodes.end();
			for(node_vector::iterator i=old_nodes.begin(); i<end; ++i)
			{
				(*i)->destroy = 0;
				node_t::kill(*i);
			}
			end = new_nodes.end();
			for(node_vector::iterator j=new_nodes.begin(); j<end; ++j)
			{
				(*j)->destroy = 0;
				node_t::kill(*j);
			}
			end = born_nodes.end();
			for(node_vector::iterator k=born_nodes.begin(); k<end; ++k)
			{
				(*k)->destroy = 0;
				node_t::kill(*k);
			}
		}
		/*DeleteCriticalSection(&cs);*/
	}

	ptr_vector old_pointers;
	ptr_vector new_pointers;
	weak_vector weak_pointers;
	//node_set nodes;
	node_vector old_nodes;
	node_vector new_nodes;
	node_vector born_nodes;
	size_t threshold;
	size_t dynamic_threshold;
	size_t allocated;
	bool collecting;
	bool current_mark;
	//CRITICAL_SECTION cs;
	/*bool cs;*/
};

data_t& data() 
{
   	static data_t instance; return instance; 
}

struct data_lock {
	data_lock() 
	{ 
		/*EnterCriticalSection(&data().cs);*//*if(data().cs) throw "double lock"; data().cs=true;*/ 
	}
	
	virtual ~data_lock() 
	{
		// LeaveCriticalSection(&data().cs);
		// data().cs=false;
	}
};

node_t::node_t(const void* obj, /*int*/size_t node, bool intern=false): 
	base(obj), size(node), /*mark(data().current_mark),*/ internal(intern), object(0), destroy(0)
{
}

void node_t::kill(node_t *node)
{
	bool internal = node->internal;
	if(node->object && node->destroy)
		node->destroy(node->object, const_cast<void*>(node->base), node->size);
	if(!internal)
		free(static_cast<void*>(node));
}

bool node_t::contains(const void* obj)
{
	static const std::less<const void*> cmp = std::less<const void*>();
	if(cmp(obj, base))
		return false;
	return cmp(obj, static_cast<const char*>(base) + size);
}

void * node_t::operator new(size_t s)
{
	void *res = malloc(s);
	if(res == 0)
		throw std::bad_alloc();
	return res;
}

void * node_t::operator new(size_t s, void *place)
{
	return place;
}

void node_t::operator delete(void *n)
{
#ifndef NDEBUG
	std::cerr << "Internal error -- badly written software" << std::endl;
	abort();
#endif
}

node_t* find_node(const void* obj)
{
	// Construct a temporary node in order to search for the object's node.
	node_t temp(obj, 0);

	// Use lower_bound to search for the "insertion point" and determine
	// if the node at this point contains the specified object.  If the
	// insertion point is at the end or does not contain the object then
	// we've failed to find the node and return an iterator to the end.
	node_vector::iterator i = std::lower_bound(data().new_nodes.begin(), data().new_nodes.end(), &temp, node_cmp);
	if(i == data().new_nodes.end() || !(*i)->contains(obj))
	{
		i = std::lower_bound(data().old_nodes.begin(), data().old_nodes.end(), &temp, node_cmp);
		if (i == data().old_nodes.end() || !(*i)->contains(obj))
			return NULL;
	}
	// Return the found iterator.
	return *i;
}

node_t* find_new_node(const void* obj)
{
	// Construct a temporary node in order to search for the object's node.
	node_t temp(obj, 0);

	// Use lower_bound to search for the "insertion point" and determine
	// if the node at this point contains the specified object.  If the
	// insertion point is at the end or does not contain the object then
	// we've failed to find the node and return an iterator to the end.
	node_vector::iterator i = std::lower_bound(data().new_nodes.begin(), data().new_nodes.end(), &temp, node_cmp);
	if(i == data().new_nodes.end() || !(*i)->contains(obj))
		return NULL;

	// Return the found iterator.
	return *i;
}

node_vector::iterator find_born_node(const void* obj)
{
	node_vector::iterator e = data().born_nodes.end();
	for(node_vector::iterator i = data().born_nodes.begin(); i!=e; ++i)
	{
		if((*i)->contains(obj))
			return i;
	}
	return e;
}

void get_node(const void* obj, void (*destroy)(void*, void*, size_t))
{
	if(!obj)
		return;

	//data_lock lock; -- podwójny lock -- po co???

	node_vector::iterator i = find_born_node(obj);
	if(i != data().born_nodes.end())
	{
		node_t* node = *i;

		data().new_nodes.push_back(node);
		data().born_nodes.erase(i);
		node->mark = data().current_mark;
		node->destroy = destroy;
		node->object = const_cast<void*>(obj);
	}
}

void mark(ptr_vector::iterator i, ptr_queue &que)
{
	// Mark the node associated with the pointer and then iterate
	// through all pointers contained by the object pointed to and
	// add them to the queue for later marking.
	node_t* node = i->second;
	if(node && node->mark != data().current_mark)
	{
		node->mark = data().current_mark;
		ptr_vector::iterator j = lower_bound(data().old_pointers.begin(),
				data().old_pointers.end(),
				ptr_pair(static_cast<pointer_base*>(const_cast<void*>(node->base)),0));
		ptr_vector::iterator end = data().old_pointers.end();
		for(; j != end; ++j)
		{
			if (node->contains(j->first))
				que.push_back(j);
			else
				break;
		}
	}
}

void presort_ptr_vect(ptr_vector::iterator begin, ptr_vector::iterator end)
{
	static const std::less<pointer_base*> cmp = std::less<pointer_base*>();
	if(end-begin < 16)
		return;

	ptr_vector::iterator i = begin+((end-begin)>>1);
	ptr_pair pivot = *i;
	*i = *begin;
	*begin = pivot;
	i = begin;

	ptr_vector::iterator j = end;
	register pointer_base* piv = pivot.first;
	ptr_pair p;

	while(i<j)
	{
		do
		{
			++i;
		} while(i<end && cmp(i->first, piv));
		do
		{
			--j;
		} while(cmp(piv, j->first)/* j>begin && ... -- this is not needed as piv==*begin */);

		if(i<j)
		{
			p = *i;
			*i = *j;
			*j = p;
		}
	}
	*begin = *j;
	*j = pivot;

	presort_ptr_vect(begin, j);
	presort_ptr_vect(j+1, end);
}

void sort_ptr_vect(ptr_vector &v)
{
	static const std::less<pointer_base*> cmp = std::less<pointer_base*>();

	ptr_vector::iterator begin = v.begin();
	ptr_vector::iterator end = v.end();

	if(end-begin < 32)
	{
		sort(begin, end);
		return;
	}

	presort_ptr_vect(begin, end);

	ptr_vector::iterator i = begin+1;
	ptr_vector::iterator j;
	ptr_pair p = *begin;
	for(j=i; j<begin+17; ++j)
	{
		if(cmp(j->first, p.first))
			p = *j;
	}
	*(begin-1) = p; // this is perfectly legal with fastvect

	while(i<end)
	{
		if(cmp(i->first, (i-1)->first))
		{
			j = i-1;
			p = *i;
			while(cmp(p.first, j->first))
			{
				*(j+1) = *j;
				--j;
			}
			*(j+1) = p;
		}
		++i;
	}

#ifndef NDEBUG
	p = *begin;
	for(i=begin+1; i<end; ++i)
	{
		//if(cmp(i->first, p.first))
		if(i->first <= p.first && p.first)
		{
			throw jcommon::RuntimeException("smartpointer order lost");
			//cerr << "Order lost at " << (i-begin) << endl;
			//abort();
		}
		p = *i;
	}
#endif
}

void presort_node_vect(node_vector::iterator begin, node_vector::iterator end)
{
	if(end-begin < 16)
		return;

	node_vector::iterator i = begin+((end-begin)>>1);
	node_t *pivot = *i;
	*i = *begin;
	*begin = pivot;
	i = begin;

	node_vector::iterator j = end;
	node_t *p;

	while(i<j)
	{
		do
		{
			++i;
		} while(i<end && node_cmp(*i, pivot));
		do
		{
			--j;
		} while(node_cmp(pivot, *j)/* j>begin && ... -- this is not needed as piv==*begin */);

		if(i<j)
		{
			p = *i;
			*i = *j;
			*j = p;
		}
	}
	*begin = *j;
	*j = pivot;

	presort_node_vect(begin, j);
	presort_node_vect(j+1, end);
}

void sort_node_vect(node_vector &v)
{
	node_vector::iterator begin = v.begin();
	node_vector::iterator end = v.end();

	if(end-begin < 32)
	{
		std::sort(begin, end, node_cmp);
		return;
	}

	presort_node_vect(begin, end);

	node_vector::iterator i = begin+1;
	node_vector::iterator j;
	node_t *p = *begin;
	for(j=i; j<begin+17; ++j)
	{
		if(node_cmp(*j, p))
			p = *j;
	}
	*(begin-1) = p; // this is perfectly legal with fastvect

	while(i<end)
	{
		if(node_cmp(*i, *(i-1)))
		{
			j = i-1;
			p = *i;
			while(node_cmp(p, *j))
			{
				*(j+1) = *j;
				--j;
			}
			*(j+1) = p;
		}
		++i;
	}
}

void clean_merge_ptr()
{
	// Clean old_pointers pulling off null references then
	// sort new_pointers and merge it into old_pointers

	ptr_vector &ops = data().old_pointers;
	ptr_vector &nps = data().new_pointers;

	sort_ptr_vect(nps);

	ptr_vector::iterator o = ops.begin();
	ptr_vector::iterator end = ops.end();
	for(ptr_vector::iterator i = o; i != end; ++i)
	{
		if(i->first != 0)
			*o++ = *i;
	}
	ops.erase(o, ops.end());
	ops.resize(ops.size()+nps.size()/*, ptr_pair(0,0)*/);

	// We do our own merge, not merge algorithm, because
	// we don't want to create new old_pointers vector
	// everytime, nor we want to go for inplace_merge, as
	// it'd be slow.
	ptr_vector::reverse_iterator r = ops.rbegin()+nps.size();
	ptr_vector::reverse_iterator n = nps.rbegin();
	size_t os = ops.size();

#ifdef _WIN32
	if (os > INT_MAX)
#else
	if ((int)os > std::numeric_limits<int>::max())
#endif
		throw jcommon::RuntimeException("No space exception");

	int s = (int)os;
	while(r != ops.rend() && n != nps.rend()) {
		--s;
		if(*r > *n)
			ops[s] = *r++;
		else
			ops[s] = *n++;
		ops[s].first->pos = -1-s;
	}

	while(r != ops.rend()) {
		--s;
		ops[s] = *r++;
		ops[s].first->pos = -1-s;
	}

	while(n != nps.rend()) {
		--s;
		ops[s] = *n++;
		ops[s].first->pos = -1-s;
	}
#ifndef NDEBUG
	if(s != 0)
		throw jcommon::RuntimeException("internal error in ptr_vectors merge");
	void *old = 0;
	end = ops.end();
	for(ptr_vector::iterator c = ops.begin(); c != end; ++c)
	{
		ASSERT_PTR_VALID(*(c->first));
		if(c->first == old)
			throw jcommon::RuntimeException("duplicated pointer");
		else if(c->first < old)
			throw jcommon::RuntimeException("pointer order lost");
		old = c->first;
	}
#endif
	nps.clear();
}

void clean_merge_nodes(size_t new_sweeped_out)
{
	// Clean old_nodes pulling off null references then
	// merge old_nodes into new_nodes

	node_vector &onv = data().old_nodes;
	node_vector &nnv = data().new_nodes;

	node_vector::iterator o = onv.begin();
	node_vector::iterator end = onv.end();
	for(node_vector::iterator i = o; i != end; ++i)
	{
		if(*i != 0)
			*o++ = *i;
	}
	onv.erase(o, onv.end());
	onv.resize(onv.size()+nnv.size()-new_sweeped_out);

	// We do our own merge, not merge algorithm, because
	// we don't want to create new old_pointers vector
	// everytime, nor we want to go for inplace_merge, as
	// it'd be slow.
	node_vector::reverse_iterator r = onv.rbegin()+(nnv.size()-new_sweeped_out);
	node_vector::reverse_iterator n = nnv.rbegin();
	size_t s = onv.size();
	while(r != onv.rend() && n != nnv.rend())
	{
		if(*n == 0)
		{
			n++;
			continue;
		}
		--s;
		if(node_cmp(*n, *r))
			onv[s] = *r++;
		else
			onv[s] = *n++;
	}
	while(r != onv.rend())
	{
		--s;
		onv[s] = *r++;
	}
	while(n != nnv.rend())
	{
		if(*n == 0)
		{
			n++;
			continue;
		}
		--s;
		onv[s] = *n++;
	}
#ifndef NDEBUG
	if(s != 0)
		throw jcommon::RuntimeException("internal error in node_sets merge");
	node_t *old = 0;
	end = onv.end();
	for(node_vector::iterator c = onv.begin(); c != end; ++c)
	{
		if(*c == old)
			throw jcommon::RuntimeException("duplicated node");
		else if(old && !node_cmp(old, *c))
			throw jcommon::RuntimeException("node order lost");
		old = *c;
	}
#endif
	nnv.clear();
}

template<class A>void* gnew(size_t size, const A& a)
{
	data_lock lock;

	bool collected = false;

	if (data().threshold != no_threshold) {
		// Determine if we've exceeded the threshold and so should collect.
		data().allocated += size;
		if (data().allocated > data().dynamic_threshold)
		{
			collect();
			collected = true;
		}
	}

	// Attempt the first allocation.  The standard requires new to throw
	// on failure but user code may change this behavior and VC++ by default
	// only returns 0.  We'll catch exceptions and if we've already collected
	// re-throw the exception.
	void* obj = 0;
	try {
	   	obj = a.alloc(size); 
	} catch(std::bad_alloc&) { 
		if (collected) throw; 
	}

	// If we've failed to allocate but new didn't throw an exception and we've
	// not collected yet we'll collect and then re-try calling new.  If new
	// throws at this point we'll ignore it and let the caller handle it.
	if (obj == 0 && !collected) {
		collect();
		obj = a.alloc(size);
	}

	// If we actually allocated memory then we need to add it to the node map.
	try {
		if (obj != 0) {
			node_t *node;
			if(a.internal_node())
				node = new(static_cast<void*>(static_cast<char*>(obj)+a.node_offset(size))) node_t(obj, size, true);
			else
				node = new node_t(obj, size);
			if(node == 0) // Dealing with VC++ 6.0 quirk
				throw std::bad_alloc();
			data().born_nodes.push_back(node);
		}
	} catch (...) {
		// If inserting into the map failed clean up and simply throw
		// a bad_alloc exception.
		a.free(obj);
		throw std::bad_alloc();
	}

	return obj;
}

template<class A>void gfree(void* obj, const A &a)
{
	data_lock lock;

	// Theoretically, none of this code will throw an exception.
	// If an exception occurs the best we can do is assume that
	// everything worked any way and ignore the exception.
	try
	{
		node_vector::iterator i = find_born_node(obj);
		node_t* node = *i;
		if (node)
			data().born_nodes.erase(i);

		// This operator really should only be called when
		// construction of an object fails, in which case
		// there won't be a "registered destructor" and the
		// following will only delete the node.
		node_t::kill(node);
	}
	catch (...)
	{
	}

	// Because there was no "registered destructor" we'll still
	// need to delete the memory allocated by operator new(GC).
	a.free(obj);
}

struct alloc_single_t {
	static void *alloc(size_t s)
	{
		return ::operator new(node_offset(s) + sizeof(node_t));
	}

	static void free(void *obj)
	{
		::operator delete(obj);
	}

	static size_t node_offset(size_t s)
	{
		size_t r = ((s+sizeof(void(*)())-1) / sizeof(void(*)())) * sizeof(void(*)());
		if(r>2000000000u) abort();
		return r;
	}

	static bool internal_node()
	{
		return true;
	}
};

alloc_single_t alloc_single;

#ifndef _SMGC_NO_ARRAY_NEW
struct alloc_array_t {
	static void *alloc(size_t s)
	{
		return ::operator new[](s);
	}

	static void free(void *obj)
	{
		::operator delete[](obj);
	}

	static size_t node_offset(size_t s)
	{
		return 0;
	}

	static bool internal_node()
	{
		return false;
	}
};

alloc_array_t alloc_array;
#endif

pointer_base::pointer_base()
{
	data_lock lock;

#ifndef NDEBUG
	destroyed = false;
#ifdef DEBUG_COMPILER
	trash = new char[1];
#endif
#endif
	ptr_vector &ps = data().new_pointers;
	pos = ps.size();
	ps.push_back(ptr_pair(this, 0));
}

pointer_base::pointer_base(const pointer_base &pb)
{
	data_lock lock;

#ifndef NDEBUG
	destroyed = false;
#ifdef DEBUG_COMPILER
	trash = new char[3];
#endif
#endif
	ptr_vector &ps = data().new_pointers;
	pos = ps.size();
	/*if(pb.pos >= 0)
	  ps.push_back(ptr_pair(this, ps[pb.pos].second));
	  else
	  ps.push_back(ptr_pair(this, data().old_pointers[-1-pb.pos].second));
	  */
	ps.push_back(ptr_pair(this, 0));
}

pointer_base::~pointer_base()
{
	data_lock lock;

#ifndef NDEBUG
#ifdef DEBUG_COMPILER
	delete[] trash;
#endif
	destroyed = true;
#endif
	if(pos >= 0)
	{
		ptr_vector &ps = data().new_pointers;
		if (pos == (int)(ps.size()-1)) {
			ps.pop_back();
		} else {
			ps[pos] = ps.back();
#ifndef NDEBUG
			if(ps[pos].first->pos != (int)(ps.size()-1)) {
				//cerr << "GC: memory corrupted; smartpointer lost!" << endl;
				//abort();
				throw jcommon::RuntimeException("smartpointer lost");
			}

			if(ps[pos].first->destroyed) {
				//cerr << "GC: memory corrupted; smartpointer destructed 2 times or overwritten!" << endl;
				//abort();
				throw jcommon::RuntimeException("smartpointer overwritten or double destructed");
			}
#endif
			ps[pos].first->pos = pos;
			ps.pop_back();
		}
	} else {
		data().old_pointers[-1-pos].first = 0;
	}
}

void pointer_base::reset_node(const void* obj, void (*destroy)(void*, void*, size_t))
{
	data_lock lock;
	get_node(obj, destroy);
}

weak_base::weak_base()
{
	data_lock lock;

#ifndef NDEBUG
	destroyed = false;
#ifdef DEBUG_COMPILER
	trash = new char[2];
#endif
#endif
	weak_vector &ps = data().weak_pointers;
	pos = ps.size();
	ps.push_back(weak_pair(this, 0));
}

weak_base::weak_base(const weak_base &pb)
{
	data_lock lock;

#ifndef NDEBUG
	destroyed = false;
#ifdef DEBUG_COMPILER
	trash = new char[4];
#endif
#endif
	weak_vector &ps = data().weak_pointers;
	pos = ps.size();
	ps.push_back(weak_pair(this, 0));
}

weak_base::~weak_base()
{
	data_lock lock;

#ifndef NDEBUG
#ifdef DEBUG_COMPILER
	delete[] trash;
#endif
	destroyed = true;
#endif
	weak_vector &ps = data().weak_pointers;
	if(pos == (int)(ps.size()-1)) {
		ps.pop_back();
	} else {
		ps[pos] = ps.back();
#ifndef NDEBUG
		if(ps[pos].first->pos != (int)(ps.size()-1)) {
			throw jcommon::RuntimeException("weakpointer lost");
		}

		if(ps[pos].first->destroyed) {
			throw jcommon::RuntimeException("weakpointer overwritten or double destructed");
		}
#endif
		ps[pos].first->pos = pos;
		ps.pop_back();
	}
}

// Register memory area in GC. Area must be freeable using supplied destroy function
void reg(void *obj, size_t size, void (*destroy)(void*, void*, size_t))
{
	data_lock lock;

	bool collected = false;

	if (data().threshold != no_threshold)
	{
		// Determine if we've exceeded the threshold and so should collect.
		data().allocated += size;
		if (data().allocated > data().dynamic_threshold)
		{
			collect();
			collected = true;
		}
	}

	// If we actually allocated memory then we need to add it to the node map.
	try
	{
		if (obj != 0)
		{
			node_t *node = new node_t(obj, size);
			if(node == 0)
				throw std::bad_alloc();
			data().new_nodes.push_back(node);
			node->destroy = destroy;
			node->object = const_cast<void*>(obj);
		}
	}
	catch (...)
	{
		// If inserting into the map failed simply throw
		// a bad_reg exception.
		throw jcommon::RuntimeException("Bad reg exception");
	}
}

#if (defined(_MSC_VER) && (_MSVC <= 1200)) || defined(_SMGC_POOR_STL)
const size_t no_threshold = ~((size_t)0);
#else
const size_t no_threshold = std::numeric_limits<size_t>::max();
#endif

const size_t default_threshold = (INT_MAX>=4194304*8)?4194304:(INT_MAX/8);

void collect()
{
	data_lock lock;

	// During the sweep phase we'll be deleting objects that could cause
	// a recursive call to 'collect' which would cause invalid results.  So
	// we prevent recursion here.
	if (data().collecting)
		return;

	data().collecting = true;

#ifndef NDEBUG
	//cerr << "Allocated: " << data().allocated << endl;
#endif

	// Toggle the 'current_mark' so that we can start over.
	data().current_mark = !data().current_mark;
	bool current_mark = data().current_mark;

	{ // Mark phase
		// Loop through all of the pointers looking for 'root' pointers.  A 'root'
		// pointer is a pointer that's not contained within the object pointed
		// to by any other pointer.  When a 'root' pointer is found it is
		// marked, and all the pointers referenced through the 'root' pointer
		// are also marked.
		clean_merge_ptr();
		//sort(data().new_nodes.begin(), data().new_nodes.end(), node_cmp);
		sort_node_vect(data().new_nodes);

		ptr_vector::iterator end = data().old_pointers.end();
		for(ptr_vector::iterator i = data().old_pointers.begin(); i != end; ++i)
		{
			pointer_base* ptr = i->first;
			if(!(reinterpret_cast<gc_ptr<void*>& >(*ptr)).ptr)
			{
				i->second = 0;
				continue;
			}
			node_t* node = i->second;
			if(!node || !node->contains((reinterpret_cast<gc_ptr<void*>& >(*ptr)).ptr))
			{
				node = find_node((reinterpret_cast<gc_ptr<void*>& >(*ptr)).ptr);
				//#ifndef NDEBUG
				//          if(!node)
				//          {
				//            //cerr << "Pointer points to nonsensical address" << endl;
				//            //abort();
				//            throw mem_corrupt("smartpointer points to nonsense address");
				//          }
				//#endif
				i->second = node;
			}
		}
		ptr_queue que;
		for(ptr_vector::iterator ii = data().old_pointers.begin(); ii != end; ++ii)
		{
			node_t* node = ii->second;
			if (!node || node->mark == current_mark)
				continue; // Don't need to check pointers that are marked.

			// If we can't find a node that contains the pointer it's a root pointer
			// and should be marked.
			node = find_node(ii->first);
			if (!node)
				que.push_back(ii);
		}
		while(que.size())
		{
			mark(que.front(), que);
			que.pop_front();
		}
		// Nullify weak pointers pointing to unmarked nodes
		weak_vector::iterator weak_end = data().weak_pointers.end();
		for(weak_vector::iterator j = data().weak_pointers.begin(); j != weak_end; ++j)
		{
			weak_base* ptr = j->first;
			if(!(reinterpret_cast<wk_ptr<void*>& >(*ptr)).ptr)
			{
				j->second = 0;
				continue;
			}
			node_t* node = j->second;
			if(!node || !node->contains((reinterpret_cast<wk_ptr<void*>& >(*ptr)).ptr))
			{
				node = find_node((reinterpret_cast<wk_ptr<void*>& >(*ptr)).ptr);
				j->second = node;
			}
			if(!node || node->mark != current_mark)
				reinterpret_cast<wk_ptr<void*>& >(*ptr).ptr = 0;
		}
	}

	{ // Sweep phase
		// Step through all of the nodes and delete any that are not marked.
		node_vector::iterator i;
		node_vector::iterator end = data().new_nodes.end();
		size_t nnsweep = 0;
		size_t freed = 0;
		size_t remain = 0;
		for (i = data().new_nodes.begin(); i != end; /*nothing*/)
		{
			node_t* node = *i;
			remain += node->size;
			if (node->mark != current_mark)
			{
				if (node->destroy == 0)
				{
					// We must be constructing this object, so we'll just mark it.
					// This prevents premature collection of objects that call
					// gc_collect during the construction phase before any gc_ptr<>'s
					// are assigned to the object.
					node->mark = current_mark;
				}
				else
				{
					++nnsweep;
					freed += node->size;
					*i = 0;
					node_t::kill(node);
				}
			}
			++i;
		}
		end = data().old_nodes.end();
		for (i = data().old_nodes.begin(); i != end; /*nothing*/)
		{
			node_t* node = *i;
			remain += node->size;
			if (node->mark != current_mark)
			{
				if (node->destroy == 0)
				{
					// We must be constructing this object, so we'll just mark it.
					// This prevents premature collection of objects that call
					// gc_collect during the construction phase before any gc_ptr<>'s
					// are assigned to the object.
					node->mark = current_mark;
				}
				else
				{
					freed += node->size;
					*i = 0;
					node_t::kill(node);
				}
			}
			++i;
		}
		remain -= freed;

		clean_merge_nodes(nnsweep);

		if(data().threshold != no_threshold && data().threshold > 0)
		{
			if(data().dynamic_threshold < freed/2 && data().dynamic_threshold > data().threshold/2)
			{
				data().dynamic_threshold /= 2;
			}
			else if(data().dynamic_threshold < freed/2 && data().dynamic_threshold > data().threshold/4)
			{
				data().dynamic_threshold = data().threshold/4;
			}
			else if(data().dynamic_threshold > remain && data().dynamic_threshold > data().threshold)
			{
				data().dynamic_threshold /= 4;
			}
			else if(data().dynamic_threshold > remain && data().dynamic_threshold > data().threshold/4)
			{
				data().dynamic_threshold = data().threshold/4;
			}
			else if(data().dynamic_threshold < remain/4)
			{
				data().dynamic_threshold *= 2;
			}
			else if(data().dynamic_threshold > freed/16)
			{
				data().dynamic_threshold *= 4;
			}
			else if(data().dynamic_threshold > freed/4)
			{
				data().dynamic_threshold *= 2;
			}
		}
	}

	data().collecting = false;
	data().allocated = 0;
}

void set_threshold(size_t bytes)
{
	data_lock lock;
	data().threshold = bytes;
	data().dynamic_threshold = bytes;
}

size_t get_threshold()
{
	data_lock lock;
	return data().threshold;
}

size_t get_dynamic_threshold()
{
	data_lock lock;
	return data().dynamic_threshold;
}

}

namespace std {
#ifdef _SMGC_POOR_TEMPLATES
	struct less<pointer_base>
	{
		bool operator()(const pointer_base &l, const pointer_base &p)
		{
			static less<void**> cmp;
			ASSERT_PTR_VALID(l);
			ASSERT_PTR_VALID(p);
			return cmp((reinterpret_cast<const const_gc_ptr<void*>&>(l)).ptr, (reinterpret_cast<const const_gc_ptr<void*>&>(p)).ptr);
		}
	};
#else

	template<typename T>
		struct less<jcommon::const_gc_ptr<T> >
		{
			bool operator()(const jcommon::const_gc_ptr<T> &l, const jcommon::const_gc_ptr<T> &p)
			{
				static less<T> cmp;
				ASSERT_PTR_VALID(l);
				ASSERT_PTR_VALID(p);
				return cmp(l.ptr, p.ptr);
			}
		};

	template<typename T>
		struct less<jcommon::gc_ptr<T> >
		{
			bool operator()(const jcommon::gc_ptr<T> &l, const jcommon::gc_ptr<T> &p)
			{
				static less<T> cmp;
				ASSERT_PTR_VALID(l);
				ASSERT_PTR_VALID(p);
				return cmp(l.ptr, p.ptr);
			}
		};

	template<typename T>
		struct less<jcommon::const_gc_arr<T> >
		{
			bool operator()(const jcommon::const_gc_arr<T> &l, const jcommon::const_gc_arr<T> &p)
			{
				static less<T> cmp;
				ASSERT_PTR_VALID(l);
				ASSERT_PTR_VALID(p);
				return cmp(l.ptr, p.ptr);
			}
		};

	template<typename T>
		struct less<jcommon::gc_arr<T> >
		{
			bool operator()(const jcommon::gc_arr<T> &l, const jcommon::gc_arr<T> &p)
			{
				static less<T> cmp;
				ASSERT_PTR_VALID(l);
				ASSERT_PTR_VALID(p);
				return cmp(l.ptr, p.ptr);
			}
		};
#endif
}

#ifndef _SMGC_NO_PLACEMENT_NEW
void* operator new(size_t s, const jcommon::gc_t&) throw (std::bad_alloc);
void operator delete(void *obj, const jcommon::gc_t&) throw ();
#ifndef _SMGC_NO_ARRAY_NEW
void* operator new[](size_t s, const jcommon::gc_t&) throw (std::bad_alloc);
void operator delete[](void *obj, const jcommon::gc_t&) throw ();
#endif
#endif

#undef GC_INTERNAL

#ifndef _SMGC_NO_PLACEMENT_NEW
void* operator new(size_t s, const jcommon::gc_t&) throw (std::bad_alloc)
{
	return gnew(s, jcommon::alloc_single);
}

void operator delete(void *obj, const jcommon::gc_t&) throw ()
{
	gfree(obj, jcommon::alloc_single);
}

#ifndef _SMGC_NO_ARRAY_NEW
void* operator new[](size_t s, const jcommon::gc_t&) throw (std::bad_alloc)
{
	return gnew(s, jcommon::alloc_array);
}

void operator delete[](void *obj, const jcommon::gc_t&) throw ()
{
	gfree(obj, jcommon::alloc_array);
}

#endif
#endif

#endif


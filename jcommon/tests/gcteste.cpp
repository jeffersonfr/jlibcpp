#include "jgc.h"

#ifndef _SMGC_POOR_STL
#include <cstdlib>
#include <iostream>
#else
#include <stdlib.h>
#include <iostream.h>
#endif

using namespace jcommon;
using namespace std;

inline static void do_nothing() 
{
}

//#define opt_collect jcommon::collect
#define opt_collect do_nothing

template<typename T>struct bst
{
	typedef gc_ptr<bst<T> > subtree;
	typedef const_gc_ptr<bst<T> > const_subtree;

	//template<typename U>
	//friend class bst;

	subtree left, right;
	T val;
	bool empty;

	bst(): 
		empty(true) 
	{
	}

	bst(const bst &b): 
		empty(false), val(b.val), left(b.left), right(b.right) 
	{
	}

	template<typename U>explicit bst(const U &v): val(v), empty(false) 
	{
	}

	template<typename U>bst<T>& operator =(const U &v)
	{
		if(empty) {
			val = v;
			empty = false;
		} else if(v < val) {
			if(!left)
				left = new(GC) bst<T>;
			*left = v;
		} else if(val < v) {
			if(!right)
				right = new(GC) bst<T>;
			*right = v;
		} else {
			//cerr << "Duplicate bst node!" << endl;
			if(!right)
				right = new(GC) bst<T>;
			*right = v;
		}

		return *this;
	}

	const T& operator *() const 
	{
	   	return val; 
	}
	
	const T* operator ->() const 
	{
	   	return &val; 
	}
	
	T& operator *() 
	{
	   	return val; 
	}
	
	T* operator ->() 
	{
	   	return &val; 
	}

};

template <typename T>ostream& operator <<(ostream &o, bst<T> &b)
{
	o << "(";
	if(b.left)
		o << *(b.left) << " ";
	if(b.val)
		o << b.val;
	if(b.right)
		o << " " << *(b.right);
	o << ")";

	return o;
}

template<typename T, typename U>gc_ptr<bst<T> >& find(gc_ptr<bst<T> >& tree, const U &v)
{
	static gc_ptr<bst<T> > nothing;

	if(!tree)
		return nothing;
	if(!(tree->empty) && (tree->val == v))
		return tree;
	else if(tree->left && (v < tree->val))
		return find(tree->left, v);
	else if(tree->right && (tree->val < v))
		return find(tree->right, v);
	else
		return nothing;
}

class base1
{
	public:
		virtual ~base1() 
		{
		}

	protected:
		int b1;
};

class base2
{
	public:
		virtual ~base2() 
		{
		}

	protected:
		int b2;
};

class inherits: public base1, public base2
{
	public:
		virtual ~inherits() {}
	public:
		inherits() { b1 = 1; b2 =2; }
};


void testmulti()
{
	try
	{
		cout << "making base1(inherits)" << endl;
		gc_ptr<base1> b1(new(GC) inherits);
		cout << "making base2(inherits)" << endl;
		gc_ptr<base2> b2(new(GC) inherits);
		gc_ptr<inherits> inh(new(GC) inherits);
		b2 = inh;
		const gc_ptr<base2> &bb = b2;
		inh = dynamic_cast_gc_ptr<inherits>(bb);
		cout << "inherits done" << endl;
	}
	catch(...)
	{
		cerr << "Invalid use of multiinheritance" << endl;
	}
}



struct cnt
{
	static unsigned count;
	
	cnt() 
	{
	   	++count; cout << " (+)" << cnt::count << flush; 
	}
	
	cnt(const cnt&) 
	{
	   	++count; cout << " (*)" << cnt::count << flush; 
	}
	
	virtual ~cnt() 
	{
	   	--count; cout << " (-)" << cnt::count << flush; 
	}
};

unsigned cnt::count = 0;

const gc_ptr<int> pass(const gc_ptr<int> p)
{
	return p;
}

gc_ptr<int> multipass(gc_ptr<int> p)
{
	return pass(pass(pass(p)));
}

const cnt pass(const cnt p)
{
	return p;
}

cnt multipass(cnt p)
{
	return pass(pass(pass(p)));
}


void testcompiler()
{
	cout << "Testing compiler validity..." << endl;

	{
		const cnt c = cnt();
		const cnt r = multipass(c);
	}

	cout << endl;
	cout << "cnt::count = " << cnt::count << endl;
	if(cnt::count) {
		cout << "your compiler (probably gcc 3.2.x) has a bug!" << endl;
		return;
	}

	{
		const_gc_ptr<int> r = multipass(new(GC) int(13));
	}
}


class contain
{
	gc_ptr<cnt> a;

	public:
		contain()
		{
			a = new(GC) cnt();
		}

		~contain()
		{
			cout << "~contain" << endl;
		}

		void check()
		{
			cout << "check" << endl;
		}
};


void testcontain()
{
	cout << "Testing containers" << endl;
	cout << endl;
	cout << "0. cnt::count = " << cnt::count << endl;

	{
		gc_ptr<contain> c = new(GC) contain();
		cout << endl;
		cout << "1. cnt::count = " << cnt::count << endl;
		cout << endl;
		cout << "collecting..." << endl;
		collect();
		cout << endl;
		cout << "2. cnt::count = " << cnt::count << endl;
		c->check();
	}

	cout << endl;
	cout << "collecting..." << endl;
	collect();
	cout << endl;
	cout << "3. cnt::count = " << cnt::count << endl;
}


void subfun()
{
	size_t i, j=0;

	gc_arr<gc_ptr<int> > arr(new(GC) gc_ptr<int>[200000]);
	//gc_ptr<int> arr[20000];
	gc_arr<const_wk_ptr<int> > wk(new(GC) const_wk_ptr<int>[400000]);

	for(i=0; i<200000; i++) {
		arr[i] = new(GC) int(i);
		wk[j++] = arr[i];
	}

	opt_collect();
	cout << "arr(1):" << flush;
	for(i=0; i<200000; i++) {
		cout << " " << *(arr[i]);// << flush;
		if(*(arr[i]) != (int)i) {
			cout << "Error(1)!" << endl;
			exit(20);
		}
	}
	cout << endl;
	opt_collect();

	for(i=0; i<200000; i++) {
		arr[i] = new(GC) int(-i);
		wk[j++] = arr[i];
	}

	jcommon::collect();
	cout << "arr(2):" << flush;
	for(i=0; i<200000; i++) {
		cout << " " << *(arr[i]);// << flush;
		if(*(arr[i]) != (int)-i) {
			cout << "Error(2)!" << endl;
			exit(20);
		}
	}
	cout << endl;

	j=0;
	cout << "wk:" << flush;
	for(i=0; i<400000; i++) {
		const_gc_ptr<int> w = wk[i].get();
		if(w) {
			cout << " " << *w;// << flush;
			j++;
		} else {
			cout << " NULL";
		}
	}

	cout << endl;
}

int main(int argc, char *argv[])
{
	//jcommon::set_threshold(0);
	testcontain();

	gc_ptr<int> a,b,c,d;
	size_t i,j;
	int v=0, w=0;

	if(argc>1)
		j = atoi(argv[1]);
	else
		j = 27890;

	testmulti();
	testcompiler();
	subfun();
	opt_collect();

	srand(123456);
	a = new(GC) int(1);
	b = new(GC) int(2);
	c = new(GC) int(3);
	d = a;
	gc_ptr<int> e(b);
	gc_ptr<int> f(new(GC) int(3));

	cout << "abcdef: " << *a << *b << *c << *d << *e << *f << endl;

	gc_ptr<bst<int> > tree(new(GC) bst<int>);

	cout << "data:";
	for(i=0; i<128; ++i) {
		v = rand();
		*tree = v;
		cout << " " << v;
		if(i == 100)
			w = v;
	}

	cout << endl;
	cout << "tree: " << *tree << endl;

	opt_collect();
	tree = find(tree, w);
	cout << "tree[w]: " << *tree << endl;

	opt_collect();

	cout << "data(2):";
	for(i=0; i<163840; ++i) {
		v = rand();
		*tree = v;
		cout << " " << i << ":" << v << flush;
		if(i == j)
			w = v;
	}
	cout << endl;
	cout << "tree(2): " << *tree << endl;

	tree = find(tree, w);
	cout << "tree[w](2): " << *tree << endl;

	cout << jcommon::get_threshold() << endl;
	cout << jcommon::get_dynamic_threshold() << endl;

	return 0;
}


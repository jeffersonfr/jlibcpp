/***************************************************************************
 *   Copyright (C) 2001 Claus Dreby                                        *
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
#ifndef J_UNIT_H
#define J_UNIT_H

#include "jobject.h"
#include "jruntimeexception.h"

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <iostream>
#include <algorithm>
#include <typeinfo>

namespace jcommon {

extern bool verbose;
extern bool line_fmt;

class Visitor;

/**
 * \brief The heart of a test system: A test. The test is meant as a base class 
 * for the tests that a client want performed. This means that all tests are to
 * be pointers dynamically allocated. However, the test system takes
 * responsibilities for freeing them again.
 *
 * The function call overload mechanism is used for the executable part of the test
 */
class Test {
	std::string nam;
public:
	/// A test just needs a name
	Test(const std::string& name): 
		nam(name) 
	{
	}
	
	virtual ~Test() 
	{
	}
	
	// The execution of the test
	virtual void operator()() 
	{
	}

	virtual void Visit(Visitor*);
	
	virtual Test* GetChild(const std::string&) 
	{
	   	return 0; 
	}
	
	std::string GetName() const 
	{
	   	return nam; 
	}

};

/** 
 * A test that is implemented by a member function.
 *
 */
template<typename C>class TestFunction : public Test {
	public:
		typedef void (C::*mfp)();

		// An object, a name, and a pointer to a member function.
		TestFunction(C* par, const std::string& name, mfp fp):
	   		Test(name), par(par), fp(fp)
		{
		}
	
		// Executed by invoking the function in the object.
		virtual void operator()()
		{
			(par->*fp)();
		}

	private:
		C* par;
		mfp fp;

};

/**
 * A ref counted reference to a test. This is what test suites are composed
 * of, and what ensures destruction.
 *
 */
class TestCase {
	size_t* cnt;
	Test* tst;
	void dec_cnt();

	public:
		// Simply wrap -- and own -- a test.
		TestCase(Test* t);
	
		// Keep the ref count
		TestCase(const TestCase& tr);
	
		/**
		 * Make a TestCase from a class and a member function.
		 *
		 * The normal usage is inside some test suite class Test:
		 *
		 * #add("id", TestCase(this, "Testing this and that", &Test::test))#
		 *
		 * to make a test that invokes the test method on the instance of the
		 * suite class.
		 * \Ref{TestFunction}
		 */
		template<typename C>TestCase(C* par, const std::string& name, typename TestFunction<C>::mfp fp): 
			cnt(new size_t(1)), tst(new TestFunction<C>(par, name, fp))
		{ 
		}

		virtual ~TestCase();

		// Assignment that maintains reference count.
		TestCase& operator=(const TestCase&);
	
		void Visit(Visitor* vp) const 
		{
		   	tst->Visit(vp); 
		}

		operator Test& () 
		{
		   	return *tst; 
		}

		operator const Test& () const 
		{
		   	return *tst; 
		}

};

/**
 * A wrapper class for the TestCase class that succedes if the correct
 * exception is generated.
 *
 */
template<typename E>class ExceptionTest : public Test {
	private:
		TestCase tc;
		char const* file;
		unsigned int line;

	public:
		/**
		 * The constructor needs a TestCase to wrap. This exception_test will
		 * fail unless the wrapped TestCase generates the exception.
		 *
		 * The name of the exception_test is copied from the wrapped test.
		 */
		ExceptionTest(char const* f, unsigned int l, const TestCase& tc):
		   	Test(static_cast<const Test&>(tc).GetName()), tc(tc), file(f), line(l) 
		{
		}

		virtual ~ExceptionTest() 
		{
		}

		// Runs the wrapped test, and fails unless the correct exception is thrown.
		virtual void operator()();

};

/**
 * Generate a TestCase that expects a specific exception from the TestCase it
 * wraps. It can be used something like
 *
 * #TestCase tc(this, "Test name", &Test::test);#
 *
 * #add("ex", exception_case<out_of_range>(tc));#
 *
 * The name of the exception_case is copied from the wrapped TestCase, and
 * the exception_case will execute the tc test case and report a failure
 * unless the #out_of_range# exception is generated.
 */
template<typename E>TestCase exception_case_f(const char* f, unsigned int l, const TestCase& tc)
{
	return TestCase(new ExceptionTest<E>(tc));
}

/**
 * Splits the string by char c. Each c will generate a new element in the
 * vector, including leading and trailing c.
 *
 */
extern std::vector<std::string> vectorize(const std::string& str, char c);

/**
 * A suite is a test that happens to be a collection of tests. This is an
 * implementation of the Composite pattern.
 *
 */
class Suite : public Test {
	std::vector<std::string> ids;
	std::vector<TestCase> tests;

	public:
		// Make an empty test suite.
		Suite(const std::string& name):
		   	Test(name) 
		{
		}
	
		virtual ~Suite() 
		{
		}

		/// Add a TestCase to the suite.
		void Add(const std::string& id, const TestCase& t);
		
		/**
		 * Get a child with the specified id.
		 * @return 0 if not found.
		 */
		virtual Test* GetChild(const std::string& id);
		
		/// An empty implementation.
		virtual void operator()() 
		{
		}
		
		/// Allow a Visitor to Visit a suite node of the test tree.
		void Visit(Visitor*);
		
		/// Get a reference to the main test suite that the main program will run.
		static Suite& Main();
		
		// Splits the string by dots, and use each id to find a suite or test.
		Test* Find(const std::string& id);
};

/**
 * The Visitor class is a base class for classes that wants to participate in
 * the Visitor pattern with the test hierarchi.
 *
 * This is a slightly extended Visitor pattern implementation, intended for
 * collaboration with the Composite pattern. The aggregate node (here the
 * suite node) is Visited twice, before and after the children are Visited.
 * This allows different algorithms to be implemented.
 */
class Visitor {
	public:
		virtual ~Visitor() 
		{
		}
		
		// Visit a test case, that is not a suite.
		virtual void Visit(Test&) = 0;
		
		// Visit a suite node before the children are Visited.
		virtual void Visit(Suite&) 
		{
		}

		/**
		 * Visit a suite after the children are Visited
		 *
		 */
		virtual void Visit(Suite&, int dummy) // post childs
		{
		}
};

// The basic for all failed assert statements.
class AssertionError : public RuntimeException {
	char const* file_;
	unsigned int line_;
	std::string msg;

	public:
		/// An assertion error with the given message.
		AssertionError(char const* file, unsigned int line, const std::string& msg): 
			RuntimeException("assertion error"), file_(file), line_(line), msg(msg) 
		{
		}
	
		std::string GetMessage() const 
		{
		   	return msg; 
		}
		
		virtual ~AssertionError() throw () 
		{
		}

		/**
		 * The virtual method used for operator<<.
		 *
		 */
		virtual void Out(std::ostream& os) const;

		char const* GetFile() 
		{
		   	return file_; 
		}
		
		unsigned int GetLine() 
		{
		   	return line_; 
		}

};

/**
 * This exception represents a failed comparison between two values of types
 * T1 and T2. Both the expected and the actually value are kept.
 *
 */
template<class T1, class T2>class AssertValueError : public AssertionError {
		T1 exp;
		T2 got;

	public:
		/// Construct by message, expected and gotten.
		AssertValueError(const char* f, unsigned int l, const std::string& msg, T1& exp, T2& got):
		   	AssertionError(f, l, msg), exp(exp), got(got)
		{
		}

		virtual ~AssertValueError() throw () 
		{
		}

		/**
		 * Specialized version that requires both T1 and T2 to support
		 * operator<<(ostream&, Tn).
		 *
		 */
		virtual void Out(std::ostream& os) const
		{
			os << GetMessage() << " [expected: `" << exp << "' got: `" << got << "']";
		}

};

// The test was not succesful.
inline void assert_fail_f(const char* f, unsigned int l, const std::string& msg)
{
	throw AssertionError(f, l, msg);
}

template<typename E>void ExceptionTest<E>::operator()()
{
	try {
		(static_cast<Test&>(tc))();
		assert_fail_f(file, line, "unexpected lack of exception");
	} catch (E& ) {
		// fine!
	}
}

// Assert that the assertion is true, that is fail #if (!assertion) ...#
template<class A> inline void assert_true_f(char const* f, unsigned int l, const std::string& msg, A assertion)
{
	if (!assertion)
		throw AssertionError(f, l, msg);
}

#define assert_true(m, a) assert_true_f(__FILE__, __LINE__, m, a)
#define assert_fail(m) assert_fail_f(__FILE__, __LINE__, m)
#define assert_eq(m, e, g) assert_eq_f(__FILE__, __LINE__, m, e, g)

// Assert that the two arguments are equal in the #==# sense.
template<class T1, class T2>inline void assert_eq_f(char const* f, unsigned int l, const std::string& msg, T1 exp, T2 got)
{
	if (!(exp == got))
		throw AssertValueError<T1,T2>(f, l, msg, exp, got);
}

/*
 * Put an assertion error to a stream, using the out method. The out method
 * is virtual.
 */
inline std::ostream& operator<<(std::ostream& os, const AssertionError& a)
{
	a.Out(os);
	return os;
}

// A mostly internal class for keeping score.
class res_cnt {
	int ok, fail, err;

	public:
		// Create a 0 count.
		res_cnt(): 
			ok(0), fail(0), err(0) 
		{
		}

		// Count one ok.
		void add_ok() 
		{
		   	++ok; 
		}
		
		// Count one fail.
		void add_fail() 
		{
		   	++fail; 
		}
		
		// Count one error.
		void add_err() 
		{
		   	++err; 
		}
		
		// get ok count.
		int n_ok() 
		{
		   	return ok; 
		}
		
		// get fail count.
		int n_fail() 
		{
		   	return fail; 
		}
		
		// get error count.
		int n_err() 
		{
		   	return err; 
		}
		
		// get total count.
		int n() 
		{
		   	return ok+fail+err; 
		}
};

/**
 * The standard text based tester. It implements the Visitor pattern for the
 * test and suite classes, and executes each test case in a depth first
 * traversal, toting the score of test cases.
 *
 * The class might be used for test executers aimed at other environments,
 * e.g. a GUI based version.
 *
 * Please note that this class is automagically instantiated by the main
 * method supplied in the library. This means that you might very well do all
 * your testing without directly laying hans on this fellow.
 */
class Tester : public Visitor {
	std::ostream& os;
	bool verbose; // list succeded tests
	bool line_fmt; // format output with file and line
	std::stack<res_cnt> accu;
	res_cnt n_suite, n_test;

	void Disp(Test& t, const std::string&);
	void Write(Test& t);
	void Write(Test& t, AssertionError& e);
	void Write(Test& t, RuntimeException& e);
	void Write(Test& t, int dummy);

	public:
		/**
		 * Create a text tester.
		 * \param os the stream to write results to.
		 * \param verbose whether to report on succesful tests
		 */
		Tester(std::ostream& os, bool verbose = false, bool line = false): 
			os(os), verbose(verbose), line_fmt(line) 
		{
		}
	
		// Get the score for tests
		res_cnt res_tests() 
		{
		   	return n_test; 
		}
		
		// Get the score for suites
		res_cnt res_suites() 
		{
		   	return n_suite; 
		}
		
		// Write the summary
		virtual void Summary();
		
		// Part of the Visitor pattern.
		virtual void Visit(Test&);
		
		// Part of the Visitor pattern.
		virtual void Visit(Suite&);
		
		// Part of the Visitor pattern; Visit to suite after children.
		virtual void Visit(Suite& t, int);
};

// A runner is the base class for the objects that actually processes the tests 
// from main. Main simply invokes the run_tests method of the current test runner
class test_runner {

	public:
		virtual ~test_runner()
		{
		}

		// run all the tests with arguments in the argc, argv set
		virtual bool run_tests(int argc, const char** argv) = 0;
};

// A plain test runner for the ordinary text version.
class plain_runner : public test_runner {
	public:
		// Run the tests specified in argv, starting at i
		bool run_tests(int argc, const char** argv)
		{
			bool res = true;

			res = run_test();
			return res;
		}

	private:
		// Run a test found in the suite::main() test by id. If id is empty run the main test
		bool run_test(const std::string& id = "")
		{
			Test* tp = Suite::Main().Find(id);
			if (!tp) {
				return false;
			}
			return run_test(tp);
		}

		// Run the test and return true if succesful. @see{run_test-id}
		bool run_test(Test* tp)
		{
			Tester tst(std::cout, verbose, line_fmt);
			tp->Visit(&tst);
			tst.Summary();
			res_cnt res(tst.res_tests());
			return res.n_err() == 0 && res.n_fail() == 0;
		}

};

}

#endif

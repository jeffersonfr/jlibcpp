// Copyright (C) 2001 Claus Dreby

#include "junit.h"

#include <sstream>
#include <stdexcept>

using namespace std;
using namespace jcommon;

// Unit++ Test
namespace {

// a test case that can fail with any exception
class test_test : public Test {
	public:
		enum result { 
			succes, 
			fail, 
			error, 
			exotic 
		};
	
		test_test(string name, result res = succes): 
			Test(name), res(res) 
		{
		}

		virtual void operator()()
		{
			switch (res) {
			case succes: break;
			case fail: assert_fail("test_test");
			case error: throw std::out_of_range("ranged");
			case exotic: throw 4711;
			}
		}

	private:
		result res;
};

// The test suite for the unit++ library
class UnitTestSample : public Suite {
	void create()
	{
		test_test a_loc_test("local");
	}

	void assert_ok()
	{
		string s("ok");
		assert_true("assert_true(true)", true);
		assert_eq("assert_eq(int)", 7, 7);
		assert_eq("assert_eq(char*, string)", "ok", s);
	}

	void fail()
	{
		string s("fejl");
		bool ok = true;
		try {
			assert_true("assert_true(false)", false);
			ok = false;
		} catch (AssertionError e) {
			ostringstream oss;
			oss << e;
			assert_eq("assert_true(false) output", "assert_true(false) [assertion failed]", oss.str());
		}
		if (!ok)
			assert_fail("no exception from assert_true(false)");
		try {
			assert_eq("assert_eq(int)", 5, 7);
			ok = false;
		} catch (AssertValueError<int,int> e) {
			ostringstream oss;
			oss << e;
			assert_eq("assert_eq(int) output", "assert_eq(int) [expected: `5' got: `7']", oss.str());
		}
		if (!ok)
			assert_fail("no exception from assert_eq(int)");
		try {
			assert_eq("assert_eq(char*, string)", "ok", s);
			ok = false;
		} catch (AssertValueError<const char*, string> e) {
		} catch (AssertValueError<char*, string> e) { // MSVC++ bug
		}

		if (!ok)
			assert_fail("no exception from assert_eq(const char*, string)");
	}

	void tester_visit()
	{
		ostringstream os;
		Tester tst(os);
		root.Visit(&tst);
		assert_eq("tests ok", 3, tst.res_tests().n_ok());
		assert_eq("tests error", 2, tst.res_tests().n_err());
		assert_eq("tests fail", 1, tst.res_tests().n_fail());
		assert_eq("suites ok", 1, tst.res_suites().n_ok());
		assert_eq("suites error", 2, tst.res_suites().n_err());
		assert_eq("suites fail", 1, tst.res_suites().n_fail());
	}
	
	void ex_test()
	{
		throw out_of_range("expected");
	}

	void get_by_id()
	{
		Test* p = root.GetChild("s2");
		assert_true("found s2", p != 0);
		Suite* sp = dynamic_cast<Suite*>(p);
		assert_true("s2 was suite", sp != 0);
		assert_eq("right s2", "S2", sp->GetName());
		p = sp->GetChild("t20");
		assert_true("found t20", p != 0);
		assert_eq("not suite", static_cast<Suite*>(0),dynamic_cast<Suite*>(p));
	}

	void vec()
	{
		string s = "three.blind.mice";
		vector<string> v(vectorize(s,'.'));
		assert_eq("v[0]", string("three"), v[0]);
		assert_eq("v[1]", string("blind"), v[1]);
		assert_eq("v[2]", string("mice"), v[2]);
		assert_eq("size", size_t(3), v.size());
		v = vectorize(s,'-');
		assert_eq("no match", s, v[0]);
		assert_eq("no match size", size_t(1), v.size());
	}

	void empty_vec()
	{
		string s("");
		vector<string> v(vectorize(s,'.'));
		assert_eq("size", size_t(0), v.size());
		s = "one..three";
		v = vectorize(s,'.');
		assert_eq("v[0]", string("one"), v[0]);
		assert_eq("v[1]", string(""), v[1]);
		assert_eq("v[2]", string("three"), v[2]);
		assert_eq("size", size_t(3), v.size());
	}

	void find()
	{
		Test* tp = root.Find("s2.s21.t210");
		assert_eq("t210", t210, tp);
		tp = root.Find("s1.s21");
		assert_eq("bad mid", static_cast<Test*>(0), tp);
	}

	Suite root;
	Test* t210;
	bool do_fail;
	
	void fail_on_flag()
	{
		assert_true("Fail option not set", !do_fail);
	}

	void test_line()
	{
		ostringstream os;
		Tester tst(os, false, true);
		string fname(__FILE__);
		// int l = __LINE__ + 4;
		struct loc_test : Test {
			loc_test() : Test("local") {}
			virtual void operator()() {
				assert_fail("failure");
			}
		} lc;
		tst.Visit(lc);

		/*
		string r(os.str());
		string::size_type i = r.find(':');
		assert_eq("file name wrong", fname, r.substr(0, i));
		string::size_type j = r.find(':', ++i);
		assert_eq("Wrong line", l, atoi(r.substr(i, j).c_str()));
		*/
	}

public:
	UnitTestSample() : Suite("Unit++ test suite"), root("The root")
	{
		do_fail = false;

		/*
		options().add("f", new options_utils::opt_flag(do_fail));
		options().alias("fail", "f");
		*/
		Suite* s1;
		Suite* s2;
		Suite* s21;
		root.Add("s1", s1 = new Suite("S1"));
		root.Add("s2", s2 = new Suite("S2"));
		s2->Add("s21", s21 = new Suite("S21"));
		s1->Add("t10", new test_test("T10"));
		s1->Add("t11", new test_test("T11"));
		s2->Add("t20", new test_test("T20", test_test::error));
		s2->Add("t22", new test_test("T22", test_test::exotic));
		s21->Add("t210", t210 = new test_test("T210"));
		s21->Add("t211", new test_test("T211", test_test::fail));
		// 
		// Adding testcases
		Suite::Main().Add("unitpp", this);
		Add("create", TestCase(this, "Create a test", &UnitTestSample::create));
		Add("assert_ok", TestCase(this, "Assert ok", &UnitTestSample::assert_ok));
		Add("fail", TestCase(this, "Assert fail", &UnitTestSample::fail));
		Add("tester_visit", TestCase(this, "Visit", &UnitTestSample::tester_visit));
		Add("exception", TestCase(new ExceptionTest<out_of_range>(
			__FILE__, __LINE__, TestCase(this, "gen ex", &UnitTestSample::ex_test))));
		Add("id_get", TestCase(this, "Get by id", &UnitTestSample::get_by_id));
		Add("vec", TestCase(this, "Vectorize", &UnitTestSample::vec));
		Add("empty_vec", TestCase(this, "Vectorize empty", &UnitTestSample::empty_vec));
		Add("find", TestCase(this, "find", &UnitTestSample::find));
		Add("fail", TestCase(this, "fail on option", &UnitTestSample::fail_on_flag));
		Add("line", TestCase(this, "line number mode", &UnitTestSample::test_line));
	}
} * theUnitTestSample = new UnitTestSample();

}

// Main Function
test_runner* runner = 0;

void set_tester(test_runner* tr)
{
	runner = tr;
}

int main(int argc, const char* argv[])
{
	plain_runner plain;
	if (!runner)
		runner = &plain;
	return runner->run_tests(argc, argv) ? 0 : 1;
}


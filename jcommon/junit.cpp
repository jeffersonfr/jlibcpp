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
 ***************************************************************************/
#include "junit.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <functional>

#include <time.h>

namespace jcommon {

bool verbose = true;
bool line_fmt = false;

void Tester::Summary()
{
	os << "Tests [Ok-Fail-Error]: [" << n_test.n_ok() << '-'
	<< n_test.n_fail() << '-' << n_test.n_err() << "]\n";
}
void Tester::Visit(Test& t)
{
	try {
		t();
		n_test.add_ok();
		Write(t);
	} catch (AssertionError& e) {
		n_test.add_fail();
		Write(t, e);
	} catch (RuntimeException& e) {
		n_test.add_err();
		Write(t, e);
	} catch (...) {
		n_test.add_err();
		Write(t, 0);
	}
}

void Tester::Visit(Suite& t)
{
	if (verbose)
		os << "****** " << t.GetName() << " ******" << std::endl;
	accu.push(n_test);
}

void Tester::Visit(Suite& , int)
{
	res_cnt r(accu.top());
	accu.pop();
	if (n_test.n_err() != r.n_err())
		n_suite.add_err();
	else if (n_test.n_fail() != r.n_fail())
		n_suite.add_fail();
	else
		n_suite.add_ok();
}
void Tester::Write(Test& t)
{
	if (verbose)
		Disp(t, "OK");
}
void Tester::Disp(Test& t, const std::string& status)
{
	os << status << ": " << t.GetName() << std::endl;
}
void Tester::Write(Test& t, AssertionError& e)
{
	if (line_fmt)
		os << e.GetFile() << ':' << e.GetLine() << ':';
	Disp(t, "FAIL");
	os << e << '\n';
}
void Tester::Write(Test& t, RuntimeException& e)
{
	Disp(t, "ERROR");
	os << "     : [" << typeid(e).name() << "] " << e.what() << '\n';
}
void Tester::Write(Test& t, int )
{
	Disp(t, "ERROR");
	os << "     : " << "unknown exception" << '\n';
}

void Test::Visit(Visitor* v)
{
	v->Visit(*this);
}
TestCase::TestCase(Test* t)
: cnt(new size_t(1)), tst(t)
{
}
TestCase::TestCase(const TestCase& t)
: cnt(t.cnt), tst(t.tst)
{
	(*cnt)++;
}
void TestCase::dec_cnt()
{
	if (--(*cnt) == 0) {
		delete cnt;
		delete tst;
	}
}
TestCase::~TestCase()
{
	dec_cnt();
}
TestCase& TestCase::operator=(const TestCase& t)
{
	++*(t.cnt);
	dec_cnt();
	cnt = t.cnt;
	tst = t.tst;
	return *this;
}
Suite& Suite::Main()
{
	static Suite instance("top");
	return instance;
}
Test* Suite::GetChild(const std::string& id)
{
	std::vector<std::string>::iterator p = std::find(ids.begin(), ids.end(), id);
	if (p != ids.end())
		return &(static_cast<Test&>(tests[p - ids.begin()]));
	return 0;
}
std::vector<std::string> vectorize(const std::string& str, char c)
{
	std::vector<std::string> res;
	std::string::const_iterator s, p;
	for (s = str.begin(); s != str.end(); ) {
		p = find(s, str.end(), c);
		res.push_back(std::string(s, p));
		s = (p == str.end()) ? p : p + 1;
	}
	return res;
}
Test* Suite::Find(const std::string& id)
{
	std::vector<std::string> ss(vectorize(id, '.'));
	Test* tp = this;
	for (std::vector<std::string>::iterator p = ss.begin(); p != ss.end(); ++p)
		if (!(tp = tp->GetChild(*p)))
			break;
	return tp;
}
void Suite::Add(const std::string& id, const TestCase& t)
{
	ids.push_back(id);
	tests.push_back(t);
}
void Suite::Visit(Visitor* v)
{
	v->Visit(*this);
	for_each(tests.begin(), tests.end(),
		std::bind2nd(std::mem_fun_ref(&TestCase::Visit), v));
	v->Visit(*this, 0);
}

void AssertionError::Out(std::ostream& os) const
{
	os << msg << std::string(" [assertion failed]");
}

}

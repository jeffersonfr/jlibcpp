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

using namespace jcommon;

// Unit++ Test
namespace {

// The test suite for the unit++ library
class UnitCalc : public Suite {

	int fator1,
		fator2;
	bool do_fail;
	
	void create()
	{
		fator1 = 3;
		fator2 = 2;
	}

	void add()
	{
		assert_eq("assert_eq(int)", fator1+fator2, 5);
	}

	void sub()
	{
		assert_eq("assert_eq(int)", fator1-fator2, 1);
	}

	void mul()
	{
		assert_eq("assert_eq(int)", fator1*fator2, 6);
	}

	void div()
	{
		assert_eq("assert_eq(int)", fator1/fator2, 1);
	}

public:
	UnitCalc():
	   	Suite("Unit++ test suite")
	{
		do_fail = false;

		Suite::Main().Add("unitpp", this);
		
		Add("create", TestCase(this, "Initializing fator1 and fator2", &UnitCalc::create));
		Add("add", TestCase(this, "Add", &UnitCalc::add));
		Add("sub", TestCase(this, "Sub", &UnitCalc::sub));
		Add("mul", TestCase(this, "Mul", &UnitCalc::mul));
		Add("div", TestCase(this, "Div", &UnitCalc::div));
	}
} * theUnitCalc = new UnitCalc();

}

int main(int argc, const char* argv[])
{
	plain_runner plain;
	
	return plain.run_tests(argc, argv) ? 0 : 1;
}


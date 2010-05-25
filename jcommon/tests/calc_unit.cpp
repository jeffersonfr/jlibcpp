#include "jcommonlib.h"

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


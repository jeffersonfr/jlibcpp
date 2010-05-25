#include "jregexp.h"
 
using namespace std;
using namespace jregexp;

void test_bracket_expr( string expr, string input, int rec_size = 1, bool recognize = true )
{
	choice<bracket_expr<char> >	the_bracket_expr;	
	the_bracket_expr.parse( expr.c_str(), expr.length() );
	
	if( the_bracket_expr.is_parsed() )
	{
		if( the_bracket_expr.parsed_size() == expr.length() )
		{
			cout << expr << " parsed succesfully input = " << input;
			if( (int)the_bracket_expr->recognize( input.c_str(), input.length() ) == rec_size )
			{
		 		if( !recognize )
				{
					cout << " recognized wrongly" << endl;
				}
				else
				{
					cout << " recognized succesfully" << endl;
				}
			}
			else
			{
		 		if( !recognize )
				{
					cout << " not recognized succesfully" << endl;
				}
				else
				{
					cout << " not recognized wrongly" << endl;
				}
			}
		}
		else
		{
			cout << expr << " parsed size don't match expected size" << endl;
		}
	}
	else
	{
		cout << expr << " not parsed" << endl;
	}
	
}

void test_ere( wstring regexp_definition, 
		wstring input, 
		const unsigned long expected_pos = 0,
		unsigned long expected_size=0,
		const bool expected_definition_result = true, 
		const bool expected_recognize_result = true )
{
	basic_non_terminal< wchar_t, ere<wchar_t> > rxp;

	rxp.parse(regexp_definition.c_str(), 
			regexp_definition.length() );
	
	if( !rxp.is_parsed() )
	{
		if( expected_definition_result )
		{
			wcout << L"- \"" << regexp_definition << L"\" wrongly detected as invalid" << endl;
		}
		else
		{
			wcout << L"  \"" << input << L"\" succesfully detected as invalid " << endl;
		}
		return;
	}

	if( rxp.parsed_size() != regexp_definition.length()  )
	{
		wcout << L"- \"" << regexp_definition << L"\" wrong size of valid regexp=";
		wcout << rxp.parsed_size() << " expected size=";
		wcout << regexp_definition.length() << endl;
		return;
	}
	
	rxp->recognize( input.c_str(), input.length() );
	if( !rxp->is_recognized() )
	{
		if( expected_recognize_result )
		{
			wcout << L"- \"" << input << L"\" wrongly not recognized by \"";
			wcout << regexp_definition << L"\"" << endl;
		}
		else
		{
			wcout << L"  \"" << input << L"\" succesfully not recognized by \"";
			wcout << regexp_definition << L"\"" << endl;
		}
		return;
	}
	
	
	if( rxp->recognized_position() != expected_pos )
	{
		wcout << L"- \"" << input << L"\" wrong position (" << rxp->recognized_position();
		wcout << ") recognized by \"";
		wcout << regexp_definition << L"\" ,expected position =" << expected_pos << endl;
		return;
	}
	
	// if( expected_size == 0 ) expected_size = input.length();
	
	if( rxp->recognized_size() != expected_size )
	{
		wcout << L"- \"" << input << L"\" wrong size (" << rxp->recognized_size();
		wcout << ") recognized by \"";
		wcout << regexp_definition << L"\" ,expected size =" << expected_size << endl;
		return;
	}
	
	// succesfully recognized - go recursively to retrieve all subexpressions
										   		
	wcout << L"  \"" << input << L"\" succesfully recognized by \"";
	wcout << regexp_definition << L"\" ,size=" << rxp->recognized_size();
	wcout << " position=" << rxp->recognized_position() << endl;	
	
	matches m;
	rxp->assign_matches( m );
	
	matches::matches_coll mc;
	m.get_all(mc);
	matches::matches_coll::iterator it = mc.begin();
	while( it != mc.end() )
	{
		cout << "    match (subexpr) key = " <<  (*it).first << "  position=";
		cout << (*it).second.get_pos() << " size=" << (*it).second.get_size();
		cout << "; " << endl;
		it++;
	}
	cout << endl;
}

void test_regexp_parser( wstring regexp_pattern, 
						wstring input, 
						const unsigned long expected_pos = 0,
						unsigned long expected_size=0,
						const bool expected_definition_result = true, 
						const bool expected_recognize_result = true )
{
	wregexp_parser rxp(regexp_pattern);
	rxp.parse(input.c_str(), input.length() );

	if( !rxp.is_pattern_valid() )
	{
		if( expected_definition_result )
		{
			wcout << L"!!! \"" << regexp_pattern << L"\" wrongly detected as invalid" << endl;
		}
		else
		{
			wcout << L"  \"" << input << L"\" succesfully detected as invalid " << endl;
		}
		return;
	}

	
	if( !rxp.is_parsed() )
	{
		if( expected_recognize_result )
		{
			wcout << L"!!! \"" << input << L"\" wrongly not recognized by \"";
			wcout << regexp_pattern << L"\"" << endl;
		}
		else
		{
			wcout << L"  \"" << input << L"\" succesfully not recognized by \"";
			wcout << regexp_pattern << L"\"" << endl;
		}
		return;
	}
	
	
	if( rxp.recognized_position() != expected_pos )
	{
		wcout << L"!!! \"" << input << L"\" wrong position (" << rxp.recognized_position();
		wcout << ") recognized by \"";
		wcout << regexp_pattern << L"\" ,expected position =" << expected_pos << endl;
		return;
	}
	
	
	if( rxp.recognized_size() != expected_size )
	{
		wcout << L"!!! \"" << input << L"\" wrong size (" << rxp.recognized_size();
		wcout << ") recognized by \"";
		wcout << regexp_pattern << L"\" ,expected size =" << expected_size << endl;
		return;
	}
	
	// succesfully recognized
	wcout << L"  \"" << input << L"\" succesfully recognized by \"";
	wcout << regexp_pattern << L"\" ,size=" << rxp.recognized_size();
	wcout << " position=" << rxp.recognized_position() << endl;	
	
	// retrieve all subexpressions data (position and size in original input buffer)
	matches m;
	rxp.assign_matches( m );
	
	matches::matches_coll mc;
	m.get_all(mc);
	matches::matches_coll::iterator it = mc.begin();
	while( it != mc.end() )
	{
		cout << "    match (subexpr) key = " <<  (*it).first << "  position=";
		cout << (*it).second.get_pos() << " size=" << (*it).second.get_size();
		cout << "; " << endl;
		it++;
	}
	
	// Following piece of code illustrates, how to retrieve subexpression. Note, 
	// that at first was called rxp.assign_matches(). Suppose we have pattern "a(a(b){1,3})" 
	// and input "aaabbbb". Now we are interested in subexression 1.1.2 = second b

	//	match subexpr = m.get("1.1.2");
	//	if( subexpr.is_valid() )
	//	{
	//		ulong pos = subexpr.get_pos();
	//		ulong size = subexpr.get_size();	
	// 		do some work
	//	}
	
	// in pos variable will be stored 4 and in size will be 1 (position base is 0)
	cout << endl;
}

int main(int argc, char* argv[])
{
	//setlocale(LC_ALL,"cs_CZ.iso88592");
	// unsigned long size = 0;
	
	// this line has no effect, if #TRACE is not uncommented above
	// parsers_common::trace_context::get().trace_on();
	
	test_bracket_expr( "[--8z--]", "-" );
	//parsers_common::trace_context::get().trace_on();
	test_bracket_expr( "[[.-.]]", "-" );
	//parsers_common::trace_context::get().trace_off();
	test_bracket_expr( "[a-bc]", "ch", true, 1 );
	test_bracket_expr( "[[:print:]]", "1" );
	test_bracket_expr( "[8--]", "-" );
	test_bracket_expr( "[a-c]", "a" );
	test_bracket_expr( "[^a-f]", "g" );
	test_bracket_expr( "[^a-f]", "c", 1, false );
	test_bracket_expr( "[[:alnum:]]", "a" );
	test_bracket_expr( "[--z]", "-" );
	test_bracket_expr( "[aa-b]", "b" );
	test_bracket_expr( "[a-bc-h]", "g" );
	test_bracket_expr( "[a-bc-de-f]", "a" );
	test_bracket_expr( "[[.a.]]", "a" );
	test_bracket_expr( "[[=a=]]", "a" );
	
	//parsers_common::trace_context::get().trace_on();
	trace_context::get().set_max_scope(8);
	test_bracket_expr( "[]]", "]" );
	test_bracket_expr( "[\\]]", "]" );
	test_bracket_expr( "[\\-]", "-" );
	test_bracket_expr( "[\\^]", "^" );
	test_bracket_expr( "[A-\\]]", "X" );
	test_bracket_expr( "[a-\\]]", "x", 1, false );
	test_bracket_expr( "[\\]-a]", "x", 1, false );
	test_bracket_expr( "[a-\\-]", "x", 1, false ); // result is recognized ????
	test_bracket_expr( "[\\--a]", "x", 1, false);
	test_bracket_expr( "[\\--b]", "a" );
	test_bracket_expr( "[\\^ab]", "a" );
	test_bracket_expr( "[\\^ab]", "^" );
	

	
	//parsers_common::trace_context::get().trace_off();
	
	// don't know how to test following commented line - also in czech locale 
	// the 'ch' should be a collating element - multibyte character, for which mblen 
	// must return 2 - now it always returns 1
	test_bracket_expr( "[[.ch.]]", "ch" );

	
	// input parameters:
	// definition, 
	// input, 
	// expected_pos = 0, 
	// expected_size=0 
	// expected_definition_result = true, 
	// expected_recognize_result = true, 
	
	test_ere( L"\".*\"", L"abc \"def\" \"ghi\" jkl", 4, 11 );	
	test_ere( L"\".*?\"", L"abc \"def\" \"ghi\" jkl", 4, 5 ); // for laziness test
	test_ere( L"a(a(b){1,3})", L"aaabbbb", 0, 6 );
	test_ere( L"(ab){2,5}", L"abab", 0, 4 );
	test_ere( L"(ab){2,5}", L"ab", 0, 0, true, false );
	test_ere( L"(a){2}", L"aa", 0, 2); 
	test_ere( L"(a){1}", L"a", 0, 1); 
	test_ere( L"((a){1,2}){2}", L"aa", 0, 2); 
	test_ere( L"((abc){1,3}){2,5}", L"abcabcabc", 0, 9); 
	test_ere( L"(((abc){2,5}){2,5}){2,5}", L"abcabcabcabcabcabcabcabc", 0, 24 );
	test_ere( L"(((abc){2,5}){2,5}){2,5}", L"abcabcabcabcabcabcabc", 0, 0, true, false );
	test_ere( L"(a)*", L"aaaa", 0, 4); 
	test_ere( L"bb*", L"abbbc", 1, 3); 
	test_ere( L"[ab]*", L"cdabababb", 2, 7); 
	test_ere( L"(abc){1,3}", L"abcabcabc", 0, 9); 
	test_ere( L"(abc){1,3}", L"cabc", 0, 4); 
	// the size is 10, as also first c falls into subexression recognition - 
	// subexpression size is 3, but position of the subexpression match is 1 - 
	// skips the first 'c'.
	test_ere( L"(abc)(cab)d{1,3}", L"cabcddcabd", 0, 10); 
	test_ere( L"(abc)(cab)d{1,3}", L"cabcddcabddddd", 0, 12);
	test_ere( L"d{1,3}d{2,5}", L"ddddd", 0, 5); 
	test_ere( L"d{1,3}", L"ddddd", 0, 3); 
	test_ere( L"((abc){1,2}){2,5}", L"abcabcabcabc", 0, 12); 
	test_ere( L"(abc){1,2}{2}", L"abcabcabcabc", 0, 12); 
	test_ere( L"a(a(b){3})", L"caabcbdcb", 1, 8); 
	test_ere( L"^a$", L"ba", 0, 0, true, false );
	test_ere( L"^a$", L"a", 0, 1 );	
	test_ere( L"^a$", L"bac", 0, 0, true, false );
	test_ere( L"^a$", L"abc", 0, 0, true, false );
	test_ere( L"^aaa$", L"aaa", 0, 3 );
	test_ere( L"^a(a)$", L"aba", 0, 3 );
	test_ere( L"^a(a)a$", L"abbbaa", 0, 6 );
	test_ere( L"^a(a(b))$", L"abbaccb", 0, 7); 
	test_ere( L"a|b", L"b", 0, 1); 
	test_ere( L"(a)(b)", L"ab", 0, 2); 
	test_ere( L"(a)|(b)", L"a", 0, 1); 
	test_ere( L"(a)|(b)|c|dd", L"add", 1, 2); 
	test_ere( L"(a)(b|c)", L"ac", 0, 2); 
	test_ere( L"(a|b)(c|d)", L"ac", 0, 2); 
	test_ere( L"(ai|b)(c|d)", L"aic", 0, 3); 

	// following fails - right anchoring subpattern - also in egrep not recognized?
	// test_ere( L"^a(a(^b$)(a$))", L"abbaba", 0, 6); 
	
	test_ere( L"^a(a(^b)a$)", L"abbaba", 0, 6); 
	test_ere( L"(wee|week)(knights|night)", L"weeknights", 0, 10); 
	test_ere( L"^a(a(b){3})$", L"aabcbdcb", 0, 8); 
	
	
	////////////////////////////////////////////////////////////////////////////
	
	test_regexp_parser( L"(ab){2,5}", L"abab", 0, 4 );
	test_regexp_parser( L"(ab){2,5}", L"ab", 0, 0, true, false );
	test_regexp_parser( L"(a){2}", L"aa", 0, 2); 
	test_regexp_parser( L"(a){1}", L"a", 0, 1); 
	test_regexp_parser( L"((a){1,2}){2}", L"aa", 0, 2); 
	test_regexp_parser( L"((abc){1,3}){2,5}", L"abcabcabc", 0, 9); 
	test_regexp_parser( L"(((abc){2,5}){2,5}){2,5}", L"abcabcabcabcabcabcabcabc", 0, 24 );
	test_regexp_parser( L"(((abc){2,5}){2,5}){2,5}", L"abcabcabcabcabcabcabc", 0, 0, true, false );
	test_regexp_parser( L"(a)*", L"aaaa", 0, 4); 
	test_regexp_parser( L"bb*", L"abbbc", 1, 3); 
	test_regexp_parser( L"[ab]*", L"cdabababb", 2, 7); 
	test_regexp_parser( L"(abc){1,3}", L"abcabcabc", 0, 9); 
	test_regexp_parser( L"(abc){1,3}", L"cabc", 0, 4); 
	// the size is 10, as also first c falls into subexression recognition - 
	// subexpression size is 3, but position of the subexpression match is 1 - 
	// skips the first 'c'.
	test_regexp_parser( L"(abc)(cab)d{1,3}", L"cabcddcabd", 0, 10); 
	test_regexp_parser( L"(abc)(cab)d{1,3}", L"cabcddcabddddd", 0, 12);
	test_regexp_parser( L"d{1,3}d{2,5}", L"ddddd", 0, 5); 
	test_regexp_parser( L"d{1,3}", L"ddddd", 0, 3); 
	test_regexp_parser( L"((abc){1,2}){2,5}", L"abcabcabcabc", 0, 12); 
	test_regexp_parser( L"(abc){1,2}{2}", L"abcabcabcabc", 0, 12); 
	test_regexp_parser( L"a(a(b){3})", L"caabcbdcb", 1, 8); 
	test_regexp_parser( L"^a$", L"ba", 0, 0, true, false );
	test_regexp_parser( L"^a$", L"a", 0, 1 );	
	test_regexp_parser( L"^a$", L"bac", 0, 0, true, false );
	test_regexp_parser( L"^a$", L"abc", 0, 0, true, false );
	test_regexp_parser( L"^aaa$", L"aaa", 0, 3 );
	test_regexp_parser( L"^a(a)$", L"aba", 0, 3 );
	test_regexp_parser( L"^a(a)a$", L"abbbaa", 0, 6 );
	test_regexp_parser( L"^a(a(b))$", L"abbaccb", 0, 7); 
	test_regexp_parser( L"a|b", L"b", 0, 1); 
	test_regexp_parser( L"(a)(b)", L"ab", 0, 2); 
	test_regexp_parser( L"(a)|(b)", L"a", 0, 1); 
	test_regexp_parser( L"a|b", L"ca", 1, 1); 
	
	// do you feel the difference between this:
	test_regexp_parser( L"(a)|(b)", L"ca", 0, 2); 
	// and this?
	test_regexp_parser( L"(^a)|(b)", L"ca", 1, 1);
	// I think it is correct
	
	test_regexp_parser( L"(a)", L"ca", 0, 2); 
	test_regexp_parser( L"(^a)", L"ca", 1, 1); 
	
	test_regexp_parser( L"(a)|(b)|c|dd", L"add", 1, 2); 
	test_regexp_parser( L"(a)(b|c)", L"ac", 0, 2); 
	test_regexp_parser( L"(a|b)(c|d)", L"ac", 0, 2); 
	test_regexp_parser( L"(ai|b)(c|d)", L"aic", 0, 3); 

	// following fails - right anchoring subpattern - also in egrep not recognized?
	//test_regexp_parser( L"^a(a(^b$)a)", L"abbaba", 0, 6); 
	
	test_regexp_parser( L"^a(a(^b)a$)", L"abbaba", 0, 6); 
	test_regexp_parser( L"(wee|week)(knights|night)", L"weeknights", 0, 10); 
	test_regexp_parser( L"o(wee|week)(knights|night)", L"helloweeknights", 4, 11); 
	test_regexp_parser( L"(wee|week)(knights|night)", L"helloweeknights", 0, 15); 
	test_regexp_parser( L"(^wee|^week)(knights|night)", L"helloweeknights", 5, 10); 
	test_regexp_parser( L"^(^wee|^week)(knights|night)", L"helloweeknights", 0, 0, true, false); 
	test_regexp_parser( L"^a(a(b){3})$", L"aabcbdcb", 0, 8); 

	return 0;
	
}

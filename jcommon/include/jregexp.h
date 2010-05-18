/***************************************************************************
 *   Copyright (C) 2005 by <still unknown>                                 *
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
#ifndef J_REGEXP_H
#define J_REGEXP_H

#include "jobject.h"
#include "jcompileexception.h"

#include <sstream>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <climits>
#include <memory>

#define TRACE

namespace jregexp {

const char* NONTERMINAL = "non-terminal";
const char* CHOICE = "Choice";
const char* DEFAULT_TYPE_PARSER = "DefaultTypeParser";
const char* PATTERN_PARSER = "PatternParser";
const char* VALUE_PARSER = "ValueParser";

/*
 * \brief The default factory, which just calls default constructor of class A (class defined by template parameter). 
 * TODO - replace with allocator?
 *
 */
template< typename A >
class default_factory
{
public:
	A* create() 
	{
	   	return new A();
   	}

};

// template< typename E > class basic_parser;

/*
 * \brief The base class for all parsers, declares base methods for
 * parsing and formatting. The list of objects of this class is used in compound
 * parsers (like \ref basic_non_terminal or \ref basic_choice)
 */
template< typename E >class basic_parser
{
	public:
		typedef std::list< basic_parser<E> * > parser_list;
		typedef typename parser_list::iterator parser_list_iterator;

		//! Enum defines only three basic types (+ undefined)
		enum e_symbol_type {  
			terminal_type,
			non_terminal_type, 
			choice_type, 
			undefined_type 
		};

		typedef e_symbol_type symbol_type;
	
		//! The default constructor
		basic_parser(const char* name, symbol_type type = terminal_type) :
			m_is_parsed(false), 
			m_is_formatted(false), 
			m_parsed_size(0),
			m_formatted_size(0),
			m_name(name) 
		{
			m_type = type;
		}
			
		//! The copy constructor
		basic_parser( const basic_parser& rhs ) :
			m_is_parsed( rhs.m_is_parsed ),
			m_is_formatted( rhs.m_is_formatted ),
			m_parsed_size( rhs.m_parsed_size ),
			m_formatted_size( rhs.m_formatted_size ),
			m_type( rhs.m_type ),
			m_name( rhs.m_name ) {};

		//! The destructor
		virtual ~basic_parser()
		{	
		}	

		//! Strategy specific method - returns the type of symbol
		symbol_type get_symbol_type() 
		{
			return this->m_type; 
		}

		//! Used for owned objects manipulation, redefined in \ref basic_object_parser
		virtual bool is_valid() const = 0;
		//! Used for owned objects manipulation, redefined in \ref basic_object_parser
		virtual void invalidate() = 0;

		/*
		 * \brief The parse method using which the input buffer is searched and the value 
		 * is copied from buffer into parsed objects manipulated by \ref 
		 * basic_object_parser. For parse method is used different implementation 
		 * depending on type of parser - if terminal_type, non_terminal_type or choice_type.
		 */
		virtual unsigned long parse( const E *buf, const unsigned long buf_length ) = 0;

		/*
		 * \brief The parse method using which the input stream is searched and the value 
		 * is copied from buffer into parsed objects manipulated by \ref basic_object_parser. 
		 * For parse method is used different implementatiobase_parsersn depending on type of 
		 * parser - if terminal_type, non_terminal_type or choice_type
		 */
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is ) = 0;

		/*
		 * \brief Used for formatting the class to output stream
		 *
		 */
		virtual std::basic_ostream<E>& format( std::basic_ostream<E>& os ) = 0;

		/*
		 * \brief Returns the size of buffer succesfully parsed last time.
		 *
		 */
		unsigned long parsed_size() const 
		{
			return this->m_parsed_size; 
		}

		/*
		 * \brief Returns the size of data succesfully formatted into output.
		 *
		 */
		unsigned long formatted_size() const 
		{
			return this->m_formatted_size; 
		}

		/*
		 * \brief Identifies whether last call to \ref parse() was succesfull or not
		 *
		 * Usually it is possible to check return value of \ref parse method -
		 * the size of parsed buffer, if greater then zero, the object was 
		 * succesfully parsed. This is not valid for \ref basic_subset - if  
		 * \ref basic_subset::parse() method returns zero, it is succesfully
		 * parsed anyway, as it accepts also no succesfull parse for all embedded parsers
		 *
		 */
		bool is_parsed() const 
		{ 
			return this->m_is_parsed; 
		}

		/*
		 * \brief Identifies whether formatting was succesfull by call to \ref format()
		 *
		 */
		bool is_formatted() const 
		{
			return this->m_is_formatted; 
		}

		/*
		 * \brief The method allows nesting of parsers in declaration, like non_terminal< non_terminal< class > > - rarely used
		 *
		 */
		virtual void push_parsers( std::list< basic_parser<E> * >& the_parser_list )
		{ 
			the_parser_list.push_back(this);
		}

		/*
		 * \brief Allows to change the flag identifying the \ref parse() result
		 * \param the_parsed_flag new flag value
		 * \return previous flag value
		 *
		 */
		bool change_parsed_flag( const bool the_parsed_flag = true ) 
		{ 
			bool b = this->m_is_parsed; 
			this->m_is_parsed= the_parsed_flag; 
			return b;
		}

		/*
		 * \brief Used for trace/debugging
		 *
		 */
		virtual const char* get_name() const 
		{ 
			return this->m_name; 
		}

		virtual bool try_next() 
		{
			this->m_is_parsed=false;return false;
		}

		virtual void set_to_begin()
		{
		}

	protected:
		//! Flag identifyng the result of last call to \ref parse() method.
		bool m_is_parsed;
		//! Flag identifyng the result of last call to \ref format() method.
		bool m_is_formatted;
		//! The size of succesfully parsed data, can be zero, if \ref parse() failed.
		unsigned long m_parsed_size;
		//! The size of succesfully formatted data, can be zero, if \ref format() failed.
		unsigned long m_formatted_size;
		//! Used for trace and/or debugging
		const char *m_name;

		/*
		 * \brief The type of grammar symbol
		 *
		 * Specific for parsing algorithm. Used in \ref basic_context_free_grammar_strategy for finding terminal types in definition
		 *
		 */
		symbol_type m_type;		

		/*
		 * \brief Every \ref parse() method implementation should call this (or specialized) 
		 * method at the beginning of the method.
		 *
		 */
		virtual void prepare_for_parsing()
		{
			// cout << get_name() << endl;
			this->m_is_parsed = false;
			this->m_parsed_size = 0;
		}


		/*
		 * \brief Every \ref format() method implementation should call this (or specialized) 
		 * method at the beginning of the method.
		 *
		 */
		virtual void prepare_for_formatting() 
		{
			this->m_is_formatted = false;
			this->m_formatted_size = 0;
		}

		/*
		 * \brief Allows to change the flag identifying the \ref format() result
		 * \param the_formatted_flag new flag value
		 * \return previous flag value
		 *
		 */
		bool change_formatted_flag( const bool the_formatted_flag = true ) 
		{ 
			bool b = this->m_is_formatted; 
			this->m_is_formatted = the_formatted_flag; 
			return b;
		}

		/*
		 * \brief Allows to change the size of succesfully parsed data 
		 * \param the_parsed_size new size value
		 * \return previous size value
		 *
		 */
		unsigned long change_parsed_size( const unsigned long the_parsed_size )
		{
			unsigned long size = this->m_parsed_size;
			this->m_parsed_size = the_parsed_size;
			return size;
		}

		/*
		 * \brief Allows to change the size of succesfully formatted data 
		 * \param the_formatted_size new size value
		 * \return previous size value
		 *
		 */
		unsigned long change_formatted_size( const unsigned long the_formatted_size )
		{
			unsigned long size = this->m_formatted_size;
			this->m_formatted_size = the_formatted_size;
			return size;
		}

		/*
		 * \brief Used for trace/debugging
		 *
		 */
		void set_name( const char* name ) 
		{
			this->m_name = name; 
		}	

};

	template< typename E >
std::basic_ostream<E>& operator <<( std::basic_ostream<E>& os, basic_parser<E>& parser )
{
	return parser.format(os);
}

	template< typename E >
inline std::basic_istream<E>& operator >>( std::basic_istream<E>& is, basic_parser<E>& parser )
{
	return parser.parse( is );
}

typedef basic_parser<char>::parser_list parsers;
typedef basic_parser<wchar_t>::parser_list wparsers;

//template< typename E, typename A, typename F=default_factory<A> >
//class basic_object_parser;

/*
 * 	The object parser, parsed data are saved in objects of type given by 
 * 	template parameter A, for terminal the parsed object is usually one item,
 * 	for non terminal the pointed object is instance of 
 * 	class which implements method push_parsers(), where it defines another
 * 	embedded parsers in specific order. The A objects are created by default 
 * 	factory or the factory method can be given as third template parameter.
 */
template< typename E, typename A, typename F=default_factory<A> >
class basic_object_parser : public basic_parser<E>
{
	public:
		typedef typename basic_parser<E>::symbol_type symbol_type;

		//! Default constructor
		basic_object_parser( const char* name, symbol_type type = basic_parser<E>::terminal_type);

		//! The constructor making the copy
		basic_object_parser( const char* name, const A &rhs, symbol_type type = basic_parser<E>::terminal_type);

		//! The copy constructor
		basic_object_parser( const basic_object_parser& rhs );

		virtual ~basic_object_parser();

		virtual bool is_valid() const 
		{ 
			return this->m_parsed_object != NULL; 
		}

		/*
		 * \brief Returns the composed parsed object
		 *
		 */
		A* get()
		{
			return this->m_parsed_object;
		}

		/*
		 * \brief Returns the composed parsed object
		 * 
		 */
		A* operator->()
		{
			return this->m_parsed_object;
		}

		/*
		 * \brief The same as auto_ptr<>::get(), additionally creates a new instance of allocated object, if NULL pointer.
		 *
		 */
		virtual A& get_valid()
		{
			if( !this->is_valid() ) 
			{
				this->m_parsed_object = F().create();
			}
			return *this->m_parsed_object;
		}

		/*
		 * \brief Const version of get_valid() - uses '=' operator for assigning the return value
		 *
		 */
		virtual const A& get_valid(A& v) const
		{
			if( m_parsed_object )
				return v = *m_parsed_object;
			else
				return v;
		}

		/*
		 * \brief Using this, the parser can be reused.
		 *
		 */
		virtual void invalidate() 
		{ 
			if( this->is_valid() )
			{
				delete this->m_parsed_object;
			}
			this->m_parsed_object = NULL;
		}

		/*
		 * \brief release ownership of parsed object and return it
		 *
		 */
		A* release() { A* tmp = this->m_parsed_object; this->m_parsed_object = NULL; return tmp; }

	protected:
		A*				m_parsed_object;

		/*
		 * \brief The method additionally validates the instance of A
		 *
		 */
		virtual void prepare_for_parsing()
		{
			basic_parser<E>::prepare_for_parsing();
			this->get_valid();
		}

		/*
		 * \brief The method additionally validates the instance of A
		 *
		 */
		virtual void prepare_for_formatting() 
		{
			basic_parser<E>::prepare_for_formatting();
			this->get_valid();
		};
};

	template< typename E, typename A, typename F >
	basic_object_parser<E,A,F>::basic_object_parser( const char* name, symbol_type type ) 
: basic_parser<E>(name,type), m_parsed_object(NULL) 
{
}

	template< typename E, typename A, typename F >
	basic_object_parser<E,A,F>::basic_object_parser( const char* name, const A &rhs, symbol_type type )
: basic_parser<E>(name,type), m_parsed_object( new A(rhs) ) 
{
}

/* 
 * \brief the conversion from const to non const - if someone wants a copy of A object, should use previous constructor
 *
 */
	template< typename E, typename A, typename F >
	basic_object_parser<E,A,F>::basic_object_parser(const basic_object_parser& rhs )
: basic_parser<E>(rhs), m_parsed_object(const_cast<basic_object_parser*>(&rhs)->release())
{
}

	template< typename E, typename A, typename F >
basic_object_parser<E,A,F>::~basic_object_parser() 
{
	if( this->is_valid() ) delete this->m_parsed_object;
}


/*
 * \brief The non_terminal - always defined as set of symbols in defined order. The
 * parsing succeedes only if all of symbols are parsed. The parse method 
 * uses simple parsing algorithm for parsing language defined by non-left-recursive grammar. 
 *
 */
template<typename E, typename A, typename F=default_factory<A> >
class basic_non_terminal : public basic_object_parser<E,A,F>
{
	public:	
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;

		//! Default constructor
		basic_non_terminal();
		//! Constructor taking compound object 
		basic_non_terminal( const A& obj );
		//! Copy constructor
		basic_non_terminal( const basic_non_terminal& rhs );
		//! The destructor
		~basic_non_terminal();

		/*
		 * \brief This parse method is taking symbols from the begin of buffer to the end, 
		 * doesn't require that the whole input is parsed. The input will be copied into 
		 * parsed objects manipulated by \sa basic_object_parser, which inherits from auto_ptr.
		 *
		 */
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );

		/*
		 * \brief This parse method is taking symbols from the begin of istream to the end, 
		 * doesn't require that the whole input is parsed The input will be copied into parsed objects 
		 * manipulated by \sa basic_object_parser. 
		 *
		 */
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );

		/*
		 * \brief Method formats all previously parsed or assigned data back into output stream.
		 * This is common format method for any composed parsers - is_formatted returns true 
		 * if all valid composed objects were formatted succesfully
		 *
		 */
		virtual std::basic_ostream<E>& format( std::basic_ostream<E>& os );

		/*
		 * \brief Method removes all referenced symbols, as after deleting the parsed object all symbols become invalid
		 *
		 */
		void invalidate() 
		{ 
			this->m_symbols.clear();
			this->m_symbols_valid = false;
			basic_object_parser<E,A,F>::invalidate();
		}

		const char* get_name() const 
		{
			std::stringstream ss;
			A a;
			ss << NONTERMINAL << "<" << a.get_name() << ">";
			return ss.str().c_str();
		}

	protected:
		/*
		 * \brief The list of symbols defining the non terminal - symbols and their order 
		 * is defined by parsed object manipulated by \sa object_parser
		 *
		 */
		parser_list	m_symbols;

		/*
		 * \brief Flag identifying whether embedded symbols were read from parsed object
		 *
		 */
		bool m_symbols_valid;

		//! Obtains the list of symbols embedded in parsed object
		virtual void prepare_symbols() 
		{ 
			if( !this->m_symbols_valid ) 
			{
				this->get_valid().push_parsers( this->m_symbols );
				this->m_symbols_valid = true;
			}
		}

		//! Obtains symbols from parsed object
		void prepare_for_parsing()
		{
			basic_object_parser<E,A,F>::prepare_for_parsing();
			prepare_symbols();
			parser_list_iterator it = this->m_symbols.begin();

		}

		//! Obtains symbols from formatted object 
		void prepare_for_formatting()
		{
			basic_object_parser<E,A,F>::prepare_for_formatting();
			prepare_symbols();
		}
};

/*
 * \brief Tries to parse all sub-parsers in random order. If no of sub-parser succeeds, 
 * the subset::parse method returns 0, succeeds anyway, is_parsed() will return true.
 *
 */
template <typename E,typename A>
class basic_subset : public basic_non_terminal<E, A>
{
	public:
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;
		basic_subset();
		~basic_subset();
		virtual unsigned long parse( const E *buf, const unsigned long buf_lenght );
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );
};

	template< typename E, typename A >
	basic_subset<E,A>::basic_subset()
: basic_non_terminal<E,A>()
{
}

	template< typename E, typename A >
basic_subset<E,A>::~basic_subset()
{
}

	template< typename E, typename A >
unsigned long basic_subset<E,A>::parse( const E *buf, const unsigned long buf_length )
{
	this->prepare_for_parsing();

	bool is_parsed = true;
	parser_list symbols;
	parser_list_iterator parse_iterator = this->m_symbols.begin();

	// copy symbols to temporary list seems to be best to me
	while( parse_iterator != this->m_symbols.end() ) 
		symbols.push_back( *(parse_iterator++) );
	while( !symbols.empty() && is_parsed )
	{
		parse_iterator = symbols.begin();
		is_parsed = false;
		while( parse_iterator != symbols.end() && !is_parsed )
		{
			(*parse_iterator)->change_strategy( this->m_strategy );
			(*parse_iterator)->parse( buf+this->m_parsed_size, 
					buf_length-this->m_parsed_size );
			if( is_parsed = (*parse_iterator)->is_parsed() ) 
			{
				this->m_parsed_size += (*parse_iterator)->parsed_size();
				// exclude from parsing
				parse_iterator = symbols.erase(parse_iterator);
			}
			else parse_iterator++; // continue with next
		}
	}
	for( parse_iterator = symbols.begin(); 
			parse_iterator != symbols.end(); 
			parse_iterator++ )
	{
		(*parse_iterator)->change_parsed_flag(false);
	}

	this->m_is_parsed = true; // always parsed

	return this->m_parsed_size;
}

	template <typename E, typename A >
std::basic_istream<E>& basic_subset<E,A>::parse( std::basic_istream<E>& is )
{
	this->prepare_for_parsing();

	bool is_parsed = true;
	parser_list symbols;
	parser_list_iterator parse_iterator = this->m_symbols.begin();

	// copy symbols to temporary list seems to be best to me
	while( parse_iterator != this->m_symbols.end() ) 
		symbols.push_back( *(parse_iterator++) );
	while( !symbols.empty() && is_parsed )
	{
		parse_iterator = symbols.begin();
		is_parsed = false;
		while( parse_iterator != symbols.end() && !is_parsed && is.good() )
		{
			(*parse_iterator)->change_strategy( this->m_strategy );
			(*parse_iterator)->parse( is );
			if( is_parsed = (*parse_iterator)->is_parsed() ) 
			{
				this->m_parsed_size += (*parse_iterator)->parsed_size();
				// exclude from parsing
				parse_iterator = symbols.erase(parse_iterator);
			}
			else parse_iterator++; // continue with next
		}
	}
	for( parse_iterator = symbols.begin(); 
			parse_iterator != symbols.end(); 
			parse_iterator++ )
	{
		(*parse_iterator)->change_parsed_flag(false);
	}

	this->m_is_parsed = true; // always parsed

	return is;
}

/*
 * \brief The class is a specialization of \ref basic_subset for char input
 *
 */
template <typename A>
class subset : public basic_subset<char, A>
{
	public:
		subset() : basic_subset<char,A>() {};
		~subset() {};
};

/*
 * \brief The class is a specialization of \ref basic_subset for wchar_t input
 *
 */
template <typename A>
class wsubset : public basic_subset<wchar_t, A>
{
	public:
		wsubset() : basic_subset<wchar_t,A>() {};
		~wsubset() {};
};

const char* ASCII_RANGE_TERMINAL = "ascii-range-terminal";

/*
 * \brief Used to recognize input item from specified range, range is specified by template parameters A and B
 *
 */
template<typename E, E A, E B, typename F=default_factory<E> >
class basic_ascii_range_terminal : public basic_object_parser<E,E,F>
{
	public:
		basic_ascii_range_terminal() :
			basic_object_parser<E,E,F>(ASCII_RANGE_TERMINAL) {};

		~basic_ascii_range_terminal() {};

		/*
		 * \brief The input is succesfully parsed, if first item in buffer is within the range specified in template parameters range from A to B
		 * \param[in]	buf	Buffer to be parsed 
		 * \param[in]	buf_length	Length of the buffer
		 * \return The lenght of the buffer part, which was parsed (==1), 0 if parse failed.
		 */
		unsigned long parse( const E *buf, const unsigned long buf_length );
		std::basic_istream<E>& parse( std::basic_istream<E>& is );

		/*
		 * \brief Formats the previously parsed or stored value into output stream
		 *
		 */
		std::basic_ostream<E>& format( std::basic_ostream<E>& os );

		const char* get_name() const
		{ 
			std::stringstream ss;
			ss << ASCII_RANGE_TERMINAL << "<" << A << "," << B << ">";
			return ss.str().c_str();
		}		
};

class trace_context
{
	public:
		enum trace_level{
			debug = 0, 
			debug1 = 1,
			debug2 = 2,
			debug3 = 3,
			info = 4,
			important = 5,
			warning = 6,
			error = 7,
			exception = 8
		};

		static trace_context& get()
		{
			static trace_context t;
			return t;
		}

		int inc_scope() { return m_scope++; }
		int dec_scope() { return --m_scope; }
		int get_scope() { return m_scope; }
		bool to_trace() { return m_to_trace; }

		void trace_on() { m_to_trace = true; }
		void trace_off() { m_to_trace = false; }
		void set_max_scope( int max_scope ) { m_max_scope = max_scope; }
		void set_trace_level( trace_level new_level ) { m_trace_level = new_level; }
		int get_trace_level() { return m_trace_level; }
		int get_max_scope() { return m_max_scope; }

	protected:
		trace_context() : m_scope(0), m_max_scope(0), m_to_trace(false), m_trace_level( (int)info ) {};
		int				m_scope;
		int				m_max_scope;
		bool			m_to_trace;
		int				m_trace_level;

};

template< typename TracedObject > class tracer
{	

	public:
		tracer(const TracedObject& traced_object,
				const char* method,
				const char* source_file, 
				long source_line = 0 ) : 
			m_traced_object(traced_object),
			m_method(method), 
			m_source_file(source_file), 
			m_source_line(source_line),
			m_trace_level((int)trace_context::info),
			m_is_intended(false)
	{
		if( is_trace_on() )
		{
			int scope = trace_context::get().inc_scope();
			if( to_trace() )
			{
				intend( scope );
				std::cout << ">> " << traced_object.get_name() << "::";
				std::cout << m_method << "()  " << m_source_file << "(" << m_source_line << ")" << std::endl;
				m_is_intended = false;
			}
		}
	}

		bool is_trace_on() 
		{
			return trace_context::get().to_trace() && (int)m_trace_level <= (int)trace_context::get().get_trace_level();
		}


		bool to_trace() 
		{ 
			return trace_context::get().to_trace() && 
				(int)m_trace_level <= (int)trace_context::get().get_trace_level() &&
				trace_context::get().get_scope() <= trace_context::get().get_max_scope();
		}

		friend inline tracer& operator << (tracer& a_tracer, const char* str)
		{	
			if( a_tracer.to_trace() )
			{ 
				a_tracer.intend( trace_context::get().get_scope() );
				std::cout << str;	
			}
			return a_tracer;
		}

		friend inline tracer& operator << (tracer& a_tracer, trace_context::trace_level l)
		{	
			a_tracer.set_trace_level( (int)l );
			return a_tracer;
		}

		friend inline tracer& operator << (tracer& a_tracer, int i)
		{	
			if( a_tracer.to_trace() )
			{ 
				a_tracer.intend( trace_context::get().get_scope() );
				std::cout << i;	
			}
			return a_tracer;
		}

		friend inline tracer& operator << (tracer& a_tracer, unsigned int i)
		{	
			if( a_tracer.to_trace() )
			{ 
				a_tracer.intend( trace_context::get().get_scope() );
				std::cout << i;	
			}
			return a_tracer;
		}

		friend inline tracer& operator << (tracer& a_tracer, unsigned long i)
		{	
			if( a_tracer.to_trace() )
			{ 
				a_tracer.intend( trace_context::get().get_scope() );
				std::cout << i;	
			}
			return a_tracer;
		}

		friend inline tracer& operator << (tracer& a_tracer, long i)
		{	
			if( a_tracer.to_trace() )
			{ 
				a_tracer.intend( trace_context::get().get_scope() );
				std::cout << i;	
			}
			return a_tracer;
		}

		friend inline tracer& operator << (tracer& a_tracer, char c )
		{
			if( a_tracer.to_trace() )
			{ 
				a_tracer.intend( trace_context::get().get_scope() );
				std::cout << c;	
			}
			return a_tracer;
		}

		friend inline tracer& operator << (tracer& a_tracer, wchar_t c )
		{
			if( a_tracer.to_trace() )
			{ 
				a_tracer.intend( trace_context::get().get_scope() );
				std::cout << c;	
			}
			return a_tracer;
		}

		inline tracer<TracedObject>& operator<<(tracer<TracedObject>& (*__pf)(tracer<TracedObject>&))
		{
			return __pf(*this);
		}

		tracer<TracedObject>& endl(tracer<TracedObject>& a_tracer)
		{
			if( a_tracer.to_trace() ) 
			{
				std::cout << std::endl;
				a_tracer.set_trace_level( (int)trace_context::info );
				a_tracer.reset_intendation();
			}
			return a_tracer;
		}

		/*	
			friend inline tracer& operator << (tracer& a_tracer, const tracer::endl& e)
			{
			if( a_tracer.to_trace() ) 
			{
			std::cout << std::endl;
			a_tracer.set_trace_level( (int)trace_context::info );
			a_tracer.reset_intendation();
			}
			return a_tracer;
			}		
			*/

		virtual ~tracer()
		{
			if( is_trace_on() )
			{
				int scope = trace_context::get().dec_scope();
				if( to_trace() )
				{
					intend( scope );
					std::cout << "<< " << m_traced_object.get_name();
					std::cout << "::" << m_method << "()" << std::endl;
					m_is_intended = false;
				}
			}
		}

	protected:
		void set_trace_level( int new_level ) 
		{
			m_trace_level = new_level; 
		}

		void reset_intendation() 
		{
			m_is_intended = false; 
		}

	private:
		const TracedObject& 		m_traced_object;
		const char* 				m_method;
		const char* 				m_source_file;
		const int					m_source_line;
		int							m_trace_level;
		bool						m_is_intended;

		void intend( int scope )
		{
			if( !m_is_intended )
			{
				std::cout << scope;
				for( int i = 0; i < scope; i++ ) 
				{
					std::cout << "    "; 
				}
				m_is_intended = true;
			}
		}

};

template<typename TracedObject> tracer<TracedObject>& endl(tracer<TracedObject>& a_tracer)
{
	return a_tracer.endl(a_tracer);
}


	template< typename E, E A, E B, typename F >
unsigned long basic_ascii_range_terminal<E,A,B,F>::parse( const E *buf, const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_ascii_range_terminal<E,A,B,F> > t(*this, "parse", __FILE__, __LINE__);
	t << "input=";
	t << buf[0];
	t << ", buf_length=";
	t << (int)buf_length;
	t << endl;
#endif
	this->prepare_for_parsing();

	if( (*buf) >= A && (*buf) <= B )
	{
#ifdef TRACE
		t << "----parse succeeded" << endl;
#endif
		this->get_valid() = (*buf);
		this->m_parsed_size = 1;
		this->m_is_parsed = true;
	}
	else
	{
#ifdef TRACE
		t << "parse failed" << endl;
#endif
	}		
	return this->m_parsed_size;
}

	template< typename E, E A, E B, typename F >
std::basic_istream<E>& basic_ascii_range_terminal<E,A,B,F>::parse( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_ascii_range_terminal<E,A,B,F> > t(*this, "parse", __FILE__, __LINE__);
#endif	
	this->prepare_for_parsing();
	if( !is.good() )
	{
		return is;
	}
	E input = A;
	std::streampos str_pos = is.tellg();

	is.read( &input, 1 );

#ifdef TRACE
	t << "input=" << input << endl;
#endif

	if( is.good() && input >= A && input <= B )
	{
#ifdef TRACE
		t << "----parser succeeded" << endl;
#endif
		this->m_parsed_size = 1;
		this->m_is_parsed = true;
		this->get_valid() = input;
	}
	else 
	{
#ifdef TRACE
		t << "parser failed" << endl;
#endif
		is.clear();
		is.seekg( str_pos );
	}
	return is;
}

	template< typename E, E A, E B, typename F >
std::basic_ostream<E>& basic_ascii_range_terminal<E,A,B,F>::format( std::basic_ostream<E>& os )
{
	this->prepare_for_formatting();

	std::streampos strPos = os.tellp();
	if( (std::streamoff)strPos < 0 ) strPos = 0;

	int out = this->get_valid();
	if( os.good() ) 
	{
		os.write( (const E*)&out,sizeof(E) );
		this->m_formatted_size = sizeof(E);
		this->m_is_formatted = true;
	}
	else 
	{
		os.clear();
		os.seekp( strPos );
	}
	return os;
}

/*
 * \brief The class is a specialization of \ref basic_ascii_range_terminal for char input
 *
 */
template< char A, char B, typename F=default_factory<char> >
class ascii_range_terminal : public basic_ascii_range_terminal<char,A,B,F>
{
	public:
		ascii_range_terminal() : basic_ascii_range_terminal<char,A,B,F>() 
	{
	}

		virtual ~ascii_range_terminal() 
		{
		}
};

/*
 * \brief The class is a specialization of \ref basic_ascii_range_terminal for wchar_t input type
 *
 */
template< wchar_t A, wchar_t B, typename F=default_factory<wchar_t> >
class wascii_range_terminal : public basic_ascii_range_terminal<wchar_t,A,B,F>
{
	public:
		wascii_range_terminal() : basic_ascii_range_terminal<wchar_t,A,B,F>() 
	{
	}

		virtual ~wascii_range_terminal() 
		{
		}

};

/*
 * \brief Choice as special case of nonterminal_type - parsed if one of symbols is parsed.
 *
 */
template <typename E, typename A, typename F=default_factory<A> >
class basic_choice : 
	public basic_non_terminal<E,A,F>
{
	public:
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;

		//! Default constructor
		basic_choice();

		//! Copy constructor
		basic_choice( const basic_choice& rhs );

		//! Constructor taking a compound object
		basic_choice( const A& obj );


		//! Destructor
		virtual ~basic_choice();

		/*
		 * \brief The method tries to parse any of the symbols, stops when finds the first successive parser
		 * \param *buf Buffer containing the word to be parsed
		 * \param buf_length The length of word, size of buffer
		 * \return The length of succesfully parsed word 
		 *
		 */
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );

		/*
		 * \brief The method tries to parse any of the symbols, stops when finds the first successive parser
		 *
		 */
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );	

		const char* get_name() const
		{
			std::stringstream ss;
			A a;
			ss << CHOICE << "<" << a.get_name() << ">";
			return ss.str().c_str();
		}
};

/*
 * \brief Parser usable when the set of acceptable terminals is much bigger then 
 * set of terminals which need to be excepted, in this case it is posible 
 * to define choice of unacceptable terminals (except_choice) and 
 * \ref parse method will success if the input word is not recognized 
 * by except_choice. For example it is necessary to recognize any character
 * except new line and line feed. The except_choice will be choice of new line 
 * and line feed then.
 */
template< typename E, typename except_choice >
class basic_except_terminal : public basic_object_parser<E,E>
{
	public:
		basic_except_terminal() : basic_object_parser<E,E>("ExceptTerminal") {};

		/*
		 * \brief The method succeedes if input word is not recognized by except_choice.
		 */
		unsigned long parse( const E *buf, const unsigned long buf_length )
		{
			this->prepare_for_parsing();

			basic_choice<E,except_choice>	except;
			except.parse( buf, buf_length );
			if( !except.is_parsed() && buf_length > 0 )
			{
				this->m_parsed_object = new E(buf[0]);
				this->m_parsed_size = 1;
				this->m_is_parsed = true;
			}
			return this->m_parsed_size;
		}

		/*
		 * \brief The method succeedes if input word is not recognized by except_choice.
		 *
		 */
		std::basic_istream<E>& parse( std::basic_istream<E>& is )
		{
			this->prepare_for_parsing();

			basic_choice<E,except_choice>	except;
			std::streampos str_pos = is.tellg();
			E input = 0;

			is.read( &input, 1 );
			if( is.good() )
			{
				except.parse( &input, 1 );
				if( !except.is_parsed() )
				{
					this->m_parsed_object = new E(input);
					this->m_parsed_size = 1;
					this->m_is_parsed = true;
				}
				else
				{
					is.clear();
					is.seekg( str_pos );
				}
			}
			return is;
		}


		/*
		 * \brief Formats the previously parsed or stored value into output stream.
		 *
		 */
		std::basic_ostream<E>& format( std::basic_ostream<E>& os )
		{
			this->prepare_for_formatting();

			std::streampos strPos = os.tellp();
			if( (std::streamoff)strPos < 0 ) strPos = 0;

			E out = this->get_valid();
			if( os.good() ) 
			{
				os.write( (const E*)&out,sizeof(E) );
				this->m_formatted_size = sizeof(E);
				this->m_is_formatted = true;
			}
			else 
			{
				os.clear();
				os.seekp( strPos );
			}
			return os;
		}

};

/*
 * \brief The class is a specialization of \ref basic_except_terminal for char input
 *
 */
template< class except_choice >
class except_terminal : public basic_except_terminal<char,except_choice>
{
};

/*
 * \brief The class is a specialization of \ref basic_except_terminal for wchar_t input
 *
 */
template< class except_choice >
class wexcept_terminal : public basic_except_terminal<wchar_t,except_choice>
{
};


/*
 * \brief Accepts defined terms in random order.
 *
 */
template <typename E, typename A>
class basic_set : public basic_non_terminal<E, A>
{
	public:
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;
		basic_set();
		virtual ~basic_set();
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );
};

	template< typename E, typename A >
basic_set<E,A>::basic_set() : basic_non_terminal<E,A>()
{
}


	template< typename E, typename A >
unsigned long basic_set<E,A>::parse( const E *buf, const unsigned long buf_length )
{
	this->prepare_for_parsing();

	parser_list symbols;
	parser_list_iterator parse_iterator = this->m_symbols.begin();
	bool is_parsed = true;

	// copy symbols to temporary list seems to be best to me
	while( parse_iterator != this->m_symbols.end() ) symbols.push_back( *(parse_iterator++) );
	while( !symbols.empty() && is_parsed )
	{
		parse_iterator = symbols.begin();
		is_parsed = false;
		while( parse_iterator != symbols.end() && !is_parsed && this->m_parsed_size <= buf_length )
		{
			(*parse_iterator)->parse( buf+this->m_parsed_size, buf_length-this->m_parsed_size );
			if( is_parsed = (*parse_iterator)->is_parsed() )
			{
				this->m_parsed_size += (*parse_iterator)->parsed_size();
				parse_iterator = symbols.erase(parse_iterator); // exclude from parsing
			}
			else parse_iterator++; // continue with next
		}
	}
	if( !symbols.empty() ) this->m_parsed_size = 0;
	else this->m_is_parsed = true;

	return this->m_parsed_size;
}


	template <typename E, typename A >
std::basic_istream<E>& basic_set<E,A>::parse( std::basic_istream<E>& is )
{
	this->prepare_for_parsing();

	parser_list symbols;
	parser_list_iterator parse_iterator = this->m_symbols.begin();
	std::streampos str_pos = is.tellg();
	bool is_parsed = true;

	// copy symbols to temporary list seems to be best to me
	while( parse_iterator != this->m_symbols.end() ) symbols.push_back( *(parse_iterator++) );
	while( !symbols.empty() && is_parsed )
	{
		parse_iterator = symbols.begin();
		is_parsed = false;
		while( parse_iterator != symbols.end() && !is_parsed && is.good() )
		{
			(*parse_iterator)->parse( is );
			if( is_parsed = (*parse_iterator)->is_parsed() )
			{
				this->m_parsed_size += (*parse_iterator)->parsed_size();
				parse_iterator = symbols.erase(parse_iterator); // exclude from parsing
			}
			else parse_iterator++; // continue with next
		}
	}
	if( !symbols.empty() )
	{
		this->m_parsed_size = 0;
		is.clear();
		is.seekg( str_pos );
	}
	else this->m_is_parsed = true;

	return is;
}

	template <typename E, typename A >
basic_set<E,A>::~basic_set()
{
}

/*
 * \brief The class is a specialization of \ref basic_set for char input
 *
 */
template <typename A>
class set : public basic_set<char, A>
{
	public:
		set() : basic_set<char,A>() {};
		~set() {};
};

/*
 * \brief The class is a specialization of \ref basic_set for wchar_t input
 *
 */
template <typename A>
class wset : public basic_set<wchar_t, A>
{
	public:
		wset() : basic_set<wchar_t,A>() {};
		~wset() {};
};

const char *REPEATED_SEQUENCE = "repeated-sequence";

/*
 * \brief Represents (possibly empty) sequence of subsequences, each subsequence
 * is parsed by P - template parameter P is a parser of subsequence.
 * TODO - provide example
 *
 */
template<typename E, 
	typename P, 
	typename F=default_factory<std::list<P> > >
	class basic_repeated_sequence : public basic_object_parser<E,std::list<P>, F >
{
	public:
		typedef std::list<P> list_P;
		typedef typename list_P::iterator list_P_iterator;
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;

		//! Default constructor
		basic_repeated_sequence() :	basic_object_parser<E,list_P,F> ( REPEATED_SEQUENCE, basic_parser<E>::non_terminal_type ) 
	{
	}

		//! Copy constructor
		basic_repeated_sequence(const basic_repeated_sequence& rhs ) :
			basic_object_parser<E,list_P,F>(rhs)
	{
	}

		~basic_repeated_sequence() 
		{
		}

		/*
		 * \brief The parse method tries to parse the subsequence as may times as possible
		 *
		 */
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );

		/*
		 * \brief The parse method tries to parse the subsequence as may times as possible
		 *
		 */
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );

		/*
		 * \brief Calls format of each subsequence's parser
		 *
		 */
		virtual std::basic_ostream<E>& format( std::basic_ostream<E>& os );

		const char* get_name() const 
		{
			std::stringstream ss;
			ss << REPEATED_SEQUENCE << "<" << P().get_name() << ">";
			return ss.str().c_str();
		}

		/*
		 * \brief Use this method to return list of objects parsed by parser (parser type 
		 * is defined by template parameter P, for example \ref non_terminal<R> )
		 *
		 */
		template< class R > void get_list( std::list<R>& l )
		{
			list_P_iterator it = this->get_valid().begin();
			while( it != this->get_valid().end() )
			{
				l.push_back( (*it).get_valid() );
				it++;
			}
		}	

	protected:
		/*
		 * \brief The method additionally prepares (clears) the lsit of parsers 
		 *
		 */
		virtual void prepare_for_parsing()
		{
			basic_object_parser<E,list_P,F>::prepare_for_parsing();
			this->get_valid().clear();
		}

};

/*
 * \brief The class is a specialization of \ref basic_repeated_sequence for char input
 *
 */
template<typename P,
	typename F=default_factory< std::list<P> > >
	class repeated_sequence : public basic_repeated_sequence<char,P,F>
{
	public:
		repeated_sequence() : basic_repeated_sequence<char,P,F>() {};
		repeated_sequence( const repeated_sequence& rhs) :
			basic_repeated_sequence<char,P,F>(rhs) {};
};

/*
 * \brief The class is a specialization of \ref basic_repeated_sequence for wchar_t input
 *
 */
template<typename P, 
	typename F=default_factory< std::list<P> > >
	class wrepeated_sequence : public basic_repeated_sequence<wchar_t,P,F>
{
	public:
		wrepeated_sequence() : basic_repeated_sequence<wchar_t,P,F>() {};
		wrepeated_sequence( const wrepeated_sequence& rhs) :
			basic_repeated_sequence<char,P,F>(rhs) {};
};

	template< typename E, typename P, typename F >
std::basic_istream<E>& basic_repeated_sequence<E,P,F>::parse( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_repeated_sequence<E,P,F> > 
		t(*this, "parse", __FILE__, __LINE__);
#endif

	this->prepare_for_parsing();
	std::streampos stream_pos = is.tellg();
	if( (std::streamoff)stream_pos < 0 ) stream_pos = 0;

	this->m_is_parsed = true;
	P p;
	while( this->m_is_parsed )
	{
		std::streampos stream_pos = is.tellg();
		if( (std::streamoff)stream_pos < 0 ) stream_pos = 0;
		p.parse(is);
		if( !p.is_parsed() ) 
		{
			is.clear();
			is.seekg( stream_pos );
			break;
		}
		this->m_parsed_size += p.parsed_size();
		this->get_valid().push_back(p);
	}

#ifdef TRACE
	// always parsed (possible empty sequence is accepted
	t << "parser succeeded " << endl;
#endif
	return is;
}

	template<typename E, typename P, typename F >
unsigned long basic_repeated_sequence<E,P,F>::parse( const E *buf, const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_repeated_sequence<E,P,F> >
		t(*this, "parse", __FILE__, __LINE__);
#endif
	prepare_for_parsing();
	this->m_is_parsed = true;
	P p;

	while( this->m_is_parsed )
	{
		p.parse(buf+this->m_parsed_size, buf_length-this->m_parsed_size);
		if( !p.is_parsed() ) 
		{
			break;
		}
		this->m_parsed_size += p.parsed_size();
		this->get_valid().push_back(p);
	}

#ifdef TRACE
	// always parsed (possible empty sequence is accepted
	t << "parser succeeded " << endl;
#endif
	return this->m_parsed_size;
}

	template<typename E, typename P, typename F >
std::basic_ostream<E>& basic_repeated_sequence<E,P,F>::format( std::basic_ostream<E>& os )
{
	this->prepare_for_formatting();
	list_P_iterator format_iterator = this->get_valid().begin();
	bool is_formatted = true;
	std::streampos streamPos = os.tellp();
	if( (std::streamoff)streamPos < 0 ) streamPos = 0;

	while(	format_iterator != this->get_valid().end() &&
			!os.bad() && 
			is_formatted )
	{
		(*format_iterator).format(os);
		this->m_formatted_size += (*format_iterator).formatted_size();
		is_formatted = (*format_iterator).is_formatted();
		format_iterator++;
	}
	return os;
}

/*
 * \brief The class is a specialization of \ref basic_non_terminal for char input
 *
 */
template<typename A, typename F=default_factory<A> >
class non_terminal : public basic_non_terminal<char,A,F>
{
	public:
		non_terminal() : basic_non_terminal<char,A,F>() 
	{
	}

		non_terminal( const A& obj ) :
			basic_non_terminal<char,A,F>(obj) 
	{
	}

		non_terminal( const non_terminal& rhs ) : 
			basic_non_terminal<char,A,F>(rhs) 
	{
	}

		virtual ~non_terminal() 
		{
		}
};

/*
 * \brief The class is a specialization of \ref basic_non_terminal for wchar_t input
 *
 */
template<typename A,typename F=default_factory<A> >
class wnon_terminal : public basic_non_terminal<wchar_t,A,F>
{
	public:
		wnon_terminal() : basic_non_terminal<wchar_t,A,F>() 
	{
	}

		wnon_terminal( const A& obj ) :
			basic_non_terminal<wchar_t,A,F>(obj) 
	{
	}

		wnon_terminal( const wnon_terminal& rhs ) : 
			basic_non_terminal<wchar_t,A,F>(rhs) 
	{
	}

		virtual ~wnon_terminal() 
		{
		}

};


	template< typename E, typename A, typename F >
	basic_non_terminal<E,A,F>::basic_non_terminal()
: basic_object_parser<E,A,F>(NONTERMINAL,basic_parser<E>::non_terminal_type),
	m_symbols_valid(false)
{
}

	template< typename E, typename A, typename F >
	basic_non_terminal<E,A,F>::basic_non_terminal( const A& obj  )
:	basic_object_parser<E,A,F>(NONTERMINAL,obj, basic_parser<E>::non_terminal_type ), 
	m_symbols_valid(false)
{
};

template< typename E, typename A, typename F >
basic_non_terminal<E,A,F>::basic_non_terminal(
		const basic_non_terminal& rhs )
:	basic_object_parser<E,A,F>(rhs),
	m_symbols_valid(false)
{
};

	template< typename E, typename A, typename F >
basic_non_terminal<E,A,F>::~basic_non_terminal()
{
}

template< typename E, typename A, typename F >
	std::basic_istream<E>& basic_non_terminal<E,A,F>
::parse( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_non_terminal<E,A,F> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();
	parser_list_iterator parse_iterator = this->m_symbols.begin();

	bool is_parsed = true;
	std::streampos stream_pos = is.tellg();
	if( (std::streamoff)stream_pos < 0 ) stream_pos = 0;

	while(	parse_iterator != this->m_symbols.end() &&
			!is.bad() && 
			is_parsed )
	{
		(*parse_iterator)->parse( is );
		this->m_parsed_size += (*parse_iterator)->parsed_size();
		is_parsed = (*parse_iterator)->is_parsed();
		parse_iterator++;
	}
	// reset the starting os buf possition in case of error
	if( parse_iterator != this->m_symbols.end() || !is_parsed)
	{
		is.clear();
		is.seekg( stream_pos );
		this->m_parsed_size = 0;
#ifdef TRACE
		t << "parser failed" << endl;
#endif
	}
	else 
	{
		this->m_is_parsed = true;
#ifdef TRACE
		t << "---parser succeeded" << endl;
#endif
	}		
	return is;
}

template< typename E, typename A, typename F >
	std::basic_ostream<E>& basic_non_terminal<E,A,F>
::format( std::basic_ostream<E>& os )
{
	this->prepare_for_formatting();
	parser_list_iterator format_iterator = this->m_symbols.begin();

	bool is_formatted = true;
	std::streampos streamPos = os.tellp();
	if( (std::streamoff)streamPos < 0 ) streamPos = 0;

	while(	format_iterator != this->m_symbols.end() &&
			!os.bad() && 
			is_formatted )
	{
		if( (*format_iterator)->is_valid() )
		{
			(*format_iterator)->format( os );
			this->m_formatted_size += (*format_iterator)->formatted_size();
			is_formatted = (*format_iterator)->is_formatted();
		}
		format_iterator++;
	}
	// reset the starting os buf possition in case of error
	if( format_iterator != this->m_symbols.end() || !is_formatted)
	{
		os.clear();
		os.seekp( streamPos );
		this->m_formatted_size = 0;
	}
	else this->m_is_formatted = true;
	return os;
}

template< typename E, typename A, typename F >
	unsigned long basic_non_terminal<E,A,F>
::parse( const E *buf, const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_non_terminal<E,A,F> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();
	parser_list_iterator parse_iterator = this->m_symbols.begin();

	bool is_parsed = true;

	while(	parse_iterator != this->m_symbols.end() && is_parsed )
	{
		(*parse_iterator)->parse( buf+this->m_parsed_size, buf_length-this->m_parsed_size );
		this->m_parsed_size += (*parse_iterator)->parsed_size();
		is_parsed = (*parse_iterator)->is_parsed();
		parse_iterator++;
	}
	// reset the starting os buf possition in case of error
	if( !is_parsed )
	{
#ifdef TRACE
		t << "parser failed" << endl;
#endif
		this->m_parsed_size = 0;
	}
	else
	{
#ifdef TRACE
		t << "----parser succeeded" << endl;
#endif
		this->m_is_parsed = true;
	}
	return this->m_parsed_size;
}

/*
 * \brief Base class for terminal parsers using std::iostream functionality. The 
 * \ref format method is common for inherited classes, see \ref basic_value_parser 
 * and \ref basic_pattern_parser.
 */
template< typename E, typename A >
class basic_default_type_parser : public basic_object_parser<E,A>
{
	public:
		basic_default_type_parser( const char* name = DEFAULT_TYPE_PARSER );
		basic_default_type_parser( const basic_default_type_parser& rhs );
		basic_default_type_parser( const A& obj, const char* name = DEFAULT_TYPE_PARSER );

		virtual ~basic_default_type_parser();

		/*
		 * \brief Uses basic_ostream::operator << to put compound object into output.
		 *
		 */
		virtual std::basic_ostream<E>& format( std::basic_ostream<E>& os );
};

/*
 * \brief Parse of pattern defined as string in constructor
 *
 */
template< typename E >
class basic_pattern_parser : 
	public basic_default_type_parser< E, std::basic_string<E> >
{
	public:
		typedef std::basic_string<E>	string_type;
		//! Default constructor
		basic_pattern_parser() : 
			basic_default_type_parser< E, string_type >(PATTERN_PARSER) {};
		/*
		 * \brief Constructor with definition of pattern as string parameter
		 * \param pattern string pattern to be searched in input buffer during parsing
		 * \param comparison_type specifies how string will be compared with input buffer
		 *
		 */
		basic_pattern_parser( const std::basic_string<E>& pattern, 
				int comparison_type = IGNORE_CASE | IGNORE_SPACE ) :
			basic_default_type_parser< E, string_type >(pattern,PATTERN_PARSER),
			comparison_type_(comparison_type) 
	{
	}

		basic_pattern_parser( const char *pattern, 
				int comparison_type = IGNORE_CASE | IGNORE_SPACE ) :
			basic_default_type_parser< E, string_type >(widen(pattern),PATTERN_PARSER),
			comparison_type_(comparison_type) 
	{
	}

		//! Destructor
		virtual ~basic_pattern_parser() 
		{
		}

		/*
		 * \brief The method compares the pattern, passed as constructor parameter, with string stored in input buffer.
		 * \param *buf the buffer containing input string 
		 * \param buf_length the length of buffer
		 * \return the size of parsed string, if succesfullt, otherwise zero
		 *
		 */
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );

		/*
		 * \brief The method succeedes if input is same as pattern passed to constructor
		 *
		 */
		std::basic_istream<E>& parse( std::basic_istream<E>& is );

		std::basic_string<E> widen( const char* pattern )
		{
			// TODO - use constructor with input of pattern, as it must be stored 
			// the while pattern including spaces
			std::basic_stringstream<E> ss;
			ss << pattern;
			return ss.str();

		}

		const char* get_name() const 
		{
			std::stringstream ss;
			std::basic_string<E> s;
			ss << PATTERN_PARSER << "<" << this->get_valid(s).c_str() << ">";
			return ss.str().c_str();
		}

	protected:
		/*
		 * \brief Defines how pattern string will be compared with input buffer.
		 *
		 */
		enum comparison_type
		{
			IGNORE_SPACE = 1, //!< White characters will be ignored in input
			IGNORE_CASE = 2 //!< Case insensitive compare
		};

		//! Stores the type of comparison
		int comparison_type_;

		virtual void invalidate() 
		{
		}

	private:
		// TODO  - use ctype<E> locale to compare national specific letters
		bool is_capital( const char cr ) 
		{
			return cr >= 'A' && cr <= 'Z';
		}

		bool is_space( const char cr ) 
		{
			return cr == ' ' || cr == '\t' || cr == '\n';
		}

		bool compare( typename string_type::iterator& it, const E cr )
		{
			if( (*it) == cr || 
					( (comparison_type_ & IGNORE_CASE ) &&
					  ( is_capital(cr) && (cr-'A') == ((*it)-'a') ||
						is_capital(*it) && ((*it)-'A') == (cr-'a') ) ) )
			{
				it++;
				return true;
			}
			else if( is_space(cr) && ( comparison_type_ & IGNORE_SPACE ) ) 
			{
				return true;
			}
			else 
			{
				return false;
			}
		}

};


	template< typename E >
unsigned long basic_pattern_parser<E>::parse( const E *buf, const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_pattern_parser<E> > t(*this, "parse", __FILE__, __LINE__);
	t << "input=";
#endif

	this->prepare_for_parsing();
	typename string_type::iterator it = this->get_valid().begin();

	while( this->m_parsed_size < buf_length && 
			it != this->get_valid().end() && 
			compare( it, buf[this->m_parsed_size] ) )
	{
#ifdef TRACE
		t << buf[this->m_parsed_size];
#endif
		this->m_parsed_size++;
	}
#ifdef TRACE
	t << endl;
#endif

	if( it == this->get_valid().end() )
	{
#ifdef TRACE
		t << "----parse succeeded" << endl;
#endif
		this->m_is_parsed = true;
	}
	else 
	{
#ifdef TRACE
		t << "parse failed" << endl;
#endif
		this->m_parsed_size = 0;
	}

	return this->m_parsed_size;
}

	template< typename E >
std::basic_istream<E>& basic_pattern_parser<E>::parse( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_pattern_parser<E> > t(*this, "parse", __FILE__, __LINE__);
	t << "input=";
#endif

	this->prepare_for_parsing();
	typename string_type::iterator it = this->get_valid().begin();

	std::streampos str_pos = is.tellg();
	E input = 0;
	bool match = true;

	while( is.good()  &&  it != this->get_valid().end()  &&  match )
	{
		this->m_parsed_size++;
		is.read( &input, 1 );
#ifdef TRACE
		t << input;
#endif
		match = compare( it, input );
	}

#ifdef TRACE
	t << endl;
#endif
	if( it == this->get_valid().end() )
	{
		this->m_is_parsed = true;
#ifdef TRACE
		t << "----parse succeeded" << endl;
#endif
	}
	else 
	{
		this->m_parsed_size = 0;
		is.clear();
		is.seekg( str_pos );
#ifdef TRACE
		t << "parse failed" << endl;
#endif
	}
	return is;
}

typedef basic_pattern_parser<char> pattern_parser;
typedef basic_pattern_parser<wchar_t> wpattern_parser;

template< typename A >
class default_type_parser : 
	public basic_default_type_parser<char,A>
{
	public:
		default_type_parser() : basic_default_type_parser<char,A>() {};
		default_type_parser(const default_type_parser& rhs) : 
			basic_default_type_parser<char,A>( rhs ) {};
		default_type_parser(const A& obj) : 
			basic_default_type_parser<char,A>( obj ) {};
};

template< typename A >
class wdefault_type_parser : 
	public basic_default_type_parser<wchar_t,A>
{
	public:
		wdefault_type_parser() : basic_default_type_parser<wchar_t,A>() {};
		wdefault_type_parser(const wdefault_type_parser& rhs) : 
			basic_default_type_parser<wchar_t,A>( rhs ) {};
		wdefault_type_parser(const A& obj) : 
			basic_default_type_parser<wchar_t,A>( obj ) {};
};

	template< typename E, typename A >
	basic_default_type_parser<E,A>::basic_default_type_parser(const char* name)
: basic_object_parser<E,A>(name,basic_parser<E>::terminal_type)
{
}


	template< typename E, typename A >
basic_default_type_parser<E,A>::~basic_default_type_parser()
{
}

template< typename E, typename A >
basic_default_type_parser<E,A>::basic_default_type_parser( 
		const basic_default_type_parser& rhs )
: basic_object_parser<E,A>(rhs)
{
}

template< typename E, typename A >
basic_default_type_parser<E,A>::basic_default_type_parser( 
		const A& obj, const char* name )
: basic_object_parser<E,A>(name, obj, basic_parser<E>::terminal_type)
{
}

template< typename E, typename A >
std::basic_ostream<E>& basic_default_type_parser<E,A>::format( 
		std::basic_ostream<E>& os )
{
	this->prepare_for_formatting();

	std::streampos strPos = os.tellp();
	os << this->get_valid();
	if( ( this->m_is_formatted = os.good() ) ) 
	{
		this->m_formatted_size = os.tellp() - strPos;
	}
	else
	{
		os.seekp(strPos);
	}
	return os;
}

const char* TERMINAL = "terminal";

/*
 * \brief This template class defines parser of type terminal_type. It is parser recognizing 
 * one specific terminal character (value template parameter)
 *
 */
template< typename E, E value, typename F=default_factory<E> >
class basic_terminal : public basic_object_parser< E, E, F >
{
	public:
		basic_terminal();
		virtual ~basic_terminal();

		virtual unsigned long parse( const E *buf, 
				const unsigned long buf_length )
		{
#ifdef TRACE
			tracer<basic_terminal<E,value> > t(*this, "parse", __FILE__, __LINE__);
			t << "input=";
			t << buf[0];
			t << ", buf_length=";
			t << (int)buf_length;
			t << endl;
#endif
			basic_object_parser< E, E, F >::prepare_for_parsing();

			if( buf_length > 0 && 
					(E)buf[0] == this->get_valid() )
			{
#ifdef TRACE
				t << "----parse succeeded" << endl;
#endif
				this->m_is_parsed = true;
				this->m_parsed_size = 1;//sizeof(value);#include "tracer.h"
			}
			else
			{
#ifdef TRACE
				t << "parse failed" << endl;
#endif
			}
			return this->m_parsed_size;
		}    
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );

		virtual std::basic_ostream<E>& format( std::basic_ostream<E>& os );    

		// during allocation the value is assigned to uint8_t- for 
		// comparison during parsing
		E& get_valid()
		{
			if( !this->is_valid() ) 
			{
				this->m_parsed_object = F().create();
				*(this->m_parsed_object) = value;
			}
			return *this->m_parsed_object;
		}

		const char* get_name() const
		{
			std::stringstream ss;
			ss << TERMINAL << "<" << value << ">";
			return ss.str().c_str();
		}

};

	template< class E, E value, typename F >
basic_terminal<E,value,F>::basic_terminal() : basic_object_parser<E,E,F>(TERMINAL) 
{
}

	template< typename E, E value, typename F >
basic_terminal<E,value,F>::~basic_terminal()
{
}

/*
 * \brief basic_terminal::format( str::ostream& os )
 *
 */
	template< typename E, E value, typename F >
std::basic_ostream<E>& basic_terminal<E,value,F>::format( std::basic_ostream<E>& os )
{
	this->prepare_for_formatting();

	std::streampos strPos = os.tellp();
	if( (std::streamoff)strPos < 0 ) strPos = 0;

	E out = this->get_valid();
	if( os.good() ) 
	{
		os.write( (E*)&out,sizeof(E) );
		this->m_formatted_size = sizeof(E);
		this->m_is_formatted = true;
	}
	else 
	{
		os.clear();
		os.seekp( strPos );
	}
	return os;
}

	template< typename E, E value, typename F >
std::basic_istream<E>& basic_terminal<E,value,F>::parse( std::basic_istream<E>& is )
{
	typedef std::basic_istream<E> std_is;

#ifdef TRACE
	tracer<basic_terminal<E,value> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();
	if( !is.good() )
	{
		return is;
	}
	E input = this->get_valid();
	std::streampos str_pos = is.tellg();

	is.read( &input, 1 );

#ifdef TRACE
	t << "input=" << input << endl;
#endif

	if( is.good() && this->get_valid() == input )
	{
#ifdef TRACE
		t << "----parser succeeded" << endl;
#endif
		this->m_parsed_size = 1;
		this->m_is_parsed = true;
	}
	else 
	{
#ifdef TRACE
		t << "parser failed" << endl;
#endif
		is.clear();
		is.seekg( str_pos );
	}
	return is;
}

/*
 * \brief The class is a specialization of \ref basic_terminal for char input
 *
 */
template< char value, typename F=default_factory<char> >
class terminal : public basic_terminal<char,value,F>
{
	public:
		terminal() : basic_terminal<char,value>() 
	{
	}

		virtual ~terminal() 
		{
		}
};

/*
 * \brief The class is a specialization of \ref basic_terminal for wchar_t input
 *
 */
template< wchar_t value, typename F=default_factory<char> >
class wterminal : public basic_terminal<wchar_t,value,F>
{
	public:
		wterminal() : basic_terminal<wchar_t,value> () 
	{
	}

		virtual ~wterminal() 
		{
		}
};

const char* OPTION = "option";

/*
 * \brief An option represents either the empty sequece of symbols, or sequence of symbols defined by second template parameter 'A'.
 *
 */
template <typename E, typename A>
class basic_option : public basic_non_terminal<E, A>
{
	public:
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;
		basic_option();
		virtual ~basic_option();
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );

		const char* get_name() const
		{
			return OPTION;
		}
};

	template< typename E, typename A >
basic_option<E,A>::basic_option() : basic_non_terminal<E,A>()
{
}

	template< typename E, typename A >
unsigned long basic_option<E,A>::parse( const E *buf, const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_option<E,A> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();

	parser_list symbols;
	parser_list_iterator parse_iterator = this->m_symbols.begin();
	this->m_is_parsed = true;

	// copy symbols to temporary list seems to be best to me
	while( parse_iterator != this->m_symbols.end() )
	{
		(*parse_iterator)->parse( buf+this->m_parsed_size, buf_length-this->m_parsed_size );
		if( (*parse_iterator)->is_parsed() )
		{
			this->m_parsed_size += (*parse_iterator)->parsed_size();
			parse_iterator++; // exclude from parsing
		}
		else 
		{
			this->m_parsed_size = 0;
			break;
		}
	}
#ifdef TRACE
	t << "----parse succeeded" << endl;
#endif
	return this->m_parsed_size;
}

	template <typename E, typename A >
std::basic_istream<E>& basic_option<E,A>::parse( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_option<E,A> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();
	parser_list_iterator parse_iterator = this->m_symbols.begin();

	bool is_parsed = true;
	std::streampos stream_pos = is.tellg();
	if( (std::streamoff)stream_pos < 0 ) stream_pos = 0;
	this->m_is_parsed = true;

	while(	parse_iterator != this->m_symbols.end() &&
			!is.bad() )
	{
		(*parse_iterator)->parse( is );
		if( (*parse_iterator)->is_parsed() )
		{
			this->m_parsed_size += (*parse_iterator)->parsed_size();
			parse_iterator++;
		}
		else
		{
			// reset the starting os buf possition in case the option is not recognized
			is.clear();
			is.seekg( stream_pos );
			this->m_parsed_size = 0;
			break;
		}
	}
#ifdef TRACE
	t << "---parser succeeded" << endl;
#endif
	return is;
}

	template <typename E, typename A >
basic_option<E,A>::~basic_option()
{
}

/*
 * \brief The class is a specialization of \ref basic_option for char input
 *
 */
template <typename A>
class option : public basic_option<char, A>
{
	public:
		option() : basic_option<char,A>() 
	{
	}

		virtual ~option() 
		{
		}

};

/*
 * \brief The class is a specialization of \ref basic_set for wchar_t input
 *
 */
template <typename A>
class woption : public basic_option<wchar_t, A>
{
	public:
		woption() : basic_option<wchar_t,A>() 
	{
	}

		virtual ~woption() 
		{
		}

};

/*
 * \brief The class is a specialization of \ref basic_choice for char input
 *
 */
template <typename A, typename F=default_factory<A> >
class choice : public basic_choice<char,A,F>
{
	public:

		choice() : basic_choice<char,A,F>() 
	{
	}

		choice(const choice& rhs) : 
			basic_choice<char,A,F>( rhs ) 
	{
	}

		choice(const A& obj) : 
			basic_choice<char,A,F>(obj) 
	{
	}

		virtual ~choice() 
		{
		}

};

/*
 * \brief The class is a specialization of \ref basic_choice for wchar_t input
 *
 */
template <typename A, typename F=default_factory<A> >
class wchoice : public basic_choice<wchar_t,A,F>
{	
	public:	

		wchoice() : basic_choice<wchar_t,A,F>() 
	{
	}

		wchoice(const wchoice& rhs) : 
			basic_choice<wchar_t,A,F>( rhs ) 
	{
	}

		wchoice(const A& obj) : 
			basic_choice<wchar_t,A,F>(obj) 
	{
	}

		virtual ~wchoice() 
		{
		}

};

// IMPLEMENTATION
	template< typename E, typename A, typename F >
	basic_choice<E,A,F>::basic_choice()
: basic_non_terminal<E,A,F>()
{
	this->set_name( CHOICE );
}

	template< typename E, typename A, typename F >
	basic_choice<E,A,F>::basic_choice( const basic_choice& rhs )
: basic_non_terminal<E,A,F>(rhs)
{
	this->set_name( CHOICE );
};

	template< typename E, typename A, typename F >
	basic_choice<E,A,F>::basic_choice( const A& obj )
: basic_non_terminal<E,A,F>(obj)
{
	this->set_name( CHOICE );
};

	template< typename E, typename A, typename F >
basic_choice<E,A,F>::~basic_choice()
{
	this->set_name( CHOICE );
}

	template< typename E, typename A, typename F >
unsigned long basic_choice<E,A,F>::parse( const E *buf, 
		const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_choice<E,A,F> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();

	parser_list_iterator parse_iterator = this->m_symbols.begin();
	while( parse_iterator != this->m_symbols.end() && !this->m_is_parsed )
	{
		// try to parse any of the symbols, stop when
		// found the first successive parser
		this->m_parsed_size = (*parse_iterator)->parse(buf, buf_length);
		this->m_is_parsed = (*parse_iterator)->is_parsed();
		if( !this->m_is_parsed )  
		{
			(*parse_iterator)->invalidate();
		}

		parse_iterator++;
	}
	// unset 'parsed' flag for rest (for parser reuse)
	while( parse_iterator != this->m_symbols.end() )
	{
		(*parse_iterator++)->invalidate();
	}
#ifdef TRACE
	if( this->m_is_parsed ) t << "----parser succeeded";
	else t << "parser failed";
	t << endl;
#endif

	return this->m_parsed_size;
}

	template< typename E, typename A, typename F >
std::basic_istream<E>& basic_choice<E,A,F>::parse( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_choice<E,A,F> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();
	parser_list_iterator parse_iterator = this->m_symbols.begin();

	std::streampos stream_pos = is.tellg();
	if( (std::streamoff)stream_pos < 0 ) stream_pos = 0;

	while(	parse_iterator != this->m_symbols.end() &&
			!is.bad() && 
			!this->m_is_parsed )
	{
		(*parse_iterator)->parse( is );
		this->m_parsed_size = (*parse_iterator)->parsed_size();
		this->m_is_parsed = (*parse_iterator)->is_parsed();
		if( !this->m_is_parsed )  
		{
			(*parse_iterator)->invalidate();
		}
		parse_iterator++;
	}

	// unset 'parsed' flag for rest (for parser reuse)
	while( parse_iterator != this->m_symbols.end() )
	{
		(*parse_iterator++)->invalidate();
	}

	// reset the starting os buf possition in case of error
	if( parse_iterator != this->m_symbols.end() )
	{
		is.clear();
		is.seekg( stream_pos );
		this->m_parsed_size = 0;
	}
#ifdef TRACE
	if( this->m_is_parsed ) t << "----parser succeeded";
	else t << "parser failed";
	t << endl;
#endif
	return is;
}

/*
 * \brief The terminal_type parser, which stores the value of specified type from input, 
 * i.e. if 	typename A is int, and input is "5=5", it stores 5, input rest will be "=5"
 *
 */
template< typename E, typename A >
class basic_value_parser : public basic_default_type_parser<E,A>
{
	public:
		basic_value_parser();

		~basic_value_parser();
		virtual unsigned long parse( const E* buf, const unsigned long buf_length );
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );

};

template<typename E>
class basic_string_parser : 
	public basic_value_parser< E, std::basic_string<E> >
{
	public:
		typedef std::basic_string<E> std_string;
		enum white_chars
		{
			NEW_LINE= '\n',
			SPACE = ' ',
			TAB = '\t',
			CARRIAGE_RETURN = 13,
			LINE_FEED = 10
		};

		enum directives
		{
			NONE = 0,
			NO_NEW_LINES = 1,
			NO_SPACES = 2,
			NO_TABS = 4,
			NO_CARRIAGE_RETURNS = 8,
			NO_LINE_FEEDS = 16
		};

		enum string_type
		{
			STRING = NONE,
			NORMALIZED_STRING = NO_CARRIAGE_RETURNS | NO_LINE_FEEDS | NO_TABS,
			TOKEN = NO_SPACES | NO_LINE_FEEDS | NO_TABS, 
			NMTOKEN = NO_SPACES | NO_LINE_FEEDS | NO_TABS
		};

		basic_string_parser();
		basic_string_parser(const E separator, int a_directives);
		~basic_string_parser();

		unsigned long parse( const E* buf, const unsigned long buf_length );
		std::basic_istream<E>& parse( std::basic_istream<E>& is );

		operator std::basic_string<E>() 
		{ 
			return this->get_valid(); 
		}

	protected:
		E			m_separator;
		int			m_directives;

};

template< typename E >
basic_string_parser<E>::basic_string_parser() :
	basic_value_parser< E, std::basic_string<E> >(), m_separator(NEW_LINE), m_directives(NO_NEW_LINES)
{
}

template< typename E >
basic_string_parser<E>::basic_string_parser(const E separator, int a_directives) : 
	basic_value_parser< E, std::basic_string<E> >(), m_separator(separator), m_directives(a_directives)
{
}

	template< typename E >
basic_string_parser<E>::~basic_string_parser()
{
}

	template< typename E >
unsigned long basic_string_parser<E>::parse( const E* buf, const unsigned long buf_length )
{
	this->prepare_for_parsing();

	this->m_is_parsed = true;
	int i = 0;
	for( i=0; i<(int)buf_length && buf[i] != this->m_separator; i++ )
	{
		if( ((this->m_directives & NO_NEW_LINES) && (buf[i] == NEW_LINE)) ||
				((this->m_directives & NO_SPACES) && (buf[i] == SPACE)) ||
				((this->m_directives & NO_TABS) && (buf[i] == TAB)) ||
				((this->m_directives & NO_CARRIAGE_RETURNS) && (buf[i] == CARRIAGE_RETURN)) ||
				((this->m_directives & NO_LINE_FEEDS) && (buf[i] == LINE_FEED)) )
		{
			this->m_is_parsed = false;
			break;
		}

	};

	if( this->m_is_parsed )
	{
		this->get_valid() = std::basic_string<E>( buf, i );
		this->m_parsed_size = this->get_valid().length();
	}
	return this->m_parsed_size;
}

	template< typename E >
std::basic_istream<E>& basic_string_parser<E>::parse( std::basic_istream<E>& is )
{
	this->prepare_for_parsing();

	std::streampos str_pos = is.tellg();
	std::basic_stringbuf<E> buf;
	is.get( buf, this->m_separator );
	typename std_string::iterator it = buf.str().begin();

	this->m_is_parsed = true;
	E input = 0;
	while( it != buf.str().end() )
	{
		input = (*it);
		// WARN:: para remover os warnnings foram adicionados parenteses no 'if' abaixo
		if( ((this->m_directives & NO_NEW_LINES) && (input == NEW_LINE)) ||
				((this->m_directives & NO_SPACES) && (input == SPACE)) ||
				((this->m_directives & NO_TABS) && (input == TAB)) ||
				((this->m_directives & NO_CARRIAGE_RETURNS) && (input == CARRIAGE_RETURN)) ||
				((this->m_directives & NO_LINE_FEEDS) && (input == LINE_FEED)) )
		{
			this->m_is_parsed = false;
			break;
		}
		it++;
	};

	if( this->m_is_parsed )
	{	
		this->get_valid() = buf.str();
		this->m_parsed_size = this->get_valid().length();
	}
	else
	{
		is.clear();
		is.seekg(str_pos);
	}	
	return is;
}

typedef basic_string_parser<char> string_parser;
typedef basic_string_parser<wchar_t> wstring_parser;

const char* FACTOR_WITH_REPETITION = "factor-with-repetition";

/*
 * \brief Represents an explitic number of sequences, where each sequence 
 * is a sequence of symbols represented by syntactic primary - template parameter P is a parser of primary.
 *
 * TODO - provide example
 *
 */
template<typename E, 
	unsigned int N, 
	typename P, 
	typename F=default_factory<std::vector<P> > >
	class basic_factor_with_repetition : 
		public basic_object_parser<E,std::vector<P>, F >
{
	public:	
		typedef std::vector<P> vector_P;
		typedef typename vector_P::iterator vector_P_iterator;
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;

		//! Default constructor
		basic_factor_with_repetition() :
			basic_object_parser<E,vector_P,F>(
					FACTOR_WITH_REPETITION,
					basic_parser<E>::non_terminal_type ) 
	{
	}

		//! Copy constructor
		basic_factor_with_repetition(const basic_factor_with_repetition& rhs ) :
			basic_object_parser<E,vector_P,F>(rhs) 
	{
	}

		/*
		 * \brief The parse method tries to parse the sequence represented by factor 
		 * (template parameter) the number of times defined by template parameter P.
		 *
		 */
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );

		/*
		 * \brief The parse method tries to parse the sequence represented by factor 
		 * (template parameter) the number of times defined by template parameter P.
		 *
		 */
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );

		/*
		 * \brief Calls format of each parser in set of parsers.
		 *
		 */
		virtual std::basic_ostream<E>& format( std::basic_ostream<E>& os );

		const char* get_name() const 
		{
			std::stringstream ss;
			P a;
			ss << FACTOR_WITH_REPETITION << "<" << a.get_name() << ">";
			return ss.str().c_str();
		}

		/*
		 * \brief Use this method to return list of objects parsed by parser (parser type 
		 * is defined by template parameter P, for example \ref non_terminal<R> )
		 *
		 */
		template< class R >
			void get_list( std::list<R>& l )
			{
				vector_P_iterator it = this->get_valid().begin();
				while( it != this->get_valid().end() )
				{
					l.push_back( (*it).get_valid() );
					it++;
				}
			}

	protected:
		/*
		 * \brief The method additionally prepares the vector of parsers (of size N - template parameter)
		 *
		 */
		virtual void prepare_for_parsing()
		{
			basic_object_parser<E,vector_P,F>::prepare_for_parsing();
			for( int i=0; i<N; i++ )
			{
				// uses copy constr of parser
				// TODO - use factory
				this->get_valid().push_back( P() );
			}
		}
};

/*
 * \brief The class is a specialization of \ref basic_factor_with_repetition for char input
 *
 */
template<unsigned int N, 
	typename P, 
	typename F=default_factory< std::vector<P> > >
	class factor_with_repetition : public basic_factor_with_repetition<char,N,P,F>
{
	public:
		factor_with_repetition() : basic_factor_with_repetition<char,N,P,F>() {};
		factor_with_repetition( const factor_with_repetition& rhs) :
			basic_factor_with_repetition<char,N,P,F>(rhs) {};
};

/*
 * \brief The class is a specialization of \ref basic_factor_with_repetition for wchar_t input
 *
 */
template<unsigned int N, 
	typename P, 
	typename F=default_factory< std::vector<P> > >
	class wfactor_with_repetition : public basic_factor_with_repetition<wchar_t,N,P,F>
{
	public:
		wfactor_with_repetition() : basic_factor_with_repetition<wchar_t,N,P,F>() {};
		wfactor_with_repetition( const wfactor_with_repetition& rhs) :
			basic_factor_with_repetition<char,N,P,F>(rhs) {};
};


template< typename E, unsigned int N, typename P, typename F >
	std::basic_istream<E>& basic_factor_with_repetition<E,N,P,F>::parse
( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_factor_with_repetition<E,N,P,F> > 
		t(*this, "parse", __FILE__, __LINE__);
#endif

	this->prepare_for_parsing();
	std::streampos stream_pos = is.tellg();
	if( (std::streamoff)stream_pos < 0 ) stream_pos = 0;

	vector_P_iterator it = this->get_valid().begin();
	while( it != this->get_valid().end() )
	{
		(*it).parse(is);
		if( !(*it).is_parsed() ) break;
		this->m_parsed_size += (*it).parsed_size();
		it++;
	}

	if( it != this->get_valid().end() )
	{
		// means it is not parsed required number of times
		is.clear();
		is.seekg( stream_pos );
		this->m_parsed_size = 0;
#ifdef TRACE
		t << "parser failed " << endl;
#endif
	}
	else
	{
#ifdef TRACE
		this->m_is_parsed = true;
		t << "parser succeeded " << endl;
#endif
	}
	return is;
}

template<typename E, unsigned int N, typename P, typename F >
unsigned long basic_factor_with_repetition<E,N,P,F>::parse( 
		const E *buf, 
		const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_factor_with_repetition<E,N,P,F> >
		t(*this, "parse", __FILE__, __LINE__);
#endif
	prepare_for_parsing();
	vector_P_iterator it = this->get_valid().begin();
	while( it != this->get_valid().end() )
	{
		(*it).parse(buf+this->m_parsed_size,buf_length-this->m_parsed_size);
		if( !(*it).is_parsed() ) break;
		this->m_parsed_size += (*it).parsed_size();
		it++;
	}

	if( it != this->get_valid().end() )
	{
		// means it is not parsed required number of times
		this->m_parsed_size = 0;

#ifdef TRACE
		t << "parser failed " << endl;
#endif
	}
	else
	{
		this->m_is_parsed = true;
#ifdef TRACE
		t << "parser succeeded " << endl;
#endif
	}
	return this->m_parsed_size;
}

	template<typename E, unsigned int N, typename P, typename F >
std::basic_ostream<E>& basic_factor_with_repetition<E,N,P,F>::format( std::basic_ostream<E>& os )
{
	this->prepare_for_formatting();
	vector_P_iterator format_iterator = this->get_valid().begin();
	bool is_formatted = true;
	std::streampos streamPos = os.tellp();
	if( (std::streamoff)streamPos < 0 ) streamPos = 0;

	while(	format_iterator != this->get_valid().end() &&
			!os.bad() && 
			is_formatted )
	{
		(*format_iterator).format(os);
		this->m_formatted_size += (*format_iterator).formatted_size();
		is_formatted = (*format_iterator).is_formatted();
		format_iterator++;
	}
	return os;
}

const char* TERM_WITH_EXCEPTION = "term-with-exception";

/*
 * \brief The syntactic term with exception - represents seuence of symbols that is not a 
 * sequence of symbols represented by exception - exception is template parameter B, accepted 
 * term is represented by template parameter A, both are instantiated as parsers, so it is 
 * possible to define parameter A as non_terminal< MyClass1> and B as terminal<' '>.
 * Template parameter F is factory (allocator) for objects of type A
 */
template<typename E, typename A, typename B, typename F=default_factory<A> >
class basic_term_with_exception : public basic_object_parser<E,A,F>
{
	public:	
		typedef typename basic_parser<E>::parser_list parser_list;
		typedef typename basic_parser<E>::parser_list_iterator parser_list_iterator;

		//! Default constructor
		basic_term_with_exception() :
			basic_object_parser<E,A,F>(
					TERM_WITH_EXCEPTION,
					basic_parser<E>::non_terminal_type ) 
	{
	}

		//! Copy constructor
		basic_term_with_exception(const basic_term_with_exception& rhs ) :
			basic_object_parser<E,A,F>(rhs) 
	{
	}

		/*
		 * \brief The parse method tries to parse exception at first - if exception is found 
		 * at the beginning of the input, the parse method fails. If exception is not found, 
		 * continues parsing the term.
		 *
		 */
		virtual unsigned long parse( const E *buf, const unsigned long buf_length );

		/*
		 * \brief The parse method tries to parse exception at first - if exception is found 
		 * at the beginning of the input, the parse method fails. If exception is not found, 
		 * continues parsing the term.
		 */
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is );

		/*
		 * \brief Does the same as A::format(). (A is parsed defined in template parameter list)
		 *
		 */
		virtual std::basic_ostream<E>& format( std::basic_ostream<E>& os )
		{
			this->prepare_for_formatting();
			return this->get_valid().format(os);
		}

		const char* get_name() const 
		{
			std::stringstream ss;
			A a;
			ss << TERM_WITH_EXCEPTION << "<" << a.get_name() << ">";
			return ss.str().c_str();
		}

};

/*
 * \brief The class is a specialization of \ref basic_term_with_exception for char input
 *
 */
template<typename A,typename B, typename F=default_factory<A> >
class term_with_exception : public basic_term_with_exception<char,A,B,F>
{
	public:
		term_with_exception() : basic_term_with_exception<char,A,B,F>() 
	{
	}

		term_with_exception( const term_with_exception& rhs) :
			basic_term_with_exception<char,A,B,F>(rhs) 
	{
	}

};

/*
 * \brief The class is a specialization of \ref basic_term_with_exception for wchar_t input
 *
 */
template<typename A,typename B, typename F=default_factory<A> >
class wterm_with_exception : public basic_term_with_exception<wchar_t,A,B,F>
{
	public:
		wterm_with_exception() : basic_term_with_exception<wchar_t,A,B>() 
	{
	}

		wterm_with_exception( const wterm_with_exception& rhs) :
			basic_term_with_exception<char,A,B,F>(rhs) 
	{
	}

};

	template< typename E, typename A, typename B, typename F>
std::basic_istream<E>& basic_term_with_exception<E,A,B,F>::parse( std::basic_istream<E>& is )
{
#ifdef TRACE
	tracer<basic_term_with_exception<E,A,B,F> > t(*this, "parse", __FILE__, __LINE__);
#endif

	this->prepare_for_parsing();

	std::streampos stream_pos = is.tellg();
	if( (std::streamoff)stream_pos < 0 ) stream_pos = 0;

	B exception;
	exception.parse(is);
	if( exception.is_parsed() )
	{
		is.clear();
		is.seekg( stream_pos );
		this->m_parsed_size = 0;
#ifdef TRACE
		t << "parser failed - exception is parsed" << endl;
#endif
		return is;
	}

	this->get_valid().parse( is );
	if( this->get_valid().is_parsed() )
	{
		this->m_is_parsed = true;
		this->m_parsed_size = this->get_valid().parsed_size();
	}
	return is;
}

	template< typename E, typename A, typename B, typename F >
unsigned long basic_term_with_exception<E,A,B,F>::parse( const E *buf, const unsigned long buf_length )
{
#ifdef TRACE
	tracer<basic_term_with_exception<E,A,B> > t(*this, "parse", __FILE__, __LINE__);
#endif
	this->prepare_for_parsing();

	B exception;
	exception.parse( buf, buf_length );
	if( exception.is_parsed() )
	{
		this->m_parsed_size = 0;
#ifdef TRACE
		t << "parser failed - exception is parsed" << endl;
#endif
		return 0;
	}
	this->get_valid().parse( buf, buf_length);
	if( this->get_valid().is_parsed() )
	{
		this->m_is_parsed = true;
		this->m_parsed_size = this->get_valid().parsed_size();
	}
	return this->m_parsed_size;
}

template< typename E, typename A >
basic_value_parser<E,A>::basic_value_parser() : 
	basic_default_type_parser<E,A>(VALUE_PARSER)
{
}

	template< typename E, typename A >
basic_value_parser<E,A>::~basic_value_parser()
{
}

	template< typename E, typename A >
unsigned long basic_value_parser<E,A>::parse( const E* buf, 
		const unsigned long buf_length )
{
	this->prepare_for_parsing();

	std::auto_ptr< std::basic_stringbuf<E> > strbufptr(
			new std::basic_stringbuf<E>( std::basic_string<E>( (const E*)buf, buf_length ) ) );
	std::basic_istream<E> is(strbufptr.get());

	std::streampos begin = is.tellg();
	is >> this->get_valid();
	if( !(this->m_is_parsed = ( !is.fail() && 
					( this->m_parsed_size = is.tellg() - begin ) > 0 ) ) )
	{
		this->m_parsed_size = 0;
	}
	return this->m_parsed_size;
}

	template< typename E, typename A >
std::basic_istream<E>& basic_value_parser<E,A>::parse( std::basic_istream<E>& is )
{
	this->prepare_for_parsing();

	std::streampos strPos = is.tellg();
	is >> this->get_valid();

	this->m_parsed_size = (std::streamoff)is.tellg() - (std::streamoff)strPos;
	this->m_is_parsed = !is.fail() && this->m_parsed_size > 0; 

	if( !this->m_is_parsed )
	{
		is.clear();
		is.seekg( strPos );
		this->m_parsed_size = 0;
	}
	return is;

}

/*
 * \brief The class is a specialization of \ref basic_value_parser for char input
 *
 */
template< typename A >
class value_parser : public basic_value_parser<char,A>
{
	public:
		value_parser() : basic_value_parser<char,A>() {};
		~value_parser () {};
};

/*
 * \brief The class is a specialization of \ref basic_value_parser for wchar_t input
 *
 */
template< typename A >
class wvalue_parser : public basic_value_parser<wchar_t,A>
{
	public:
		wvalue_parser() : basic_value_parser<wchar_t,A>() {};
		~wvalue_parser () {};
};

class match
{
	unsigned long 	m_pos;
	unsigned long 	m_size;
	bool			m_is_valid;
	public:
	match() : m_pos(0), m_size(0), m_is_valid(false) {};
	match( unsigned long pos, unsigned long size ) :
		m_pos(pos), 
		m_size(size), 
		m_is_valid(true) 
	{
	}

	virtual ~match() 
	{
	}

	unsigned long get_pos() const
	{
		return this->m_pos;
	}

	unsigned long get_size() const
	{
		return this->m_size;
	}

	void set_pos( unsigned long pos )
	{
		this->m_pos = pos;
	}

	void set_size( unsigned long size )
	{
		this->m_size = size;
	}

	const match& operator = ( const match& m )
	{
		this->m_pos = m.get_pos();
		this->m_size = m.get_size();

		return m;
	}

	bool is_valid()
	{
		return this->m_is_valid;
	}
};

class matches
{
	public:

		typedef std::map<std::string, match>			matches_coll;
		typedef matches_coll::key_type					match_key;
		typedef matches_coll::value_type				match_value;
		typedef std::list<match>						matches_ret_coll;

	protected:
		matches_coll								m_matches;

	public:	
		void insert( match_key key, match value )
		{
			this->m_matches.insert( match_value( key, value ) );

		}

		match get( match_key key )
		{
			matches_coll::iterator it = this->m_matches.find( key );
			if( it != this->m_matches.end() )
			{
				return (*it).second;
			}
			else
			{
				return match(); // with return match, which is not valid (its 
				// is_valid() returns false)
			}
		}

		void get_all(matches_ret_coll& l)
		{
			matches_coll::iterator it = this->m_matches.begin();
			while( it != this->m_matches.end() )
			{
				l.push_back( (*it).second );
				it++;
			}
		}

		void get_all(matches_coll& m)
		{
			m = this->m_matches;
		}

		void clear()
		{
			this->m_matches.clear();
		}

		void extend( matches::match_key parent_key, 
				matches& sub_matches )
		{
			matches_coll ret_coll;
			sub_matches.get_all( ret_coll );

			matches_coll::iterator it = ret_coll.begin();
			while( it != ret_coll.end() )
			{
				matches::match_key new_key = parent_key;
				new_key += (*it).first; // the key if (*it).first must begin with dot
				insert( new_key, (*it).second );
				it++;
			}
		}

		matches() 
		{
		}

		virtual ~matches() 
		{
		}

};

template< typename E >
class ere_base
{
	typedef typename basic_parser<E>::parser_list parser_list;

	protected:
	unsigned long	m_rec_size;
	unsigned long	m_rec_pos;
	bool			m_is_rec;
	public:
	ere_base()
		// CHANGE:: m_is_rec(false), 
		// CHANGE:: m_rec_pos(0), 
		// CHANGE:: m_rec_size(0) 
	{
		m_is_rec = false;
		m_rec_pos = 0;
		m_rec_size = 0;
	}

	virtual ~ere_base()
	{
	}

	bool is_recognized()
	{
		return this->m_is_rec;
	}

	void unset()
	{
		this->m_is_rec = false;
	}

	void set( const unsigned long pos, const unsigned long size )
	{
		this->m_is_rec = true;
		this->m_rec_pos = pos;
		this->m_rec_size = size;
	}

	unsigned long recognized_size() 
	{
		return this->m_rec_size;
	}

	unsigned long recognized_position() 
	{
		return this->m_rec_pos;
	}

	const match& get_match()
	{
		return this->m_match( this->m_rec_pos, this->m_rec_size );
	}

	virtual void assign_matches( matches::match_key parent_address, unsigned long& branch_pos, matches& m ) = 0;
	virtual unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions = true ) = 0;
	virtual void push_parsers( parser_list& l ) = 0;

	std::string to_str( const unsigned long i )
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}

};

template< typename E > class extended_reg_exp;

/*!
  The one of classes implementing following regular expression 
grammar:\n \n
extended_reg_exp	=> ere_branch '|' extended_reg_exp | extended_reg_exp \n
ere_branch			=> ere_expression ere_branch | ere_expression \n
ere_expression		=> ere_expression_and_dups | one_char_or_coll_elem_ere | 
'^' | '$' | '('extended_reg_exp')' \n
ere_expression_and_dups	=> ere_expression_no_dup ere_dups \n
ere_expression_no_dup	=> one_char_or_coll_elem_ere | '^' | '$' 
| '('extended_reg_exp')' \n
ere_dups			=> ere_dupl_symbol ere_dups | ere_dupl_symbol \n
ere_dupl_symbol		=> '*' | '+' | '?' | '{' number '}] | '{' number ',' '}' |
'{' number ',' number '}' \n
one_char_or_coll_elem_ere => 'all except special chars' | '\' special_chars
'.' | bracket_expr \n
special_chars		=> '.' | '[' | '\' | '(' | ')' | '*' | '+' | '?' | '{' 
| '}' | '|' | '^' | '$' \n
*/
template<typename E>
class ere_dupl_symbol
{
	typedef typename basic_parser<E>::parser_list parser_list;

	class dup_count
	{
		protected:
			basic_terminal<E,'{'>				m_left_bracket;
			basic_value_parser<E,unsigned long>	m_repeat_min;
			basic_terminal<E,'}'>				m_right_bracket;
		public:
			virtual ~dup_count()
			{
			}

			void push_parsers( parser_list &l )
			{
				l.push_back(&this->m_left_bracket);
				l.push_back(&this->m_repeat_min);
				l.push_back(&this->m_right_bracket);
			}
			const char* get_name() const { return "{repeat_min}"; }

			virtual unsigned long get_min()
			{
				return this->m_repeat_min.get_valid();
			}
			virtual unsigned long get_max()
			{
				return this->m_repeat_min.get_valid();
			}
	};

	class dup_count_unbound : public dup_count
	{
		protected:
			basic_terminal<E,','>	m_sep;
		public:
			void push_parsers( parser_list &l )
			{
				l.push_back(&this->m_left_bracket);
				l.push_back(&this->m_repeat_min);
				l.push_back(&this->m_sep);
				l.push_back(&this->m_right_bracket);
			}
			const char* get_name() const { return "{repeat_min,}"; }

			unsigned long get_max()
			{
				// TODO - find the maximum integer
				long i = -1;
				return (unsigned long)i;
				// return MAX_INT;
			}

	};

	class dup_count_max : public dup_count_unbound
	{
		protected:
			basic_value_parser<E,unsigned long> m_repeat_max;
		public:
			void push_parsers( parser_list &l )
			{
				l.push_back(&this->m_left_bracket);
				l.push_back(&this->m_repeat_min);
				l.push_back(&this->m_sep);
				l.push_back(&this->m_repeat_max);
				l.push_back(&this->m_right_bracket);
			}
			const char* get_name() const { return "{repeat_min,repeat_max}"; }

			unsigned long get_max()
			{
				return this->m_repeat_max.get_valid();
			}

	};

	class lazy_star
	{
		basic_terminal<E, '*'>		m_star;
		basic_terminal<E, '?'>		m_lazy;
		public:
		void push_parsers( parser_list& l )
		{
			l.push_back( &this->m_star );
			l.push_back( &this->m_lazy );
		}
		const char* get_name() const { return "*?"; }
	};

	class lazy_plus
	{
		basic_terminal<E, '+'>		m_plus;
		basic_terminal<E, '?'>		m_lazy;
		public:
		void push_parsers( parser_list& l )
		{
			l.push_back( &this->m_plus );
			l.push_back( &this->m_lazy );
		}
		const char* get_name() const { return "+?"; }
	};	

	basic_non_terminal<E, lazy_star>	m_lazy_star;
	basic_non_terminal<E, lazy_plus>	m_lazy_plus;
	basic_terminal<E,'*'>			m_star;
	basic_terminal<E,'+'>			m_plus;
	basic_terminal<E,'?'>			m_question_mark;
	basic_non_terminal<E,dup_count>	m_dup;
	basic_non_terminal<E,dup_count_unbound>
		m_dup_unbound;
	basic_non_terminal<E,dup_count_max>
		m_dup_max;

	unsigned long								m_rep;
	public:

	ere_dupl_symbol() : 
		m_lazy_star(),
		m_lazy_plus(),
		m_dup(),
		m_dup_unbound(),
		m_dup_max() 
	{
	}

	virtual ~ere_dupl_symbol() 
	{
	}

	bool is_lazy()
	{
		return this->m_lazy_star.is_parsed() || this->m_lazy_plus.is_parsed();
	}

	unsigned long get_min()
	{
		if( this->m_star.is_parsed() ) return 0;
		else if( this->m_plus.is_parsed() ) return 1;
		else if( this->m_lazy_star.is_parsed() ) return 0;
		else if( this->m_lazy_plus.is_parsed() ) return 1;
		else if( this->m_question_mark.is_parsed() ) return 0;
		else if( this->m_dup.is_parsed() ) return this->m_dup->get_min();
		else if( this->m_dup_unbound.is_parsed() ) return this->m_dup_unbound->get_min();
		else return this->m_dup_max->get_min();
	}

	unsigned long get_max()
	{
		if( this->m_star.is_parsed() ) return INT_MAX;
		else if( this->m_plus.is_parsed() ) return INT_MAX;
		else if( this->m_lazy_star.is_parsed() ) return INT_MAX;
		else if( this->m_lazy_plus.is_parsed() ) return INT_MAX;
		else if( this->m_question_mark.is_parsed() ) return 1;
		else if( this->m_dup.is_parsed() ) return this->m_dup->get_max();
		else if( this->m_dup_unbound.is_parsed() ) return this->m_dup_unbound->get_max();
		else return this->m_dup_max->get_max();
	}

	void set_recognized_dup( unsigned long rec_dup )
	{
		this->m_rep = rec_dup;
	}

	unsigned long get_recognized_dup()
	{
		return this->m_rep;
	}

	void push_parsers( parser_list &l )
	{
		l.push_back(&this->m_lazy_star);
		l.push_back(&this->m_lazy_plus);
		l.push_back(&this->m_star);
		l.push_back(&this->m_plus);
		l.push_back(&this->m_question_mark);
		l.push_back(&this->m_dup);
		l.push_back(&this->m_dup_unbound);
		l.push_back(&this->m_dup_max);
	}

	const char* get_name() const 
	{
		return "ere_dupl"; 
	}

};

/*!
  The one of classes implementing following regular expression 
grammar:\n \n
extended_reg_exp	=> ere_branch '|' extended_reg_exp | extended_reg_exp \n
ere_branch			=> ere_expression ere_branch | ere_expression \n
ere_expression		=> ere_expression_and_dups | one_char_or_coll_elem_ere | 
'^' | '$' | '('extended_reg_exp')' \n
ere_expression_and_dups	=> ere_expression_no_dup ere_dups \n
ere_expression_no_dup	=> one_char_or_coll_elem_ere | '^' | '$' 
| '('extended_reg_exp')' \n
ere_dups			=> ere_dupl_symbol ere_dups | ere_dupl_symbol \n
ere_dupl_symbol		=> '*' | '+' | '?' | '{' number '}' | '{' number ',' '}' |
'{' number ',' number '}' \n
one_char_or_coll_elem_ere => 'all except special chars' | '\' special_chars
'.' | bracket_expr \n
special_chars		=> '.' | '[' | '\' | '(' | ')' | '*' | '+' | '?' | '{' 
| '}' | '|' | '^' | '$' \n
*/
template< typename E >
class ere_dups
{
	typedef typename basic_parser<E>::parser_list 
		parser_list;

	unsigned long m_rep;
	class dup_and_dup_branch
	{
		basic_choice<E,ere_dupl_symbol<E> > 
			m_dup;
		basic_choice<E,ere_dups<E> >	m_dups;

		public:
		void push_parsers( parser_list& l )
		{
			l.push_back( &this->m_dup );
			l.push_back( &this->m_dups );
		}
		const char* get_name() const { return "ere_dup,ere_dups"; }


		unsigned long get_max()
		{
			double max = this->m_dup->get_max();
			if( max == INT_MAX )
				return INT_MAX;

			double max_from_branch = this->m_dups->get_max();
			if( max_from_branch == INT_MAX )
				return INT_MAX;

			max = max * max_from_branch;
			if( max >= INT_MAX )
				return INT_MAX;

			return (unsigned long)max;
		}

		bool is_lazy()
		{
			return (this->m_dup.is_parsed() && this->m_dup->is_lazy()) ||
				(this->m_dups.is_parsed() && this->m_dups->is_lazy());
		}

		unsigned long get_min()
		{
			return this->m_dup->get_min() * this->m_dups->get_min();
		}
	};

	basic_non_terminal<E, dup_and_dup_branch > m_dup_and_dups;
	basic_choice<E,ere_dupl_symbol<E> > m_dup;

	public:

	bool is_lazy()
	{
		return (this->m_dup_and_dups.is_parsed() && this->m_dup_and_dups->is_lazy()) || (this->m_dup.is_parsed() && this->m_dup->is_lazy());
	}

	void push_parsers( parser_list& l )
	{
		l.push_back( &this->m_dup_and_dups );
		l.push_back( &this->m_dup );
	}

	const char* get_name() const 
	{
		return "ere_dups"; 
	}

	void set_recognized_dup( unsigned long rec_dup )
	{
		this->m_rep = rec_dup;
	}

	unsigned long get_recognized_dup()
	{
		return this->m_rep;
	}

	unsigned long get_max()
	{
		if( this->m_dup_and_dups.is_parsed() )
			return this->m_dup_and_dups->get_max();
		else
			return this->m_dup->get_max();
	}

	unsigned long get_min()
	{
		if( this->m_dup_and_dups.is_parsed() )
			return this->m_dup_and_dups->get_min();
		else
			return this->m_dup->get_min();
	}

	ere_dups() : m_rep(0) 
	{
	}

	virtual ~ere_dups() 
	{
	}

};

template<typename E>
class meta_char_choice
{
	typedef typename basic_parser<E>::parser_list parser_list;

	basic_terminal<E,'^'>			m_circumflex;
	basic_terminal<E,'-'>			m_hyphen;
	basic_terminal<E,']'>			m_right_bracket;

	public:
	meta_char_choice() {};

	~meta_char_choice() {};
	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_circumflex );
		l.push_back( &this->m_hyphen );
		l.push_back( &this->m_right_bracket );
	}	
	const char* get_name() const { return "^|-|]"; }

	int compare( const E* buf, const unsigned long buf_length )
	{
		if( this->m_hyphen.is_parsed() )
		{
			if( std::use_facet< std::collate<E> >(std::locale()).compare(buf, buf+1, this->m_hyphen.get(), this->m_hyphen.get()+1 ) == 0 )
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else if( this->m_right_bracket.is_parsed() )
		{
			if( std::use_facet< std::collate<E> >(std::locale()).compare( buf, buf+1, this->m_right_bracket.get(), this->m_right_bracket.get()+1) == 0 )
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if( std::use_facet< std::collate<E> >(std::locale()).compare( buf, buf+1, this->m_circumflex.get(), this->m_circumflex.get()+1) == 0 )
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
};

template< typename E >
class collating_symbol
{
	typedef typename basic_parser<E>::parser_list parser_list;

	class collating_base
	{
		protected:
			int m_size;
		public:
			int matched_size() { return this->m_size; }
			collating_base() : m_size(0) {};
	};

	class opendot_coll_elem_single_dotclose : public collating_base
	{
		basic_terminal<E, '['>	m_open;
		basic_terminal<E, '.'>	m_left_dot;
		basic_except_terminal<E,meta_char_choice<E> >	
			m_coll_elem_single;
		basic_terminal<E, '.'>	m_right_dot;
		basic_terminal<E, ']'>	m_close;
		public:

		void push_parsers( parser_list &l )
		{
			l.push_back(&this->m_open);
			l.push_back(&this->m_left_dot);
			l.push_back(&this->m_coll_elem_single);
			l.push_back(&this->m_right_dot);
			l.push_back(&this->m_close);
		}
		const char* get_name() const { return "'[','.',collating_element_single,'.',']'"; }

		int compare( const E* buf, const unsigned long buf_length )
		{
			int ret = std::use_facet< std::collate<E> >(std::locale()).compare(buf, buf, this->m_coll_elem_single.get(), this->m_coll_elem_single.get() );
			if( ret == 0 ) 
				this->m_size =1;
			else
				this->m_size = 0;
			return ret;
		}	
	};

	class opendot_coll_elem_multi_dotclose : public collating_base
	{
		typedef std::ctype_base ctypebase;
		basic_terminal<E, '['>	m_open;
		basic_terminal<E, '.'>	m_left_dot;
		basic_string_parser<E>	m_coll_elem_multi;
		// right dot is also used as separator in string parser - see constructor
		basic_terminal<E, '.'>	m_right_dot;
		basic_terminal<E, ']'>	m_close;
		public:
		opendot_coll_elem_multi_dotclose() : 
			collating_base(),
			m_coll_elem_multi('.', basic_string_parser<E>::NONE) {};

		void push_parsers( parser_list &l )
		{
			l.push_back(&this->m_open);
			l.push_back(&this->m_left_dot);
			l.push_back(&this->m_coll_elem_multi);
			l.push_back(&this->m_right_dot);
			l.push_back(&this->m_close);
		}

		const char* get_name() const 
		{
			return "'[','.',collating_element_multi,'.',']'"; 
		}

		int compare( const E* buf, const unsigned long buf_length )
		{			
			unsigned long to_compare_length =
				collating_symbol<E>::get_symbol_len(buf,buf_length);
			std::basic_string<E> multichar = *(m_coll_elem_multi.get());
			// check the size of element in buf  - try the longest
			int ret = std::use_facet< std::collate<E> >(std::locale()).compare(buf, buf+to_compare_length, multichar.c_str(), multichar.c_str() + multichar.length() );
			this->m_size = to_compare_length;
			return ret;
		}	
	};

	class opendot_metachar_dotclose : public collating_base
	{
		basic_terminal<E, '['>	m_open;
		basic_terminal<E, '.'>	m_left_dot;
		basic_choice<E,meta_char_choice<E> >	
			m_meta_char;
		basic_terminal<E, '.'>	m_right_dot;
		basic_terminal<E, ']'>	m_close;
		public:
		void push_parsers( parser_list &l )
		{
			l.push_back(&this->m_open);
			l.push_back(&this->m_left_dot);
			l.push_back(&this->m_meta_char);
			l.push_back(&this->m_right_dot);
			l.push_back(&this->m_close);
		}	
		const char* get_name() const { return "'[','.',meta_char,'.',']'"; }

		int compare( const E* buf, const unsigned long buf_length )
		{
			int ret = this->m_meta_char->compare(buf,buf_length);
			this->m_size = 1; //this->m_meta_char->matched_size();
			return ret;
		}	
	};

	basic_non_terminal<E, opendot_coll_elem_single_dotclose> m_coll_elem_single;
	basic_non_terminal<E, opendot_coll_elem_multi_dotclose> m_multichar;
	basic_non_terminal<E, opendot_metachar_dotclose>  m_metachar;
	int m_size;
	public:
	collating_symbol() : 
		m_coll_elem_single(),
		// CHANGE:: m_metachar(),
		// CHANGE:: m_multichar(),
		m_size(0) {};

	void push_parsers( parser_list &l )
	{
		l.push_back(&this->m_multichar);
		l.push_back(&this->m_metachar);
		l.push_back(&this->m_coll_elem_single);
	}	
	const char* get_name() const { return "collating_symbol"; }

	int matched_size() { return this->m_size; }
	int compare( const E* buf, const unsigned long buf_length )
	{
		int ret = 0;
		if( this->m_coll_elem_single.is_parsed() )
		{
			ret = this->m_coll_elem_single->compare( buf, buf_length );
			this->m_size = this->m_coll_elem_single->matched_size();
		}
		else if( this->m_multichar.is_parsed() )
		{
			ret = this->m_multichar->compare( buf, buf_length );
			this->m_size = this->m_multichar->matched_size();
		}
		else
		{
			ret = this->m_metachar->compare( buf, buf_length );
			this->m_size = this->m_metachar->matched_size();
		}
		return ret;
	}
	static unsigned long get_symbol_len( const E* buf, const unsigned long buf_length )
	{
		return 1; // TODO - see below commented out methods for the problem
	}
};

template<typename E>
class collating_choice
{
	typedef typename basic_parser<E>::parser_list parser_list;
	typedef std::ctype_base ctypebase;

	// bug XXXXX correction  - define parser of escaped special chars
	class escaped_right_bracket
	{
		basic_terminal<E,'\\'>				m_esc;
		basic_terminal<E,']'>					m_right_bracket;
		public:
		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_esc );
			l.push_back( &this->m_right_bracket );
		}	
		const char* get_name() const { return "escaped_right_bracket"; }
	};
	class escaped_hyphen
	{
		basic_terminal<E,'\\'>				m_esc;
		basic_terminal<E,'-'>					m_hyphen;
		public:
		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_esc );
			l.push_back( &this->m_hyphen );
		}	
		const char* get_name() const { return "escaped_hyphen"; }
	};
	class escaped_circumflex
	{
		basic_terminal<E,'\\'>				m_esc;
		basic_terminal<E,'^'>					m_circumflex;
		public:
		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_esc );
			l.push_back( &this->m_circumflex );
		}	
		const char* get_name() const { return "escaped_circumflex"; }
	};

	basic_non_terminal<E,escaped_right_bracket>	m_escaped_right_bracket;
	basic_non_terminal<E,escaped_hyphen>			m_escaped_hyphen;
	basic_non_terminal<E,escaped_circumflex>		m_escaped_circumflex;
	basic_except_terminal<E,meta_char_choice<E> >	m_non_metas;
	basic_choice<E,collating_symbol<E> >		m_collating_symbol;

	unsigned long m_size;

	public:
	collating_choice() : m_size(0) 
	{
	}

	virtual ~collating_choice() 
	{
	}

	int matched_size() 
	{
		return this->m_size; 
	}

	int compare( const E* buf, const unsigned long buf_length )
	{
		int ret = -1;
		this->m_size = 0;
		if( this->m_non_metas.is_parsed() )
		{
			unsigned long to_compare_length = 
				collating_symbol<E>::get_symbol_len(buf,buf_length);

			ret = std::use_facet< std::collate<E> >(std::locale()).compare(buf, buf+to_compare_length, this->m_non_metas.get(),	this->m_non_metas.get()+1 );
			this->m_size = to_compare_length;
		}
		else if( this->m_escaped_right_bracket.is_parsed() )
		{
			unsigned long to_compare_length = 
				collating_symbol<E>::get_symbol_len(buf,buf_length);
			const E i(']');
			ret = std::use_facet< std::collate<E> >(std::locale()).compare(buf,buf+to_compare_length,&i, &i+1 );
			this->m_size = 1;
			this->m_size = 1;
		}
		else if( this->m_escaped_hyphen.is_parsed() )
		{
			unsigned long to_compare_length = 
				collating_symbol<E>::get_symbol_len(buf,buf_length);
			const E i('-');
			ret = std::use_facet< std::collate<E> >(std::locale()).compare(buf,buf+to_compare_length,&i,&i+1 );
			this->m_size = 1;
		}
		else if( this->m_escaped_circumflex.is_parsed() )
		{
			unsigned long to_compare_length = 
				collating_symbol<E>::get_symbol_len(buf,buf_length);
			const E i('^');
			ret = std::use_facet< std::collate<E> >(std::locale()).compare(buf,buf+to_compare_length,&i,&i+1 );
			this->m_size = 1;
		}
		else
		{
			ret = this->m_collating_symbol->compare(buf,buf_length); 
			this->m_size = this->m_collating_symbol->matched_size();
		}
		return ret;
	}

	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_escaped_right_bracket );
		l.push_back( &this->m_escaped_hyphen );
		l.push_back( &this->m_escaped_circumflex );
		l.push_back( &this->m_collating_symbol );
		l.push_back( &this->m_non_metas );
	}	
	const char* get_name() const { return "collating_choice"; }
};

template<typename E>
class class_name
{
	typedef typename basic_parser<E>::parser_list parser_list;
	typedef std::ctype_base ctypebase;

	basic_pattern_parser<E>		m_alnum;
	basic_pattern_parser<E>		m_cntrl;
	basic_pattern_parser<E>		m_lower;
	basic_pattern_parser<E>		m_space;
	basic_pattern_parser<E>		m_alpha;
	basic_pattern_parser<E>		m_digit;
	basic_pattern_parser<E>		m_print;
	basic_pattern_parser<E>		m_upper;
	basic_pattern_parser<E>		m_blank;
	basic_pattern_parser<E>		m_graph;
	basic_pattern_parser<E>		m_punct;
	basic_pattern_parser<E>		m_xdigit;

	int m_size;	
	public:
	class_name() : 
		m_alnum( "alnum" ),
		m_cntrl( "cntrl" ),
		m_lower( "lower" ),
		m_space( "space" ),
		m_alpha( "alpha" ),
		m_digit( "digit" ),
		m_print( "print" ),
		m_upper( "upper" ),
		m_blank( "blank" ),
		m_graph( "graph" ),
		m_punct( "punct" ),
		m_xdigit( "xdigit" ),
		m_size(0) {};

	int matched_size() 
	{
		return this->m_size; 
	}

	void push_parsers( parser_list &l )
	{
		l.push_back(&this->m_alnum);
		l.push_back(&this->m_cntrl);
		l.push_back(&this->m_lower);
		l.push_back(&this->m_space);
		l.push_back(&this->m_alpha);
		l.push_back(&this->m_digit);
		l.push_back(&this->m_print);
		l.push_back(&this->m_upper);
		l.push_back(&this->m_blank);
		l.push_back(&this->m_graph);
		l.push_back(&this->m_punct);
		l.push_back(&this->m_xdigit);
	}

	const char * get_name() const 
	{
		return "class_name"; 
	}

	unsigned long compare( const E* buf, const unsigned long buf_length )
	{
		// CHANGE:: std::ctype_base t;
		// CHANGE:: int ret = 0;
		ctypebase::mask m;


		if( this->m_alnum.is_parsed() ) m=ctypebase::alnum;
		if( this->m_cntrl.is_parsed() ) m=ctypebase::cntrl;
		if( this->m_lower.is_parsed() ) m=ctypebase::lower;
		if( this->m_space.is_parsed() ) m=ctypebase::space;
		if( this->m_alpha.is_parsed() ) m=ctypebase::alpha;
		if( this->m_digit.is_parsed() ) m=ctypebase::digit;
		if( this->m_print.is_parsed() ) m=ctypebase::print;
		if( this->m_upper.is_parsed() ) m=ctypebase::upper;
		// if( this->m_blank.is_parsed() ) m=ctypebase::blank;
		if( this->m_graph.is_parsed() ) m=ctypebase::graph;
		if( this->m_punct.is_parsed() ) m=ctypebase::punct;
		if( this->m_xdigit.is_parsed() ) m=ctypebase::xdigit;

		while( this->m_size < (int)buf_length )
		{
			if( !std::use_facet< std::ctype<E> >(std::locale()).is( buf, 
						buf+this->m_size+1, &m ) )
			{
				break;
			}
			this->m_size++;
		}

		if( this->m_size > 0 )
			return 0;
		else
			return 1;
	}

	virtual ~class_name()
	{
	}

};

template<typename E>
class character_class
{
	typedef typename basic_parser<E>::parser_list parser_list;
	basic_terminal<E,'['>			m_open;
	basic_terminal<E,':'>			m_open_colon;
	basic_choice<E,class_name<E> >	m_class_name;
	basic_terminal<E,':'>			m_close_colon;
	basic_terminal<E,']'>			m_close;

	public:
	void push_parsers( parser_list &l )
	{
		l.push_back(&this->m_open);
		l.push_back(&this->m_open_colon);
		l.push_back(&this->m_class_name);
		l.push_back(&this->m_close_colon);
		l.push_back(&this->m_close);
	}
	const char* get_name() const { return "'[',':',class_name,':',']'"; }

	int compare( const E* buf, const unsigned long buf_length )
	{
		return this->m_class_name->compare(buf,buf_length);
	}

	unsigned long matched_size() { return this->m_class_name->matched_size(); }

};

template<typename E>
class equivalence_class
{
	typedef typename basic_parser<E>::parser_list parser_list;

	class equivalence_base
	{
		protected:
			int m_size;
		public:
			equivalence_base() : m_size(0) {};
			int matched_size() { return m_size; }
	};

	class openequal_coll_elem_single_equalclose : public equivalence_base
	{
		basic_terminal<E, '['>	m_open;
		basic_terminal<E, '='>	m_left_equal;
		basic_except_terminal<E,meta_char_choice<E> >	
			m_coll_elem_single;
		basic_terminal<E, '='>	m_right_equal;
		basic_terminal<E, ']'>	m_close;
		public:

		void push_parsers( parser_list &l )
		{
			l.push_back(&this->m_open);
			l.push_back(&this->m_left_equal);
			l.push_back(&this->m_coll_elem_single);
			l.push_back(&this->m_right_equal);
			l.push_back(&this->m_close);
		}
		const char* get_name() const { return "'[','=',collating_elem_single,'=',']'"; }

		int compare( const E* buf, const unsigned long buf_length )
		{
			int ret = std::use_facet< std::collate<E> >(std::locale()).compare(
					buf,
					buf,
					m_coll_elem_single.get(),
					m_coll_elem_single.get() );
			if( ret == 0 )
				this->m_size = 1;
			else
				this->m_size = 0;
			return ret;
		}	
	};

	class openequal_coll_elem_multi_equalclose : public equivalence_base
	{
		typedef std::ctype_base ctypebase;
		basic_terminal<E, '['>	m_open;
		basic_terminal<E, '.'>	m_left_equal;
		basic_string_parser<E>	m_coll_elem_multi;
		basic_terminal<E, '.'>	m_right_equal;
		basic_terminal<E, ']'>	m_close;
		public:
		openequal_coll_elem_multi_equalclose() : 
			m_coll_elem_multi('.', basic_string_parser<E>::NONE) {};
		void push_parsers( parser_list &l )
		{
			l.push_back(&this->m_open);
			l.push_back(&this->m_left_equal);
			l.push_back(&this->m_coll_elem_multi);
			l.push_back(&this->m_right_equal);
			l.push_back(&this->m_close);
		}
		const char* get_name() const { return "'[','.',collating_elem_multi,'.',']'"; }

		int compare( const E* buf, const unsigned long buf_length )
		{
			unsigned long to_compare_length = 0;
			// check the size of element in buf  - try the longest
			while( to_compare_length < buf_length )
			{
				to_compare_length++;
				ctypebase::mask m = ctypebase::print;
				if( !std::use_facet< std::ctype<E> >(std::locale()).is( buf, 
							buf+to_compare_length, &m ) )
				{
					break;
				}

			}
			std::basic_string<E> multichar = *(m_coll_elem_multi.get());
			int ret = std::use_facet< std::collate<E> >(std::locale()).compare(
					buf,
					buf+to_compare_length,
					multichar.c_str(),
					multichar.c_str() + multichar.length() );
			this->m_size = to_compare_length;
			return ret;
		}	
	};
	int m_size;
	public:
	basic_non_terminal<E, openequal_coll_elem_single_equalclose> m_coll_elem_single;
	basic_non_terminal<E, openequal_coll_elem_multi_equalclose> m_multichar;
	public:
	equivalence_class() : 
		// CHANGE:: m_coll_elem_single(),
		// CHANGE:: m_multichar(),
		m_size(0) 
	{
	}

	int matched_size() 
	{
		return this->m_size; 
	}

	void push_parsers( parser_list &l )
	{
		l.push_back(&this->m_coll_elem_single);
		l.push_back(&this->m_multichar);
	}

	const char * get_name() const 
	{
		return "equivalence_class"; 
	}

	int compare( const E* buf, const unsigned long buf_length )
	{
		int ret = 0;
		if( this->m_coll_elem_single.is_parsed() )
		{
			ret = this->m_coll_elem_single->compare( buf, buf_length );
			this->m_size = this->m_coll_elem_single->matched_size();
		}
		else
		{
			ret = this->m_multichar->compare( buf, buf_length );
			this->m_size = this->m_multichar->matched_size();
		}
		return ret;
	}
};

template<typename E>
class single_expression
{
	typedef typename basic_parser<E>::parser_list parser_list;

	basic_choice<E,collating_choice<E> > m_collating_choice;
	basic_non_terminal<E,character_class<E> >	m_character_class;
	basic_choice<E,equivalence_class<E> > m_equivalence_class;

	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		int ret = 0;
		int size = 0;
		if( this->m_collating_choice.is_parsed() )
		{
			ret = this->m_collating_choice->compare(buf,buf_length);
			size = this->m_collating_choice->matched_size();
		}
		else if( this->m_character_class.is_parsed() )
		{
			ret = this->m_character_class->compare(buf,buf_length);
			size = this->m_character_class->matched_size();
		}
		else
		{
			ret = this->m_equivalence_class->compare(buf,buf_length);
			size = this->m_equivalence_class->matched_size();
		}
		if( ret == 0 )
			return size;
		else
			return 0;
	}
	public:
	single_expression() 
	{
	}

	virtual ~single_expression() 
	{
	}

	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_equivalence_class );
		l.push_back( &this->m_character_class );
		l.push_back( &this->m_collating_choice );
	}

	const char* get_name() const 
	{
		return "single_expression"; 
	}

};

template<typename E>
class start_range
{
	typedef typename basic_parser<E>::parser_list parser_list;

	basic_choice<E,collating_choice<E> >	m_collating_choice;
	basic_terminal<E,'-'>				m_hyphen;
	int m_size;
	public:
	int compare(const E* buf, const unsigned long buf_length)
	{
		// CHANGE:: int ret = this->m_collating_choice->compare(buf, buf_length);

		this->m_size = this->m_collating_choice->matched_size();

		// CHANGE:: 
		return 0;
	}

	int matched_size() 
	{
		return this->m_size; 
	}

	start_range() : m_size(0) 
	{
	}

	virtual ~start_range() 
	{
	}

	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_collating_choice );
		l.push_back( &this->m_hyphen );
	}

	const char* get_name() const 
	{
		return "collating_choice,-"; 
	}

};

template<typename E>
class range_expression
{
	typedef typename basic_parser<E>::parser_list parser_list;

	class start_range_and_end_range
	{
		basic_non_terminal<E,start_range<E> >	m_start_range;
		basic_choice<E,collating_choice<E> > m_end_range;

		public:
		unsigned long recognize( const E* buf, const unsigned long buf_length )
		{
			int start_ret = this->m_start_range->compare(buf,buf_length);
			int size = this->m_start_range->matched_size();
			while( start_ret < 0 && size > 1) 
			{
				size--;
				// while the buf is not higher as start range, try smaller -
				// collating symbol in buffer
				start_ret = this->m_start_range->compare(buf,size);
			}
			int end_ret = 0;
			if( start_ret > 0 )
			{
				end_ret = this->m_end_range->compare(buf,size);
				while( end_ret > 0 && 
						size > 1 && 
						this->m_start_range->compare(buf,size-1) >= 0 )
				{
					size--;
					end_ret = this->m_end_range->compare(buf,size);
				}
			}
			if( start_ret >=0 && end_ret <= 0 )
				return size;
			else
				return 0;
		}

		start_range_and_end_range() : m_start_range() {};
		~start_range_and_end_range() {};
		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_start_range );
			l.push_back( &this->m_end_range );
		}	
		const char* get_name() const { return "start_range,end_range"; }

	};

	class start_range_and_hyphen
	{
		basic_non_terminal<E,start_range<E> >	m_start_range;
		basic_terminal<E,'-'>	m_hyphen;

		public:
		unsigned long recognize( const E* buf, const unsigned long buf_length )
		{
			int start_ret = this->m_start_range->compare(buf,buf_length);
			int size = this->m_start_range->matched_size();
			while( start_ret < 0 && size > 1) 
			{
				size--;
				// while the buf is not higher as start range, try smaller -
				// collating symbol in buffer
				start_ret = this->m_start_range->compare(buf,size);
			}
			int end_ret = 0;
			if( start_ret > 0 )
			{
				end_ret = std::use_facet< std::collate<E> >(std::locale()).compare(
						buf,
						buf+size,
						this->m_hyphen.get(),
						this->m_hyphen.get()+1 );
				while( end_ret > 0 && 
						size > 1 && 
						this->m_start_range->compare(buf,size-1) >= 0 )
				{
					size--;
					end_ret = std::use_facet< std::collate<E> >(std::locale()).compare(
							buf,
							buf+size,
							this->m_hyphen.get(),
							this->m_hyphen.get()+1 );
				}
			}
			if( start_ret >=0 && end_ret <= 0 )
				return size;
			else
				return 0;
		}

		start_range_and_hyphen() : m_start_range() 
		{
		}

		virtual ~start_range_and_hyphen() 
		{
		}

		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_start_range );
			l.push_back( &this->m_hyphen );
		}

		const char* get_name() const 
		{
			return "start_range,-"; 
		}

	};	

	basic_non_terminal<E,start_range_and_end_range > m_start_end_range;
	basic_non_terminal<E,start_range_and_hyphen >	m_start_and_hyphen;

	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		if( this->m_start_end_range.is_parsed() )
		{
			return this->m_start_end_range->recognize(buf,buf_length);
		}
		else
		{
			return this->m_start_and_hyphen->recognize(buf,buf_length);
		}
	}
	range_expression() : m_start_end_range(), m_start_and_hyphen() {};

	~range_expression() {};
	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_start_end_range );
		l.push_back( &this->m_start_and_hyphen );
	}	
	const char* get_name() const { return "range_expression"; }

};

template<typename E>
class expression_term
{
	typedef typename basic_parser<E>::parser_list parser_list;
	basic_choice<E,single_expression<E> >	
		m_single_expression;
	basic_choice<E,range_expression<E> >		
		m_range_expression;

	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		if( this->m_single_expression.is_parsed() )
		{
			return this->m_single_expression->recognize(buf,buf_length);
		}
		else
		{
			return this->m_range_expression->recognize(buf,buf_length);
		}
	}
	public:
	expression_term() 
	{
	}

	virtual ~expression_term() 
	{
	}

	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_range_expression );
		l.push_back( &this->m_single_expression );
	}

	const char* get_name() const 
	{
		return "expression_term"; 
	}

};

template<typename E>
class follow_list
{
	typedef typename basic_parser<E>::parser_list parser_list;
	public:
	class follow_list_and_expression_term
	{
		basic_choice<E,follow_list<E> >			m_follow_list;
		basic_choice<E,expression_term<E> >		m_expression_term;

		public:
		unsigned long recognize( const E* buf, const unsigned long buf_length )
		{
			unsigned long size = this->m_follow_list->recognize(buf,buf_length);
			if( size == 0 )
			{
				size = this->m_expression_term->recognize(buf,buf_length);
			}
			return size;
		}

		follow_list_and_expression_term() 
		{
		}

		virtual ~follow_list_and_expression_term() 
		{
		}

		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_expression_term );
			l.push_back( &this->m_follow_list );
		}

		const char* get_name() const 
		{
			return "follow_list,expression_term"; 
		}

	};

	basic_choice<E,expression_term<E> > m_expression_term;
	basic_non_terminal<E,follow_list_and_expression_term > m_follow_and_expression;

	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		if( this->m_expression_term.is_parsed() )
		{
			return this->m_expression_term->recognize(buf,buf_length);
		}
		else
		{
			return this->m_follow_and_expression->recognize(buf,buf_length);
		}
	}

	public:
	follow_list() : m_follow_and_expression() 
	{
	}

	virtual ~follow_list() 
	{
	}

	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_follow_and_expression );
		l.push_back( &this->m_expression_term );
	}

	const char* get_name() const 
	{
		return "follow_list"; 
	}

};

template<typename E>
class bracket_list
{
	typedef typename basic_parser<E>::parser_list parser_list;

	class follow_list_and_hyphen
	{
		basic_choice<E,follow_list<E> >	m_follow_list;
		basic_terminal<E,'-'>		m_hyphen;

		public:
		unsigned long recognize( const E* buf, const unsigned long buf_length )
		{
			unsigned long size = this->m_follow_list->recognize(buf,buf_length);
			if( size == 0 )
			{
				if( buf[0] == this->m_hyphen.get_valid() )
				{
					size = 1;
				}
			}
			return size;
		}
		follow_list_and_hyphen() {};

		~follow_list_and_hyphen() {};
		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_follow_list );
			l.push_back( &this->m_hyphen );
		}	
		const char* get_name() const { return "follow_list,'-'"; }

	};

	class hyphen_and_follow_list
	{
		basic_terminal<E,'-'>					m_hyphen;
		basic_choice<E,follow_list<E> >			m_follow_list;
		public:
		unsigned long recognize( const E* buf, const unsigned long buf_length )
		{
			if( buf[0] == this->m_hyphen.get_valid() )
			{
				return 1;
			} 
			else
			{
				return this->m_follow_list->recognize(buf,buf_length);
			}
		}
		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_hyphen );
			l.push_back( &this->m_follow_list );
		}	
		const char* get_name() const { return "'-',follow_list,-"; }
	};

	class hyphen_hyphen_end_range
	{
		protected:
			basic_terminal<E,'-'>					m_hyphen;
			basic_terminal<E,'-'>					m_sep;
			basic_choice<E,collating_choice<E> >		m_end_range;
		public:
			virtual ~hyphen_hyphen_end_range()
			{
			}

			virtual unsigned long recognize( const E* buf, const unsigned long buf_length )
			{
				std::locale l;

				int end_ret = this->m_end_range->compare(buf,buf_length);
				int size = this->m_end_range->matched_size();
				while( end_ret > 0 && size > 1) 
				{
					size--;
					// while the buf is not higher as start range, try smaller -
					// collating symbol in buffer
					end_ret = this->m_end_range->compare(buf,size);
				}
				int start_ret = 0;
				if( end_ret < 0 )
				{
					start_ret = std::use_facet<std::collate<E> >(l).compare(buf, buf+size, this->m_hyphen.get(), this->m_hyphen.get()+1);
					while( start_ret < 0 && 
							size > 1 && 
							this->m_end_range->compare(buf,size-1) <= 0 )
					{
						size--;
						start_ret = std::use_facet<std::collate<E> >(l).compare(buf, buf+size, this->m_hyphen.get(), this->m_hyphen.get()+1);
					}
				}
				if( start_ret >=0 && end_ret <= 0 )
					return size;
				else
					return 0;
			}
			virtual void push_parsers( parser_list &l )
			{
				l.push_back( &this->m_hyphen );
				l.push_back( &this->m_sep );
				l.push_back( &this->m_end_range );
			}	
			const char* get_name() const { return "'-','-',collating_choice"; }
	};

	class hyphen_hyphen_endrange_follow_list : public hyphen_hyphen_end_range 
	{
		protected:
			basic_choice<E,follow_list<E> >	m_follow_list;
		public:
			virtual ~hyphen_hyphen_endrange_follow_list()
			{
			}

			virtual unsigned long recognize( const E* buf, const unsigned long buf_length )
			{
				int ret = hyphen_hyphen_end_range::recognize(buf,buf_length);
				if( ret == 0 )
				{
					return this->m_follow_list->recognize(buf,buf_length);
				} 
				else
				{
					return ret;
				}
			}

			void push_parsers( parser_list &l )
			{
				hyphen_hyphen_end_range::push_parsers(l);
				l.push_back( &this->m_follow_list );
			}

			const char* get_name() const 
			{
				return "'-','-',collating_choice,follow_list"; 
			}
	};

	class right_bracket_follow_list
	{
		protected:
			basic_terminal<E,']'>						m_right_bracket;
			basic_choice<E,follow_list<E> >			m_follow_list;
		public:
			virtual ~right_bracket_follow_list()
			{
			}

			virtual unsigned long recognize( const E* buf, const unsigned long buf_length )
			{
				if( buf_length > 0 && buf[0] == ']' )
				{
					return this->m_follow_list->recognize(buf+1,buf_length-1);
				}
				else
				{
					return 0;
				}
			}
			void push_parsers( parser_list &l )
			{
				l.push_back( &this->m_right_bracket );
				l.push_back( &this->m_follow_list );
			}	
			const char* get_name() const { return "']',follow_list"; }
	};
	basic_choice<E,follow_list<E> > m_follow_list;
	basic_non_terminal<E,follow_list_and_hyphen >	m_follow_list_hyphen;
	basic_non_terminal<E,hyphen_and_follow_list >	m_hyphen_follow_list;
	basic_non_terminal<E,hyphen_hyphen_end_range>	m_hyphen_hyphen_end_range;
	basic_non_terminal<E,hyphen_hyphen_endrange_follow_list> m_hyphen_hyphen_endrange_follow;
	basic_non_terminal<E,right_bracket_follow_list> m_right_bracket_follow_list;

	basic_terminal<E,']'>	m_right_bracket;

	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		if( this->m_follow_list.is_parsed() )
		{
			return this->m_follow_list->recognize(buf,buf_length);
		}
		else if( this->m_follow_list_hyphen.is_parsed() )
		{
			return this->m_follow_list_hyphen->recognize(buf,buf_length);
		} 
		else if( this->m_hyphen_follow_list.is_parsed() )
		{
			return this->m_hyphen_follow_list->recognize(buf,buf_length);
		}
		else if( this->m_hyphen_hyphen_end_range.is_parsed() )
		{
			return this->m_hyphen_hyphen_end_range->recognize(buf,buf_length);
		}
		else if( this->m_hyphen_hyphen_endrange_follow.is_parsed() )
		{
			return this->m_hyphen_hyphen_endrange_follow->recognize(buf,buf_length);
		}
		else if( this->m_right_bracket_follow_list.is_parsed() )
		{
			return this->m_right_bracket_follow_list->recognize(buf,buf_length);
		}
		else
		{
			return buf_length > 0 && buf[0] == ']';
		}
	}

	bracket_list() : 
		m_follow_list_hyphen(),
		m_hyphen_follow_list(),
		m_hyphen_hyphen_end_range(),
		m_hyphen_hyphen_endrange_follow(),
		m_right_bracket_follow_list() {}; 

	virtual ~bracket_list() 
	{
	}

	void push_parsers( parser_list &l )
	{
		/*
		   l.push_back( &m_follow_list );
		   l.push_back( &this->m_follow_list_hyphen );
		   l.push_back( &this->m_hyphen_follow_list );
		   l.push_back( &this->m_hyphen_hyphen_end_range );
		   l.push_back( &this->m_hyphen_hyphen_endrange_follow );
		   */
		l.push_back( &this->m_hyphen_hyphen_endrange_follow );
		l.push_back( &this->m_hyphen_hyphen_end_range );
		l.push_back( &this->m_hyphen_follow_list );
		l.push_back( &this->m_follow_list_hyphen );
		l.push_back( &this->m_follow_list );
		l.push_back( &this->m_right_bracket_follow_list );
		l.push_back( &this->m_right_bracket );
	}	

	const char* get_name() const 
	{ 
		return "bracket_list"; //"follow_list|follow_list,'-'|'-',follow_list|'-','-',end_range|'-','-',collating_choice,follow_list"; 
	}

};

template<typename E>
class nonmatching_list
{
	typedef typename basic_parser<E>::parser_list parser_list;
	basic_terminal<E, '^'>				m_circumflex;
	basic_choice<E, bracket_list<E> >	m_bracket_list;
	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		// not recognized
		if( this->m_bracket_list->recognize(buf,buf_length) == 0 )
		{
			return 1; // TODO - if collating element size is more then 1?
		}

		return 0;
	}

	nonmatching_list() 
	{
	}

	virtual ~nonmatching_list() 
	{
	}

	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_circumflex );
		l.push_back( &this->m_bracket_list );
	}	

	const char* get_name() const 
	{
		return "nonmatching_list"; 
	}


};

template<typename E>
class matching_list
{
	typedef typename basic_parser<E>::parser_list parser_list;
	basic_choice<E,bracket_list<E> >	m_bracket_list;
	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		return this->m_bracket_list->recognize(buf,buf_length);
	}

	matching_list() 
	{
	}

	virtual ~matching_list() 
	{
	}

	void push_parsers( parser_list &l )
	{
		l.push_back( &this->m_bracket_list );
	}

	const char* get_name() const 
	{
		return "matching_list"; 
	}

};

template<typename E>
class bracket_expr
{
	typedef typename basic_parser<E>::parser_list parser_list;

	class left_bracket_nonmatching_list_right_bracket{
		basic_terminal<E,'['>	m_left_bracket;
		basic_non_terminal<E,nonmatching_list<E> > m_nonmatching_list;
		basic_terminal<E,']'> m_right_bracket;
		public:
		unsigned long recognize( const E* buf, const unsigned long buf_length )
		{
			return this->m_nonmatching_list->recognize(buf,buf_length);
		}

		left_bracket_nonmatching_list_right_bracket() :
			m_nonmatching_list() 
		{
		}

		virtual ~left_bracket_nonmatching_list_right_bracket() 
		{
		}

		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_left_bracket );
			l.push_back( &this->m_nonmatching_list );
			l.push_back( &this->m_right_bracket );
		}

		const char* get_name() const 
		{
			return "[nonmatching_list]"; 
		}
	};

	class left_bracket_matching_list_rigth_bracket
	{
		basic_terminal<E,'['>					m_left_bracket;
		basic_non_terminal<E,matching_list<E> >	m_matching_list;
		basic_terminal<E,']'>					m_right_bracket;
		public:
		unsigned long recognize( const E* buf, const unsigned long buf_length )
		{
			return this->m_matching_list->recognize(buf,buf_length);
		}
		public:
		left_bracket_matching_list_rigth_bracket() :
			m_matching_list() 
		{
		}

		virtual ~left_bracket_matching_list_rigth_bracket() 
		{
		}

		void push_parsers( parser_list &l )
		{
			l.push_back( &this->m_left_bracket );
			l.push_back( &this->m_matching_list );
			l.push_back( &this->m_right_bracket );
		}

		const char* get_name() const 
		{
			return "[matching_list]"; 
		}

	};	

	basic_non_terminal<E,left_bracket_matching_list_rigth_bracket > m_matching;
	basic_non_terminal<E,left_bracket_nonmatching_list_right_bracket > m_nonmatching;

	public:
	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		if( this->m_matching.is_parsed() )
		{
			return this->m_matching->recognize(buf,buf_length);
		}
		else
		{
			return this->m_nonmatching->recognize(buf,buf_length);
		}
	}

	bracket_expr() :
		m_matching(),
		m_nonmatching() 
	{
	}

	virtual ~bracket_expr() 
	{
	}

	void push_parsers( parser_list &l )
	{
		// l.push_back( &m_matching );
		// l.push_back( &m_nonmatching );
		l.push_back( &this->m_nonmatching );
		l.push_back( &this->m_matching );
	}	

	const char* get_name() const 
	{
		return "[matching_list]|[nonmatching_list]"; 
	}

};

template<typename E>
class special_chars
{
	typedef typename basic_parser<E>::parser_list parser_list;

	basic_terminal<E,'.'>	m_period;
	basic_terminal<E,'['>	m_left_bracket;
	basic_terminal<E,'\\'>	m_backslash;
	basic_terminal<E,'('>	m_left_parenthesis;
	basic_terminal<E,')'>	m_right_parenthesis;
	basic_terminal<E,'*'>	m_asterix;
	basic_terminal<E,'+'>	m_plus;
	basic_terminal<E,'?'>	m_question_mark;
	basic_terminal<E,'{'>	m_left_brace;
	basic_terminal<E,'}'>	m_right_brace;
	basic_terminal<E,'|'>	m_vertical_line;
	basic_terminal<E,'^'>	m_circumflex;
	basic_terminal<E,'$'>	m_dollar;

	public:
	special_chars() 
	{
	}

	virtual ~special_chars() 
	{
	}

	void push_parsers( parser_list &l )
	{    
		l.push_back(&this->m_period);
		l.push_back(&this->m_left_bracket);
		l.push_back(&this->m_backslash);
		l.push_back(&this->m_left_parenthesis);
		l.push_back(&this->m_right_parenthesis);
		l.push_back(&this->m_asterix);
		l.push_back(&this->m_plus);
		l.push_back(&this->m_question_mark);
		l.push_back(&this->m_left_brace);
		l.push_back(&this->m_right_brace);
		l.push_back(&this->m_vertical_line);
		l.push_back(&this->m_circumflex);
		l.push_back(&this->m_dollar);
	}

	const char * get_name() const 
	{
		return "special_chars"; 
	}


	unsigned long recognize( const E* buf, const unsigned long buf_length )
	{
		E c=0;
		if( this->m_period.is_parsed() ) c = this->m_period.get_valid();
		else if( this->m_left_bracket.is_parsed() ) c = this->m_left_bracket.get_valid();
		else if( this->m_backslash.is_parsed() ) c = this->m_backslash.get_valid();
		else if( this->m_left_parenthesis.is_parsed() ) c = this->m_left_parenthesis.get_valid();
		else if( this->m_right_parenthesis.is_parsed() ) c = this->m_right_parenthesis.get_valid();
		else if( this->m_asterix.is_parsed() ) c = this->m_asterix.get_valid();
		else if( this->m_plus.is_parsed() ) c = this->m_plus.get_valid();
		else if( this->m_question_mark.is_parsed() ) c = this->m_question_mark.get_valid();
		else if( this->m_left_brace.is_parsed() ) c = this->m_left_brace.get_valid();
		else if( this->m_right_brace.is_parsed() ) c = this->m_right_brace.get_valid();
		else if( this->m_vertical_line.is_parsed() ) c = this->m_vertical_line.get_valid();
		else if( this->m_circumflex.is_parsed() ) c = this->m_circumflex.get_valid();
		else if( this->m_dollar.is_parsed()) c = this->m_dollar.get_valid();

		if( c == buf[0] ) return 1;
		else return 0;
	}

};

/*!
  The one of classes implementing following regular expression 
grammar:\n \n
extended_reg_exp	=> ere_branch '|' extended_reg_exp | extended_reg_exp \n
ere_branch			=> ere_expression ere_branch | ere_expression \n
ere_expression		=> ere_expression_and_dups | one_char_or_coll_elem_ere | 
'^' | '$' | '('extended_reg_exp')' \n
ere_expression_and_dups	=> ere_expression_no_dup ere_dups \n
ere_expression_no_dup	=> one_char_or_coll_elem_ere | '^' | '$' 
| '('extended_reg_exp')' \n
ere_dups			=> ere_dupl_symbol ere_dups | ere_dupl_symbol \n
ere_dupl_symbol		=> '*' | '+' | '?' | '{' number '}] | '{' number ',' '}' |
'{' number ',' number '}' \n
one_char_or_coll_elem_ere => 'all except special chars' | '\' special_chars
'.' | bracket_expr \n
special_chars		=> '.' | '[' | '\' | '(' | ')' | '*' | '+' | '?' | '{' 
| '}' | '|' | '^' | '$' \n
*/
template<typename E>
class one_char_or_coll_elem_ere
{
	typedef typename basic_parser<E>::parser_list parser_list;
	typedef bracket_expr<E>	bracket_expr_type;
	typedef collating_symbol<E>	collating_symbol_type;

	class quoted_char
	{
		basic_terminal<E,'\\'>			m_backslash;
		basic_choice<E, special_chars<E> >
			m_spec_chars;
		public:
		quoted_char() 
		{
		}

		virtual ~quoted_char() 
		{
		}

		void push_parsers( parser_list &l )
		{
			l.push_back(&this->m_backslash);
			l.push_back(&this->m_spec_chars);
		}
		const char* get_name() const { return "\\special_chars"; }

		unsigned long recognize( const E* buf, 
				const unsigned long buf_length )
		{
			return this->m_spec_chars->recognize(buf,buf_length);
		}
	};
	basic_except_terminal<E,special_chars<E> >
		m_ord_char;
	basic_non_terminal<E,quoted_char>	m_quoted_char;
	basic_terminal<E,'.'>				m_dot;
	basic_choice<E, bracket_expr_type >	m_bracket_expr;
	public:
	one_char_or_coll_elem_ere() 
	{
	}

	~one_char_or_coll_elem_ere() 
	{
	}

	void push_parsers( parser_list &l )
	{
		l.push_back(&this->m_ord_char);
		l.push_back(&this->m_quoted_char);
		l.push_back(&this->m_dot);
		l.push_back(&this->m_bracket_expr);
	}

	const char* get_name() const 
	{
		return "one_char|collating_element"; 
	}

	unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset )
	{
		if( this->m_ord_char.is_parsed() )
		{
			if( this->m_ord_char.get_valid() == buf[buf_offset] )
			{
				return collating_symbol_type::get_symbol_len(buf+buf_offset, buf_length-buf_offset);
			}
			else
			{
				return 0;
			}
		} 
		else if( this->m_quoted_char.is_parsed() )
		{      
			return this->m_quoted_char->recognize( buf+buf_offset, buf_length-buf_offset );
		}
		else if( this->m_dot.is_parsed() )
		{
			return collating_symbol_type::get_symbol_len( buf+buf_offset, buf_length-buf_offset );
		}
		else
		{
			return this->m_bracket_expr->recognize( buf+buf_offset, buf_length-buf_offset );
		}
	}
};

/*
   The one of classes implementing following regular expression 
grammar:\n \n
extended_reg_exp	=> ere_branch '|' extended_reg_exp | extended_reg_exp \n
ere_branch			=> ere_expression ere_branch | ere_expression \n
ere_expression		=> ere_expression_and_dups | one_char_or_coll_elem_ere | 
'^' | '$' | '('extended_reg_exp')' \n
ere_expression_and_dups	=> ere_expression_no_dup ere_dups \n
ere_expression_no_dup	=> one_char_or_coll_elem_ere | '^' | '$' 
| '('extended_reg_exp')' \n
ere_dups			=> ere_dupl_symbol ere_dups | ere_dupl_symbol \n
ere_dupl_symbol		=> '*' | '+' | '?' | '{' number '}] | '{' number ',' '}' |
'{' number ',' number '}' \n
one_char_or_coll_elem_ere => 'all except special chars' | '\' special_chars
'.' | bracket_expr \n
special_chars		=> '.' | '[' | '\' | '(' | ')' | '*' | '+' | '?' | '{' 
| '}' | '|' | '^' | '$' \n
*/
template<typename E>
class ere_expression : public ere_base<E>
{
	typedef typename basic_parser<E>::parser_list parser_list;
	class subexpression : public ere_base<E>
	{
		basic_terminal<E,'('>				m_left_paren;
		basic_choice<E,extended_reg_exp<E> >	m_extened_regexp;
		basic_terminal<E,')'>				m_right_paren;
		public:
		subexpression() : ere_base<E>() 
		{
		}

		virtual ~subexpression() 
		{
		}

		unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions = true );
		void assign_matches( matches::match_key key, unsigned long& branch_pos, matches& m );

		void push_parsers( parser_list& l );
		const char* get_name() const 
		{
			return "(subexpression)"; 
		}

	};

	class ere_expression_no_dup : public ere_expression<E>
	{
		public:

			void push_parsers( parser_list& l )
			{
				l.push_back( &this->m_one_elem );
				l.push_back( &this->m_left_anchor );
				l.push_back( &this->m_right_anchor );
				l.push_back( &this->m_subexpression );
			}	
			const char* get_name() const { return "one_char_or_coll_elem_ere | '^' | '$' | (subexpression)"; }
	};

	class ere_expression_and_dupl : public ere_base<E>
	{
		basic_choice<E,ere_expression_no_dup > m_ere;
		basic_choice<E, ere_dups<E> > m_dup;
		matches	m_matches;

		unsigned long divide_and_recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, unsigned long rep, bool assign_matches );
		public: 
		ere_expression_and_dupl() : ere_base<E>() 
		{
		}

		virtual ~ere_expression_and_dupl() 
		{
		}

		unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions = true );
		void assign_matches( matches::match_key key, unsigned long& branch_pos, matches& m );
		void push_parsers( parser_list& l );
		const char* get_name() const 
		{
			return "ere_expression,dupl"; 
		}

		bool is_lazy() 
		{
			return this->m_dup.is_parsed() && this->m_dup->is_lazy(); 
		}

	};

	basic_choice<E,one_char_or_coll_elem_ere<E> > m_one_elem;
	basic_terminal<E,'^'> m_left_anchor;
	basic_terminal<E,'$'> m_right_anchor;
	basic_non_terminal<E,subexpression> m_subexpression;
	basic_non_terminal<E,ere_expression_and_dupl> m_ere_expr_and_dup;
	public:

	ere_expression() : 
		ere_base<E>(),
		m_subexpression(),
		m_ere_expr_and_dup()
	{
	}

	virtual ~ere_expression() 
	{
	}

	bool is_right_anchor() 
	{
		return this->m_right_anchor.is_parsed(); 
	}

	bool is_left_anchor() 
	{
		return this->m_left_anchor.is_parsed(); 
	}

	bool is_subexpression() 
	{
		return this->m_subexpression.is_parsed(); 
	}

	bool is_lazy()
	{ 
		return this->m_ere_expr_and_dup.is_parsed() && this->m_ere_expr_and_dup->is_lazy(); 
	}

	unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions = true );
	void assign_matches( matches::match_key key, unsigned long& branch_pos, matches& m );
	void push_parsers( parser_list& l );
	const char* get_name() const 
	{ 
		return "ere_expression"; 
	}


};

template< typename E > class ere_expression;

/*!
  The one of classes implementing following regular expression 
grammar:\n \n
extended_reg_exp	=> ere_branch '|' extended_reg_exp | extended_reg_exp \n
ere_branch			=> ere_expression ere_branch | ere_expression \n
ere_expression		=> ere_expression_and_dups | one_char_or_coll_elem_ere | 
'^' | '$' | '('extended_reg_exp')' \n
ere_expression_and_dups	=> ere_expression_no_dup ere_dups \n
ere_expression_no_dup	=> one_char_or_coll_elem_ere | '^' | '$' 
| '('extended_reg_exp')' \n
ere_dups			=> ere_dupl_symbol ere_dups | ere_dupl_symbol \n
ere_dupl_symbol		=> '*' | '+' | '?' | '{' number '}] | '{' number ',' '}' |
'{' number ',' number '}' \n
one_char_or_coll_elem_ere => 'all except special chars' | '\' special_chars
'.' | bracket_expr \n
bracket_expr		=> '[' matching_list ']' | '[' non_matching_list ']'
\n
*/
template<typename E>
class ere_branch : public ere_base<E>
{
	typedef typename basic_parser<E>::parser_list 
		parser_list;

	class ere_expression_ere_branch : public ere_base<E>
	{
		basic_choice<E, ere_expression<E> >	m_ere_expression;	
		basic_choice<E, ere_branch<E> >		m_ere_branch;
		public:	
		ere_expression_ere_branch() : ere_base<E>() {};
		~ere_expression_ere_branch() {};

		unsigned long recognize( const E* buf, 
				const unsigned long buf_length, 
				const unsigned long buf_offset,
				bool try_positions = true );
		void push_parsers( parser_list& l );
		const char* get_name() const { return "ere_expression,ere_branch"; }

		void assign_matches( matches::match_key parent_address, 
				unsigned long& branch_pos, 
				matches& m );
	};

	basic_choice<E, ere_expression<E> >	m_ere;
	basic_non_terminal<E,ere_expression_ere_branch> m_ere_expression_ere_branch;

	public:
	ere_branch() :
		ere_base<E>(), 
		m_ere_expression_ere_branch() 
	{
	}

	virtual ~ere_branch() 
	{
	}

	unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions = true );

	void assign_matches( matches::match_key parent_address, unsigned long& branch_pos, matches& m );

	bool is_right_anchor()
	{
		return this->m_ere.is_parsed() && this->m_ere->is_right_anchor();
	}

	void push_parsers( parser_list& l );
	const char* get_name() const 
	{
		return "ere_branch"; 
	}

};

/*
   unsigned long collating_symbol<char>::get_symbol_len( const char* buf, const unsigned long buf_length )
   {
// TODO - don't know how to test this - when mbrlen returns value > 1?
// I tested with setting the diferent locales and sending digraph values (like ch for 
// czech locale, but never get value > 1
//std::mbstate_t state;
//std::memset( &state,'\0',sizeof(state) );
return 1;
int len = mblen( buf, buf_length );
if( len < 0 ) 
{
len = 0;
}
return (unsigned long)len;
}

unsigned long collating_symbol<wchar_t>::get_symbol_len( const wchar_t* buf,
const unsigned long buf_length )
{
// TODO - check, if this is OK
if( buf_length > 0 )
return 1;
else
return 0;
}
*/

/*!
  The one of classes implementing following regular expression 
grammar:\n \n
extended_reg_exp	=> ere_branch '|' extended_reg_exp | extended_reg_exp \n
ere_branch			=> ere_expression ere_branch | ere_expression \n
ere_expression		=> ere_expression_and_dups | one_char_or_coll_elem_ere | 
'^' | '$' | '('extended_reg_exp')' \n
ere_expression_and_dups	=> ere_expression_no_dup ere_dups \n
ere_expression_no_dup	=> one_char_or_coll_elem_ere | '^' | '$' 
| '('extended_reg_exp')' \n
ere_dups			=> ere_dupl_symbol ere_dups | ere_dupl_symbol \n
ere_dupl_symbol		=> '*' | '+' | '?' | '{' number '}] | '{' number ',' '}' |
'{' number ',' number '}' \n
one_char_or_coll_elem_ere => 'all except special chars' | '\' special_chars
'.' | bracket_expr \n
special_chars		=> '.' | '[' | '\' | '(' | ')' | '*' | '+' | '?' | '{' 
| '}' | '|' | '^' | '$' \n
*/
template<typename E>
class extended_reg_exp : public ere_base<E>
{
	typedef typename basic_parser<E>::parser_list parser_list;

	class ere_branch_or_extended_reg_exp : public ere_base<E>
	{
		basic_choice<E, ere_branch<E> >	m_branch;
		basic_terminal<E,'|'>			m_or;
		basic_choice<E, extended_reg_exp<E> >	m_ext_regexp;
		public:
		ere_branch_or_extended_reg_exp() : ere_base<E>() 
		{
		}

		~ere_branch_or_extended_reg_exp() 
		{
		}

		unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions = true );
		void assign_matches( matches::match_key parent_address, unsigned long& branch_pos, matches& m );
		void push_parsers( parser_list& l );
		const char* get_name() const 
		{
			return "ere_branch|ere"; 
		}

	};

	basic_choice<E, ere_branch<E> >	m_branch;
	basic_non_terminal<E, ere_branch_or_extended_reg_exp > m_branch_or_ext_regexp;

	public:

	extended_reg_exp() : 
		ere_base<E>(), 
		m_branch_or_ext_regexp() 
	{
	}

	virtual ~extended_reg_exp() 
	{
	}

	unsigned long recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions = true  );
	void assign_matches( matches::match_key parent_address, unsigned long& branch_pos, matches& m );

	void push_parsers( parser_list& l );
	const char* get_name() const 
	{
		return "ere"; 
	}

};

	template< typename E > 
unsigned long ere_expression<E>::subexpression::recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions )
{
	this->m_extened_regexp->recognize( buf, buf_length, buf_offset );
	this->m_is_rec = this->m_extened_regexp->is_recognized();
	if( this->m_is_rec )
	{
		this->m_rec_size = this->m_extened_regexp->recognize( buf, buf_length, buf_offset );
		this->m_rec_pos = this->m_extened_regexp->recognized_position();
	}
	return this->m_rec_size;
}

	template< typename E > 
void ere_expression<E>::subexpression::assign_matches( matches::match_key key, unsigned long& branch_pos, matches& m )
{
	key += '.';
	key += this->to_str(branch_pos);
	this->m_extened_regexp->assign_matches( key, branch_pos, m );
}


	template< typename E > 
void ere_expression<E>::subexpression::push_parsers( parser_list& l )
{
	l.push_back( &this->m_left_paren );
	l.push_back( &this->m_extened_regexp );
	l.push_back( &this->m_right_paren );
}

	template< typename E > 
unsigned long ere_expression<E>::ere_expression_and_dupl::divide_and_recognize(	const E* buf, const unsigned long buf_length, const unsigned long buf_offset, unsigned long rep, bool assign_matches )
{
	this->m_is_rec = true;
	if( rep == this->m_dup->get_max() )
	{
		this->m_dup->set_recognized_dup(rep);
		return 0;
	}
	unsigned long best_left_side_buf_len = 0;
	unsigned long best_rec_size = 0;
	unsigned long left_side_buf_len = buf_length;
	unsigned long left_rec_size = 0;
	while( this->m_is_rec )
	{
		this->m_ere->recognize( buf, left_side_buf_len, buf_offset, true );
		if( this->m_ere->is_recognized() )
		{
			left_rec_size = this->m_ere->recognized_size();
			// m_ere->recognized_position() is always same as buf_offset
			this->m_rec_size = divide_and_recognize( 
					buf, 
					buf_length, 
					buf_offset + left_rec_size,
					rep+1,
					false );
			if( this->m_is_rec )
			{
				// previous method succeeded
				if( best_rec_size < this->m_rec_size + left_rec_size )
				{
					best_rec_size = this->m_rec_size + left_rec_size;
					best_left_side_buf_len = left_side_buf_len;
				}	
			}
			left_side_buf_len = buf_offset + left_rec_size - 1;
			this->m_is_rec = true;
		}
		else
		{
			this->m_is_rec = false;
		}
	}

	if( best_rec_size > 0 )
	{
		if( assign_matches )
		{
			this->m_ere->recognize( buf, best_left_side_buf_len, buf_offset, true );
			unsigned long branch_pos = rep;
			this->m_ere->assign_matches( "", branch_pos, this->m_matches );

			this->m_rec_size = 0;
			left_rec_size = this->m_ere->recognized_size();
			divide_and_recognize( 
					buf, 
					buf_length, 
					buf_offset + left_rec_size,
					rep+1,
					true );
			this->m_rec_size += left_rec_size;
		}
		else
		{
			this->m_is_rec = true;
			this->m_rec_size = best_rec_size;
		}
	}
	else if( rep >= this->m_dup->get_min() ) // the ere was recognized for no buf_length
	{
		this->m_rec_size = 0;
		this->m_is_rec = true;
	}
	else
	{
		this->m_is_rec = false;
	}
	return this->m_rec_size;	
}


	template< typename E > 
unsigned long ere_expression<E>::ere_expression_and_dupl::recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions )
{
	unsigned long rep = 1;

	if( this->m_ere->is_subexpression() )
	{
		this->m_matches.clear();
		// different handling for subexpressions - need to try different 
		// splits on buffer, so expressions like ((ab){1,3}){5} match the 
		// input like "ababababab"
		// --- recursive calls
		divide_and_recognize( buf, 
				buf_length,
				buf_offset,
				0,
				true );
		this->m_rec_pos = buf_offset;
		return this->m_rec_size;
	}

	// else the ere is not subexpression - can be made faster then
	this->m_ere->recognize( buf, buf_length, buf_offset, try_positions );
	this->m_is_rec = this->m_ere->is_recognized();
	this->m_rec_pos = this->m_ere->recognized_position();
	this->m_rec_size = this->m_ere->recognized_size();

	if( this->m_ere->is_recognized() && this->m_ere->is_subexpression() )
	{
		unsigned long branch_pos = 0;
		this->m_ere->assign_matches( "", branch_pos, this->m_matches );
	}

	while( this->m_is_rec && rep < this->m_dup->get_max() )
	{
		this->m_ere->recognize( buf, 
				buf_length,
				this->m_rec_pos+this->m_rec_size,
				false ); // don't try next positions
		if( this->m_ere->is_recognized() )
		{
			this->m_rec_size += this->m_ere->recognized_size();
			rep++;
		}
		else if( rep < this->m_dup->get_min() )
		{
			this->m_is_rec = false;
			this->m_rec_pos = 0;
			this->m_rec_size = 0;
			this->m_matches.clear(); // negative return
		}
		else
		{
			break; // positive return
		}
	}
	this->m_dup->set_recognized_dup( rep );
	return this->m_rec_size;	
}

	template< typename E > 
void ere_expression<E>::ere_expression_and_dupl::assign_matches( matches::match_key key, unsigned long& branch_pos, matches& m )
{
	if( this->m_ere->is_subexpression() )
	{
		// in this case matches are in private collection, don't need 
		// to assign in loop
		m.extend( key, this->m_matches );
	}
	else 
	{
		for( int i = 0; i < (int)this->m_dup->get_recognized_dup(); i++ )
		{
			this->m_ere->assign_matches( key, branch_pos, m );
		}
	}
}

	template< typename E > 
void ere_expression<E>::ere_expression_and_dupl::push_parsers( parser_list& l )
{
	l.push_back( &this->m_ere );
	l.push_back( &this->m_dup );
}

	template<typename E>
unsigned long ere_expression<E>::recognize(const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions)	
{
	this->m_is_rec = false;
	this->m_rec_pos = buf_offset;
	this->m_rec_size = 0;

	if( this->m_one_elem.is_parsed() )
	{
		while( !this->m_is_rec && this->m_rec_pos < buf_length )
		{
			this->m_rec_size = this->m_one_elem->recognize( buf, buf_length, this->m_rec_pos );
			this->m_is_rec = this->m_rec_size > 0;
			if( this->m_is_rec )
			{
				// this->m_rec_size = this->m_one_elem->recognized_size();
			}
			else if( try_positions )
			{
				this->m_rec_pos++;
			}
			else
			{
				break;
			}
		}
	}
	else if( this->m_left_anchor.is_parsed() || this->m_right_anchor.is_parsed() )
	{
		this->m_is_rec = true;
	} 
	else if( this->m_ere_expr_and_dup.is_parsed() )
	{
		this->m_ere_expr_and_dup->recognize( buf, buf_length, buf_offset, try_positions );
		this->m_is_rec = this->m_ere_expr_and_dup->is_recognized() ;
		if( this->m_is_rec )
		{
			this->m_rec_pos = this->m_ere_expr_and_dup->recognized_position();
			this->m_rec_size = this->m_ere_expr_and_dup->recognized_size();
		}
	}
	else
	{
		this->m_subexpression->recognize( buf, buf_length, buf_offset );
		this->m_is_rec = this->m_subexpression->is_recognized();
		// Dont check in
		unsigned long off = buf_offset;
		while( !this->m_is_rec && off < buf_length )
		{
			off++;
			this->m_subexpression->recognize( buf, buf_length, off );
			this->m_is_rec = this->m_subexpression->is_recognized();
		}

		this->m_rec_size = this->m_subexpression->recognized_size() + 
			this->m_subexpression->recognized_position() -
			off;
		this->m_rec_pos = off;
	}
	return this->m_rec_size;
}

	template<typename E>
void ere_expression<E>::assign_matches( matches::match_key key, unsigned long& branch_pos, matches& m )
{
	if( this->m_subexpression.is_parsed() )
	{
		branch_pos++;
		this->m_subexpression->assign_matches( key, branch_pos, m );
	} 
	else if( this->m_ere_expr_and_dup.is_parsed() )
	{
		this->m_ere_expr_and_dup->assign_matches( key, branch_pos, m );
	}
}

	template<typename E>
void ere_expression<E>::push_parsers( parser_list& l )
{
	l.push_back( &this->m_ere_expr_and_dup );
	l.push_back( &this->m_one_elem );
	l.push_back( &this->m_left_anchor );
	l.push_back( &this->m_right_anchor );
	l.push_back( &this->m_subexpression );
}

	template<typename E>
unsigned long ere_branch<E>::ere_expression_ere_branch::recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions )
{
	this->m_is_rec = false;
	this->m_rec_pos = 0;
	this->m_rec_size = 0;
	if( this->m_ere_branch->is_right_anchor() )
	{
		// different processing for this kind of expression - no splitting retries
		this->m_ere_expression->recognize( buf, buf_length, buf_offset );
		if( this->m_ere_expression->is_recognized() )
		{
			this->m_rec_size = this->m_ere_expression->recognized_size();
			this->m_rec_pos = this->m_ere_expression->recognized_position();
			this->m_is_rec = ((this->m_rec_pos + this->m_rec_size) == buf_length);
			if( !this->m_is_rec )
			{
				this->m_rec_pos = 0;
				this->m_rec_size = 0;
			}
		}
		return this->m_rec_size;
	}

	if( this->m_ere_expression->is_left_anchor() )
	{
		this->m_ere_branch->recognize( buf, buf_length, buf_offset, true );
		this->m_is_rec = this->m_ere_branch->is_recognized() &&
			(this->m_ere_branch->recognized_position() == buf_offset);
		if( this->m_is_rec )
		{
			this->m_rec_pos = buf_offset;
			this->m_rec_size = this->m_ere_branch->recognized_size();
		}
		return this->m_rec_size;
	}


	unsigned long buf_split = 0;
	unsigned long longest_recognized_size = 0;
	unsigned long buf_split_for_shortest_size = 0;
	unsigned long shortest_left_recognized_size = INT_MAX;
	unsigned long buf_split_for_longest_size = buf_split;
	bool left_side_recognized = true;
	this->m_is_rec = true;

	// try all possible splits - this downsizes performance, but is necessary to 
	// recognize expressions like (wee|week)(knights|night)  - for input 
	// 'weeknights' must recognize all 10 characters
	while( left_side_recognized && buf_split < buf_length - buf_offset )
	{
		this->m_ere_expression->recognize( buf, buf_length-buf_split, buf_offset );
		left_side_recognized = this->m_ere_expression->is_recognized();
		if( left_side_recognized )
		{
			this->m_rec_size = this->m_ere_expression->recognized_size();
			this->m_rec_pos = this->m_ere_expression->recognized_position();

			// try the right side
			this->m_ere_branch->recognize( 
					buf,
					buf_length,
					this->m_rec_pos + this->m_rec_size,
					false ); // the 'false' says that trying next 
			// positions is not required
			this->m_is_rec = this->m_ere_branch->is_recognized();
			if( this->m_is_rec )
			{
				buf_split = buf_length - this->m_rec_pos - this->m_rec_size;
				if( shortest_left_recognized_size > this->m_rec_size )
					// compared with ere_expression->recognized_size()
				{
					// use if ereexpressino is 'lazy' (which seems strange in this alg.)
					shortest_left_recognized_size = this->m_rec_size;
					buf_split_for_shortest_size = buf_split;
				}

				this->m_rec_size += this->m_ere_branch->recognized_size();
				if( longest_recognized_size < this->m_rec_size )
				{
					buf_split_for_longest_size = buf_split;
					longest_recognized_size = this->m_rec_size;
				}
			}
			// try more narrow split for left side
			buf_split++;
		}			
	}
	// now once again for found best split - here I feel possible 
	// performance improvement - TODO if there were no recognitions after the 
	// best one, don't need to recognize again
	if( longest_recognized_size > 0 || shortest_left_recognized_size < INT_MAX )
	{
		if( this->m_ere_expression->is_lazy() )
		{
			this->m_ere_expression->recognize( buf, 
					buf_length-buf_split_for_shortest_size, 
					buf_offset );
		}
		else
		{
			// greedy
			this->m_ere_expression->recognize( buf, 
					buf_length-buf_split_for_longest_size, 
					buf_offset );
		}
		this->m_rec_size = this->m_ere_expression->recognized_size();
		this->m_rec_pos = this->m_ere_expression->recognized_position();

		// try the right side
		this->m_ere_branch->recognize( 
				buf,
				buf_length,
				this->m_rec_pos + this->m_rec_size,
				false ); // the 'false' says that trying next 
		// positions is not required
		this->m_rec_size += this->m_ere_branch->recognized_size();
		this->m_is_rec = true;

	}
	else
	{
		this->m_is_rec = false;
	}
	return this->m_rec_size;
}	

	template<typename E>
void ere_branch<E>::ere_expression_ere_branch::assign_matches( matches::match_key parent_address, unsigned long& branch_pos, matches& m )
{
	if( this->m_ere_branch.is_parsed() && this->m_ere_expression.is_parsed() )
	{
		this->m_ere_expression->assign_matches( parent_address, branch_pos, m );
		this->m_ere_branch->assign_matches( parent_address, branch_pos, m );
	}
}

	template<typename E>
void ere_branch<E>::ere_expression_ere_branch::push_parsers( parser_list& l )
{
	l.push_back( &this->m_ere_expression );
	l.push_back( &this->m_ere_branch );
}

	template<typename E>
unsigned long ere_branch<E>::recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset, bool try_positions )
{
	if( this->m_ere.is_parsed() )
	{
		this->m_ere->recognize( buf, buf_length, buf_offset, try_positions );
		this->m_is_rec = this->m_ere->is_recognized();
		this->m_rec_pos = this->m_ere->recognized_position();
		this->m_rec_size = this->m_ere->recognized_size();
	}
	else
	{
		this->m_ere_expression_ere_branch->recognize( buf, buf_length, buf_offset, try_positions );
		this->m_is_rec = this->m_ere_expression_ere_branch->is_recognized();
		this->m_rec_pos = this->m_ere_expression_ere_branch->recognized_position();
		this->m_rec_size = this->m_ere_expression_ere_branch->recognized_size();
	}			
	return this->m_rec_size;
}

	template<typename E>
void ere_branch<E>::assign_matches( matches::match_key parent_address, unsigned long& branch_pos, matches& m )
{
	if( this->m_ere.is_parsed() )
	{
		this->m_ere->assign_matches( parent_address, branch_pos, m );
	}
	else
	{
		this->m_ere_expression_ere_branch->assign_matches( parent_address, branch_pos, m );
	}
}

	template<typename E>
void ere_branch<E>::push_parsers( parser_list& l )
{
	l.push_back( &this->m_ere_expression_ere_branch );
	l.push_back( &this->m_ere );
}

	template<typename E>
unsigned long extended_reg_exp<E>::ere_branch_or_extended_reg_exp::recognize( const E* buf, const unsigned long buf_length,	const unsigned long buf_offset, bool try_positions )
{
	this->m_rec_size = 0;
	this->m_rec_pos = 0;
	this->m_ext_regexp->recognize( buf, buf_length, buf_offset );
	this->m_branch->recognize( buf, buf_length, buf_offset );
	// check whichone is better match
	if( this->m_ext_regexp->is_recognized() && this->m_branch->is_recognized() )
	{
		if( this->m_branch->recognized_size() > this->m_ext_regexp->recognized_size() )
		{
			this->m_ext_regexp->unset();
		}
		else
		{
			this->m_branch->unset();
		}
	}

	this->m_is_rec = true;
	if( this->m_ext_regexp->is_recognized() )
	{
		this->m_rec_pos = this->m_ext_regexp->recognized_position();
		this->m_rec_size = this->m_ext_regexp->recognized_size();
	}
	else if( this->m_branch->is_recognized() )
	{
		this->m_rec_pos = this->m_branch->recognized_position();
		this->m_rec_size = this->m_branch->recognized_size();
	}
	else
	{
		this->m_is_rec = false;
	}
	return this->m_rec_size;
}

	template<typename E>
void extended_reg_exp<E>::ere_branch_or_extended_reg_exp::assign_matches( matches::match_key parent_address, unsigned long& branch_pos, matches& m )
{
	if( this->m_branch->is_recognized() )
	{
		this->m_branch->assign_matches( parent_address, branch_pos, m );
	}
	else
	{
		this->m_ext_regexp->assign_matches( parent_address, branch_pos, m );
	}
}

	template<typename E>
void extended_reg_exp<E>::ere_branch_or_extended_reg_exp::push_parsers( parser_list& l )
{
	l.push_back( &this->m_branch );
	l.push_back( &this->m_or );
	l.push_back( &this->m_ext_regexp );
}

	template<typename E>
unsigned long extended_reg_exp<E>::recognize( const E* buf, const unsigned long buf_length, const unsigned long buf_offset,	bool try_positions )
{
	this->m_rec_pos = 0;
	this->m_rec_size = 0;
	if( this->m_branch.is_parsed() )
	{
		this->m_branch->recognize( buf, buf_length, buf_offset );
		this->m_is_rec = this->m_branch->is_recognized();
		if( this->m_is_rec )
		{
			this->m_rec_pos = this->m_branch->recognized_position();
			this->m_rec_size = this->m_branch->recognized_size();
		}
	}
	else
	{
		this->m_branch_or_ext_regexp->recognize( buf, buf_length, buf_offset );
		this->m_is_rec = this->m_branch_or_ext_regexp->is_recognized();
		if( this->m_is_rec )
		{
			this->m_rec_pos = this->m_branch_or_ext_regexp->recognized_position();
			this->m_rec_size = this->m_branch_or_ext_regexp->recognized_size();
		}
	}
	return this->m_rec_size;
}

	template<typename E>
void extended_reg_exp<E>::assign_matches( matches::match_key key, unsigned long& branch_pos, matches& m )
{
	m.insert( key, match( this->m_rec_pos, this->m_rec_size ) );
	unsigned long new_branch_pos = 0;
	if( this->m_branch.is_parsed() )
	{
		this->m_branch->assign_matches( key, new_branch_pos, m );
	}
	else
	{
		this->m_branch_or_ext_regexp->assign_matches( key, new_branch_pos, m );
	}
}

	template<typename E>
void extended_reg_exp<E>::push_parsers( parser_list& l )
{
	l.push_back( &this->m_branch_or_ext_regexp );
	l.push_back( &this->m_branch );
}

template< typename E >
class ere
{
	typedef typename basic_parser<E>::parser_list parser_list;

	basic_choice< E, extended_reg_exp<E> >	m_ext_reg_exp;
	unsigned long	m_rec_size;
	unsigned long	m_rec_pos;
	bool			m_is_rec;

	public:
	ere() : m_rec_size(0), m_rec_pos(0), m_is_rec(false)  
	{
	}

	virtual ~ere() 
	{
	}

	unsigned long recognize( const E* buf, const unsigned long buf_length, bool try_positions = true )
	{
		this->m_ext_reg_exp->recognize( buf, buf_length, 0, try_positions );
		this->m_is_rec = this->m_ext_reg_exp->is_recognized();
		this->m_rec_pos = this->m_ext_reg_exp->recognized_position();
		this->m_rec_size = this->m_ext_reg_exp->recognized_size();
		return this->m_rec_size;
	}

	void assign_matches( matches& m )
	{
		unsigned long branch_pos = 0;
		matches::match_key key = "1";
		if( this->m_ext_reg_exp.is_parsed() ) this->m_ext_reg_exp->assign_matches( key, branch_pos, m );
	}

	void push_parsers( parser_list& l )
	{
		l.push_back( &this->m_ext_reg_exp );
	}

	const char* get_name() const 
	{
		return "ere"; 
	}

	bool is_recognized()
	{
		return this->m_is_rec;
	}

	const unsigned long recognized_size() const
	{
		return this->m_rec_size;
	}

	const unsigned long recognized_position() const
	{
		return this->m_rec_pos;
	}
};

/*
   template<typename E>
   class end_range
   {
   choice<collating_choice> m_end_range;

   public:
   bool is_greater_or_equals( const E* buf, const unsigned long buf_length )
   {
   return this->m_collating_choice->is_greater_ro_equals(buf,buf_length);
   }

   unsigned long recognized_size()
   {
   return this->m_collating_choice->recognized_size();
   }

   end_range();

   virtual ~end_range();

   };
   */

// following commentary is used for main page in API documentation built
// by DOXYGEN

/*! \mainpage Regular expression parser (regexp_parser)
  The project implements the C++ parser class \ref base_parsers::basic_regexp_parser usable for finding the matching sequence in the input using regular expression. The regular expression pattern is a parameter in parsers constructor. Also allows to retrieve subexpressions matches in few steps. Supports wide chars.\n \n
  The recognition of the pattern and finding of the input matches accomplishes The Open Group specification Issue 6, IEEE Std 1003.1, 2004 Edition, Copyright 2001-2004 The IEEE and The Open Group \n \n
  There are two specializations of the \ref base_parsers::basic_regexp_parser template class - one for char (regexp_parser) and one for wchar_t (wregexp_parser) input type.\n \n
  To use regexp_parser in your project, copy/extract all include files from ./src/base_parsers and ./src/regexp_parsers into include directory accessible from your project, include the regexp_parser.h file and use it, for example, in following manner:


  \code
  regexp_parser rxp( "(^wee|^week)(knights|night)" );
  \endcode

  If regexp pattern comes also from user input, it is possible to check the pattern:

  \code 
  if( !rxp.is_pattern_valid() )
  {
  cerr << "invalid pattern" << endl;
  }
  \endcode

  To parse the input (find the matching part), simply call parse() method

  \code 
  string input = "helloweeknights";
  rxp.parse( input.c_str, input.length() );
  if( rxp.is_parsed() )
  {
  cout << "input part recognized by (^wee|^week)(knights|night) on position ";
  cout rxp.recognized_position() << "with size=" << rxp.recognized_size() << endl;
// output is: 
// input part recognized by (^wee|^week)(knights|night) on position 5 with size=10
}
else
{
cout << "input not recognized by (^wee|^week)(knights|night)" << endl;two
}
\endcode

The parser is also capable to return matches of the regexp and to address 
all subexpressions matches. Each subexpression is addressed by a key in dotted form.
In the match key, the number specifies the subexpression position. By '.' 
are separated levels, if subexpressions are submerged i.e. "(a)" can be addressed 
by 1.1, "((a))" by 1.1.1 etc.\n
By number is specified the subexpression position on the level. Suppose we 
have pattern "a(a(b){1,3})" and input "aaabbbb". Now we are interested in 
subexression 1.1.2 = second b
\code
regexp_parser rxp("a(a(b){1,3})");
rxp.parse( "aaabbbb", 7 );

matches m;
rxp.assign_matches(m);
match subexpr = m.get("1.1.2");
if( subexpr.is_valid() )
{
ulong pos = subexpr.get_pos();	// == 4
ulong size = subexpr.get_size();// == 1	
// do some work
}
\endcode

To iterate through all matches, it is possible to use method get_all().

\code
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

\endcode

Note for developers:\n
To test regular pattern recognition (not finding the matches, at least not in this version), it is possible to use tracer in ./src/common directory. But to be honest, I never used that (produces too much traces).

\code
// when using this directove, the ./src/common/tracer.h must be included
#define TRACE
.
.
.
// switch the trace on
trace_context::get().trace_on();
// will trace regexp recognition (or by other words, building the parsing engine).
regexp_parser rxp("a(a(b){1,3})");
// switch the trace off
trace_context::get().trace_off();


\endcode

With use of matches it is possible to manipulate the input string. See also regexp_test.cpp in test directory.\n
\n
Following are test results from regexp_parser tests:
\code

[--8z--] parsed succesfully input = - recognized succesfully
[[.-.]] parsed succesfully input = - recognized succesfully
[a-bc] parsed succesfully input = ch recognized succesfully
[[:print:]] parsed succesfully input = 1 recognized succesfully
[8--] parsed succesfully input = - recognized succesfully
[a-c] parsed succesfully input = a recognized succesfully
[^a-f] parsed succesfully input = g recognized succesfully
[^a-f] parsed succesfully input = c not recognized succesfully
[[:alnum:]] parsed succesfully input = a recognized succesfully
[--z] parsed succesfully input = - recognized succesfully
[aa-b] parsed succesfully input = b recognized succesfully
[a-bc-h] parsed succesfully input = g recognized succesfully
[a-bc-de-f] parsed succesfully input = a recognized succesfully
[[.a.]] parsed succesfully input = a recognized succesfully
[[=a=]] parsed succesfully input = a recognized succesfully
[[.ch.]] parsed succesfully input = ch not recognized wrongly
"abc "def" "ghi" jkl" succesfully recognized by "".*"" ,size=11 position=4
match (subexpr) key = 1  position=4 size=11;

"abc "def" "ghi" jkl" succesfully recognized by "".*?"" ,size=5 position=4
match (subexpr) key = 1  position=4 size=5;

"aaabbbb" succesfully recognized by "a(a(b){1,3})" ,size=6 position=0
match (subexpr) key = 1  position=0 size=6;
match (subexpr) key = 1.1  position=1 size=5;
match (subexpr) key = 1.1.1  position=3 size=1;
match (subexpr) key = 1.1.2  position=4 size=1;
match (subexpr) key = 1.1.3  position=5 size=1;

"abab" succesfully recognized by "(ab){2,5}" ,size=4 position=0
match (subexpr) key = 1  position=0 size=4;
match (subexpr) key = 1.1  position=0 size=2;
match (subexpr) key = 1.2  position=2 size=2;

"ab" succesfully not recognized by "(ab){2,5}"
"aa" succesfully recognized by "(a){2}" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"a" succesfully recognized by "(a){1}" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;
match (subexpr) key = 1.1  position=0 size=1;

"aa" succesfully recognized by "((a){1,2}){2}" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;
match (subexpr) key = 1.2.1  position=1 size=1;

"abcabcabc" succesfully recognized by "((abc){1,3}){2,5}" ,size=9 position=0
match (subexpr) key = 1  position=0 size=9;
match (subexpr) key = 1.1  position=0 size=6;
match (subexpr) key = 1.1.1  position=0 size=3;
match (subexpr) key = 1.1.2  position=3 size=3;
match (subexpr) key = 1.2  position=6 size=3;
match (subexpr) key = 1.2.1  position=6 size=3;

"abcabcabcabcabcabcabcabc" succesfully recognized by "(((abc){2,5}){2,5}){2,5}" ,size=24 position=0
match (subexpr) key = 1  position=0 size=24;
match (subexpr) key = 1.1  position=0 size=12;
match (subexpr) key = 1.1.1  position=0 size=6;
match (subexpr) key = 1.1.1.1  position=0 size=3;
match (subexpr) key = 1.1.1.2  position=3 size=3;
match (subexpr) key = 1.1.2  position=6 size=6;
match (subexpr) key = 1.1.2.1  position=6 size=3;
match (subexpr) key = 1.1.2.2  position=9 size=3;
match (subexpr) key = 1.2  position=12 size=12;
match (subexpr) key = 1.2.1  position=12 size=6;
match (subexpr) key = 1.2.1.1  position=12 size=3;
match (subexpr) key = 1.2.1.2  position=15 size=3;
match (subexpr) key = 1.2.2  position=18 size=6;
match (subexpr) key = 1.2.2.1  position=18 size=3;
match (subexpr) key = 1.2.2.2  position=21 size=3;

"abcabcabcabcabcabcabc" succesfully not recognized by "(((abc){2,5}){2,5}){2,5}"
"aaaa" succesfully recognized by "(a)*" ,size=4 position=0
match (subexpr) key = 1  position=0 size=4;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;
match (subexpr) key = 1.3  position=2 size=1;
match (subexpr) key = 1.4  position=3 size=1;

"abbbc" succesfully recognized by "bb*" ,size=3 position=1
match (subexpr) key = 1  position=1 size=3;

"cdabababb" succesfully recognized by "[ab]*" ,size=7 position=2
match (subexpr) key = 1  position=2 size=7;

"abcabcabc" succesfully recognized by "(abc){1,3}" ,size=9 position=0
match (subexpr) key = 1  position=0 size=9;
match (subexpr) key = 1.1  position=0 size=3;
match (subexpr) key = 1.2  position=3 size=3;
match (subexpr) key = 1.3  position=6 size=3;

"cabc" succesfully recognized by "(abc){1,3}" ,size=4 position=0
match (subexpr) key = 1  position=0 size=4;
match (subexpr) key = 1.1  position=1 size=3;

"cabcddcabd" succesfully recognized by "(abc)(cab)d{1,3}" ,size=10 position=0
match (subexpr) key = 1  position=0 size=10;
match (subexpr) key = 1.1  position=1 size=3;
match (subexpr) key = 1.2  position=6 size=3;

"cabcddcabddddd" succesfully recognized by "(abc)(cab)d{1,3}" ,size=12 position=0
match (subexpr) key = 1  position=0 size=12;
match (subexpr) key = 1.1  position=1 size=3;
match (subexpr) key = 1.2  position=6 size=3;

"ddddd" succesfully recognized by "d{1,3}d{2,5}" ,size=5 position=0
match (subexpr) key = 1  position=0 size=5;

"ddddd" succesfully recognized by "d{1,3}" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;

"abcabcabcabc" succesfully recognized by "((abc){1,2}){2,5}" ,size=12 position=0
match (subexpr) key = 1  position=0 size=12;
match (subexpr) key = 1.1  position=0 size=6;
match (subexpr) key = 1.1.1  position=0 size=3;
match (subexpr) key = 1.1.2  position=3 size=3;
match (subexpr) key = 1.2  position=6 size=6;
match (subexpr) key = 1.2.1  position=6 size=3;
match (subexpr) key = 1.2.2  position=9 size=3;

"abcabcabcabc" succesfully recognized by "(abc){1,2}{2}" ,size=12 position=0
match (subexpr) key = 1  position=0 size=12;
match (subexpr) key = 1.1  position=0 size=3;
match (subexpr) key = 1.2  position=3 size=3;
match (subexpr) key = 1.3  position=6 size=3;
match (subexpr) key = 1.4  position=9 size=3;

"caabcbdcb" succesfully recognized by "a(a(b){3})" ,size=8 position=1
match (subexpr) key = 1  position=1 size=8;
match (subexpr) key = 1.1  position=2 size=7;
match (subexpr) key = 1.1.1  position=3 size=1;
match (subexpr) key = 1.1.2  position=5 size=1;
match (subexpr) key = 1.1.3  position=8 size=1;

"ba" succesfully not recognized by "^a$"
"a" succesfully recognized by "^a$" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;

"bac" succesfully not recognized by "^a$"
"abc" succesfully not recognized by "^a$"
"aaa" succesfully recognized by "^aaa$" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;

"aba" succesfully recognized by "^a(a)$" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;
match (subexpr) key = 1.1  position=2 size=1;

"abbbaa" succesfully recognized by "^a(a)a$" ,size=6 position=0
match (subexpr) key = 1  position=0 size=6;
match (subexpr) key = 1.1  position=4 size=1;

"abbaccb" succesfully recognized by "^a(a(b))$" ,size=7 position=0
match (subexpr) key = 1  position=0 size=7;
match (subexpr) key = 1.1  position=3 size=4;
match (subexpr) key = 1.1.1  position=6 size=1;

"b" succesfully recognized by "a|b" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;

"ab" succesfully recognized by "(a)(b)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"a" succesfully recognized by "(a)|(b)" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;
match (subexpr) key = 1.1  position=0 size=1;

"add" succesfully recognized by "(a)|(b)|c|dd" ,size=2 position=1
match (subexpr) key = 1  position=1 size=2;

"ac" succesfully recognized by "(a)(b|c)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"ac" succesfully recognized by "(a|b)(c|d)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"aic" succesfully recognized by "(ai|b)(c|d)" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;
match (subexpr) key = 1.1  position=0 size=2;
match (subexpr) key = 1.2  position=2 size=1;

"abbaba" succesfully recognized by "^a(a(^b)a$)" ,size=6 position=0
match (subexpr) key = 1  position=0 size=6;
match (subexpr) key = 1.1  position=3 size=3;
match (subexpr) key = 1.1.1  position=4 size=1;

"weeknights" succesfully recognized by "(wee|week)(knights|night)" ,size=10 position=0
match (subexpr) key = 1  position=0 size=10;
match (subexpr) key = 1.1  position=0 size=3;
match (subexpr) key = 1.2  position=3 size=7;

"aabcbdcb" succesfully recognized by "^a(a(b){3})$" ,size=8 position=0
match (subexpr) key = 1  position=0 size=8;
match (subexpr) key = 1.1  position=1 size=7;
match (subexpr) key = 1.1.1  position=2 size=1;
match (subexpr) key = 1.1.2  position=4 size=1;
match (subexpr) key = 1.1.3  position=7 size=1;

"abab" succesfully recognized by "(ab){2,5}" ,size=4 position=0
match (subexpr) key = 1  position=0 size=4;
match (subexpr) key = 1.1  position=0 size=2;
match (subexpr) key = 1.2  position=2 size=2;

"ab" succesfully not recognized by "(ab){2,5}"
"aa" succesfully recognized by "(a){2}" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"a" succesfully recognized by "(a){1}" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;
match (subexpr) key = 1.1  position=0 size=1;

"aa" succesfully recognized by "((a){1,2}){2}" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;
match (subexpr) key = 1.2.1  position=1 size=1;

"abcabcabc" succesfully recognized by "((abc){1,3}){2,5}" ,size=9 position=0
match (subexpr) key = 1  position=0 size=9;
match (subexpr) key = 1.1  position=0 size=6;
match (subexpr) key = 1.1.1  position=0 size=3;
match (subexpr) key = 1.1.2  position=3 size=3;
match (subexpr) key = 1.2  position=6 size=3;
match (subexpr) key = 1.2.1  position=6 size=3;

"abcabcabcabcabcabcabcabc" succesfully recognized by "(((abc){2,5}){2,5}){2,5}" ,size=24 position=0
match (subexpr) key = 1  position=0 size=24;
match (subexpr) key = 1.1  position=0 size=12;
match (subexpr) key = 1.1.1  position=0 size=6;
match (subexpr) key = 1.1.1.1  position=0 size=3;
match (subexpr) key = 1.1.1.2  position=3 size=3;
match (subexpr) key = 1.1.2  position=6 size=6;
match (subexpr) key = 1.1.2.1  position=6 size=3;
match (subexpr) key = 1.1.2.2  position=9 size=3;
match (subexpr) key = 1.2  position=12 size=12;
match (subexpr) key = 1.2.1  position=12 size=6;
match (subexpr) key = 1.2.1.1  position=12 size=3;
match (subexpr) key = 1.2.1.2  position=15 size=3;
match (subexpr) key = 1.2.2  position=18 size=6;
match (subexpr) key = 1.2.2.1  position=18 size=3;
match (subexpr) key = 1.2.2.2  position=21 size=3;

"abcabcabcabcabcabcabc" succesfully not recognized by "(((abc){2,5}){2,5}){2,5}"
"aaaa" succesfully recognized by "(a)*" ,size=4 position=0
match (subexpr) key = 1  position=0 size=4;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;
match (subexpr) key = 1.3  position=2 size=1;
match (subexpr) key = 1.4  position=3 size=1;

"abbbc" succesfully recognized by "bb*" ,size=3 position=1
match (subexpr) key = 1  position=1 size=3;

"cdabababb" succesfully recognized by "[ab]*" ,size=7 position=2
match (subexpr) key = 1  position=2 size=7;

"abcabcabc" succesfully recognized by "(abc){1,3}" ,size=9 position=0
match (subexpr) key = 1  position=0 size=9;
match (subexpr) key = 1.1  position=0 size=3;
match (subexpr) key = 1.2  position=3 size=3;
match (subexpr) key = 1.3  position=6 size=3;

"cabc" succesfully recognized by "(abc){1,3}" ,size=4 position=0
match (subexpr) key = 1  position=0 size=4;
match (subexpr) key = 1.1  position=1 size=3;

"cabcddcabd" succesfully recognized by "(abc)(cab)d{1,3}" ,size=10 position=0
match (subexpr) key = 1  position=0 size=10;
match (subexpr) key = 1.1  position=1 size=3;
match (subexpr) key = 1.2  position=6 size=3;

"cabcddcabddddd" succesfully recognized by "(abc)(cab)d{1,3}" ,size=12 position=0
match (subexpr) key = 1  position=0 size=12;
match (subexpr) key = 1.1  position=1 size=3;
match (subexpr) key = 1.2  position=6 size=3;

"ddddd" succesfully recognized by "d{1,3}d{2,5}" ,size=5 position=0
match (subexpr) key = 1  position=0 size=5;

"ddddd" succesfully recognized by "d{1,3}" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;

"abcabcabcabc" succesfully recognized by "((abc){1,2}){2,5}" ,size=12 position=0
match (subexpr) key = 1  position=0 size=12;
match (subexpr) key = 1.1  position=0 size=6;
match (subexpr) key = 1.1.1  position=0 size=3;
match (subexpr) key = 1.1.2  position=3 size=3;
match (subexpr) key = 1.2  position=6 size=6;
match (subexpr) key = 1.2.1  position=6 size=3;
match (subexpr) key = 1.2.2  position=9 size=3;

"abcabcabcabc" succesfully recognized by "(abc){1,2}{2}" ,size=12 position=0
match (subexpr) key = 1  position=0 size=12;
match (subexpr) key = 1.1  position=0 size=3;
match (subexpr) key = 1.2  position=3 size=3;
match (subexpr) key = 1.3  position=6 size=3;
match (subexpr) key = 1.4  position=9 size=3;

"caabcbdcb" succesfully recognized by "a(a(b){3})" ,size=8 position=1
match (subexpr) key = 1  position=1 size=8;
match (subexpr) key = 1.1  position=2 size=7;
match (subexpr) key = 1.1.1  position=3 size=1;
match (subexpr) key = 1.1.2  position=5 size=1;
match (subexpr) key = 1.1.3  position=8 size=1;

"ba" succesfully not recognized by "^a$"
"a" succesfully recognized by "^a$" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;

"bac" succesfully not recognized by "^a$"
"abc" succesfully not recognized by "^a$"
"aaa" succesfully recognized by "^aaa$" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;

"aba" succesfully recognized by "^a(a)$" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;
match (subexpr) key = 1.1  position=2 size=1;

"abbbaa" succesfully recognized by "^a(a)a$" ,size=6 position=0
match (subexpr) key = 1  position=0 size=6;
match (subexpr) key = 1.1  position=4 size=1;

"abbaccb" succesfully recognized by "^a(a(b))$" ,size=7 position=0
match (subexpr) key = 1  position=0 size=7;
match (subexpr) key = 1.1  position=3 size=4;
match (subexpr) key = 1.1.1  position=6 size=1;

"b" succesfully recognized by "a|b" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;

"ab" succesfully recognized by "(a)(b)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"a" succesfully recognized by "(a)|(b)" ,size=1 position=0
match (subexpr) key = 1  position=0 size=1;
match (subexpr) key = 1.1  position=0 size=1;

"ca" succesfully recognized by "a|b" ,size=1 position=1
match (subexpr) key = 1  position=1 size=1;

"ca" succesfully recognized by "(a)|(b)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=1 size=1;

"ca" succesfully recognized by "(^a)|(b)" ,size=1 position=1
match (subexpr) key = 1  position=1 size=1;
match (subexpr) key = 1.1  position=1 size=1;

"ca" succesfully recognized by "(a)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=1 size=1;

"ca" succesfully recognized by "(^a)" ,size=1 position=1
match (subexpr) key = 1  position=1 size=1;
match (subexpr) key = 1.1  position=1 size=1;

"add" succesfully recognized by "(a)|(b)|c|dd" ,size=2 position=1
match (subexpr) key = 1  position=1 size=2;

"ac" succesfully recognized by "(a)(b|c)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"ac" succesfully recognized by "(a|b)(c|d)" ,size=2 position=0
match (subexpr) key = 1  position=0 size=2;
match (subexpr) key = 1.1  position=0 size=1;
match (subexpr) key = 1.2  position=1 size=1;

"aic" succesfully recognized by "(ai|b)(c|d)" ,size=3 position=0
match (subexpr) key = 1  position=0 size=3;
match (subexpr) key = 1.1  position=0 size=2;
match (subexpr) key = 1.2  position=2 size=1;

"abbaba" succesfully recognized by "^a(a(^b)a$)" ,size=6 position=0
match (subexpr) key = 1  position=0 size=6;
match (subexpr) key = 1.1  position=3 size=3;
match (subexpr) key = 1.1.1  position=4 size=1;

"weeknights" succesfully recognized by "(wee|week)(knights|night)" ,size=10 position=0
match (subexpr) key = 1  position=0 size=10;
match (subexpr) key = 1.1  position=0 size=3;
match (subexpr) key = 1.2  position=3 size=7;

"helloweeknights" succesfully recognized by "o(wee|week)(knights|night)" ,size=11 position=4
match (subexpr) key = 1  position=4 size=11;
match (subexpr) key = 1.1  position=5 size=3;
match (subexpr) key = 1.2  position=8 size=7;

"helloweeknights" succesfully recognized by "(wee|week)(knights|night)" ,size=15 position=0
match (subexpr) key = 1  position=0 size=15;
match (subexpr) key = 1.1  position=5 size=3;
match (subexpr) key = 1.2  position=8 size=7;

"helloweeknights" succesfully recognized by "(^wee|^week)(knights|night)" ,size=10 position=5
match (subexpr) key = 1  position=5 size=10;
match (subexpr) key = 1.1  position=5 size=3;
match (subexpr) key = 1.2  position=8 size=7;

"aabcbdcb" succesfully recognized by "^a(a(b){3})$" ,size=8 position=0
match (subexpr) key = 1  position=0 size=8;
match (subexpr) key = 1.1  position=1 size=7;
match (subexpr) key = 1.1.1  position=2 size=1;
match (subexpr) key = 1.1.2  position=4 size=1;
match (subexpr) key = 1.1.3  position=7 size=1;
\endcode

*/

const char* REGEXP_PARSER = "regexpParser";

/*!
  The class for parsing strings using regular expressions.
  The implementation accomplishes 
  The Open Group specification Issue 6, IEEE Std 1003.1, 2004 Edition, 
  Copyright 2001-2004 The IEEE and The Open Group.
  The parser was also extended to return matches of the regexp and 
  to address all subexpressions matches. With use of matches it is 
  possible to manipulate the input string	
  */
template< typename E >
class basic_regexp_parser : public basic_object_parser< E, std::basic_string<E> >
{
	public:
		basic_regexp_parser( std::basic_string<E> regular_expression ) : 
			basic_object_parser< E, std::basic_string<E> >(REGEXP_PARSER),
			m_regular_expression( regular_expression ) {};

		~basic_regexp_parser() {};

		//! Returns 'true', if regexp pattern passed into constructor is valid
		bool is_pattern_valid()
		{
			if( !this->m_regexp.is_parsed() )
			{
				this->m_regexp.parse( this->m_regular_expression.c_str(), 
						this->m_regular_expression.length() );
			}
			return this->m_regexp.is_parsed();
		}

		//! Uses regexp pattern to find match in the input buffer
		/*! 
		  The constructor parameter is used as regexp pattern during parsing
		  \param buf input buffer
		  \param buf_length input buffer length
		  \return parsed size
		  */	
		virtual unsigned long parse( const E *buf, const unsigned long buf_length )
		{
			this->prepare_for_parsing();

			if( this->is_valid() )
			{
				this->m_parsed_size = this->m_regexp->recognize( buf, buf_length );
			}
			else
			{
				return 0;
			}
			if( this->m_regexp->is_recognized() )
			{
				this->m_parsed_object = 
					new std::basic_string<E>( buf, this->m_parsed_size );
				this->m_is_parsed = true;
			}
			return this->m_parsed_size;
		}

		//! Uses regexp pattern to find match in the input stream
		/*! 
		  The constructor parameter is used as regexp pattern during parsing
		  \param is input stream
		  \return input stream with 'get' position shifted by parsed size
		  */	
		virtual std::basic_istream<E>& parse( std::basic_istream<E>& is )
		{
			this->prepare_for_parsing();

			// TODO - implement to pass istream to regexp - now limited to input in one line
			std::basic_stringbuf<E> buf;
			std::streampos pos = is.tellg();
			if( is.good() )
			{
				is.get( buf );
				unsigned long length = buf.str().length();
				if( this->is_valid() )
				{
					this->m_regexp->recognize( buf.str().c_str(), length );
				}
				else
				{
					return is;
				}
				if( this->m_regexp->is_recognized() )
				{
					this->m_parsed_size = this->m_regexp->recognized_position() + this->m_regexp->recognized_size();
					this->m_parsed_object = 
						new std::basic_string<E>( buf.str().c_str(), this->m_parsed_size );
					this->m_is_parsed = true;
					pos += this->m_parsed_size;
					is.clear();
					is.seekg(pos);
				}
				else
				{
					is.clear();
					is.seekg(pos);
				}
			}
			return is;
		}

		//! Returns the match position in the input
		unsigned long recognized_position()
		{
			if( is_pattern_valid() && this->m_regexp->is_recognized() )
			{
				return this->m_regexp->recognized_position();
			}
			else
			{
				return 0;
			}
		}

		//! Returns the match size in the input
		unsigned long recognized_size()
		{
			if( is_pattern_valid() && this->m_regexp->is_recognized() )
			{
				return this->m_regexp->recognized_size();
			}
			else
			{
				return 0;
			}
		}

		//! Assigns matches of all subexpressions
		/*!
		  The method retrieves all subexpressions data (position and size in 
		  original input buffer). Each subexpression is addressed by a key. In the 
		  match key, the number specifies the subexpression position, 
		  by '.' are separated levels, i.e. "a" is addresses by 1, "(a)" 
		  by 1.1, "((a))" by 1.1.1 etc. By number is specified the 
		  subexpression position on the level. Suppose we have pattern "a(a(b){1,3})" 
		  and input "aaabbbb". Now we are interested in subexression 1.1.2 = second b
		  \code
		  regexp_parser rxp("a(a(b){1,3})");
		  rxp.parse( "aaabbbb", 7 );

		  matches m;
		  rxp.assign_matches(m);
		  match subexpr = m.get("1.1.2");
		  if( subexpr.this->is_valid() )
		  {
		  ulong pos = subexpr.get_pos();	// == 4
		  ulong size = subexpr.get_size();// == 1	
		// do some work
		}
		\endcode
		*/
		void assign_matches( matches& m )
		{
			if( is_pattern_valid() && this->m_regexp->is_recognized() )
			{
				this->m_regexp->assign_matches(m);
			}
		}

		//! Formats the preceding characters together with match string back into output stream
		virtual std::basic_ostream<E>& format( std::basic_ostream<E> &os )
		{
			os << this->get_valid();
			return os;
		}

		//! Returns the match as a string together with characters before match position
		operator std::basic_string<E>()
		{
			return this->get_valid();
		}	

	protected:
		std::basic_string<E>								m_regular_expression;
		basic_non_terminal< E, ere<E> >	m_regexp;

	protected:
		void prepare_for_parsing()
		{
			basic_object_parser< E,std::basic_string<E> >::prepare_for_parsing();
			if( !this->m_regexp.is_parsed() )
			{
				this->m_regexp.parse( this->m_regular_expression.c_str(), 
						this->m_regular_expression.length() );
			}
		}

};

typedef basic_regexp_parser<char> regexp_parser;
typedef basic_regexp_parser<wchar_t> wregexp_parser;

}

#endif


/***************************************************************************
 *   Copyright (C) 2005 by Lee Thomason <modified version>                 *
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
#include "jcommon/jxmlparser.h"

namespace jcommon {

XmlCursor::XmlCursor()
{
	Clear(); 
}

void XmlCursor::Clear()
{
	row = col = -1; 
}

XmlVisitor::~XmlVisitor() 
{
}

bool XmlVisitor::VisitEnter(const XmlDocument& doc)	
{
	return true; 
}

bool XmlVisitor::VisitExit(const XmlDocument& doc)	
{
	return true; 
}

bool XmlVisitor::VisitEnter(const XmlElement& element, const XmlAttribute* firstAttribute)
{
	return true; 
}

bool XmlVisitor::VisitExit(const XmlElement& element)
{
	return true; 
}

bool XmlVisitor::Visit(const XmlDeclaration& declaration)
{
	return true; 
}

bool XmlVisitor::Visit(const XmlText& text)
{
	return true; 
}

bool XmlVisitor::Visit(const XmlComment& comment)
{
	return true; 
}

bool XmlVisitor::Visit(const XmlUnknown& unknown)
{ 
	return true; 
}

bool XmlBase::IsWhiteSpace( char c )
{ 
	return (isspace( (uint8_t) c ) || c == '\n' || c == '\r');
}

bool XmlBase::IsWhiteSpace( int c )
{
	if ( c < 256 ) {
		return IsWhiteSpace( (char) c );
	}
	return false;	// Again, only truly correct for English/Latin...but usually works.
}

const char * XmlBase::GetChar(const char* p, char* value, int* length, XmlEncoding encoding)
{
	assert( p );
	if ( encoding == TIXML_ENCODING_UTF8 ) {
		*length = utf8ByteTable[ *((const uint8_t *)p) ];
		assert( *length >= 0 && *length < 5 );
	} else {
		*length = 1;
	}
	
	if ( *length == 1 ) {
		if ( *p == '&' ) {
			return GetEntity( p, value, length, encoding );
		}

		*value = *p;

		return p+1;
	} else if ( *length ) {
		//strncpy( value, p, *length );	// lots of compilers don't like this function (unsafe),
		// and the null terminator isn't needed
		for( int i=0; p[i] && i<*length; ++i ) {
			value[i] = p[i];
		}
		
		return p + (*length);
	} else {
		// Not valid text.
		return 0;
	}
}

int XmlBase::ToLower(int v, XmlEncoding encoding)
{
	if ( encoding == TIXML_ENCODING_UTF8 ) {
		if ( v < 128 ) return tolower( v );
		return v;
	} else {
		return tolower( v );
	}
}

XmlBase::XmlBase()	:	userData(0)
{
}

XmlBase::~XmlBase()
{
}

void XmlBase::SetCondenseWhiteSpace(bool condense)
{
	condenseWhiteSpace = condense; 
}

bool XmlBase::IsWhiteSpaceCondensed()
{
	return condenseWhiteSpace; 
}

int XmlBase::Row() const	
{ 
	return location.row + 1; 
}

int XmlBase::Column() const		// SEE Row
{
	return location.col + 1; 
}

void XmlBase::SetUserData(void *user)
{ 
	userData = user; 
}	///< Set a pointer to arbitrary user data.

void * XmlBase::GetUserData()
{ 
	return userData; 
}	///< Get a pointer to arbitrary user data.

const void * XmlBase::GetUserData() const 
{ 
	return userData; 
}	///< Get a pointer to arbitrary user data.

const char * XmlNode::Value() const 
{
	return _value.c_str (); 
}

const std::string& XmlNode::ValueStr() const 
{
	return _value; 
}

void XmlNode::SetValue(const char * value) 
{
	_value = value;
}

void XmlNode::SetValue( const std::string& value )	
{
	_value = value; 
}

XmlNode* XmlNode::Parent()
{
	return _parent; 
}

const XmlNode* XmlNode::Parent() const
{ 
	return _parent;
}

const XmlNode* XmlNode::FirstChild() const 
{
	return _firstChild; 
}		///< The first child of this node. Will be null if there are no children.

XmlNode* XmlNode::FirstChild()
{
	return _firstChild; 
}

XmlNode* XmlNode::FirstChild( const char * value )  
{
	// Call through to the const version - safe since nothing is changed.
	// Exiting syntax: cast this to a const (always safe) call the method,
	// cast the return back to non-const.
	return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->FirstChild( value ));
}

const XmlNode* XmlNode::LastChild() const	
{
	return _lastChild;
}		/// The last child of this node. Will be null if there are no children.


XmlNode* XmlNode::LastChild()	
{
	return _lastChild; 
}

XmlNode* XmlNode::LastChild( const char * value ) 
{
	return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->LastChild( value ));
}

const XmlNode* XmlNode::FirstChild( const std::string& value ) const	
{
	return FirstChild (value.c_str ());	
}	///< STL std::string form.

XmlNode* XmlNode::FirstChild( const std::string& value )
{
	return FirstChild (value.c_str ());
}	///< STL std::string form.

const XmlNode* XmlNode::LastChild( const std::string& value ) const	
{
	return LastChild (value.c_str ());
}	///< STL std::string form.

XmlNode* XmlNode::LastChild( const std::string& value )
{
	return LastChild (value.c_str ());
}	///< STL std::string form.

XmlNode* XmlNode::IterateChildren( XmlNode* previous ) 
{
	return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->IterateChildren( previous ) );
}

XmlNode* XmlNode::IterateChildren( const char * value, XmlNode* previous ) 
{
	return const_cast< XmlNode* >( const_cast< const XmlNode* >(this)->IterateChildren( value, previous ) );
}

const XmlNode* XmlNode::IterateChildren( const std::string& value, const XmlNode* previous ) const
{	
	return IterateChildren (value.c_str (), previous);
}	///< STL std::string form.

XmlNode* XmlNode::IterateChildren( const std::string& value, XmlNode* previous )
{
	return IterateChildren (value.c_str (), previous);
}

// Navigate to a sibling node.
const XmlNode* XmlNode::PreviousSibling() const
{
	return _prev;
}

XmlNode* XmlNode::PreviousSibling()	
{ 
	return _prev;
}

XmlNode* XmlNode::PreviousSibling( const char *prev ) 
{
	return const_cast< XmlNode* >( const_cast< const XmlNode* >(this)->PreviousSibling( prev ) );
}

const XmlNode* XmlNode::PreviousSibling( const std::string& value ) const
{
	return PreviousSibling (value.c_str ());
}

XmlNode* XmlNode::PreviousSibling( const std::string& value )
{
	return PreviousSibling (value.c_str ());
}

const XmlNode* XmlNode::NextSibling( const std::string& value) const
{
	return NextSibling (value.c_str ());	
}
	
XmlNode* XmlNode::NextSibling( const std::string& value) 
{
	return NextSibling (value.c_str ());
}

const XmlNode* XmlNode::NextSibling() const	
{
	return _next;
}

XmlNode* XmlNode::NextSibling()	
{ 
	return _next;
}

XmlNode* XmlNode::NextSibling( const char* next ) 
{
	return const_cast< XmlNode* >( const_cast< const XmlNode* >(this)->NextSibling( next ) );
}

XmlElement* XmlNode::NextSiblingElement() 
{
	return const_cast< XmlElement* >( const_cast< const XmlNode* >(this)->NextSiblingElement() );
}

XmlElement* XmlNode::NextSiblingElement( const char *next )
{
	return const_cast< XmlElement* >( const_cast< const XmlNode* >(this)->NextSiblingElement( next ) );
}

const XmlElement* XmlNode::NextSiblingElement( const std::string& value) const	
{
	return NextSiblingElement (value.c_str ());
}	///< STL std::string form.

XmlElement* XmlNode::NextSiblingElement( const std::string& value)
{	
	return NextSiblingElement (value.c_str ());
}	///< STL std::string form.

XmlElement* XmlNode::FirstChildElement() 
{
	return const_cast< XmlElement* >( const_cast< const XmlNode* >(this)->FirstChildElement() );
}

XmlElement* XmlNode::FirstChildElement( const char * value )
{
	return const_cast< XmlElement* >( const_cast< const XmlNode* >(this)->FirstChildElement( value ) );
}

const XmlElement* XmlNode::FirstChildElement( const std::string& value ) const	
{
	return FirstChildElement (value.c_str ());
}	///< STL std::string form.

XmlElement* XmlNode::FirstChildElement( const std::string& value )
{
	return FirstChildElement (value.c_str ());
}	///< STL std::string form.

int XmlNode::Type() const
{ 
	return _type; 
}

XmlDocument* XmlNode::GetDocument()
{
	return const_cast< XmlDocument* >( const_cast< const XmlNode* >(this)->GetDocument() );
}

bool XmlNode::HasChildren() const
{
	return _firstChild != NULL;
}

const XmlDocument * XmlNode::ToDocument() const
{
	return NULL;
}

const XmlElement * XmlNode::ToElement() const 
{
	return NULL;
}

const XmlComment * XmlNode::ToComment() const 
{
	return NULL;
}

const XmlUnknown * XmlNode::ToUnknown() const
{
	return NULL; 
}

const XmlText * XmlNode::ToText() const 
{
	return NULL; 
}

const XmlDeclaration * XmlNode::ToDeclaration() const
{
	return NULL;
}

XmlDocument * XmlNode::ToDocument()    
{
	return NULL; 
}

XmlElement * XmlNode::ToElement()	
{ 
	return NULL; 
}

XmlComment * XmlNode::ToComment() 
{ 
	return NULL;
}

XmlUnknown * XmlNode::ToUnknown()	   
{
	return NULL;
}

XmlText * XmlNode::ToText()       
{
	return NULL;
}

XmlDeclaration * XmlNode::ToDeclaration() 
{
	return NULL;
}

XmlAttribute::XmlAttribute() : XmlBase()
{
	_document = 0;
	_prev = 0;
	_next = 0;
}

XmlAttribute::XmlAttribute( const std::string& name, const std::string& value )
{
	_name = name;
	_value = value;
	_document = 0;
	_prev = 0;
	_next = 0;
}

XmlAttribute::XmlAttribute( const char * name, const char * value )
{
	_name = name;
	_value = value;
	_document = 0;
	_prev = 0;
	_next = 0;
}

const char * XmlAttribute::Name()  const
{ 
	return _name.c_str(); 
}

const char * XmlAttribute::Value() const
{
	return _value.c_str();
}

const std::string& XmlAttribute::ValueStr() const	
{
	return _value; 
}	

const std::string& XmlAttribute::NameTStr() const 
{
	return _name; 
}

void XmlAttribute::SetName( const char* name )	
{
	_name = name; 
}

void XmlAttribute::SetValue( const char* value )	
{
	_value = value;
}

void XmlAttribute::SetName( const std::string& name )
{
	_name = name; 
}

void XmlAttribute::SetValue( const std::string& value )
{ 
	_value = value;
}

XmlAttribute* XmlAttribute::Next() 
{
	return const_cast< XmlAttribute* >( const_cast< const XmlAttribute* >(this)->Next() ); 
}

XmlAttribute* XmlAttribute::Previous() 
{
	return const_cast< XmlAttribute* >( const_cast< const XmlAttribute* >(this)->Previous() ); 
}

bool XmlAttribute::operator==( const XmlAttribute& rhs ) const 
{
	return rhs._name == _name; 
}

bool XmlAttribute::operator<( const XmlAttribute& rhs )	 const
{
	return _name < rhs._name; 
}

bool XmlAttribute::operator>( const XmlAttribute& rhs )  const 
{
	return _name > rhs._name; 
}

void XmlAttribute::Print( FILE* cfile, int depth ) const 
{
	Print( cfile, depth, 0 );
}

void XmlAttribute::SetDocument( XmlDocument* doc )	
{
	_document = doc; 
}

const XmlAttribute* XmlAttributeSet::First()	const	
{
	return ( sentinel._next == &sentinel ) ? 0 : sentinel._next; 
}

XmlAttribute* XmlAttributeSet::First()					
{
	return ( sentinel._next == &sentinel ) ? 0 : sentinel._next; 
}

const XmlAttribute* XmlAttributeSet::Last() const
{
	return ( sentinel._prev == &sentinel ) ? 0 : sentinel._prev; 
}

XmlAttribute* XmlAttributeSet::Last()
{
	return ( sentinel._prev == &sentinel ) ? 0 : sentinel._prev; 
}

XmlAttribute*	XmlAttributeSet::Find( const char* name ) 
{
	return const_cast< XmlAttribute* >( const_cast< const XmlAttributeSet* >(this)->Find( name ) );
}

XmlAttribute*	XmlAttributeSet::Find( const std::string& name ) 
{
	return const_cast< XmlAttribute* >( const_cast< const XmlAttributeSet* >(this)->Find( name ) );
}

int XmlElement::QueryFloatAttribute( const char* name, float* value ) const 
{
	double d;
	int result = QueryDoubleAttribute( name, &d );
	if ( result == TIXML_SUCCESS ) {
		*value = (float)d;
	}
	return result;
}

void XmlElement::RemoveAttribute( const std::string& name )
{
	RemoveAttribute (name.c_str ());
}

const XmlAttribute* XmlElement::FirstAttribute() const	
{
	return attributeSet.First();
}

XmlAttribute* XmlElement::FirstAttribute()
{ 
	return attributeSet.First(); 
}

const XmlAttribute* XmlElement::LastAttribute()	const 	
{
	return attributeSet.Last();
}

XmlAttribute* XmlElement::LastAttribute()
{
	return attributeSet.Last(); 
}

const XmlElement * XmlElement::ToElement()     const 
{
	return this; 
}

XmlElement * XmlElement::ToElement()
{
	return this;
}

XmlComment::XmlComment() : XmlNode( XmlNode::COMMENT ) 
{
}

XmlComment::XmlComment( const char* value ) : XmlNode( XmlNode::COMMENT ) 
{
	SetValue( value );
}

XmlComment::~XmlComment()	
{
}

const XmlComment*  XmlComment::ToComment() const 
{
	return this; 
} 

XmlComment*  XmlComment::ToComment() 
{
	return this; 
}

XmlText::XmlText (const char * initValue ) : XmlNode (XmlNode::TEXT)
{
	SetValue( initValue );
	_cdata = false;
}

XmlText::~XmlText() 
{
}

XmlText::XmlText( const std::string& initValue ) : XmlNode (XmlNode::TEXT)
{
	SetValue( initValue );
	_cdata = false;
}

XmlText::XmlText( const XmlText& copy ) : XmlNode( XmlNode::TEXT )
{
	copy.CopyTo( this ); 
}

void XmlText::operator=( const XmlText& base )
{ 
	base.CopyTo( this ); 
}

const XmlText* XmlText::ToText() const 
{
	return this; 
}

XmlText * XmlText::ToText()
{
	return this;
}

XmlDeclaration::XmlDeclaration()   : XmlNode( XmlNode::DECLARATION ) 
{
}

XmlDeclaration::~XmlDeclaration()	
{
}

const char *XmlDeclaration::Version() const	
{
	return _version.c_str (); 
}

/// Encoding. Will return an empty string if none was found.
const char *XmlDeclaration::Encoding() const		
{
	return _encoding.c_str (); 
}

/// Is this a standalone document?
const char *XmlDeclaration::Standalone() const
{ 
	return _standalone.c_str (); 
}

void XmlDeclaration::Print( FILE* cfile, int depth ) const 
{
	Print( cfile, depth, 0 );
}

const XmlDeclaration* XmlDeclaration::ToDeclaration() const 
{
	return this; 
}

XmlDeclaration*       XmlDeclaration::ToDeclaration()
{
	return this; 
}

XmlUnknown::XmlUnknown() : XmlNode( XmlNode::UNKNOWN )	
{
}

XmlUnknown::~XmlUnknown() 
{
}

XmlUnknown::XmlUnknown( const XmlUnknown& copy ) : XmlNode( XmlNode::UNKNOWN )
{
	copy.CopyTo( this ); 
}

void XmlUnknown::operator=( const XmlUnknown& copy )	
{
	copy.CopyTo( this ); 
}

const XmlUnknown*     XmlUnknown::ToUnknown()     const
{
	return this; 
}

XmlUnknown * XmlUnknown::ToUnknown()	 
{
	return this;
} ///< Cast to a more defined type. Will return null not of the requested type.

XmlDocument::~XmlDocument() 
{
	Clear();
	location.Clear();
}

bool XmlDocument::LoadFile( const std::string& filename, XmlEncoding encoding )///< STL std::string version.
{
	//		StringToBuffer f( filename );
	//		return ( f.buffer && LoadFile( f.buffer, encoding ));
	return LoadFile( filename.c_str(), encoding );
}

bool XmlDocument::SaveFile( const std::string& filename ) const		///< STL std::string version.
{
	//		StringToBuffer f( filename );
	//		return ( f.buffer && SaveFile( f.buffer ));
	return SaveFile( filename.c_str() );
}

const XmlElement* XmlDocument::RootElement() const	
{ 
	return FirstChildElement(); 
}

XmlElement* XmlDocument::RootElement()
{
	return FirstChildElement(); 
}

bool XmlDocument::Error() const
{
	return error; 
}

const char * XmlDocument::ErrorDesc() const	
{
	return errorDesc.c_str ();
}

int XmlDocument::ErrorId()	const
{
	return errorId; 
}

int XmlDocument::ErrorRow()
{
	return errorLocation.row+1;
}

int XmlDocument::ErrorCol()	
{
	return errorLocation.col+1;
}	///< The column where the error occured. See ErrorRow()

void XmlDocument::SetTabSize( int tabsize )	
{
	_tabsize = tabsize;
}

int XmlDocument::TabSize() const
{
	return _tabsize; 
}

void XmlDocument::ClearError()
{
	error = false; 
	errorId = 0; 
	errorDesc = ""; 
	errorLocation.row = errorLocation.col = 0; 
	//errorLocation.last = 0; 
}

void XmlDocument::Print() const			
{
	Print( stdout, 0 );
}

const XmlDocument*    XmlDocument::ToDocument()    const
{
	return this; 
} ///< Cast to a more defined type. Will return null not of the requested type.

XmlDocument*          XmlDocument::ToDocument()     
{
	return this;
} ///< Cast to a more defined type. Will return null not of the requested type.

XmlHandle::XmlHandle( XmlNode* node )		
{ 
	this->node = node; 
}

/// Copy constructor
XmlHandle::XmlHandle( const XmlHandle& ref )	
{ 
	this->node = ref.node;
}

XmlHandle XmlHandle::operator=( const XmlHandle& ref )
{
	this->node = ref.node; return *this; 
}

XmlHandle XmlHandle::FirstChild( const std::string& value ) const
{ 
	return FirstChild( value.c_str() );
}

XmlHandle XmlHandle::FirstChildElement( const std::string& value ) const
{
	return FirstChildElement( value.c_str() );
}

XmlHandle XmlHandle::Child( const std::string& value, int index ) const		
{ 
	return Child( value.c_str(), index ); 
}

XmlHandle XmlHandle::ChildElement( const std::string& value, int index ) const	
{
	return ChildElement( value.c_str(), index );
}

XmlNode* XmlHandle::ToNode() const	
{ 
	return node; 
} 

XmlElement* XmlHandle::ToElement() const	
{ 
	return ( ( node && node->ToElement() ) ? node->ToElement() : 0 ); 
}

XmlText* XmlHandle::ToText() const
{ 
	return ( ( node && node->ToText() ) ? node->ToText() : 0 ); 
}

XmlUnknown* XmlHandle::ToUnknown() const		
{
	return ( ( node && node->ToUnknown() ) ? node->ToUnknown() : 0 ); 
}

XmlNode* XmlHandle::Node() const
{
	return ToNode(); 
}

XmlElement* XmlHandle::Element() const	
{ 
	return ToElement();
}

XmlText* XmlHandle::Text() const	
{ 
	return ToText();
}

XmlUnknown* XmlHandle::Unknown() const
{ 
	return ToUnknown(); 
}

XmlPrinter::XmlPrinter():
	_depth( 0 ), 
	_simpleTextPrint( false ), 
	_buffer(), 
	_indent( "    " ), 
	_lineBreak( "\n" ) 
{
}

void XmlPrinter::SetIndent( const char* indent )
{
	_indent = indent ? indent : "" ;
}

const char* XmlPrinter::Indent()
{
	return _indent.c_str();
}

void XmlPrinter::SetLineBreak( const char* lineBreak )
{ 
	_lineBreak = lineBreak ? lineBreak : "";
}

const char* XmlPrinter::LineBreak()
{ 
	return _lineBreak.c_str();
}

void XmlPrinter::SetStreamPrinting()
{ 
	_indent = "";
	_lineBreak = "";
}

const std::string& XmlPrinter::Str()
{
	return _buffer; 
}

void XmlPrinter::DoIndent()	
{
	for( int i=0; i<_depth; ++i ) {
		_buffer += _indent;
	}
}

void XmlPrinter::DoLineBreak() 
{
	_buffer += _lineBreak;
}

// INIT XML PARSER ERROR

// The goal of the seperate error file is to make the first
// step towards localization. tinyxml (currently) only supports
// english error messages, but the could now be translated.
//
// It also cleans up the code a bit.
//

const char* XmlBase::errorString[ TIXML_ERROR_STRING_COUNT ] =
{
	"No error",
	"Error",
	"Failed to open file",
	"Memory allocation failed.",
	"Error parsing Element.",
	"Failed to read Element name",
	"Error reading Element value.",
	"Error reading Attributes.",
	"Error: empty tag.",
	"Error reading end tag.",
	"Error parsing Unknown.",
	"Error parsing Comment.",
	"Error parsing Declaration.",
	"Error document empty.",
	"Error null (0) or unexpected EOF found in input stream.",
	"Error parsing CDATA.",
	"Error when XmlDocument added to document, because XmlDocument can only be at the root.",
};

// Note tha "PutString" hardcodes the same list. This
// is less flexible than it appears. Changing the entries
// or order will break putstring.	
XmlBase::Entity XmlBase::entity[ NUM_ENTITY ] = 
{
	{ "&amp;",  5, '&' },
	{ "&lt;",   4, '<' },
	{ "&gt;",   4, '>' },
	{ "&quot;", 6, '\"' },
	{ "&apos;", 6, '\'' }
};

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
// Including the basic of this table, which determines the #bytes in the
// sequence from the lead byte. 1 placed for invalid sequences --
// although the result will be junk, pass it through as much as possible.
// Beware of the non-characters in UTF-8:	
//				ef bb bf (Microsoft "lead bytes")
//				ef bf be
//				ef bf bf 

const uint8_t TIXML_UTF_LEAD_0 = 0xefU;
const uint8_t TIXML_UTF_LEAD_1 = 0xbbU;
const uint8_t TIXML_UTF_LEAD_2 = 0xbfU;

const int XmlBase::utf8ByteTable[256] = 
{
	//	0	1	2	3	4	5	6	7	8	9	a	b	c	d	e	f
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x00
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x10
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x20
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x30
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x40
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x50
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x60
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x70	End of ASCII range
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x80 0x80 to 0xc1 invalid
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x90 
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xa0 
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xb0 
		1,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xc0 0xc2 to 0xdf 2 byte
		2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xd0
		3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	// 0xe0 0xe0 to 0xef 3 byte
		4,	4,	4,	4,	4,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1	// 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
};


void XmlBase::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
	const unsigned long BYTE_MASK = 0xBF;
	const unsigned long BYTE_MARK = 0x80;
	const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	if (input < 0x80) 
		*length = 1;
	else if ( input < 0x800 )
		*length = 2;
	else if ( input < 0x10000 )
		*length = 3;
	else if ( input < 0x200000 )
		*length = 4;
	else
		{ *length = 0; return; }	// This code won't covert this correctly anyway.

	output += *length;

	// Scary scary fall throughs.
	switch (*length) 
	{
		case 4:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 3:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 2:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 1:
			--output; 
			*output = (char)(input | FIRST_BYTE_MARK[*length]);
	}
}


/*static*/ int XmlBase::IsAlpha( uint8_t anyByte, XmlEncoding /*encoding*/ )
{
	// This will only work for low-ascii, everything else is assumed to be a valid
	// letter. I'm not sure this is the best approach, but it is quite tricky trying
	// to figure out alhabetical vs. not across encoding. So take a very 
	// conservative approach.

//	if ( encoding == TIXML_ENCODING_UTF8 )
//	{
		if ( anyByte < 127 )
			return isalpha( anyByte );
		else
			return 1;	// What else to do? The unicode set is huge...get the english ones right.
//	}
//	else
//	{
//		return isalpha( anyByte );
//	}
}


/*static*/ int XmlBase::IsAlphaNum( uint8_t anyByte, XmlEncoding /*encoding*/ )
{
	// This will only work for low-ascii, everything else is assumed to be a valid
	// letter. I'm not sure this is the best approach, but it is quite tricky trying
	// to figure out alhabetical vs. not across encoding. So take a very 
	// conservative approach.

//	if ( encoding == TIXML_ENCODING_UTF8 )
//	{
		if ( anyByte < 127 )
			return isalnum( anyByte );
		else
			return 1;	// What else to do? The unicode set is huge...get the english ones right.
//	}
//	else
//	{
//		return isalnum( anyByte );
//	}
}


class XmlParsingData
{
	friend class XmlDocument;
  public:
	void Stamp( const char* now, XmlEncoding encoding );

	const XmlCursor& Cursor()	{ return cursor; }

  private:
	// Only used by the document!
	XmlParsingData( const char* start, int tabsize, int row, int col )
	{
		assert( start );
		stamp = start;
		this->tabsize = tabsize;
		cursor.row = row;
		cursor.col = col;
	}

	XmlCursor		cursor;
	const char*		stamp;
	int				tabsize;
};


void XmlParsingData::Stamp( const char* now, XmlEncoding encoding )
{
	assert( now );

	// Do nothing if the tabsize is 0.
	if ( tabsize < 1 )
	{
		return;
	}

	// Get the current row, column.
	int row = cursor.row;
	int col = cursor.col;
	const char* p = stamp;
	assert( p );

	while ( p < now )
	{
		// Treat p as unsigned, so we have a happy compiler.
		const uint8_t* pU = (const uint8_t *)p;

		// Code contributed by Fletcher Dunn: (modified by lee)
		switch (*pU) {
			case 0:
				// We *should* never get here, but in case we do, don't
				// advance past the terminating null character, ever
				return;

			case '\r':
				// bump down to the next line
				++row;
				col = 0;				
				// Eat the character
				++p;

				// Check for \r\n sequence, and treat this as a single character
				if (*p == '\n') {
					++p;
				}
				break;

			case '\n':
				// bump down to the next line
				++row;
				col = 0;

				// Eat the character
				++p;

				// Check for \n\r sequence, and treat this as a single
				// character.  (Yes, this bizarre thing does occur still
				// on some arcane platforms...)
				if (*p == '\r') {
					++p;
				}
				break;

			case '\t':
				// Eat the character
				++p;

				// Skip to next tab stop
				col = (col / tabsize + 1) * tabsize;
				break;

			case TIXML_UTF_LEAD_0:
				if ( encoding == TIXML_ENCODING_UTF8 )
				{
					if ( *(p+1) && *(p+2) )
					{
						// In these cases, don't advance the column. These are
						// 0-width spaces.
						if ( *(pU+1)==TIXML_UTF_LEAD_1 && *(pU+2)==TIXML_UTF_LEAD_2 )
							p += 3;	
						else if ( *(pU+1)==0xbfU && *(pU+2)==0xbeU )
							p += 3;	
						else if ( *(pU+1)==0xbfU && *(pU+2)==0xbfU )
							p += 3;	
						else
							{ p +=3; ++col; }	// A normal character.
					}
				}
				else
				{
					++p;
					++col;
				}
				break;

			default:
				if ( encoding == TIXML_ENCODING_UTF8 )
				{
					// Eat the 1 to 4 byte utf8 character.
					int step = XmlBase::utf8ByteTable[*((const uint8_t *)p)];
					if ( step == 0 )
						step = 1;		// Error case from bad encoding, but handle gracefully.
					p += step;

					// Just advance one column, of course.
					++col;
				}
				else
				{
					++p;
					++col;
				}
				break;
		}
	}
	cursor.row = row;
	cursor.col = col;
	assert( cursor.row >= -1 );
	assert( cursor.col >= -1 );
	stamp = p;
	assert( stamp );
}


const char* XmlBase::SkipWhiteSpace( const char* p, XmlEncoding encoding )
{
	if ( !p || !*p )
	{
		return 0;
	}

	if (encoding == TIXML_ENCODING_UTF8) {
		while (*p) {
			const uint8_t *pU = (const uint8_t *)p;
			
			// Skip the stupid Microsoft UTF-8 Byte order marks
			if (*(pU+0)==TIXML_UTF_LEAD_0 && *(pU+1)==TIXML_UTF_LEAD_1 && *(pU+2)==TIXML_UTF_LEAD_2 ) {
				p += 3;
				continue;
			} else if(*(pU+0)==TIXML_UTF_LEAD_0 && *(pU+1)==0xbfU && *(pU+2)==0xbeU ) {
				p += 3;
				continue;
			} else if(*(pU+0)==TIXML_UTF_LEAD_0 && *(pU+1)==0xbfU && *(pU+2)==0xbfU ) {
				p += 3;
				continue;
			}

			// Still using old rules for white space.
			if ( IsWhiteSpace( *p ) || *p == '\n' || *p =='\r' ) {
				++p;
			} else {
				break;
			}
		}
	} else {
		while (*p && (IsWhiteSpace( *p ) || *p == '\n' || *p =='\r')) {
			++p;
		}
	}

	return p;
}

/*static*/ bool XmlBase::StreamWhiteSpace( std::istream * in, std::string * tag )
{
	for( ;; )
	{
		if ( !in->good() ) return false;

		int c = in->peek();
		// At this scope, we can't get to a document. So fail silently.
		if ( !IsWhiteSpace( c ) || c <= 0 )
			return true;

		*tag += (char) in->get();
	}
}

/*static*/ bool XmlBase::StreamTo( std::istream * in, int character, std::string * tag )
{
	//assert( character > 0 && character < 128 );	// else it won't work in utf-8
	while ( in->good() )
	{
		int c = in->peek();
		if ( c == character )
			return true;
		if ( c <= 0 )		// Silent failure: can't get document at this scope
			return false;

		in->get();
		*tag += (char) c;
	}
	return false;
}

// One of nyXML's more performance demanding functions. Try to keep the memory overhead down. The
// "assign" optimization removes over 10% of the execution time.
//
const char* XmlBase::ReadName( const char* p, std::string * name, XmlEncoding encoding )
{
	// Oddly, not supported on some comilers,
	//name->clear();
	// So use this:
	*name = "";
	assert( p );

	// Names start with letters or underscores.
	// Of course, in unicode, tinyxml has no idea what a letter *is*. The
	// algorithm is generous.
	//
	// After that, they can be letters, underscores, numbers,
	// hyphens, or colons. (Colons are valid ony for namespaces,
	// but tinyxml can't tell namespaces from names.)
	if (    p && *p 
		 && ( IsAlpha( (uint8_t) *p, encoding ) || *p == '_' ) )
	{
		const char* start = p;
		while(		p && *p
				&&	(		IsAlphaNum( (uint8_t) *p, encoding ) 
						 || *p == '_'
						 || *p == '-'
						 || *p == '.'
						 || *p == ':' ) )
		{
			//(*name) += *p; // expensive
			++p;
		}
		if ( p-start > 0 ) {
			name->assign( start, p-start );
		}
		return p;
	}
	return 0;
}

const char* XmlBase::GetEntity( const char* p, char* value, int* length, XmlEncoding encoding )
{
	// Presume an entity, and pull it out.
    std::string ent;
	int i;
	*length = 0;

	if ( *(p+1) && *(p+1) == '#' && *(p+2) )
	{
		unsigned long ucs = 0;
		ptrdiff_t delta = 0;
		unsigned mult = 1;

		if ( *(p+2) == 'x' )
		{
			// Hexadecimal.
			if ( !*(p+3) ) return 0;

			const char* q = p+3;
			q = strchr( q, ';' );

			if ( !q || !*q ) return 0;

			delta = q-p;
			--q;

			while ( *q != 'x' )
			{
				if ( *q >= '0' && *q <= '9' )
					ucs += mult * (*q - '0');
				else if ( *q >= 'a' && *q <= 'f' )
					ucs += mult * (*q - 'a' + 10);
				else if ( *q >= 'A' && *q <= 'F' )
					ucs += mult * (*q - 'A' + 10 );
				else 
					return 0;
				mult *= 16;
				--q;
			}
		}
		else
		{
			// Decimal.
			if ( !*(p+2) ) return 0;

			const char* q = p+2;
			q = strchr( q, ';' );

			if ( !q || !*q ) return 0;

			delta = q-p;
			--q;

			while ( *q != '#' )
			{
				if ( *q >= '0' && *q <= '9' )
					ucs += mult * (*q - '0');
				else 
					return 0;
				mult *= 10;
				--q;
			}
		}
		if ( encoding == TIXML_ENCODING_UTF8 )
		{
			// convert the UCS to UTF-8
			ConvertUTF32ToUTF8( ucs, value, length );
		}
		else
		{
			*value = (char)ucs;
			*length = 1;
		}
		return p + delta + 1;
	}

	// Now try to match it.
	for( i=0; i<NUM_ENTITY; ++i )
	{
		if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 )
		{
			assert( strlen( entity[i].str ) == entity[i].strLength );
			*value = entity[i].chr;
			*length = 1;
			return ( p + entity[i].strLength );
		}
	}

	// So it wasn't an entity, its unrecognized, or something like that.
	*value = *p;	// Don't put back the last one, since we return it!
	//*length = 1;	// Leave unrecognized entities - this doesn't really work.
					// Just writes strange XML.
	return p+1;
}


bool XmlBase::StringEqual( const char* p,
							 const char* tag,
							 bool ignoreCase,
							 XmlEncoding encoding )
{
	assert( p );
	assert( tag );
	if ( !p || !*p )
	{
		assert( 0 );
		return false;
	}

	const char* q = p;

	if ( ignoreCase )
	{
		while ( *q && *tag && ToLower( *q, encoding ) == ToLower( *tag, encoding ) )
		{
			++q;
			++tag;
		}

		if ( *tag == 0 )
			return true;
	}
	else
	{
		while ( *q && *tag && *q == *tag )
		{
			++q;
			++tag;
		}

		if ( *tag == 0 )		// Have we found the end of the tag, and everything equal?
			return true;
	}
	return false;
}

const char* XmlBase::ReadText(	const char* p, 
									std::string * text, 
									bool trimWhiteSpace, 
									const char* endTag, 
									bool caseInsensitive,
									XmlEncoding encoding )
{
    *text = "";
	if (    !trimWhiteSpace			// certain tags always keep whitespace
		 || !condenseWhiteSpace )	// if true, whitespace is always kept
	{
		// Keep all the white space.
		while (	   p && *p
				&& !StringEqual( p, endTag, caseInsensitive, encoding )
			  )
		{
			int len;
			char cArr[4] = { 0, 0, 0, 0 };
			p = GetChar( p, cArr, &len, encoding );
			text->append( cArr, len );
		}
	}
	else
	{
		bool whitespace = false;

		// Remove leading white space:
		p = SkipWhiteSpace( p, encoding );
		while (	   p && *p
				&& !StringEqual( p, endTag, caseInsensitive, encoding ) )
		{
			if ( *p == '\r' || *p == '\n' )
			{
				whitespace = true;
				++p;
			}
			else if ( IsWhiteSpace( *p ) )
			{
				whitespace = true;
				++p;
			}
			else
			{
				// If we've found whitespace, add it before the
				// new character. Any whitespace just becomes a space.
				if ( whitespace )
				{
					(*text) += ' ';
					whitespace = false;
				}
				int len;
				char cArr[4] = { 0, 0, 0, 0 };
				p = GetChar( p, cArr, &len, encoding );
				if ( len == 1 )
					(*text) += cArr[0];	// more efficient
				else
					text->append( cArr, len );
			}
		}
	}
	if ( p ) 
		p += strlen( endTag );
	return p;
}


void XmlDocument::StreamIn( std::istream * in, std::string * tag )
{
	// The basic issue with a document is that we don't know what we're
	// streaming. Read something presumed to be a tag (and hope), then
	// identify it, and call the appropriate stream method on the tag.
	//
	// This "pre-streaming" will never read the closing ">" so the
	// sub-tag can orient itself.

	if ( !StreamTo( in, '<', tag ) ) 
	{
		SetError( TIXML_ERROR_PARSING_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return;
	}

	while ( in->good() )
	{
		int tagIndex = (int) tag->length();
		while ( in->good() && in->peek() != '>' )
		{
			int c = in->get();
			if ( c <= 0 )
			{
				SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
				break;
			}
			(*tag) += (char) c;
		}

		if ( in->good() )
		{
			// We now have something we presume to be a node of 
			// some sort. Identify it, and call the node to
			// continue streaming.
			XmlNode* node = Identify( tag->c_str() + tagIndex, TIXML_DEFAULT_ENCODING );

			if ( node )
			{
				node->StreamIn( in, tag );
				bool isElement = node->ToElement() != 0;
				delete node;
				node = 0;

				// If this is the root element, we're done. Parsing will be
				// done by the >> operator.
				if ( isElement )
				{
					return;
				}
			}
			else
			{
				SetError( TIXML_ERROR, 0, 0, TIXML_ENCODING_UNKNOWN );
				return;
			}
		}
	}
	// We should have returned sooner.
	SetError( TIXML_ERROR, 0, 0, TIXML_ENCODING_UNKNOWN );
}


const char* XmlDocument::Parse( const char* p, XmlParsingData* prevData, XmlEncoding encoding )
{
	ClearError();

	// Parse away, at the document level. Since a document
	// contains nothing but other tags, most of what happens
	// here is skipping white space.
	if ( !p || !*p )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	// Note that, for a document, this needs to come
	// before the while space skip, so that parsing
	// starts from the pointer we are given.
	location.Clear();
	if ( prevData )
	{
		location.row = prevData->cursor.row;
		location.col = prevData->cursor.col;
	}
	else
	{
		location.row = 0;
		location.col = 0;
	}
	XmlParsingData data( p, TabSize(), location.row, location.col );
	location = data.Cursor();

	if ( encoding == TIXML_ENCODING_UNKNOWN )
	{
		// Check for the Microsoft UTF-8 lead bytes.
		const uint8_t *pU = (const uint8_t *)p;
		if (	*(pU+0) && *(pU+0) == TIXML_UTF_LEAD_0
			 && *(pU+1) && *(pU+1) == TIXML_UTF_LEAD_1
			 && *(pU+2) && *(pU+2) == TIXML_UTF_LEAD_2 )
		{
			encoding = TIXML_ENCODING_UTF8;
			useMicrosoftBOM = true;
		}
	}

    p = SkipWhiteSpace( p, encoding );
	if ( !p )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}

	while ( p && *p )
	{
		XmlNode* node = Identify( p, encoding );
		if ( node )
		{
			p = node->Parse( p, &data, encoding );
			LinkEndChild( node );
		}
		else
		{
			break;
		}

		// Did we get encoding info?
		if (    encoding == TIXML_ENCODING_UNKNOWN
			 && node->ToDeclaration() )
		{
			XmlDeclaration* dec = node->ToDeclaration();
			const char* enc = dec->Encoding();
			assert( enc );

			if ( *enc == 0 )
				encoding = TIXML_ENCODING_UTF8;
			else if ( StringEqual( enc, "UTF-8", true, TIXML_ENCODING_UNKNOWN ) )
				encoding = TIXML_ENCODING_UTF8;
			else if ( StringEqual( enc, "UTF8", true, TIXML_ENCODING_UNKNOWN ) )
				encoding = TIXML_ENCODING_UTF8;	// incorrect, but be nice
			else 
				encoding = TIXML_ENCODING_LEGACY;
		}

		p = SkipWhiteSpace( p, encoding );
	}

	// Was this empty?
	if ( !_firstChild ) {
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, encoding );
		return 0;
	}

	// All is well.
	return p;
}

void XmlDocument::SetError( int err, const char* pError, XmlParsingData* data, XmlEncoding encoding )
{	
	// The first error in a chain is more accurate - don't set again!
	if ( error )
		return;

	assert( err > 0 && err < TIXML_ERROR_STRING_COUNT );
	error   = true;
	errorId = err;
	errorDesc = errorString[ errorId ];

	errorLocation.Clear();
	if ( pError && data )
	{
		data->Stamp( pError, encoding );
		errorLocation = data->Cursor();
	}
}

XmlNode * XmlNode::Identify( const char* p, XmlEncoding encoding )
{
	XmlNode* returnNode = 0;

	p = SkipWhiteSpace( p, encoding );
	if( !p || !*p || *p != '<' )
	{
		return 0;
	}

	XmlDocument* doc = GetDocument();
	p = SkipWhiteSpace( p, encoding );

	if ( !p || !*p )
	{
		return 0;
	}

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Decleration: <?xml
	// - Everthing else is unknown to tinyxml.
	//

	const char* xmlHeader = { "<?xml" };
	const char* commentHeader = { "<!--" };
	const char* dtdHeader = { "<!" };
	const char* cdataHeader = { "<![CDATA[" };

	try {
		if ( StringEqual( p, xmlHeader, true, encoding ) ) {
			returnNode = new XmlDeclaration();
		} else if ( StringEqual( p, commentHeader, false, encoding ) ) {
			returnNode = new XmlComment();
		} else if ( StringEqual( p, cdataHeader, false, encoding ) ) {
			XmlText* text = new XmlText( "" );
			text->SetCDATA( true );
			returnNode = text;
		} else if ( StringEqual( p, dtdHeader, false, encoding ) ) {
			returnNode = new XmlUnknown();
		} else if (    IsAlpha( *(p+1), encoding ) || *(p+1) == '_' ) {
			returnNode = new XmlElement( "" );
		} else {
			returnNode = new XmlUnknown();
		}
	} catch (std::bad_alloc &) {
		returnNode = NULL;
	}

	if ( returnNode ) {
		// Set the parent, so it can report errors
		returnNode->_parent = this;
	} else {
		if ( doc )
			doc->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN );
	}

	return returnNode;
}

void XmlElement::StreamIn (std::istream * in, std::string * tag)
{
	// We're called with some amount of pre-parsing. That is, some of "this"
	// element is in "tag". Go ahead and stream to the closing ">"
	while( in->good() )
	{
		int c = in->get();
		if ( c <= 0 )
		{
			XmlDocument* document = GetDocument();
			if ( document )
				document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
			return;
		}
		(*tag) += (char) c ;
		
		if ( c == '>' )
			break;
	}

	if ( tag->length() < 3 ) return;

	// Okay...if we are a "/>" tag, then we're done. We've read a complete tag.
	// If not, identify and stream.

	if (    tag->at( tag->length() - 1 ) == '>' 
		 && tag->at( tag->length() - 2 ) == '/' )
	{
		// All good!
		return;
	}
	else if ( tag->at( tag->length() - 1 ) == '>' )
	{
		// There is more. Could be:
		//		text
		//		cdata text (which looks like another node)
		//		closing tag
		//		another node.
		for ( ;; )
		{
			StreamWhiteSpace( in, tag );

			// Do we have text?
			if ( in->good() && in->peek() != '<' ) 
			{
				// Yep, text.
				XmlText text( "" );
				text.StreamIn( in, tag );

				// What follows text is a closing tag or another node.
				// Go around again and figure it out.
				continue;
			}

			// We now have either a closing tag...or another node.
			// We should be at a "<", regardless.
			if ( !in->good() ) return;
			assert( in->peek() == '<' );
			int tagIndex = (int) tag->length();

			bool closingTag = false;
			bool firstCharFound = false;

			for( ;; )
			{
				if ( !in->good() )
					return;

				int c = in->peek();
				if ( c <= 0 )
				{
					XmlDocument* document = GetDocument();
					if ( document )
						document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
					return;
				}
				
				if ( c == '>' )
					break;

				*tag += (char) c;
				in->get();

				// Early out if we find the CDATA id.
				if ( c == '[' && tag->size() >= 9 )
				{
					size_t len = tag->size();
					const char* start = tag->c_str() + len - 9;
					if ( strcmp( start, "<![CDATA[" ) == 0 ) {
						assert( !closingTag );
						break;
					}
				}

				if ( !firstCharFound && c != '<' && !IsWhiteSpace( c ) )
				{
					firstCharFound = true;
					if ( c == '/' )
						closingTag = true;
				}
			}
			// If it was a closing tag, then read in the closing '>' to clean up the input stream.
			// If it was not, the streaming will be done by the tag.
			if ( closingTag )
			{
				if ( !in->good() )
					return;

				int c = in->get();
				if ( c <= 0 )
				{
					XmlDocument* document = GetDocument();
					if ( document )
						document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
					return;
				}
				assert( c == '>' );
				*tag += (char) c;

				// We are done, once we've found our closing tag.
				return;
			}
			else
			{
				// If not a closing tag, id it, and stream.
				const char* tagloc = tag->c_str() + tagIndex;
				XmlNode* node = Identify( tagloc, TIXML_DEFAULT_ENCODING );
				if ( !node )
					return;
				node->StreamIn( in, tag );
				delete node;
				node = 0;

				// No return: go around from the beginning: text, closing tag, or node.
			}
		}
	}
}

const char * XmlElement::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
	p = SkipWhiteSpace( p, encoding );
	XmlDocument* document = GetDocument();

	if ( !p || !*p )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, 0, 0, encoding );
		return 0;
	}

	if ( data )
	{
		data->Stamp( p, encoding );
		location = data->Cursor();
	}

	if ( *p != '<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, p, data, encoding );
		return 0;
	}

	p = SkipWhiteSpace( p+1, encoding );

	// Read the name.
	const char* pErr = p;

   p = ReadName( p, &_value, encoding );
	if ( !p || !*p ) {
		if ( document )	document->SetError( TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME, pErr, data, encoding );
		return 0;
	}

	std::string endTag ("</");

	endTag += _value;
	endTag += ">";

	// Check for and read attributes. Also look for an empty
	// tag or an end tag.
	while ( p && *p )
	{
		pErr = p;
		p = SkipWhiteSpace( p, encoding );
		if ( !p || !*p )
		{
			if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
			return 0;
		}
		if ( *p == '/' )
		{
			++p;
			// Empty tag.
			if ( *p  != '>' )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_EMPTY, p, data, encoding );		
				return 0;
			}
			return (p+1);
		}
		else if ( *p == '>' )
		{
			// Done with attributes (if there were any.)
			// Read the value -- which can include other
			// elements -- read the end tag, and return.
			++p;
			p = ReadValue( p, data, encoding );		// Note this is an Element method, and will set the error if one happens.
			if ( !p || !*p )
				return 0;

			// We should find the end tag now
			if ( StringEqual( p, endTag.c_str(), false, encoding ) )
			{
				p += endTag.length();
				return p;
			}
			else
			{
				if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG, p, data, encoding );
				return 0;
			}
		}
		else
		{
			try {
				// Try to read an attribute:
				XmlAttribute* attrib = new XmlAttribute();

				if (!attrib) {
					if (document) {
						document->SetError( TIXML_ERROR_OUT_OF_MEMORY, pErr, data, encoding );
					}

					return 0;
				}

				attrib->SetDocument( document );
				pErr = p;
				p = attrib->Parse( p, data, encoding );

				if (!p || !*p) {
					if (document) {
						document->SetError( TIXML_ERROR_PARSING_ELEMENT, pErr, data, encoding );
					}

					delete attrib;

					return 0;
				}

				// Handle the strange case of double attributes:
				XmlAttribute* node = attributeSet.Find( attrib->NameTStr() );
				if (node) {
					node->SetValue( attrib->Value() );

					delete attrib;

					return 0;
				}

				attributeSet.Add( attrib );
			} catch (std::bad_alloc &) {
			}
		}
	}
	return p;
}


const char * XmlElement::ReadValue(const char* p, XmlParsingData* data, XmlEncoding encoding )
{
	XmlDocument* document = GetDocument();

	// Read in text and elements in any order.
	const char* pWithWhiteSpace = p;
	p = SkipWhiteSpace( p, encoding );

	while ( p && *p ) {
		if (*p != '<') {
			try {
				// Take what we have, make a text element.
				XmlText* textNode = new XmlText( "" );

				if ( XmlBase::IsWhiteSpaceCondensed() ) {
					p = textNode->Parse( p, data, encoding );
				} else {
					// Special case: we want to keep the white space so that leading spaces aren't removed.
					p = textNode->Parse( pWithWhiteSpace, data, encoding );
				}

				if ( !textNode->Blank() ) {
					LinkEndChild( textNode );
				} else {
					delete textNode;
				}
			} catch (std::bad_alloc &) {
				if (document) {
					document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, encoding );
				}

				return 0;
			}
		} else {
			// We hit a '<'
			// Have we hit a new element or an end tag? This could also be
			// a XmlText in the "CDATA" style.
			if ( StringEqual( p, "</", false, encoding ) ) {
				return p;
			} else {
				XmlNode* node = Identify( p, encoding );
				if ( node ) {
					p = node->Parse( p, data, encoding );
					LinkEndChild( node );
				} else {
					return 0;
				}
			}
		}
		pWithWhiteSpace = p;
		p = SkipWhiteSpace( p, encoding );
	}

	if ( !p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ELEMENT_VALUE, 0, 0, encoding );
	}	
	return p;
}


void XmlUnknown::StreamIn( std::istream * in, std::string * tag )
{
	while ( in->good() )
	{
		int c = in->get();	
		if ( c <= 0 )
		{
			XmlDocument* document = GetDocument();
			if ( document )
				document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
			return;
		}
		(*tag) += (char) c;

		if ( c == '>' )
		{
			// All is well.
			return;		
		}
	}
}


const char* XmlUnknown::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
	XmlDocument* document = GetDocument();
	p = SkipWhiteSpace( p, encoding );

	if ( data )
	{
		data->Stamp( p, encoding );
		location = data->Cursor();
	}
	if ( !p || !*p || *p != '<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_UNKNOWN, p, data, encoding );
		return 0;
	}

	_value = "";
	++p;

	while ( p && *p && *p != '>' ) {
		_value += *p;
		++p;
	}

	if ( !p )
	{
		if ( document )	document->SetError( TIXML_ERROR_PARSING_UNKNOWN, 0, 0, encoding );
	}
	if ( *p == '>' )
		return p+1;
	return p;
}

void XmlComment::StreamIn( std::istream * in, std::string * tag )
{
	while ( in->good() )
	{
		int c = in->get();	
		if ( c <= 0 )
		{
			XmlDocument* document = GetDocument();
			if ( document )
				document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
			return;
		}

		(*tag) += (char) c;

		if ( c == '>' 
			 && tag->at( tag->length() - 2 ) == '-'
			 && tag->at( tag->length() - 3 ) == '-' )
		{
			// All is well.
			return;		
		}
	}
}


const char* XmlComment::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
	XmlDocument* document = GetDocument();

	_value = "";

	p = SkipWhiteSpace( p, encoding );

	if ( data )
	{
		data->Stamp( p, encoding );
		location = data->Cursor();
	}
	const char* startTag = "<!--";
	const char* endTag   = "-->";

	if ( !StringEqual( p, startTag, false, encoding ) )
	{
		document->SetError( TIXML_ERROR_PARSING_COMMENT, p, data, encoding );
		return 0;
	}
	p += strlen( startTag );
	p = ReadText( p, &_value, false, endTag, false, encoding );
	return p;
}


const char* XmlAttribute::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
	p = SkipWhiteSpace( p, encoding );
	if ( !p || !*p ) return 0;

//	int tabsize = 4;
//	if ( document )
//		tabsize = document->TabSize();

	if ( data )
	{
		data->Stamp( p, encoding );
		location = data->Cursor();
	}
	// Read the name, the '=' and the value.
	const char* pErr = p;
	p = ReadName( p, &_name, encoding );
	if ( !p || !*p ) {
		if ( _document ) {
			_document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data, encoding );
		}
		return 0;
	}
	p = SkipWhiteSpace( p, encoding );
	if ( !p || !*p || *p != '=' ) {
		if ( _document ) {
			_document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
		}
		return 0;
	}

	++p;	// skip '='
	p = SkipWhiteSpace( p, encoding );
	if ( !p || !*p ) {
		if ( _document ) {
			_document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
		}
		return 0;
	}
	
	const char* end;
	const char SINGLE_QUOTE = '\'';
	const char DOUBLE_QUOTE = '\"';

	if ( *p == SINGLE_QUOTE ) {
		++p;
		end = "\'";		// single quote in string
		p = ReadText( p, &_value, false, end, false, encoding );
	} else if ( *p == DOUBLE_QUOTE ) {
		++p;
		end = "\"";		// double quote in string
		p = ReadText( p, &_value, false, end, false, encoding );
	} else {
		// All attribute values should be in single or double quotes.
		// But this is such a common error that the parser will try
		// its best, even without them.
		_value = "";
		while (p && *p // existence
				&& !IsWhiteSpace( *p ) && *p != '\n' && *p != '\r'	// whitespace
				&& *p != '/' && *p != '>' )	// tag end
		{
			if ( *p == SINGLE_QUOTE || *p == DOUBLE_QUOTE ) {
				// [ 1451649 ] Attribute values with trailing quotes not handled correctly
				// We did not have an opening quote but seem to have a 
				// closing one. Give up and throw an error.
				if ( _document ) {
					_document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data, encoding );
				}
				return 0;
			}
			_value += *p;
			++p;
		}
	}
	return p;
}

void XmlText::StreamIn( std::istream * in, std::string * tag )
{
	while ( in->good() ) {
		int c = in->peek();	
		if ( !_cdata && (c == '<' ) ) {
			return;
		}
		if ( c <= 0 ) {
			XmlDocument* document = GetDocument();
			if ( document ) {
				document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
			}
			return;
		}

		(*tag) += (char) c;
		in->get();	// "commits" the peek made above

		if ( _cdata && c == '>' && tag->size() >= 3 ) {
			size_t len = tag->size();
			if ( (*tag)[len-2] == ']' && (*tag)[len-3] == ']' ) {
				// terminator of cdata.
				return;
			}
		}    
	}
}

const char* XmlText::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
	XmlDocument* document = GetDocument();
	
	_value = "";

	if ( data ) {
		data->Stamp( p, encoding );
		location = data->Cursor();
	}

	const char* const startTag = "<![CDATA[";
	const char* const endTag   = "]]>";

	if ( _cdata || StringEqual( p, startTag, false, encoding ) ) {
		_cdata = true;

		if ( !StringEqual( p, startTag, false, encoding ) ) {
			document->SetError( TIXML_ERROR_PARSING_CDATA, p, data, encoding );
			return 0;
		}

		p += strlen( startTag );

		// Keep all the white space, ignore the encoding, etc.
		while (	p && *p && !StringEqual( p, endTag, false, encoding )) {
			_value += *p;
			++p;
		}

		std::string dummy; 
		p = ReadText( p, &dummy, false, endTag, false, encoding );
		return p;
	} else {
		bool ignoreWhite = true;

		const char* end = "<";
		p = ReadText( p, &_value, ignoreWhite, end, false, encoding );
		if ( p )
			return p-1;	// don't truncate the '<'
		return 0;
	}
}

void XmlDeclaration::StreamIn( std::istream * in, std::string * tag )
{
	while ( in->good() )
	{
		int c = in->get();
		if ( c <= 0 )
		{
			XmlDocument* document = GetDocument();
			if ( document )
				document->SetError( TIXML_ERROR_EMBEDDED_NULL, 0, 0, TIXML_ENCODING_UNKNOWN );
			return;
		}
		(*tag) += (char) c;

		if ( c == '>' )
		{
			// All is well.
			return;
		}
	}
}

const char* XmlDeclaration::Parse( const char* p, XmlParsingData* data, XmlEncoding encoding )
{
	p = SkipWhiteSpace( p, encoding );
	// Find the beginning, find the end, and look for
	// the stuff in-between.
	XmlDocument* document = GetDocument();
	if ( !p || !*p || !StringEqual( p, "<?xml", true, encoding ) ) {
		if ( document ) document->SetError( TIXML_ERROR_PARSING_DECLARATION, 0, 0, encoding );
		return 0;
	}
	if ( data ) {
		data->Stamp( p, encoding );
		location = data->Cursor();
	}
	p += 5;

	_version = "";
	_encoding = "";
	_standalone = "";

	while ( p && *p ) {
		if ( *p == '>' ) {
			++p;
			return p;
		}

		p = SkipWhiteSpace( p, encoding );
		if ( StringEqual( p, "version", true, encoding ) ) {
			XmlAttribute attrib;
			p = attrib.Parse( p, data, encoding );		
			_version = attrib.Value();
		} else if ( StringEqual( p, "encoding", true, encoding ) ) {
			XmlAttribute attrib;
			p = attrib.Parse( p, data, encoding );		
			_encoding = attrib.Value();
		} else if ( StringEqual( p, "standalone", true, encoding ) ) {
			XmlAttribute attrib;
			p = attrib.Parse( p, data, encoding );		
			_standalone = attrib.Value();
		} else {
			// Read over whatever it is.
			while( p && *p && *p != '>' && !IsWhiteSpace( *p ) ) {
				++p;
			}
		}
	}
	return 0;
}

bool XmlText::Blank() const
{
	for ( unsigned i=0; i<_value.length(); i++ ) {
		if ( !IsWhiteSpace( _value[i] ) ) {
			return false;
		}
	}

	return true;
}

// INIT XML 

bool XmlBase::condenseWhiteSpace = true;

void XmlBase::PutString( const std::string& str, std::string* outString )
{
	int i=0;

	while( i<(int)str.length() )
	{
		uint8_t c = (uint8_t) str[i];

		if (    c == '&' 
		     && i < ( (int)str.length() - 2 )
			 && str[i+1] == '#'
			 && str[i+2] == 'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while ( i<(int)str.length()-1 )
			{
				outString->append( str.c_str() + i, 1 );
				++i;
				if ( str[i] == ';' )
					break;
			}
		}
		else if ( c == '&' )
		{
			outString->append( entity[0].str, entity[0].strLength );
			++i;
		}
		else if ( c == '<' )
		{
			outString->append( entity[1].str, entity[1].strLength );
			++i;
		}
		else if ( c == '>' )
		{
			outString->append( entity[2].str, entity[2].strLength );
			++i;
		}
		else if ( c == '\"' )
		{
			outString->append( entity[3].str, entity[3].strLength );
			++i;
		}
		else if ( c == '\'' )
		{
			outString->append( entity[4].str, entity[4].strLength );
			++i;
		}
		else if ( c < 32 )
		{
			// Easy pass at non-alpha/numeric/symbol
			// Below 32 is symbolic.
			char buf[ 32 ];
			
			#if defined(TIXML_SNPRINTF)		
				TIXML_SNPRINTF( buf, sizeof(buf), "&#x%02X;", (unsigned) ( c & 0xff ) );
			#else
				sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
			#endif		

			//*ME:	warning C4267: convert 'size_t' to 'int'
			//*ME:	Int-Cast to make compiler happy ...
			outString->append( buf, (int)strlen( buf ) );
			++i;
		}
		else
		{
			//char realc = (char) c;
			//outString->append( &realc, 1 );
			*outString += (char) c;	// somewhat more efficient function call.
			++i;
		}
	}
}


XmlNode::XmlNode( NodeType type ) : XmlBase()
{
	_parent = 0;
	_type = type;
	_firstChild = 0;
	_lastChild = 0;
	_prev = 0;
	_next = 0;
}


XmlNode::~XmlNode()
{
	XmlNode* node = _firstChild;
	XmlNode* temp = 0;

	while ( node ) {
		temp = node;
		node = node->_next;
		delete temp;
	}	
}


void XmlNode::CopyTo( XmlNode* target ) const
{
	target->SetValue (_value.c_str() );
	target->userData = userData; 
}


void XmlNode::Clear()
{
	XmlNode* node = _firstChild;
	XmlNode* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->_next;
		delete temp;
	}	

	_firstChild = 0;
	_lastChild = 0;
}


XmlNode* XmlNode::LinkEndChild( XmlNode* node )
{
	assert( node->_parent == 0 || node->_parent == this );
	assert( node->GetDocument() == 0 || node->GetDocument() == this->GetDocument() );

	if ( node->Type() == XmlNode::DOCUMENT ) {
		delete node;

		if ( GetDocument() ) {
			GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		}

		return 0;
	}

	node->_parent = this;

	node->_prev = _lastChild;
	node->_next = 0;

	if ( _lastChild ) {
		_lastChild->_next = node;
	} else {
		_firstChild = node;			// it was an empty list.
	}

	_lastChild = node;

	return node;
}


XmlNode* XmlNode::InsertEndChild( const XmlNode& addThis )
{
	if ( addThis.Type() == XmlNode::DOCUMENT )
	{
		if ( GetDocument() ) GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return 0;
	}
	XmlNode* node = addThis.Clone();
	if ( !node )
		return 0;

	return LinkEndChild( node );
}


XmlNode* XmlNode::InsertBeforeChild( XmlNode* beforeThis, const XmlNode& addThis )
{	
	if ( !beforeThis || beforeThis->_parent != this ) {
		return 0;
	}

	if ( addThis.Type() == XmlNode::DOCUMENT ) {
		if ( GetDocument() ) {
			GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		}

		return 0;
	}

	XmlNode* node = addThis.Clone();
	
	if ( !node ) {
		return 0;
	}
	
	node->_parent = this;

	node->_next = beforeThis;
	node->_prev = beforeThis->_prev;
	if ( beforeThis->_prev ) {
		beforeThis->_prev->_next = node;
	} else {
		assert( _firstChild == beforeThis );
		_firstChild = node;
	}
	beforeThis->_prev = node;
	return node;
}


XmlNode* XmlNode::InsertAfterChild( XmlNode* afterThis, const XmlNode& addThis )
{
	if ( !afterThis || afterThis->_parent != this ) {
		return 0;
	}

	if ( addThis.Type() == XmlNode::DOCUMENT ) {
		if ( GetDocument() ) {
			GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
		}

		return 0;
	}

	XmlNode* node = addThis.Clone();

	if ( !node ) {
		return 0;
	}

	node->_parent = this;

	node->_prev = afterThis;
	node->_next = afterThis->_next;
	if ( afterThis->_next ) {
		afterThis->_next->_prev = node;
	} else {
		assert( _lastChild == afterThis );
		_lastChild = node;
	}
	afterThis->_next = node;
	return node;
}


XmlNode* XmlNode::ReplaceChild( XmlNode* replaceThis, const XmlNode& withThis )
{
	if ( replaceThis->_parent != this )
		return 0;

	XmlNode* node = withThis.Clone();
	
	if ( !node ) {
		return 0;
	}

	node->_next = replaceThis->_next;
	node->_prev = replaceThis->_prev;

	if ( replaceThis->_next ) {
		replaceThis->_next->_prev = node;
	} else {
		_lastChild = node;
	}

	if ( replaceThis->_prev ) {
		replaceThis->_prev->_next = node;
	} else {
		_firstChild = node;
	}

	delete replaceThis;
	node->_parent = this;

	return node;
}


bool XmlNode::RemoveChild( XmlNode* removeThis )
{
	if ( removeThis->_parent != this ) {	
		assert( 0 );
		return false;
	}

	if ( removeThis->_next ) {
		removeThis->_next->_prev = removeThis->_prev;
	} else {
		_lastChild = removeThis->_prev;
	}

	if ( removeThis->_prev ) {
		removeThis->_prev->_next = removeThis->_next;
	} else {
		_firstChild = removeThis->_next;
	}

	delete removeThis;

	return true;
}

const XmlNode* XmlNode::FirstChild( const char * value ) const
{
	const XmlNode* node;

	for ( node = _firstChild; node; node = node->_next ) {
		if ( strcmp( node->Value(), value ) == 0 )
			return node;
	}

	return 0;
}


const XmlNode* XmlNode::LastChild( const char * value ) const
{
	const XmlNode* node;

	for ( node = _lastChild; node; node = node->_prev ) {
		if ( strcmp( node->Value(), value ) == 0 )
			return node;
	}

	return 0;
}


const XmlNode* XmlNode::IterateChildren( const XmlNode* previous ) const
{
	if ( !previous ) {
		return FirstChild();
	} else {
		assert( previous->_parent == this );
		return previous->NextSibling();
	}
}


const XmlNode* XmlNode::IterateChildren( const char * val, const XmlNode* previous ) const
{
	if ( !previous ) {
		return FirstChild( val );
	} else {
		assert( previous->_parent == this );
		return previous->NextSibling( val );
	}
}


const XmlNode* XmlNode::NextSibling( const char * value ) const 
{
	const XmlNode* node;

	for ( node = _next; node; node = node->_next ) {
		if ( strcmp( node->Value(), value ) == 0 ) {
			return node;
		}
	}

	return 0;
}

const XmlNode* XmlNode::PreviousSibling( const char * value ) const
{
	const XmlNode* node;

	for ( node = _prev; node; node = node->_prev ) {
		if ( strcmp( node->Value(), value ) == 0 ) {
			return node;
		}
	}

	return 0;
}


void XmlElement::RemoveAttribute( const char * name )
{
	std::string str( name );
	XmlAttribute* node = attributeSet.Find( str );
	
	if ( node ) {
		attributeSet.Remove( node );
		delete node;
	}
}

const XmlElement* XmlNode::FirstChildElement() const
{
	const XmlNode* node;

	for (	node = FirstChild(); node; node = node->NextSibling() ) {
		if ( node->ToElement() ) {
			return node->ToElement();
		}
	}

	return 0;
}

const XmlElement* XmlNode::FirstChildElement( const char * value ) const
{
	const XmlNode* node;

	for (	node = FirstChild( value ); node; node = node->NextSibling( value ) ) {
		if ( node->ToElement() ) {
			return node->ToElement();
		}
	}

	return 0;
}


const XmlElement* XmlNode::NextSiblingElement() const
{
	const XmlNode* node;

	for (	node = NextSibling(); node; node = node->NextSibling() ) {
		if ( node->ToElement() ) {
			return node->ToElement();
		}
	}

	return 0;
}


const XmlElement* XmlNode::NextSiblingElement( const char * value ) const
{
	const XmlNode* node;

	for (	node = NextSibling( value ); node; node = node->NextSibling( value ) ) {
		if ( node->ToElement() ) {
			return node->ToElement();
		}
	}
	
	return 0;
}


const XmlDocument* XmlNode::GetDocument() const
{
	const XmlNode* node;

	for( node = this; node; node = node->_parent ) {
		if ( node->ToDocument() ) {
			return node->ToDocument();
		}
	}

	return 0;
}


XmlElement::XmlElement (const char * value)
	: XmlNode( XmlNode::ELEMENT )
{
	_firstChild = 0;
	_lastChild = 0;
	_value = value;
}


XmlElement::XmlElement( const std::string& value ) 
	: XmlNode( XmlNode::ELEMENT )
{
	_firstChild = 0;
	_lastChild = 0;
	_value = value;
}


XmlElement::XmlElement( const XmlElement& copy)
	: XmlNode( XmlNode::ELEMENT )
{
	_firstChild = 0;
	_lastChild = 0;
	copy.CopyTo( this );	
}


void XmlElement::operator=( const XmlElement& base )
{
	ClearThis();
	base.CopyTo( this );
}


XmlElement::~XmlElement()
{
	ClearThis();
}


void XmlElement::ClearThis()
{
	Clear();

	while( attributeSet.First() ) {
		XmlAttribute* node = attributeSet.First();
		attributeSet.Remove( node );
		delete node;
	}
}

const char* XmlElement::Attribute( std::string name ) const
{
	return Attribute((const char *)name.c_str());
}

const char* XmlElement::Attribute( std::string name, int* i ) const
{
	return Attribute((const char *)name.c_str(), i);
}

const char* XmlElement::Attribute( std::string name, double* d ) const
{
	return Attribute((const char *)name.c_str(), d);
}

int XmlElement::QueryIntAttribute( const char* name, int* ival ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue( ival );
}

const char* XmlElement::Attribute( const char* name ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( node )
		return node->Value();
	return 0;
}

const char* XmlElement::Attribute( const char* name, int* i ) const
{
	const char* s = Attribute( name );
	if ( i )
	{
		if ( s ) {
			*i = atoi( s );
		}
		else {
			*i = 0;
		}
	}
	return s;
}

const char* XmlElement::Attribute( const char* name, double* d ) const
{
	const char* s = Attribute( name );
	if ( d )
	{
		if ( s ) {
			*d = atof( s );
		}
		else {
			*d = 0;
		}
	}
	return s;
}

int XmlElement::QueryIntAttribute( const std::string& name, int* ival ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryIntValue( ival );
}


int XmlElement::QueryDoubleAttribute( const char* name, double* dval ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue( dval );
}


int XmlElement::QueryDoubleAttribute( const std::string& name, double* dval ) const
{
	const XmlAttribute* node = attributeSet.Find( name );
	if ( !node )
		return TIXML_NO_ATTRIBUTE;
	return node->QueryDoubleValue( dval );
}


void XmlElement::SetAttribute( const char * name, int val )
{	
	char buf[64];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF( buf, sizeof(buf), "%d", val );
	#else
		sprintf( buf, "%d", val );
	#endif
	SetAttribute( name, buf );
}


void XmlElement::SetAttribute( const std::string& name, int val )
{	
   std::ostringstream oss;
   oss << val;
   SetAttribute( name, oss.str() );
}


void XmlElement::SetDoubleAttribute( const char * name, double val )
{	
	char buf[256];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF( buf, sizeof(buf), "%f", val );
	#else
		sprintf( buf, "%f", val );
	#endif
	SetAttribute( name, buf );
}


void XmlElement::SetAttribute( const char * cname, const char * cvalue )
{
	std::string name( cname );
	std::string value( cvalue );

	XmlAttribute* node = attributeSet.Find( name );
	if ( node ) {
		node->SetValue( value );

		return;
	}

	try {
		XmlAttribute* attrib = new XmlAttribute( cname, cvalue );
		if ( attrib ) {
			attributeSet.Add( attrib );
		}
	} catch (std::bad_alloc &) {
		XmlDocument* document = GetDocument();

		if ( document ) {
			document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN );
		}
	}
}


void XmlElement::SetAttribute( const std::string& name, const std::string& value )
{
	XmlAttribute* node = attributeSet.Find( name );

	if ( node ) {
		node->SetValue( value );
		return;
	}

	try {
		XmlAttribute* attrib = new XmlAttribute( name, value );
		if ( attrib ) {
			attributeSet.Add( attrib );
		}
	} catch (std::bad_alloc &) {
		XmlDocument* document = GetDocument();

		if ( document ) {
			document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0, TIXML_ENCODING_UNKNOWN );
		}
	}
}


void XmlElement::Print( FILE* cfile, int depth ) const
{
	int i;
	assert( cfile );
	for ( i=0; i<depth; i++ ) {
		fprintf( cfile, "    " );
	}

	fprintf( cfile, "<%s", _value.c_str() );

	const XmlAttribute* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{
		fprintf( cfile, " " );
		attrib->Print( cfile, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	XmlNode* node;
	if ( !_firstChild ) {
		fprintf( cfile, " />" );
	} else if ( _firstChild == _lastChild && _firstChild->ToText() ) {
		fprintf( cfile, ">" );
		_firstChild->Print( cfile, depth + 1 );
		fprintf( cfile, "</%s>", _value.c_str() );
	} else {
		fprintf( cfile, ">" );

		for ( node = _firstChild; node; node=node->NextSibling() ) {
			if ( !node->ToText() ) {
				fprintf( cfile, "\n" );
			}
			node->Print( cfile, depth+1 );
		}
		fprintf( cfile, "\n" );
		for( i=0; i<depth; ++i ) {
			fprintf( cfile, "    " );
		}
		fprintf( cfile, "</%s>", _value.c_str() );
	}
}


void XmlElement::CopyTo( XmlElement* target ) const
{
	// superclass:
	XmlNode::CopyTo( target );

	// Element class: 
	// Clone the attributes, then clone the children.
	const XmlAttribute* attribute = 0;
	for(	attribute = attributeSet.First();
	attribute;
	attribute = attribute->Next() )
	{
		target->SetAttribute( attribute->Name(), attribute->Value() );
	}

	XmlNode* node = 0;
	for ( node = _firstChild; node; node = node->NextSibling() )
	{
		target->LinkEndChild( node->Clone() );
	}
}

bool XmlElement::Accept( XmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this, attributeSet.First() ) ) 
	{
		for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


XmlNode* XmlElement::Clone() const
{
	try {
		XmlElement* clone = new XmlElement( Value() );

		CopyTo( clone );
		
		return clone;
	} catch (std::bad_alloc &) {
	}
	
	return NULL;
}

std::string XmlElement::GetText() const
{
	const XmlNode* child = this->FirstChild();
	
	if ( child ) {
		const XmlText* childText = child->ToText();
		
		if ( childText ) {
			return childText->Value();
		}
	}

	return "";
}


XmlDocument::XmlDocument() : XmlNode( XmlNode::DOCUMENT )
{
	_tabsize = 4;
	useMicrosoftBOM = false;
	ClearError();
}

XmlDocument::XmlDocument( const char * documentName ) : 
	XmlNode( XmlNode::DOCUMENT )
{
	_tabsize = 4;
	useMicrosoftBOM = false;
	_value = documentName;
	ClearError();
}


XmlDocument::XmlDocument( const std::string& documentName ) : 
	XmlNode( XmlNode::DOCUMENT )
{
	_tabsize = 4;
	useMicrosoftBOM = false;
	_value = documentName;
	ClearError();
}


XmlDocument::XmlDocument( const XmlDocument& copy ) : 
	XmlNode( XmlNode::DOCUMENT )
{
	copy.CopyTo( this );
}

void XmlDocument::operator=( const XmlDocument& copy )
{
	Clear();
	copy.CopyTo( this );
}


bool XmlDocument::LoadFile( XmlEncoding encoding )
{
	return LoadFile( Value(), encoding );
}


bool XmlDocument::SaveFile() const
{
	return SaveFile( Value() );
}

bool XmlDocument::LoadFile( const char* path, XmlEncoding encoding )
{
	// There was a really terrifying little bug here. The code:
	//		value = filename
	// in the STL case, cause the assignment method of the std::string to
	// be called. What is strange, is that the std::string had the same
	// address as it's c_str() method, and so bad things happen. Looks
	// like a bug in the Microsoft STL implementation.
	// Add an extra string to avoid the crash.
	std::string filename( path );
	_value = filename;

	// reading in binary mode so that tinyxml can normalize the EOL
	FILE* file = fopen( _value.c_str (), "rb" );	

	if ( file ) {
		bool result = LoadFile( file, encoding );
		fclose( file );
		return result;
	} else {
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	}
}

bool XmlDocument::LoadFile( FILE* file, XmlEncoding encoding )
{
	if ( !file ) 
	{
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	}

	// Delete the existing data:
	Clear();
	location.Clear();

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length == 0 )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	}

	// If we have a file, assume it is all one big XML file, and read it in.
	// The document parser may decide the document ends sooner than the entire file, however.
	std::string data;
	data.reserve( length );

	// Subtle bug here. nyXml did use fgets. But from the XML spec:
	// 2.11 End-of-Line Handling
	// <snip>
	// <quote>
	// ...the XML processor MUST behave as if it normalized all line breaks in external 
	// parsed entities (including the document entity) on input, before parsing, by translating 
	// both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
	// a single #xA character.
	// </quote>
	//
	// It is not clear fgets does that, and certainly isn't clear it works cross platform. 
	// Generally, you expect fgets to translate from the convention of the OS to the c/unix
	// convention, and not work generally.

	/*
	while( fgets( buf, sizeof(buf), file ) )
	{
		data += buf;
	}
	*/

	char *buf = NULL;
	
	try {
		buf = new char[ length+1 ];
	} catch (std::bad_alloc &) {
		return false;
	}

	buf[0] = 0;

	if ( fread( buf, length, 1, file ) != 1 ) {
		delete [] buf;

		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );

		return false;
	}

	const char* lastPos = buf;
	const char* p = buf;

	buf[length] = 0;
	while( *p ) {
		assert( p < (buf+length) );
		if ( *p == 0xa ) {
			// Newline character. No special rules for this. Append all the characters
			// since the last string, and include the newline.
			data.append( lastPos, (p-lastPos+1) );	// append, include the newline
			++p;									// move past the newline
			lastPos = p;							// and point to the new buffer (may be 0)
			assert( p <= (buf+length) );
		}
		else if ( *p == 0xd ) {
			// Carriage return. Append what we have so far, then
			// handle moving forward in the buffer.
			if ( (p-lastPos) > 0 ) {
				data.append( lastPos, p-lastPos );	// do not add the CR
			}
			data += (char)0xa;						// a proper newline

			if ( *(p+1) == 0xa ) {
				// Carriage return - new line sequence
				p += 2;
				lastPos = p;
				assert( p <= (buf+length) );
			}
			else {
				// it was followed by something else...that is presumably characters again.
				++p;
				lastPos = p;
				assert( p <= (buf+length) );
			}
		}
		else {
			++p;
		}
	}
	// Handle any left over characters.
	if ( p-lastPos ) {
		data.append( lastPos, p-lastPos );
	}		
	delete [] buf;
	buf = 0;

	Parse( data.c_str(), 0, encoding );

	if (  Error() )
        return false;
    else
		return true;
}


bool XmlDocument::SaveFile( const char * filename ) const
{
	// The old c stuff lives on...
	FILE* fp = fopen( filename, "w" );
	if ( fp )
	{
		bool result = SaveFile( fp );
		fclose( fp );
		return result;
	}
	return false;
}


bool XmlDocument::SaveFile( FILE* fp ) const
{
	if ( useMicrosoftBOM ) 
	{
		const uint8_t TIXML_UTF_LEAD_0 = 0xefU;
		const uint8_t TIXML_UTF_LEAD_1 = 0xbbU;
		const uint8_t TIXML_UTF_LEAD_2 = 0xbfU;

		fputc( TIXML_UTF_LEAD_0, fp );
		fputc( TIXML_UTF_LEAD_1, fp );
		fputc( TIXML_UTF_LEAD_2, fp );
	}
	Print( fp, 0 );
	return (ferror(fp) == 0);
}


void XmlDocument::CopyTo( XmlDocument* target ) const
{
	XmlNode::CopyTo( target );

	target->error = error;
	target->errorDesc = errorDesc.c_str ();

	XmlNode* node = 0;
	for ( node = _firstChild; node; node = node->NextSibling() ) {
		target->LinkEndChild( node->Clone() );
	}	
}


XmlNode * XmlDocument::Clone() const
{
	try {
		XmlDocument* clone = new XmlDocument();

		CopyTo( clone );
		
		return clone;
	} catch (std::bad_alloc &) {
	}

	return NULL;
}


void XmlDocument::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( cfile, depth );
		fprintf( cfile, "\n" );
	}
}


bool XmlDocument::Accept( XmlVisitor* visitor ) const
{
	if ( visitor->VisitEnter( *this ) )
	{
		for ( const XmlNode* node=FirstChild(); node; node=node->NextSibling() )
		{
			if ( !node->Accept( visitor ) )
				break;
		}
	}
	return visitor->VisitExit( *this );
}


const XmlAttribute* XmlAttribute::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( _next->_value.empty() && _next->_name.empty() ) {
		return 0;
	}

	return _next;
}

/*
XmlAttribute* XmlAttribute::Next()
{
	// We are using knowledge of the sentinel. The sentinel have a value or name.
	if ( _next->value.empty() && _next->name.empty() ) {
		return 0;
	}

	return _next;
}
*/

const XmlAttribute* XmlAttribute::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel have a value or name.
	if ( _prev->_value.empty() && _prev->_name.empty() ) {
		return 0;
	}

	return _prev;
}

/*
XmlAttribute* XmlAttribute::Previous()
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( _prev->value.empty() && _prev->name.empty() ) {
		return 0;
	}

	return _prev;
}
*/

void XmlAttribute::Print( FILE* cfile, int /*depth*/, std::string* str ) const
{
	std::string n, v;

	PutString( _name, &n );
	PutString( _value, &v );

	if (_value.find ('\"') == std::string::npos) {
		if ( cfile ) {
			fprintf (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
		}
		if ( str ) {
			(*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
		}
	} else {
		if ( cfile ) {
			fprintf (cfile, "%s='%s'", n.c_str(), v.c_str() );
		}
		if ( str ) {
			(*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
		}
	}
}


int XmlAttribute::QueryIntValue( int* ival ) const
{
	if ( sscanf( _value.c_str(), "%d", ival ) == 1 ) {
		return TIXML_SUCCESS;
	}
	return TIXML_WRONG_TYPE;
}

int XmlAttribute::QueryDoubleValue( double* dval ) const
{
	if ( sscanf( _value.c_str(), "%lf", dval ) == 1 ) {
		return TIXML_SUCCESS;
	}
	return TIXML_WRONG_TYPE;
}

void XmlAttribute::SetIntValue( int value )
{
	char buf [64];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF(buf, sizeof(buf), "%d", value);
	#else
		sprintf (buf, "%d", value);
	#endif
	SetValue (buf);
}

void XmlAttribute::SetDoubleValue( double value )
{
	char buf [256];
	#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF( buf, sizeof(buf), "%lf", value);
	#else
		sprintf (buf, "%lf", value);
	#endif
	SetValue (buf);
}

int XmlAttribute::IntValue() const
{
	return atoi (_value.c_str ());
}

double  XmlAttribute::DoubleValue() const
{
	return atof (_value.c_str ());
}


XmlComment::XmlComment( const XmlComment& copy ) : XmlNode( XmlNode::COMMENT )
{
	copy.CopyTo( this );
}


void XmlComment::operator=( const XmlComment& base )
{
	Clear();
	base.CopyTo( this );
}


void XmlComment::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	for ( int i=0; i<depth; i++ ) {
		fprintf( cfile,  "    " );
	}
	fprintf( cfile, "<!--%s-->", _value.c_str() );
}


void XmlComment::CopyTo( XmlComment* target ) const
{
	XmlNode::CopyTo( target );
}


bool XmlComment::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlComment::Clone() const
{
	try {
		XmlComment* clone = new XmlComment();

		CopyTo( clone );
	
		return clone;
	} catch (std::bad_alloc &) {
	}

	return NULL;
}


void XmlText::Print( FILE* cfile, int depth ) const
{
	assert( cfile );
	if ( _cdata ) {
		int i;
		fprintf( cfile, "\n" );
		for ( i=0; i<depth; i++ ) {
			fprintf( cfile, "    " );
		}
		fprintf( cfile, "<![CDATA[%s]]>\n", _value.c_str() );	// unformatted output
	} else {
		std::string buffer;
		PutString( _value, &buffer );
		fprintf( cfile, "%s", buffer.c_str() );
	}
}

bool XmlText::CDATA() const
{
   	return _cdata; 
}

void XmlText::SetCDATA( bool cdata )
{
   	_cdata = cdata; 
}

void XmlText::CopyTo( XmlText* target ) const
{
	XmlNode::CopyTo( target );
	target->_cdata = _cdata;
}


bool XmlText::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlText::Clone() const
{	
	try {
		XmlText *clone = new XmlText( "" );

		CopyTo( clone );
	
		return clone;
	} catch (std::bad_alloc &) {
	}

	return NULL;
}


XmlDeclaration::XmlDeclaration( const char * version, const char * encoding, const char * standalone )
	: XmlNode( XmlNode::DECLARATION )
{
	_version = version;
	_encoding = encoding;
	_standalone = standalone;
}


XmlDeclaration::XmlDeclaration(	const std::string& version, const std::string& encoding, const std::string& standalone )
	: XmlNode( XmlNode::DECLARATION )
{
	_version = version;
	_encoding = encoding;
	_standalone = standalone;
}


XmlDeclaration::XmlDeclaration( const XmlDeclaration& copy )
	: XmlNode( XmlNode::DECLARATION )
{
	copy.CopyTo( this );	
}


void XmlDeclaration::operator=( const XmlDeclaration& copy )
{
	Clear();
	copy.CopyTo( this );
}


void XmlDeclaration::Print( FILE* cfile, int /*depth*/, std::string* str ) const
{
	if ( cfile ) {
		fprintf( cfile, "<?xml " );
	}

	if ( str ) {
		(*str) += "<?xml ";
	}

	if ( !_version.empty() ) {
		if ( cfile ) {
			fprintf (cfile, "version=\"%s\" ", _version.c_str ());
		}

		if ( str ) { 
			(*str) += "version=\""; (*str) += _version; (*str) += "\" "; 
		}
	}
	
	if ( !_encoding.empty() ) {
		if ( cfile ) {
			fprintf (cfile, "encoding=\"%s\" ", _encoding.c_str ());
		}

		if ( str ) {
			(*str) += "encoding=\""; (*str) += _encoding; (*str) += "\" "; 
		}
	}
	
	if ( !_standalone.empty() ) {
		if ( cfile ) {
			fprintf (cfile, "standalone=\"%s\" ", _standalone.c_str ());
		}

		if ( str ) { 
			(*str) += "standalone=\""; (*str) += _standalone; (*str) += "\" "; 
		}
	}
	
	if ( cfile ) {
		fprintf( cfile, "?>" );
	}
	
	if ( str ) {
		(*str) += "?>";
	}
}


void XmlDeclaration::CopyTo( XmlDeclaration* target ) const
{
	XmlNode::CopyTo( target );

	target->_version = _version;
	target->_encoding = _encoding;
	target->_standalone = _standalone;
}


bool XmlDeclaration::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlDeclaration::Clone() const
{	
	try {
		XmlDeclaration* clone = new XmlDeclaration();

		CopyTo( clone );
	
		return clone;
	} catch (std::bad_alloc &) {
	}

	return NULL;
}


void XmlUnknown::Print( FILE* cfile, int depth ) const
{
	for ( int i=0; i<depth; i++ ) {
		fprintf( cfile, "    " );
	}

	fprintf( cfile, "<%s>", _value.c_str() );
}


void XmlUnknown::CopyTo( XmlUnknown* target ) const
{
	XmlNode::CopyTo( target );
}


bool XmlUnknown::Accept( XmlVisitor* visitor ) const
{
	return visitor->Visit( *this );
}


XmlNode* XmlUnknown::Clone() const
{
	try {
		XmlUnknown* clone = new XmlUnknown();

		CopyTo( clone );

		return clone;
	} catch (std::bad_alloc &) {
	}

	return NULL;
}


XmlAttributeSet::XmlAttributeSet()
{
	sentinel._next = &sentinel;
	sentinel._prev = &sentinel;
}


XmlAttributeSet::~XmlAttributeSet()
{
	assert( sentinel._next == &sentinel );
	assert( sentinel._prev == &sentinel );
}


void XmlAttributeSet::Add( XmlAttribute* addMe )
{
	assert( !Find( std::string( addMe->Name() ) ) );	// Shouldn't be multiply adding to the set.

	addMe->_next = &sentinel;
	addMe->_prev = sentinel._prev;

	sentinel._prev->_next = addMe;
	sentinel._prev = addMe;
}

void XmlAttributeSet::Remove( XmlAttribute* removeMe )
{
	XmlAttribute* node;

	for( node = sentinel._next; node != &sentinel; node = node->_next ) {
		if ( node == removeMe ) {
			node->_prev->_next = node->_next;
			node->_next->_prev = node->_prev;
			node->_next = 0;
			node->_prev = 0;
			return;
		}
	}

	assert( 0 );		// we tried to remove a non-linked attribute.
}


const XmlAttribute* XmlAttributeSet::Find( const std::string& name ) const
{
	for( const XmlAttribute* node = sentinel._next; node != &sentinel; node = node->_next ) {
		if ( node->_name == name ) {
			return node;
		}
	}

	return 0;
}

/*
XmlAttribute*	XmlAttributeSet::Find( const std::string& name )
{
	for( XmlAttribute* node = sentinel._next; node != &sentinel; node = node->_next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}
*/


const XmlAttribute* XmlAttributeSet::Find( const char* name ) const
{
	for( const XmlAttribute* node = sentinel._next; node != &sentinel; node = node->_next )
	{
		if ( strcmp( node->_name.c_str(), name ) == 0 )
			return node;
	}
	return 0;
}

/*
XmlAttribute*	XmlAttributeSet::Find( const char* name )
{
	for( XmlAttribute* node = sentinel._next; node != &sentinel; node = node->_next )
	{
		if ( strcmp( node->name.c_str(), name ) == 0 )
			return node;
	}
	return 0;
}
*/

std::istream& operator>> (std::istream & in, XmlNode & base)
{
	std::string tag;
	tag.reserve( 8 * 1000 );
	base.StreamIn( &in, &tag );

	base.Parse( tag.c_str(), 0, TIXML_DEFAULT_ENCODING );
	return in;
}


std::ostream& operator<< (std::ostream & out, const XmlNode & base)
{
	XmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out << printer.Str();

	return out;
}


std::string& operator<< (std::string& out, const XmlNode& base )
{
	XmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept( &printer );
	out.append( printer.Str() );

	return out;
}


XmlHandle XmlHandle::FirstChild() const
{
	if ( node )
	{
		XmlNode* child = node->FirstChild();
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChild( const char * value ) const
{
	if ( node )
	{
		XmlNode* child = node->FirstChild( value );
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChildElement() const
{
	if ( node )
	{
		XmlElement* child = node->FirstChildElement();
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::FirstChildElement( const char * value ) const
{
	if ( node )
	{
		XmlElement* child = node->FirstChildElement( value );
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::Child( int count ) const
{
	if ( node )
	{
		int i;
		XmlNode* child = node->FirstChild();
		for (	i=0;
				child && i<count;
				child = child->NextSibling(), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::Child( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		XmlNode* child = node->FirstChild( value );
		for (	i=0;
				child && i<count;
				child = child->NextSibling( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::ChildElement( int count ) const
{
	if ( node )
	{
		int i;
		XmlElement* child = node->FirstChildElement();
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement(), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


XmlHandle XmlHandle::ChildElement( const char* value, int count ) const
{
	if ( node )
	{
		int i;
		XmlElement* child = node->FirstChildElement( value );
		for (	i=0;
				child && i<count;
				child = child->NextSiblingElement( value ), ++i )
		{
			// nothing
		}
		if ( child )
			return XmlHandle( child );
	}
	return XmlHandle( 0 );
}


bool XmlPrinter::VisitEnter( const XmlDocument& )
{
	return true;
}

bool XmlPrinter::VisitExit( const XmlDocument& )
{
	return true;
}

bool XmlPrinter::VisitEnter( const XmlElement& element, const XmlAttribute* firstAttribute )
{
	DoIndent();
	
	_buffer += "<";
	_buffer += element.Value();

	for( const XmlAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next() ) {
		_buffer += " ";
		attrib->Print( 0, 0, &_buffer );
	}

	if ( !element.FirstChild() ) {
		_buffer += " />";
		DoLineBreak();
	} else {
		_buffer += ">";
		if (    element.FirstChild()->ToText()
			  && element.LastChild() == element.FirstChild()
			  && element.FirstChild()->ToText()->CDATA() == false )
		{
			_simpleTextPrint = true;
			// no DoLineBreak()!
		} else {
			DoLineBreak();
		}
	}

	++_depth;	
	
	return true;
}


bool XmlPrinter::VisitExit( const XmlElement& element )
{
	--_depth;

	if ( !element.FirstChild() ) {
		// nothing.
	} else {
		if ( _simpleTextPrint ) {
			_simpleTextPrint = false;
		} else {
			DoIndent();
		}
		
		_buffer += "</";
		_buffer += element.Value();
		_buffer += ">";

		DoLineBreak();
	}

	return true;
}


bool XmlPrinter::Visit( const XmlText& text )
{
	if ( text.CDATA() ) {
		DoIndent();

		_buffer += "<![CDATA[";
		_buffer += text.Value();
		_buffer += "]]>";
		
		DoLineBreak();
	} else if ( _simpleTextPrint ) {
		_buffer += text.Value();
	} else {
		DoIndent();
		
		_buffer += text.Value();
		
		DoLineBreak();
	}

	return true;
}


bool XmlPrinter::Visit( const XmlDeclaration& declaration )
{
	DoIndent();
	declaration.Print( 0, 0, &_buffer );
	DoLineBreak();
	return true;
}


bool XmlPrinter::Visit( const XmlComment& comment )
{
	DoIndent();
	_buffer += "<!--";
	_buffer += comment.Value();
	_buffer += "-->";
	DoLineBreak();
	return true;
}


bool XmlPrinter::Visit( const XmlUnknown& unknown )
{
	DoIndent();
	_buffer += "<";
	_buffer += unknown.Value();
	_buffer += ">";
	DoLineBreak();
	return true;
}


}


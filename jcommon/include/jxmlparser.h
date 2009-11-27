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
/**
 * \brief Original code (2.0 and earlier )copyright (c) 2000-2006 Lee Thomason 
 * (www.grinninglizard.com)
 *
 * www.sourceforge.net/projects/tinyxml
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 * not claim that you wrote the original software. If you use this
 * software in a product, an acknowledgment in the product documentation
 * would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */
 
#ifndef J_XMLPARSER_H
#define J_XMLPARSER_H

#include "jobject.h"

#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

/**
 * \brief The support for explicit isn't that universal, and it isn't really
 * required - it is used to check that the XmlString class isn't incorrectly
 * used. Be nice to old compilers and macro it here:
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
#define TIXML_EXPLICIT explicit		// Microsoft visual studio, version 6 and higher.
#elif defined(__GNUC__) && (__GNUC__ >= 3 )
#define TIXML_EXPLICIT explicit		// GCC version 3 and higher.s
#else
#define TIXML_EXPLICIT
#endif

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#if defined( _DEBUG ) && !defined( DEBUG )		// Help out windows:
#define DEBUG
#endif

/**
 * \brief Deprecated library function hell. Compilers want to use the
 * new safe versions. This probably doesn't fully address the problem,
 * but it gets closer. There are too many compilers for me to fully
 * test. If you get compilation troubles, undefine TIXML_SAFE
 */
#define TIXML_SAFE

#ifdef TIXML_SAFE

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )	// Microsoft visual studio, version 2005 and higher.
#define TIXML_SNPRINTF _snprintf_s
#define TIXML_SNSCANF  _snscanf_s

#elif defined(_MSC_VER) && (_MSC_VER >= 1200 )	// Microsoft visual studio, version 6 and higher.
#define TIXML_SNPRINTF _snprintf
#define TIXML_SNSCANF  _snscanf

#elif defined(__GNUC__) && (__GNUC__ >= 3 ) && !defined(__CYGWIN32__)	// GCC version 3 and higher.s
#define TIXML_SNPRINTF snprintf
#define TIXML_SNSCANF  snscanf

#endif
#endif	

namespace jcommon {
	
class XmlDocument;
class XmlElement;
class XmlComment;
class XmlUnknown;
class XmlAttribute;
class XmlText;
class XmlDeclaration;
class XmlParsingData;

const int TIXML_MAJOR_VERSION = 2;
const int TIXML_MINOR_VERSION = 5;
const int TIXML_PATCH_VERSION = 1;

/**
 * \brief Internal structure for tracking location of items in the XML file.
 */
struct XmlCursor{
	/** \brief */
	int row;	// 0 based.
	/** \brief */
	int col;	// 0 based.
	
	/**
	 * \brief
	 *
	 */
	XmlCursor();
	
	/**
	 * \brief
	 *
	 */
	void Clear();
};

/**
 * \brief If you call the Accept() method, it requires being passed a XmlVisitor
 * class to handle callbacks. For nodes that contain other nodes (Document, Element)
 * you will get called with a VisitEnter/VisitExit pair. Nodes that are always leaves
 * are simple called with Visit().
 *
 * If you return 'true' from a Visit method, recursive parsing will continue. If you return
 * false, <b>no children of this node or its sibilings</b> will be Visited.
 * 
 * All flavors of Visit methods have a default implementation that returns 'true' (continue 
 * visiting). You need to only override methods that are interesting to you.
 * 
 * Generally Accept() is called on the XmlDocument, although all nodes suppert Visiting.
 * 
 * You should never change the document from a callback.
 * 
 * @sa XmlNode::Accept()
 */
class XmlVisitor{
	private:
		
	public:
		/**
		 * \brief Destructor.
		 *
		 */
		virtual ~XmlVisitor();
		
		/**
		 * \brief Visit a document.
		 *
		 */
		virtual bool VisitEnter(const XmlDocument& doc);
		
		/**
		 * \brief Visit a document.
		 *
		 */
		virtual bool VisitExit(const XmlDocument& doc);	
			
		/**
		 * \brief Visit an element.
		 *
		 */
		virtual bool VisitEnter(const XmlElement& element, const XmlAttribute* firstAttribute);
		
		/**
		 * \brief Visit an element.
		 *
		 */
		virtual bool VisitExit(const XmlElement& element);
		
		/**
		 * \brief Visit a declaration.
		 *
		 */
		virtual bool Visit(const XmlDeclaration& declaration);
		
		/**
		 * \brief Visit a text node
		 *
		 */
		virtual bool Visit(const XmlText& text);
		
		/** 
		 * \brief Visit a comment node
		 *
		 */
		virtual bool Visit(const XmlComment& comment);
		
		/**
		 * \brief Visit an unknow node.
		 *
		 */
		virtual bool Visit(const XmlUnknown& unknown);
		
};

/**
 * \brief Only used by Attribute::Query functions.
 *
 */
enum { 
	TIXML_SUCCESS,
	TIXML_NO_ATTRIBUTE,
	TIXML_WRONG_TYPE
};

/**
 * \brief Used by the parsing routines.
 *
 */
enum XmlEncoding {
	TIXML_ENCODING_UNKNOWN,
	TIXML_ENCODING_UTF8,
	TIXML_ENCODING_LEGACY
};

const XmlEncoding TIXML_DEFAULT_ENCODING = TIXML_ENCODING_UNKNOWN;

/** 
 * \brief XmlBase is a base class for every class in nyXml.
 * It does little except to establish that nyXml classes
 * can be printed and provide some utility functions.
 * 		
 * In XML, the document and elements can contain
 * other elements and other types of nodes.
 * 			
 * @verbatim
 * A Document can contain: Element (container or leaf)
 * Comment (leaf)
 * Unknown (leaf)
 * Declaration( leaf )
 * 
 * An Element can contain:	Element (container or leaf)
 * Text	(leaf)
 * Attributes (not on tree)
 * Comment (leaf)
 * Unknown (leaf)
 * 
 * A Decleration contains: Attributes (not on tree)
 * 	@endverbatim
 */
class XmlBase{
	friend class XmlNode;
	friend class XmlElement;
	friend class XmlDocument;
	
	public:
		enum {
			TIXML_NO_ERROR = 0,
			TIXML_ERROR,
			TIXML_ERROR_OPENING_FILE,
			TIXML_ERROR_OUT_OF_MEMORY,
			TIXML_ERROR_PARSING_ELEMENT,
			TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME,
			TIXML_ERROR_READING_ELEMENT_VALUE,
			TIXML_ERROR_READING_ATTRIBUTES,
			TIXML_ERROR_PARSING_EMPTY,
			TIXML_ERROR_READING_END_TAG,
			TIXML_ERROR_PARSING_UNKNOWN,
			TIXML_ERROR_PARSING_COMMENT,
			TIXML_ERROR_PARSING_DECLARATION,
			TIXML_ERROR_DOCUMENT_EMPTY,
			TIXML_ERROR_EMBEDDED_NULL,
			TIXML_ERROR_PARSING_CDATA,
			TIXML_ERROR_DOCUMENT_TOP_ONLY,
			TIXML_ERROR_STRING_COUNT
		};
		
	private:
		/**
		 * \brief
		 *
		 */
		XmlBase(const XmlBase &);
		
		/**
		 * \brief Not allowed.
		 *
		 */
		void operator=(const XmlBase &base);
		
		struct Entity{
			const char *str;
			unsigned int strLength;
			char chr;
		};
		
		enum {
			NUM_ENTITY = 5,
			MAX_ENTITY_LENGTH = 6
		};
		
		static Entity entity[NUM_ENTITY];
		static bool condenseWhiteSpace;
		
	protected:
		/**
		 * \brief
		 *
		 */
		static const char * SkipWhiteSpace(const char *, XmlEncoding encoding);
		
		/**
		 * \brief
		 *
		 */
		static bool IsWhiteSpace( char c );
		
		/**
		 * \brief
		 *
		 */
		static bool IsWhiteSpace( int c );
		
		/**
		 * \brief
		 *
		 */
		static bool	StreamWhiteSpace(std::istream *in, std::string *tag);
		
		/**
		 * \brief
		 *
		 */
		static bool StreamTo(std::istream *in, int character, std::string *tag);
		
		/**
		 * \brief Reads an XML name into the string provided. Returns a pointer just 
		 * past the last character of the name, or 0 if the function has an error.
		 * 
		 */
		static const char * ReadName(const char *p, std::string *name, XmlEncoding encoding);
		
		/**
		 * \brief Reads text. Returns a pointer past the given end tag. Wickedly complex options,
		 * but it keeps the (sensitive) code in one place.
		 * 
		 * \param in Where to start.
		 * \param text The string read.
		 * \param ignoreWhiteSpace Whether to keep the white space.
		 * \param endTag What ends this text.
		 * \param ignoreCase Whether to ignore case in the end tag.
		 * \param enconding The current encoding.
		 */
		static const char * ReadText(const char* in, std::string* text, bool ignoreWhiteSpace, const char* endTag, bool ignoreCase, XmlEncoding encoding);
		
		/**
		 * \brief If an entity has been found, transform it into a character.
		 *
		 */
		static const char * GetEntity(const char* in, char* value, int* length, XmlEncoding encoding);
		
		/**
		 * \brief Get a character, while interpreting entities. The length can be from 0 to 4 bytes.
		 *
		 */
		static const char * GetChar(const char* p, char* _value, int* length, XmlEncoding encoding);
		
		/**
		 * \brief Puts a string to a stream, expanding entities as it goes. Note this 
		 * should not contian the '<', '>', etc, or they will be transformed into entities.
		 *
		 */
		static void PutString(const std::string& str, std::string* out);
		
		/**
		 * \brief Return true if the next characters in the stream are any of the endTag sequences.
		 * Ignore case only works for english, and should only be relied on when comparing 
		 * to English words: StringEqual( p, "version", true ) is fine.
		 */
		static bool StringEqual(const char *p, const char *endTag, bool ignoreCase, XmlEncoding encoding);
		
		/**
		 * \brief
		 *
		 */
		static const char * errorString[TIXML_ERROR_STRING_COUNT];
		
		XmlCursor location;
		
		/** \brief Field containing a generic user pointer */
		void *userData;
		
		/**
		 * \brief None of these methods are reliable for any language except English.
		 * Good for approximation, not great for accuracy.
		 *
		 */
		static int IsAlpha(uint8_t anyByte, XmlEncoding encoding);
		
		/**
		 * \brief
		 *
		 */
		static int IsAlphaNum(uint8_t anyByte, XmlEncoding encoding);
		
		/**
		 * \brief
		 *
		 */
		static int ToLower(int v, XmlEncoding encoding);
		
		/**
		 * \brief
		 *
		 */
		static void ConvertUTF32ToUTF8(unsigned long input, char *output, int *length);
		
	public:
		/**
		 * \brief
		 *
		 */
		XmlBase();
		
		/**
		 * \brief
		 *
		 */
		virtual ~XmlBase();
		
		/**	
		 * \brief All nyXml classes can print themselves to a filestream or the string 
		 * class (XmlString in non-STL mode, std::string in STL mode.) Either or both
		 * cfile and str can be null.
		 * 
		 * This is a formatted print, and will insert tabs and newlines.
		 * 
		 * (For an unformatted stream, use the << operator.)
		 */
		virtual void Print(FILE* cfile, int depth) const = 0;
		
		/**	
		 * The world does not agree on whether white space should be kept or not. In order
		 * to make everyone happy, these global, static functions are provided to set 
		 * whether or not nyXml will condense all white space into a single space or not.
		 * The default is to condense. Note changing this value is not thread safe.
		 */
		static void SetCondenseWhiteSpace(bool condense);
		
		/**
		 * \brief Return the current white space setting.
		 *
		 */
		static bool IsWhiteSpaceCondensed();
		
		/**
		 * \brief Return the position, in the original source file, of this node or attribute.
		 * The row and column are 1-based. (That is the first row and first column is
		 * 1,1). If the returns values are 0 or less, then the parser does not have	a row and
		 * column value.
		 *  
		 * Generally, the row and column value will be set when the XmlDocument::Load(),
		 * XmlDocument::LoadFile(), or any XmlNode::Parse() is called. It will NOT be set
		 * when the DOM was created from operator>>.
		 * 
		 * The values reflect the initial load. Once the DOM is modified programmatically
		 * (by adding or changing nodes and attributes) the new values will NOT update to
		 * reflect changes in the document.
		 * 
		 * There is a minor performance cost to computing the row and column. Computation
		 * can be disabled if XmlDocument::SetTabSize() is called with 0 as the value.
		 * 
		 * @sa XmlDocument::SetTabSize()
		 */
		int Row() const;
		
		/**
		 * \brief
		 *
		 */
		int Column() const;		// SEE Row
		
		/**
		 * \brief
		 *
		 */
		void SetUserData(void *user);
		
		/**
		 * \brief
		 *
		 */
		void * GetUserData();
		
		/**
		 * \brief
		 *
		 */
		const void * GetUserData() const;
		
		/**
		 * \brief Table that returs, for a given lead byte, the total number of bytes
		 * in the UTF-8 sequence.
		 *
		 */
		static const int utf8ByteTable[256];
		
		/**
		 * \brief
		 *
		 * \param encoding TIXML_ENCODING_UNKNOWN
		 */
		virtual const char * Parse(const char *p, XmlParsingData *data, XmlEncoding encoding) = 0;
		
};

/**
 * \brief The parent class for everything in the Document Object Model.
 * (Except for attributes). Nodes have siblings, a parent, and children. 
 * A node can be in a document, or stand on its own. The type of a 
 * XmlNode can be queried, and it can be cast to its more defined type.
 */
class XmlNode : public XmlBase{
	friend class XmlDocument;
	friend class XmlElement;
	
	public:
		/**
		 * \brief An input stream operator, for every class. Tolerant of newlines and
		 * formatting, but doesn't expect them.
		 * 
		 */
		friend std::istream& operator >> (std::istream& in, XmlNode& base);
	
		/**
		 * \brief An output stream operator, for every class. Note that this outputs
		 * without any newlines or formatting, as opposed to Print(), which includes
		 * tabs and new lines.
		 * 
		 * The operator<< and operator>> are not completely symmetric. Writing a node
		 * to a stream is very well defined. You'll get a nice stream of output, without
		 * any extra whitespace or newlines.
		 * 
		 * But reading is not as well defined. (As it always is.) If you create a XmlElement
		 * (for example) and read that from an input stream, the text needs to define an
		 * element or junk will result. This is true of all input streams, but it's worth
		 * keeping in mind.
		 * 
		 * A XmlDocument will read nodes until it reads a root element, and all the children
		 * of that root element.
		 */	
		friend std::ostream& operator<< (std::ostream& out, const XmlNode& base);
		
		/**
		 * \brief Appends the XML node or attribute to a std::string.
		 *
		 */
		friend std::string& operator<< (std::string& out, const XmlNode& base );
		
		
		/** 
		 * \brief The types of XML nodes supported by nyXml. (All the unsupported types 
		 * are picked up by UNKNOWN)
		 * 
		 */
		enum NodeType {
			DOCUMENT,
			ELEMENT,
			COMMENT,
			UNKNOWN,
			TEXT,
			DECLARATION,
			TYPECOUNT
		};
		
		/**
		 * \brief
		 *
		 */
		virtual ~XmlNode();
		
		/**
		 * \brief The meaning of 'value' changes for the specific type of XmlNode.
		 * @verbatim
		 * Document:	filename of the xml file
		 * Element:	name of the element
		 * Comment:	the comment text
		 * Unknown:	the tag contents
		 * Text:		the text string
		 * @endverbatim
		 * 
		 * The subclasses will wrap this function.
		 */
		const char * Value() const;
		
		/**
		 * \brief Return Value() as a std::string. If you only use STL, this is more 
		 * efficient than calling Value(). Only available in STL mode.
		 * 
		 */
		const std::string & ValueStr() const;
		
		/**
		 * \brief Changes the value of the node. Defined as:
		 * @verbatim
		 *	Document:	filename of the xml file
		 *	Element:	name of the element
		 *	Comment:	the comment text
		 *	Unknown:	the tag contents
		 *	Text:		the text string
		 *	@endverbatim
		 */
		void SetValue(const char * _value);
		
		/**
		 * \brief STL std::string form.
		 *
		 */
		void SetValue( const std::string& _value )	;
		
		/**
		 * \brief Delete all the children of this node. Does not affect 'this'.
		 *
		 */
		void Clear();
		
		/**
		 * \brief One step up the DOM.
		 *
		 */
		XmlNode* Parent();
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* Parent() const;
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* FirstChild() const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* FirstChild();
		
		/**
		 * \brief The first child of this node with the matching 'value'. Will be null 
		 * if none found.
		 *
		 */
		const XmlNode* FirstChild( const char * value ) const;
		
		/**
		 * \brief The first child of this node with the matching 'value'. Will be null if none found.
		 *
		 */
		XmlNode* FirstChild( const char * _value );
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* LastChild() const	;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* LastChild()	;
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* LastChild( const char * value ) const;			/// The last child of this node matching 'value'. Will be null if there are no children.
		
		/**
		 * \brief
		 *
		 */
		XmlNode* LastChild( const char * _value ) ;
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* FirstChild( const std::string& _value ) const	;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* FirstChild( const std::string& _value );
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* LastChild( const std::string& _value ) const	;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* LastChild( const std::string& _value );
		
		/** An alternate way to walk the children of a node.
		  One way to iterate over nodes is:
		  @verbatim
		  for( child = parent->FirstChild(); child; child = child->NextSibling() )
		  @endverbatim
		  
		  IterateChildren does the same thing with the syntax:
		  @verbatim
		  child = 0;
		  while( child = parent->IterateChildren( child ) )
		  @endverbatim
		  
		  IterateChildren takes the previous child as input and finds
		  the next one. If the previous child is null, it returns the
		  first. IterateChildren will return null when done.
		  */
		const XmlNode* IterateChildren( const XmlNode* previous ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* IterateChildren( XmlNode* previous ) ;
		
		/**
		 * \brief This flavor of IterateChildren searches for children with a particular 'value'
		 *
		 */
		const XmlNode* IterateChildren( const char * value, const XmlNode* previous ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* IterateChildren( const char * _value, XmlNode* previous ) ;
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* IterateChildren( const std::string& _value, const XmlNode* previous ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* IterateChildren( const std::string& _value, XmlNode* previous );
		
		/** Add a new node related to this. Adds a child past the LastChild.
		  Returns a pointer to the new object or NULL if an error occured.
		  */
		XmlNode* InsertEndChild( const XmlNode& addThis );
		
		/** Add a new node related to this. Adds a child past the LastChild.
		  
			NOTE: the node to be added is passed by pointer, and will be
			henceforth owned (and deleted) by tinyXml. This method is efficient
			and avoids an extra copy, but should be used with care as it
			uses a different memory model than the other insert functions.
		
			@sa InsertEndChild
		*/
		XmlNode* LinkEndChild( XmlNode* addThis );
		
		/** Add a new node related to this. Adds a child before the specified child.
		  Returns a pointer to the new object or NULL if an error occured.
		  */
		XmlNode* InsertBeforeChild( XmlNode* beforeThis, const XmlNode& addThis );
		
		/** Add a new node related to this. Adds a child after the specified child.
		  Returns a pointer to the new object or NULL if an error occured.
		  */
		XmlNode* InsertAfterChild(  XmlNode* afterThis, const XmlNode& addThis );
		
		/** Replace a child of this node.
		  Returns a pointer to the new object or NULL if an error occured.
		  */
		XmlNode* ReplaceChild( XmlNode* replaceThis, const XmlNode& withThis );
		
		/**
		 * \brief Delete a child of this node.
		 *
		 */
		bool RemoveChild( XmlNode* removeThis );
		
		/**
		 * \brief Navigate to a sibling node.
		 *
		 */
		const XmlNode* PreviousSibling() const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* PreviousSibling();
		
		/**
		 * \brief Navigate to a sibling node.
		 *
		 */
		const XmlNode* PreviousSibling( const char * ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* PreviousSibling( const char *_prev ) ;
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* PreviousSibling( const std::string& _value ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* PreviousSibling( const std::string& _value );
		
		/**
		 * \brief
		 *
		 */
		const XmlNode* NextSibling( const std::string& _value) const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* NextSibling( const std::string& _value) ;
		
		/**
		 * \brief Navigate to a sibling node.
		 *
		 */
		const XmlNode* NextSibling() const	;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* NextSibling()	;
		
		/**
		 * \brief Navigate to a sibling node with the given 'value'.
		 *
		 */
		const XmlNode* NextSibling( const char * ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlNode* NextSibling( const char* _next ) ;
		
		/** Convenience function to get through elements.
		  Calls NextSibling and ToElement. Will skip all non-Element
		  nodes. Returns 0 if there is not another element.
		  */
		const XmlElement* NextSiblingElement() const;
		
		/**
		 * \brief
		 *
		 */
		XmlElement* NextSiblingElement() ;
		
		/** 
		 * Convenience function to get through elements.
		 * Calls NextSibling and ToElement. Will skip all non-Element
		 * nodes. Returns 0 if there is not another element.
		 */
		const XmlElement* NextSiblingElement( const char * ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlElement* NextSiblingElement( const char *_next );
		
		/**
		 * \brief
		 *
		 */
		const XmlElement* NextSiblingElement( const std::string& _value) const;
		
		/**
		 * \brief
		 *
		 */
		XmlElement* NextSiblingElement( const std::string& _value);
		
		/**
		 * \brief Convenience function to get through elements.
		 *
		 */
		const XmlElement* FirstChildElement()	const;
		
		/**
		 * \brief
		 *
		 */
		XmlElement* FirstChildElement() ;
		
		/**
		 * \brief Convenience function to get through elements.
		 *
		 */
		const XmlElement* FirstChildElement( const char * _value ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlElement* FirstChildElement( const char * _value );
		
		/**
		 * \brief
		 *
		 */
		const XmlElement* FirstChildElement( const std::string& _value ) const	;
		
		/**
		 * \brief
		 *
		 */
		XmlElement* FirstChildElement( const std::string& _value );
		
		/** 
		 * \brief Query the type (as an enumerated value, above) of this node.
		 * The possible types are: DOCUMENT, ELEMENT, COMMENT, UNKNOWN, TEXT, and DECLARATION.
		 * 
	  	 */
		int Type() const;
		
		/** 
		 * Return a pointer to the Document this node lives in. Returns null if not in a document.
		 * 
		 */
		const XmlDocument* GetDocument() const;
		
		/**
		 * \brief
		 *
		 */
		XmlDocument* GetDocument();
		
		/**
		 * \brief Returns true if this node has no children.
		 *
		 */
		bool HasChildren() const;
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlDocument*    ToDocument()    const;
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlElement*     ToElement()     const ;
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlComment*     ToComment()     const ;
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlUnknown*     ToUnknown()     const;
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlText*        ToText()        const ;
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlDeclaration* ToDeclaration() const;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlDocument*          ToDocument() ;   
		
		/**
		 * \brief
		 *
		 */
		virtual XmlElement*           ToElement()	;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlComment*           ToComment() ;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlUnknown*           ToUnknown()	   ;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlText*	            ToText()       ;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlDeclaration*       ToDeclaration();
		
		/** Create an exact duplicate of this node and return it. The memory must be deleted
		  by the caller. 
		  */
		virtual XmlNode* Clone() const = 0;
		
		/** Accept a hierchical visit the nodes in the nyXML DOM. Every node in the 
		  XML tree will be conditionally visited and the host will be called back
		  via the XmlVisitor interface.
		  
		  This is essentially a SAX interface for nyXML. (Note however it doesn't re-parse
		  the XML for the callbacks, so the performance of nyXML is unchanged by using this
		  interface versus any other.)
		  
		  The interface has been based on ideas from:
		  
		  - http://www.saxproject.org/
		  - http://c2.com/cgi/wiki?HierarchicalVisitorPattern 
		  
		  Which are both good references for "visiting".
		  
		  An example of using Accept():
		  @verbatim
		  XmlPrinter printer;
		  tinyxmlDoc.Accept( &printer );
		  const char* xmlcstr = printer.CStr();
		  @endverbatim
		  */
		virtual bool Accept( XmlVisitor* visitor ) const = 0;
		
	protected:
		/**
		 * \brief
		 *
		 */
		XmlNode( NodeType _type );
		
		/**
		 * \brief Copy to the allocated object. Shared functionality between Clone, 
		 * Copy constructor, and the assignment operator.
		 *
		 */
		void CopyTo( XmlNode* target ) const;
		
		/**
		 * \brief The real work of the input operator.
		 *
		 */
		virtual void StreamIn( std::istream* in, std::string* tag ) = 0;
		
		/**
		 * \brief Figure out what is at *p, and parse it. Returns null if it is not an xml node.
		 *
		 */
		XmlNode* Identify( const char* start, XmlEncoding encoding );
		
		XmlNode*		parent;
		NodeType		type;
		
		XmlNode*		firstChild;
		XmlNode*		lastChild;
		
		std::string	value;
		
		XmlNode*		prev;
		XmlNode*		next;
		
	private:
		/**
		 * \brief
		 *
		 */
		XmlNode( const XmlNode& );				// not implemented.
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlNode& base );	// not allowed.
};


/** An attribute is a name-value pair. Elements have an arbitrary
  number of attributes, each with a unique name.
  
  @note The attributes are not XmlNodes, since they are not
  part of the tinyXML document object model. There are other
  suggested ways to look at this problem.
  */
class XmlAttribute : public XmlBase
{
	friend class XmlAttributeSet;
	
	public:
		/**
		 * \brief Construct an empty attribute.
		 *
		 */
		XmlAttribute();
	
		/**
		 * \brief std::string constructor.
		 *
		 */
		XmlAttribute( const std::string& _name, const std::string& _value );
		
		/**
		 * \brief Construct an attribute with a name and value.
		 *
		 */
		XmlAttribute( const char * _name, const char * _value );
		
		/**
		 * \brief
		 *
		 */
		const char*		Name()  const;
		
		/**
		 * \brief
		 *
		 */
		const char*		Value() const;
		
		/**
		 * \brief
		 *
		 */
		const std::string& ValueStr() const	;
		
		/**
		 * \brief
		 *
		 */
		int				IntValue() const;				///< Return the value of this attribute, converted to an integer.
		/**
		 * \brief
		 *
		 */
		double			DoubleValue() const;			///< Return the value of this attribute, converted to a double.
		
		// Get the tinyxml string representation
		const std::string& NameTStr() const ;
		
		/** QueryIntValue examines the value string. It is an alternative to the
		  IntValue() method with richer error checking.
		  If the value is an integer, it is stored in 'value' and 
		  the call returns TIXML_SUCCESS. If it is not
		  an integer, it returns TIXML_WRONG_TYPE.
		  
		  A specialized but useful call. Note that for success it returns 0,
		  which is the opposite of almost all other nyXml calls.
		  */
		int QueryIntValue( int* _value ) const;
		
		/**
		 * \brief QueryDoubleValue examines the value string. See QueryIntValue().
		 *
		 */
		int QueryDoubleValue( double* _value ) const;
		
		/**
		 * \brief
		 *
		 */
		void SetName( const char* _name )	;
		
		/**
		 * \brief
		 *
		 */
		void SetValue( const char* _value )	;
		
		/**
		 * \brief
		 *
		 */
		void SetIntValue( int _value );										///< Set the value from an integer.
		
		/**
		 * \brief
		 *
		 */
		void SetDoubleValue( double _value );								///< Set the value from a double.
		
		/**
		 * \brief STL std::string form.
		 *
		 */
		void SetName( const std::string& _name );
		
		/**
		 * \brief STL std::string form.	
		 *
		 */
		void SetValue( const std::string& _value );
		
		/**
		 * \brief Get the next sibling attribute in the DOM. Returns null at end.
		 *
		 */
		const XmlAttribute* Next() const;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute* Next() ;
		
		/**
		 * \brief Get the previous sibling attribute in the DOM. Returns null at beginning.
		 *
		 */
		const XmlAttribute* Previous() const;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute* Previous() ;
		
		/**
		 * \brief
		 *
		 */
		bool operator==( const XmlAttribute& rhs ) const ;
		
		/**
		 * \brief
		 *
		 */
		bool operator<( const XmlAttribute& rhs )	 const;
		
		/**
		 * \brief
		 *
		 */
		bool operator>( const XmlAttribute& rhs )  const ;
		
		/*	Attribute parsing starts: first letter of the name returns: 
		 *	the next char after the value end quote
		 *
		 */
		virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );
		
		/**
		 * \brief Prints this Attribute to a FILE stream.
		 *
		 */
		virtual void Print( FILE* cfile, int depth ) const ;
		
		/**
		 * \brief
		 *
		 */
		void Print( FILE* cfile, int depth, std::string* str ) const;
		
		/**
		 * \brief Set the document pointer so the attribute can report errors.
		 *
		 */
		void SetDocument( XmlDocument* doc );
		
	private:
		/**
		 * \brief
		 *
		 */
		XmlAttribute( const XmlAttribute& );				// not implemented.
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlAttribute& base );	// not allowed.
		
		XmlDocument*	document;	// A pointer back to a document, for error reporting.
		std::string name;
		std::string value;
		XmlAttribute*	prev;
		XmlAttribute*	next;
};

/*	A class used to manage a group of attributes.
	It is only used internally, both by the ELEMENT and the DECLARATION.
	
	The set can be changed transparent to the Element and Declaration
	classes that use it, but NOT transparent to the Attribute
	which has to implement a next() and previous() method. Which makes
	it a bit problematic and prevents the use of STL.
	
	This version is implemented with circular lists because:
	- I like circular lists
	- it demonstrates some independence from the (typical) doubly linked list.
	*/
class XmlAttributeSet {
	public:
		/**
		 * \brief
		 *
		 */
		XmlAttributeSet();
		
		/**
		 * \brief
		 *
		 */
		~XmlAttributeSet();
		
		/**
		 * \brief
		 *
		 */
		void Add( XmlAttribute* attribute );
		
		/**
		 * \brief
		 *
		 */
		void Remove( XmlAttribute* attribute );
		
		/**
		 * \brief
		 *
		 */
		const XmlAttribute* First()	const;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute* First()	;	
		
		/**
		 * \brief
		 *
		 */
		const XmlAttribute* Last() const;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute* Last();
		
		/**
		 * \brief
		 *
		 */
		const XmlAttribute*	Find( const char* _name ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute*	Find( const char* _name ) ;
		
		/**
		 * \brief
		 *
		 */
		const XmlAttribute*	Find( const std::string& _name ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute*	Find( const std::string& _name ) ;
		
	private:
		// ME: Because of hidden/disabled copy-construktor in XmlAttribute (sentinel-element),
		// ME: this class must be also use a hidden/disabled copy-constructor !!!
		XmlAttributeSet( const XmlAttributeSet& );	// not allowed
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlAttributeSet& );	// not allowed (as XmlAttribute)
		
		XmlAttribute sentinel;
};


/** The element is a container class. It has a value, the element name,
  and can contain other elements, text, comments, and unknowns.
  Elements also contain an arbitrary number of attributes.
  */
class XmlElement : public XmlNode
{
	public:
		/**
		 * \brief Construct an element.
		 *
		 */
		XmlElement (const char * in_value);
		
		/**
		 * \brief std::string constructor.
		 *
		 */
		XmlElement( const std::string& _value );
		
		/**
		 * \brief
		 *
		 */
		XmlElement( const XmlElement& );
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlElement& base );
		
		/**
		 * \brief
		 *
		 */
		virtual ~XmlElement();
		
		/**
		 * \brief Given an attribute name, Attribute() returns the value
		 * for the attribute of that name, or null if none exists.
		 * 
		 */
		const char* Attribute( std::string name ) const;
		
		/** 
		 * \brief Given an attribute name, Attribute() returns the value for the 
		 * attribute of that name, or null if none exists. If the attribute 
		 * exists and can be converted to an integer, the integer value will 
		 * be put in the return 'i', if 'i' is non-null.
		 */
		const char* Attribute( std::string name, int* i ) const;
		
		/** 
		 * \brief Given an attribute name, Attribute() returns the value
		 * for the attribute of that name, or null if none exists.
		 * If the attribute exists and can be converted to an double,
		 * the double value will be put in the return 'd', if 'd' is non-null.
		 * 
		 */
		const char* Attribute( std::string name, double* d ) const;
		
		/**
		 * \brief Given an attribute name, Attribute() returns the value
		 * for the attribute of that name, or null if none exists.
		 * 
		 */
		const char* Attribute( const char* name ) const;
		
		/** 
		 * \brief Given an attribute name, Attribute() returns the value for the 
		 * attribute of that name, or null if none exists. If the attribute 
		 * exists and can be converted to an integer, the integer value will 
		 * be put in the return 'i', if 'i' is non-null.
		 */
		const char* Attribute( const char* name, int* i ) const;
		
		/** 
		 * \brief Given an attribute name, Attribute() returns the value
		 * for the attribute of that name, or null if none exists.
		 * If the attribute exists and can be converted to an double,
		 * the double value will be put in the return 'd', if 'd' is non-null.
		 * 
		 */
		const char* Attribute( const char* name, double* d ) const;
		
		/** 
		 * \brief QueryIntAttribute examines the attribute - it is an alternative to the
		 * Attribute() method with richer error checking. If the attribute is an integer, 
		 * it is stored in 'value' and the call returns TIXML_SUCCESS. If it is not
		 * an integer, it returns TIXML_WRONG_TYPE. If the attribute  does not exist, 
		 * then TIXML_NO_ATTRIBUTE is returned.
		 * 
		 */	
		int QueryIntAttribute( const char* name, int* _value ) const;
		
		/**
		 * \brief QueryDoubleAttribute examines the attribute - see QueryIntAttribute().
		 *
		 */
		int QueryDoubleAttribute( const char* name, double* _value ) const;
		
		/**
		 * \brief QueryFloatAttribute examines the attribute - see QueryIntAttribute().
		 *
		 */
		int QueryFloatAttribute( const char* name, float* _value ) const ;
		
		/**
		 * Template form of the attribute query which will try to read the
		 * attribute into the specified type. Very easy, very powerful, but
		 * be careful to make sure to call this with the correct type.
		 * 
		 * \return TIXML_SUCCESS, TIXML_WRONG_TYPE, or TIXML_NO_ATTRIBUTE
		 * 
		 */
		template< typename T > int QueryValueAttribute( const std::string& name, T* outValue ) const
		{
			const XmlAttribute* node = attributeSet.Find( name );
			if ( !node )
				return TIXML_NO_ATTRIBUTE;
			
			std::stringstream sstream( node->ValueStr() );
			sstream >> *outValue;
			if ( !sstream.fail() )
				return TIXML_SUCCESS;
			return TIXML_WRONG_TYPE;
		}
		
		/** 
		 * \brief Sets an attribute of name to a given value. The attribute
		 * will be created if it does not exist, or changed if it does.
		 * 
		 */
		void SetAttribute( const char* name, const char * _value );
		
		/**
		 * \brief
		 *
		 */
		int QueryIntAttribute( const std::string& name, int* _value ) const;
		
		/**
		 * \brief
		 *
		 */
		int QueryDoubleAttribute( const std::string& name, double* _value ) const;
		
		/**
		 * \brief
		 *
		 */
		void SetAttribute( const std::string& name, const std::string& _value );
		
		/**
		 * \brief
		 *
		 */
		void SetAttribute( const std::string& name, int _value );
		
		/** Sets an attribute of name to a given value. The attribute
		  will be created if it does not exist, or changed if it does.
		  */
		void SetAttribute( const char * name, int value );
		
		/**
		 * \brief Sets an attribute of name to a given value. The attribute
		 * will be created if it does not exist, or changed if it does.
		 * 
		 */
		void SetDoubleAttribute( const char * name, double value );
		
		/** 
		 * \brief Deletes an attribute with the given name.
		 *
		 */
		void RemoveAttribute( const char * name );
		
		/**
		 * \brief
		 *
		 */
		void RemoveAttribute( const std::string& name );
		
		/**
		 * \brief
		 *
		 */
		const XmlAttribute* FirstAttribute() const	;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute* FirstAttribute();
		
		/**
		 * \brief
		 *
		 */
		const XmlAttribute* LastAttribute()	const 	;
		
		/**
		 * \brief
		 *
		 */
		XmlAttribute* LastAttribute();
		
		/** Convenience function for easy access to the text inside an element. Although easy
		  and concise, GetText() is limited compared to getting the XmlText child
		  and accessing it directly.
		  
		  If the first child of 'this' is a XmlText, the GetText()
		  returns the character string of the Text node, else null is returned.
		  
		  This is a convenient method for getting the text of simple contained text:
		  @verbatim
		  <foo>This is text</foo>
		  const char* str = fooElement->GetText();
		  @endverbatim
		  
		  'str' will be a pointer to "This is text". 
		  
		  Note that this function can be misleading. If the element foo was created from
		  this XML:
		  @verbatim
		  <foo><b>This is text</b></foo> 
		  @endverbatim
		  
		  then the value of str would be null. The first child node isn't a text node, it is
		  another element. From this XML:
		  @verbatim
		  <foo>This is <b>text</b></foo> 
		  @endverbatim
		  GetText() will return "This is ".
		  
			WARNING: GetText() accesses a child node - don't become confused with the 
			similarly named XmlHandle::Text() and XmlNode::ToText() which are 
			safe type casts on the referenced node.
			*/
		std::string GetText() const;
		
		/**
		 * \brief Creates a new Element and returns it - the returned element is a copy.
		 *
		 */
		 virtual XmlNode* Clone() const;

		 /**
		  * \brief Print the Element to a FILE stream.
		  *
		  */
		virtual void Print( FILE* cfile, int depth ) const;
		
		/**
		 * \brief Attribtue parsing starts: next char past '<' returns: next char past '>'
		 *
		 */
		virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlElement*     ToElement()     const ;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlElement*           ToElement();
		
		/** Walk the XML tree visiting this node and all of its children. 
		*/
		virtual bool Accept( XmlVisitor* visitor ) const;
		
	protected:
		
		/**
		 * \brief
		 *
		 */
		void CopyTo( XmlElement* target ) const;
		
		/**
		 * \brief Like clear, but initializes 'this' object as well

		 *
		 */
		void ClearThis();
		
		/**
		 * \brief Used to be public [internal use]
		 *
		 */
		virtual void StreamIn( std::istream * in, std::string * tag );
		
		/**
		 * \brief Reads the "value" of the element -- another element, or text.
		 * This should terminate with the current end tag.
		 *
		 */
		const char* ReadValue( const char* in, XmlParsingData* prevData, XmlEncoding encoding );
		
	private:
		
		XmlAttributeSet attributeSet;
};

/**	
 * \brief An XML comment.
 *
 */
class XmlComment : public XmlNode
{
	public:
		/**
		 * \brief Constructs an empty comment.
		 *
		 */
		XmlComment();
		
		/**
		 * \brief Construct a comment from text.
		 *
		 */
		XmlComment( const char* _value );
		
		/**
		 * \brief
		 *
		 */
		XmlComment( const XmlComment& );
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlComment& base );
		
		/**
		 * \brief
		 *
		 */
		virtual ~XmlComment();
		
		/**
		 * \brief Returns a copy of this Comment.
		 *
		 */
		virtual XmlNode* Clone() const;
		
		/**
		 * \brief Write this Comment to a FILE stream.
		 *
		 */
		virtual void Print( FILE* cfile, int depth ) const;
		
		/**
		 * \brief Attribtue parsing starts: at the ! of the !-- returns: next char past '>'
		 *
		 */
		virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );
		
		/**
		 * \brief Cast to a more defined type. Will return null not of the requested type.
		 *
		 */
		virtual const XmlComment*  ToComment() const;
		
		/**
		 * \brief Cast to a more defined type. Will return null not of the requested type.
		 *
		 */
		virtual XmlComment*  ToComment() ;
		
		/** 
		 * \brief Walk the XML tree visiting this node and all of its children. 
		 *
		 */
		virtual bool Accept( XmlVisitor* visitor ) const;
		
	protected:
		/**
		 * \brief
		 *
		 */
		void CopyTo( XmlComment* target ) const;
		
		/**
		 * \brief used to be public
		 *
		 */
		virtual void StreamIn( std::istream * in, std::string * tag );
		
		//	virtual void StreamOut( TIXML_OSTREAM * out ) const;
		
	private:
		
};

/** 
 * \brief XML text. A text node can have 2 ways to output the next. "normal" output 
 * and CDATA. It will default to the mode it was parsed from the XML file and
 * you generally want to leave it alone, but you can change the output mode with 
 * SetCDATA() and query it with CDATA().
 *
 */
class XmlText : public XmlNode{
	friend class XmlElement;

	public:
		/**
		 * \brief Constructor for text element. By default, it is treated as 
		 * normal, encoded text. If you want it be output as a CDATA text
		 * element, set the parameter _cdata to 'true'
		 * 
  		 */
		XmlText (const char * initValue );
	
		/**
		 * \brief
		 *
		 */
		virtual ~XmlText() ;
		
		/**
		 * \brief
		 *
		 */
		XmlText( const std::string& initValue );
		
		/**
		 * \brief
		 *
		 */
		XmlText( const XmlText& copy ) ;
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlText& base );
		
		/**
		 * \brief Write this text object to a FILE stream.
		 *
		 */
		virtual void Print( FILE* cfile, int depth ) const;
		
		/**
		 * \brief Queries whether this represents text using a CDATA section.
		 *
		 */
		bool CDATA() const;
		
		/**
		 * \brief Turns on or off a CDATA representation of text.
		 *
		 */
		void SetCDATA( bool _cdata );
		
		/**
		 * \brief
		 *
		 */
		virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );
		
		/**
		 * \brief Cast to a more defined type. Will return null not of the requested type.
		 *
		 */
		virtual const XmlText* ToText() const;

		/**
		 * \brief Cast to a more defined type. Will return null not of the requested type.
		 *
		 */
		virtual XmlText * ToText();
		
		/** 
		 * \brief Walk the XML tree visiting this node and all of its children. 
		 *
		 */
		virtual bool Accept( XmlVisitor* content ) const;
		
	protected :
		/**
		 * \brief Creates a new Element and returns it.
		 *
		 */
		virtual XmlNode* Clone() const;

		/**
		 * \brief
		 *
		 */
		void CopyTo( XmlText* target ) const;
		
		/**
		 * \brief Returns true if all white space and new lines.
		 *
		 */
		bool Blank() const;

		/**
		 * \brief
		 *
		 */
		virtual void StreamIn( std::istream * in, std::string * tag );
		
	private:
		bool cdata;			// true if this should be input and output as a CDATA style text element
};

/**
 * \brief In correct XML the declaration is the first entry in the file.
 *   @verbatim
 *   <?xml version="1.0" standalone="yes"?>
 *   @endverbatim
 *   
 *   nyXml will happily read or write files without a declaration,
 *   however. There are 3 possible attributes to the declaration:
 *   version, encoding, and standalone.
 *   
 * Note: In this version of the code, the attributes are
 * handled as special cases, not generic attributes, simply
 * because there can only be at most 3 and they are always the same.
 */
class XmlDeclaration : public XmlNode{
	public:
		/**
		 * \brief Construct an empty declaration.
		 *
		 */
		XmlDeclaration();
		
		/**
		 * \brief Constructor.
		 *
		 */
		XmlDeclaration(const std::string& _version,
				const std::string& _encoding,
				const std::string& _standalone );
		
		/**
		 * \brief Construct.
		 *
		 */
		XmlDeclaration(const char* _version,
				const char* _encoding,
				const char* _standalone );
		
		/**
		 * \brief
		 *
		 */
		XmlDeclaration( const XmlDeclaration& copy );
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlDeclaration& copy );
		
		/**
		 * \brief
		 *
		 */
		virtual ~XmlDeclaration();
		
		/**
		 * \brief Version. Will return an empty string if none was found.
		 *
		 */
		const char *Version() const	;
		
		/**
		 * \brief Encoding. Will return an empty string if none was found.
		 *
		 */
		const char *Encoding() const;
		
		/** 
		 * \brief Is this a standalone document?
		 *
		 */
		const char *Standalone() const;
		
		/**
		 * \brief Creates a copy of this Declaration and returns it.
		 *
		 * 
		 */
		virtual XmlNode* Clone() const;
		
		/**
		 * \brief Print this declaration to a FILE stream.
		 *
		 */
		virtual void Print( FILE* cfile, int depth, std::string* str ) const;
		
		/**
		 * \brief
		 *
		 */
		virtual void Print( FILE* cfile, int depth ) const ;
		
		/**
		 * \brief
		 *
		 */
		virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlDeclaration* ToDeclaration() const ;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlDeclaration*       ToDeclaration();
		
		/**
		 * \brief Walk the XML tree visiting this node and all of its children. 
		 * 
		 */
		virtual bool Accept( XmlVisitor* visitor ) const;
		
	protected:
		/**
		 * \brief
		 *
		 */
		void CopyTo( XmlDeclaration* target ) const;
		
		/**
		 * \brief used to be public
		 *
		 */
		virtual void StreamIn( std::istream * in, std::string * tag );
		
	private:
		std::string version;
		std::string encoding;
		std::string standalone;
};


/**
 * \brief Any tag that tinyXml doesn't recognize is saved as an
 *  unknown. It is a tag of text, but should not be modified.
 *  It will be written back to the XML, unchanged, when the file
 *  is saved.
 *  
 *  DTD tags get thrown into XmlUnknowns.
 */
class XmlUnknown : public XmlNode{
	public:
		/**
		 * \brief
		 *
		 */
		XmlUnknown() ;
		
		/**
		 * \brief
		 *
		 */
		virtual ~XmlUnknown() ;
		
		/**
		 * \brief
		 *
		 */
		XmlUnknown( const XmlUnknown& copy ) ;
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlUnknown& copy );
		
		/**
		 * \brief Creates a copy of this Unknown and returns it.
		 *
		 */
		virtual XmlNode* Clone() const;
		
		/**
		 * \brief Print this Unknown to a FILE stream.
		 *
		 */
		virtual void Print( FILE* cfile, int depth ) const;
		
		/**
		 * \brief
		 *
		 */
		virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlUnknown*     ToUnknown()     const;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlUnknown*           ToUnknown()	 ;
		
		/**
		 * \brief Walk the XML tree visiting this node and all of its children. 
		 * 
		 */
		virtual bool Accept( XmlVisitor* content ) const;
		
	protected:
		/**
		 * \brief
		 *
		 */
		void CopyTo( XmlUnknown* target ) const;
		
		/**
		 * \brief
		 *
		 */
		virtual void StreamIn( std::istream * in, std::string * tag );
		
	private:
		
};


/**
 * \brief Always the top level node. A document binds together all the
 * XML pieces. It can be saved, loaded, and printed to the screen.
 * The 'value' of a document node is the xml file name.
 * 
 */
class XmlDocument : public XmlNode{
	public:
		/**
		 * \brief Create an empty document, that has no name.
		 *
		 */
		XmlDocument();
		
		/**
		 * \brief Create a document with a name. The name of the document is also 
		 * the filename of the xml.
		 *
		 */
		XmlDocument( const char * documentName );
		
		/**
		 * \brief Constructor.
		 *
		 */
		XmlDocument( const std::string& documentName );
		
		/**
		 * \brief
		 *
		 */
		XmlDocument( const XmlDocument& copy );
		
		/**
		 * \brief
		 *
		 */
		void operator=( const XmlDocument& copy );
		
		/**
		 * \brief
		 *
		 */
		virtual ~XmlDocument();
		
		/**
		 * \brief Load a file using the current document value. Returns true 
		 * if successful. Will delete any existing document data before loading.
		 *
		 */
		bool LoadFile( XmlEncoding encoding = TIXML_DEFAULT_ENCODING );
		
		/**
		 * \brief Save a file using the current document value. Returns true if successful.
		 *
		 */
		bool SaveFile() const;
		
		/**
		 * \brief Load a file using the given filename. Returns true if successful.
		 *
		 */
		bool LoadFile( const char * filename, XmlEncoding encoding = TIXML_DEFAULT_ENCODING );
		
		/**
		 * \brief Save a file using the given filename. Returns true if successful.
		 *
		 */
		bool SaveFile( const char * filename ) const;
		
		/**
		 * \brief Load a file using the given FILE*. Returns true if successful. Note that 
		 * this method doesn't stream - the entire object pointed at by the FILE* will be 
		 * interpreted as an XML file. nyXML doesn't stream in XML from the current file 
		 * location. Streaming may be added in the future.
		 *
		 */
		bool LoadFile( FILE*, XmlEncoding encoding = TIXML_DEFAULT_ENCODING );
		
		/**
		 * \brief Save a file using the given FILE*. Returns true if successful.
		 *
		 */
		bool SaveFile( FILE* ) const;
		
		/**
		 * \brief
		 *
		 */
		bool LoadFile( const std::string& filename, XmlEncoding encoding = TIXML_DEFAULT_ENCODING );

		/**
		 * \brief
		 *
		 */
		bool SaveFile( const std::string& filename ) const;		///< STL std::string version.
		
		/** Parse the given null terminated block of xml data. Passing in an encoding to this
		  method (either TIXML_ENCODING_LEGACY or TIXML_ENCODING_UTF8 will force nyXml
		  to use that encoding, regardless of what nyXml might otherwise try to detect.
		  */
		virtual const char* Parse( const char* p, XmlParsingData* data = 0, XmlEncoding encoding = TIXML_DEFAULT_ENCODING );
		
		/** Get the root element -- the only top level element -- of the document.
		  In well formed XML, there should only be one. nyXml is tolerant of
		  multiple elements at the document level.
		  */
		const XmlElement* RootElement() const	;
		
		XmlElement* RootElement();
		
		/** If an error occurs, Error will be set to true. Also,
		  - The ErrorId() will contain the integer identifier of the error (not generally useful)
		  - The ErrorDesc() method will return the name of the error. (very useful)
		  - The ErrorRow() and ErrorCol() will return the location of the error (if known)
		  */	
		bool Error() const;
		
		/// Contains a textual (english) description of the error if one occurs.
		const char * ErrorDesc() const	;
		
		/** Generally, you probably want the error string ( ErrorDesc() ). But if you
		  prefer the ErrorId, this function will fetch it.
		  */
		int ErrorId()	const;
		
		/** Returns the location (if known) of the error. The first column is column 1, 
		  and the first row is row 1. A value of 0 means the row and column wasn't applicable
		  (memory errors, for example, have no row/column) or the parser lost the error. (An
		  error in the error reporting, in that case.)
		  
		  @sa SetTabSize, Row, Column
		  */
		int ErrorRow();
		
		int ErrorCol()	;
		
		/** SetTabSize() allows the error reporting functions (ErrorRow() and ErrorCol())
		  to report the correct values for row and column. It does not change the output
		  or input in any way.
		  
		  By calling this method, with a tab size
		  greater than 0, the row and column of each node and attribute is stored
		  when the file is loaded. Very useful for tracking the DOM back in to
		  the source file.
		  
		  The tab size is required for calculating the location of nodes. If not
		  set, the default of 4 is used. The tabsize is set per document. Setting
		  the tabsize to 0 disables row/column tracking.
		  
		  Note that row and column tracking is not supported when using operator>>.
		  
		  The tab size needs to be enabled before the parse or load. Correct usage:
		  @verbatim
		  XmlDocument doc;
		  doc.SetTabSize( 8 );
		  doc.Load( "myfile.xml" );
		  @endverbatim
		  
		  @sa Row, Column
		  */
		void SetTabSize( int _tabsize )	;
		
		int TabSize() const;
		
		/**
		 * \brief If you have handled the error, it can be reset with this call. The error
		 * state is automatically cleared if you Parse a new XML block.
		 *
		 */
		void ClearError();
		
		/** 
		 * \brief Write the document to standard out using formatted printing ("pretty print").
		 *
		 */
		void Print() const;
		
		/**
		 * \brief Write the document to a string using formatted printing ("pretty print").
		 * This will allocate a character array (new char[]) and return it as a pointer. The
		 * calling code pust call delete[] on the return char* to avoid a memory leak.
		 *
		 */
		//char* PrintToMemory() const; 
		
		/**
		 * \brief Print this Document to a FILE stream.
		 *
		 */
		virtual void Print( FILE* cfile, int depth = 0 ) const;
		
		/**
		 * \brief
		 *
		 */
		void SetError( int err, const char* errorLocation, XmlParsingData* prevData, XmlEncoding encoding );
		
		/**
		 * \brief
		 *
		 */
		virtual const XmlDocument*    ToDocument()    const;
		
		/**
		 * \brief
		 *
		 */
		virtual XmlDocument*          ToDocument()   ;
		
		/** 
		 * \brief Walk the XML tree visiting this node and all of its children. 
		 *
		 */
		virtual bool Accept( XmlVisitor* content ) const;
		
	protected :
		/**
		 * \brief
		 *
		 */
		virtual XmlNode* Clone() const;
		/**
		 * \brief
		 *
		 */
		virtual void StreamIn( std::istream * in, std::string * tag );
		
	private:
		/**
		 * \brief
		 *
		 */
		void CopyTo( XmlDocument* target ) const;
		
		bool error;
		int  errorId;
		std::string errorDesc;
		int tabsize;
		XmlCursor errorLocation;
		bool useMicrosoftBOM;		// the UTF-8 BOM were found when read. Note this, and try to write.
};


/**
  A XmlHandle is a class that wraps a node pointer with null checks; this is
  an incredibly useful thing. Note that XmlHandle is not part of the nyXml
  DOM structure. It is a separate utility class.
  
  Take an example:
  @verbatim
  <Document>
  <Element attributeA = "valueA">
  <Child attributeB = "value1" />
  <Child attributeB = "value2" />
  </Element>
  <Document>
  @endverbatim
  
  Assuming you want the value of "attributeB" in the 2nd "Child" element, it's very 
  easy to write a *lot* of code that looks like:
  
  @verbatim
  XmlElement* root = document.FirstChildElement( "Document" );
  if ( root )
  {
  XmlElement* element = root->FirstChildElement( "Element" );
  if ( element )
  {
  XmlElement* child = element->FirstChildElement( "Child" );
  if ( child )
  {
  XmlElement* child2 = child->NextSiblingElement( "Child" );
  if ( child2 )
  {
// Finally do something useful.
@endverbatim

And that doesn't even cover "else" cases. XmlHandle addresses the verbosity
of such code. A XmlHandle checks for null	pointers so it is perfectly safe 
and correct to use:

@verbatim
XmlHandle docHandle( &document );
XmlElement* child2 = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).Child( "Child", 1 ).ToElement();
if ( child2 )
{
// do something useful
@endverbatim

Which is MUCH more concise and useful.

It is also safe to copy handles - internally they are nothing more than node pointers.
@verbatim
XmlHandle handleCopy = handle;
@endverbatim

What they should not be used for is iteration:

@verbatim
int i=0; 
while ( true )
{
XmlElement* child = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).Child( "Child", i ).ToElement();
if ( !child )
break;
// do something
++i;
}
@endverbatim

It seems reasonable, but it is in fact two embedded while loops. The Child method is 
a linear walk to find the element, so this code would iterate much more than it needs 
to. Instead, prefer:

@verbatim
XmlElement* child = docHandle.FirstChild( "Document" ).FirstChild( "Element" ).FirstChild( "Child" ).ToElement();

for( child; child; child=child->NextSiblingElement() )
{
	// do something
}
@endverbatim
*/
class XmlHandle{
	public:
		/// Create a handle from any node (at any depth of the tree.) This can be a null pointer.
		XmlHandle( XmlNode* _node )		;
		
		/// Copy constructor
		XmlHandle( const XmlHandle& ref )	;
		
		XmlHandle operator=( const XmlHandle& ref );
		
		/// Return a handle to the first child node.
		XmlHandle FirstChild() const;
		
		/// Return a handle to the first child node with the given name.
		XmlHandle FirstChild( const char * value ) const;
		
		/// Return a handle to the first child element.
		XmlHandle FirstChildElement() const;
		
		/// Return a handle to the first child element with the given name.
		XmlHandle FirstChildElement( const char * value ) const;
		
		/** Return a handle to the "index" child with the given name. 
		  The first child is 0, the second 1, etc.
		  */
		XmlHandle Child( const char* value, int index ) const;
		
		/** Return a handle to the "index" child. 
		  The first child is 0, the second 1, etc.
		  */
		XmlHandle Child( int index ) const;
		
		/** Return a handle to the "index" child element with the given name. 
		  The first child element is 0, the second 1, etc. Note that only XmlElements
		  are indexed: other types are not counted.
		  */
		XmlHandle ChildElement( const char* value, int index ) const;
		
		/** Return a handle to the "index" child element. 
		  The first child element is 0, the second 1, etc. Note that only XmlElements
		  are indexed: other types are not counted.
		  */
		XmlHandle ChildElement( int index ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlHandle FirstChild( const std::string& _value ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlHandle FirstChildElement( const std::string& _value ) const;
		
		/**
		 * \brief
		 *
		 */
		XmlHandle Child( const std::string& _value, int index ) const	;	
		
		/**
		 * \brief
		 *
		 */
		XmlHandle ChildElement( const std::string& _value, int index ) const	;
		
		/** Return the handle as a XmlNode. This may return null.
		*/
		XmlNode* ToNode() const	;
		
		/** Return the handle as a XmlElement. This may return null.
		*/
		XmlElement* ToElement() const	;
		
		/**	Return the handle as a XmlText. This may return null.
		*/
		XmlText* ToText() const;
		
		/** Return the handle as a XmlUnknown. This may return null.
		*/
		XmlUnknown* ToUnknown() const		;
		
		/** @deprecated use ToNode. 
		  Return the handle as a XmlNode. This may return null.
		  */
		XmlNode* Node() const;
		
		/** @deprecated use ToElement. 
		  Return the handle as a XmlElement. This may return null.
		  */
		XmlElement* Element() const	;
		
		/**	@deprecated use ToText()
		  Return the handle as a XmlText. This may return null.
		  */
		XmlText* Text() const	;
		
		/** @deprecated use ToUnknown()
		  Return the handle as a XmlUnknown. This may return null.
		  */
		XmlUnknown* Unknown() const;
		
	private:
		XmlNode* node;
};

/** Print to memory functionality. The XmlPrinter is useful when you need to:

	-# Print to memory (especially in non-STL mode)
	-# Control formatting (line endings, etc.)

	When constructed, the XmlPrinter is in its default "pretty printing" mode.
	Before calling Accept() you can call methods to control the printing
	of the XML document. After XmlNode::Accept() is called, the printed document can
	be accessed via the CStr(), Str(), and Size() methods.

	XmlPrinter uses the Visitor API.
	@verbatim
	XmlPrinter printer;
	printer.SetIndent( "\t" );

	doc.Accept( &printer );
	fprintf( stdout, "%s", printer.CStr() );
	@endverbatim
*/
class XmlPrinter : public XmlVisitor
{
	public:
		/**
		 * \brief
		 *
		 */
		XmlPrinter();
		
		/**
		 * \brief
		 *
		 */
		virtual bool VisitEnter( const XmlDocument& doc );
		
		/**
		 * \brief
		 *
		 */
		virtual bool VisitExit( const XmlDocument& doc );
		
		/**
		 * \brief
		 *
		 */
		virtual bool VisitEnter( const XmlElement& element, const XmlAttribute* firstAttribute );
		
		/**
		 * \brief
		 *
		 */
		virtual bool VisitExit( const XmlElement& element );
		
		/**
		 * \brief
		 *
		 */
		virtual bool Visit( const XmlDeclaration& declaration );
		
		/**
		 * \brief
		 *
		 */
		virtual bool Visit( const XmlText& text );
		
		/**
		 * \brief
		 *
		 */
		virtual bool Visit( const XmlComment& comment );
		
		/**
		 * \brief
		 *
		 */
		virtual bool Visit( const XmlUnknown& unknown );
		
		/** Set the indent characters for printing. By default 4 spaces
		  but tab (\t) is also useful, or null/empty string for no indentation.
		  */
		void SetIndent( const char* _indent );
		
		/// Query the indention string.
		const char* Indent();
		
		/** Set the line breaking string. By default set to newline (\n). 
		  Some operating systems prefer other characters, or can be
		  set to the null/empty string for no indenation.
		  */
		void SetLineBreak( const char* _lineBreak );
		
		/// Query the current line breaking string.
		const char* LineBreak();
		
		/** Switch over to "stream printing" which is the most dense formatting without 
		  linebreaks. Common when the XML is needed for network transmission.
		  */
		void SetStreamPrinting();
		
		/// Return the result.
		const char* CStr();

		/// Return the length of the result string.
		size_t Size();
		
		/// Return the result.
		const std::string& Str();
		
	private:
		/**
		 * \brief
		 *
		 */
		void DoIndent();
		
		/**
		 * \brief
		 *
		 */
		void DoLineBreak() ;
		
		int depth;
		bool simpleTextPrint;
		std::string buffer;
		std::string indent;
		std::string lineBreak;
};

}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif


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
#include "jcommon/jxmlparser.h"

#include <iostream>

static int gPass = 0;
static int gFail = 0;

bool XmlTest (const char* testString, const char* expected, const char* found, bool noEcho = false)
{
	bool pass = !strcmp( expected, found );

	if ( pass ) {
		std::cout << "[pass]" << std::endl;
	} else {
		std::cout << "[fail]" << std::endl;
	}

	if ( noEcho ) {
		std::cout << " " << testString << std::endl;
	} else {
		std::cout << " " << testString << " [" << expected << "][" << found << "]" << std::endl;
	}

	if ( pass ) {
		++gPass;
	} else {
		++gFail;
	}

	return pass;
}


bool XmlTest( const char* testString, int expected, int found, bool noEcho = false )
{
	bool pass = ( expected == found );

	if ( pass ) {
		std::cout << "[pass]" << std::endl;
	} else {
		std::cout << "[fail]" << std::endl;
	}

	if ( noEcho ) {
		std::cout << " " << testString << std::endl;
	} else {
		std::cout << " " << testString << " [" << expected << "][" << found << "]" << std::endl;
	}

	if ( pass ) {
		++gPass;
	} else {
		++gFail;
	}

	return pass;
}

// This file demonstrates some basic functionality of nyXml.
// Note that the example is very contrived. It presumes you know
// what is in the XML file. But it does test the basic operations,
// and show how to add and remove nodes.
int main()
{
	// We start with the 'demoStart' todo list. Process it. And
	// should hopefully end up with the todo list as illustrated.
	const char* demoStart =
		"<?xml version=\"1.0\"  standalone='no' >\n"
		"<!-- Our to do list data -->"
		"<ToDo>\n"
		"<!-- Do I need a secure PDA? -->\n"
		"<Item priority=\"1\" distance='close'> Go to the <bold>Toy store!</bold></Item>"
		"<Item priority=\"2\" distance='none'> Do bills   </Item>"
		"<Item priority=\"2\" distance='far &amp; back'> Look for Evil Dinosaurs! </Item>"
		"</ToDo>";
		
	{

		/*	What the todo list should look like after processing.
			In stream (no formatting) representation. */
		const char* demoEnd =
			"<?xml version=\"1.0\" standalone=\"no\" ?>"
			"<!-- Our to do list data -->"
			"<ToDo>"
			"<!-- Do I need a secure PDA? -->"
			"<Item priority=\"2\" distance=\"close\">Go to the"
			"<bold>Toy store!"
			"</bold>"
			"</Item>"
			"<Item priority=\"1\" distance=\"far\">Talk to:"
			"<Meeting where=\"School\">"
			"<Attendee name=\"Marple\" position=\"teacher\" />"
			"<Attendee name=\"Voel\" position=\"counselor\" />"
			"</Meeting>"
			"<Meeting where=\"Lunch\" />"
			"</Item>"
			"<Item priority=\"2\" distance=\"here\">Do bills"
			"</Item>"
			"</ToDo>";

		// The example parses from the character string (above):
		#if defined( WIN32 ) && defined( TUNE )
		_CrtMemCheckpoint( &startMemState );
		#endif	

		{
			// Write to a file and read it back, to check file I/O.

      jcommon::XmlDocument doc( "demotest.xml" );
			doc.Parse( demoStart );

			if ( doc.Error() ) {
				std::cout << "Error in " << doc.Value() << ": " << doc.ErrorDesc() << std::endl;

				exit( 1 );
			}

			doc.SaveFile();
		}

		jcommon::XmlDocument doc( "demotest.xml" );
		bool loadOkay = doc.LoadFile();

		if ( !loadOkay ) {
			std::cout << "Could not load test file 'demotest.xml'. Error='" << doc.ErrorDesc() << "'. Exiting." << std::endl;

			exit( 1 );
		}

		std::cout << "** Demo doc read from disk: **\n" << std::endl;
		std::cout << "** Printing via doc.Print **" << std::endl;

		doc.Print( stdout );

		{
			std::cout << "** Printing via XmlPrinter **" << std::endl;
			jcommon::XmlPrinter printer;
			doc.Accept( &printer );
			std::cout << printer.Str() << std::endl;
		}

		{
			std::cout << "** Printing via operator<< **\n" << doc << std::endl;
		}

		jcommon::XmlNode* node = 0;
		jcommon::XmlElement* todoElement = 0;
		jcommon::XmlElement* itemElement = 0;

		// --------------------------------------------------------
		// An example of changing existing attributes, and removing
		// an element from the document.
		// --------------------------------------------------------

		// Get the "ToDo" element.
		// It is a child of the document, and can be selected by name.
		node = doc.FirstChild( "ToDo" );
		assert( node );
		todoElement = node->ToElement();
		assert( todoElement  );

		// Going to the toy store is now our second priority...
		// So set the "priority" attribute of the first item in the list.
		node = todoElement->FirstChildElement();	// This skips the "PDA" comment.
		assert( node );
		itemElement = node->ToElement();
		assert( itemElement  );
		itemElement->SetAttribute( "priority", 2 );

		// Change the distance to "doing bills" from
		// "none" to "here". It's the next sibling element.
		itemElement = itemElement->NextSiblingElement();
		assert( itemElement );
		itemElement->SetAttribute( "distance", "here" );

		// Remove the "Look for Evil Dinosaurs!" item.
		// It is 1 more sibling away. We ask the parent to remove
		// a particular child.
		itemElement = itemElement->NextSiblingElement();
		todoElement->RemoveChild( itemElement );

		itemElement = 0;

		// --------------------------------------------------------
		// What follows is an example of created elements and text
		// nodes and adding them to the document.
		// --------------------------------------------------------

		// Add some meetings.
		jcommon::XmlElement item( "Item" );
		item.SetAttribute( "priority", "1" );
		item.SetAttribute( "distance", "far" );

		jcommon::XmlText text( "Talk to:" );

		jcommon::XmlElement meeting1( "Meeting" );
		meeting1.SetAttribute( "where", "School" );

		jcommon::XmlElement meeting2( "Meeting" );
		meeting2.SetAttribute( "where", "Lunch" );

		jcommon::XmlElement attendee1( "Attendee" );
		attendee1.SetAttribute( "name", "Marple" );
		attendee1.SetAttribute( "position", "teacher" );

		jcommon::XmlElement attendee2( "Attendee" );
		attendee2.SetAttribute( "name", "Voel" );
		attendee2.SetAttribute( "position", "counselor" );

		// Assemble the nodes we've created:
		meeting1.InsertEndChild( attendee1 );
		meeting1.InsertEndChild( attendee2 );

		item.InsertEndChild( text );
		item.InsertEndChild( meeting1 );
		item.InsertEndChild( meeting2 );

		// And add the node to the existing list after the first child.
		node = todoElement->FirstChild( "Item" );
		assert( node );
		itemElement = node->ToElement();
		assert( itemElement );

		todoElement->InsertAfterChild( itemElement, item );

		std::cout << "\n** Demo doc processed: **\n" << std::endl;
		
		doc.Print( stdout );

		std::cout << "** Demo doc processed to stream: **\n" << doc << std::endl << std::endl;

		// --------------------------------------------------------
		// Different tests...do we have what we expect?
		// --------------------------------------------------------

		int count = 0;
		jcommon::XmlElement*	element;

		//////////////////////////////////////////////////////

		std::cout << "** Basic structure. **\n";
		std::ostringstream outputStream( std::ostringstream::out );
		outputStream << doc;
		XmlTest( "Output stream correct.",	std::string( demoEnd ).c_str(),
											outputStream.str().c_str(), true );

		node = doc.RootElement();
		XmlTest( "Root element exists.", true, ( node != 0 && node->ToElement() ) );
		XmlTest ( "Root element value is 'ToDo'.", "ToDo",  node->Value());

		node = node->FirstChild();
		XmlTest( "First child exists & is a comment.", true, ( node != 0 && node->ToComment() ) );
		node = node->NextSibling();
		XmlTest( "Sibling element exists & is an element.", true, ( node != 0 && node->ToElement() ) );
		XmlTest ( "Value is 'Item'.", "Item", node->Value() );

		node = node->FirstChild();
		XmlTest ( "First child exists.", true, ( node != 0 && node->ToText() ) );
		XmlTest ( "Value is 'Go to the'.", "Go to the", node->Value() );

		//////////////////////////////////////////////////////
		std::cout << "\n** Iterators. **" << std::endl;

		// Walk all the top level nodes of the document.
		count = 0;
		
		for( node = doc.FirstChild(); node; node = node->NextSibling() ) {
			count++;
		}

		XmlTest( "Top level nodes, using First / Next.", 3, count );

		count = 0;

		for( node = doc.LastChild(); node; node = node->PreviousSibling() ) {
			count++;
		}

		XmlTest( "Top level nodes, using Last / Previous.", 3, count );

		// Walk all the top level nodes of the document, using a different syntax.
		count = 0;

		for( node = doc.IterateChildren( 0 ); node; node = doc.IterateChildren( node ) ) {
			count++;
		}

		XmlTest( "Top level nodes, using IterateChildren.", 3, count );

		// Walk all the elements in a node.
		count = 0;
		for( element = todoElement->FirstChildElement(); element; element = element->NextSiblingElement() ) {
			count++;
		}
		XmlTest( "Children of the 'ToDo' element, using First / Next.",
			3, count );

		// Walk all the elements in a node by value.
		count = 0;
		for( node = todoElement->FirstChild( "Item" ); node; node = node->NextSibling( "Item" ) ) {
			count++;
		}

		XmlTest( "'Item' children of the 'ToDo' element, using First/Next.", 3, count );

		count = 0;

		for( node = todoElement->LastChild( "Item" ); node; node = node->PreviousSibling( "Item" ) ) {
			count++;
		}

		XmlTest( "'Item' children of the 'ToDo' element, using Last/Previous.", 3, count );

		{
			std::cout << "\n** Parsing. **\n";
			std::istringstream parse0( "<Element0 attribute0='foo0' attribute1= noquotes attribute2 = '&gt;' />" );
      jcommon::XmlElement element0( "default" );
			parse0 >> element0;

			XmlTest ( "Element parsed, value is 'Element0'.", "Element0", element0.Value() );
			XmlTest ( "Reads attribute 'attribute0=\"foo0\"'.", "foo0", element0.Attribute( "attribute0" ));
			XmlTest ( "Reads incorrectly formatted 'attribute1=noquotes'.", "noquotes", element0.Attribute( "attribute1" ) );
			XmlTest ( "Read attribute with entity value '>'.", ">", element0.Attribute( "attribute2" ) );
		}

		{
			const char* error =	"<?xml version=\"1.0\" standalone=\"no\" ?>\n"
								"<passages count=\"006\" formatversion=\"20020620\">\n"
								"    <wrong error>\n"
								"</passages>";

			jcommon::XmlDocument docTest;
			docTest.Parse( error );
			XmlTest( "Error row", docTest.ErrorRow(), 3 );
			XmlTest( "Error column", docTest.ErrorCol(), 17 );
		}

		{
			//////////////////////////////////////////////////////
			std::cout << "\n** Streaming. **\n";

			// Round trip check: stream in, then stream back out to verify. The stream
			// out has already been checked, above. We use the output

			std::istringstream inputStringStream( outputStream.str() );
			jcommon::XmlDocument document0;

			inputStringStream >> document0;

			std::ostringstream outputStream0( std::ostringstream::out );
			outputStream0 << document0;

			XmlTest( "Stream round trip correct.",	std::string( demoEnd ).c_str(), outputStream0.str().c_str(), true );

			std::string str;
			str << document0;

			XmlTest( "String printing correct.", std::string( demoEnd ).c_str(), str.c_str(), true );
		}
	}
	
	{
		const char* str = "<doc attr0='1' attr1='2.0' attr2='foo' />";

		jcommon::XmlDocument doc;
		doc.Parse( str );

    jcommon::XmlElement* ele = doc.FirstChildElement();

		int iVal, result;
		double dVal;

		result = ele->QueryDoubleAttribute( "attr0", &dVal );
		XmlTest( "Query attribute: int as double", result, TIXML_SUCCESS );
		XmlTest( "Query attribute: int as double", (int)dVal, 1 );
		result = ele->QueryDoubleAttribute( "attr1", &dVal );
		XmlTest( "Query attribute: double as double", (int)dVal, 2 );
		result = ele->QueryIntAttribute( "attr1", &iVal );
		XmlTest( "Query attribute: double as int", result, TIXML_SUCCESS );
		XmlTest( "Query attribute: double as int", iVal, 2 );
		result = ele->QueryIntAttribute( "attr2", &iVal );
		XmlTest( "Query attribute: not a number", result, TIXML_WRONG_TYPE );
		result = ele->QueryIntAttribute( "bar", &iVal );
		XmlTest( "Query attribute: does not exist", result, TIXML_NO_ATTRIBUTE );
	}
	
	{
		const char* str =	"\t<?xml version=\"1.0\" standalone=\"no\" ?>\t<room doors='2'>\n"
							"</room>";

		jcommon::XmlDocument doc;
		doc.SetTabSize( 8 );
		doc.Parse( str );

		jcommon::XmlHandle docHandle( &doc );
		jcommon::XmlHandle roomHandle = docHandle.FirstChildElement( "room" );

		assert( docHandle.Node() );
		assert( roomHandle.Element() );

		jcommon::XmlElement* room = roomHandle.Element();
		assert( room );
		jcommon::XmlAttribute* doors = room->FirstAttribute();
		assert( doors );

		XmlTest( "Location tracking: Tab 8: room row", room->Row(), 1 );
		XmlTest( "Location tracking: Tab 8: room col", room->Column(), 49 );
		XmlTest( "Location tracking: Tab 8: doors row", doors->Row(), 1 );
		XmlTest( "Location tracking: Tab 8: doors col", doors->Column(), 55 );
	}
	
	{
		const char* str =	"\t<?xml version=\"1.0\" standalone=\"no\" ?>\t<room doors='2'>\n"
							"  <!-- Silly example -->\n"
							"    <door wall='north'>A great door!</door>\n"
							"\t<door wall='east'/>"
							"</room>";

		jcommon::XmlDocument doc;
		doc.Parse( str );

		jcommon::XmlHandle docHandle( &doc );
		jcommon::XmlHandle roomHandle = docHandle.FirstChildElement( "room" );
		jcommon::XmlHandle commentHandle = docHandle.FirstChildElement( "room" ).FirstChild();
		jcommon::XmlHandle textHandle = docHandle.FirstChildElement( "room" ).ChildElement( "door", 0 ).FirstChild();
		jcommon::XmlHandle door0Handle = docHandle.FirstChildElement( "room" ).ChildElement( 0 );
		jcommon::XmlHandle door1Handle = docHandle.FirstChildElement( "room" ).ChildElement( 1 );

		assert( docHandle.Node() );
		assert( roomHandle.Element() );
		assert( commentHandle.Node() );
		assert( textHandle.Text() );
		assert( door0Handle.Element() );
		assert( door1Handle.Element() );

		jcommon::XmlDeclaration* declaration = doc.FirstChild()->ToDeclaration();
		assert( declaration );
		jcommon::XmlElement* room = roomHandle.Element();
		assert( room );
		jcommon::XmlAttribute* doors = room->FirstAttribute();
		assert( doors );
		jcommon::XmlText* text = textHandle.Text();
		jcommon::XmlComment* comment = commentHandle.Node()->ToComment();
		assert( comment );
		jcommon::XmlElement* door0 = door0Handle.Element();
		jcommon::XmlElement* door1 = door1Handle.Element();

		XmlTest( "Location tracking: Declaration row", declaration->Row(), 1 );
		XmlTest( "Location tracking: Declaration col", declaration->Column(), 5 );
		XmlTest( "Location tracking: room row", room->Row(), 1 );
		XmlTest( "Location tracking: room col", room->Column(), 45 );
		XmlTest( "Location tracking: doors row", doors->Row(), 1 );
		XmlTest( "Location tracking: doors col", doors->Column(), 51 );
		XmlTest( "Location tracking: Comment row", comment->Row(), 2 );
		XmlTest( "Location tracking: Comment col", comment->Column(), 3 );
		XmlTest( "Location tracking: text row", text->Row(), 3 ); 
		XmlTest( "Location tracking: text col", text->Column(), 24 );
		XmlTest( "Location tracking: door0 row", door0->Row(), 3 );
		XmlTest( "Location tracking: door0 col", door0->Column(), 5 );
		XmlTest( "Location tracking: door1 row", door1->Row(), 4 );
		XmlTest( "Location tracking: door1 col", door1->Column(), 5 );
	}


	// --------------------------------------------------------
	// UTF-8 testing. It is important to test:
	//	1. Making sure name, value, and text read correctly
	//	2. Row, Col functionality
	//	3. Correct output
	// --------------------------------------------------------
	std::cout << "\n** UTF-8 **" << std::endl;
	{
		jcommon::XmlDocument doc( "utf8test.xml" );
		doc.LoadFile();
		if ( doc.Error() && doc.ErrorId() == jcommon::XmlBase::TIXML_ERROR_OPENING_FILE ) {
			std::cout << "WARNING: File 'utf8test.xml' not found.\n(Are you running the test from the wrong directory?)\nCould not test UTF-8 functionality." << std::endl;
		} else {
			jcommon::XmlHandle docH( &doc );
			// Get the attribute "value" from the "Russian" element and check it.
			jcommon::XmlElement* element = docH.FirstChildElement( "document" ).FirstChildElement( "Russian" ).Element();
			const uint8_t correctValue[] = {	
				0xd1U, 0x86U, 0xd0U, 0xb5U, 0xd0U, 0xbdU, 0xd0U, 0xbdU, 0xd0U, 0xbeU, 0xd1U, 0x81U, 0xd1U, 0x82U, 0xd1U, 0x8cU, 0 };

			XmlTest( "UTF-8: Russian value.", (const char*)correctValue, element->Attribute( "value" ), true );
			XmlTest( "UTF-8: Russian value row.", 4, element->Row() );
			XmlTest( "UTF-8: Russian value column.", 5, element->Column() );

			const uint8_t russianElementName[] = {	
				0xd0U, 0xa0U, 0xd1U, 0x83U, 0xd1U, 0x81U, 0xd1U, 0x81U, 0xd0U, 0xbaU, 0xd0U, 0xb8U, 0xd0U, 0xb9U, 0 };

			const char russianText[] = "<\xD0\xB8\xD0\xBC\xD0\xB5\xD0\xB5\xD1\x82>";

			jcommon::XmlText* text = docH.FirstChildElement( "document" ).FirstChildElement( (const char*) russianElementName ).Child( 0 ).Text();
			XmlTest( "UTF-8: Browsing russian element name.", russianText, text->Value(), true );
			XmlTest( "UTF-8: Russian element name row.", 7, text->Row() );
			XmlTest( "UTF-8: Russian element name column.", 47, text->Column() );

			jcommon::XmlDeclaration* dec = docH.Child( 0 ).Node()->ToDeclaration();
			XmlTest( "UTF-8: Declaration column.", 1, dec->Column() );
			XmlTest( "UTF-8: Document column.", 1, doc.Column() );

			// Now try for a round trip.
			doc.SaveFile( "utf8testout.xml" );

			// Check the round trip.
			char savedBuf[256];
			char verifyBuf[256];
			int okay = 1;

			FILE* saved  = fopen( "utf8testout.xml", "r" );
			FILE* verify = fopen( "utf8testverify.xml", "r" );
			if ( saved && verify )
			{
				while ( fgets( verifyBuf, 256, verify ) )
				{
					if (fgets( savedBuf, 256, saved ) == nullptr) 
					{
						break;
					}

					if ( strcmp( verifyBuf, savedBuf ) )
					{
						okay = 0;
						break;
					}
				}
				fclose( saved );
				fclose( verify );
			}
			XmlTest( "UTF-8: Verified multi-language round trip.", 1, okay );

			// On most Western machines, this is an element that contains
			// the word "resume" with the correct accents, in a latin encoding.
			// It will be something else completely on non-wester machines,
			// which is why nyXml is switching to UTF-8.
			const char latin[] = "<element>r\x82sum\x82</element>";

			jcommon::XmlDocument latinDoc;
			latinDoc.Parse( latin, 0, TIXML_ENCODING_LEGACY );

			text = latinDoc.FirstChildElement()->FirstChild()->ToText();
			XmlTest( "Legacy encoding: Verify text element.", "r\x82sum\x82", text->Value() );
		}
	}		

	//////////////////////
	// Copy and assignment
	//////////////////////
	std::cout << "** Copy and Assignment **" << std::endl;
	{
		jcommon::XmlElement element( "foo" );
		element.Parse( "<element name='value' />", 0, TIXML_ENCODING_UNKNOWN );

		jcommon::XmlElement elementCopy( element );
		jcommon::XmlElement elementAssign( "foo" );
		elementAssign.Parse( "<incorrect foo='bar'/>", 0, TIXML_ENCODING_UNKNOWN );
		elementAssign = element;

		XmlTest( "Copy/Assign: element copy #1.", "element", elementCopy.Value() );
		XmlTest( "Copy/Assign: element copy #2.", "value", elementCopy.Attribute( "name" ) );
		XmlTest( "Copy/Assign: element assign #1.", "element", elementAssign.Value() );
		XmlTest( "Copy/Assign: element assign #2.", "value", elementAssign.Attribute( "name" ) );
		XmlTest( "Copy/Assign: element assign #3.", true, ( 0 == elementAssign.Attribute( "foo" )) );

		jcommon::XmlComment comment;
		comment.Parse( "<!--comment-->", 0, TIXML_ENCODING_UNKNOWN );
		jcommon::XmlComment commentCopy( comment );
		jcommon::XmlComment commentAssign;
		commentAssign = commentCopy;
		XmlTest( "Copy/Assign: comment copy.", "comment", commentCopy.Value() );
		XmlTest( "Copy/Assign: comment assign.", "comment", commentAssign.Value() );

		jcommon::XmlUnknown unknown;
		unknown.Parse( "<[unknown]>", 0, TIXML_ENCODING_UNKNOWN );
		jcommon::XmlUnknown unknownCopy( unknown );
		jcommon::XmlUnknown unknownAssign;
		unknownAssign.Parse( "incorrect", 0, TIXML_ENCODING_UNKNOWN );
		unknownAssign = unknownCopy;
		XmlTest( "Copy/Assign: unknown copy.", "[unknown]", unknownCopy.Value() );
		XmlTest( "Copy/Assign: unknown assign.", "[unknown]", unknownAssign.Value() );
		
		jcommon::XmlText text( "TextNode" );
		jcommon::XmlText textCopy( text );
		jcommon::XmlText textAssign( "incorrect" );
		textAssign = text;
		XmlTest( "Copy/Assign: text copy.", "TextNode", textCopy.Value() );
		XmlTest( "Copy/Assign: text assign.", "TextNode", textAssign.Value() );

		jcommon::XmlDeclaration dec;
		dec.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
		jcommon::XmlDeclaration decCopy( dec );
		jcommon::XmlDeclaration decAssign;
		decAssign = dec;

		XmlTest( "Copy/Assign: declaration copy.", "UTF-8", decCopy.Encoding() );
		XmlTest( "Copy/Assign: text assign.", "UTF-8", decAssign.Encoding() );

		jcommon::XmlDocument doc;
		elementCopy.InsertEndChild( textCopy );
		doc.InsertEndChild( decAssign );
		doc.InsertEndChild( elementCopy );
		doc.InsertEndChild( unknownAssign );

		jcommon::XmlDocument docCopy( doc );
		jcommon::XmlDocument docAssign;
		docAssign = docCopy;

		std::string original, copy, assign;
		original << doc;
		copy << docCopy;
		assign << docAssign;
		XmlTest( "Copy/Assign: document copy.", original.c_str(), copy.c_str(), true );
		XmlTest( "Copy/Assign: document assign.", original.c_str(), assign.c_str(), true );

	}	

	//////////////////////////////////////////////////////
	std::cout << "** Parsing, no Condense Whitespace **" << std::endl;
	jcommon::XmlBase::SetCondenseWhiteSpace( false );
	{
		std::istringstream parse1( "<start>This  is    \ntext</start>" );
		jcommon::XmlElement text1( "text" );
		parse1 >> text1;

		XmlTest ( "Condense white space OFF.", "This  is    \ntext",
					text1.FirstChild()->Value(),
					true );
	}
  jcommon::XmlBase::SetCondenseWhiteSpace( true );

	//////////////////////////////////////////////////////
	// GetText();
	{
		const char* str = "<foo>This is text</foo>";
		jcommon::XmlDocument doc;
		doc.Parse( str );
		const jcommon::XmlElement* element = doc.RootElement();

		XmlTest( "GetText() normal use.", "This is text", element->GetText().c_str() );

		str = "<foo><b>This is text</b></foo>";
		doc.Clear();
		doc.Parse( str );
		element = doc.RootElement();

		XmlTest( "GetText() contained element.", element->GetText().size() == 0, true );

		str = "<foo>This is <b>text</b></foo>";
		doc.Clear();
		jcommon::XmlBase::SetCondenseWhiteSpace( false );
		doc.Parse( str );
		jcommon::XmlBase::SetCondenseWhiteSpace( true );
		element = doc.RootElement();

		XmlTest( "GetText() partial.", "This is ", element->GetText().c_str() );
	}


	//////////////////////////////////////////////////////
	// CDATA
	{
		const char* str =	"<xmlElement>"
								"<![CDATA["
									"I am > the rules!\n"
									"...since I make symbolic puns"
								"]]>"
							"</xmlElement>";
		jcommon::XmlDocument doc;
		doc.Parse( str );
		doc.Print();

		XmlTest( "CDATA parse.", doc.FirstChildElement()->FirstChild()->Value(), 
								 "I am > the rules!\n...since I make symbolic puns",
								 true );

		//cout << doc << '\n';

		doc.Clear();

		std::istringstream parse0( str );
		parse0 >> doc;
		//cout << doc << '\n';

		XmlTest( "CDATA stream.", doc.FirstChildElement()->FirstChild()->Value(), 
								 "I am > the rules!\n...since I make symbolic puns",
								 true );

		jcommon::XmlDocument doc1 = doc;
		//doc.Print();

		XmlTest( "CDATA copy.", doc1.FirstChildElement()->FirstChild()->Value(), 
								 "I am > the rules!\n...since I make symbolic puns",
								 true );
	}
	{
		// [ 1482728 ] Wrong wide char parsing
		char buf[256];
		buf[255] = 0;
		for( int i=0; i<255; ++i ) buf[i] = i>=32 ? i : 32;
		std::string str( "<xmlElement><![CDATA[" );
		str += buf;
		str += "]]></xmlElement>";

		jcommon::XmlDocument doc;
		doc.Parse( str.c_str() );

		jcommon::XmlPrinter printer;
		printer.SetStreamPrinting();
		doc.Accept( &printer );

		XmlTest( "CDATA with all bytes #1.", str.c_str(), printer.Str().c_str(), true );

		doc.Clear();
		std::istringstream iss( printer.Str() );
		iss >> doc;
		std::string out;
		out << doc;
		XmlTest( "CDATA with all bytes #2.", out.c_str(), printer.Str().c_str(), true );
	}
	{
		// [ 1480107 ] Bug-fix for STL-streaming of CDATA that contains tags
		// CDATA streaming had a couple of bugs, that this tests for.
		const char* str =	"<xmlElement>"
								"<![CDATA["
									"<b>I am > the rules!</b>\n"
									"...since I make symbolic puns"
								"]]>"
							"</xmlElement>";
		jcommon::XmlDocument doc;
		doc.Parse( str );
		doc.Print();

		XmlTest( "CDATA parse. [ 1480107 ]", doc.FirstChildElement()->FirstChild()->Value(), 
								 "<b>I am > the rules!</b>\n...since I make symbolic puns",
								 true );


		doc.Clear();

		std::istringstream parse0( str );
		parse0 >> doc;

		XmlTest( "CDATA stream. [ 1480107 ]", doc.FirstChildElement()->FirstChild()->Value(), 
								 "<b>I am > the rules!</b>\n...since I make symbolic puns",
								 true );

		jcommon::XmlDocument doc1 = doc;
		//doc.Print();

		XmlTest( "CDATA copy. [ 1480107 ]", doc1.FirstChildElement()->FirstChild()->Value(), 
								 "<b>I am > the rules!</b>\n...since I make symbolic puns",
								 true );
	}
	
	//////////////////////////////////////////////////////
	// Visit()
	//////////////////////////////////////////////////////
	
	std::cout << "** Fuzzing... **" << std::endl;

	const int FUZZ_ITERATION = 300;

	// The only goal is not to crash on bad input.
	int len = (int) strlen( demoStart );
	for( int i=0; i<FUZZ_ITERATION; ++i ) 
	{
		char* demoCopy = new char[ len+1 ];
		strcpy( demoCopy, demoStart );

		demoCopy[ i%len ] = (char)((i+1)*3);
		demoCopy[ (i*7)%len ] = '>';
		demoCopy[ (i*11)%len ] = '<';

    jcommon::XmlDocument xml;
		xml.Parse( demoCopy );

		delete [] demoCopy;
	}
	
	std::cout << "** Fuzzing Complete. **" << std::endl;
	
	//////////////////////////////////////////////////////
	std::cout << "** Bug regression tests **" << std::endl;

	// InsertBeforeChild and InsertAfterChild causes crash.
	{
		jcommon::XmlElement parent( "Parent" );
		jcommon::XmlElement childText0( "childText0" );
		jcommon::XmlElement childText1( "childText1" );
		jcommon::XmlNode* childNode0 = parent.InsertEndChild( childText0 );
		jcommon::XmlNode* childNode1 = parent.InsertBeforeChild( childNode0, childText1 );

		XmlTest( "Test InsertBeforeChild on empty node.", ( childNode1 == parent.FirstChild() ), true );
	}

	{
		// InsertBeforeChild and InsertAfterChild causes crash.
		jcommon::XmlElement parent( "Parent" );
		jcommon::XmlElement childText0( "childText0" );
		jcommon::XmlElement childText1( "childText1" );
		jcommon::XmlNode* childNode0 = parent.InsertEndChild( childText0 );
		jcommon::XmlNode* childNode1 = parent.InsertAfterChild( childNode0, childText1 );

		XmlTest( "Test InsertAfterChild on empty node. ", ( childNode1 == parent.LastChild() ), true );
	}

	// Reports of missing constructors, irregular string problems.
	{
		// Missing constructor implementation. No test -- just compiles.
		jcommon::XmlText text( "Missing" );

		// Missing implementation:
		jcommon::XmlDocument doc;
		std::string name = "missing";
		doc.LoadFile( name );

		jcommon::XmlText textSTL( name );
	}

	// Long filenames crashing STL version
	{
		jcommon::XmlDocument doc( "midsummerNightsDreamWithAVeryLongFilenameToConfuseTheStringHandlingRoutines.xml" );
		bool loadOkay = doc.LoadFile();
		loadOkay = true;	// get rid of compiler warning.
		// Won't pass on non-dev systems. Just a "no crash" check.
		XmlTest( "Long filename. ", true, loadOkay );
	}

	{
		// Entities not being written correctly.
		// From Lynn Allen

		const char* passages =
			"<?xml version=\"1.0\" standalone=\"no\" ?>"
			"<passages count=\"006\" formatversion=\"20020620\">"
				"<psg context=\"Line 5 has &quot;quotation marks&quot; and &apos;apostrophe marks&apos;."
				" It also has &lt;, &gt;, and &amp;, as well as a fake copyright &#xA9;.\"> </psg>"
			"</passages>";

		jcommon::XmlDocument doc( "passages.xml" );
		doc.Parse( passages );
		jcommon::XmlElement* psg = doc.RootElement()->FirstChildElement();
		const char* context = psg->Attribute( "context" );
		const char* expected = "Line 5 has \"quotation marks\" and 'apostrophe marks'. It also has <, >, and &, as well as a fake copyright \xC2\xA9.";

		XmlTest( "Entity transformation: read. ", expected, context, true );

		FILE* textfile = fopen( "textfile.txt", "w" );
		if ( textfile ) {
			psg->Print( textfile, 0 );
			fclose( textfile );
		}
		textfile = fopen( "textfile.txt", "r" );
		assert( textfile );
		if ( textfile ) {
			char buf[ 1024 ];
			if (fgets( buf, 1024, textfile ) != nullptr) {
				XmlTest( "Entity transformation: write. ",
						 "<psg context=\'Line 5 has &quot;quotation marks&quot; and &apos;apostrophe marks&apos;."
						 " It also has &lt;, &gt;, and &amp;, as well as a fake copyright \xC2\xA9.' />",
						 buf,
						 true );
			}
		}
		fclose( textfile );
	}

	{
		FILE* textfile = fopen( "test5.xml", "w" );
		if ( textfile ) {
			fputs("<?xml version='1.0'?><a.elem xmi.version='2.0'/>", textfile);
			fclose(textfile);

			jcommon::XmlDocument doc;
			doc.LoadFile( "test5.xml" );
			XmlTest( "dot in element attributes and names", doc.Error(), 0);
		}
	}

	{
		FILE* textfile = fopen( "test6.xml", "w" );
		if ( textfile ) {
			fputs("<element><Name>1.1 Start easy ignore fin thickness&#xA;</Name></element>", textfile );
			fclose(textfile);

			jcommon::XmlDocument doc;
			bool result = doc.LoadFile( "test6.xml" );
			XmlTest( "Entity with one digit.", result, true );

			jcommon::XmlText* text = doc.FirstChildElement()->FirstChildElement()->FirstChild()->ToText();
			XmlTest( "Entity with one digit.", text->Value(), "1.1 Start easy ignore fin thickness\n" );
		}
	}

	{
		// DOCTYPE not preserved (950171)
		// 
		const char* doctype =
			"<?xml version=\"1.0\" ?>"
			"<!DOCTYPE PLAY SYSTEM 'play.dtd'>"
			"<!ELEMENT title (#PCDATA)>"
			"<!ELEMENT books (title,authors)>"
			"<element />";

		jcommon::XmlDocument doc;
		doc.Parse( doctype );
		doc.SaveFile( "test7.xml" );
		doc.Clear();
		doc.LoadFile( "test7.xml" );
		
		jcommon::XmlHandle docH( &doc );
		jcommon::XmlUnknown* unknown = docH.Child( 1 ).Unknown();
		XmlTest( "Correct value of unknown.", "!DOCTYPE PLAY SYSTEM 'play.dtd'", unknown->Value() );
		jcommon::XmlNode* node = docH.Child( 2 ).Node();
		std::string str;
		str << (*node);
		XmlTest( "Correct streaming of unknown.", "<!ELEMENT title (#PCDATA)>", str.c_str() );
	}

	{
		// [ 791411 ] Formatting bug
		// Comments do not stream out correctly.
		const char* doctype =  "<!-- Somewhat<evil> -->";
		jcommon::XmlDocument doc;
		doc.Parse( doctype );

		jcommon::XmlHandle docH( &doc );
		jcommon::XmlComment* comment = docH.Child( 0 ).Node()->ToComment();

		XmlTest( "Comment formatting.", " Somewhat<evil> ", comment->Value() );
		std::string str;
		str << (*comment);
		XmlTest( "Comment streaming.", "<!-- Somewhat<evil> -->", str.c_str() );
	}

	{
		// [ 870502 ] White space issues
		jcommon::XmlDocument doc;
		jcommon::XmlText* text;
		jcommon::XmlHandle docH( &doc );
	
		const char* doctype0 = "<element> This has leading and trailing space </element>";
		const char* doctype1 = "<element>This has  internal space</element>";
		const char* doctype2 = "<element> This has leading, trailing, and  internal space </element>";

		jcommon::XmlBase::SetCondenseWhiteSpace( false );
		doc.Clear();
		doc.Parse( doctype0 );
		text = docH.FirstChildElement( "element" ).Child( 0 ).Text();
		XmlTest( "White space kept.", " This has leading and trailing space ", text->Value() );

		doc.Clear();
		doc.Parse( doctype1 );
		text = docH.FirstChildElement( "element" ).Child( 0 ).Text();
		XmlTest( "White space kept.", "This has  internal space", text->Value() );

		doc.Clear();
		doc.Parse( doctype2 );
		text = docH.FirstChildElement( "element" ).Child( 0 ).Text();
		XmlTest( "White space kept.", " This has leading, trailing, and  internal space ", text->Value() );

		jcommon::XmlBase::SetCondenseWhiteSpace( true );
		doc.Clear();
		doc.Parse( doctype0 );
		text = docH.FirstChildElement( "element" ).Child( 0 ).Text();
		XmlTest( "White space condensed.", "This has leading and trailing space", text->Value() );

		doc.Clear();
		doc.Parse( doctype1 );
		text = docH.FirstChildElement( "element" ).Child( 0 ).Text();
		XmlTest( "White space condensed.", "This has internal space", text->Value() );

		doc.Clear();
		doc.Parse( doctype2 );
		text = docH.FirstChildElement( "element" ).Child( 0 ).Text();
		XmlTest( "White space condensed.", "This has leading, trailing, and internal space", text->Value() );
	}

	{
		// Double attributes
		const char* doctype = "<element attr='red' attr='blue' />";

		jcommon::XmlDocument doc;
		doc.Parse( doctype );
		
		XmlTest( "Parsing repeated attributes.", 0, (int)doc.Error() );	// not an  error to tinyxml
		XmlTest( "Parsing repeated attributes.", "blue", doc.FirstChildElement( "element" )->Attribute( "attr" ) );
	}

	{
		// Embedded null in stream.
		const char* doctype = "<element att\0r='red' attr='blue' />";

		jcommon::XmlDocument doc;
		doc.Parse( doctype );
		XmlTest( "Embedded null throws error.", true, doc.Error() );

		std::istringstream strm( doctype );
		doc.Clear();
		doc.ClearError();
		strm >> doc;
		XmlTest( "Embedded null throws error.", true, doc.Error() );
	}

	{
		// Legacy mode test. (This test may only pass on a western system)
		const char* str =
			"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
			"<ä>"
			"CöntäntßäöüÄÖÜ"
			"</ä>";

		jcommon::XmlDocument doc;
		doc.Parse( str );

		jcommon::XmlHandle docHandle( &doc );
		jcommon::XmlHandle aHandle = docHandle.FirstChildElement( "ä" );
		jcommon::XmlHandle tHandle = aHandle.Child( 0 );
		assert( aHandle.Element() );
		assert( tHandle.Text() );
		XmlTest( "ISO-8859-1 Parsing.", "CöntäntßäöüÄÖÜ", tHandle.Text()->Value() );
	}

	{
		// Empty documents should return TIXML_ERROR_PARSING_EMPTY, bug 1070717
		const char* str = "    ";
		jcommon::XmlDocument doc;
		doc.Parse( str );
		XmlTest( "Empty document error TIXML_ERROR_DOCUMENT_EMPTY", jcommon::XmlBase::TIXML_ERROR_DOCUMENT_EMPTY, doc.ErrorId() );
	}

	{
		// Bug [ 1195696 ] from marlonism
		jcommon::XmlBase::SetCondenseWhiteSpace(false); 
		jcommon::XmlDocument xml; 
		xml.Parse("<text><break/>This hangs</text>"); 
		XmlTest( "Test safe error return.", xml.Error(), false );
	}

	{
		// Bug [ 1243992 ] - another infinite loop
		jcommon::XmlDocument doc;
		doc.SetCondenseWhiteSpace(false);
		doc.Parse("<p><pb></pb>test</p>");
	} 
	
	{
		// Low entities
		jcommon::XmlDocument xml;
		xml.Parse( "<test>&#x0e;</test>" );
		const char result[] = { 0x0e, 0 };
		XmlTest( "Low entities.", xml.FirstChildElement()->GetText().c_str(), result );
		xml.Print();
	}

	{
		// Bug [ 1451649 ] Attribute values with trailing quotes not handled correctly
		jcommon::XmlDocument xml;
		xml.Parse( "<foo attribute=bar\" />" );
		XmlTest( "Throw error with bad end quotes.", xml.Error(), true );
	}
	{
		// Bug [ 1449463 ] Consider generic query
		jcommon::XmlDocument xml;
		xml.Parse( "<foo bar='3' />" );
    jcommon::XmlElement* ele = xml.FirstChildElement();
		double d;
		int i;
		float f;
		bool b;

		XmlTest( "QueryValueAttribute", ele->QueryValueAttribute( "bar", &d ), TIXML_SUCCESS );
		XmlTest( "QueryValueAttribute", ele->QueryValueAttribute( "bar", &i ), TIXML_SUCCESS );
		XmlTest( "QueryValueAttribute", ele->QueryValueAttribute( "bar", &f ), TIXML_SUCCESS );
		XmlTest( "QueryValueAttribute", ele->QueryValueAttribute( "bar", &b ), TIXML_WRONG_TYPE );
		XmlTest( "QueryValueAttribute", ele->QueryValueAttribute( "nobar", &b ), TIXML_NO_ATTRIBUTE );

		XmlTest( "QueryValueAttribute", (d==3.0), true );
		XmlTest( "QueryValueAttribute", (i==3), true );
		XmlTest( "QueryValueAttribute", (f==3.0f), true );
	}

	{
		// [ 1505267 ] redundant malloc in XmlElement::Attribute
		jcommon::XmlDocument xml;
		xml.Parse( "<foo bar='3' />" );
		jcommon::XmlElement* ele = xml.FirstChildElement();
		double d;
		int i;

		std::string bar = "bar";

		const std::string atrrib = ele->Attribute( bar );
		ele->Attribute( bar, &d );
		ele->Attribute( bar, &i );

		XmlTest( "Attribute", atrrib.empty(), false );
		XmlTest( "Attribute", (d==3.0), true );
		XmlTest( "Attribute", (i==3), true );
	}

	{
		// [ 1356059 ] Allow XMLDocument to only be at the top level
		jcommon::XmlDocument xml, xml2;
		xml.InsertEndChild( xml2 );
		XmlTest( "Document only at top level.", xml.Error(), true );
		XmlTest( "Document only at top level.", xml.ErrorId(), jcommon::XmlBase::TIXML_ERROR_DOCUMENT_TOP_ONLY );
	}

	/*  1417717 experiment
	{
		XmlDocument xml;
		xml.Parse("<text>Dan & Tracie</text>");
		xml.Print(stdout);
	}
	{
		XmlDocument xml;
		xml.Parse("<text>Dan &foo; Tracie</text>");
		xml.Print(stdout);
	}
	*/

  /* INFO:: create a simple xml
	jcommon::XmlDocument doc;
	jcommon::XmlElement tag("tag"); 

	tag.SetAttribute("alias", "1");
	tag.SetAttribute("guid", "2"); 

	doc.LinkEndChild(&tag);
	doc.SaveFile("teste.xml");
  */

	std::cout << "\nPass " << gPass << ", Fail " << gFail << std::endl;

	return gFail;
}



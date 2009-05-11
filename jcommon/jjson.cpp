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
#include "jjson.h"

#include <sstream>

#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <typeinfo>
#include <math.h>
#include <ctype.h>

namespace jcommon {

JSONIterator :: JSONIterator( const JSONNode * node )
{
	mRoot = node;
	mCurrent = node;
}

JSONIterator :: ~JSONIterator()
{
}

const JSONNode * JSONIterator :: getNext()
{
	if( NULL == mCurrent ) return NULL;

	const JSONNode * retNode = mCurrent;

	// find first left child
	if( JSONNode::eObject == mCurrent->getType() ) {
		JSONObjectNode * object = static_cast<JSONObjectNode*>((JSONNode*)mCurrent);

		mCurrent = object->getValue(0);
	} else if( JSONNode::eArray == mCurrent->getType() ) {
		JSONArrayNode * array = static_cast<JSONArrayNode*>((JSONNode*)mCurrent);

		mCurrent = array->getValue( 0 );
	} else if( JSONNode::ePair == mCurrent->getType() ) {
		JSONPairNode * pair = static_cast<JSONPairNode*>((JSONNode*)mCurrent);
		mCurrent = pair->getValue();
	} else {
		mCurrent = NULL;
	}

	// find next sibling
	if( NULL == mCurrent ) {
		const JSONNode * parent = NULL;
		
		mCurrent = retNode;
		
		if( NULL != mCurrent ) {
			parent = mCurrent->getParent();
		}

		// WARN:: adicionado para resolver um loop infinito
		if (parent == NULL) {
			return NULL;
		}

		for( ; NULL != parent; ) {
			if( JSONNode::eObject == parent->getType() ) {
				JSONObjectNode * object = static_cast<JSONObjectNode*>((JSONNode*)parent);

				int index = -1;
				for( int i = 0; i < object->getCount(); i++ ) {
					if( mCurrent == object->getValue( i ) ) {
						index = i;
						break;
					}
				}

				if( index >= 0 && index < ( object->getCount() - 1 ) ) {
					mCurrent = object->getValue( index + 1 );
				} else {
					mCurrent = NULL;
				}
			} else if( JSONNode::eArray == parent->getType() ) {
				JSONArrayNode * array = static_cast<JSONArrayNode*>((JSONNode*)parent);

				int index = -1;
				for( int i = 0; i < array->getCount(); i++ ) {
					if( mCurrent == array->getValue( i ) ) {
						index = i;
						break;
					}
				}

				if( index >= 0 && index < ( array->getCount() - 1 ) ) {
					mCurrent = array->getValue( index + 1 );
				} else {
					mCurrent = NULL;
				}
			} else if( JSONNode::ePair == parent->getType() ) {
				mCurrent = NULL;
			} else {
				mCurrent = NULL;
				assert( 0 ); // should not occur
			}

			if( NULL == mCurrent ) {
				mCurrent = parent;
				parent = mCurrent->getParent();
				if( NULL == parent ) mCurrent = NULL;
				if( mRoot == mCurrent ) mCurrent = NULL;
			} else {
				break;
			}
		}
	}

	return retNode;
}

JSONNode :: JSONNode( int type )
	: mType( type )
{
	mParent = NULL;
}

JSONNode :: ~JSONNode()
{
}

int JSONNode :: getType() const
{
	return mType;
}

JSONNode * JSONNode :: getParent() const
{
	return mParent;
}

void JSONNode :: setParent( JSONNode * parent )
{
	mParent = parent;
}

int JSONNode :: isLastChild() const
{
	if( NULL == mParent ) return 0;

	if( JSONNode::eObject == mParent->getType() ) {
		return ((JSONObjectNode*)mParent)->isLastChild( this );
	} else if( JSONNode::eArray == mParent->getType() ) {
		return ((JSONArrayNode*)mParent)->isLastChild( this );
	} else if( JSONNode::ePair == mParent->getType() ) {
		JSONObjectNode * object = (JSONObjectNode*)mParent->getParent();
		return object->isLastChild( mParent );
	}

	return 0;
}

JSONStringNode :: JSONStringNode( const char * value )
	: JSONNode( eString )
{
	mValue = strdup( value );
}

JSONStringNode :: ~JSONStringNode()
{
	if( NULL != mValue ) free( mValue );
	mValue = NULL;
}

const char * JSONStringNode :: getValue()
{
	return mValue;
}

JSONDoubleNode :: JSONDoubleNode( double value )
	: JSONNode( eDouble )
{
	mValue = value;
}

JSONDoubleNode :: ~JSONDoubleNode()
{
}

double JSONDoubleNode :: getValue()
{
	return mValue;
}

JSONIntNode :: JSONIntNode( int value )
	: JSONNode( eInt )
{
	mValue = value;
}

JSONIntNode :: ~JSONIntNode()
{
}

int JSONIntNode :: getValue()
{
	return mValue;
}

JSONNullNode :: JSONNullNode()
	: JSONNode( eNull )
{
}

JSONNullNode :: ~JSONNullNode()
{
}

JSONBooleanNode :: JSONBooleanNode( bool value )
	: JSONNode( eBoolean )
{
	mValue = value;
}

JSONBooleanNode :: ~JSONBooleanNode()
{
}

bool JSONBooleanNode :: getValue()
{
	return mValue;
}

JSONCommentNode :: JSONCommentNode( const char * comment )
	: JSONNode( eComment )
{
	mValue = strdup( comment );
}

JSONCommentNode :: ~JSONCommentNode()
{
	free( mValue );
}

const char * JSONCommentNode :: getValue()
{
	return mValue;
}

JSONPairNode :: JSONPairNode()
	: JSONNode( ePair )
{
	mName = NULL;
	mValue = NULL;
}

JSONPairNode :: ~JSONPairNode()
{
	if( NULL != mName ) free( mName );
	mName = NULL;

	if( NULL != mValue ) delete mValue;
	mValue = NULL;
}

void JSONPairNode :: setName( const char * name )
{
	if( name != mName ) {
		if( NULL != mName ) free( mName );
		mName = strdup( name );
	}
}

const char * JSONPairNode :: getName()
{
	return mName;
}

void JSONPairNode :: setValue( JSONNode * value )
{
	value->setParent( this );
	mValue = value;
}

JSONNode * JSONPairNode :: getValue()
{
	return mValue;
}

JSONObjectNode :: JSONObjectNode()
	: JSONNode( eObject )
{
	mValueList = new JSONArrayList();
}

JSONObjectNode :: ~JSONObjectNode()
{
	for( int i = 0; i < mValueList->getCount(); i++ ) {
		delete (JSONNode*)mValueList->getItem(i);
	}
	delete mValueList;
	mValueList = NULL;
}

int JSONObjectNode :: getCount()
{
	return mValueList->getCount();
}

int JSONObjectNode :: addValue( JSONPairNode * value )
{
	value->setParent( this );

	mValueList->append( value );

	return 0;
}

JSONPairNode * JSONObjectNode :: getValue( int index )
{
	return (JSONPairNode*)mValueList->getItem( index );
}

JSONPairNode * JSONObjectNode :: takeValue( int index )
{
	JSONPairNode * ret = (JSONPairNode*)mValueList->takeItem( index );
	if( NULL != ret ) ret->setParent( NULL );

	return ret;
}

int JSONObjectNode :: isLastChild( const JSONNode * value ) const
{
	const void * lastChild = mValueList->getItem( JSONArrayList::LAST_INDEX );

	return NULL == value ? 0 : ( lastChild == value ? 1 : 0 );
}

int JSONObjectNode :: Find( const char * name )
{
	int ret = -1;
	for( int i = 0; i < mValueList->getCount(); i++ ) {
		JSONPairNode * iter = (JSONPairNode*)mValueList->getItem(i);
		if( 0 == strcmp( iter->getName(), name ) ) {
			ret = i;
			break;
		}
	}

	return ret;
}

JSONArrayNode :: JSONArrayNode()
	: JSONNode( eArray )
{
	mValueList = new JSONArrayList();
}

JSONArrayNode :: ~JSONArrayNode()
{
	for( int i = 0; i < mValueList->getCount(); i++ ) {
		delete (JSONNode*)mValueList->getItem(i);
	}
	delete mValueList;
	mValueList = NULL;
}

int JSONArrayNode :: getCount()
{
	return mValueList->getCount();
}

int JSONArrayNode :: addValue( JSONNode * value )
{
	value->setParent( this );
	return mValueList->append( value );
}

JSONNode * JSONArrayNode :: getValue( int index )
{
	return (JSONNode*)mValueList->getItem( index );
}

JSONNode * JSONArrayNode :: takeValue( int index )
{
	JSONNode * ret = (JSONNode*)mValueList->takeItem( index );
	if( NULL != ret ) ret->setParent( NULL );

	return ret;
}

int JSONArrayNode :: isLastChild( const JSONNode * value ) const
{
	const void * lastChild = mValueList->getItem( JSONArrayList::LAST_INDEX );

	return NULL == value ? 0 : ( lastChild == value ? 1 : 0 );
}

JSONDomParser :: JSONDomParser()
{
	mParser = new JSONPullParser();	
	mContainer = new JSONArrayNode();
	mCurrent = mContainer;
}

JSONDomParser :: ~JSONDomParser()
{
	if( NULL != mParser ) delete mParser;
	mParser = NULL;

	if( NULL != mContainer ) delete mContainer;
	mContainer = NULL;
}

int JSONDomParser :: append( const char * source, int len )
{
	int ret = 0;

	for( int pos = 0; pos < len && NULL == mParser->getError(); pos += 64 ) {
		int realLen = ( len - pos ) > 64 ? 64 : ( len - pos );
		ret += mParser->append( source + pos, realLen );
		buildTree();
	}

	return ret;
}

void JSONDomParser :: addNode( JSONNode * node )
{
	if( JSONNode::eObject == mCurrent->getType() ) {
		assert( JSONNode::ePair == node->getType() );
		((JSONObjectNode*)mCurrent)->addValue( (JSONPairNode*)node );
	} else if( JSONNode::ePair == mCurrent->getType() ) {
		((JSONPairNode*)mCurrent)->setValue( node );
		mCurrent = mCurrent->getParent();
	} else {
		assert( JSONNode::eArray == mCurrent->getType() );
		((JSONArrayNode*)mCurrent)->addValue( node );

		if( mCurrent == mContainer ) node->setParent( NULL );
	}
}

void JSONDomParser :: buildTree()
{
	for( JSONPullEvent * event = mParser->getNext();
			NULL != event;
			event = mParser->getNext() ) {

		switch( event->getEventType() ) {
			case JSONPullEvent::eStartObject:
			{
				JSONObjectNode * newObject = new JSONObjectNode();
				addNode( newObject );
				mCurrent = newObject;
				break;
			}
			case JSONPullEvent::eEndObject:
			{
				mCurrent = mCurrent->getParent();
				if( mCurrent && JSONNode::ePair == mCurrent->getType() ) {
					mCurrent = mCurrent->getParent();
					assert( JSONNode::eObject == mCurrent->getType() );
				}
				break;
			}
			case JSONPullEvent::eStartArray:
			{
				JSONArrayNode * newArray = new JSONArrayNode();
				addNode( newArray );
				mCurrent = newArray;
				break;
			}
			case JSONPullEvent::eEndArray:
			{
				mCurrent = mCurrent->getParent();
				if( mCurrent && JSONNode::ePair == mCurrent->getType() ) {
					mCurrent = mCurrent->getParent();
					assert( JSONNode::eObject == mCurrent->getType() );
				}
				break;
			}
			case JSONPullEvent::eName:
			{
				JSONPairNode * newPair = new JSONPairNode();
				newPair->setName( ((JSONTextEvent*)event)->getText() );
				addNode( newPair );
				mCurrent = newPair;
				break;
			}
			case JSONPullEvent::eNull:
			{
				JSONNullNode * newNull = new JSONNullNode();
				addNode( newNull );
				break;
			}
			case JSONPullEvent::eString:
			{
				JSONStringNode * newString = new JSONStringNode( ((JSONTextEvent*)event)->getText() );
				addNode( newString );
				break;
			}
			case JSONPullEvent::eNumber:
			{
				JSONNode * newNode = NULL;

				const char * text = ((JSONTextEvent*)event)->getText();
				if( NULL != strchr( text, '.' ) ) {
					newNode = new JSONDoubleNode( strtod( text, NULL ) );
				} else {
					newNode = new JSONIntNode( atoi( text ) );
				}
				addNode( newNode );
				break;
			}
			case JSONPullEvent::eBoolean:
			{
				const char * text = ((JSONTextEvent*)event)->getText();
				
				JSONBooleanNode * newBoolean = new JSONBooleanNode( 0 == strcasecmp( text, "true" ) );
				addNode( newBoolean );
				break;
			}
			case JSONPullEvent::eComment:
			{
				const char * text = ((JSONTextEvent*)event)->getText();
				
				JSONCommentNode * newComment = new JSONCommentNode( text );
				addNode( newComment );
				break;
			}
			case JSONPullEvent::ePadding:
				// ignore
				break;
		}

		delete event;
	}
}

const char * JSONDomParser :: getError() const
{
	return mParser->getError();
}

const JSONNode * JSONDomParser :: getValue() const
{
	return mContainer->getValue( 0 );
}

JSONDomBuffer :: JSONDomBuffer( const JSONNode * node, int indent )
{
	mBuffer = new JSONStringBuffer();
	dump( node, mBuffer, indent ? 0 : -1 );
}

JSONDomBuffer :: ~JSONDomBuffer()
{
	delete mBuffer;
	mBuffer = NULL;
}

const char * JSONDomBuffer :: getBuffer() const
{
	return mBuffer->getBuffer();
}

int JSONDomBuffer :: getSize() const
{
	return mBuffer->getSize();
}

void JSONDomBuffer :: dump( const JSONNode * node,
		JSONStringBuffer * buffer, int level )
{
	if( JSONNode::eObject == node->getType() ) {
		dumpObject( node, buffer, level );
	} else if( JSONNode::eArray == node->getType() ) {
		dumpArray( node, buffer, level );
	} else {
		char tmpBuffer[ 128 ] = { 0 };

		if( JSONNode::eString == node->getType() ) {
			buffer->append( '"' );
			JSONCodec::encode( ((JSONStringNode*)node)->getValue(), buffer );
			buffer->append( '"' );
		} else if( JSONNode::eDouble == node->getType() ) {
			JSONCodec::encode( ((JSONDoubleNode*)node)->getValue(), buffer );
		} else if( JSONNode::eInt == node->getType() ) {
			snprintf( tmpBuffer, sizeof( tmpBuffer ), "%d", ((JSONIntNode*)node)->getValue() );
			buffer->append( tmpBuffer );
		} else if( JSONNode::eBoolean == node->getType() ) {
			buffer->append( ((JSONBooleanNode*)node)->getValue() ? "true" : "false" );
		} else if( JSONNode::eNull == node->getType() ) {
			buffer->append( "null" );
		} else {
			assert( JSONNode::eComment == node->getType() );

			buffer->append( "//" );
			buffer->append( ((JSONCommentNode*)node)->getValue() );
			buffer->append( "\r\n" );
		}
	}
}

void JSONDomBuffer :: dumpObject( const JSONNode * node,
		JSONStringBuffer * buffer, int level )
{
	JSONObjectNode * objectNode = (JSONObjectNode*)node;

	buffer->append( '{' );

	for( int i = 0; i < objectNode->getCount(); i++ ) {
		if( level >= 0 ) {
			buffer->append( "\n" );
			for( int i = 0; i <= level; i++ ) buffer->append( "\t" );
		}

		JSONPairNode * pairNode = objectNode->getValue( i );
		const char * name = pairNode->getName();
		const JSONNode * iter = pairNode->getValue();

		buffer->append( '"' );

		JSONCodec::encode( name, buffer );

		buffer->append( "\" : " );

		dump( iter, buffer, level >= 0 ? level + 1 : -1 );

		if( ( i + 1 ) != objectNode->getCount()
				&& JSONNode::eComment != iter->getType() ) buffer->append( ',' );
	}

	if( level >= 0 ) {
		buffer->append( "\n" );
		for( int i = 0; i < level; i++ ) buffer->append( "\t" );
	}

	buffer->append( '}' );
}

void JSONDomBuffer :: dumpArray( const JSONNode * node,
		JSONStringBuffer * buffer, int level )
{
	JSONArrayNode * arrayNode = (JSONArrayNode*)node;

	buffer->append( '[' );

	for( int i = 0; i < arrayNode->getCount(); i++ ) {
		if( level >= 0 ) {
			buffer->append( "\n" );
			for( int i = 0; i <= level; i++ ) buffer->append( "\t" );
		}

		const JSONNode * iter = arrayNode->getValue( i );

		dump( iter, buffer, level >= 0 ? level + 1 : -1 );

		if( ( i + 1 ) != arrayNode->getCount()
				&& JSONNode::eComment != iter->getType() ) buffer->append( ',' );
	}

	if( level >= 0 ) {
		buffer->append( "\n" );
		for( int i = 0; i < level; i++ ) buffer->append( "\t" );
	}

	buffer->append( ']' );
}

JSONPullParser :: JSONPullParser()
{
	mEventQueue = new JSONPullEventQueue();
	mReader = new JSONSpaceReader( 1 );
	mStack = new JSONArrayList();
	mNameList = new JSONArrayList();

	mLevel = 0;

	mError = NULL;
	memset( mErrorSegment, 0, sizeof( mErrorSegment ) );
	mErrorIndex = mColIndex = mRowIndex = 0;

	mObjectCount = 0;
}

JSONPullParser :: ~JSONPullParser()
{
	delete mEventQueue;
	mEventQueue = NULL;

	delete mReader;
	mReader = NULL;

	delete mStack;
	mStack = NULL;

	for( int i = 0; i < mNameList->getCount(); i++ ) {
		free( (char*)mNameList->getItem( i ) );
	}
	delete mNameList;
	mNameList = NULL;
}

int JSONPullParser :: append( const char * source, int len )
{
	if( NULL != mError ) return 0;

	int consumed = 0;

	for( int i = 0; i < len && NULL == mError; i++ ) {

		consumed++;

		char c = source[ i ];

		mErrorSegment[ mErrorIndex++ % sizeof( mErrorSegment ) ] = c;
		mReader->read( this, c );
		if( '\n' == c ) {
			mRowIndex++;
			mColIndex = 0;
		} else {
			mColIndex++;
		}
	}

	return consumed;
}

JSONPullEvent * JSONPullParser :: getNext()
{
	JSONPullEvent * event = mEventQueue->dequeue();

	if( NULL != event ) {
		if( JSONPullEvent::eStartObject == event->getEventType() ) mLevel++;
		if( JSONPullEvent::eEndObject == event->getEventType() ) mLevel--;
	}

	return event;
}

const char * JSONPullParser :: getError()
{
	return mError;
}

int JSONPullParser :: getLevel()
{
	return mLevel;
}

void JSONPullParser :: changeReader( JSONReader * reader )
{
	JSONPullEvent * event = mReader->getEvent( this );

	if( NULL != event ) {
		const int eventType = event->getEventType();

		char errmsg[ 256 ] = { 0 };

		if( JSONPullEvent::eStartObject == eventType ) {
			mStack->append( (void*)"o" );

			++mObjectCount;
		} else if( JSONPullEvent::eEndObject == eventType ) {
			if( mObjectCount <= mNameList->getCount() ) {
				snprintf( errmsg, sizeof( errmsg ), "miss value for name \"%s\"",
						(char*)mNameList->getItem( JSONArrayList::LAST_INDEX ) );
			} else {
				char * last = (char*)mStack->takeItem( JSONArrayList::LAST_INDEX );
				if( NULL != last ) {
					if( 'o' != *last ) {
						snprintf( errmsg, sizeof( errmsg ), "mismatched object, start-with<%s>", last );
					}
				} else {
					snprintf( errmsg, sizeof( errmsg ), "mismatched object, start-with<NULL>" );
				}

				--mObjectCount;
				if( isObjectParent() ) free( mNameList->takeItem( JSONArrayList::LAST_INDEX ) );
			}
		} else if( JSONPullEvent::eStartArray == eventType ) {
			mStack->append( (void*)"a" );
		} else if( JSONPullEvent::eEndArray == eventType ) {
			char * last = (char*)mStack->takeItem( JSONArrayList::LAST_INDEX );
			if( NULL != last ) {
				if( 'a' != *last ) {
					snprintf( errmsg, sizeof( errmsg ), "mismatched array, start-with<%s>", last );
				}
			} else {
				snprintf( errmsg, sizeof( errmsg ), "mismatched array, start-with<NULL>" );
			}

			if( isObjectParent() ) free( mNameList->takeItem( JSONArrayList::LAST_INDEX ) );
		} else if( JSONPullEvent::eName == eventType ) {
			mNameList->append( strdup( ((JSONNameEvent*)event)->getText() ) );
		} else if( JSONPullEvent::eString == eventType
				|| JSONPullEvent::eNumber == eventType
				|| JSONPullEvent::eBoolean == eventType
				|| JSONPullEvent::eNull == eventType ) {
			if( isObjectParent() ) free( mNameList->takeItem( JSONArrayList::LAST_INDEX ) );
		}

		if( '\0' == errmsg[0] ) {
			mEventQueue->enqueue( event );
		} else {
			delete event;
			setError( errmsg );
		}
	}

	delete mReader;
	mReader = reader;
}

void JSONPullParser :: setError( const char * error )
{
	if( NULL != error ) {
		if( NULL != mError ) free( mError );

		char segment[ 2 * sizeof( mErrorSegment ) + 1 ];
		{
			memset( segment, 0, sizeof( segment ) );

			char temp[ sizeof( mErrorSegment ) + 1 ];
			memset( temp, 0, sizeof( temp ) );
			if( mErrorIndex < (int)sizeof( mErrorSegment ) ) {
				strncpy( temp, mErrorSegment, mErrorIndex );
			} else {
				int offset = mErrorIndex % sizeof( mErrorSegment );
				strncpy( temp, mErrorSegment + offset, sizeof( mErrorSegment ) - offset );
				strncpy( temp + sizeof( mErrorSegment ) - offset, mErrorSegment, offset );
			}

			for( char * pos = temp, * dest = segment; '\0' != *pos; pos++ ) {
				if( '\r' == *pos ) {
					*dest++ = '\\';
					*dest++ = 'r';
				} else if( '\n' == *pos ) {
					*dest++ = '\\';
					*dest++ = 'n';
				} else if( '\t' == *pos ) {
					*dest++ = '\\';
					*dest++ = 't';
				} else {
					*dest++ = *pos;
				}
			}
		}

		char msg[ 512 ];
		snprintf( msg, sizeof( msg), "%s ( occured at row(%d), col(%d) : %s )",
				error, mRowIndex + 1, mColIndex + 1, segment );

		mError = strdup( msg );
	}
}

int JSONPullParser :: isWait4Name()
{
	int ret = 0;

	if( isObjectParent() && mObjectCount > mNameList->getCount() ) ret = 1;

	return ret;
}

int JSONPullParser :: isObjectParent()
{
	int ret = 0;
	if( mStack->getCount() > 0 ) {
		char * last = (char*)mStack->getItem( JSONArrayList::LAST_INDEX );
		if( 'o' == *last ) ret = 1;		
	}

	return ret;
}

const char JSONCodec :: ESCAPE_CHARS [] =
		{ '"', '\\', '\b', '\f', '\n', '\r', '\t' };
const char JSONCodec :: UNESCAPED_CHARS [] =
		{ '"', '\\', 'b',  'f',  'n',  'r',  't' };

int JSONCodec :: decode( const char * encodeValue,
		JSONStringBuffer * outBuffer )
{
	int isEscape = 0;

	const char * pos = encodeValue;
	for( ; '\0' != *pos; pos++ ) {
		if( '\\' == *pos && 0 == isEscape ) {
			isEscape = 1;
		} else {
			if( 1 == isEscape ) {
				isEscape = 0;
				int index = -1;
				for( int i = 0; i < (int)sizeof( UNESCAPED_CHARS ); i++ ) {
					if( UNESCAPED_CHARS[i] == *pos ) {
						index = i;
						break;
					}
				}
				if( index >= 0 ) {
					outBuffer->append( ESCAPE_CHARS[ index ] );
				} else {
					// unknown escape, keep it not change
					outBuffer->append( '\\' );
					outBuffer->append( *pos );
				}
			} else {
				outBuffer->append( *pos );
			}
		}
	}

	return 0;
}

int JSONCodec :: encode( const char * decodeValue,
		JSONStringBuffer * outBuffer )
{
	const char * pos = decodeValue;
	for( ; '\0' != *pos; pos++ ) {
		int index = -1;
		for( int i = 0; i < (int)sizeof( ESCAPE_CHARS ); i++ ) {
			if( ESCAPE_CHARS[i] == *pos ) {
				index = i;
				break;
			}
		}
		if( index >= 0 ) {
			outBuffer->append( '\\' );
			outBuffer->append( UNESCAPED_CHARS[index] );
		} else {
			outBuffer->append( *pos );
		}	
	}

	return 0;
}

int JSONCodec :: encode( double value, JSONStringBuffer * outBuffer )
{
	char buffer[ 128 ] = { 0 };

	if( ((double)ceil( value )) == value ) {
		snprintf( buffer, sizeof( 128 ), "%.0f", value );
	} else {
		snprintf( buffer, sizeof( 128 ), "%f", value );
	}

	return outBuffer->append( buffer );
}

JSONReader :: JSONReader()
{
	mBuffer = new JSONStringBuffer();
}

JSONReader :: ~JSONReader()
{
	delete mBuffer;
}

void JSONReader :: changeReader( JSONPullParser * parser, JSONReader * reader )
{
	//printf( "\nchange: %s\n", typeid( *reader ).name() );
	parser->changeReader( reader );
}

void JSONReader :: setError( JSONPullParser * parser, const char * error )
{
	parser->setError( error );
}

int JSONReader :: isWait4Name( JSONPullParser * parser )
{
	return parser->isWait4Name();
}

int JSONReader :: isObjectParent( JSONPullParser * parser )
{
	return parser->isObjectParent();
}

JSONSpaceReader :: JSONSpaceReader( int hasReadComma )
{
	mHasReadComma = hasReadComma;
}

JSONSpaceReader :: ~JSONSpaceReader()
{
}

void JSONSpaceReader :: read( JSONPullParser * parser, char c )
{
	if( isspace( c ) ) {
		//skip
	} else if( ',' == c && 0 == mHasReadComma ) {
		mHasReadComma = 1;
	} else if( '/' == c ) {
		JSONReader * reader = new JSONCommentReader(
				new JSONSpaceReader( mHasReadComma ) );
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '}' == c ) {
		JSONReader * reader = new JSONEndObjectReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( ']' == c ) {
		JSONReader * reader = new JSONEndArrayReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		if( 0 == mHasReadComma ) {
			setError( parser, "miss ',' between values" );
		} else if( isWait4Name( parser ) ) {
			JSONReader * reader = new JSONNameReader();
			changeReader( parser, reader );
			reader->read( parser, c );
		} else {
			if( isObjectParent( parser ) ) {
				if( ':' == c ) {
					changeReader( parser, new JSONValueReader() );
				} else {
					setError( parser, "miss ':' between name & value" );
				}
			} else {
				if( '{' == c ) {
					JSONReader * reader = new JSONStartObjectReader();
					changeReader( parser, reader );
					reader->read( parser, c );
				} else if( '[' == c ) {
					JSONReader * reader = new JSONStartArrayReader();
					changeReader( parser, reader );	
					reader->read( parser, c );
				} else {
					JSONReader * reader = new JSONValueReader();
					changeReader( parser, reader );
					reader->read( parser, c );
				}
			}
		}
	}
}

JSONPullEvent * JSONSpaceReader :: getEvent( JSONPullParser * parser )
{
	return NULL;
}

JSONStartObjectReader :: JSONStartObjectReader( int hasReadLeftBrace )
{
	mHasReadLeftBrace = 0 == hasReadLeftBrace ? 0 : 2;
}

JSONStartObjectReader :: ~JSONStartObjectReader()
{
}

void JSONStartObjectReader :: read( JSONPullParser * parser, char c )
{
	if( isspace( c ) ) {
		//skip
	} else if( 0 == mHasReadLeftBrace && '{' == c ) {
		mHasReadLeftBrace = 1;
	} else if( '}' == c ) {
		JSONReader * reader = new JSONEndObjectReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '"' == c ) {
		JSONReader * reader = new JSONNameReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '/' == c ) {
		JSONReader * reader = new JSONCommentReader(
				new JSONStartObjectReader( mHasReadLeftBrace ) );
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		setError( parser, "unknown name" );
	}
}

JSONPullEvent * JSONStartObjectReader :: getEvent( JSONPullParser * parser )
{
	return new JSONStartObjectEvent();
}

JSONEndObjectReader :: JSONEndObjectReader()
{
}

JSONEndObjectReader :: ~JSONEndObjectReader()
{
}

void JSONEndObjectReader :: read( JSONPullParser * parser, char c )
{
	if( '}' == c ) {
		changeReader( parser, new JSONSpaceReader() );
	} else {
		setError( parser, "unknown object end" );
	}
}

JSONPullEvent * JSONEndObjectReader :: getEvent( JSONPullParser * parser )
{
	return new JSONEndObjectEvent();
}

JSONStartArrayReader :: JSONStartArrayReader()
{
	mHasReadLeftBracket = 0;
}

JSONStartArrayReader :: ~JSONStartArrayReader()
{
}

void JSONStartArrayReader :: read( JSONPullParser * parser, char c )
{
	if( 0 == mHasReadLeftBracket && '[' == c ) {
		mHasReadLeftBracket = 1;
	} else if( ']' == c ) {
		JSONReader * reader = new JSONEndArrayReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		JSONReader * reader = new JSONSpaceReader( 1 );
		changeReader( parser, reader );
		reader->read( parser, c );
	}
}

JSONPullEvent * JSONStartArrayReader :: getEvent( JSONPullParser * parser )
{
	return new JSONStartArrayEvent();
}

JSONEndArrayReader :: JSONEndArrayReader()
{
}

JSONEndArrayReader :: ~JSONEndArrayReader()
{
}

void JSONEndArrayReader :: read( JSONPullParser * parser, char c )
{
	if( ']' == c ) {
		changeReader( parser, new JSONSpaceReader() );
	} else {
		setError( parser, "unknown array end" );
	}
}

JSONPullEvent * JSONEndArrayReader :: getEvent( JSONPullParser * parser )
{
	return new JSONEndArrayEvent();
}

JSONNameReader :: JSONNameReader()
{
	mLastChar = '\0';
	mHasReadStartQuot = 0;
}

JSONNameReader :: ~JSONNameReader()
{
}

void JSONNameReader :: read( JSONPullParser * parser, char c )
{
	if( 0 == mHasReadStartQuot ) {
		if( isspace( c ) ) {
			//skip
		} else if( '"' == c ) {
			mHasReadStartQuot = 1;
		} else {
			setError( parser, "unknown name" );
		}
	} else {
		if( '"' == c && '\\' != mLastChar ) {
			changeReader( parser, new JSONSpaceReader( 1 ) );
		} else {
			mLastChar = c;
			mBuffer->append( c );
		}
	}
}

JSONPullEvent * JSONNameReader :: getEvent( JSONPullParser * parser )
{
	JSONStringBuffer outBuffer;

	JSONCodec::decode( mBuffer->getBuffer(), &outBuffer );

	JSONNameEvent * event = new JSONNameEvent();
	event->setText( outBuffer.getBuffer(), outBuffer.getSize() );

	return event;
}

JSONValueReader :: JSONValueReader()
{
}

JSONValueReader :: ~JSONValueReader()
{
}

void JSONValueReader :: read( JSONPullParser * parser, char c )
{
	if( isspace( c ) ) {
		//skip
	} else if( '"' == c ) {
		JSONReader * reader = new JSONStringReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '-' == c || isdigit( c ) ) {
		JSONReader * reader = new JSONNumberReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( 't' == c || 'T' == c || 'f' == c || 'F' == c ) {
		JSONReader * reader = new JSONBooleanReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( 'n' == c || 'N' == c ) {
		JSONReader * reader = new JSONNullReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '/' == c ) {
		JSONReader * reader = new JSONCommentReader(
				new JSONValueReader() );
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '{' == c ) {
		JSONReader * reader = new JSONStartObjectReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else if( '[' == c ) {
		JSONReader * reader = new JSONStartArrayReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		setError( parser, "unknown value" );
	}
}

JSONPullEvent * JSONValueReader :: getEvent( JSONPullParser * parser )
{
	return NULL;
}

JSONStringReader :: JSONStringReader()
{
	mLastChar = '\0';
	mHasReadStartQuot = 0;
}

JSONStringReader :: ~JSONStringReader()
{
}

void JSONStringReader :: read( JSONPullParser * parser, char c )
{
	if( 0 == mHasReadStartQuot ) {
		if( isspace( c ) ) {
			//skip
		} else if( '"' == c ) {
			mHasReadStartQuot = 1;
		} else {
			setError( parser, "unknown string" );
		}
	} else {
		if( '"' == c && '\\' != mLastChar ) {
			changeReader( parser, new JSONSpaceReader() );
		} else {
			mLastChar = c;
			mBuffer->append( c );
		}
	}
}

JSONPullEvent * JSONStringReader :: getEvent( JSONPullParser * parser )
{
	JSONStringBuffer outBuffer;

	JSONCodec::decode( mBuffer->getBuffer(), &outBuffer );

	JSONStringEvent * event = new JSONStringEvent();
	event->setText( outBuffer.getBuffer(), outBuffer.getSize() );

	return event;
}

JSONNumberReader :: JSONNumberReader()
{
}

JSONNumberReader :: ~JSONNumberReader()
{
}

void JSONNumberReader :: read( JSONPullParser * parser, char c )
{
	if( isdigit( c ) || '-' == c || '+' == c
			|| 'e' == c || 'E' == c || '.' == c ) {
		mBuffer->append( c );
	} else {
		JSONReader * reader = new JSONSpaceReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	}
}

JSONPullEvent * JSONNumberReader :: getEvent( JSONPullParser * parser )
{
	JSONNumberEvent * event = new JSONNumberEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

JSONBooleanReader :: JSONBooleanReader()
{
}

JSONBooleanReader :: ~JSONBooleanReader()
{
}

void JSONBooleanReader :: read( JSONPullParser * parser, char c )
{
	if( !isalpha( c ) ) {
		JSONReader * reader = new JSONSpaceReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		mBuffer->append( c );
	}
}

JSONPullEvent * JSONBooleanReader :: getEvent( JSONPullParser * parser )
{
	JSONBooleanEvent * event = new JSONBooleanEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

JSONNullReader :: JSONNullReader()
{
}

JSONNullReader :: ~JSONNullReader()
{
}

void JSONNullReader :: read( JSONPullParser * parser, char c )
{
	if( !isalpha( c ) ) {
		JSONReader * reader = new JSONSpaceReader();
		changeReader( parser, reader );
		reader->read( parser, c );
	} else {
		mBuffer->append( c );
	}
}

JSONPullEvent * JSONNullReader :: getEvent( JSONPullParser * parser )
{
	JSONNullEvent * event = new JSONNullEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

JSONCommentReader :: JSONCommentReader( JSONReader * savedReader )
{
	mHasReadStartSolidus = 0;
	mSavedReader = savedReader;
}

JSONCommentReader :: ~JSONCommentReader()
{
}

void JSONCommentReader :: read( JSONPullParser * parser, char c )
{
	if( mHasReadStartSolidus < 2 ) {
		if( '/' == c ) {
			mHasReadStartSolidus++;
		} else {
			setError( parser, "unknown comment" );
		}
	} else if( '\n' == c ) {
		changeReader( parser, mSavedReader );
	} else {
		if( '\r' != c ) mBuffer->append( c );
	}
}

JSONPullEvent * JSONCommentReader :: getEvent( JSONPullParser * parser )
{
	JSONCommentEvent * event = new JSONCommentEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

JSONPaddingReader :: JSONPaddingReader()
{
}

JSONPaddingReader :: ~JSONPaddingReader()
{
}

void JSONPaddingReader :: read( JSONPullParser * parser, char c )
{
	if( !isspace( c ) ) {
		setError( parser, "unexpected padding" );
	}
}

JSONPullEvent * JSONPaddingReader :: getEvent( JSONPullParser * parser )
{
	JSONPaddingEvent * event = new JSONPaddingEvent();
	event->setText( mBuffer->getBuffer(), mBuffer->getSize() );

	return event;
}

const int JSONArrayList::LAST_INDEX = -1;

JSONArrayList :: JSONArrayList( int initCount )
{
	mMaxCount = initCount <= 0 ? 2 : initCount;
	mCount = 0;
	mFirst = (void**)malloc( sizeof( void * ) * mMaxCount );
}

JSONArrayList :: ~JSONArrayList()
{
	free( mFirst );
	mFirst = NULL;
}

int JSONArrayList :: getCount() const
{
	return mCount;
}

int JSONArrayList :: append( void * value )
{
	if( NULL == value ) return -1;

	if( mCount >= mMaxCount ) {
		mMaxCount = ( mMaxCount * 3 ) / 2 + 1;
		mFirst = (void**)realloc( mFirst, sizeof( void * ) * mMaxCount );
		assert( NULL != mFirst );
		memset( mFirst + mCount, 0, ( mMaxCount - mCount ) * sizeof( void * ) );
	}

	mFirst[ mCount++ ] = value;

	return 0;
}

void * JSONArrayList :: takeItem( int index )
{
	void * ret = NULL;

	if( LAST_INDEX == index ) index = mCount -1;
	if( index < 0 || index >= mCount ) return ret;

	ret = mFirst[ index ];

	mCount--;

	if( ( index + 1 ) < mMaxCount ) {
		memmove( mFirst + index, mFirst + index + 1,
			( mMaxCount - index - 1 ) * sizeof( void * ) );
	} else {
		mFirst[ index ] = NULL;
	}

	return ret;
}

const void * JSONArrayList :: getItem( int index ) const
{
	const void * ret = NULL;

	if( LAST_INDEX == index ) index = mCount - 1;
	if( index < 0 || index >= mCount ) return ret;

	ret = mFirst[ index ];

	return ret;
}

void JSONArrayList :: sort( int ( * cmpFunc )( const void *, const void * ) )
{
	for( int i = 0; i < mCount - 1; i++ ) {
		int min = i;
		for( int j = i + 1; j < mCount; j++ ) {
			if( cmpFunc( mFirst[ min ], mFirst[ j ] ) > 0 ) {
				min = j;
			}
		}

		if( min != i ) {
			void * temp = mFirst[ i ];
			mFirst[ i ] = mFirst[ min ];
			mFirst[ min ] = temp;
		}
	}
}

JSONQueue :: JSONQueue()
{
	mMaxCount = 8;
	mEntries = (void**)malloc( sizeof( void * ) * mMaxCount );

	mHead = mTail = mCount = 0;
}

JSONQueue :: ~JSONQueue()
{
	free( mEntries );
	mEntries = NULL;
}

void JSONQueue :: push( void * item )
{
	if( mCount >= mMaxCount ) {
		mMaxCount = ( mMaxCount * 3 ) / 2 + 1;
		void ** newEntries = (void**)malloc( sizeof( void * ) * mMaxCount );

		unsigned int headLen = 0, tailLen = 0;
		if( mHead < mTail ) {
			headLen = mTail - mHead;
		} else {
			headLen = mCount - mTail;
			tailLen = mTail;
		}

		memcpy( newEntries, &( mEntries[ mHead ] ), sizeof( void * ) * headLen );
		if( tailLen ) {
			memcpy( &( newEntries[ headLen ] ), mEntries, sizeof( void * ) * tailLen );
		}

		mHead = 0;
		mTail = headLen + tailLen;

		free( mEntries );
		mEntries = newEntries;
	}

	mEntries[ mTail++ ] = item;
	mTail = mTail % mMaxCount;
	mCount++;
}

void * JSONQueue :: pop()
{
	void * ret = NULL;

	if( mCount > 0 ) {
		ret = mEntries[ mHead++ ];
		mHead = mHead % mMaxCount;
		mCount--;
	}

	return ret;
}

void * JSONQueue :: top()
{
	return mCount > 0 ? mEntries[ mHead ] : NULL;
}

JSONStringBuffer :: JSONStringBuffer()
{
	init();
}

void JSONStringBuffer :: init()
{
	mSize = 0;
	mMaxSize = 8;
	mBuffer = (char*)malloc( mMaxSize );
	memset( mBuffer, 0, mMaxSize );
}

JSONStringBuffer :: ~JSONStringBuffer()
{
	free( mBuffer );
}

int JSONStringBuffer :: append( char c )
{
	if( mSize >= ( mMaxSize - 1 ) ) {
		mMaxSize += ( mMaxSize * 3 ) / 2 + 1;
		mBuffer = (char*)realloc( mBuffer, mMaxSize );
		assert( NULL != mBuffer );
		memset( mBuffer + mSize, 0, mMaxSize - mSize );
	}
	mBuffer[ mSize++ ] = c;

	return 0;
}

int JSONStringBuffer :: append( const char * value, int size )
{
	if( NULL == value ) return -1;

	size = ( size <= 0 ? strlen( value ) : size );
	if( size <= 0 ) return -1;

	if( ( size + mSize ) > ( mMaxSize - 1 ) ) {
		mMaxSize += size;
		mBuffer = (char*)realloc( mBuffer, mMaxSize );
		assert( NULL != mBuffer );
		memset( mBuffer + mSize, 0, mMaxSize - mSize );
	}

	memcpy( mBuffer + mSize, value, size );
	mSize += size;

	return 0;
}

int JSONStringBuffer :: getSize() const
{
	return mSize;
}

const char * JSONStringBuffer :: getBuffer() const
{
	return mBuffer;
}

char * JSONStringBuffer :: takeBuffer()
{
	char * ret = mBuffer;

	mBuffer = NULL;
	init();

	return ret;
}

void JSONStringBuffer :: clean()
{
	memset( mBuffer, 0, mMaxSize );
	mSize = 0;
}

JSONPullEvent :: JSONPullEvent( int eventType )
	: mEventType( eventType )
{
}

JSONPullEvent :: ~JSONPullEvent()
{
}

int JSONPullEvent :: getEventType()
{
	return mEventType;
}

JSONPullEventQueue :: JSONPullEventQueue()
{
	mQueue = new JSONQueue();
}

JSONPullEventQueue :: ~JSONPullEventQueue()
{
	for( ; NULL != mQueue->top(); ) {
		JSONPullEvent * event = (JSONPullEvent*)mQueue->pop();
		delete event;
	}

	delete mQueue;
}

void JSONPullEventQueue :: enqueue( JSONPullEvent * event )
{
	mQueue->push( event );
}

JSONPullEvent * JSONPullEventQueue :: dequeue()
{
	return (JSONPullEvent*)mQueue->pop();
}

JSONStartObjectEvent :: JSONStartObjectEvent()
	: JSONPullEvent( eStartObject )
{
}

JSONStartObjectEvent :: ~JSONStartObjectEvent()
{
}

JSONEndObjectEvent :: JSONEndObjectEvent()
	: JSONPullEvent( eEndObject )
{
}

JSONEndObjectEvent :: ~JSONEndObjectEvent()
{
}

JSONStartArrayEvent :: JSONStartArrayEvent()
	: JSONPullEvent( eStartArray )
{
}

JSONStartArrayEvent :: ~JSONStartArrayEvent()
{
}

JSONEndArrayEvent :: JSONEndArrayEvent()
	: JSONPullEvent( eEndArray )
{
}

JSONEndArrayEvent :: ~JSONEndArrayEvent()
{
}

JSONTextEvent :: JSONTextEvent( int eventType )
	: JSONPullEvent( eventType )
{
	mText = NULL;
}

JSONTextEvent :: ~JSONTextEvent()
{
	if( NULL != mText ) free( mText );
	mText = NULL;
}

void JSONTextEvent :: setText( const char * text, int len )
{
	if( NULL != text ) {
		if( NULL != mText ) free( mText );
		mText = (char*)malloc( len + 1 );
		memcpy( mText, text, len );
		mText[ len ] = '\0';
	}
}

const char * JSONTextEvent :: getText() const
{
	return mText;
}

JSONNullEvent :: JSONNullEvent()
	: JSONTextEvent( eNull )
{
}

JSONNullEvent :: ~JSONNullEvent()
{
}

JSONNameEvent :: JSONNameEvent()
	: JSONTextEvent( eName )
{
}

JSONNameEvent :: ~JSONNameEvent()
{
}

JSONStringEvent :: JSONStringEvent()
	: JSONTextEvent( eString )
{
}

JSONStringEvent :: ~JSONStringEvent()
{
}

JSONNumberEvent :: JSONNumberEvent()
	: JSONTextEvent( eNumber )
{
}

JSONNumberEvent :: ~JSONNumberEvent()
{
}

JSONBooleanEvent :: JSONBooleanEvent()
	: JSONTextEvent( eBoolean )
{
}

JSONBooleanEvent :: ~JSONBooleanEvent()
{
}

JSONCommentEvent :: JSONCommentEvent()
	: JSONTextEvent( eComment )
{
}

JSONCommentEvent :: ~JSONCommentEvent()
{
}

JSONPaddingEvent :: JSONPaddingEvent()
	: JSONTextEvent( ePadding )
{
}

JSONPaddingEvent :: ~JSONPaddingEvent()
{
}

}


/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "jjson.h"

using namespace jcommon;

static void postPrint( const JSONNode * node )
{
	if( node->isLastChild() ) {
		const JSONNode * parent = node->getParent();
		if( JSONNode::ePair == parent->getType() ) {
			parent = parent->getParent();
		}

		if( JSONNode::eObject == parent->getType() ) {
			printf( "}" );
		} else {
			assert( JSONNode::eArray == parent->getType() );
			printf( "]" );
		}

		postPrint( parent );
	} else {
		if( NULL != node->getParent() ) printf( "," );
	}
}

int main( int argc, char * argv[] )
{
	char * filename = NULL;

	if( argc < 2 ) {
		printf( "Usage: %s <json_file>\n", argv[0] );
		exit( -1 );
	} else {
		filename = argv[1];
	}

	FILE * fp = fopen ( filename, "r" );
	if( NULL == fp ) {
		printf( "cannot not open %s\n", filename );
		exit( -1 );
	}

	struct stat aStat;
	char * source = NULL;
	stat( filename, &aStat );
	source = ( char * ) malloc ( aStat.st_size + 1 );
	fread ( source, aStat.st_size, sizeof ( char ), fp );
	fclose ( fp );
	source[ aStat.st_size ] = '\0';

	JSONDomParser parser;
	parser.append( source, strlen( source ) );

	if( NULL != parser.getError() ) {
		printf( "\n\nerror: %s\n", parser.getError() );
	}

	free( source );

	printf( "Test DomBuffer\n" );

	JSONDomBuffer buffer( parser.getValue(), 1 );

	printf( "%s\n", buffer.getBuffer() );

	printf( "\n" );

	printf( "Test Iterator\n" );

	JSONIterator iterator( parser.getValue() );

	for( ; ; ) {
		const JSONNode * node = iterator.getNext();
		if( NULL == node ) break;

		switch( node->getType() ) {
			case JSONNode::eObject:
				printf( "{" );
				break;
			case JSONNode::eArray:
				printf( "[" );
				break;
			case JSONNode::ePair:
			{
				JSONStringBuffer buffer;
				JSONCodec::encode( ((JSONPairNode*)node)->getName(), &buffer );
				printf( "\"%s\" : ", buffer.getBuffer() );
				break;
			}
			case JSONNode::eString:
			{
				JSONStringBuffer buffer;
				JSONCodec::encode( ((JSONStringNode*)node)->getValue(), &buffer );
				printf( "\"%s\"", buffer.getBuffer() );
				postPrint( node );
				break;
			}
			case JSONNode::eNull:
				printf( "null" );
				postPrint( node );
				break;
			case JSONNode::eDouble:
				printf( "%lf", ((JSONDoubleNode*)node)->getValue() );
				postPrint( node );
				break;
			case JSONNode::eInt:
				printf( "%d", ((JSONIntNode*)node)->getValue() );
				postPrint( node );
				break;
			case JSONNode::eBoolean:
				printf( "%s", ((JSONBooleanNode*)node)->getValue() ? "true" : "false" );
				postPrint( node );
				break;
			case JSONNode::eComment:
				printf( "//%s\n", ((JSONCommentNode*)node)->getValue() );
				break;
			default:
				printf( "unknown\n" );
				break;
		}


	}

	printf( "\n" );

	return 0;
}


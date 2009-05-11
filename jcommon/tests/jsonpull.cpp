/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "jjson.h"

using namespace jcommon;

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

	JSONPullParser parser;
	parser.append( source, strlen( source ) );

	free( source );

	for( JSONPullEvent * event = parser.getNext();
			NULL != event;
			event = parser.getNext() ) {

		switch( event->getEventType() ) {
			case JSONPullEvent::eStartObject:
				printf( "{\n" );
				break;
			case JSONPullEvent::eEndObject:
				printf( "},\n" );
				break;
			case JSONPullEvent::eStartArray:
				printf( "[" );
				break;
			case JSONPullEvent::eEndArray:
				printf( "],\n" );
				break;
			case JSONPullEvent::eNull:
				printf( "null,\n" );
				break;
			case JSONPullEvent::eName:
				printf( "\"%s\" : ", ((JSONTextEvent*)event)->getText() );
				break;
			case JSONPullEvent::eString:
				printf( "\"%s\",\n", ((JSONTextEvent*)event)->getText() );
				break;
			case JSONPullEvent::eNumber:
				printf( "%s,\n", ((JSONTextEvent*)event)->getText() );
				break;
			case JSONPullEvent::eBoolean:
				printf( "%s,\n", ((JSONTextEvent*)event)->getText() );
				break;
			case JSONPullEvent::eComment:
				printf( "//%s\n", ((JSONTextEvent*)event)->getText() );
				break;
			case JSONPullEvent::ePadding:
				printf( "%s\n", ((JSONTextEvent*)event)->getText() );
				break;
		}

		delete event;
	}

	if( NULL != parser.getError() ) {
		printf( "\n\nerror: %s\n", parser.getError() );
	}

	return 0;
}


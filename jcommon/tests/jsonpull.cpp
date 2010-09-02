#include "jjson.h"

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace jcommon;

int main( int argc, char * argv[] )
{
	char * filename = NULL;

	if( argc < 2 ) {
		std::cout << "Usage: " << argv[0] << " <json_file>" << std::endl;
		exit( -1 );
	} else {
		filename = argv[1];
	}

	FILE * fp = fopen ( filename, "r" );
	if( NULL == fp ) {
		std::cout << "cannot not open " << filename << std::endl;
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
				std::cout << "{" << std::endl;
				break;
			case JSONPullEvent::eEndObject:
				std::cout << "}," << std::endl;
				break;
			case JSONPullEvent::eStartArray:
				std::cout << "[";
				break;
			case JSONPullEvent::eEndArray:
				std::cout << "]," << std::endl;
				break;
			case JSONPullEvent::eNull:
				std::cout << "null,\n" << std::endl;
				break;
			case JSONPullEvent::eName:
				std::cout << "\"" << ((JSONTextEvent*)event)->getText() << "\" : "; 
				break;
			case JSONPullEvent::eString:
				std::cout << "\"" << ((JSONTextEvent*)event)->getText() << "\", " << std::endl;
				break;
			case JSONPullEvent::eNumber:
				std::cout << ((JSONTextEvent*)event)->getText() << ", " << std::endl;
				break;
			case JSONPullEvent::eBoolean:
				std::cout << ((JSONTextEvent*)event)->getText() << ", " << std::endl;
				break;
			case JSONPullEvent::eComment:
				std::cout << "//" << ((JSONTextEvent*)event)->getText() << std::endl;
				break;
			case JSONPullEvent::ePadding:
				std::cout << ((JSONTextEvent*)event)->getText() << std::endl;
				break;
		}

		delete event;
	}

	if( NULL != parser.getError() ) {
		std::cout << "\nerror: " << parser.getError() << std::endl;
	}

	return 0;
}


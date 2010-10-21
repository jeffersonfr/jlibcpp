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


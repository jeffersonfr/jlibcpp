#include "jjson.h"

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace jcommon;

static void postPrint( const JSONNode * node )
{
	if( node->isLastChild() ) {
		const JSONNode * parent = node->getParent();
		if( JSONNode::ePair == parent->getType() ) {
			parent = parent->getParent();
		}

		if( JSONNode::eObject == parent->getType() ) {
			std::cout << "}";
		} else {
			assert( JSONNode::eArray == parent->getType() );
			std::cout << "]";
		}

		postPrint( parent );
	} else {
		if( NULL != node->getParent() ) {
			std::cout << ",";
		}
	}
}

int main( int argc, char * argv[] )
{
	char * filename = NULL;

	if( argc < 2 ) {
		std::cout << "usage: " << argv[0] << " <json_file>" << std::endl;
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

	JSONDomParser parser;
	parser.append( source, strlen( source ) );

	if( NULL != parser.getError() ) {
		std::cout << "\n\nerror: " << parser.getError() << std::endl;
	}

	free( source );

	std::cout << "Test DomBuffer" << std::endl;

	JSONDomBuffer buffer( parser.getValue(), 1 );

	std::cout << buffer.getBuffer() << std::endl << std::endl;

	std::cout << "Test Iterator" << std::endl;

	JSONIterator iterator( parser.getValue() );

	for( ; ; ) {
		const JSONNode * node = iterator.getNext();
		if( NULL == node ) break;

		switch( node->getType() ) {
			case JSONNode::eObject:
				std::cout << "{";
				break;
			case JSONNode::eArray:
				std::cout << "[";
				break;
			case JSONNode::ePair:
			{
				JSONStringBuffer buffer;
				JSONCodec::encode( ((JSONPairNode*)node)->getName(), &buffer );
				std::cout << "\"" << buffer.getBuffer() << "\" : ";
				break;
			}
			case JSONNode::eString:
			{
				JSONStringBuffer buffer;
				JSONCodec::encode( ((JSONStringNode*)node)->getValue(), &buffer );
				std::cout << "\"" << buffer.getBuffer() << "\"";
				postPrint( node );
				break;
			}
			case JSONNode::eNull:
			std::cout << "null";
				postPrint( node );
				break;
			case JSONNode::eDouble:
				std::cout << ((JSONDoubleNode*)node)->getValue();
				postPrint( node );
				break;
			case JSONNode::eInt:
				std::cout << ((JSONIntNode*)node)->getValue();
				postPrint( node );
				break;
			case JSONNode::eBoolean:
				std::cout << ((((JSONBooleanNode*)node)->getValue() == true)?"true":"false");
				postPrint( node );
				break;
			case JSONNode::eComment:
				std::cout << "//" << ((JSONCommentNode*)node)->getValue() << std::endl;
				break;
			default:
				std::cout << "unknown" << std::endl;
				break;
		}
	}

	std::cout << std::endl;

	return 0;
}


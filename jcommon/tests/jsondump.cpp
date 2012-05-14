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
#include "jparserexception.h"

#include <vector>
#include <iostream>

#include <stdio.h>

#define IDENT(n) for (int i = 0; i < n; ++i) printf("    ")

void populate_sources(const char *filter, std::vector<std::vector<char> > &sources)
{
	char filename[256];

	for (int i = 1; i < 64; ++i) {
		sprintf(filename, filter, i);
		FILE *fp = fopen(filename, "rb");

		if (fp == NULL) {
			break;
		}

		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		std::vector<char> buffer(size + 1);
		fread (&buffer[0], 1, size, fp);
		fclose(fp);
		sources.push_back(buffer);
	}

	printf("Loaded %d json files\n", sources.size());
}

void print(jcommon::JSONValue *value, int ident = 0)
{
	IDENT(ident);

	if (value->GetName() != NULL) {
		printf("\"%s\" = ", value->GetName());
	}

	switch(value->GetType()) {
		case jcommon::JSON_NULL:
			printf("null\n");
			break;
		case jcommon::JSON_OBJECT:
		case jcommon::JSON_ARRAY:
			printf(value->GetType() == jcommon::JSON_OBJECT ? "{\n" : "[\n");
			for (jcommon::JSONValue *it = value->GetFirstChild(); it; it = it->NextSibling()) {
				print(it, ident + 1);
			}
			IDENT(ident);
			printf(value->GetType() == jcommon::JSON_OBJECT ? "}\n" : "]\n");
			break;
		case jcommon::JSON_STRING:
			printf("\"%s\"\n", value->GetString());
			break;
		case jcommon::JSON_INT:
			printf("%d\n", value->GetInteger());
			break;
		case jcommon::JSON_FLOAT:
			printf("%f\n", value->GetFloat());
			break;
		case jcommon::JSON_BOOL:
			printf(value->GetBoolean()?"true\n":"false\n");
			break;
	}
}

bool parse(char *source)
{
	try {
		jcommon::JSONValue *root = jcommon::JSON::Parse(source);

		print(root);

		delete root;
	} catch (jcommon::ParserException &e) {
		std::cout << e.what() << std::endl;
	}

	return false;
}

int main(int argc, char **argv)
{
	// Fail
	std::vector<std::vector<char> > sources;
	int passed = 0;
	
	printf(":: FAIL ::n\n");

	populate_sources("test/fail%d.json", sources);
	
	for (size_t i = 0; i < sources.size(); ++i) {
		printf("Parsing %d\n", i + 1);

		if (parse(&sources[i][0])) {
			++passed;
		}
	}

	printf("Passed %d from %d tests\n", passed, sources.size());

	// Pass
	printf("\n:: PASS ::n\n");
	sources.clear();
	populate_sources("test/pass%d.json", sources);
	passed = 0;

	for (size_t i = 0; i < sources.size(); ++i) {
		printf("Parsing %d\n", i + 1);

		if (parse(&sources[i][0])) {
			++passed;
		}
	}

	printf("Passed %d from %d tests\n", passed, sources.size());

	return 0;
}

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
#include "joptions.h"

#include <iostream>
#include <string>
#include <sstream>

#include <time.h>

extern int opterr;

#ifdef _WIN32

#include <windows.h>

char	*optarg;		// global argument pointer
int		optind = 0; 	// global argv index

int getopt(int argc, TCHAR *argv[], TCHAR *optstring)
{
	static TCHAR *next = NULL;
	if (optind == 0)
		next = NULL;

	optarg = NULL;

	if (next == NULL || *next == '\0')
	{
		if (optind == 0)
			optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
		{
			optarg = NULL;
			if (optind < argc)
				optarg = argv[optind];
			return EOF;
		}

		if (strcmp(argv[optind], "--") == 0)
		{
			optind++;
			optarg = NULL;
			if (optind < argc)
				optarg = argv[optind];
			return EOF;
		}

		next = argv[optind];
		next++;		// skip past -
		optind++;
	}

	char c = *next++;
	char *cp = strchr(optstring, c);

	if (cp == NULL || c == ':')
		return '?';

	cp++;
	if (*cp == ':')
	{
		if (*next != '\0')
		{
			optarg = next;
			next = NULL;
		}
		else if (optind < argc)
		{
			optarg = argv[optind];
			optind++;
		}
		else
		{
			return '?';
		}
	}

	return c;
}
#else
int opterr = 0;
#endif

namespace jcommon {

Options::Options(int argc, char **argv):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Options");

	_argc = argc;
	_argv = argv;
}

Options::~Options()
{
}

void Options::SetOptions(std::string options)
{
	if (_argc < 2) {
		return;
	}

	if ((void *)_argv == NULL) {
		return;
	}

	int opt;

	_is_default = false;

#ifdef _WIN32
	while ((opt = getopt(_argc, _argv, (TCHAR *)options.c_str())) != EOF) {
#else
	while ((opt = getopt(_argc, _argv, options.c_str())) != EOF) {
#endif
		if (opt != '?' && opt != ':') {
			if ((void *)optarg != NULL) {
		 		_tags[opt] = optarg;
			} else {
		 		_tags[opt] = "";
			}
		}
	}
}

bool Options::ExistsOption(std::string key)
{
	if (key.empty() == true) {
		return false;
	}

	return _tags.find((int)key[0]) != _tags.end();
}

bool Options::ExistsArgument(std::string key)
{
	if (key.empty() == true) {
		return false;
	}

	return _tags[key[0]] != "";
}

std::string Options::GetArgument(std::string key)
{
	if (key.empty() == true) {
		return false;
	}

	return _tags[key[0]];
}

}

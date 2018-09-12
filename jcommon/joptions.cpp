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
#include "jcommon/joptions.h"

#include <unistd.h>

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

	if ((void *)_argv == nullptr) {
		return;
	}

	int opt;

	_is_default = false;

	while ((opt = getopt(_argc, _argv, options.c_str())) != EOF) {
		if (opt != '?' && opt != ':') {
			if ((void *)optarg != nullptr) {
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
		return "";
	}

	return _tags[key[0]];
}

}

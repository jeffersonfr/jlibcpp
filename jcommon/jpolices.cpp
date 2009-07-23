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
#include "jpolices.h"
#include "jruntimeexception.h"
#include "jstringutils.h"
#include "jfile.h"

#include <sstream>
#include <iostream>
#include <vector>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

namespace jcommon {

Polices::Polices()
{
}

Polices::~Polices()
{
}

void Polices::Load(std::string filename) 
{
	_filename = filename;

	jio::File file(_filename);

	std::string tag;
	int r,
		state = 0;
	char c;

	std::string id,
		content;

	while ((r = file.Read((char *)&c, 1)) != EOF) {
		if (state == 0) {
			id = "";
			content = "";

			if (c == '#') {
				content = "#";
				state = 1;
			} else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
				id = id + c;
				state = 2;
			}
		} else if (state == 1) {
			if (c == '\n') {
				struct jpolice_t t;

				t.key = jcommon::StringUtils::Trim(id);
				t.value = jcommon::StringUtils::Trim(content);
				t.id = 1;

				_polices.push_back(t);

				content = "";
				state = 0;
			} else {
				content = content + c;
			}
		} else if (state == 2) {
			if (c != '{' && c != '\n') {
				id = id + c;
			} else {
				state = 3;
			}
		} else if (state == 3) {
			if (c != '}' ) {
				content = content + c;
			} else {
				struct jpolice_t t;

				t.key = jcommon::StringUtils::Trim(id);
				t.value = jcommon::StringUtils::Trim(content);
				t.id = 1;

				_polices.push_back(t);

				state = 0;
			}
		}
	}

	/*
	   for (std::vector<struct police_t>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
	   printf(":: id [%s], content [%s] %d\n", (*i).key.c_str(), (*i).value.c_str(), (*i).id);
	   }
	   */
}

void Polices::Save()
{
	try {
		jio::File f(_filename, jio::F_WRITE_ONLY | jio::F_LARGEFILE | jio::F_TRUNCATE);

		for (std::vector<struct jpolice_t>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
			std::ostringstream o;

			struct jpolice_t p = *i;

			if (p.id == 0) {
				o << p.value << std::endl;
			} else {
				o << p.key << " {\n\t" << p.value << "\n}\n" << std::endl;
			}

			f.Write(o.str().c_str(), (long)o.str().size());
		}

		f.Flush();
	} catch (...) {
		throw jcommon::RuntimeException("File not found !");
	}
}

int Polices::GetSize()
{
	return _polices.size();
}

std::vector<struct jpolice_t> & Polices::GetPolices()
{
	return _polices;
}

std::string Polices::GetContent(std::string id) 
{
	for (std::vector<struct jpolice_t>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
		if ((*i).id == 1) {
			return (*i).value;
		}
	}

	return "";
}


}


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
#include "Stdafx.h"
#include "jproperties.h"
#include "jstringutils.h"
#include "jautolock.h"
#include "jruntimeexception.h"
#include "jillegalargumentexception.h"
#include "jfile.h"

#include <iostream>
#include <fstream>

namespace jcommon {

Properties::Properties():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Properties");
}

Properties::~Properties()
{
}

void Properties::Load(std::string filename_, std::string escape_)
{
	jthread::AutoLock lock(&_mutex);

	_filename = filename_;

	std::ifstream reader(_filename.c_str());
	std::string line;

	while (std::getline(reader, line)) {
		line = jcommon::StringUtils::Trim(line);

		if (line.find("#") == 0) {
			// INFO:: comments
		} else {
			std::string::size_type r1 = line.find(escape_);
			std::string::size_type r2 = line.find("#");

			if (line.size() > 0 && r1 != std::string::npos && (r2 == std::string::npos || (r2 != std::string::npos && r1 < r2))) {
				std::string key = jcommon::StringUtils::Trim(line.substr(0, r1));
				std::string value = jcommon::StringUtils::Trim(line.substr(r1+1));

				SetTextParam(key, value);
			} else {
				// INFO:: comments
			}
		}
	}

	reader.close();
}

void Properties::Save(std::string escape_)
{
	jthread::AutoLock lock(&_mutex);

	jio::File *file = NULL;

	try {
		file = jio::File::OpenFile(_filename, (jio::jfile_flags_t)(jio::JFF_WRITE_ONLY | jio::JFF_LARGEFILE | jio::JFF_TRUNCATE));
	
		if (file == NULL) {
			file = jio::File::CreateFile(_filename, (jio::jfile_flags_t)(jio::JFF_WRITE_ONLY | jio::JFF_LARGEFILE | jio::JFF_TRUNCATE));
		
			if (file == NULL) {
				throw RuntimeException(jcommon::StringUtils::Format("Unable to save properties at '%s'", _filename.c_str()));
			}
		}

		std::map<std::string, std::string> params = GetParameters();

		for (std::map<std::string, std::string>::iterator i=params.begin(); i!=params.end(); i++) {
			std::string line = i->first + " " + escape_ + " " + i->second + "\n";

			file->Write(line.c_str(), (int64_t)line.size());
		}

		file->Flush();
		file->Close();

		delete file;
	} catch (jcommon::RuntimeException &e) {
		if (file != NULL) {
			delete file;
		}

		throw e;
	}
}

}


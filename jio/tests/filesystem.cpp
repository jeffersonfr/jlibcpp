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
#include "jfile.h"
#include "jruntimeexception.h"

#include <iostream>
#include <map>
#include <string>

int main()
{
	std::map<std::string, jio::File *> files;
	
	files["file10"] = jio::File::OpenFile("/tmp/file10"); // NULL (if not exists)
	files["file21"] = jio::File::CreateFile("/tmp/file20"); // valid (if not exists)
	files["file30"] = jio::File::CreateTemporaryFile("/tmp/file30", "prefix"); // NULL (path is not a directory)
	files["file31"] = jio::File::CreateTemporaryFile("/tmp", "prefix"); // valid (if not exists)
	files["file32"] = jio::File::CreateTemporaryFile("/tmp", "prefix", "sufix"); // valid (if not exists)
	files["dir10"] = jio::File::OpenDirectory("ABC"); // NULL (if not exists)
	files["dir11"] = jio::File::OpenDirectory("/tmp"); // valid (if exists)
	files["dir20"] = jio::File::CreateDirectory("/tmp"); // NULL (if exists)
	files["dir21"] = jio::File::CreateDirectory("/tmp/dir21"); // valid (if not exists)
	files["dir30"] = jio::File::CreateTemporaryDirectory("/tmp/dir30", "prefix"); // NULL (if not exists)
	files["dir31"] = jio::File::CreateTemporaryDirectory("/tmp", "prefix"); // valid (if not exists)

	for (std::map<std::string, jio::File *>::iterator i=files.begin(); i!=files.end(); i++) {
		jio::File *file = i->second;

		if (file != NULL) {
			printf("[%s] = %s\n", i->first.c_str(), file->GetAbsolutePath().c_str());
		} else {
			printf("[%s] = %p\n", i->first.c_str(), file);
		}
	}

	printf("\npress any key to remove files/directories\n");

	getchar();

	for (std::map<std::string, jio::File *>::iterator i=files.begin(); i!=files.end(); i++) {
		jio::File *file = i->second;

		if (file != NULL) {
			printf("Removing [%s = %s]\n", i->first.c_str(), file->GetAbsolutePath().c_str());

			try {
				file->Remove();
				file->Close();
			} catch (jcommon::RuntimeException &e) {
				e.PrintStackTrace();
			}

			delete file;
		}
	}

	files.clear();

	return 0;
}

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

#include <iostream>

int main()
{
	char *args[6] = {
		(char *)"./file", 
		(char *)"/file",
		(char *)"../../../file",
		(char *)"././././../../teste/file",
		(char *)"/teste1/../../teste2/./././////file",
		(char *)"/../../teste/./././././././file"
	};

	for (int i=0; i<6; i++) {
		jio::File *file = jio::File::OpenFile(args[i]);

		std::cout << "File Reference: [" << args[i] << "]" << std::endl;
		std::cout << "GetPath: [" << file->GetPath() << "]" << std::endl;
		std::cout << "GetName: [" << file->GetName() << "]" << std::endl;
		std::cout << "Process: [" << jio::File::GetFixedPath(file->GetPath()) << "]" << std::endl;
		std::cout << "Normalize: [" << jio::File::NormalizePath(file->GetPath()) << "]" << std::endl;
		std::cout << "GetAbsolutePath: [" << file->GetAbsolutePath() << "]" << std::endl;
		std::cout << "GetCanonicalPath: [" << file->GetCanonicalPath() << "]\n" << std::endl;

		delete file;
	}

	return 0;
}

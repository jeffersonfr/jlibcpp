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
	jio::File *files[6];

	files[0] = new jio::File("./file");
	files[1] = new jio::File("././././../../teste/file");
	files[2] = new jio::File("/file");
	files[3] = new jio::File("../../../file");
	files[4] = new jio::File("/teste1/../../teste2/./././////file");
	files[5] = new jio::File("/../../teste/./././././././file");
	// files[5] = new jio::File("../../../..");

	for (int i=0; i<6; i++) {
		std::cout << "Path: [" << files[i]->GetPath() << "] Name: [" << files[i]->GetName() << "] Normalize: [" << jio::File::NormalizePath(files[i]->GetPath()) << "] Process: [" << jio::File::ProcessPath(files[i]->GetPath()) << "]" << std::endl;
		std::cout << "Canonical: [" << files[i]->GetCanonicalPath() << "] Absolute: [" << files[i]->GetAbsolutePath() << "]\n" << std::endl;
	}

	return 0;
}

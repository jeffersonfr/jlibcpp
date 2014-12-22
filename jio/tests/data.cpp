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
#include "jfileinputstream.h"
#include "jfileoutputstream.h"
#include "jdatainputstream.h"
#include "jdataoutputstream.h"

#include <iostream>

int main() 
{
	jio::FileOutputStream *fos = new jio::FileOutputStream("/tmp/data.txt");
	jio::DataOutputStream *dos = new jio::DataOutputStream(fos);
	
	dos->WriteBoolean(true);
	dos->WriteInteger(3232);
	dos->WriteString("Hello, world !");
	dos->WriteFloat(3.141592654);

	dos->Flush();
	dos->Close();

	delete dos;
	delete fos;

	jio::FileInputStream *fis = new jio::FileInputStream("/tmp/data.txt");
	jio::DataInputStream *dis = new jio::DataInputStream(fis);

	std::string s;
	bool b;
	int i;
	float f;

	b = dis->ReadBoolean();
	i = dis->ReadInteger();
	s = dis->ReadString();
	f = dis->ReadFloat();

	std::cout << ":: " << b << ", " << i << ", " << s << ", " << f << std::endl;

	delete dis;
	delete fis;

	return EXIT_SUCCESS;
}


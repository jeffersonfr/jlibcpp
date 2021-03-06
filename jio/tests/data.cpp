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
#include "jio/jfileinputstream.h"
#include "jio/jfileoutputstream.h"
#include "jio/jdatainputstream.h"
#include "jio/jdataoutputstream.h"

#include <iostream>

int main() 
{
	jio::FileOutputStream fos("/tmp/data.txt");
	jio::DataOutputStream dos(&fos);
	
	dos.WriteBoolean(true);
	dos.WriteInteger(3232);
	dos.WriteString("Hello, world !");
	dos.WriteFloat(3.141592654);

	dos.Flush();
	dos.Close();

	jio::FileInputStream fis("/tmp/data.txt");
	jio::DataInputStream dis(&fis);

	std::string s;
	bool b;
	int i;
	float f;

	b = dis.ReadBoolean();
	i = dis.ReadInteger();
	s = dis.ReadString();
	f = dis.ReadFloat();

	std::cout << ":: " << b << ", " << i << ", " << s << ", " << f << std::endl;

	return EXIT_SUCCESS;
}


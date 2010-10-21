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
#include "jwrapperprocess.h"

#include <iostream>
#include <sstream>

#include <string.h>

using namespace std;
using namespace jshared;

int ostream_test()
{
  WrapperProcess grep("/bin/ls -l");

  grep.Write("This is the first line");
  grep.Write("This is Seven: 7");
  
  stringbuf sb;
  char buf[200];

  grep.Read(buf, 200);
  
  cout << "ostream test OK!" << endl;
  cout << buf << endl;

  return 0;
}

int main() 
{
  ostream_test();
  
  char *params[] = {
		(char *)"-i", 
	  (char *)"-v", 
	  (char *)"test"
  };
  char *dum[] = {
		(char *)"-l"
	};
  char *s = (char *)"This is\na small test\nvery small\nbut a test\n";
  
  WrapperProcess grep("/bin/grep", params);
  WrapperProcess wc("/usr/bin/wc", dum);    

  grep.Write(s, strlen(s));
  wc.Write(s, strlen(s));

  grep.WaitAllData();
 
  char buf[256];
  int len = grep.Read(buf, 255);
  
  buf[len] = 0;
  
  cout << "read " << len << " bytes: " << buf << endl;
  
  wc.Write(buf,len);
  wc.WaitAllData();
  
  len = wc.Read(buf, 255);
  buf[len] = 0;
 
  cout << "read " << len << " bytes: " << buf << endl;

  return 0;
}


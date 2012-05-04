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
#include "jprocess.h"

#include <iostream>
#include <sstream>

#include <string.h>

using namespace std;
using namespace jshared;

void test1()
{
	std::cout << ".. test1 .." << std::endl; 

	int length;
  char buf[256];
 
  Process ls("/bin/ls -l");

	ls.Start();

	while ((length = ls.GetInputStream()->Read(buf, 255)) > 0) {
		if (::write(1, buf, length) < 0) {
			break;
		}
	}
}

void test2()
{
	std::cout << ".. test2 .." << std::endl; 

	// std::string s = "This is\na small test\nvery small\nbut a test\n";
	std::string s = "12\n13\n";
	int length;
  char buf[256];
  
 	Process grep("./multiply");

	grep.Start();

  grep.GetOutputStream()->Write(s.c_str(), s.size());

	while ((length = grep.GetInputStream()->Read(buf, 255)) > 0) {
		if (::write(1, buf, length) < 0) {
			break;
		}
	}
}

void test3()
{
	std::cout << ".. test3 .." << std::endl; 

	std::string s = "Peter Winston\nPhilip Johnson\nAnna Anderson\n";
	int length;
  char buf[256];
  
	Process sort("sort -k 2");

	sort.Start();

  sort.GetOutputStream()->Write(s.c_str(), s.size());
  sort.GetOutputStream()->Close();

	while ((length = sort.GetInputStream()->Read(buf, 255)) > 0) {
		if (::write(1, buf, length) < 0) {
			break;
		}
	}
}

int main() 
{
  test1();
 	test2();
  test3();
  
  return 0;
}


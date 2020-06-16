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
#include "jmath/jdigest.h"

#include <iostream>
#include <sstream>
#include <iomanip>

std::string to_hex(std::string data)
{
  std::ostringstream
    o;

  for (int i=0; i<(int)data.size(); i++) {
    o << std::hex << std::setw(2) << std::setfill('0') << int(data[i] & 0xff);
  }

  return o.str();
}

int main(int argc, char *argv[])
{
  std::string
    msg = "Hello, world ! Have a nice day !";

  // using "echo -n" with commands in shell
	std::string t[][2] = {
		// {"md2", ""}, // 
		{"md4", "ab6d656ae9db9c1f794f0780d917e3a8"}, // openssl md4
		{"md5", "7bc849b1dfed31e7e534cce7ef976e47"}, // md5sum
		{"sha1", "b1df6f6e76cf342dc2e36f58af27d4e6f6b039f3"}, // sha1sum
		// {"sha224", ""}, // sha224sum
		{"sha256", "af379c4c8067bb39c2fe4ace41b3b88cf3c8632ff0311123cc175d64229f1ac2"}, // sha256sum
		// {"sha384", ""}, // sha384sum
		{"sha512", "88e3dd186c9502760be5185d6982ded67357d4976c60b001f3188e6407b11a645da8ec10e524732504a7c1a659af85dacd87fd3329837359dc50b709d348e1d9"}, // sha512sum test.dat
		{"ripemd160", "a3c3e65ccda8dece14eac2846030d8cc0ab87707"}, // openssl rmd160
		// {"mdc2", ""} // 
	};

	for (int i=0; i<6; i++) {
    std::string
      hash = to_hex(jmath::Digest::Hash(t[i][0], msg));

    if (hash != t[i][1]) {
      std::cout << "- error:: " << t[i][0] << std::endl;
      std::cout << "\t" << hash << " != " << t[i][1] << std::endl;
    } else {
      std::cout << "- ok:: " << t[i][0] << std::endl;
    }
	}

	return 0;
}


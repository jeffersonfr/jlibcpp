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

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

std::string DATA;

void report(std::string w, std::string s)
{
	printf("%s::[%s]\n", w.c_str(), s.c_str());
}

void test(jmath::Digest *d, std::string known)
{
	d->Update(DATA.c_str(), DATA.size());

	report("loop", d->GetResult());
	report("again", d->GetResult());

	d->Reset();
	
	for (int i=0; i<(int)DATA.size(); i++) {
		d->Update(DATA.c_str()+i, 1);
	}

	report("reset", d->GetResult());
	report("known", known);

  jmath::Digest digest(d->GetMethod());

	std::string A = "hello";
	std::string B = "world";
	std::string C = A+B;

	digest.Update(C.c_str(), C.size());

	std::string a = digest.GetResult();

	d->Reset();
	d->Update(C.c_str(), C.size());

	std::string b = d->GetResult();

	if (a != b) {
		printf("ERROR\n");
	}

	d->Reset();
	d->Update(A.c_str(), A.size());
	d->Update(B.c_str(), B.size());

	b = d->GetResult();
	
	if (a != b) {
		printf("ERROR\n");
	}
}

int main(int argc, char *argv[])
{
	int fd = open("data.txt", O_RDONLY);
	char tmp[4096];
	int size = read(fd, tmp, 4096);

	close(fd);

	DATA = std::string(tmp, size);

	std::string t[][2] = {
		// {"md2", ""}, // 
		{"md4", "6ae8f00dd52c7290166f6e2beec58299"}, // openssl md4 test.dat
		{"md5", "11214187444f6d47ea910bf1cc3168bb"}, // md5sum test.dat
		{"sha1", "4a5a7326dcd824cedfc2be13999fa182ec4a51b3"}, // sha1sum test.dat
		// {"sha224", "0dccf071e09e617d97a08ef77378be680dd3f07b0c80e3be5c1240b7"}, // sha224sum test.dat
		{"sha256", "181862a52923af652428dc1ce4948696a275f939c6eb0e5e88762da0ada5b3fd"}, // sha256sum test.dat
		// {"sha384", "bf118f2f0e76f4bbd5c0f3179a0a0b4916ccf695f8821b54f31a2982d9816bc19d58436af85adda48cb9d64a1f6e82c4"}, // sha384sum test.dat
		{"sha512", "3c4be5711fe2b1adef3c4ed751fbcec6543f5df6ddb3e1b0a71ded8bf6cd40a00967ff5a216a749dc4787954546866d575ab2bd2b07110f66013534d985e9d8e"}, // sha512sum test.dat
		{"ripemd160", "acf797625812255d44842d143c6981f0e821fde5"}, // openssl rmd160 test.dat
		// {"mdc2", ""} // 
	};

	for (int i=0; i<6; i++) {
		printf("Test:: %s\n", t[i][0].c_str());

    jmath::Digest *d = new jmath::Digest(t[i][0]);

		test(d, t[i][1]);

		delete d;
	}

	return 0;
}


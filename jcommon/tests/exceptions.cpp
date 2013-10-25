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
#include "jioexception.h"
#include "jruntimeexception.h"
#include "jnullpointerexception.h"

#include <iostream>

#include <stdlib.h>

void f() 
{
	throw jio::IOException("f() file not found");
}

void g()
{
	try {
		f();
	} catch (jcommon::Exception &e) {
		throw jcommon::RuntimeException(&e, "g() invalid reference");
	}
}

void h()
{
	try {
		g();
	} catch (jcommon::Exception &e) {
		throw jcommon::RuntimeException(&e, "h() unknown error");
	}
}

void i()
{
	try {
		h();
	} catch (jcommon::Exception &e) {
		throw jcommon::NullPointerException(&e, "i() null pointer");
	}
}

int main(int argc, char *argv[])
{
	std::cout << "Print Stack Trace" << std::endl;

	try {
		i();
	} catch (jcommon::Exception &e) {
		e.PrintStackTrace();
	}

	return EXIT_SUCCESS;
}

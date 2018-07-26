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
#include "jexception/jioexception.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jnullpointerexception.h"

#include <iostream>

#include <stdlib.h>

void f() 
{
	throw jexception::IOException("f() file not found");
}

void g()
{
	try {
		f();
	} catch (jexception::Exception &e) {
		throw jexception::RuntimeException(&e, "g() invalid reference");
	}
}

void h()
{
	try {
		g();
	} catch (jexception::Exception &e) {
		throw jexception::RuntimeException(&e, "h() unknown error");
	}
}

void i()
{
	try {
		h();
	} catch (jexception::Exception &e) {
		throw jexception::NullPointerException(&e, "i() null pointer");
	}
}

int main(int argc, char *argv[])
{
	std::cout << "Print Stack Trace" << std::endl;

	try {
		i();
	} catch (jexception::Exception &e) {
		e.PrintStackTrace();
	}

	return EXIT_SUCCESS;
}


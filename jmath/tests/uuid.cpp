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
#include "jmath/juuid.h"
#include "jexception/jinvalidargumentexception.h"

#include <iostream>

using namespace jmath;

void test(std::string id, bool expect)
{
	std::string date;
	bool result = true;

	try {
		jmath::UUID uuid(id);

		date = uuid.GetDate().what();
	} catch (jexception::InvalidArgumentException &e) {
		result = false;
	}

	std::cout << "UUID::[" << id << "], expect::[" << expect << "], result::[" << result << "]" << std::endl;

	if (result == true) {
		std::cout << date << std::endl;
	}
}

int main()
{
	test("a99459ce-f80b-11e4-bbce-e006e6ced7cf", true);
	test("84949cc5-4701-4a84-895b-354c584a981b", true);
	test("84949CC5-4701-4A84-895B-354C584A981B", true);
	test("84949cc5-4701-4a84-895b-354c584a981bc", false);
	test("84949cc5-4701-4a84-895b-354c584a981", false);
	test("84949cc5x4701-4a84-895b-354c584a981b", false);
	test("84949cc504701-4a84-895b-354c584a981b", false);
	test("84949cc5-470104a84-895b-354c584a981b", false);
	test("84949cc5-4701-4a840895b-354c584a981b", false);
	test("84949cc5-4701-4a84-895b0354c584a981b", false);
	test("g4949cc5-4701-4a84-895b-354c584a981b", false);
	test("84949cc5-4701-4a84-895b-354c584a981g", false);

	return 0;
}


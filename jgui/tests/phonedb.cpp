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
#include "phonedb.h"

#include <algorithm>

namespace mtel {

static bool phone_compare(const PhoneDB::phone_t &a, const PhoneDB::phone_t &b) 
{
	if (a.name > b.name) {
			return false;
	}

	return true;
}

PhoneDB::PhoneDB(std::string file)
{
		_file = file;
}

PhoneDB::~PhoneDB()
{
}

bool PhoneDB::Load()
{
		XmlDocument doc(_file.c_str());

		if (!doc.LoadFile()) {
				return false;
		}

		XmlElement *root;
		XmlElement *psg;

		// parser servern node
		root = doc.RootElement()->FirstChildElement("notes");
		if (root != NULL) {
				if (strcmp(root->Value(), "notes") == 0) {
						std::string name,
								phone1,
								phone2;

						psg = root->FirstChildElement("phone");

						do {
								if (psg == NULL || strcmp(psg->Value(), "phone") != 0) {
										break;
								}

								if (psg->Attribute("name") != NULL) {
										name = psg->Attribute("name");
								}

								if (psg->Attribute("phone1") != NULL) {
										phone1 = psg->Attribute("phone1");
								}

								if (psg->Attribute("phone2") != NULL) {
										phone2 = psg->Attribute("phone2");
								}

								if (name != "" && (phone1 != "" || phone2 != "")) {
										struct phone_t t;

										t.name = name;
										t.phone1 = phone1;
										t.phone2 = phone2;

										events.push_back(t);
								}
						} while ((psg = psg->NextSiblingElement("phone")) != NULL);
				}
		}

		std::sort(events.begin(), events.end(), phone_compare);

		return true;
}

int PhoneDB::GetCapacity()
{
		return 150;
}

int PhoneDB::GetSize()
{
		return events.size();
}

struct PhoneDB::phone_t * PhoneDB::Get(int i)
{
		if (i < 0 || i >= GetSize()) {
				return NULL;
		}

		return &events[i];
}

void PhoneDB::Remove(int i)
{
		if (i < 0 || i >= GetSize()) {
				return;
		}

		events.erase(events.begin()+i);

		std::sort(events.begin(), events.end(), phone_compare);
}

bool PhoneDB::IsFull()
{
		return (GetSize() >= GetCapacity());
}

bool PhoneDB::IsEmpty()
{
		return (GetSize() == 0);
}

bool PhoneDB::Add(std::string name, std::string phone1, std::string phone2)
{
		if (IsFull() == true) {
				return false;
		}

		struct phone_t t;

		t.name = name;
		t.phone1 = phone1;
		t.phone2 = phone2;

		events.push_back(t);
		
		std::sort(events.begin(), events.end(), phone_compare);

		return true;
}

bool PhoneDB::Update(int i, std::string name, std::string phone1, std::string phone2)
{
		if (i < 0 || i >= GetSize()) {
				return false;
		}

		struct phone_t t = events[i];

		events[i].name = name;
		events[i].phone1 = phone1;
		events[i].phone2 = phone2;

		std::sort(events.begin(), events.end(), phone_compare);

		return false;
}

int PhoneDB::Search(std::string name)
{
		for (int i=0; i<(int)events.size(); i++) {
				struct phone_t t = events[i];

				if (strncasecmp(name.c_str(), t.name.c_str(), name.size()) == 0) {
						return i;
				}
		}

		return -1;
}

int PhoneDB::Find(std::string name, std::string phone1, std::string phone2)
{
		for (int i=0; i<(int)events.size(); i++) {
				struct phone_t t = events[i];

				if (t.name == name &&
					t.phone1 == phone1 &&
					t.phone2 == phone2) {
						return i;
				}
		}

		return -1;
}

bool PhoneDB::Save()
{
		std::ostringstream o;

		o << "<phone>" << std::endl;
		o << "<notes>" << std::endl;

		for (int i=0; i<(int)events.size(); i++) {
				struct phone_t t = events[i];

				o << "<phone name=\"" << t.name << "\" phone1=\"" << t.phone1 << "\" phone2=\"" << t.phone2 << "\"/>" << std::endl;
		}

		o << "</notes>" << std::endl;
		o << "</phone>" << std::endl;

		XmlDocument doc;

		doc.Parse(o.str().c_str());

		if (doc.Error()) {
				return false;
		}

		doc.SaveFile(_file.c_str());

		return true;
}

void PhoneDB::RemoveAll()
{
		events.clear();
}

}

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
#include "jcommonlib.h"

using namespace jcommon;

struct luaxml_t {
	std::string id;
	int field; // 0: text, 1: attribute
	int index;
};

std::string xml;

/*
 * \brief Request parameters from a xml document.
 *
 * text value:
 * 		x.y.z
 * 		x.y[i].z
 * 		x.[i]
 * attribute value:
 * 		x.y[i].z:[j]
 * 		x.y[i].z:w
 */
std::string xmlmap(std::string id)
{
	std::vector<luaxml_t> tags;
	std::string tag,
		index;
	int i = 0,
		j,
		state = 255;
	char c;

	while ((c = id[i++]) != '\0') {
		if (state == 255) {
			tag = "";
			index = "";
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
				tag = c;
				state = 0;
			}
		} else if (state == 0) {
			if (c == '.') {
				struct luaxml_t t;

				t.id = tag;
				t.index = 0;
				t.field = 0;

				tags.push_back(t);

				tag = "";
			} else if (c == ':') {
				struct luaxml_t t;

				t.id = tag;
				t.index = 0;
				t.field = 0;

				tags.push_back(t);

				tag = "";
				state = 2;
			} else if (c == '[') {
				state = 1;
			} else {
				tag = tag + c;
			}
		} else if (state == 1) {
			if (c == ']') {
				struct luaxml_t t;

				t.id = tag;
				t.index = atoi(index.c_str());
				t.field = 0;

				tags.push_back(t);

				state = 255;
			} else if (c >= '0' && c <= '9') {
				index = index + c;
			}
		} else if (state == 2) {
			if (c == '[') {
				state = 3;
			} else {
				tag = tag + c;
			}
		} else if (state == 3) {
			if (c == ']') {
				struct luaxml_t t;

				t.id = "";
				t.index = atoi(index.c_str());;
				t.field = 1;

				tags.push_back(t);

				break;
			} else if (c >= '0' && c <= '9') {
				index = index + c;
			}
		}
	}

	if (state == 0) {
		struct luaxml_t t;

		t.id = tag;
		t.index = 0;
		t.field = 0;

		tags.push_back(t);

	} else if (state == 2) {
		struct luaxml_t t;

		t.id = tag;
		t.index = 0;
		t.field = 1;

		tags.push_back(t);

	}

	// xml parser
	XmlDocument doc;

	doc.Parse(xml.c_str());

	if (doc.Error()) {
		return "";
	}

	XmlElement *psg;

	// root = doc.RootElement()->FirstChildElement("body");
	
	psg = doc.RootElement();

	if (psg == NULL) {
		return "";
	}

	i = 0;

	while (i < tags.size()) {
		struct luaxml_t t = tags[i];

		if (psg == NULL) {
			return "";
		}

		if (t.field == 0) {
			j = 0;

			while (j < t.index) {
				// TODO:: sair daqui
				if (t.id == "" || strcmp(psg->Value(), t.id.c_str()) == 0) {
					j++;
				}

				psg = psg->NextSiblingElement();

				if (psg == NULL) {
					return "";
				}
			}
			
			if (j != t.index) {
				return "";
			}

			i++;

			if (i == tags.size()) {
				return psg->GetText();
			} else {
				if (tags[i].field == 0) {
					psg = psg->FirstChildElement();
				}
			}
		} else if (t.field == 1) {
			if (t.id != "") {
				if (psg->Attribute(t.id.c_str()) != NULL) {
					return psg->Attribute(t.id.c_str());
				} else {
					return "";
				}
			} else {
				jcommon::XmlAttribute *attr = psg->FirstAttribute();

				for (j=0; j<t.index; j++) {
					attr = attr->Next();

					if (attr == NULL) {
						return "";
					}
				}

				if (j != t.index) {
					return "";
				}

				return attr->Value();
			}
		} else {
			return "";
		}
	}

	return "";
}

int main()
{
	xml = ""
		"<html>"
		"<body>"
		"<p>p1</p>"
		"<p>p2</p>"
		"<p>p3</p>"
		"<p><font color=#00ff00 size=12 name=arial>p4</font></p>"
		"<font color=#001122>ifont</font>"
		"</body>"
		"</html>";

	std::string s;

	s = xmlmap("html.body.p");
	std::cout << "Return:: [" << s << "]" << std::endl;
	s = xmlmap("html.body.p[2]");
	std::cout << "Return:: [" << s << "]" << std::endl;
	s = xmlmap("html.body.p[3].font");
	std::cout << "Return:: [" << s << "]" << std::endl;
	s = xmlmap("html.body.[1]");
	std::cout << "Return:: [" << s << "]" << std::endl;
	s = xmlmap("html.body.p[3].font:size");
	std::cout << "Return:: [" << s << "]" << std::endl;
	s = xmlmap("html.body.p[3].font:[2]");
	std::cout << "Return:: [" << s << "]" << std::endl;
	
	return 0;
}


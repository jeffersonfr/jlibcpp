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
#ifndef J_HTMLPARSER_H
#define J_HTMLPARSER_H

#include "jinputstream.h"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace jcommon {

enum jtag_type_t {
	BODY_TAG,
	TEXT_TAG
};

class Tag{

	private:
		std::list<Tag *> _childs;
		std::map<std::string, std::string> _attributes;

		Tag *_parent;
		std::string _name;
		jtag_type_t _type;

	public:
		Tag(std::string name, jtag_type_t type);
		virtual ~Tag();

		void SetParent(Tag *parent);
		Tag * GetParent();
		void AddChild(Tag *child);
		std::list<Tag *> & GetChilds();
		std::string GetName();
		jtag_type_t GetType();
		int GetAttributesSize();
		void SetAttribute(std::string key, std::string value);
		std::string GetAttribute(std::string key);
		std::map<std::string, std::string> & GetAttributes();

};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class HTMLParser{

	private:
		Tag *_root;

		void InnerDump(Tag *t, std::string tab);
		std::string ConvertEscape(std::string s);

	public:
		HTMLParser();
		virtual ~HTMLParser();

		void Parse(std::string file);
		void Parse(jio::InputStream *input);
		Tag * GetRoot();

		unsigned int ConvertNamedColor(std::string s);
		
		void Dump();

};

}

#endif

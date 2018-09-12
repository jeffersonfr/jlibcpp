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

#include "jio/jinputstream.h"

#include <vector>
#include <list>
#include <map>

namespace jnetwork {

enum jtag_type_t {
	JTT_BODY,
	JTT_TEXT
};

class Tag{

	private:
		/** \brief */
		std::list<Tag *> _childs;
		/** \brief */
		std::map<std::string, std::string> _attributes;
		/** \brief */
		Tag *_parent;
		/** \brief */
		std::string _name;
		/** \brief */
		jtag_type_t _type;

	public:
		/**
		 * \brief
		 *
		 */
		Tag(std::string name, jtag_type_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Tag();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetParent(Tag *parent);
		
		/**
		 * \brief
		 *
		 */
		virtual Tag * GetParent();
		
		/**
		 * \brief
		 *
		 */
		virtual void AddChild(Tag *child);
		
		/**
		 * \brief
		 *
		 */
		virtual std::list<Tag *> & GetChilds();
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetName();
		
		/**
		 * \brief
		 *
		 */
		virtual jtag_type_t GetType();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetAttributesSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetAttribute(std::string key, std::string value);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetAttribute(std::string key);
		
		/**
		 * \brief
		 *
		 */
		virtual const std::map<std::string, std::string> & GetAttributes();

};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class HTMLParser{

	private:
		/** \brief */
		Tag *_root;
		/** \brief */
		void InnerDump(Tag *t, std::string tab);
		/** \brief */
		std::string ConvertEscape(std::string s);

	public:
		/**
		 * \brief 
		 *
		 */
		HTMLParser();

		/**
		 * \brief 
		 *
		 */
		virtual ~HTMLParser();

		/**
		 * \brief 
		 *
		 */
		virtual void Parse(std::string file);

		/**
		 * \brief 
		 *
		 */
		virtual void Parse(jio::InputStream *input);

		/**
		 * \brief 
		 *
		 */
		virtual Tag * GetRoot();

		/**
		 * \brief 
		 *
		 */
		virtual uint32_t ConvertNamedColor(std::string s);

		/**
		 * \brief 
		 *
		 */
		virtual void Dump();

};

}

#endif

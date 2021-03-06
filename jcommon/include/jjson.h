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
#ifndef JSON_H
#define JSON_H

#include "jio/jinputstream.h"

#include <sstream>

namespace jcommon {

/**
 * \brief
 *
 */
enum json_type_t {
  JSON_NULL,
  JSON_OBJECT,
  JSON_ARRAY,
  JSON_STRING,
  JSON_INT,
  JSON_FLOAT,
  JSON_BOOL,
};

/**
 * \brief Library VJSON modified. 
 *
 * Copyright by J. Random Hacker, 2009
 *
 * Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php
 *
 */
class JSONValue : public jcommon::Object{

  friend class JSON;

  private:
    /** \brief */
    JSONValue *_parent;
    /** \brief */
    JSONValue *_next_sibling;
    /** \brief */
    JSONValue *_first_child;
    /** \brief */
    JSONValue *_last_child;
    /** \brief */
    std::string _name;
    /** \brief */
    std::string _value;
    /** \brief */
    json_type_t _type;

  private:
    /**
     * \brief
     *
     */
    void Append(JSONValue *rhs);

  public:
    /**
     * \brief
     *
     */
    JSONValue();

    /**
     * \brief
     *
     */
    virtual ~JSONValue();

    /**
     * \brief
     *
     */
    virtual json_type_t GetType();

    /**
     * \brief
     *
     */
    virtual std::string GetName();

    /**
     * \brief
     *
     */
    virtual std::string GetValue();

    /**
     * \brief
     *
     */
    virtual void SetValue(std::string value);

    /**
     * \brief
     *
     */
    virtual JSONValue * GetParent();

    /**
     * \brief
     *
     */
    virtual JSONValue * NextSibling();

    /**
     * \brief
     *
     */
    virtual JSONValue * GetFirstChild();

    /**
     * \brief
     *
     */
    virtual JSONValue * GetLastChild();

};

class JSON : public virtual jcommon::Object {

  private:
    JSON()
    {
    }

  public:
    /**
     * \brief
     *
     */
    static JSONValue * Parse(jio::InputStream *stream);

    /**
     * \brief
     *
     */
    static std::string Dump(jcommon::JSONValue *value);

};

}

#endif

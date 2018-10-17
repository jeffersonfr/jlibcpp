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
#include "jcommon/jjson.h"
#include "jexception/jparserexception.h"

#include <sstream>
#include <algorithm>

#include <memory.h>

/**
 * \brief Library VJSON modified. 
 *
 * Copyright by J. Random Hacker, 2009
 *
 * Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php
 *
 */
#define __ERROR(it, desc)	\
	std::ostringstream o;	\
	o << "Parse exception at line " << lines << ": " << desc;	\
	throw jexception::ParserException(o.str());	\

#define __CHECK_TOP()	\
	if (!top) {	\
		__ERROR(it, "Unexpected character");	\
	}	\

#define __IS_DIGIT(c) ((c) >= '0' && (c) <= '9')

namespace jcommon {

JSONValue::JSONValue():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::JSONValue");

	_type = JSON_NULL;
	_parent = nullptr;

	_next_sibling = nullptr;
	_first_child = nullptr;
	_last_child = nullptr;
}

JSONValue::~JSONValue()
{
	std::vector<JSONValue *> values;

	for (JSONValue *value = _first_child; value != nullptr;) {
		JSONValue *next = value->_next_sibling;

		delete value;

		value = next;
	}
}

void JSONValue::Append(JSONValue *rhs)
{
	rhs->_parent = this;

	if (_last_child) {
		_last_child = _last_child->_next_sibling = rhs;
	} else {
		_first_child = _last_child = rhs;
	}
}

json_type_t JSONValue::GetType()
{
	return _type;
}

std::string JSONValue::GetName()
{
	return _name;
}

std::string JSONValue::GetValue()
{
	return _value;
}

void JSONValue::SetValue(std::string value)
{
	_value = value;
}

JSONValue * JSONValue::GetParent()
{
	return _parent;
}

JSONValue * JSONValue::NextSibling()
{
	return _next_sibling;
}

JSONValue * JSONValue::GetFirstChild()
{
	return _first_child;
}

JSONValue * JSONValue::GetLastChild()
{
	return _last_child;
}

// convert string to integer
char * atoi(char *first, char *last, int *out)
{
	int sign = 1;

	if (first != last) {
		if (*first == '-') {
			sign = -1;
			++first;
		} else if (*first == '+') {
			++first;
		}
	}

	int result = 0;

	for (; first != last && __IS_DIGIT(*first); ++first) {
		result = 10 * result + (*first - '0');
	}

	*out = result * sign;

	return first;
}

// convert hexadecimal string to uint32_teger
char * hatoui(char *first, char *last, uint32_t *out)
{
	unsigned int result = 0;

	for (; first != last; ++first) {
		int digit;
		if (__IS_DIGIT(*first)) {
			digit = *first - '0';
		} else if (*first >= 'a' && *first <= 'f') {
			digit = *first - 'a' + 10;
		} else if (*first >= 'A' && *first <= 'F') {
			digit = *first - 'A' + 10;
		} else {
			break;
		}

		result = 16 * result + (unsigned int)digit;
	}
	
	*out = result;

	return first;
}

// convert string to floating point
char * atof(char *first, char *last, double *out)
{
	// sign
	float sign = 1;

	if (first != last) {
		if (*first == '-') {
			sign = -1;
			++first;
		} else if (*first == '+') {
			++first;
		}
	}

	// integer part
	float result = 0;

	for (; first != last && __IS_DIGIT(*first); ++first) {
		result = 10 * result + (float)(*first - '0');
	}

	// fraction part
	if (first != last && *first == '.') {
		++first;

		float inv_base = 0.1f;

		for (; first != last && __IS_DIGIT(*first); ++first) {
			result += (float)(*first - '0') * inv_base;
			inv_base *= 0.1f;
		}
	}

	// result w\o exponent
	result *= sign;

	// exponent
	bool exponent_negative = false;
	int exponent = 0;

	if (first != last && (*first == 'e' || *first == 'E')) {
		++first;

		if (*first == '-') {
			exponent_negative = true;
			++first;
		} else if (*first == '+') {
			++first;
		}

		for (; first != last && __IS_DIGIT(*first); ++first) {
			exponent = 10 * exponent + (*first - '0');
		}
	}

	if (exponent) {
		float power_of_ten = 10;

		for (; exponent > 1; exponent--) {
			power_of_ten *= 10;
		}

		if (exponent_negative) {
			result /= power_of_ten;
		} else {
			result *= power_of_ten;
		}
	}

	*out = result;

	return first;
}

JSONValue * JSON::Parse(jio::InputStream *stream)
{
  if (stream == nullptr) {
    return nullptr;
  }

  char source[stream->Available()];
  char *ptr = source;

  while (stream->Available() > 0) {
    int r = stream->Read(ptr, 4096);

    if (r > 0) {
      ptr = ptr + r;
    }
  }

  *ptr = '\0';

	JSONValue *root = 0;
	JSONValue *top = 0;

	int escaped_newlines = 0;
	int lines = 0;
	char *it = source;
	char *name = nullptr;

	while (*it) {
		// skip white space
		while (*it == '\x20' || *it == '\x9' || *it == '\xD' || *it == '\xA') {
			++it;
		}

		switch (*it) {
		case '\0':
			break;
		case '{':
		case '[': {
				// create new value
				JSONValue *object = new JSONValue();

				// name
				object->_name = "";

				if (name != nullptr) {
					object->_name = std::string(name);
				}

				name = 0;

				// type
				object->_type = (*it == '{') ? JSON_OBJECT : JSON_ARRAY;

				// skip open character
				++it;

				// set top and root
				if (top) {
					top->Append(object);
				} else if (!root) {
					root = object;
				} else {
					__ERROR(it, "Second root. Only one root allowed");
				}
				top = object;
			}

			break;
		case '}':
		case ']': {
				if (!top || top->_type != ((*it == '}') ? JSON_OBJECT : JSON_ARRAY)) {
					__ERROR(it, "Mismatch closing brace/bracket");
				}

				// skip close character
				++it;

				// set top
				top = top->_parent;
			}

			break;
		case ':':
			if (!top || top->_type != JSON_OBJECT) {
				__ERROR(it, "Unexpected character");
			}

			++it;

			break;
		case ',':
			__CHECK_TOP();
			
			++it;

			break;
		case '"': {
				__CHECK_TOP();

				// skip '"' character
				++it;

				char *first = it;
				char *last = it;

				while (*it) {
					if ((unsigned char)*it < '\x20') {
						__ERROR(first, "Control characters not allowed in strings");
					} else if (*it == '\\') {
						switch (it[1]) {
						case '"':
							*last = '"';
							break;
						case '\\':
							*last = '\\';
							break;
						case '/':
							*last = '/';
							break;
						case 'b':
							*last = '\b';
							break;
						case 'f':
							*last = '\f';
							break;
						case 'n':
							*last = '\n';
							++escaped_newlines;
							break;
						case 'r':
							*last = '\r';
							break;
						case 't':
							*last = '\t';
							break;
						case 'u': {
								unsigned int codepoint;

								if (hatoui(it + 2, it + 6, &codepoint) != it + 6) {
									__ERROR(it, "Bad unicode codepoint");
								}

								if (codepoint <= 0x7F) {
									*last = (char)codepoint;
								} else if (codepoint <= 0x7FF) {
									*last++ = (char)(0xC0 | (codepoint >> 6));
									*last = (char)(0x80 | (codepoint & 0x3F));
								} else if (codepoint <= 0xFFFF) {
									*last++ = (char)(0xE0 | (codepoint >> 12));
									*last++ = (char)(0x80 | ((codepoint >> 6) & 0x3F));
									*last = (char)(0x80 | (codepoint & 0x3F));
								}
							}

							it += 4;

							break;
						default:
							__ERROR(first, "Unrecognized escape sequence");
						}

						++last;
						it += 2;
					} else if (*it == '"') {
						*last = 0;
						++it;
						break;
					} else {
						*last++ = *it++;
					}
				}

				if (!name && top->_type == JSON_OBJECT) {
					// field name in object
					name = first;
				} else {
					// new string value
					JSONValue *object = new JSONValue();

					object->_name = "";

					if (name != nullptr) {
						object->_name = std::string(name);
					}

					name = 0;

					object->_type = JSON_STRING;
						
					object->_value = "";

					if (first != nullptr) {
						object->_value = std::string(first);
					}

					top->Append(object);
				}
			}
			break;

		case 'n':
		case 't':
		case 'f': {
				__CHECK_TOP();

				// new null/bool value
				JSONValue *object = new JSONValue();

				object->_name = "";

				if (name != nullptr) {
					object->_name = std::string(name);
				}

				name = 0;

				// null
				if (it[0] == 'n' && it[1] == 'u' && it[2] == 'l' && it[3] == 'l') {
					object->_type = JSON_NULL;
					it += 4;
				}
				// true
				else if (it[0] == 't' && it[1] == 'r' && it[2] == 'u' && it[3] == 'e') {
					object->_type = JSON_BOOL;
					it += 4;
				}
				// false
				else if (it[0] == 'f' && it[1] == 'a' && it[2] == 'l' && it[3] == 's' && it[4] == 'e') {
					object->_type = JSON_BOOL;
					it += 5;
				} else {
					__ERROR(it, "Unknown identifier");
				}

				top->Append(object);
			}
			break;

		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': {
				__CHECK_TOP();

				// new number value
				JSONValue *object = new JSONValue();

				object->_name = "";

				if (name != nullptr) {
					object->_name = std::string(name);
				}

				name = 0;

				object->_type = JSON_INT;

				while (*it != '\x20' && *it != '\x9' && *it != '\xD' && *it != '\xA' && *it != ',' && *it != ']' && *it != '}') {
					if (*it == '.' || *it == 'e' || *it == 'E') {
						object->_type = JSON_FLOAT;
					}
					++it;
				}

				top->Append(object);
			}

			break;
		default:
			__ERROR(it, "Unexpected character");
		}
	}

	if (top) {
		__ERROR(it, "Not all objects/arrays have been properly closed");
	}

	return root;
}

void InternalDump(jcommon::JSONValue *value, std::ostringstream &out)
{   
  if (value->GetName().empty() == false) {
    out << "\"" << value->GetName() << "\":";
  }

  switch(value->GetType()) {
    case jcommon::JSON_NULL:
      out << "null";

      break;
    case jcommon::JSON_OBJECT:
    case jcommon::JSON_ARRAY:
      out << ((value->GetType() == jcommon::JSON_OBJECT)?"{":"[");

      for (jcommon::JSONValue *it = value->GetFirstChild(); it; it = it->NextSibling()) {
        InternalDump(it, out);
      }

      out << ((value->GetType() == jcommon::JSON_OBJECT)?"}":"]");

      if (value->NextSibling() != nullptr) {
        out << ",";
      }

      break;
    case jcommon::JSON_STRING:
      out << "\"" << value->GetValue() << "\"";
      
      if (value->NextSibling() != nullptr) {
        out << ",";
      }

      break;
    case jcommon::JSON_INT:
    case jcommon::JSON_FLOAT:
    case jcommon::JSON_BOOL:
      out << value->GetValue();

      if (value->NextSibling() != nullptr) {
        out << ",";
      }

      break;
  }   
}   

std::string JSON::Dump(jcommon::JSONValue *value)
{
  std::ostringstream out;

  // out << "[";

  InternalDump(value, out);

  // out << "]";

  return out.str();
}

}

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
#include "Stdafx.h"
#include "jjson.h"
#include "jparserexception.h"

#include <memory.h>
#include <algorithm>

#define ERROR(it, desc)											\
	std::ostringstream o;											\
	delete [] dup;														\
	o << "Parse exception at line " 					\
		<< lines << ": " << desc;								\
	throw jcommon::ParserException(o.str());	\

#define CHECK_TOP() 												\
	if (!top) {																\
		ERROR(it, "Unexpected character");			\
	}																					\

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')

namespace jcommon {

JSONValue::JSONValue():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::JSONValue");

	_type = JSON_NULL;
	_name = NULL;
	_parent = NULL;

	_next_sibling = NULL;
	_first_child = NULL;
	_last_child = NULL;
	
	_string_value = NULL;
	_int_value = 0;
	_double_value = 0.0;
}

JSONValue::~JSONValue()
{
	std::vector<JSONValue *> values;

	for (JSONValue *value=_first_child; value!=NULL;) {
		JSONValue *next = value->_next_sibling;

		delete value;

		value = next;
	}

	if (_name != NULL) {
		delete [] _name;
	}

	if (_string_value != NULL) {
		delete [] _string_value;
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

char * JSONValue::GetName()
{
	return _name;
}

char * JSONValue::GetString()
{
	return _string_value;
}

bool JSONValue::GetBoolean()
{
	return (bool)_int_value;
}

int JSONValue::GetInteger()
{
	return _int_value;
}

double JSONValue::GetFloat()
{
	return _double_value;
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
	int result = 0;

	if (first != last) {
		if (*first == '-') {
			sign = -1;
			++first;
		} else if (*first == '+') {
			++first;
		}
	}

	for (; first != last && IS_DIGIT(*first); ++first) {
		result = 10 * result + (*first - '0');
	}

	*out = result * sign;

	return first;
}

// convert hexadecimal string to uint32_teger
char * hatoui(char *first, char *last, uint32_t *out)
{
	uint32_t result = 0;

	for (; first != last; ++first) {
		int digit;
		if (IS_DIGIT(*first)) {
			digit = *first - '0';
		} else if (*first >= 'a' && *first <= 'f') {
			digit = *first - 'a' + 10;
		} else if (*first >= 'A' && *first <= 'F') {
			digit = *first - 'A' + 10;
		} else {
			break;
		}

		result = 16 * result + digit;
	}

	*out = result;

	return first;
}

// convert string to floating point
char * atof(char *first, char *last, double *out)
{
	// sign
	double sign = 1;

	if (first != last) {
		if (*first == '-') {
			sign = -1;
			++first;
		} else if (*first == '+') {
			++first;
		}
	}

	// integer part
	double result = 0;

	for (; first != last && IS_DIGIT(*first); ++first) {
		result = 10 * result + (*first - '0');
	}

	// fraction part
	if (first != last && *first == '.') {
		++first;

		double inv_base = 0.1f;
		for (; first != last && IS_DIGIT(*first); ++first) {
			result += (*first - '0') * inv_base;
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

		for (; first != last && IS_DIGIT(*first); ++first) {
			exponent = 10 * exponent + (*first - '0');
		}
	}

	if (exponent) {
		double power_of_ten = 10;

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

JSONValue * JSON::Parse(const char *source)
{
	if (source == NULL) {
		return NULL;
	}

	JSONValue *root = NULL;
	JSONValue *top = NULL;

	int escaped_newlines = 0;
	int lines = 0;
	char *dup = strdup(source);
	char *it = dup;
	char *name = NULL;

	while (*it) {
		if ((*it) == '\n') {
			lines++;
		}

		switch (*it) {
			case '{':
			case '[': {
					// create new value
					JSONValue *object = new JSONValue();

					// name
					object->_name = NULL;

					if (name != NULL) {
						object->_name = strdup(name);
					}
					
					name = NULL;

					// type
					object->_type = (*it == '{')?JSON_OBJECT:JSON_ARRAY;

					// skip open character
					++it;

					// set top and root
					if (top) {
						top->Append(object);
					} else if (!root) {
						root = object;
					} else {
						ERROR(it, "Second root. Only one root allowed");
					}

					top = object;
				}
				break;
			case '}':
			case ']': {
					if (!top || top->_type != ((*it == '}')?JSON_OBJECT:JSON_ARRAY)) {
						ERROR(it, "Mismatch closing brace/bracket");
					}

					// skip close character
					++it;

					// set top
					top = top->_parent;
				}
				break;
			case ':':
				if (!top || top->_type != JSON_OBJECT) {
					ERROR(it, "Unexpected character");
				}
				++it;
				break;
			case ',':
				CHECK_TOP();
				++it;
				break;
			case '"': {
					CHECK_TOP();
					// skip '"' character
					++it;

					char *first = it;
					char *last = it;

					while (*it) {
						if ((uint8_t)*it < '\x20') {
							ERROR(first, "Control characters not allowed in strings");
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
										uint32_t codepoint;

										if (hatoui(it + 2, it + 6, &codepoint) != it + 6) {
											ERROR(it, "Bad unicode codepoint");
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
									ERROR(first, "Unrecognized escape sequence");
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

						object->_name = NULL;

						if (name != NULL) {
							object->_name = strdup(name);
						}

						name = NULL;

						object->_type = JSON_STRING;
						object->_string_value = NULL;

						if (first != NULL) {
							object->_string_value = strdup(first);
						}

						top->Append(object);
					}
				}
				break;
			case 'n':
			case 't':
			case 'f': {
					CHECK_TOP();

					// new null/bool value
					JSONValue *object = new JSONValue();

					object->_name = NULL;

					if (name != NULL) {
						object->_name = strdup(name);
					}

					name = NULL;

					if (strncasecmp(it, "null", 4) == 0) {
						// null
						object->_type = JSON_NULL;
						it += 4;
					} else if (strncasecmp(it, "true", 4) == 0) {
						// true
						object->_type = JSON_BOOL;
						object->_string_value = strdup("true");
						object->_int_value = 1;
						it += 4;
					} else if (strncasecmp(it, "false", 5) == 0) {
						// false
						object->_type = JSON_BOOL;
						object->_string_value = strdup("false");
						object->_int_value = 0;
						it += 5;
					} else {
						ERROR(it, "Unknown identifier");
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
					CHECK_TOP();

					// new number value
					JSONValue *object = new JSONValue();

					object->_name = NULL;

					if (name != NULL) {
						object->_name = strdup(name);
					}

					name = NULL;

					object->_type = JSON_INT;

					char *first = it;

					while (*it != '\x20' && *it != '\x09' && *it != '\x0d' && *it != '\x0a' && *it != ',' && *it != ']' && *it != '}') {
						if (*it == '.' || *it == 'e' || *it == 'E') {
							object->_type = JSON_FLOAT;
						}
						++it;
					}

					if (object->_type == JSON_INT && atoi(first, it, &object->_int_value) != it) {
						ERROR(first, "Bad integer number");
					}

					if (object->_type == JSON_FLOAT && atof(first, it, &object->_double_value) != it) {
						ERROR(first, "Bad float number");
					}

					object->_string_value = strndup(first, it-first);

					top->Append(object);
				}
				break;
			default:
				ERROR(it, "Unexpected character");
		}

		// skip white space
		while (*it == '\x20' || *it == '\x9' || *it == '\xD' || *it == '\xA') {
			++it;
		}
	}

	if (top) {
		ERROR(it, "Not all objects/arrays have been properly closed");
	}

	delete [] dup;

	return root;
}

}

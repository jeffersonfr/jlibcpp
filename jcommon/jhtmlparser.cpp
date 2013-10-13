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
#include "jhtmlparser.h"
#include "jstringutils.h"
#include "jfileinputstream.h"
#include "jioexception.h"

namespace jcommon{

enum jtag_state_t {
	BODY_STATE,
	TEXT_STATE,
	COMMENT_STATE,
	WHITE_STATE
};

Tag::Tag(std::string name, jtag_type_t type)
{
	_parent = NULL;
	_name = name;
	_type = type;

	if (type == JTT_BODY) {
		std::string::size_type pos = name.find(" ");
		std::string attributes;

		if (pos != std::string::npos) {
			_name = name.substr(0, pos);
			attributes = name.substr(pos);
		}
		
		_name = jcommon::StringUtils::ToLower(_name);

		// parse attributes
		std::string attr, value;
		int i = 0;
		char c, escape;

		while (i < (int)attributes.size()) {
			attr = "";
			value = "";

			while ((c = attributes[i++]) != '=' && i < (int)attributes.size()) {
				attr += attributes[i-1];
			}

			while ((c = attributes[i++]) != '\"' && c != '\'' && i < (int)attributes.size()) {
				if (isspace(c) == 0) {
					value = c;

					break;
				}
			}

			if (i >= (int)attributes.size()) {
				break;
			}

			if (c == '\'' || c == '\"') {
				escape = c;

				while ((c = attributes[i++]) != escape && i < (int)attributes.size()) {
					value += attributes[i-1];
				}
			} else {
				while ((c = attributes[i++]) != ' ' && i < (int)attributes.size()) {
					value += attributes[i-1];
				}
			}

			attr = jcommon::StringUtils::ToLower(jcommon::StringUtils::Trim(attr));
			value = jcommon::StringUtils::Trim(value);

			_attributes[attr] = value;
		}
	}
}

Tag::~Tag()
{
}

void Tag::SetParent(Tag *parent)
{
	_parent = parent;
}

Tag * Tag::GetParent()
{
	return _parent;
}

void Tag::AddChild(Tag *child)
{
	_childs.push_back(child);
}

std::list<Tag *> & Tag::GetChilds()
{
	return _childs;
}

std::string Tag::GetName()
{
	return _name;
}

jtag_type_t Tag::GetType()
{
	return _type;
}

int Tag::GetAttributesSize()
{
	return _attributes.size();
}

void Tag::SetAttribute(std::string key, std::string value)
{
	_attributes[key] = value;
}

std::string Tag::GetAttribute(std::string key)
{
	return _attributes[key];
}

std::map<std::string, std::string> & Tag::GetAttributes() 
{
	return _attributes;
}

HTMLParser::HTMLParser()
{
	_root = NULL;
}

HTMLParser::~HTMLParser()
{
	if (_root != NULL) {
		delete _root;
	}
}

void HTMLParser::Parse(std::string file)
{
	jio::FileInputStream *input = NULL;

	try {
		input = new jio::FileInputStream(file);

		Parse(input);
	} catch (jio::IOException &) {
		if (input != NULL) {
			delete input;
		}
	}
}

void HTMLParser::Parse(jio::InputStream *input)
{
	std::list<Tag *> tags;
	jtag_state_t state = WHITE_STATE;
	std::string tag,
		escape,
		comment;
	int r, 
			opentag_count = 0,
			space_count = 0;
	char c;
	bool escape_flag = false;
	Tag *root = NULL;
	
	root = new Tag("page", JTT_BODY);

	tags.push_back(root);

	while ((r  = (int)input->Read((char *)&c, 1)) != EOF) {
		if (c == '\n' || c == '\r' || c == '\t') {
			c = ' ';
		}

		if (state == WHITE_STATE) {
			escape_flag = false;
			opentag_count = 0;
			space_count = 0;

			if (c == '<') {
				state = BODY_STATE;
				tag = "";
				opentag_count = 0;
				space_count = 0;
			} else if (c == '&') {
				state = TEXT_STATE;
				escape_flag = true;
			} else if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
				state = TEXT_STATE;
				tag = tag + c;
			}
		} else if (state == COMMENT_STATE) {
			if (c == '>') {
				if (comment == "--") {
					state = WHITE_STATE;
				}
			}

			if (c == '-' && comment.size() < 2) {
				comment = comment + c;
			} else {
				comment = "";
			}
		} else if (state == TEXT_STATE) {
			if (c == '<') {
				if (escape_flag == true) {
					tag = tag + "&" + escape;
					escape = "";
					escape_flag = false;
				}

				// add text tag
				Tag *t = new Tag(tag, JTT_TEXT);

				t->SetParent(*tags.begin());
				t->GetParent()->GetChilds().push_back(t);

				state = BODY_STATE;
				tag = "";
				opentag_count = 0;
				space_count = 0;
			} else if (c == '&') {
				// - se naum especificar o ';' eu devo ler infinitamente ?
				escape_flag = true;
			} else if (c == ';') {
				if (escape_flag == true) {
					// TODO:: convert char
					std::string o = ConvertEscape(escape);
					tag = tag + o; //escape;
					escape = "";
					escape_flag = false;
				}
			} else {
				if (escape_flag == true) {
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
						escape = escape + c;
					} else {
						if (c == ' ') {
							space_count = 1;
						}

						tag = tag + "&" + escape;
						escape = "";
						escape_flag = false;
					}
				} else {
					if (c != ' ') {
						space_count = 0;
					} else {
						space_count++;
					}

					if (space_count < 2) {
						tag = tag + c;
					}
				}
			}
		} else if (state == BODY_STATE) {
			if (c == '>') {
				if (opentag_count > 0) {
					opentag_count--;
				} else {
					if (tag.size() > 1 && tag[0] == '/') {
						tag = jcommon::StringUtils::ToLower(tag.substr(1));

						for (std::list<Tag *>::iterator i=tags.begin(); i!=tags.end(); i++) {
							if ((*i)->GetName() == tag) {
								while (tags.size() > 2) {
									Tag *t = (*tags.begin());

									tags.erase(tags.begin());

									if (t->GetName() == tag) {
										break;
									}
								}

								break;
							}
						}
					} else {
						// tag = jcommon::StringUtils::ToLower(tag);

						Tag *t = NULL;

						if (tag.size() >= 1 && tag[tag.size()-1] != '/') {
							t = new Tag(tag, JTT_BODY);
						} else {
							t = new Tag(tag.substr(0, tag.size()-1), JTT_BODY);
						}

						t->SetParent(*tags.begin());
						t->GetParent()->GetChilds().push_back(t);

						if (tag.size() >= 1 && tag[tag.size()-1] != '/') {
							// CHANGE:: no insert childs
							// tags.push_front(t);
							
							if (jcommon::StringUtils::ToLower(tag) != "br") {
								tags.push_front(t);
							}
						}
					}
					
					state = WHITE_STATE;
					tag = "";
				}
			} else if (c == '<') {
				opentag_count++;
			} else {
				tag = tag + c;

				if (tag.find("!--") == 0) {
					state = COMMENT_STATE;
				}
			}
		}
	}

	if (state == TEXT_STATE) {
		if (escape_flag == true) {
			tag = tag + "&" + escape;
		}

		Tag *t = new Tag(tag, JTT_TEXT);

		t->SetParent(*tags.begin());
		t->GetParent()->GetChilds().push_back(t);
	}

	_root = root;
}

Tag * HTMLParser::GetRoot()
{
	return _root;
}

unsigned int HTMLParser::ConvertNamedColor(std::string s)
{
	if (s == "aliceblue") return 0xf0f8ff;
	if (s == "antiquewhite") return 0xfaebd7;
	if (s == "aqua") return 0x00ffff;
	if (s == "aquamarine") return 0x7fffd4;
	if (s == "azure") return 0xf0ffff;
	if (s == "beige") return 0xf5f5dc;
	if (s == "bisque") return 0xffe4c4;
	if (s == "black") return 0x000000;
	if (s == "blanchedalmond") return 0xffebcd;
	if (s == "blue") return 0x0000ff;
	if (s == "blueviolet") return 0x8a2be2;
	if (s == "brown") return 0xa52a2a;
	if (s == "burlywood") return 0xdeb887;
	if (s == "cadetblue") return 0x5f9ea0;
	if (s == "chartreuse") return 0x7fff00;
	if (s == "chocolate") return 0xd2691e;
	if (s == "coral") return 0xff7f50;
	if (s == "cornflowerblue") return 0x6495ed;
	if (s == "cornsilk") return 0xfff8dc;
	if (s == "crimson") return 0xdc143c;
	if (s == "cyan") return 0x00ffff;
	if (s == "darkblue") return 0x00008b;
	if (s == "darkcyan") return 0x008b8b;
	if (s == "DarkGoldenRod") return 0xb8860b;
	if (s == "darkgray") return 0xa9a9a9;
	if (s == "darkgrey") return 0xa9a9a9;
	if (s == "darkgreen") return 0x006400;
	if (s == "DarkKhaki") return 0xbdb76b;
	if (s == "darkmagenta") return 0x8b008b;
	if (s == "DarkOliveGreen") return 0x556b2f;
	if (s == "darkorange") return 0xff8c00;
	if (s == "DarkOrchid") return 0x9932cc;
	if (s == "darkred") return 0x8b0000;
	if (s == "darksalmon") return 0xe9967a;
	if (s == "DarkSeaGreen") return 0x8fbc8f;
	if (s == "DarkSlateBlue") return 0x483d8b;
	if (s == "DarkSlateGray") return 0x2f4f4f;
	if (s == "DarkSlateGrey") return 0x3f4f4f;
	if (s == "DarkTurquoise") return 0x00ced1;
	if (s == "darkviolet") return 0x9400d3;
	if (s == "deeppink") return 0xff1493;
	if (s == "DeepSkyBlue") return 0x00bf00;
	if (s == "DimGray") return 0x696969;
	if (s == "DimGrey") return 0x696969;
	if (s == "DodgerBlue") return 0x1e90ff;
	if (s == "FireBrick") return 0xb22222;
	if (s == "FloralWhite") return 0xfffaf0;
	if (s == "ForestGreen") return 0x228b22;
	if (s == "Fuchsia") return 0xff00ff;
	if (s == "Gainsboro") return 0xdcdcdc;
	if (s == "GhostWhite") return 0xf8f8ff;
	if (s == "gold") return 0xffd700;
	if (s == "GoldenRod") return 0xdaa520;
	if (s == "Gray") return 0x808080;
	if (s == "grey") return 0x808080;
	if (s == "green") return 0x008000;
	if (s == "GreenYellow") return 0xadff2f;
	if (s == "HoneyDew") return 0xf0fff0;
	if (s == "HotPink") return 0xff69b4;
	if (s == "indianred") return 0xcd5c5c;
	if (s == "indigo") return 0x4b0082;
	if (s == "ivory") return 0xfffff0;
	if (s == "Khaki") return 0xf0e68c;
	if (s == "lavender") return 0xe6e6fa;
	if (s == "LavenderBlush") return 0xfff0f5;
	if (s == "LawnGreen") return 0x7cfc00;
	if (s == "LemonChiffon") return 0xfffacd;
	if (s == "lightblue") return 0xadd8e6;
	if (s == "lightcoral") return 0xf08080;
	if (s == "lightcyan") return 0xe0ffff;
	if (s == "LightGoldenRodYellow") return 0xfafad2;
	if (s == "lightgray") return 0xd3d3d3;
	if (s == "lightgrey") return 0xd3d3d3;
	if (s == "lightgreen") return 0x90ee90;
	if (s == "lightpink") return 0xffb6c1;
	if (s == "lightsalmon") return 0xffa07a;
	if (s == "LightSeaGreen") return 0x20b2aa;
	if (s == "LightSkyBlue") return 0x87cefa;
	if (s == "LightSlateGray") return 0x778899;
	if (s == "LightSlateGrey") return 0x778899;
	if (s == "LightSteelBlue") return 0xb0c4de;
	if (s == "LightYellow") return 0xffffe0;
	if (s == "lime") return 0x00ff00;
	if (s == "limegreen") return 0x32cd32;
	if (s == "Linen") return 0xfaf0e6;
	if (s == "magenta") return 0xff00ff;
	if (s == "maroon") return 0x800000;
	if (s == "MediumAquaMarine") return 0x6ccdaa;
	if (s == "MediumBlue") return 0x0000cd;
	if (s == "MediumOrchid") return 0xba55d3;
	if (s == "MediumPurple") return 0x9370d6;
	if (s == "MediumSeaGreen") return 0x3cb371;
	if (s == "MediumSlateBlue") return 0x7b68ee;
	if (s == "MediumSpringGreen") return 0x00fa9a;
	if (s == "MediumTurquoise") return 0x48d1cc;
	if (s == "MediumVioletRed") return 0xc71585;
	if (s == "MidnightBlue") return 0x191970;
	if (s == "MintCream") return 0xf5fffa;
	if (s == "MistyRose") return 0xffe4e1;
	if (s == "Moccasin") return 0xffe4b5;
	if (s == "NavajoWhite") return 0xffdead;
	if (s == "navy") return 0x000080;
	if (s == "OldLace") return 0xfdf5e6;
	if (s == "olive") return 0x808000;
	if (s == "OliveDrab") return 0x6b8e23;
	if (s == "orange") return 0xffa500;
	if (s == "orangered") return 0xff4500;
	if (s == "Orchid") return 0xda70d6;
	if (s == "PaleGoldenRod") return 0xeee8aa;
	if (s == "PaleGreen") return 0x98fb98;
	if (s == "PaleTurquoise") return 0xafeeee;
	if (s == "PaleVioletRed") return 0xd87093;
	if (s == "PapayaWhip") return 0xffefd5;
	if (s == "PeachPuff") return 0xffdab9;
	if (s == "peru") return 0xcd853f;
	if (s == "pink") return 0xffc0cb;
	if (s == "plum") return 0xdda0dd;
	if (s == "PowderBlue") return 0xb0e0e6;
	if (s == "purple") return 0x800080;
	if (s == "red") return 0xff0000;
	if (s == "RosyBrown") return 0xbc8f8f;
	if (s == "RoyalBlue") return 0x4169e1;
	if (s == "SaddleBrown") return 0x8b4513;
	if (s == "salmon") return 0xfa8072;
	if (s == "SandyBrown") return 0xf4a460;
	if (s == "seagreen") return 0x2e8b57;
	if (s == "seashell") return 0xfff5ee;
	if (s == "sienna") return 0xa0522d;
	if (s == "silver") return 0xc0c0c0;
	if (s == "skyblue") return 0x87ceeb;
	if (s == "stateblue") return 0x6a5acd;
	if (s == "stategray") return 0x708090;
	if (s == "stategrey") return 0x708090;
	if (s == "snow") return 0xfffafa;
	if (s == "SpringGreen") return 0x00ff7f;
	if (s == "SteelBlue") return 0x4682b4;
	if (s == "tan") return 0xd2b48c;
	if (s == "teal") return 0x008080;
	if (s == "Thistle") return 0xd8bfd8;
	if (s == "Tomato") return 0xff6347;
	if (s == "Turquoise") return 0x40e0d0;
	if (s == "Violet") return 0xee82ee;
	if (s == "Wheat") return 0xf5deb3;
	if (s == "white") return 0xffffff;
	if (s == "WhiteSmoke") return 0xf5f5f5;
	if (s == "Yellow") return 0xffff00;
	if (s == "YellowGreen") return 0x9acd32;

	return 0;
}

std::string HTMLParser::ConvertEscape(std::string s)
{
	s = jcommon::StringUtils::Trim(s);

	if (s == "quot") {
		s = "\"";
	} else if (s == "amp") {
		s = "&";
	} else if (s == "lt") {
		s = "<";
	} else if (s == "gt") {
		s = ">";
	} else if (s == "nbsp") {
		s = "\xa0";
	} else if (s == "iexcl") {
		s =  "\xa1";
	} else if (s == "cent") {
		s = "\xa2";
	} else if (s == "pound") {
		s = "\xa3";
	} else if (s == "curren") {
		s = "\xa4";
	} else if (s == "yen") {
		s = "\xa5";
	} else if (s == "brvbar") {
		s = "\xa6";
	} else if (s == "sect") {
		s = "\xa7";
	} else if (s == "uml") {
		s = "\xa8";
	} else if (s == "copy") {
		s = "\xa9";
	} else if (s == "ordf") {
		s = "\xaa";
	} else if (s == "laquo") {
		s = "\xab";
	} else if (s == "not") {
		s = "\xac";
	} else if (s == "shy") {
		s = "\xad";
	} else if (s == "reg") {
		s = "\xae";
	} else if (s == "macr") {
		s = "\xaf";
	} else if (s == "deg") {
		s = "\xb0";
	} else if (s == "plusmn") {
		s = "\xb1";
	} else if (s == "sup2") {
		s = "\xb2";
	} else if (s == "sup3") {
		s = "\xb3";
	} else if (s == "acute") {
		s = "\xb4";
	} else if (s == "micro") {
		s = "\xb5";
	} else if (s == "para") {
		s = "\xb6";
	} else if (s == "middot") {
		s = "\xb7";
	} else if (s == "cedil") {
		s = "\xb8";
	} else if (s == "sup1") {
		s = "\xb9";
	} else if (s == "ordm") {
		s = "\xba";
	} else if (s == "raquo") {
		s = "\xbb";
	} else if (s == "frac14") {
		s = "\xbc";
	} else if (s == "frac12") {
		s = "\xbd";
	} else if (s == "frac34") {
		s = "\xbe";
	} else if (s == "iquest") {
		s = "\xbf";
	} else if (s == "Agrave") {
		s = "\xc0";
	} else if (s == "Aacute") {
		s = "\xc1";
	} else if (s == "Acirc") {
		s = "\xc2";
	} else if (s == "Atilde") {
		s = "\xc3";
	} else if (s == "Amul") {
		s = "\xc4";
	} else if (s == "Aring") {
		s = "\xc5";
	} else if (s == "AElig") {
		s = "\xc6";
	} else if (s == "Ccdedil") {
		s = "\xc7";
	} else if (s == "Egrave") {
		s = "\xc8";
	} else if (s == "Eacute") {
		s = "\xc9";
	} else if (s == "Ecirc") {
		s = "\xca";
	} else if (s == "Emul") {
		s = "\xcb";
	} else if (s == "Igrave") {
		s = "\xcc";
	} else if (s == "Iacute") {
		s = "\xcd";
	} else if (s == "Icirc") {
		s = "\xce";
	} else if (s == "Emul") {
		s = "\xcf";
	} else if (s == "ETH") {
		s = "\xd0";
	} else if (s == "Ntilde") {
		s = "\xd1";
	} else if (s == "Ograve") {
		s = "\xd2";
	} else if (s == "Oacute") {
		s = "\xd3";
	} else if (s == "Ocirc") {
		s = "\xd4";
	} else if (s == "Otilde") {
		s = "\xd5";
	} else if (s == "Omul") {
		s = "\xd6";
	} else if (s == "times") {
		s = "\xd7";
	} else if (s == "Oslash") {
		s = "\xd8";
	} else if (s == "Ugrave") {
		s = "\xd9";
	} else if (s == "Uacute") {
		s = "\xda";
	} else if (s == "Ucirc") {
		s = "\xdb";
	} else if (s == "Uuml") {
		s = "\xdc";
	} else if (s == "Yacute") {
		s = "\xdd";
	} else if (s == "THORN") {
		s = "\xde";
	} else if (s == "szlig") {
		s = "\xdf";
	} else if (s == "agrave") {
		s = "\xe0";
	} else if (s == "aacute") {
		s = "\xe1";
	} else if (s == "acirc") {
		s = "\xe2";
	} else if (s == "atilde") {
		s = "\xe3";
	} else if (s == "auml") {
		s = "\xe4";
	} else if (s == "aring") {
		s = "\xe5";
	} else if (s == "aelig") {
		s = "\xe6";
	} else if (s == "ccedil") {
		s = "\xe7";
	} else if (s == "egrave") {
		s = "\xe8";
	} else if (s == "eacute") {
		s = "\xe9";
	} else if (s == "ecirc") {
		s = "\xea";
	} else if (s == "euml") {
		s = "\xeb";
	} else if (s == "igrave") {
		s = "\xec";
	} else if (s == "iacute") {
		s = "\xed";
	} else if (s == "icirc") {
		s = "\xee";
	} else if (s == "iuml") {
		s = "\xef";
	} else if (s == "eth") {
		s = "\xf0";
	} else if (s == "ntilde") {
		s = "\xf1";
	} else if (s == "ograve") {
		s = "\xf2";
	} else if (s == "oacute") {
		s = "\xf3";
	} else if (s == "ocirc") {
		s = "\xf4";
	} else if (s == "otilde") {
		s = "\xf5";
	} else if (s == "ouml") {
		s = "\xf6";
	} else if (s == "divide") {
		s = "\xf7";
	} else if (s == "slash") {
		s = "\xf8";
	} else if (s == "ugrave") {
		s = "\xf9";
	} else if (s == "uacute") {
		s = "\xfa";
	} else if (s == "ucirc") {
		s = "\xfb";
	} else if (s == "uuml") {
		s = "\xfc";
	} else if (s == "yacute") {
		s = "\xfd";
	} else if (s == "thorn") {
		s = "\xfe";
	} else if (s == "yuml") {
		s = "\xff";
	} else if (s == "") {
		s = "\0";
	}

	return s;
}

void HTMLParser::InnerDump(Tag *t, std::string tab)
{
	if (t == NULL) {
		return;
	}

	if (t->GetType() == JTT_BODY) {
		std::string attr;

		for (std::map<std::string, std::string>::iterator i=t->GetAttributes().begin(); i!=t->GetAttributes().end(); i++) {
			attr = attr + " " + i->first + "=\"" + i->second + "\"";
		}

		if (t->GetChilds().size() > 0) {
			printf("%s<%s%s>\n", tab.c_str(), t->GetName().c_str(), attr.c_str());
		} else {
			if (t->GetName() != "br") {
				printf("%s<%s%s />\n", tab.c_str(), t->GetName().c_str(), attr.c_str());
			} else {
				printf("%s<%s%s>\n", tab.c_str(), t->GetName().c_str(), attr.c_str());
			}
		}
	} else {
		printf("%s%s\n", tab.c_str(), t->GetName().c_str());
	}

	for (std::list<Tag *>::iterator i=t->GetChilds().begin(); i!=t->GetChilds().end(); i++) {
		InnerDump((*i), tab+"  ");
	}

	if (t->GetType() == JTT_BODY) {
		if (t->GetChilds().size() > 0) {
			printf("%s</%s>\n", tab.c_str(), t->GetName().c_str());
		}
	}
}

void HTMLParser::Dump()
{
	if (_root == NULL) {
		return;
	}

	for (std::list<Tag *>::iterator i=_root->GetChilds().begin(); i!=_root->GetChilds().end(); i++) {
		InnerDump((*i), "");
	}
}

}


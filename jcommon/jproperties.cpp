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
#include "jcommonlib.h"
#include "jiolib.h"
#include "jthreadlib.h"

namespace jcommon {

#ifdef _WIN32

long getdelim(char **lineptr, size_t *n, int delim, FILE *stream) {
	size_t i;

	if (!lineptr || !n) {
		errno=EINVAL;
		return -1;
	}

	if (!*lineptr) {
		*n=0;
	}

	for (i=0; ; ) {
		int x=fgetc(stream);
		if (i>=*n) {
			int tmp=*n+100;
			char *new_ptr = (char *)realloc(*lineptr,tmp);
			
			if (!new_ptr) {
				return -1;
			}

			*lineptr=new_ptr;
			*n=tmp;
		}

		if (x==EOF) { 
			if (!i) {
				return -1; 
			}

			(*lineptr)[i]=0; 
			
			return i; 
		}

		(*lineptr)[i]=x;
		++i;

		if (x==delim) {
			break;
		}
	}

	(*lineptr)[i]=0;

	return i;
}

int getline(char **lineptr, size_t *n, FILE *stream)
{
  return getdelim(lineptr, n, '\n', stream);
}

#endif
	
Properties::Properties():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Properties");
}

Properties::~Properties()
{
}

int Properties::LeftTrim(char **str, int length)
{
	if ((*str) == NULL) {
		return 0;
	}
	
	char *p;
	int count;

	p = (*str);
	
	while (*p == ' ' || *p == '\t') {
		p++;
	}

	if (p == (*str)) {
		return length;
	}

	count = p - (*str);

	memcpy((*str), p, length - count + 1);

	(*str)[length - count + 1] = '\0';

	return (length - count);
}

int Properties::RightTrim(char **str, int length)
{
	if ((*str) == NULL) {
		return 0;
	}
	
	char *q;

	q = (*str) + length - 1;

	while ((*q == ' '|| *q == '\t') && q != (*str)) {
		q--;
	}

	if (q == ((*str) + length - 1)) {
		return length;
	}

	*(++q) = '\0';

	return ((*str) - q - 1);
}

int Properties::Trim(char **str)
{
	int length;

	length = strlen(*str);

	length = LeftTrim(str, length);

	if (length == -1) {
		return -1;
	}
	
	length = RightTrim(str, length);
	
	return length;
}

void Properties::Load(std::string filename_, std::string escape_)
{
	jthread::AutoLock lock(&_mutex);

	_filename = filename_;

	jio::FileInputStream is(_filename);
	jio::BufferedReader reader(&is);

	while (reader.IsEOF() == false) {
		std::string line = jcommon::StringUtils::Trim(reader.ReadLine());

		if (line.find("#") == 0) {
				struct jproperty_t prop;
	
				prop.value = line;
				prop.comment = true;
					
				properties.push_back(prop);
		} else {
			jcommon::StringTokenizer tokens(line, escape_, jcommon::SPLIT_FLAG, false);

			if (tokens.GetSize() == 2) {
					struct jproperty_t prop;

					prop.key = jcommon::StringUtils::Trim(tokens.GetToken(0));
					prop.value = jcommon::StringUtils::Trim(tokens.GetToken(1));
					prop.comment = false;

					properties.push_back(prop);
			}
		}
	}
}

void Properties::Save(std::string escape_)
{
	jthread::AutoLock lock(&_mutex);

	try {
		jio::File f(_filename, jio::F_WRITE_ONLY | jio::F_LARGEFILE | jio::F_TRUNCATE);
	
		for (std::vector<struct jproperty_t>::iterator i=properties.begin(); i != properties.end(); i++) {
			std::ostringstream o;
			
			jproperty_t p = *i;
			
			if (p.comment == false) {
				o << p.key << " " << escape_ << " " << p.value << std::endl;
			} else {
				o << p.value << std::endl;
			}

			f.Write(o.str().c_str(), (long)o.str().size());
		}

		f.Flush();
		f.Close();
	} catch (...) {
		throw RuntimeException("File not found !");
	}
}

void Properties::SetPropertyByName(std::string key, std::string value)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct jproperty_t>::iterator i=properties.begin(); i != properties.end(); i++) {
		if ((*i).comment == false && (*i).key == key) {
			(*i).value = value;

			return;
		}
	}

	struct jproperty_t p;

	p.key = key;
	p.value = value;
	p.comment = false;

	properties.push_back(p);
}

std::string Properties::GetPropertyByName(std::string key, std::string reserv)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct jproperty_t>::iterator i=properties.begin(); i != properties.end(); i++) {
		jproperty_t p = *i;
		
		if (p.comment == false && p.key == key) {
			return p.value;
		}
	}
	
	return reserv;
}

std::string Properties::GetPropertyByIndex(int index, std::string reserv)
{
	jthread::AutoLock lock(&_mutex);

	if ((int)properties.size() == 0 || index < 0 || index > (int)properties.size()-1) {
		return reserv;
	}

	return properties[index].value;
}

void Properties::RemovePropertyByName(std::string key)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct jproperty_t>::iterator i=properties.begin(); i != properties.end(); i++) {
		if ((*i).key == key) {
			properties.erase(i);

			break;
		}
	}
}

void Properties::RemovePropertyByIndex(int index)
{
	jthread::AutoLock lock(&_mutex);

	if ((int)properties.size() == 0 || index < 0 || index > (int)properties.size()-1) {
		throw IllegalArgumentException("Index out of bounds");
	}

	properties.erase(properties.begin()+index);
}

std::vector<std::string> * Properties::GetProperties()
{
	jthread::AutoLock lock(&_mutex);

	std::vector<std::string> *v = new std::vector<std::string>();

	for (std::vector<struct jproperty_t>::iterator i=properties.begin(); i != properties.end(); i++) {
		if ((*i).comment == false) {
			v->push_back((*i).key);	
		}
	}
	
	return v;
}

}


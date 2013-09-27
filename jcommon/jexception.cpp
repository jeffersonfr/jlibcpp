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
#include "jexception.h"

namespace jcommon {

Exception::Exception():
    jcommon::Object(), std::exception()
{
	jcommon::Object::SetClassName("jcommon::Exception");
}

Exception::Exception(std::string reason_):
    jcommon::Object(), std::exception()
{
	jcommon::Object::SetClassName("jcommon::Exception");
	
	_reason = reason_;
}

Exception::Exception(Exception *exception, std::string reason):
    jcommon::Object(), std::exception()
{
	jcommon::Object::SetClassName("jcommon::Exception");
	
	if (exception == NULL) {
		return;
	}

	_reason = reason;

	SetStackTrace(&exception->GetStackTrace());

	_exceptions.push_back(dynamic_cast<Exception *>(exception->Clone()));
}

Exception::~Exception() throw()
{
	for (int i=0; i<(int)_exceptions.size(); i++) {
		delete _exceptions[i];
	}
}

std::string Exception::GetMessage()
{
	return _reason;
}

Exception * Exception::GetCause()
{
	if (_exceptions.empty() == true) {
		return this;
	}

	return _exceptions[0];
}

const std::vector<Exception *> & Exception::GetStackTrace()
{
	return _exceptions;
}

void Exception::SetStackTrace(const std::vector<Exception *> *stack)
{
	if (stack == NULL) {
		return;
	}

	_exceptions.clear();

	for (int i=0; i<(int)stack->size(); i++) {
		_exceptions.push_back(dynamic_cast<Exception *>((*stack)[i]->Clone()));
	}
}

void Exception::PrintStackTrace()
{
	std::cout << "[" << GetFullClassName() << "] " << _reason << std::endl;

	for (int i=(int)_exceptions.size()-1; i>=0; i--) {
		Exception *e = _exceptions[i];

		std::cout << "[" << e->GetFullClassName() << "] " << e->GetMessage() << std::endl;
	}
}

jcommon::Object * Exception::Clone()
{
	Exception *o = new Exception(*this);

	if (_exceptions.empty() == false) {
		o->SetStackTrace(&_exceptions);
	}

	return (jcommon::Object *)o;
}

std::string Exception::what()
{
	return std::exception::what();
}

}

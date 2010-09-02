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
#include "jobservable.h"

namespace jcommon {

Observable::Observable():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Observable");
}

Observable::~Observable()
{
}

void Observable::AddObserver(Observer *o)
{
	_observers.push_back(o);
}

void Observable::RemoveObserver(Observer *o)
{
	std::vector<Observer *>::iterator i = std::find(_observers.begin(), _observers.end(), o);

	if (i != _observers.end()) {
		_observers.erase(i);
	}
}

void Observable::RemoveAllObservers()
{
	_observers.clear();
}

void Observable::NotifyObservers(void *v)
{
	for (std::vector<Observer *>::iterator i=_observers.begin(); i!=_observers.end(); i++) {
		(*i)->Update(v);
	}
}

void Observable::SetChanged(bool b)
{
	_changed = b; 
}

bool Observable::HasChanged()
{
	return _changed;
}

int Observable::CountObservers()
{
	return _observers.size();
}

}


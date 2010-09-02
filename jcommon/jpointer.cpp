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
#include "jpointer.h"

namespace jcommon {

ReferenceCounter *ReferenceCounter::_instance = NULL;

ReferenceCounter::ReferenceCounter() 
{
}

ReferenceCounter *ReferenceCounter::getInstance() 
{
	if (_instance == NULL) {
		_instance = new ReferenceCounter();
	}

	return _instance;
}

void ReferenceCounter::Allocate(void *p) 
{
	if (p == NULL) {
		return;
	}

	if (_counter.count(p) == 0) {
		_counter[p] = 0;
	}

	_counter[p]++;
}

void ReferenceCounter::Free(void *p) 
{
	if (!contem(p)) {
		return;
	}

	_counter[p]--;
	
	if (_counter[p] == 0) {
		_counter.erase(p);
	}
}

bool ReferenceCounter::contem(void *p) 
{
	return (p != NULL) && (_counter.count(p) > 0);
}

}

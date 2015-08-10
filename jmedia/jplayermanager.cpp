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
#include "jplayermanager.h"
#include "jurl.h"

#if defined(DIRECTFB_UI)
#include "nativelightplayer.h"
#include "nativeheavyplayer.h"
#elif defined(SDL2_UI)
#elif defined(GTK3_UI)
#endif

namespace jmedia {

std::map<jplayer_hints_t, bool> PlayerManager::_hints;

PlayerManager::PlayerManager():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jmedia::PlayerManager");
}

PlayerManager::~PlayerManager()
{
}

Player * PlayerManager::CreatePlayer(std::string url_) throw (MediaException)
{
	jcommon::URL url(url_);

	if (url.GetProtocol() == "file" || url.GetProtocol() == "http") {
#if defined(DIRECTFB_UI)
		std::map<jplayer_hints_t, bool>::iterator i = _hints.find(JPH_LIGHTWEIGHT);

		if (i == _hints.end() || i->second == true) {
			return new NativeLightPlayer(url.GetPath());
		} else {
			return new NativeHeavyPlayer(url.GetPath());
		}
#elif defined(SDL2_UI)
#elif defined(GTK3_UI)
#endif
	}

	return NULL;
}
		
void PlayerManager::SetHint(jplayer_hints_t hint, bool value)
{
	_hints[hint] = value;
}

bool PlayerManager::GetHint(jplayer_hints_t hint)
{
	std::map<jplayer_hints_t, bool>::iterator i = _hints.find(hint);

	if (i != _hints.end()) {
		return i->second;
	}

	return false;
}

}


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

#if defined(DIRECTFB_MEDIA)
#include "directfblightplayer.h"
#include "directfbheavyplayer.h"
#endif

#if defined(LIBVLC_MEDIA)
#include "libvlclightplayer.h"
#endif

#if defined(LIBAV_MEDIA)
#include "libavlightplayer.h"
#endif

#if defined(LIBXINE_MEDIA)
#include "libxinelightplayer.h"
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
	if (_hints.size() == 0) {
		_hints[JPH_CACHING] = false;
		_hints[JPH_LIGHTWEIGHT] = false;
		_hints[JPH_SECURITY] = false;
		_hints[JPH_PLUGINS] = false;
	}

#if defined(DIRECTFB_MEDIA)
	try {
		if (_hints[JPH_LIGHTWEIGHT] == false) {
			return new DirectFBHeavyPlayer(url_);
		}
			
		return new DirectFBLightPlayer(url_);
	} catch (jcommon::Exception &e) {
	}
#endif

#if defined(LIBVLC_MEDIA)
	try {
		return new LibVLCLightPlayer(url_);
	} catch (jcommon::Exception &e) {
	}
#endif

#if defined(LIBAV_MEDIA)
	try {
		return new LibAVLightPlayer(url_);
	} catch (jcommon::Exception &e) {
	}
#endif
	
#if defined(LIBXINE_MEDIA)
	try {
		return new LibXineLightPlayer(url_);
	} catch (jcommon::Exception &e) {
	}
#endif

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


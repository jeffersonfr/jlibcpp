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
#include "jmedia/jplayermanager.h"
#include "jnetwork/jurl.h"
#include "jexception/jmediaexception.h"

#if defined(LIBVLC_MEDIA)
#include "libvlclightplayer.h"
#endif

#if defined(LIBAV_MEDIA)
#include "libavlightplayer.h"
#endif

#if defined(LIBXINE_MEDIA)
#include "libxinelightplayer.h"
#endif

#if defined(V4L2_MEDIA)
#include "v4l2lightplayer.h"
#endif

#if defined(GIF_MEDIA)
#include "giflightplayer.h"
#endif

#if defined(IMAGE_LIST_MEDIA)
#include "ilistlightplayer.h"
#endif

#if defined(ALSA_MEDIA)
#include "alsalightplayer.h"
#endif

#if defined(MIXER_MEDIA)
#include "mixerlightplayer.h"
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

Player * PlayerManager::CreatePlayer(std::string url_)
{
	if (_hints.size() == 0) {
		_hints[JPH_CACHING] = false;
		_hints[JPH_LIGHTWEIGHT] = true;
		_hints[JPH_SECURITY] = false;
		_hints[JPH_PLUGINS] = false;
	}

	jnetwork::URL url(url_);

#if defined(MIXER_MEDIA)
	try {
		return new MixerLightPlayer(url_);
	} catch (jexception::Exception &e) {
	}
#endif

  // INFO:: only the MixerLightPlayer could do the mixer
  if (strcasecmp(url.GetProtocol().c_str(), "mixer") == 0) {
    return NULL;
  }

#if defined(ALSA_MEDIA)
	try {
		return new AlsaLightPlayer(url_);
	} catch (jexception::Exception &e) {
	}
#endif

#if defined(IMAGE_LIST_MEDIA)
	try {
		return new ImageListLightPlayer(url_);
	} catch (jexception::Exception &e) {
	}
#endif

#if defined(V4L2_MEDIA)
	try {
		if (strncasecmp(url.GetProtocol().c_str(), "v4l", 3) == 0) {
			return new V4L2LightPlayer(url.GetPath());
		}
	} catch (jexception::Exception &e) {
	}
#endif

#if defined(GIF_MEDIA)
	try {
		return new GIFLightPlayer(url_);
	} catch (jexception::Exception &e) {
	}
#endif

#if defined(LIBVLC_MEDIA)
	try {
		return new LibVLCLightPlayer(url_);
	} catch (jexception::Exception &e) {
	}
#endif

#if defined(LIBAV_MEDIA)
	try {
		return new LibAVLightPlayer(url_);
	} catch (jexception::Exception &e) {
	}
#endif
	
#if defined(LIBXINE_MEDIA)
	try {
		return new LibXineLightPlayer(url_);
	} catch (jexception::Exception &e) {
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


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
#include "jmedia/jaudiomixercontrol.h"

namespace jmedia {

Audio::Audio():
  jcommon::Object()
{
	jcommon::Object::SetClassName("jmedia::Audio");
}

Audio::~Audio()
{
}

bool Audio::IsLoopEnabled()
{
  return false;
}

float Audio::GetVolume()
{
  return 0.0f;
}

void Audio::SetLoopEnabled(bool enabled)
{
}

void Audio::SetVolume(float volume)
{
}


AudioMixerControl::AudioMixerControl():
	Control("audio.mixer")
{
	jcommon::Object::SetClassName("jmedia::AudioMixerControl");
}
		
AudioMixerControl::~AudioMixerControl()
{
}

Audio * AudioMixerControl::CreateAudio(std::string filename)
{
  return nullptr;
}

void AudioMixerControl::StartSound(Audio *audio)
{
}

void AudioMixerControl::StopSound(Audio *audio)
{
}

}

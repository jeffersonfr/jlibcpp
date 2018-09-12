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
#include "jmedia/binds/mixer/include/mixerlightplayer.h"
#include "jmedia/jaudiomixercontrol.h"
#include "jio/jfileinputstream.h"
#include "jexception/jmediaexception.h"
#include "jexception/jcontrolexception.h"

#include <fcntl.h>

#include <SDL2/SDL.h>

#define AUDIO_MAX_SOUNDS 32

// ffmpeg -i in.mp3 -acodec pcm_s16le -ac 2 -ar 48000 out.wav

namespace jmedia {

class AudioImpl : public jmedia::Audio {
  
  public:
		/** \brief */
    float _volume;
		/** \brief */
    uint32_t _buffer_index;
		/** \brief */
    uint32_t _buffer_size;
		/** \brief */
    uint8_t _fade;
		/** \brief */
    uint8_t *_buffer;
		/** \brief */
    bool _is_loop_enabled;
		/** \brief */
    bool _is_reference;

  public:
    AudioImpl()
    {
      _buffer_index = 0;
      _buffer_size = 0;
      _buffer = nullptr;
      _fade = 0;
      _is_loop_enabled = false;
      _volume = 1.0f;
      _is_reference = false;
    }

    virtual ~AudioImpl()
    {
      if (_is_reference == false) {
        SDL_FreeWAV(_buffer);
      }
    }

    virtual bool IsLoopEnabled()
    {
      return _is_loop_enabled;
    }

    virtual float GetVolume()
    {
      return _volume;
    }

    virtual void SetLoopEnabled(bool enabled)
    {
      _is_loop_enabled = enabled;
    }

    virtual void SetVolume(float volume)
    {
      _volume = volume;

      if (_volume < 0.0f) {
        _volume = 0.0f;
      }

      if (_volume > 1.0f) {
        _volume = 1.0f;
      }
    }

};

class AudioMixerControlImpl : public AudioMixerControl {
	
	private:
		/** \brief */
    std::vector<Audio *> _audios;
		/** \brief */
		MixerLightPlayer *_player;
		/** \brief */
    SDL_AudioDeviceID _device;

	private:
    static void AudioCallback(void *userdata, uint8_t *stream, int len)
    {
      AudioMixerControlImpl *mixer = reinterpret_cast<AudioMixerControlImpl *>(userdata);
      int tempLength;
      bool music = false;

      memset(stream, 0, len);

      for (std::vector<Audio *>::iterator i=mixer->_audios.begin(); i!=mixer->_audios.end(); ) {
        AudioImpl *audio = static_cast<AudioImpl *>(*i);

        if (audio->_buffer_index > 0) {
          int volume = (int)(audio->GetVolume()*SDL_MIX_MAXVOLUME);

          if (audio->_fade == 1 && audio->IsLoopEnabled() == true) {
            music = true;

            if (volume > 0) {
              volume--;
            } else {
              audio->_buffer_index = 0;
            }
          }

          if (music && audio->IsLoopEnabled() == true && audio->_fade == 0) {
            tempLength = 0;
          } else {
            tempLength = ((uint32_t)len > audio->_buffer_index) ? audio->_buffer_index : (uint32_t)len;
          }

          SDL_MixAudioFormat(stream, audio->_buffer + audio->_buffer_size - audio->_buffer_index, 32784, tempLength, volume);

          audio->_buffer_index -= tempLength;

          i++;
        } else if(audio->_fade == 0 && audio->IsLoopEnabled() == true) {
          audio->_buffer_index = audio->_buffer_size;

          i++;
        } else {
          i = mixer->_audios.erase(i);

          delete audio;
          audio = nullptr;
        }
      }
    }

    void PlayAudio(Audio *audio)
    {
      if (audio == nullptr) {
        return;
      }

      if (audio->IsLoopEnabled() == false) {
        if (_audios.size() >= AUDIO_MAX_SOUNDS) {
          return;
        }
      }

      SDL_LockAudioDevice(_device);

      if (audio->IsLoopEnabled() == true) {
        AddMusic(audio);
      } else {
        AddAudio(audio);
      }

      SDL_UnlockAudioDevice(_device);
    }

    void AddMusic(Audio *audio)
    {
      bool found = false;

      for (std::vector<Audio *>::iterator i=_audios.begin(); i!=_audios.end(); i++) {
        AudioImpl *audio = static_cast<AudioImpl *>(*i);

        if (audio->IsLoopEnabled() == true) {
          if (audio->_fade == 0) {
            if (found == true) {
              audio->_buffer_index = 0;
              audio->_volume = 0;
            }

            audio->_fade = 1;
          } else if(audio->_fade == 1) {
            found = true;
          }
        }
      }

      AddAudio(audio);
    }

    void AddAudio(Audio *audio)
    {
      AudioImpl *tmp = new AudioImpl;

      *tmp = *static_cast<AudioImpl *>(audio);
    
      tmp->_fade = 1;
      tmp->_buffer_index = tmp->_buffer_size;
      tmp->_is_reference = true;

      _audios.push_back(tmp);
    }

	public:
		AudioMixerControlImpl(MixerLightPlayer *player):
			AudioMixerControl()
		{
			_player = player;
      _device = 0;
		}

		virtual ~AudioMixerControlImpl()
		{
		}

    virtual void PauseDevice()
    {
      if (_device > 0) {
        SDL_PauseAudioDevice(_device, 1);
      }
    }

    virtual void ResumeDevice()
    {
      if (_device > 0) {
        SDL_PauseAudioDevice(_device, 0);
      }
    }

    virtual void CloseDevice()
    {
      if (_device > 0) {
        SDL_CloseAudioDevice(_device);
      }
    }

    virtual Audio * CreateAudio(std::string filename)
    {
      AudioImpl *audio = new AudioImpl;
      SDL_AudioSpec spec;

      SDL_memset(&spec, 0, sizeof(spec));

      if (SDL_LoadWAV(filename.c_str(), &spec, &(audio->_buffer), &(audio->_buffer_size)) == nullptr) {
        delete audio;
        audio = nullptr;

        return nullptr;
      }

      audio->_fade = 0;

      audio->_buffer_index = audio->_buffer_size;

      // INFO:: create the audio _device with the specification of the first audio file
      if (_device == 0) {
        spec.callback = AudioCallback;
        spec.userdata = this;

        if ((_device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE)) == 0) {
		      throw jexception::MediaException("Unable to open the audio device");
        }

        SDL_PauseAudioDevice(_device, 0);
      }

      return audio;
    }

    void StartSound(Audio *audio)
    {
      PlayAudio(audio);
    }

    void StopSound(Audio *audio)
    {
      // TODO:: remomver o audio da lista
    }

};

MixerLightPlayer::MixerLightPlayer(jnetwork::URL url):
	jmedia::Player()
{
  if (strcasecmp(url.GetProtocol().c_str(), "mixer") != 0) {
		throw jexception::MediaException("Invalid argument");
  }
      
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		throw jexception::MediaException("Unable to initialize the audio system");
  }

  if ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) == 0) {
		throw jexception::MediaException("Unable to initialize the audio system");
  }

	_controls.push_back(new AudioMixerControlImpl(this));
}

MixerLightPlayer::~MixerLightPlayer()
{
	Close();
	
	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();
}

void MixerLightPlayer::Play()
{
  // std::unique_lock<std::mutex> lock(_mutex);
      
  Pause();
}

void MixerLightPlayer::Pause()
{
  AudioMixerControlImpl *impl = static_cast<AudioMixerControlImpl *>(_controls[0]);

  impl->PauseDevice();
}

void MixerLightPlayer::Resume()
{
  AudioMixerControlImpl *impl = static_cast<AudioMixerControlImpl *>(_controls[0]);

  impl->ResumeDevice();
}

void MixerLightPlayer::Stop()
{
}

void MixerLightPlayer::Close()
{
  AudioMixerControlImpl *impl = static_cast<AudioMixerControlImpl *>(_controls[0]);

  impl->CloseDevice();
  // SDL_Quit();
}

void MixerLightPlayer::SetCurrentTime(uint64_t time)
{
}

uint64_t MixerLightPlayer::GetCurrentTime()
{
	uint64_t time = 0LL;

	return time;
}

uint64_t MixerLightPlayer::GetMediaTime()
{
	uint64_t time = 0LL;

	return time;
}

void MixerLightPlayer::SetLoop(bool b)
{
}

bool MixerLightPlayer::IsLoop()
{
	return false;
}

void MixerLightPlayer::SetDecodeRate(double rate)
{
}

double MixerLightPlayer::GetDecodeRate()
{
	return 1.0;
}

jgui::Component * MixerLightPlayer::GetVisualComponent()
{
	return nullptr;
}

}


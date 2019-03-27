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
#include "mixerlightplayer.h"

#include "jmedia/jaudiomixercontrol.h"
#include "jio/jmemoryinputstream.h"
#include "jexception/jmediaexception.h"
#include "jexception/jcontrolexception.h"

#include <fcntl.h>

#include <SDL2/SDL.h>

#define AUDIO_MAX_SOUNDS 32

// ffmpeg -i in.mp3 -acodec pcm_s16le -ac 2 -ar 48000 out.wav

namespace jmedia {

struct jaudio_device_t {
  SDL_AudioDeviceID device;
  SDL_AudioSpec spec;
  SDL_AudioSpec wanted;
  int reference;
};

std::vector<jaudio_device_t> g_audio_devices;

SDL_AudioDeviceID CreateAudioDevice(SDL_AudioSpec wanted)
{
  for (std::vector<struct jaudio_device_t>::const_iterator i=g_audio_devices.begin(); i!=g_audio_devices.end(); i++) {
    struct jaudio_device_t t = *i;

    if (t.spec.format == wanted.format and t.spec.channels == wanted.channels and t.spec.freq == wanted.freq) {
      t.reference++;

      return t.device;
    }
  }

  for (std::vector<struct jaudio_device_t>::const_iterator i=g_audio_devices.begin(); i!=g_audio_devices.end(); i++) {
    struct jaudio_device_t t = *i;

    if (t.wanted.format == wanted.format and t.wanted.channels == wanted.channels and t.wanted.freq == wanted.freq) {
      t.reference++;

      return t.device;
    }
  }

  SDL_AudioDeviceID
    device;
  SDL_AudioSpec 
    spec;

  device = SDL_OpenAudioDevice(nullptr, 0, &wanted, &spec, SDL_AUDIO_ALLOW_ANY_CHANGE);

  if (device == 0) {
    return 0;
  }

  SDL_PauseAudioDevice(device, 0);

  struct jaudio_device_t t = {
    .device = device,
    .spec = spec,
    .wanted = wanted,
    .reference = 1
  };

  g_audio_devices.push_back(t);

  return device;
}

void DestroyAudioDevice(SDL_AudioDeviceID id)
{
  for (std::vector<struct jaudio_device_t>::const_iterator i=g_audio_devices.begin(); i!=g_audio_devices.end(); i++) {
    struct jaudio_device_t t = *i;

    if (t.device == id) {
      t.reference--;

      if (t.reference == 0) {
        SDL_CloseAudioDevice(t.device);
      }

      g_audio_devices.erase(i);

      break;
    }
  }
}

class MixerAudioImpl : public jmedia::Audio {
  
  public:
		/** \brief */
    jio::InputStream *_stream;
		/** \brief */
    float _volume;
		/** \brief */
    uint8_t *_buffer;
		/** \brief */
    uint8_t _fade;
		/** \brief */
    bool _is_loop;
		/** \brief */
    bool _is_local;

  public:
    MixerAudioImpl()
    {
      _stream = nullptr;
      _buffer = nullptr;
      _fade = 0;
      _is_loop = false;
      _volume = 1.0f;
      _is_local = false;
    }

    virtual ~MixerAudioImpl()
    {
      if (_is_local == true) {
        delete _stream;
        _stream = nullptr;

        SDL_FreeWAV(_buffer);
        _buffer = nullptr;
      }
    }

    virtual bool IsLoopEnabled()
    {
      return _is_loop;
    }

    virtual float GetVolume()
    {
      return _volume;
    }

    virtual void SetLoopEnabled(bool enabled)
    {
      _is_loop = enabled;
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

class MixerAudioMixerControlImpl : public AudioMixerControl {
	
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
      MixerAudioMixerControlImpl *mixer = reinterpret_cast<MixerAudioMixerControlImpl *>(userdata);
      bool music = false;

      memset(stream, 0, len);

      for (std::vector<Audio *>::iterator i=mixer->_audios.begin(); i!=mixer->_audios.end(); ) {
        MixerAudioImpl *audio = static_cast<MixerAudioImpl *>(*i);

        if (audio->_stream->Available() > 0) {
          int volume = (int)(audio->GetVolume()*SDL_MIX_MAXVOLUME);

          if (audio->_fade == 1 && audio->IsLoopEnabled() == true) {
            music = true;

            if (volume > 0) {
              volume--;
            } else {
              audio->_stream->Skip(audio->_stream->Available());
            }
          }

          if (music == false or audio->IsLoopEnabled() == false or audio->_fade != 0) {
            char 
              data[len];
            int 
              length = audio->_stream->Read(data, len);

            if (length > 0) {
              SDL_MixAudioFormat(stream, (uint8_t *)data, 32784, length, volume);
            }
          }

          i++;
        } else if(audio->_fade == 0 && audio->IsLoopEnabled() == true) {
          audio->_stream->Reset();

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
        MixerAudioImpl *audio = static_cast<MixerAudioImpl *>(*i);

        if (audio->IsLoopEnabled() == true) {
          if (audio->_fade == 0) {
            if (found == true) {
              audio->_stream->Skip(audio->_stream->Available());
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
      MixerAudioImpl *tmp = new MixerAudioImpl;

      *tmp = *static_cast<MixerAudioImpl *>(audio);
    
      tmp->_fade = 1;
      tmp->_stream->Reset();
      tmp->_is_local = false;

      _audios.push_back(tmp);
    }

	public:
		MixerAudioMixerControlImpl(MixerLightPlayer *player):
			AudioMixerControl()
		{
			_player = player;
      _device = 0;
		}

		virtual ~MixerAudioMixerControlImpl()
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
        DestroyAudioDevice(_device);
      }
    }

    virtual Audio * CreateAudio(std::string filename)
    {
      MixerAudioImpl 
        *audio = new MixerAudioImpl;
      SDL_AudioSpec 
        spec;

      SDL_memset(&spec, 0, sizeof(spec));

      Uint8 
        *buffer = nullptr;
      Uint32
        length = 0;

      if (SDL_LoadWAV(filename.c_str(), &spec, &buffer, &length) == nullptr) {
        delete audio;
        audio = nullptr;

        return nullptr;
      }

      audio->_buffer = buffer;
      audio->_fade = 0;
      audio->_stream = new jio::MemoryInputStream(buffer, length);
      audio->_is_local = true;

      spec.callback = AudioCallback;
      spec.userdata = this;

      _device = CreateAudioDevice(spec);

      if (_device == 0) {
        throw jexception::MediaException("Unable to open the audio device");
      }

      return audio;
    }

    virtual Audio * CreateAudio(jio::InputStream *stream, jaudio_format_t format, int frequency, int channels)
    {
      MixerAudioImpl 
        *audio = new MixerAudioImpl;
      SDL_AudioSpec 
        spec;

      SDL_memset(&spec, 0, sizeof(spec));

      audio->_fade = 0;
      audio->_stream = stream;
      audio->_is_local = false;

      if (format == JAF_AUDIO_S8) {
        spec.format = AUDIO_S8;
      } else if (format == JAF_AUDIO_U8) {
        spec.format = JAF_AUDIO_U8;
      } else if (format == JAF_AUDIO_S16LSB) {
        spec.format = JAF_AUDIO_S16LSB;
      } else if (format == JAF_AUDIO_S16MSB) {
        spec.format = AUDIO_S16MSB;
      } else if (format == JAF_AUDIO_S16SYS) {
        spec.format = AUDIO_S16SYS;
      } else if (format == JAF_AUDIO_S16) {
        spec.format = AUDIO_S16;
      } else if (format == JAF_AUDIO_U16LSB) {
        spec.format = AUDIO_U16LSB;
      } else if (format == JAF_AUDIO_U16MSB) {
        spec.format = AUDIO_U16MSB;
      } else if (format == JAF_AUDIO_U16) {
        spec.format = AUDIO_U16;
      } else if (format == JAF_AUDIO_S32LSB) {
        spec.format = AUDIO_S32LSB;
      } else if (format == JAF_AUDIO_S32MSB) {
        spec.format = AUDIO_S32MSB;
      } else if (format == JAF_AUDIO_S32SYS) {
        spec.format = AUDIO_S32SYS;
      } else if (format == JAF_AUDIO_S32) {
        spec.format = AUDIO_S32;
      } else if (format == JAF_AUDIO_F32LSB) {
        spec.format = AUDIO_F32LSB;
      } else if (format == JAF_AUDIO_F32MSB) {
        spec.format = AUDIO_F32MSB;
      } else if (format == JAF_AUDIO_F32SYS) {
        spec.format = AUDIO_F32SYS;
      } else if (format == JAF_AUDIO_F32) {
        spec.format = AUDIO_F32;
      }

      spec.freq = frequency; // is->sdl_sample_rate;
      spec.channels = channels; // is->sdl_channels;
      spec.silence = 0;
      spec.samples = 4096; // SDL_AUDIO_BUFFER_SIZE;

      spec.callback = AudioCallback;
      spec.userdata = this;

      _device = CreateAudioDevice(spec);

      if (_device == 0) {
        throw jexception::MediaException("Unable to open the audio device");
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

	_controls.push_back(new MixerAudioMixerControlImpl(this));
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
  MixerAudioMixerControlImpl *impl = static_cast<MixerAudioMixerControlImpl *>(_controls[0]);

  impl->PauseDevice();
}

void MixerLightPlayer::Resume()
{
  MixerAudioMixerControlImpl *impl = static_cast<MixerAudioMixerControlImpl *>(_controls[0]);

  impl->ResumeDevice();
}

void MixerLightPlayer::Stop()
{
}

void MixerLightPlayer::Close()
{
  MixerAudioMixerControlImpl *impl = static_cast<MixerAudioMixerControlImpl *>(_controls[0]);

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


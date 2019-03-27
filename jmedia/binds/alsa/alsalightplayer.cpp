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
#include "alsalightplayer.h"

#include "jmedia/jvolumecontrol.h"
#include "jio/jfileinputstream.h"
#include "jexception/jmediaexception.h"
#include "jexception/jcontrolexception.h"

#define ALSA_PLAYER_DEVICE_NAME "default"
#define ALSA_PLAYER_MIXER_NAME "Master"
#define ALSA_PLAYER_MIXER_INDEX 0
#define ALSA_PLAYER_BUFFER 1024

namespace jmedia {

class AlsaVolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		AlsaLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;
		/** \brief */
		snd_mixer_t *_mixer;
		/** \brief */
		snd_mixer_elem_t *_elem;
		/** \brief */
		snd_mixer_selem_id_t *_sid;

	public:
		AlsaVolumeControlImpl(AlsaLightPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;
			_is_muted = false;
			
			snd_mixer_selem_id_alloca(&_sid);

			// sets simple-mixer index and name
			snd_mixer_selem_id_set_index(_sid, ALSA_PLAYER_MIXER_INDEX);
			snd_mixer_selem_id_set_name(_sid, ALSA_PLAYER_MIXER_NAME);

			if ((snd_mixer_open(&_mixer, 0)) < 0) {
				_mixer = nullptr;
				return;
			}

			if ((snd_mixer_attach(_mixer, ALSA_PLAYER_DEVICE_NAME)) < 0) {
				snd_mixer_close(_mixer);
				_mixer = nullptr;

				return;
			}

			if ((snd_mixer_selem_register(_mixer, nullptr, nullptr)) < 0) {
				snd_mixer_close(_mixer);
				_mixer = nullptr;

				return;
			}

			if (snd_mixer_load(_mixer) < 0) {
				snd_mixer_close(_mixer);
				_mixer = nullptr;
			
				return;
			}

			_elem = snd_mixer_find_selem(_mixer, _sid);
			
			if (!_elem) {
				snd_mixer_close(_mixer);
				_mixer = nullptr;

				return;
			}

			SetLevel(100);
		}

		virtual ~AlsaVolumeControlImpl()
		{
			snd_mixer_close(_mixer);
		}

		virtual int GetLevel()
		{
			long level = 0;
			
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_mixer != nullptr) {
				long minv, maxv;

				snd_mixer_selem_get_playback_volume_range(_elem, &minv, &maxv);

				printf("Volume range <%ld,%ld>\n", minv, maxv);

				if (snd_mixer_selem_get_playback_volume(_elem, SND_MIXER_SCHN_MONO, &level) >= 0) {
					level = (100*(level-minv))/(maxv-minv); // make the value bound from 0 to 100
				}
			}

			return (int)level;
		}

		virtual void SetLevel(int level)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			_level = (level < 0)?0:(level > 100)?100:level;

			if (_mixer != nullptr) {
				long minv, maxv;

				snd_mixer_selem_get_playback_volume_range(_elem, &minv, &maxv);

				printf("Volume range <%ld,%ld>\n", minv, maxv);
				
				level = (level * (maxv - minv) / (100 - 1)) + minv;

				if (snd_mixer_selem_set_playback_volume_all(_elem, level) < 0) {
					return;
				}
			}
		}
		
		virtual bool IsMute()
		{
			return _is_muted;
		}

		virtual void SetMute(bool b)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);
	
			_is_muted = b;
			
			if (_is_muted == true) {
				int level = _level;

				SetLevel(0);

				_level = level;
			} else {
				SetLevel(_level);
			}
		}

};

static bool load_wave_params(const char *file, uint32_t *channels_, uint32_t *bits_per_sample_, uint32_t *sample_rate_)
{
	int fd;

	fd = open(file, O_RDONLY);

	if (fd < 0) {
		printf("Unable to open file\n");

		return false;
	}

	uint32_t size, format_length, sample_rate, avg_bytes_sec; // our 32 bit values
	uint16_t format_tag, channels, block_align, bits_per_sample; // our 16 values
	uint8_t id[4];

	if (read(fd, id, sizeof(uint8_t)*4) <= 0) { // read in first four bytes
		return false;
	}
	if (!strcmp((const char *)id, (const char *)"RIFF")) { // we had 'RIFF' let's continue
		if (read(fd, &size, sizeof(uint32_t)) <= 0) { // read in 32bit size value
			return false;
		}
		if (read(fd, id, sizeof(uint8_t)*4) <= 0) { // read in 4 byte string now
			return false;
		}
		if (!strcmp((const char *)id, (const char *)"WAVE")) { // this is probably a wave file since it contained "WAVE"
			if (read(fd, id, sizeof(uint8_t)*4) <= 0) { // read in 4 bytes "fmt ";
				return false;
			}
			if (read(fd, &format_length, sizeof(uint32_t)) <= 0) {
				return false;
			}
			if (read(fd, &format_tag, sizeof(uint16_t)) <= 0) { // check mmreg.h (i think?) for other
				return false;
			}
			if (format_tag == 0x0001) { // WAVE_FORMAT_PCM
				// that's ok !
			} else if (format_tag == 0x0003) { // WAVE_FORMAT_IEEE_FLOAT
				return false;
			} else if (format_tag == 0x0006) { // WAVE_FORMAT_ALAW
				return false;
			} else if (format_tag == 0x0007) { // WAVE_FORMAT_MULAW
				return false;
			} else if (format_tag == 0xfffe) { // WAVE_FORMAT_EXTENSIBLE
				return false;
			} else {
				return false;
			}
			// possible format tags like ADPCM
			if (read(fd, &channels, sizeof(uint16_t)) <= 0) { // 1 mono, 2 stereo
				return false;
			}
			if (read(fd, &sample_rate, sizeof(uint32_t)) <= 0) { // like 44100, 22050, etc...
				return false;
			}
			if (read(fd, &avg_bytes_sec, sizeof(uint32_t)) <= 0) { // probably won't need this
				return false;
			}
			if (read(fd, &block_align, sizeof(uint16_t)) <= 0) { // probably won't need this
				return false;
			}
			if (read(fd, &bits_per_sample, sizeof(uint16_t)) <= 0) { // 8 bit or 16 bit file?
				return false;
			}
			if (read(fd, id, sizeof(uint8_t)*2) <= 0) { // size of the extension (0 or 22)
				return false;
			}
			if (read(fd, id, sizeof(uint8_t)*2) <= 0) { // number of valid bits
				return false;
			}
			if (read(fd, id, sizeof(uint8_t)*4) <= 0) { // channel position mask
				return false;
			}

			*channels_ = channels;
			*bits_per_sample_ = bits_per_sample;
			*sample_rate_ = sample_rate;

			return true;
		} else {
			// printf("Error: RIFF file but not a wave file\n");
		}
	} else {
		// printf("Error: not a RIFF file\n");
	}

	return false;
}

AlsaLightPlayer::AlsaLightPlayer(jnetwork::URL url):
	jmedia::Player()
{
	_file = url.GetPath();
	_media_time = 0LL;
	_decode_rate = 1.0;
	_is_loop = false;
	_stream = nullptr;
	_pcm_handle = nullptr;
	_params = nullptr;
	_buffer = nullptr;
	_buffer_length = 0;
	_sample_rate = 0;
	_bit_depth = 0;
	_channels = 0;
	_is_closed = false;
	_is_playing = false;

	_stream = new jio::FileInputStream(_file);

	if (load_wave_params(_file.c_str(), &_channels, &_bit_depth, &_sample_rate) == false) {
		throw jexception::MediaException("Unable to open a wav file");
	}

	int pcm;

	if ((pcm = snd_pcm_open(&_pcm_handle, ALSA_PLAYER_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		throw jexception::MediaException("Unable to open the default pcm device");
	}

	snd_pcm_hw_params_alloca(&_params);
	snd_pcm_hw_params_any(_pcm_handle, _params);

	if ((pcm = snd_pcm_hw_params_set_access(_pcm_handle, _params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		throw jexception::MediaException("Cannot set interleaved mode");
	}

	if ((pcm = snd_pcm_hw_params_set_format(_pcm_handle, _params, SND_PCM_FORMAT_S16_LE)) < 0) {
		throw jexception::MediaException("Cannot set the PCM_S16_LE format");
	}

	if ((pcm = snd_pcm_hw_params_set_channels(_pcm_handle, _params, _channels)) < 0) {
		throw jexception::MediaException("Cannot set the channels number");
	}

	if ((pcm = snd_pcm_hw_params_set_rate_near(_pcm_handle, _params, &_sample_rate, 0)) < 0) {
		throw jexception::MediaException("Cannot set the rate");
	}

	if ((pcm = snd_pcm_hw_params(_pcm_handle, _params)) < 0) {
		throw jexception::MediaException("Cannot set the hardware parameters");
	}

	snd_pcm_hw_params_get_period_size(_params, &_frames, 0);

	printf("Alsa Player:: dev-name:[%s], dev-state:[%s], channels:[%d], bit-depth:[%d], sample-rate:[%d]\n", 
			snd_pcm_name(_pcm_handle), snd_pcm_state_name(snd_pcm_state(_pcm_handle)), _channels, _bit_depth, _sample_rate);

	_buffer_length = _frames * _channels * _bit_depth / 8;

	_buffer = (uint8_t *)malloc(_buffer_length);

	_component = new jgui::Component();

	_controls.push_back(new AlsaVolumeControlImpl(this));
}

AlsaLightPlayer::~AlsaLightPlayer()
{
	Close();
	
	delete _stream;
	_stream = nullptr;

	delete _component;
	_component = nullptr;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();
}

void AlsaLightPlayer::Play()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_pcm_handle != nullptr && _is_playing == false) {
    _thread = std::thread(&AlsaLightPlayer::Run, this);
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_STARTED));
	}
}

void AlsaLightPlayer::Pause()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_pcm_handle != nullptr && snd_pcm_state(_pcm_handle) == SND_PCM_STATE_RUNNING) {
		snd_pcm_pause(_pcm_handle, 1);
	
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_PAUSED));
	}
}

void AlsaLightPlayer::Resume()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_pcm_handle != nullptr && snd_pcm_state(_pcm_handle) == SND_PCM_STATE_PAUSED) {
		snd_pcm_pause(_pcm_handle, 0);
	
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_RESUMED));
	}
}

void AlsaLightPlayer::Stop()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_pcm_handle != nullptr) {
		snd_pcm_state_t state = snd_pcm_state(_pcm_handle);
		
		if (
				// state == SND_PCM_STATE_OPEN ||
				// state == SND_PCM_STATE_SETUP ||
				// state == SND_PCM_STATE_PREPARED ||
				state == SND_PCM_STATE_RUNNING ||
				state == SND_PCM_STATE_XRUN ||
				state == SND_PCM_STATE_DRAINING ||
				state == SND_PCM_STATE_PAUSED ||
				state == SND_PCM_STATE_SUSPENDED ||
				// state == SND_PCM_STATE_DISCONNECTED
				0) {
			snd_pcm_drop(_pcm_handle);
			snd_pcm_reset(_pcm_handle);
		}
	}

	if (_is_playing == true) {
		_is_playing = false;

    _thread.join();
	}

	_stream->Reset();
}

void AlsaLightPlayer::Close()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_pcm_handle != nullptr) {
		snd_pcm_close(_pcm_handle);
	}
}

void AlsaLightPlayer::SetCurrentTime(uint64_t time)
{
	double period;
	
  std::unique_lock<std::mutex> lock(_mutex);

	period = _channels * _sample_rate * _bit_depth * 60.0 / 8;

	if (period != 0) {
		_stream->Reset();
		_stream->Skip(time*period/(60*1000LL));
	}
}

uint64_t AlsaLightPlayer::GetCurrentTime()
{
	uint64_t time = 0LL;
	double period;

  std::unique_lock<std::mutex> lock(_mutex);

	period = _channels * _sample_rate * _bit_depth * 60.0 / 8;

	if (period != 0) {
		time = (int)(60.0*(_stream->GetSize()-_stream->Available())/period) * 1000LL;
	}

	return time;
}

uint64_t AlsaLightPlayer::GetMediaTime()
{
	uint64_t time = 0LL;
	double period;
	
	period = _channels * _sample_rate * _bit_depth * 60.0 / 8;

	if (period != 0) {
		time = (int)(60.0*_stream->GetSize()/period) * 1000LL;
	}

	return time;
}

void AlsaLightPlayer::SetLoop(bool b)
{
  std::unique_lock<std::mutex> lock(_mutex);

	_is_loop = b;
}

bool AlsaLightPlayer::IsLoop()
{
	return _is_loop;
}

void AlsaLightPlayer::SetDecodeRate(double rate)
{
  std::unique_lock<std::mutex> lock(_mutex);

	_decode_rate = rate;

	if (_decode_rate == 0.0) {
		Pause();
	} else {
		Resume();
	}
}

double AlsaLightPlayer::GetDecodeRate()
{
	return _decode_rate;
}

jgui::Component * AlsaLightPlayer::GetVisualComponent()
{
	return _component;
}

void AlsaLightPlayer::Run()
{
	_stream->Reset();

	// CHANGE:: skip initial noise
	_stream->Skip(128); 

	_is_playing = true;

	snd_pcm_prepare(_pcm_handle);
	snd_pcm_reset(_pcm_handle);
	snd_pcm_start(_pcm_handle);

	int r;

	do {
		if ((r = _stream->Read((char *)_buffer, _buffer_length)) <= 0) {
			snd_pcm_drain(_pcm_handle);

			if (_is_loop == true) {
				_stream->Reset();

				continue;
			}

			break;
		}

		if ((r = snd_pcm_writei(_pcm_handle, _buffer, _frames)) == -EPIPE) {
			snd_pcm_prepare(_pcm_handle);
		}

		if (r < 0) {
			break;
		}
	} while (_is_playing == true);

	DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_FINISHED));
}

}


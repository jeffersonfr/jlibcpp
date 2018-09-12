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
#include "jmedia/jsynthesizer.h"
#include "jexception/jruntimeexception.h"

#include <thread>

#include <math.h>

namespace jmedia {

double sawtooth_wave(double a) 
{
	a = fmod(a, 2*M_PI);

	return a/2/M_PI*2 - 1;
}

double triangle_wave(double a) 
{
	a = fmod(a, 2*M_PI);

	if (a > M_PI) {
		return (2*M_PI-a)/M_PI*2 - 1;
	}

	return a/M_PI*2 - 1;
}

double square_wave(double a) 
{
	a = fmod(a, 2.0*M_PI);

	return (a < 0.0)?-1.0:1.0;
}

double sine_wave(double a) 
{
	return sin(a);
}

double noise_wave(double a) 
{
	return 2*(double)rand()/RAND_MAX - 1;
}

double silence_wave(double a)
{
	return 0;
}

Synthesizer::Synthesizer(std::string device_name, int channels, int sample_rate):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jmedia::Synthesizer");

	if (channels < 0) {
		throw jexception::RuntimeException("Invalid number of channels");
	}

	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	int err;

	_device_name = device_name; // "default", "plughw:0,0";
	_output = nullptr;
	_function = square_wave;
	_format = SND_PCM_FORMAT_S16;
	_sample_rate = sample_rate;
	_channels = channels;
	_buffer_time = 1000;
	_buffer_size = 1000;
	_period_size = 1000;
	_volume = 1.0;

	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	if ((err = snd_output_stdio_attach(&_output, stdout, 0)) < 0) {
		throw jexception::RuntimeException("Output failed");
	}

	if ((err = snd_pcm_open(&_handle, _device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		throw jexception::RuntimeException("Playback open error");
	}

	if ((err = SetAudioParameters(_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		throw jexception::RuntimeException("Setting of hwparams failed");
	}

	if ((err = SetAudioParameters(_handle, swparams)) < 0) {
		throw jexception::RuntimeException("Setting of swparams failed");
	}

	// printf("Playback device is %s, Stream parameters are %iHz, %s, %i channels\n", _device_name.c_str(), _sample_rate, snd_pcm_format_name(_format), _channels);
}

Synthesizer::~Synthesizer()
{
	snd_pcm_drain(_handle);
	snd_pcm_close(_handle);
}

void Synthesizer::GenerateSamples(int16_t *samples, int channel, double frequency, int count, double *_phase) 
{
	double phase = *_phase;
	double max_phase = 1.0 / frequency;
	double step = 1.0 / (double)_sample_rate;
	int ires;

	while (count-- > 0) {
		double res = _function((phase * 2 * M_PI) / max_phase - M_PI) * 32767 * _volume;

		ires = res;

		for (int i=0; i<_channels; i++) {
			if (i == channel) {
				*samples++ = ires;
			} else {
				*samples++ = 0;
			}
		}

		phase += step;
		if (phase >= max_phase)
			phase -= max_phase;
	}

	*_phase = phase;
}

int Synthesizer::SetAudioParameters(snd_pcm_t *handle, snd_pcm_hw_params_t *params, snd_pcm_access_t access)
{
	uint32_t sample_rate;
	int err, dir;
	snd_pcm_uframes_t period_size_min;
	snd_pcm_uframes_t period_size_max;
	snd_pcm_uframes_t buffer_size_min;
	snd_pcm_uframes_t buffer_size_max;
	snd_pcm_uframes_t buffer_time_to_size;

	/* choose all parameters */
	err = snd_pcm_hw_params_any(handle, params);
	if (err < 0) {
		// printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));

		return err;
	}

	/* set the interleaved read/write format */
	err = snd_pcm_hw_params_set_access(handle, params, access);
	if (err < 0) {
		// printf("Access type not available for playback: %s\n", snd_strerror(err));

		return err;
	}

	err = snd_pcm_hw_params_set_format(handle, params, _format);
	if (err < 0) {
		return err;
	}

	err = snd_pcm_hw_params_set_channels(handle, params, _channels);
	if (err < 0) {
		// printf("Channels count (%i) not available for playbacks: %s\n", _channels, snd_strerror(err));

		return err;
	}

	sample_rate = _sample_rate;
	err = snd_pcm_hw_params_set_rate(handle, params, _sample_rate, 0);
	if (err < 0) {
		// printf("Rate %iHz not available for playback: %s\n", _sample_rate, snd_strerror(err));

		return err;
	}

	if (sample_rate != _sample_rate) {
		// printf("Rate doesn't match (requested %iHz, get %iHz, err %d)\n", _sample_rate, sample_rate, err);

		return -EINVAL;
	}

	// printf("Rate set to %iHz (requested %iHz)\n", sample_rate, _sample_rate);

	/* set the buffer time */
	buffer_time_to_size = ( (snd_pcm_uframes_t)_buffer_time * _sample_rate) / 1000000;
	err = snd_pcm_hw_params_get_buffer_size_min(params, &buffer_size_min);
	err = snd_pcm_hw_params_get_buffer_size_max(params, &buffer_size_max);
	dir=0;
	err = snd_pcm_hw_params_get_period_size_min(params, &period_size_min,&dir);
	dir=0;
	err = snd_pcm_hw_params_get_period_size_max(params, &period_size_max,&dir);

	// printf("Buffer size range from %lu to %lu\n",buffer_size_min, buffer_size_max);
	// printf("Period size range from %lu to %lu\n",period_size_min, period_size_max);
	// printf("Buffer time size %lu\n",buffer_time_to_size);

	_buffer_size = buffer_time_to_size;

	if (buffer_size_max < _buffer_size) {
		_buffer_size = buffer_size_max;
	}

	if (buffer_size_min > _buffer_size) {
		_buffer_size = buffer_size_min;
	}

	_period_size = _buffer_size/8;
	_buffer_size = _period_size*8;

	// printf("To choose buffer_size = %lu\n", _buffer_size);
	// printf("To choose period_size = %lu\n", _period_size);

	dir=0;
	err = snd_pcm_hw_params_set_period_size_near(handle, params, &_period_size, &dir);
	if (err < 0) {
		// printf("Unable to set period size %lu for playback: %s\n", _period_size, snd_strerror(err));

		return err;
	}
	dir=0;
	err = snd_pcm_hw_params_get_period_size(params, &_period_size, &dir);
	if (err < 0) {
		// printf("Unable to get period size for playback: %s\n", snd_strerror(err));
	}

	dir=0;
	err = snd_pcm_hw_params_set_buffer_size_near(handle, params, &_buffer_size);
	if (err < 0) {
		// printf("Unable to set buffer size %lu for playback: %s\n", _buffer_size, snd_strerror(err));

		return err;
	}

	err = snd_pcm_hw_params_get_buffer_size(params, &_buffer_size);

	// printf("was set period_size = %lu\n", _period_size);
	// printf("was set buffer_size = %lu\n", _buffer_size);

	if (2*_period_size > _buffer_size) {
		// printf("buffer to small, could not use\n");

		return err;
	}

	/* write the parameters to device */
	err = snd_pcm_hw_params(handle, params);
	if (err < 0) {
		// printf("Unable to set hw params for playback: %s\n", snd_strerror(err));

		return err;
	}

	return 0;
}

int Synthesizer::SetAudioParameters(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams) 
{
	int err;

	/* get the current swparams */
	err = snd_pcm_sw_params_current(handle, swparams);
	if (err < 0) {
		// printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));

		return err;
	}

	/* start the transfer when a period is full */
	err = snd_pcm_sw_params_set_start_threshold(handle, swparams, _period_size);
	if (err < 0) {
		// printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));

		return err;
	}

	/* allow the transfer when at least period_size samples can be processed */
	err = snd_pcm_sw_params_set_avail_min(handle, swparams, _period_size);
	if (err < 0) {
		// printf("Unable to set avail min for playback: %s\n", snd_strerror(err));

		return err;
	}

	/* write the parameters to the playback device */
	err = snd_pcm_sw_params(handle, swparams);
	if (err < 0) {
		// printf("Unable to set sw params for playback: %s\n", snd_strerror(err));

		return err;
	}

	return 0;
}

int Synthesizer::Underflow(snd_pcm_t *handle, int err) 
{
	if (err == -EPIPE) {	/* under-run */
		err = snd_pcm_prepare(handle);
		if (err < 0) {
			// printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
		}

		return 0;
	} else if (err == -ESTRPIPE) {
		while ((err = snd_pcm_resume(handle)) == -EAGAIN) {
			// wait until the suspend flag is released
      std::this_thread::sleep_for(std::chrono::seconds((1)));
		}

		if (err < 0) {
			err = snd_pcm_prepare(handle);
			if (err < 0) {
				// printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
			}
		}

		return 0;
	}

	return err;
}

int Synthesizer::WriteSamples(snd_pcm_t *handle, int channel, double frequency, double periods, int16_t *samples)
{
	double phase = 0;
	int16_t *ptr;
	int err, cptr, n;

	for(n = 0; n < (int)periods; n++) {
		GenerateSamples(samples, channel, frequency, _period_size, &phase);
		ptr = samples;
		cptr = _period_size;

		while (cptr > 0) {
			err = snd_pcm_writei(handle, ptr, cptr);

			if (err == -EAGAIN)
				continue;

			if (err < 0) {
				if (Underflow(handle, err) < 0) {
					// printf("Write error: %s\n", snd_strerror(err));

					return -1;
				}

				break;	/* skip one period */
			}

			ptr += (err * _channels);
			cptr -= err;
		}
	}

	return 0;
}

void Synthesizer::SetWave(double (* function)(double))
{
	_function = function;
}

void * Synthesizer::GetWave()
{
	return (void *)_function;
}

void Synthesizer::SetVolume(int volume)
{
	if (volume < 0) {
		volume = 0;
	}

	if (volume > 100) {
		volume = 100;
	}

	_volume = volume/100.0;
}

int Synthesizer::GetVolume()
{
	return (int)(_volume * 100.0);
}

void Synthesizer::Play(int channel, double frequency, double duration)
{
	int16_t *samples;

	samples = (short int *)malloc((_period_size*_channels*snd_pcm_format_width(_format))/8);

	if (samples == nullptr) {
		return;
	}

	WriteSamples(_handle, channel, frequency, duration*_sample_rate/(double)_period_size, samples);

	free(samples);
}

}

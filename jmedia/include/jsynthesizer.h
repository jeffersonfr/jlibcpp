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
#ifndef J_SYNTHESIZER_H
#define J_SYNTHESIZER_H

#include "jcommon/jobject.h"

#include <alsa/asoundlib.h>

namespace jmedia {

class Synthesizer : public virtual jcommon::Object {

	private:
		/* \brief playback device */
		std::string _device_name;
		/* \brief sample format */
		snd_pcm_format_t _format;
		/* \brief stream rate */
		uint32_t _sample_rate;
		/* \brief ring buffer length in us */
		uint32_t _buffer_time;
		/* \brief count of channels */
		int _channels;
		/* \brief */
		snd_output_t *_output;
		/* \brief */
		snd_pcm_uframes_t _buffer_size;
		/* \brief */
		snd_pcm_uframes_t _period_size;
		/* \brief */
		snd_pcm_t *_handle;
		/* \brief waveform of sound */
		double (* _function)(double);
		/* \brief */
		double _volume;

	private:
		/**
		 * \brief
		 *
		 */
		void GenerateSamples(int16_t *samples, int channel, double frequency, int count, double *_phase); 

		/**
		 * \brief
		 *
		 */
		int SetAudioParameters(snd_pcm_t *handle, snd_pcm_hw_params_t *params, snd_pcm_access_t access);

		/**
		 * \brief
		 *
		 */
		int SetAudioParameters(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams);

		/**
		 * \brief
		 *
		 */
		int Underflow(snd_pcm_t *handle, int err);

		/**
		 * \brief
		 *
		 */
		int WriteSamples(snd_pcm_t *handle, int channel, double frequency, double periods, int16_t *samples);

	public:
		/**
		 * \brief
		 *
		 */
		Synthesizer(std::string device_name = std::string("default"), int channels = 1, int sample_rate = 8192);

		/**
		 * \brief
		 *
		 */
		virtual ~Synthesizer();

		/**
		 * \brief
		 *
		 */
		virtual void SetWave(double (* function)(double));

		/**
		 * \brief
		 *
		 */
		virtual void *GetWave();

		/**
		 * \brief
		 *
		 */
		virtual void SetVolume(int volume);

		/**
		 * \brief
		 *
		 */
		virtual int GetVolume();

		/** 
		 * \brief Plays a beep in the selected channel.
		 *
		 * \param channel Individual channel
		 * \param frequency Value of frequency
		 * \param duration Delay in milliseconds
		 */
		virtual void Play(int channel, double frequency, double duration);

};

}

#endif

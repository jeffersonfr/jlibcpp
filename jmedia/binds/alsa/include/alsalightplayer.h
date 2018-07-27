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
#ifndef J_ALSALIGHTPLAYER_H
#define J_ALSALIGHTPLAYER_H

#include "jmedia/jplayer.h"
#include "jgui/jcomponent.h"
#include "jio/jinputstream.h"

#include <thread>
#include <mutex>

#include <alsa/asoundlib.h>

namespace jmedia {

class AlsaLightPlayer : public jmedia::Player {

	public:
		/** \brief */
    std::thread _thread;
		/** \brief */
    std::mutex _mutex;
		/** \brief */
		std::string _file;
		/** \brief */
		jgui::Component *_component;
		/** \brief */
		double _decode_rate;
		/** \brief */
		uint64_t _media_time;
		/** \brief */
		jio::InputStream *_stream;
		/** \brief */
		snd_pcm_t *_pcm_handle;
		/** \brief */
		snd_pcm_hw_params_t *_params;
		/** \brief */
		snd_pcm_uframes_t _frames;
		/** \brief */
		uint8_t *_buffer;
		/** \brief */
		uint32_t _buffer_length;
		/** \brief */
		uint32_t _sample_rate;
		/** \brief */
		uint32_t _bit_depth;
		/** \brief */
		uint32_t _channels;
		/** \brief */
		bool _is_closed;
		/** \brief */
		bool _is_playing;
		/** \brief */
		bool _is_loop;

	public:
		/**
		 * \brief
		 *
		 */
		AlsaLightPlayer(std::string file);

		/**
		 * \brief
		 *
		 */
		virtual ~AlsaLightPlayer();

		/**
		 * \brief
		 *
		 */
		virtual void Play();

		/**
		 * \brief
		 *
		 */
		virtual void Pause();

		/**
		 * \brief
		 *
		 */
		virtual void Stop();

		/**
		 * \brief
		 *
		 */
		virtual void Resume();

		/**
		 * \brief
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentTime(uint64_t i);

		/**
		 * \brief
		 *
		 */
		virtual uint64_t GetCurrentTime();

		/**
		 * \brief
		 *
		 */
		virtual uint64_t GetMediaTime();

		/**
		 * \brief
		 *
		 */
		virtual void SetLoop(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsLoop();
		
		/**
		 * \brief
		 *
		 */
		virtual double GetDecodeRate();

		/**
		 * \brief
		 *
		 */
		virtual void SetDecodeRate(double rate);

		/**
		 * \brief
		 *
		 */
		virtual jgui::Component * GetVisualComponent();

		/**
		 * \brief
		 *
		 */
		virtual void Run();

};

}

#endif

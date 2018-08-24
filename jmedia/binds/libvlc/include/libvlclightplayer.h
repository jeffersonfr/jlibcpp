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
#ifndef J_LIBVLCLIGHTPLAYER_H
#define J_LIBVLCLIGHTPLAYER_H

#include "jmedia/jplayer.h"
#include "jgui/jcomponent.h"
#include "jnetwork/jurl.h"

#include <thread>
#include <mutex>

struct libvlc_instance_t;
struct libvlc_media_player_t;
struct libvlc_event_manager_t;

namespace jmedia {

class LibVLCLightPlayer : public jmedia::Player {

	public:
		/** \brief */
		libvlc_instance_t *_engine;
		/** \brief */
		libvlc_media_player_t *_provider;
		/** \brief */
		libvlc_event_manager_t *_event_manager;
		/** \brief */
		std::string _file;
		/** \brief */
		jgui::Component *_component;
		/** \brief */
		double _aspect;
		/** \brief */
		double _frames_per_second;
		/** \brief */
		double _decode_rate;
		/** \brief */
		uint64_t _media_time;
		/** \brief */
		bool _is_paused;
		/** \brief */
		bool _is_closed;
		/** \brief */
		bool _is_loop;
		/** \brief */
		bool _has_audio;
		/** \brief */
		bool _has_video;

	private:
		/**
		 * \brief Loops a video at the end of media.
		 *
		 */
		virtual void Run();

	public:
		/**
		 * \brief
		 *
		 */
		LibVLCLightPlayer(jnetwork::URL url);

		/**
		 * \brief
		 *
		 */
		virtual ~LibVLCLightPlayer();

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

};

}

#endif

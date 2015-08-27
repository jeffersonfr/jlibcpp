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
#ifndef J_NATIVEHEAVYPLAYER_H
#define J_NATIVEHEAVYPLAYER_H

#include "jplayer.h"
#include "jthread.h"
#include "jcomponent.h"

#include <directfb.h>

namespace jmedia {

class DirectFBHeavyPlayer : public jmedia::Player, public jthread::Thread {

	public:
		/** \brief */
		IDirectFBVideoProvider *_provider;
		/** \brief */
		IDirectFBEventBuffer *_events;
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		std::string _file;
		/** \brief */
		jmedia::Control *_video_size;
		/** \brief */
		jmedia::Control *_video_format;
		/** \brief */
		jgui::Component *_component;
		/** \brief */
		double _aspect;
		/** \brief */
		double _decode_rate;
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
		 * \brief
		 *
		 */
		static void Callback(void *ctx);
		
	public:
		/**
		 * \brief
		 *
		 */
		DirectFBHeavyPlayer(std::string file);

		/**
		 * \brief
		 *
		 */
		virtual ~DirectFBHeavyPlayer();

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

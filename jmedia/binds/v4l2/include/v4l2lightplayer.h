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
#ifndef J_V4L2LIGHTPLAYER_H
#define J_V4L2LIGHTPLAYER_H

#include "jmedia/binds/v4l2/include/videograbber.h"
#include "jmedia/jplayer.h"
#include "jgui/jcomponent.h"

#include <mutex>

namespace jmedia {

class V4L2LightPlayer : public jmedia::Player, public jmedia::V4LFrameListener {

	public:
		/** \brief */
    std::mutex _mutex;
		/** \brief */
		std::string _file;
		/** \brief */
		jgui::Component *_component;
		/** \brief */
		double _aspect;
		/** \brief */
		double _decode_rate;
		/** \brief */
		double _frame_rate;
		/** \brief */
		uint64_t _media_time;
		/** \brief */
		bool _is_paused;
		/** \brief */
		bool _is_loop;
		/** \brief */
		bool _is_closed;
		/** \brief */
		bool _has_audio;
		/** \brief */
		bool _has_video;
		/** \brief */
		VideoGrabber *_grabber;

	private:
		/**
		 * \brief
		 *
		 */
		virtual void ProcessFrame(const uint8_t *buffer, int width, int height, jgui::jpixelformat_t format);

	public:
		/**
		 * \brief
		 *
		 */
		V4L2LightPlayer(std::string file);

		/**
		 * \brief
		 *
		 */
		virtual ~V4L2LightPlayer();

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

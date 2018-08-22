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
#ifndef J_ANIMATION_H
#define J_ANIMATION_H

#include "jgui/jcomponent.h"
#include "jgui/jimage.h"

#include <mutex>
#include <vector>
#include <thread>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Animation : public Component {

	private:
		/** \brief */
		std::vector<Image *> _images;
		/** \brief */
		std::mutex _animation_mutex;
		/** \brief */
    std::thread _thread;
		/** \brief */
		int _index;
		/** \brief */
		int _interval;
		/** \brief */
		bool _running;

	public:
		/**
		 * \brief
		 *
		 */
		Animation(int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Animation();

		/**
		 * \brief
		 *
		 */
		virtual void SetInterval(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void AddImage(jgui::Image *image);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveImage(jgui::Image *image);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveAll();
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<jgui::Image *> & GetImages();
		
		/**
		 * \brief
		 *
		 */
		virtual void Start();
		
		/**
		 * \brief
		 *
		 */
		virtual void Stop();
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void Run();

};

}

#endif


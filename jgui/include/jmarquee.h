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
#ifndef J_MARQUEE_H
#define J_MARQUEE_H

#include "jgui/jcomponent.h"

#include <thread>
#include <mutex>

namespace jgui {

/**
 * \brief
 *
 */
enum jmarquee_mode_t {
	JMM_LOOP,
	JMM_BOUNCE
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Marquee : public Component {

	private:
		/** \brief */
    std::thread _thread;
		/** \brief */
		std::mutex _marquee_mutex;
		/** \brief */
		std::string _text;
		/** \brief */
		jmarquee_mode_t _type;
		/** \brief */
		int _position;
		/** \brief */
		int _interval;
		/** \brief */
		int _index;
		/** \brief */
		int _step;
		/** \brief */
		bool _running;

	public:
		/**
		 * \brief
		 *
		 */
		Marquee(std::string text, int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Marquee();

		/**
		 * \brief
		 *
		 */
		virtual void SetType(jmarquee_mode_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetStep(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetInterval(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetText(std::string text);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetText();
		
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


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
#ifndef J_ALLEGRO5APPLICATION_H
#define J_ALLEGRO5APPLICATION_H

#include "jgui/japplication.h"
#include "jgui/jwindow.h"

#include <map>
#include <condition_variable>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Allegro5Application : public jgui::Application {

	private:
		/** \brief */
		struct cursor_params_t {
			Image *cursor;
			int hot_x;
			int hot_y;
		};

		/** \brief */
		std::map<jcursor_style_t, struct cursor_params_t> _cursors;
		/** \brief */
		std::condition_variable _init_sem;
		/** \brief */
		std::condition_variable _exit_sem;

	private:
		/**
		 * \brief
		 *
		 */
		void InternalInitCursors();

		/**
		 * \brief
		 *
		 */
		void InternalReleaseCursors();

		/**
		 * \brief
		 *
		 */
		void InternalPaint();

		/**
		 * \brief
		 *
		 */
		virtual void InternalInit(int argc = 0, char **argv = NULL);

		/**
		 * \brief
		 *
		 */
		virtual void InternalLoop();

		/**
		 * \brief
		 *
		 */
		virtual void InternalQuit();

	public:
		/**
		 * \brief
		 *
		 */
		Allegro5Application();
		
		/**
		 * \brief
		 *
		 */
		virtual ~Allegro5Application();

};

}

#endif

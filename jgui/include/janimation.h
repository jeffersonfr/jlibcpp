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

#include "jruntimeexception.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jthread.h"

#include "jtextlistener.h"
#include "jcomponent.h"
#include "joffscreenimage.h"

#include <iostream>
#include <cstdlib>
#include <vector>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Animation : public Component, public jthread::Thread{

	private:
		std::vector<OffScreenImage *> _images;
		int _index,
			_interval;
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
		void AddImage(std::string file);
		
		/**
		 * \brief
		 *
		 */
		void SetInterval(int i);
		
		/**
		 * \brief
		 *
		 */
		void RemoveAll();
		
		/**
		 * \brief
		 *
		 */
		void Release();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool b);
		
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


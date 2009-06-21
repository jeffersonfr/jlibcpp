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
#ifndef J_OFFSCREENIMAGE_H
#define J_OFFSCREENIMAGE_H

#include "jgraphics.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

class Graphics;

class OffScreenImage : public virtual jcommon::Object{

	protected:
		Graphics *graphics;
		int _width,
			_height,
			_scale_width,
			_scale_height;

	public:
		OffScreenImage(int width, int height, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		virtual ~OffScreenImage();

		Graphics * GetGraphics();

		int GetWidth();
		int GetHeight();

		int GetScaleWidth();
		int GetScaleHeight();

		void Release();
		void Restore();

};

}

#endif 

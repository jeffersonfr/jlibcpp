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
#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include "jbutton.h"
#include "joffscreenimage.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class ButtonListener;
class ButtonEvent;

class ImageButton : public Button{

	protected:
		OffScreenImage *prefetch1,
			  *prefetch2;
		std::string _image,
			_image_focus;

	public:
		ImageButton(std::string image, std::string label, int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~ImageButton();

		void SetImage(std::string image);
		void SetImageFocus(std::string image);
		
		virtual void SetSize(int width, int height);
		virtual void Paint(Graphics *g);

};

}

#endif


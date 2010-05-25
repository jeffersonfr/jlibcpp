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
#ifndef J_IMAGEBUTTON_H
#define J_IMAGEBUTTON_H

#include "jbutton.h"
#include "joffscreenimage.h"

#include <string>

#include <stdlib.h>

namespace jgui {

class ButtonListener;
class ButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ImageButton : public Button{

	protected:
		OffScreenImage *_image_icon,
			*_image_focus_icon;
		std::string _image,
			_image_focus;

	public:
		/**
		 * \brief
		 *
		 */
		ImageButton(std::string label, std::string image, int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ImageButton();

		/**
		 * \brief
		 *
		 */
		virtual void SetImage(std::string image);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetImageFocus(std::string image);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif


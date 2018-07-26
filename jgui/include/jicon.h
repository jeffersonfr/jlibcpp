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
#ifndef J_ICON_H
#define J_ICON_H

#include "jgui/jcomponent.h"
#include "jgui/jimage.h"

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Icon : public jgui::Component {

	private:
		/** \brief */
		jgui::Image *_image;
		/** \brief */
		jhorizontal_align_t _halign;
		/** \brief */
		jvertical_align_t _valign;

	public:
		/**
		 * \brief
		 *
		 */
		Icon(jgui::Image *image, int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Icon();

		/**
		 * \brief
		 *
		 */
		virtual void SetImage(jgui::Image *image);

		/**
		 * \brief
		 *
		 */
		virtual void SetHorizontalAlign(jhorizontal_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jhorizontal_align_t GetHorizontalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalAlign(jvertical_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jvertical_align_t GetVerticalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif 

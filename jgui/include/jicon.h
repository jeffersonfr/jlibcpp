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

#include "jcomponent.h"
#include "jimage.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Icon : public jgui::Component{

	private:
		jgui::Image *_image;
		std::string _file,
			_text;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		bool _wrap;

	public:
		/**
		 * \brief
		 *
		 */
		Icon(std::string, int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Icon();

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
		virtual void SetImage(std::string file);

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

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
#ifndef J_WIDGET_H
#define J_WIDGET_H

#include "jcontainer.h"
#include "jimage.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Widget : public jgui::Container{

	protected:
		/**
		 * \brief
		 *
		 */
		struct frame_subtitle_t {
			jgui::Image *image;
			std::string subtitle;
		};

	protected:
		/** \brief */
		std::vector<frame_subtitle_t> _subtitles;
		/** \brief */
		jgui::Image *_icon;
		/** \brief */
		std::string _title;

	public:
		/**
		 * \brief
		 *
		 */
		Widget(int x = 0, int y = 0, int width = -1, int height = -1);
		
		/**
		 * \brief
		 *
		 */
		Widget(std::string title, int x = 0, int y = 0, int width = -1, int height = -1);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Widget();

		/**
		 * \brief
		 *
		 */
		virtual void SetTitle(std::string title);

		/**
		 * \brief
		 *
		 */
		virtual std::string GetTitle();

		/**
		 * \brief
		 *
		 */
		virtual void SetIcon(std::string icon);

		/**
		 * \brief
		 *
		 */
		virtual void SetIcon(jgui::Image *image);

		/**
		 * \brief
		 *
		 */
		virtual jgui::Image * GetIcon();

		/**
		 * \brief
		 *
		 */
		virtual void AddSubtitle(std::string image, std::string label);

		/**
		 * \brief
		 *
		 */
		virtual void RemoveAllSubtitles();

		/**
		 * \brief
		 *
		 */
		virtual void PaintGlassPane(Graphics *g);

};

}

#endif 

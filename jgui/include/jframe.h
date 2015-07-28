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
#ifndef J_FRAME_H
#define J_FRAME_H

#include "jkeylistener.h"
#include "jmouselistener.h"
#include "jinputmanager.h"
#include "jwindow.h"
#include "jfont.h"
#include "jcontainer.h"
#include "jicon.h"
#include "jthread.h"
#include "jmutex.h"
#include "jdate.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jframe_button_t {
	JFB_MINIMIZE	= 1,
	JFB_MAXIMIZE	= 2,
	JFB_CLOSE 		= 4
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Frame : public Window {

	friend class Component;

	private:
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
		jthread::Mutex _paint_mutex;
		/** \brief */
		Image *_icon_image;
		/** \brief */
		Image *_icon_close;
		/** \brief */
		Image *_icon_maximize;
		/** \brief */
		Image *_icon_restore;
		/** \brief */
		std::string _title;
		/** \brief */
		std::string _icon;
		/** \brief */
		int _init_type;
		/** \brief */
		int _frame_state;
		/** \brief */
		jframe_button_t _frame_buttons;

	public:
		/**
		 * \brief
		 *
		 */
		Frame(std::string title, int x = 0, int y = 0, int width = -1, int height = -1);
		
		/**
		 * \brief
		 *
		 */
		Frame(int x = 0, int y = 0, int width = -1, int height = -1);

		/**
		 * \brief
		 *
		 */
		virtual ~Frame();

		/**
		 * \brief
		 *
		 */
		virtual void Pack(bool fit = true);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetIcon();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetIcon(std::string icon);

		/**
		 * \brief
		 *
		 */
		virtual jframe_button_t GetFrameButtons();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetFrameButtons(jframe_button_t buttons);

		/**
		 * \brief
		 *
		 */
		virtual void SetDefaultExitEnabled(bool b);
		
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
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void PaintScrollbars(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void PaintGlassPane(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyReleased(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyTyped(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);

};

}

#endif 

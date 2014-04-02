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
class Frame : public KeyListener, public MouseListener, public Window {

	friend class Component;

	private:
		/**
		 * \brief
		 *
		 */
		struct frame_subtitle_t {
			std::string image;
			std::string subtitle;
		};

	private:
		/**
		 * \brief
		 *
		 */
		virtual void KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void MouseWheel(MouseEvent *event);

	protected:
		std::vector<frame_subtitle_t> _subtitles;
		jthread::Mutex _input_mutex,
			_paint_mutex;
		jthread::Condition _frame_sem;
		std::string _title,
			_icon;
		int _init_type;
		int _old_x,
				_old_y,
				_old_width,
				_old_height;
		bool _input_enabled,
				 _background_visible,
				 _move_enabled,
				 _release_enabled,
				 _resize_enabled,
				 _is_maximized;
		jframe_button_t _frame_buttons;
		jkeyevent_symbol_t _last_key_code;

	public:
		/**
		 * \brief
		 *
		 */
		Frame(std::string title, int x, int y, int width, int height, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		Frame(int x, int y, int width, int height, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);

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
		virtual void Maximize();
		
		/**
		 * \brief
		 *
		 */
		virtual void Restore();

		/**
		 * \brief
		 *
		 */
		virtual bool Hide();
		
		/**
		 * \brief
		 *
		 */
		virtual bool Show(bool modal = true);

		/**
		 * \brief
		 *
		 */
		virtual void Release();

		/**
		 * \brief
		 *
		 */
		virtual jinsets_t GetInsets();

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
		virtual void SetMoveEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetResizeEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsMoveEnabled();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsResizeEnabled();
		
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
		virtual void SetInputEnabled(bool b);
		
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
		virtual jkeyevent_symbol_t GetLastKeyCode();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLastKeyCode(jkeyevent_symbol_t key);

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void PaintGlassPane(Graphics *g);

};

}

#endif 

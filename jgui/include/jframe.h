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
#include "jframeinputlistener.h"
#include "jicon.h"
#include "jthread.h"
#include "jmutex.h"
#include "jdate.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

enum jframe_buttons_t {
	FB_NONE = 0,
	FB_MAXIMIZE = 1,
	FB_RELEASE = 2
};

class Frame : public KeyListener, public MouseListener, public Window {

	friend class Component;

	private:
		struct frame_subtitle_t {
			std::string image;
			std::string subtitle;
		};

	private:
		virtual void KeyPressed(KeyEvent *event);

		virtual void MousePressed(MouseEvent *event);
		virtual void MouseReleased(MouseEvent *event);
		virtual void MouseClicked(MouseEvent *event);
		virtual void MouseMoved(MouseEvent *event);
		virtual void MouseWheel(MouseEvent *event);

	protected:
		jthread::Mutex _input_mutex,
			_paint_mutex;
		jthread::Condition _frame_sem;

		std::vector<FrameInputListener *> _key_listeners;
		std::vector<frame_subtitle_t> _subtitles;

		std::string _title,
			_icon;
		int _init_type,
			_relative_mouse_x,
			_relative_mouse_y,
			_relative_mouse_w,
			_relative_mouse_h,
			_mouse_state,
			_frame_buttons;
		int _old_x,
				_old_y,
				_old_width,
				_old_height;
		bool _input_locked,
			 _input_enabled,
			 _background_visible,
			 _default_exit,
			 _move_enabled,
			 _resize_enabled,
			 _is_maximized;
		jkey_symbol_t _last_key_code;
		jcursor_style_t _default_cursor;

	public:
		Frame(std::string title, int x, int y, int width, int height, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		virtual ~Frame();

		virtual void Pack();
		virtual void Maximize();
		virtual void Restore();

		virtual bool Hide();
		virtual bool Show(bool modal = true);

		virtual void Release();

		virtual jinsets_t GetInsets();

		virtual std::string GetIcon();
		virtual void SetIcon(std::string icon);

		virtual int GetFrameButtons();
		virtual void SetFrameButtons(int buttons);

		virtual void SetMoveEnabled(bool b);
		virtual void SetResizeEnabled(bool b);
		virtual bool IsMoveEnabled();
		virtual bool IsResizeEnabled();
		virtual void SetDefaultExitEnabled(bool b);
		virtual void SetTitle(std::string title);
		virtual std::string GetTitle();
		virtual void SetInputEnabled(bool b);
		virtual void AddSubtitle(std::string image, std::string label);
		virtual void RemoveAllSubtitles();

		virtual void RequestComponentFocus(jgui::Component *c);
		virtual void ReleaseComponentFocus(jgui::Component *c);
		virtual jgui::Component * GetComponentInFocus();

		virtual jkey_symbol_t GetLastKeyCode();
		virtual void SetLastKeyCode(jkey_symbol_t key);

		virtual void Paint(Graphics *g);

		void RegisterInputListener(FrameInputListener *listener);
		void RemoveInputListener(FrameInputListener *listener);
		std::vector<FrameInputListener *> & GetFrameInputListeners();

};

}

#endif 

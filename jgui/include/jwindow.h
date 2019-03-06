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
#ifndef J_WINDOW_H
#define J_WINDOW_H

#include "jgui/jcontainer.h"
#include "jgui/jimage.h"
#include "jgui/jgridlayout.h"
#include "jgui/jeventmanager.h"
#include "jevent/jwindowlistener.h"

#include <thread>

namespace jgui{

enum jwindow_state_t {
	JWS_NORMAL,
	JWS_MAXIMIZE,
	JWS_MINIMIZE,
  JWS_FULLSCREEN
};

enum jwindow_rotation_t {
	JWR_NONE,
	JWR_90,
	JWR_180,
	JWR_270
};

enum jcursor_style_t {
	JCS_DEFAULT,
	JCS_CROSSHAIR,
	JCS_EAST,
	JCS_WEST,
	JCS_NORTH,
	JCS_SOUTH,
	JCS_HAND,
	JCS_MOVE,
	JCS_NS,
	JCS_WE,
	JCS_NW_CORNER,
	JCS_NE_CORNER,
	JCS_SW_CORNER,
	JCS_SE_CORNER,
	JCS_TEXT,
	JCS_WAIT
};

class Graphics;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Window : public jgui::Container {

  /**
   * \brief
   *
   */
  struct frame_subtitle_t {
    jgui::Image *image;
    std::string subtitle;
  };

	private:
		/** \brief */
    std::vector<jevent::EventObject *> _window_events;
		/** \brief */
	  std::vector<jevent::KeyListener *> _key_listeners;
		/** \brief */
	  std::vector<jevent::MouseListener *> _mouse_listeners;
		/** \brief */
		std::vector<jevent::WindowListener *> _window_listeners;
		/** \brief */
		std::vector<struct frame_subtitle_t> _subtitles;
		/** \brief */
		std::thread _exec_thread;
		/** \brief */
 	  std::mutex _event_mutex;
		/** \brief */
 	  std::condition_variable _event_condition;
		/** \brief */
 	  std::mutex _key_listener_mutex;
		/** \brief */
 	  std::mutex _mouse_listener_mutex;
		/** \brief */
		std::mutex _paint_mutex;
		/** \brief */
		std::mutex _window_listener_mutex;
    /** \brief */
    Window *_instance;
    /** \brief */
    Font *_font;
    /** \brief */
    EventManager *_event_manager;
		/** \brief */
		Component *_focus_owner;
    /** \brief */
    Theme _theme;

	public:
		/**
		 * \brief
		 *
		 */
		Window(Window *window);
		
		/**
		 * \brief
		 *
		 */
		Window(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		Window(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Window();

		/**
		 * \brief
		 *
		 */
		virtual EventManager * GetEventManager();
		
		/**
		 * \brief Entry point to applications.
		 *
		 */
		virtual void ShowApp();

		/**
		 * \brief
		 *
		 */
		virtual void Exec();

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
    jgui::Component * GetFocusOwner();
    /**
     * \brief
     *
     */
    void RequestComponentFocus(jgui::Component *c);
    /**
     * \brief
     *
     */
    void ReleaseComponentFocus(jgui::Component *c);
    /**
     * \brief
     *
     */
    Container * GetFocusCycleRootAncestor();

    /**
		 * \brief
		 *
		 */
		virtual void AddSubtitle(jgui::Image *image, std::string label);

		/**
		 * \brief
		 *
		 */
		virtual void RemoveAllSubtitles();

		/**
		 * \brief
		 *
		 */
		virtual void ToggleFullScreen();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetOpacity(float opacity);
		
		/**
		 * \brief
		 *
		 */
		virtual float GetOpacity();
		
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
		virtual void SetResizable(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsResizable();

		/**
		 * \brief
		 *
		 */
		virtual void SetUndecorated(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsUndecorated();

		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual jgui::jregion_t GetVisibleBounds();

		/**
		 * \brief
		 *
		 */
		virtual void Repaint(Component *cmp = nullptr);
		
		/**
		 * \brief
		 *
		 */
		virtual void PaintBackground(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void PaintGlassPane(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

		/**
		 * \brief Called after all drawings have been synchronized.
		 *
		 */
		virtual void Flush();

		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool visible);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsVisible();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursorLocation(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetCursorLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursorEnabled(bool enable);

		/**
		 * \brief
		 *
		 */
		virtual bool IsCursorEnabled();

		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(jcursor_style_t cursor);

		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(Image *shape, int hotx, int hoty);

		/**
		 * \brief
		 *
		 */
		virtual jcursor_style_t GetCursor();

		/**
		 * \brief
		 *
		 */
		virtual void SetRotation(jwindow_rotation_t t);

		/**
		 * \brief
		 *
		 */
		virtual jwindow_rotation_t GetRotation();

		/**
		 * \brief
		 *
		 */
    virtual void PushEvent(jevent::EventObject *event);

		/**
		 * \brief
		 *
		 */
    virtual std::vector<jevent::EventObject *> GrabEvents();

		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyReleased(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyTyped(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(jevent::MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(jevent::MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(jevent::MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(jevent::MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void RegisterKeyListener(jevent::KeyListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveKeyListener(jevent::KeyListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual const std::vector<jevent::KeyListener *> & GetKeyListeners();
		
		/**
		 * \brief
		 *
		 */
		virtual void RegisterMouseListener(jevent::MouseListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveMouseListener(jevent::MouseListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual const std::vector<jevent::MouseListener *> & GetMouseListeners();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterWindowListener(jevent::WindowListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveWindowListener(jevent::WindowListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchWindowEvent(jevent::WindowEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual const std::vector<jevent::WindowListener *> & GetWindowListeners();

};

}

#endif 

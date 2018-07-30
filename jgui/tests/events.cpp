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
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jcommon/jdate.h"

#include <algorithm>
#include <iostream>

#define LONG_PRESS_TIME	2000

enum userevent_type_t {
	ON_KEY_DOWN_EVENT,
	ON_KEY_UP_EVENT,
	ON_KEY_PRESS_EVENT,
	ON_KEY_LONGPRESS_EVENT,
	ON_MOUSE_PRESS_EVENT,
	ON_MOUSE_RELEASE_EVENT,
	ON_MOUSE_MOVE_EVENT,
	ON_MOUSE_WHEEL_EVENT,
	ON_MOUSE_CLICK_EVENT
};

class UserEvent {

	private:
		userevent_type_t _type;
		// \brief key event
		jevent::jkeyevent_symbol_t _symbol;
		jevent::jkeyevent_modifiers_t _mod;
		// \brief mouse events
		jevent::jmouseevent_button_t _button;
		int _code;
		int _x;
		int _y;
		double _vx;
		double _vy;
		int _click_count;

	public:
		UserEvent(userevent_type_t type, jevent::jkeyevent_modifiers_t mod, int code, jevent::jkeyevent_symbol_t symbol)
		{
			_type = type;

			_code = code;
			_symbol = symbol;
			_mod = mod;
		}

		UserEvent(userevent_type_t type, jevent::jmouseevent_button_t button, int click_count, int x, int y, double vx, double vy)
		{
			_type = type;

			_button = button;
			_click_count = click_count;
			_x = x;
			_y = y;
			_vx = vx;
			_vy = vy;
		}

		virtual ~UserEvent()
		{
		}

		userevent_type_t GetType()
		{
			return _type;
		}

		int GetKeySymbol()
		{
			return _symbol;
		}

		int GetKeyCode()
		{
			return _code;
		}

		jevent::jkeyevent_modifiers_t GetModifiers()
		{
			return _mod;
		}

		jevent::jmouseevent_button_t GetButton()
		{
			return _button;
		}

		int GetClickCount()
		{
			return _click_count;
		}

		int GetX()
		{
			return _x;
		}

		int GetY()
		{
			return _y;
		}

		double GetVelocityX()
		{
			return _vx;
		}

		double GetVelocityY()
		{
			return _vy;
		}

};

class UserEventListener {

	protected:
		UserEventListener()
		{
		}

	public:
		virtual ~UserEventListener()
		{
		}

		virtual void OnKeyDown(UserEvent *event)
		{
		}

		virtual void OnKeyPress(UserEvent *event)
		{
		}

		virtual void OnKeyUp(UserEvent *event)
		{
		}

		virtual void OnKeyLongPress(UserEvent *event)
		{
		}

		virtual void OnMousePress(UserEvent *event)
		{
		}

		virtual void OnMouseRelease(UserEvent *event)
		{
		}

		virtual void OnMouseClick(UserEvent *event)
		{
		}

		virtual void OnMouseMove(UserEvent *event)
		{
		}

		virtual void OnMouseWheel(UserEvent *event)
		{
		}

};

class UserEventManager : public jevent::KeyListener, public jevent::MouseListener {

	private:
		struct event_t {
			bool key_down;
			long start_time;
		};

	private:
		static UserEventManager *_instance;

		std::map<jevent::jkeyevent_symbol_t, event_t *> _events;
		std::vector<UserEventListener *> _user_listeners;
		jgui::jpoint_t _last_mouse_location;
		uint64_t _last_mouse_move;

	private:
		UserEventManager()
		{
			_last_mouse_move = 0LL;
			_last_mouse_location.x = 0LL;
			_last_mouse_location.y = 0LL;
		}

		void DispatchUserEvent(UserEvent *event)
		{
			if (event == NULL) {
				return;
			}

			int k = 0,
					size = (int)_user_listeners.size();

			while (k++ < (int)_user_listeners.size()) {
				UserEventListener *listener = _user_listeners[k-1];

				if (event->GetType() == ON_KEY_DOWN_EVENT) {
					listener->OnKeyDown(event);
				} else if (event->GetType() == ON_KEY_UP_EVENT) {
					listener->OnKeyUp(event);
				} else if (event->GetType() == ON_KEY_PRESS_EVENT) {
					listener->OnKeyPress(event);
				} else if (event->GetType() == ON_KEY_LONGPRESS_EVENT) {
					listener->OnKeyLongPress(event);
				} else if (event->GetType() == ON_MOUSE_PRESS_EVENT) {
					listener->OnMousePress(event);
				} else if (event->GetType() == ON_MOUSE_RELEASE_EVENT) {
					listener->OnMouseRelease(event);
				} else if (event->GetType() == ON_MOUSE_MOVE_EVENT) {
					listener->OnMouseMove(event);
				} else if (event->GetType() == ON_MOUSE_WHEEL_EVENT) {
					listener->OnMouseWheel(event);
				} else if (event->GetType() == ON_MOUSE_CLICK_EVENT) {
					listener->OnMouseClick(event);
				}

				if (size != (int)_user_listeners.size()) {
					size = (int)_user_listeners.size();

					k--;
				}
			}

			delete event;
		}

	public:
		virtual ~UserEventManager()
		{
		}

		static UserEventManager * GetInstance()
		{
			if ((void *)_instance == NULL) {
				_instance = new UserEventManager();
			}

			return _instance;
		}

		void RegisterUserEventListener(UserEventListener *listener)
		{
			if (listener == NULL) {
				return;
			}

			if (std::find(_user_listeners.begin(), _user_listeners.end(), listener) == _user_listeners.end()) {
				_user_listeners.push_back(listener);
			}
		}

		void RemoveUserEventListener(UserEventListener *listener)
		{
			if (listener == NULL) {
				return;
			}

			std::vector<UserEventListener *>::iterator i = std::find(_user_listeners.begin(), _user_listeners.end(), listener);

			if (i != _user_listeners.end()) {
				_user_listeners.erase(i);
			}
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			struct event_t *t = _events[event->GetSymbol()];

			if ((void *)t == NULL) {
				t = new struct event_t;

				t->key_down = false;
				t->start_time = -1L;
			}

			if (t->key_down == false) {
				_events[event->GetSymbol()] = t;

				t->key_down = true;
				t->start_time = jcommon::Date::CurrentTimeMillis();

				DispatchUserEvent(new UserEvent(ON_KEY_DOWN_EVENT, event->GetModifiers(), event->GetKeyCode(), event->GetSymbol()));
			}

			long current_time = jcommon::Date::CurrentTimeMillis();

			if ((current_time-t->start_time) >= LONG_PRESS_TIME) {
				t->start_time = jcommon::Date::CurrentTimeMillis();

				DispatchUserEvent(new UserEvent(ON_KEY_LONGPRESS_EVENT, event->GetModifiers(), event->GetKeyCode(), event->GetSymbol()));
			}

			DispatchUserEvent(new UserEvent(ON_KEY_PRESS_EVENT, event->GetModifiers(), event->GetKeyCode(), event->GetSymbol()));

			return true;
		}

		virtual bool KeyReleased(jevent::KeyEvent *event)
		{
			struct event_t *t = _events[event->GetSymbol()];

			if ((void *)t != NULL) {
				t->key_down = false;

				DispatchUserEvent(new UserEvent(ON_KEY_UP_EVENT, event->GetModifiers(), event->GetKeyCode(), event->GetSymbol()));
			}

			return true;
		}

		virtual bool MousePressed(jevent::MouseEvent *event)
		{
			if (event->GetClickCount() == 1) {
				DispatchUserEvent(new UserEvent(ON_MOUSE_CLICK_EVENT, event->GetButton(), event->GetClickCount(), event->GetX(), event->GetY(), 0.0, 0.0));
			}

			DispatchUserEvent(new UserEvent(ON_MOUSE_PRESS_EVENT, event->GetButton(), event->GetClickCount(), event->GetX(), event->GetY(), 0.0, 0.0));

			return true;
		}

		virtual bool MouseReleased(jevent::MouseEvent *event)
		{
			DispatchUserEvent(new UserEvent(ON_MOUSE_RELEASE_EVENT, event->GetButton(), event->GetClickCount(), event->GetX(), event->GetY(), 0.0, 0.0));

			return true;
		}

		virtual bool MouseMoved(jevent::MouseEvent *event)
		{
			double tdiff = (double)(jcommon::Date::CurrentTimeMillis()-_last_mouse_move),
						 mdiff = (tdiff > -10 && tdiff < 10)?10:tdiff,
						 vx = (event->GetX()-_last_mouse_location.x)/mdiff,
						 vy = (event->GetY()-_last_mouse_location.y)/mdiff;

			DispatchUserEvent(new UserEvent(ON_MOUSE_MOVE_EVENT, event->GetButton(), event->GetClickCount(), event->GetX(), event->GetY(), vx, vy));

			_last_mouse_move = jcommon::Date::CurrentTimeMillis();
			_last_mouse_location.x = event->GetX();
			_last_mouse_location.y = event->GetY();

			return true;
		}

		virtual bool MouseWheel(jevent::MouseEvent *event)
		{
			DispatchUserEvent(new UserEvent(ON_MOUSE_WHEEL_EVENT, event->GetButton(), event->GetClickCount(), event->GetX(), event->GetY(), 0.0, 0.0));

			return true;
		}

};

UserEventManager *UserEventManager::_instance = NULL;

class Test : public jgui::Window, public UserEventListener {

	private:
		jgui::jpoint_t _ball;
		jgui::jpoint_t _ball_diff;
		int _raio;
		int _color;
		bool _pressed;

	public:
		Test():
			jgui::Window(0, 0, 960, 540)
		{
			_ball.x = 960/2;
			_ball.y = 540/2;
			_raio = 32;
			_color = 0;
			_pressed = false;

			UserEventManager::GetInstance()->RegisterUserEventListener(this);
		}

		virtual ~Test()
		{
			UserEventManager::GetInstance()->RemoveUserEventListener(this);
		}

		virtual void OnKeyDown(UserEvent *event)
		{
			std::cout << "OnKeyDown: " << event->GetKeyCode() << std::endl;
		}

		virtual void OnKeyPress(UserEvent *event)
		{
			std::cout << "OnKeyPress: " << event->GetKeyCode() << std::endl;
		}

		virtual void OnKeyUp(UserEvent *event)
		{
			std::cout << "OnKeyUp: " << event->GetKeyCode() << std::endl;
			
			if ((event->GetKeySymbol() == jevent::JKS_ESCAPE || event->GetKeySymbol() == jevent::JKS_EXIT)) {
				SetVisible(false);
			}
		}

		virtual void OnKeyLongPress(UserEvent *event)
		{
			std::cout << "OnKeyLongPress" << std::endl;
		}

		virtual void OnMousePress(UserEvent *event)
		{
			std::cout << "OnMousePress: "  << event->GetClickCount() << ", Button: " << event->GetButton() << std::endl;

      jgui::jpoint_t
        location = GetLocation();
			int 
        cx = event->GetX() - location.x,
			  cy = event->GetY() - location.y;
			int 
        x = cx-_ball.x,
				y = cy-_ball.y;

			if ((x*x+y*y) <= _raio*_raio) {
				if (event->GetClickCount() == 1) {
					_pressed = true;

					_ball_diff.x = -x;
					_ball_diff.y = -y;
				} else {
					_pressed = false;

					if (event->GetClickCount() == 2) {
						_color = (_color == 0)?1:0;

						Repaint();
					}
				}
			}
		}

		virtual void OnMouseRelease(UserEvent *event)
		{
			std::cout << "OnMouseRelease" << ", Button: " << event->GetButton() << std::endl;

			_pressed = false;
		}

		virtual void OnMouseClick(UserEvent *event)
		{
			std::cout << "OnMouseClick" << ", Button: " << event->GetButton() << std::endl;
		}

		virtual void OnMouseMove(UserEvent *event)
		{
			jgui::jpoint_t
				location = GetLocation();
			jgui::jsize_t
				size = GetSize();
			int cx = event->GetX() - location.x;
			int cy = event->GetY() - location.y;

			std::cout << "OnMouseMove: " << cx << ", " << cy << " [" << event->GetVelocityX() << ", " << event->GetVelocityY() << "]" << std::endl;

			if (_pressed == true) {
				_ball.x = cx+_ball_diff.x;
				_ball.y = cy+_ball_diff.y;

				if (_ball.x < _raio) {
					_ball.x = _raio;
				}

				if (_ball.y < _raio) {
					_ball.y = _raio;
				}

				if (_ball.x > (size.width - _raio)) {
					_ball.x = size.width - _raio;
				}

				if (_ball.y > (size.height - _raio)) {
					_ball.y = size.height - _raio;
				}

				Repaint();
			}
		}

		virtual void OnMouseWheel(UserEvent *event)
		{
			std::cout << "OnMouseWheel: " << event->GetClickCount() << std::endl;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

			if (_color == 0) {
				g->SetColor(jgui::Color::Red);
			} else if (_color == 1) {
				g->SetColor(jgui::Color::Blue);
			}

			g->FillCircle(_ball.x, _ball.y, _raio);
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Test app;

	app.RegisterKeyListener(UserEventManager::GetInstance());
	app.RegisterMouseListener(UserEventManager::GetInstance());

	app.SetTitle("Events");
	app.SetVisible(true);

	jgui::Application::Loop();

	return 0;
}


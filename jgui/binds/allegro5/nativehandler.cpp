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
#include "Stdafx.h"
#include "nativehandler.h"
#include "nativegraphics.h"
#include "nativetypes.h"
#include "jimage.h"
#include "jproperties.h"
#include "jruntimeexception.h"
#include "jdate.h"

#include <allegro5/allegro.h>

#define NATIVE_HANDLER_REQUEST_FLIP ALLEGRO_GET_EVENT_TYPE('n', 'h', 'r', 'f')

namespace jgui {

ALLEGRO_DISPLAY *_display = NULL;
ALLEGRO_BITMAP *_surface;
ALLEGRO_EVENT_SOURCE _user_event;

class InputEventDispatcher : public jthread::Thread {

	private:
		std::vector<jcommon::EventObject *> _events;
		jthread::Mutex _mutex;
		jthread::Condition _sem;
		Application *_window;
		bool _is_running;

	public:
		InputEventDispatcher(Application *window)
		{
			_is_running = true;
			_window = window;
		}

		virtual ~InputEventDispatcher()
		{
			_is_running = false;

			_sem.Notify();

			WaitThread();
		}

		virtual void PostEvent(jcommon::EventObject *event)
		{
			_mutex.Lock();

			_events.push_back(event);

			_sem.Notify();

			_mutex.Unlock();
		}

		virtual void Run()
		{
			jcommon::EventObject *event = NULL;

			do {
				_mutex.Lock();

				while (_events.size() == 0) {
					_sem.Wait(&_mutex);

					if (_is_running == false) {
						_mutex.Unlock();

						return;
					}
				}
				
				event = *_events.begin();

				_events.erase(_events.begin());
	
				_mutex.Unlock();

				if (dynamic_cast<jgui::KeyEvent *>(event) != NULL) {
					_window->DispatchKeyEvent(dynamic_cast<jgui::KeyEvent *>(event));
				} else if (dynamic_cast<jgui::MouseEvent *>(event) != NULL) {
					_window->DispatchMouseEvent(dynamic_cast<jgui::MouseEvent *>(event));
				}
			} while (_is_running == true);
		}

};

static jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case ALLEGRO_KEY_ENTER:
			return JKS_ENTER; // JKS_RETURN;
		case ALLEGRO_KEY_BACKSPACE:
			return JKS_BACKSPACE;
		case ALLEGRO_KEY_TAB:
			return JKS_TAB;
		// case ALLEGRO_KEY_CANCEL:
		//	return JKS_CANCEL;
		case ALLEGRO_KEY_ESCAPE:
			return JKS_ESCAPE;
		case ALLEGRO_KEY_SPACE:
			return JKS_SPACE;
		// case ALLEGRO_KEY_EXCLAIM:
		// 	return JKS_EXCLAMATION_MARK;
		// case ALLEGRO_KEY_QUOTEDBL:
		// 	return JKS_QUOTATION;
		// case ALLEGRO_KEY_HASH:
		// 	return JKS_NUMBER_SIGN;
		// case ALLEGRO_KEY_DOLLAR:
		// 	return JKS_DOLLAR_SIGN;
		// case ALLEGRO_KEY_PERCENT_SIGN:
		//	return JKS_PERCENT_SIGN;
		// case ALLEGRO_KEY_AMPERSAND:   
		// 	return JKS_AMPERSAND;
		case ALLEGRO_KEY_QUOTE:
			return JKS_APOSTROPHE;
		// case ALLEGRO_KEY_LEFTPAREN:
		// 	return JKS_PARENTHESIS_LEFT;
		// case ALLEGRO_KEY_RIGHTPAREN:
		// 	return JKS_PARENTHESIS_RIGHT;
		// case ALLEGRO_KEY_ASTERISK:
		// 	return JKS_STAR;
		// case ALLEGRO_KEY_PLUS:
		// 	return JKS_PLUS_SIGN;
		case ALLEGRO_KEY_COMMA:   
			return JKS_COMMA;
		case ALLEGRO_KEY_MINUS:
			return JKS_MINUS_SIGN;
		// case ALLEGRO_KEY_PERIOD:  
		// 	return JKS_PERIOD;
		case ALLEGRO_KEY_SLASH:
			return JKS_SLASH;
		case ALLEGRO_KEY_0:     
			return JKS_0;
		case ALLEGRO_KEY_1:
			return JKS_1;
		case ALLEGRO_KEY_2:
			return JKS_2;
		case ALLEGRO_KEY_3:
			return JKS_3;
		case ALLEGRO_KEY_4:
			return JKS_4;
		case ALLEGRO_KEY_5:
			return JKS_5;
		case ALLEGRO_KEY_6:
			return JKS_6;
		case ALLEGRO_KEY_7:
			return JKS_7;
		case ALLEGRO_KEY_8:
			return JKS_8;
		case ALLEGRO_KEY_9:
			return JKS_9;
		// case ALLEGRO_KEY_COLON:
		// 	return JKS_COLON;
		case ALLEGRO_KEY_SEMICOLON:
			return JKS_SEMICOLON;
		// case ALLEGRO_KEY_LESS:
		// 	return JKS_LESS_THAN_SIGN;
		case ALLEGRO_KEY_EQUALS: 
			return JKS_EQUALS_SIGN;
		// case ALLEGRO_KEY_GREATER:
		// 	return JKS_GREATER_THAN_SIGN;
		// case ALLEGRO_KEY_QUESTION:   
		// 	return JKS_QUESTION_MARK;
		case ALLEGRO_KEY_AT:
			return JKS_AT;
		case ALLEGRO_KEY_A:
			return JKS_A;
		case ALLEGRO_KEY_B:
			return JKS_B;
		case ALLEGRO_KEY_C:
			return JKS_C;
		case ALLEGRO_KEY_D:
			return JKS_D;
		case ALLEGRO_KEY_E:
			return JKS_E;
		case ALLEGRO_KEY_F:
			return JKS_F;
		case ALLEGRO_KEY_G:
			return JKS_G;
		case ALLEGRO_KEY_H:
			return JKS_H;
		case ALLEGRO_KEY_I:
			return JKS_I;
		case ALLEGRO_KEY_J:
			return JKS_J;
		case ALLEGRO_KEY_K:
			return JKS_K;
		case ALLEGRO_KEY_L:
			return JKS_L;
		case ALLEGRO_KEY_M:
			return JKS_M;
		case ALLEGRO_KEY_N:
			return JKS_N;
		case ALLEGRO_KEY_O:
			return JKS_O;
		case ALLEGRO_KEY_P:
			return JKS_P;
		case ALLEGRO_KEY_Q:
			return JKS_Q;
		case ALLEGRO_KEY_R:
			return JKS_R;
		case ALLEGRO_KEY_S:
			return JKS_S;
		case ALLEGRO_KEY_T:
			return JKS_T;
		case ALLEGRO_KEY_U:
			return JKS_U;
		case ALLEGRO_KEY_V:
			return JKS_V;
		case ALLEGRO_KEY_W:
			return JKS_W;
		case ALLEGRO_KEY_X:
			return JKS_X;
		case ALLEGRO_KEY_Y:
			return JKS_Y;
		case ALLEGRO_KEY_Z:
			return JKS_Z;
		// case ALLEGRO_KEY_LEFTBRACKET:
		// 	return JKS_SQUARE_BRACKET_LEFT;
		case ALLEGRO_KEY_BACKSLASH:   
			return JKS_BACKSLASH;
		// case ALLEGRO_KEY_RIGHTBRACKET:
		// 	return JKS_SQUARE_BRACKET_RIGHT;
		// case ALLEGRO_KEY_CARET:
		// 	return JKS_CIRCUMFLEX_ACCENT;
		// case ALLEGRO_KEY_UNDERSCORE:    
		// 	return JKS_UNDERSCORE;
		case ALLEGRO_KEY_BACKQUOTE:
			return JKS_GRAVE_ACCENT;
			/*
		case ALLEGRO_KEY_a:       
			return JKS_a;
		case ALLEGRO_KEY_b:
			return JKS_b;
		case ALLEGRO_KEY_c:
			return JKS_c;
		case ALLEGRO_KEY_d:
			return JKS_d;
		case ALLEGRO_KEY_e:
			return JKS_e;
		case ALLEGRO_KEY_f:
			return JKS_f;
		case ALLEGRO_KEY_g:
			return JKS_g;
		case ALLEGRO_KEY_h:
			return JKS_h;
		case ALLEGRO_KEY_i:
			return JKS_i;
		case ALLEGRO_KEY_j:
			return JKS_j;
		case ALLEGRO_KEY_k:
			return JKS_k;
		case ALLEGRO_KEY_l:
			return JKS_l;
		case ALLEGRO_KEY_m:
			return JKS_m;
		case ALLEGRO_KEY_n:
			return JKS_n;
		case ALLEGRO_KEY_o:
			return JKS_o;
		case ALLEGRO_KEY_p:
			return JKS_p;
		case ALLEGRO_KEY_q:
			return JKS_q;
		case ALLEGRO_KEY_r:
			return JKS_r;
		case ALLEGRO_KEY_s:
			return JKS_s;
		case ALLEGRO_KEY_t:
			return JKS_t;
		case ALLEGRO_KEY_u:
			return JKS_u;
		case ALLEGRO_KEY_v:
			return JKS_v;
		case ALLEGRO_KEY_w:
			return JKS_w;
		case ALLEGRO_KEY_x:
			return JKS_x;
		case ALLEGRO_KEY_y:
			return JKS_y;
		case ALLEGRO_KEY_z:
			return JKS_z;
		case ALLEGRO_KEY_CURLY_BRACKET_LEFT:
			return JKS_CURLY_BRACKET_LEFT;
		case ALLEGRO_KEY_VERTICAL_BAR:  
			return JKS_VERTICAL_BAR;
		case ALLEGRO_KEY_CURLY_BRACKET_RIGHT:
			return JKS_CURLY_BRACKET_RIGHT;
		*/
		case ALLEGRO_KEY_TILDE:  
			return JKS_TILDE;
		case ALLEGRO_KEY_DELETE:
			return JKS_DELETE;
		case ALLEGRO_KEY_LEFT:
			return JKS_CURSOR_LEFT;
		case ALLEGRO_KEY_RIGHT:
			return JKS_CURSOR_RIGHT;
		case ALLEGRO_KEY_UP:  
			return JKS_CURSOR_UP;
		case ALLEGRO_KEY_DOWN:
			return JKS_CURSOR_DOWN;
		case ALLEGRO_KEY_INSERT:  
			return JKS_INSERT;
		case ALLEGRO_KEY_HOME:     
			return JKS_HOME;
		case ALLEGRO_KEY_END:
			return JKS_END;
		case ALLEGRO_KEY_PGUP:
			return JKS_PAGE_UP;
		case ALLEGRO_KEY_PGDN:
			return JKS_PAGE_DOWN;
		// case ALLEGRO_KEY_PRINT:   
		// 	return JKS_PRINT;
		case ALLEGRO_KEY_PAUSE:
			return JKS_PAUSE;
		// case ALLEGRO_KEY_RED:
		// 	return JKS_RED;
		// case ALLEGRO_KEY_GREEN:
		// 	return JKS_GREEN;
		// case ALLEGRO_KEY_YELLOW:
		// 	return JKS_YELLOW;
		// case ALLEGRO_KEY_BLUE:
		// 	return JKS_BLUE;
		case ALLEGRO_KEY_F1:
		 	return JKS_F1;
		case ALLEGRO_KEY_F2:
		 	return JKS_F2;
		case ALLEGRO_KEY_F3:
			return JKS_F3;
		case ALLEGRO_KEY_F4:
			return JKS_F4;
		case ALLEGRO_KEY_F5:
			return JKS_F5;
		case ALLEGRO_KEY_F6:     
			return JKS_F6;
		case ALLEGRO_KEY_F7:    
		 	return JKS_F7;
		case ALLEGRO_KEY_F8:   
			return JKS_F8;
		case ALLEGRO_KEY_F9:  
			return JKS_F9;
		case ALLEGRO_KEY_F10: 
		 	return JKS_F10;
		case ALLEGRO_KEY_F11:
			return JKS_F11;
		case ALLEGRO_KEY_F12:
		 	return JKS_F12;
		case ALLEGRO_KEY_LSHIFT:
		case ALLEGRO_KEY_RSHIFT:
		 	return JKS_SHIFT;
		case ALLEGRO_KEY_LCTRL:
		case ALLEGRO_KEY_RCTRL:
		 	return JKS_CONTROL;
		case ALLEGRO_KEY_ALT:
		 	return JKS_ALT;
		case ALLEGRO_KEY_ALTGR:
			return JKS_ALTGR;
		// case ALLEGRO_KEY_LMETA:
		// case ALLEGRO_KEY_RMETA:
		// 	return JKS_META;
		// case ALLEGRO_KEY_LSUPER:
		// case ALLEGRO_KEY_RSUPER:
		// 	return JKS_SUPER;
		// case ALLEGRO_KEY_HYPER:
		// 	return JKS_HYPER;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

static InputEventDispatcher *_dispatcher = NULL;

NativeHandler::NativeHandler():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeHandler");

	_display = NULL;
	_graphics = NULL;
	_is_running = false;
	_is_initialized = false;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = 0LL;
	_click_count = 1;

	InternalInitialize();
}

NativeHandler::~NativeHandler()
{
}

void NativeHandler::InternalInitialize()
{
	if (_is_initialized == true) {
		return;
	}

	if (al_init() == false) {
		throw jcommon::RuntimeException("Problem to init allegro5");
	}

	al_install_keyboard();
	al_install_mouse();
	
	ALLEGRO_DISPLAY_MODE mode;
	
	if (al_get_display_mode(0, &mode) == NULL) {
		throw jcommon::RuntimeException("Could not get screen mode");
	}

	_size.width = _screen.width = mode.width;
	_size.height = _screen.height = mode.height;

	InternalInitCursors();

	_dispatcher = new InputEventDispatcher(this);

	_dispatcher->Start();

	_is_initialized = true;
}

void NativeHandler::InternalRelease()
{
	InternalReleaseCursors();

	// SDL_Quit();
	
	delete _dispatcher;
	_dispatcher = NULL;

	_is_initialized = false;
}

void NativeHandler::InternalInitCursors()
{
#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = Image::CreateImage(JPF_ARGB, w, h);												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);	\
																																				\
	_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = Image::CreateImage(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(JCS_DEFAULT, 0, 0, 8, 8);
	CURSOR_INIT(JCS_CROSSHAIR, 4, 3, 15, 15);
	CURSOR_INIT(JCS_EAST, 4, 4, 22, 15);
	CURSOR_INIT(JCS_WEST, 5, 4, 9, 15);
	CURSOR_INIT(JCS_NORTH, 6, 4, 15, 8);
	CURSOR_INIT(JCS_SOUTH, 7, 4, 15, 22);
	CURSOR_INIT(JCS_HAND, 1, 0, 15, 15);
	CURSOR_INIT(JCS_MOVE, 8, 4, 15, 15);
	CURSOR_INIT(JCS_NS, 2, 4, 15, 15);
	CURSOR_INIT(JCS_WE, 3, 4, 15, 15);
	CURSOR_INIT(JCS_NW_CORNER, 8, 1, 10, 10);
	CURSOR_INIT(JCS_NE_CORNER, 9, 1, 20, 10);
	CURSOR_INIT(JCS_SW_CORNER, 6, 1, 10, 20);
	CURSOR_INIT(JCS_SE_CORNER, 7, 1, 20, 20);
	CURSOR_INIT(JCS_TEXT, 7, 0, 15, 15);
	CURSOR_INIT(JCS_WAIT, 8, 0, 15, 15);
	
	delete cursors;

	SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

void NativeHandler::RequestFlip()
{
	ALLEGRO_EVENT event;

	event.user.type = NATIVE_HANDLER_REQUEST_FLIP;

	al_emit_user_event(&_user_event, &event, NULL);
}

void NativeHandler::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

void NativeHandler::MainLoop()
{
	_display = al_create_display(_size.width, _size.height);

	if (_display == NULL) {
		return;
	}

	al_set_new_display_option(ALLEGRO_UPDATE_DISPLAY_REGION, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;
	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;

	if (_is_fullscreen_enabled == false) {
		al_set_new_display_flags(ALLEGRO_WINDOWED);
	} else {
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_RESIZABLE); // | ALLEGRO_GENERATE_EXPOSE_EVENTS);
	}

	if (_is_undecorated == true) {
		al_set_new_display_flags(ALLEGRO_NOFRAME);
	}

	// al_set_new_display_refresh_rate(60);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	_surface = al_create_bitmap(_size.width, _size.height);
	
	if (_surface == NULL) {
		throw jcommon::RuntimeException("Cannot get a window's surface");
	}

	NativeGraphics *native_graphics = new NativeGraphics(this, (void *)_surface, NULL, JPF_ARGB, _size.width, _size.height);

	_graphics = native_graphics;

	al_init_user_event_source(&_user_event);

	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(_display));
	al_register_event_source(queue, &_user_event);

	ALLEGRO_EVENT event;

	_is_running = true;

	_init_sem.Notify();

	do {
		if (_is_running == false) {
			break;
		}

		// TODO:: verificar se a espera eh melhor que 
		// al_get_next_event(queue, &event);
    al_wait_for_event(queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			_is_running = false;
		} else if (event.type == NATIVE_HANDLER_REQUEST_FLIP) {
			native_graphics->InternalFlip(_surface);
		} else {
			InternalEventHandler(event);
		}

		// INFO:: expose events
		// graphics->ReleaseFlip();
		// Repaint();

		usleep(10000);
	} while (_is_running == true);
	
  al_destroy_bitmap(_surface);

	delete _graphics;
	_graphics = NULL;

	al_destroy_event_queue(queue);
}

void NativeHandler::SetFullScreenEnabled(bool b)
{
	if (_is_fullscreen_enabled == b) {
		return;
	}

	_is_fullscreen_enabled = b;
	
	/*
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_FULLSCREEN;
	event.user.data1 = this;

	SDL_PushEvent(&event);
	*/
}

void NativeHandler::WaitForExit()
{
	// CHANGE:: if continues to block exit, change to timed semaphore
	while (_is_visible == true) {
		_exit_sem.Wait();
	}
}

void NativeHandler::SetTitle(std::string title)
{
	_title = title;

	/*
	if (_window != NULL) {
		SDL_SetWindowTitle(_window, _title.c_str());
	}
	*/
}

void NativeHandler::SetOpacity(int i)
{
	Application::SetOpacity(i);

	if (_is_visible == true) {
		// SDL_SetWindowOpacity(_window, (float)(i/256.0));
	}
}

void NativeHandler::SetUndecorated(bool b)
{
	_is_undecorated = b;

	/*
	if (_is_visible == true) {
		if (_is_undecorated == true) {
			SDL_SetWindowBordered(_window, SDL_FALSE);
		} else {
			SDL_SetWindowBordered(_window, SDL_TRUE);
		}
	}
	*/
}

bool NativeHandler::IsUndecorated()
{
	return _is_undecorated;
}

void NativeHandler::SetVisible(bool b)
{
	// if true, create a window and block
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_OPENED));
	// if false, destroy the window and send the events
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSING));
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSED));
	
	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == true) {
		// TODO:: create window

		DoLayout();
		Start();

		_init_sem.Wait();
		
		Repaint();
	} else {
		_is_running = false;

		WaitThread();
	
		InternalRelease();

		_exit_sem.Notify();
	}
}

void NativeHandler::SetBounds(int x, int y, int width, int height)
{
	Application::SetBounds(x, y, width, height);
	
	if (_is_visible == false) {
		return;
	}

	/*
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETBOUNDS;
	event.user.data1 = this;

	SDL_PushEvent(&event);
	*/
}

void NativeHandler::SetLocation(int x, int y)
{
	Application::SetLocation(x, y);
	
	if (_is_visible == false) {
		return;
	}

	/*
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETLOCATION;
	event.user.data1 = this;

	SDL_PushEvent(&event);
	*/
}

void NativeHandler::SetResizable(bool b)
{
	Application::SetResizable(b);

	if (_is_visible == false) {
		return;
	}

	// SDL_SetWindowResizable(_window, _is_resizable);
}

void NativeHandler::SetSize(int width, int height)
{
	Application::SetSize(width, height);
	
	if (_is_visible == false) {
		return;
	}

	/*
	SDL_SetWindowMinimumSize(_window, _minimum_size.width, _minimum_size.height);
	SDL_SetWindowMaximumSize(_window, _maximum_size.width, _maximum_size.height);

	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETSIZE;
	event.user.data1 = this;

	SDL_PushEvent(&event);
	*/
}

void NativeHandler::Move(int x, int y)
{
	Application::Move(x, y);
	
	if (_is_visible == false) {
		return;
	}

	/*
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETLOCATION;
	event.user.data1 = this;

	SDL_PushEvent(&event);
	*/
}

void NativeHandler::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	/*
	if (x > _screen.width) {
		x = _screen.width;
	}

	if (y > _screen.height) {
		y = _screen.height;
	}
	*/

	// if (_window != NULL) {
	// 	SDL_WarpMouseInWindow(_window, x, y);
	// }
	
	// SDL_WarpMouseInWindow(NULL, x, y);
	// SDL_WarpMouseGlobal(x, y);
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	// SDL_GetMouseState(&p.x, &p.y);
	// SDL_GetGlobalMouseState(&p.x, &p.y);

	return p;
}

void NativeHandler::SetCursorEnabled(bool b)
{
	jgui::Application::SetCursorEnabled(b);

	// SDL_ShowCursor((_is_cursor_enabled == false)?SDL_DISABLE:SDL_ENABLE);
}

bool NativeHandler::IsCursorEnabled()
{
	return jgui::Application::IsCursorEnabled();
}

void NativeHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void NativeHandler::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	/*
	jsize_t t = shape->GetSize();
	uint32_t *data = NULL;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == NULL) {
		return;
	}

	SDL_Surface *surface = NULL;
	uint32_t rmask = 0x000000ff;
	uint32_t gmask = 0x0000ff00;
	uint32_t bmask = 0x00ff0000;
	uint32_t amask = 0xff000000;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#endif

	surface = SDL_CreateRGBSurfaceFrom(data, t.width, t.height, 32, t.width*4, rmask, gmask, bmask, amask);

	if (surface == NULL) {
		delete [] data;

		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != NULL) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);

	delete [] data;
	*/
}

void NativeHandler::PostEvent(KeyEvent *event)
{
	if (event == NULL) {
		return;
	}

	_dispatcher->PostEvent(event);
}

void NativeHandler::PostEvent(MouseEvent *event)
{
	if (event == NULL) {
		return;
	}

	_dispatcher->PostEvent(event);
}

void NativeHandler::SetRotation(jwidget_rotation_t t)
{
	jgui::Application::SetRotation(t);
}

jwidget_rotation_t NativeHandler::GetRotation()
{
	return jgui::Application::GetRotation();
}

void NativeHandler::InternalEventHandler(ALLEGRO_EVENT event)
{
	if (event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
		SetCursor(GetCursor());

		DispatchWidgetEvent(new WidgetEvent(this, JWET_ENTERED));
	} else if (event.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) {
		SetCursor(JCS_DEFAULT);

		DispatchWidgetEvent(new WidgetEvent(this, JWET_LEAVED));
	} else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
		al_acknowledge_resize(event.display.source);
		
		/* TODO::
		_size.width = event.window.data1;
		_size.height = event.window.data2;

		_graphics->SetNativeSurface((void *)_surface, _size.width, _size.height);
		_graphics->ReleaseFlip();
		*/

		Repaint();

		DispatchWidgetEvent(new WidgetEvent(this, JWET_RESIZED));
	} else if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		} else if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		} else if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_ALT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		} else if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_ALTGR) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALTGR);
		} else if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_LWIN) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_META);
		} else if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_RWIN) != 0) {
			// mod = (jkeyevent_modifiers_t)(mod | JKM_RMETA);
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			type = JKT_PRESSED;

			// TODO:: grab pointer events
		} else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			type = JKT_RELEASED;

			// TODO:: ungrab pointer events
		}

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.keyboard.keycode);

		_dispatcher->PostEvent(new KeyEvent(this, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == ALLEGRO_EVENT_MOUSE_AXES || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		_mouse_x = event.mouse.x;
		_mouse_y = event.mouse.y;

		_mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
		_mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);

		if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			type = JMT_MOVED;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
				_mouse_buttons[event.mouse.button-1] = true;

				type = JMT_PRESSED;
			} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
				_mouse_buttons[event.mouse.button-1] = false;

				type = JMT_RELEASED;
			}

			if (event.mouse.button == 1) {
				button = JMB_BUTTON1;
			} else if (event.mouse.button == 2) {
				button = JMB_BUTTON2;
			} else if (event.mouse.button == 3) {
				button = JMB_BUTTON3;
			}

			// _click_count = 1;

			if (type == JMT_PRESSED) {
				if ((jcommon::Date::CurrentTimeMillis()-_last_keypress) < 200L) {
					_click_count = _click_count + 1;
				} else {
					_click_count = 1;
				}
			
				_last_keypress = jcommon::Date::CurrentTimeMillis();

				mouse_z = _click_count;
			}
		} else if (event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
			type = JMT_ROTATED;
			mouse_z = event.mouse.dz;
		}
		
		if (_mouse_buttons[1-1] == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		}

		if (_mouse_buttons[2-1] == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		}

		if (_mouse_buttons[3-1] == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		_dispatcher->PostEvent(new MouseEvent(this, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	}
}

void NativeHandler::Run()
{
	MainLoop();
}

}

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
#include "jfont.h"

namespace jgui {

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

static jkeyevent_symbol_t TranslateToNativeKeySymbol(SDL_Keysym symbol)
{
	switch (symbol.sym) {
		case SDLK_RETURN:
			return JKS_ENTER; // JKS_RETURN;
		case SDLK_BACKSPACE:
			return JKS_BACKSPACE;
		case SDLK_TAB:
			return JKS_TAB;
		// case SDLK_CANCEL:
		//	return JKS_CANCEL;
		case SDLK_ESCAPE:
			return JKS_ESCAPE;
		case SDLK_SPACE:
			return JKS_SPACE;
		case SDLK_EXCLAIM:
			return JKS_EXCLAMATION_MARK;
		case SDLK_QUOTEDBL:
			return JKS_QUOTATION;
		case SDLK_HASH:
			return JKS_NUMBER_SIGN;
		case SDLK_DOLLAR:
			return JKS_DOLLAR_SIGN;
		// case SDLK_PERCENT_SIGN:
		//	return JKS_PERCENT_SIGN;
		case SDLK_AMPERSAND:   
			return JKS_AMPERSAND;
		case SDLK_QUOTE:
			return JKS_APOSTROPHE;
		case SDLK_LEFTPAREN:
			return JKS_PARENTHESIS_LEFT;
		case SDLK_RIGHTPAREN:
			return JKS_PARENTHESIS_RIGHT;
		case SDLK_ASTERISK:
			return JKS_STAR;
		case SDLK_PLUS:
			return JKS_PLUS_SIGN;
		case SDLK_COMMA:   
			return JKS_COMMA;
		case SDLK_MINUS:
			return JKS_MINUS_SIGN;
		case SDLK_PERIOD:  
			return JKS_PERIOD;
		case SDLK_SLASH:
			return JKS_SLASH;
		case SDLK_0:     
			return JKS_0;
		case SDLK_1:
			return JKS_1;
		case SDLK_2:
			return JKS_2;
		case SDLK_3:
			return JKS_3;
		case SDLK_4:
			return JKS_4;
		case SDLK_5:
			return JKS_5;
		case SDLK_6:
			return JKS_6;
		case SDLK_7:
			return JKS_7;
		case SDLK_8:
			return JKS_8;
		case SDLK_9:
			return JKS_9;
		case SDLK_COLON:
			return JKS_COLON;
		case SDLK_SEMICOLON:
			return JKS_SEMICOLON;
		case SDLK_LESS:
			return JKS_LESS_THAN_SIGN;
		case SDLK_EQUALS: 
			return JKS_EQUALS_SIGN;
		case SDLK_GREATER:
			return JKS_GREATER_THAN_SIGN;
		case SDLK_QUESTION:   
			return JKS_QUESTION_MARK;
		case SDLK_AT:
			return JKS_AT;
			/*
		case SDLK_CAPITAL_A:
			return JKS_A;
		case SDLK_CAPITAL_B:
			return JKS_B;
		case SDLK_CAPITAL_C:
			return JKS_C;
		case SDLK_CAPITAL_D:
			return JKS_D;
		case SDLK_CAPITAL_E:
			return JKS_E;
		case SDLK_CAPITAL_F:
			return JKS_F;
		case SDLK_CAPITAL_G:
			return JKS_G;
		case SDLK_CAPITAL_H:
			return JKS_H;
		case SDLK_CAPITAL_I:
			return JKS_I;
		case SDLK_CAPITAL_J:
			return JKS_J;
		case SDLK_CAPITAL_K:
			return JKS_K;
		case SDLK_CAPITAL_L:
			return JKS_L;
		case SDLK_CAPITAL_M:
			return JKS_M;
		case SDLK_CAPITAL_N:
			return JKS_N;
		case SDLK_CAPITAL_O:
			return JKS_O;
		case SDLK_CAPITAL_P:
			return JKS_P;
		case SDLK_CAPITAL_Q:
			return JKS_Q;
		case SDLK_CAPITAL_R:
			return JKS_R;
		case SDLK_CAPITAL_S:
			return JKS_S;
		case SDLK_CAPITAL_T:
			return JKS_T;
		case SDLK_CAPITAL_U:
			return JKS_U;
		case SDLK_CAPITAL_V:
			return JKS_V;
		case SDLK_CAPITAL_W:
			return JKS_W;
		case SDLK_CAPITAL_X:
			return JKS_X;
		case SDLK_CAPITAL_Y:
			return JKS_Y;
		case SDLK_CAPITAL_Z:
			return JKS_Z;
			*/
		case SDLK_LEFTBRACKET:
			return JKS_SQUARE_BRACKET_LEFT;
		case SDLK_BACKSLASH:   
			return JKS_BACKSLASH;
		case SDLK_RIGHTBRACKET:
			return JKS_SQUARE_BRACKET_RIGHT;
		case SDLK_CARET:
			return JKS_CIRCUMFLEX_ACCENT;
		case SDLK_UNDERSCORE:    
			return JKS_UNDERSCORE;
		case SDLK_BACKQUOTE:
			return JKS_GRAVE_ACCENT;
		case SDLK_a:       
			return JKS_a;
		case SDLK_b:
			return JKS_b;
		case SDLK_c:
			return JKS_c;
		case SDLK_d:
			return JKS_d;
		case SDLK_e:
			return JKS_e;
		case SDLK_f:
			return JKS_f;
		case SDLK_g:
			return JKS_g;
		case SDLK_h:
			return JKS_h;
		case SDLK_i:
			return JKS_i;
		case SDLK_j:
			return JKS_j;
		case SDLK_k:
			return JKS_k;
		case SDLK_l:
			return JKS_l;
		case SDLK_m:
			return JKS_m;
		case SDLK_n:
			return JKS_n;
		case SDLK_o:
			return JKS_o;
		case SDLK_p:
			return JKS_p;
		case SDLK_q:
			return JKS_q;
		case SDLK_r:
			return JKS_r;
		case SDLK_s:
			return JKS_s;
		case SDLK_t:
			return JKS_t;
		case SDLK_u:
			return JKS_u;
		case SDLK_v:
			return JKS_v;
		case SDLK_w:
			return JKS_w;
		case SDLK_x:
			return JKS_x;
		case SDLK_y:
			return JKS_y;
		case SDLK_z:
			return JKS_z;
		// case SDLK_CURLY_BRACKET_LEFT:
		//	return JKS_CURLY_BRACKET_LEFT;
		// case SDLK_VERTICAL_BAR:  
		//	return JKS_VERTICAL_BAR;
		// case SDLK_CURLY_BRACKET_RIGHT:
		//	return JKS_CURLY_BRACKET_RIGHT;
		// case SDLK_TILDE:  
		//	return JKS_TILDE;
		case SDLK_DELETE:
			return JKS_DELETE;
		case SDLK_LEFT:
			return JKS_CURSOR_LEFT;
		case SDLK_RIGHT:
			return JKS_CURSOR_RIGHT;
		case SDLK_UP:  
			return JKS_CURSOR_UP;
		case SDLK_DOWN:
			return JKS_CURSOR_DOWN;
		case SDLK_INSERT:  
			return JKS_INSERT;
		case SDLK_HOME:     
			return JKS_HOME;
		case SDLK_END:
			return JKS_END;
		case SDLK_PAGEUP:
			return JKS_PAGE_UP;
		case SDLK_PAGEDOWN:
			return JKS_PAGE_DOWN;
		// case SDLK_PRINT:   
		//	return JKS_PRINT;
		case SDLK_PAUSE:
			return JKS_PAUSE;
		// case SDLK_RED:
		//	return JKS_RED;
		// case SDLK_GREEN:
		//	return JKS_GREEN;
		// case SDLK_YELLOW:
		//	return JKS_YELLOW;
		// case SDLK_BLUE:
		//	return JKS_BLUE;
		case SDLK_F1:
			return JKS_F1;
		case SDLK_F2:
			return JKS_F2;
		case SDLK_F3:
			return JKS_F3;
		case SDLK_F4:
			return JKS_F4;
		case SDLK_F5:
			return JKS_F5;
		case SDLK_F6:     
			return JKS_F6;
		case SDLK_F7:    
			return JKS_F7;
		case SDLK_F8:   
			return JKS_F8;
		case SDLK_F9:  
			return JKS_F9;
		case SDLK_F10: 
			return JKS_F10;
		case SDLK_F11:
			return JKS_F11;
		case SDLK_F12:
			return JKS_F12;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return JKS_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return JKS_CONTROL;
		case SDLK_LALT:
		case SDLK_RALT:
			return JKS_ALT;
		// case SDLK_ALTGR:
		//	return JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return JKS_META;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return JKS_SUPER;
		// case SDLK_HYPER:
		//	return JKS_HYPER;
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

	_window = NULL;
	_surface = NULL;
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

	if (SDL_Init(SDL_INIT_EVERYTHING)) {  
		throw jcommon::RuntimeException("Problem to init sdl2");
	}

	SDL_DisplayMode display;

	if (SDL_GetCurrentDisplayMode(0, &display) != 0) {
		throw jcommon::RuntimeException("Could not get screen mode");
	}

	_size.width = _screen.width = display.w;
	_size.height = _screen.height = display.h;

	InternalInitCursors();

	_dispatcher = new InputEventDispatcher(this);

	_dispatcher->Start();

	_is_initialized = true;
}

void NativeHandler::InternalRelease()
{
	InternalReleaseCursors();

	SDL_Quit();
	
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

void NativeHandler::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

void NativeHandler::MainLoop()
{
	SDL_Event event;
	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	
	if (_is_undecorated == true) {
		flags = flags | SDL_WINDOW_BORDERLESS;
	}

	// INFO:: create the main window
	_window = SDL_CreateWindow(_title.c_str(), _location.x, _location.y, _size.width, _size.height, flags);

	if (_window == NULL) {
		throw jcommon::RuntimeException("Cannot create a window");
	}

	// SDL_SetWindowBordered(_window, SDL_FALSE);

	_surface = SDL_CreateRenderer(_window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // SDL_RENDERER_SOFTWARE

	if (_surface == NULL) {
		throw jcommon::RuntimeException("Cannot get a window's surface");
	}

	_graphics = new NativeGraphics(this, (void *)_surface, NULL, JPF_ARGB, _size.width, _size.height);

	SDL_SetWindowMinimumSize(_window, _minimum_size.width, _minimum_size.height);
	SDL_SetWindowMaximumSize(_window, _maximum_size.width, _maximum_size.height);

	SDL_ShowWindow(_window);

	_is_running = true;

	_init_sem.Notify();

	// INFO:: process loop of events
	do {
		while (SDL_PollEvent(&event)) {
			if (_is_running == false) {
				break;
			}

			if (event.type == USER_NATIVE_EVENT_APPLICATION_FLIP) {
				NativeGraphics *g = reinterpret_cast<NativeGraphics *>(event.user.data1);

				g->InternalFlip();
			} else if (event.type == USER_NATIVE_EVENT_APPLICATION_SETSIZE) {
				SDL_SetWindowSize(_window, _size.width, _size.height);
			} else if (event.type == USER_NATIVE_EVENT_APPLICATION_SETLOCATION) {
				SDL_SetWindowPosition(_window, _location.x, _location.y);
			} else if (event.type == USER_NATIVE_EVENT_APPLICATION_SETBOUNDS) {
				SDL_SetWindowPosition(_window, _location.x, _location.y);
				SDL_SetWindowSize(_window, _size.width, _size.height);
			} else if (event.type == USER_NATIVE_EVENT_APPLICATION_FULLSCREEN) {
				if (_is_fullscreen_enabled == false) {
				  SDL_SetWindowFullscreen(_window, 0);
				} else {
					// INFO:: for real fullscreen mode, with videomode change
				  // SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
					// INFO:: for fake fullscree mode, without videomode change
				  SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				}
			} else {
				InternalEventHandler(event);
			}
		}
	} while (_is_running == true);
	
	if (_is_fullscreen_enabled == true) {
		SDL_SetWindowFullscreen(_window, 0);
	}

	SDL_DestroyRenderer(_surface);  

	// INFO:: release the main window
	SDL_HideWindow(_window);

	_surface = NULL;

	if (_window != NULL) {  
		SDL_DestroyWindow(_window);  
	} 

	_window = NULL;

	delete _graphics;
	_graphics = NULL;
}

void NativeHandler::SetFullScreenEnabled(bool b)
{
	if (_is_fullscreen_enabled == b) {
		return;
	}

	_is_fullscreen_enabled = b;
	
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_FULLSCREEN;
	event.user.data1 = this;

	SDL_PushEvent(&event);
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

	if (_window != NULL) {
		SDL_SetWindowTitle(_window, _title.c_str());
	}
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

	if (_is_visible == true) {
		if (_is_undecorated == true) {
			SDL_SetWindowBordered(_window, SDL_FALSE);
		} else {
			SDL_SetWindowBordered(_window, SDL_TRUE);
		}
	}
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

	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETBOUNDS;
	event.user.data1 = this;

	SDL_PushEvent(&event);
}

void NativeHandler::SetLocation(int x, int y)
{
	Application::SetLocation(x, y);
	
	if (_is_visible == false) {
		return;
	}

	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETLOCATION;
	event.user.data1 = this;

	SDL_PushEvent(&event);
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

	SDL_SetWindowMinimumSize(_window, _minimum_size.width, _minimum_size.height);
	SDL_SetWindowMaximumSize(_window, _maximum_size.width, _maximum_size.height);

	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETSIZE;
	event.user.data1 = this;

	SDL_PushEvent(&event);
}

void NativeHandler::Move(int x, int y)
{
	Application::Move(x, y);
	
	if (_is_visible == false) {
		return;
	}

	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_SETLOCATION;
	event.user.data1 = this;

	SDL_PushEvent(&event);
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
	
	SDL_WarpMouseInWindow(NULL, x, y);
	// SDL_WarpMouseGlobal(x, y);
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	SDL_GetMouseState(&p.x, &p.y);
	// SDL_GetGlobalMouseState(&p.x, &p.y);

	return p;

	/*
	if (_window->_window != NULL) {
		jpoint_t t = _window->GetLocation();
		SDL_GetMouseState(&p.x, &p.y);
		// SDL_GetGlobalMouseState(&p.x, &p.y);

		p.x = p.x - t.x;
		p.y = p.y - t.y;
	}
	*/

	return p;
}

void NativeHandler::SetCursorEnabled(bool b)
{
	jgui::Application::SetCursorEnabled(b);

	SDL_ShowCursor((_is_cursor_enabled == false)?SDL_DISABLE:SDL_ENABLE);
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

void NativeHandler::InternalEventHandler(SDL_Event event)
{
	if (event.type == SDL_WINDOWEVENT) {
		NativeGraphics *graphics = dynamic_cast<NativeGraphics *>(_graphics);
		
		if (event.window.event == SDL_WINDOWEVENT_ENTER) {
			// SDL_CaptureMouse(true);
			// void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
			// SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>
			
			SetCursor(GetCursor());

			DispatchWidgetEvent(new WidgetEvent(this, JWET_ENTERED));
		} else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
			// SDL_CaptureMouse(false);
			// void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
			// SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

			SetCursor(JCS_DEFAULT);

			DispatchWidgetEvent(new WidgetEvent(this, JWET_LEAVED));
		} else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
			DispatchWidgetEvent(new WidgetEvent(this, JWET_OPENED));
		} else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
			DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSED));
		} else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
			graphics->ReleaseFlip();

			Repaint();
		} else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
			DispatchWidgetEvent(new WidgetEvent(this, JWET_MOVED));
		} else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
			_size.width = event.window.data1;
			_size.height = event.window.data2;

			graphics->SetNativeSurface((void *)_surface, _size.width, _size.height);
			graphics->ReleaseFlip();

			Repaint();

			DispatchWidgetEvent(new WidgetEvent(this, JWET_RESIZED));
		} else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
		} else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
		} else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
		} else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
		} else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
		}
  } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if ((event.key.keysym.mod & KMOD_LSHIFT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		} else if ((event.key.keysym.mod & KMOD_RSHIFT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		} else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		} else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		} else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		} else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		// } else if ((event.key.keysym.mod & ) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_ALTGR);
		// } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_META);
		// } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_META);
		// } else if ((event.key.keysym.mod & ) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_SUPER);
		// } else if ((event.key.keysym.mod & ) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_HYPER);
		}

		type = (jkeyevent_type_t)(0);

		if (event.key.state == SDL_PRESSED) {
			type = JKT_PRESSED;

			// TODO:: grab pointer events
		} else if (event.key.state == SDL_RELEASED) {
			type = JKT_RELEASED;

			// TODO:: ungrab pointer events
		}

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym);

		_dispatcher->PostEvent(new KeyEvent(this, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
		if (event.type == SDL_MOUSEMOTION) {
			// e.motion.x/y
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			// e.button.button == SDL_BUTTON_LEFT
			// e.button.clicks
		} else if (event.type == SDL_MOUSEBUTTONUP) {
		} else if (event.type == SDL_MOUSEWHEEL) {
		}

		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		_mouse_x = event.motion.x;
		_mouse_y = event.motion.y;

		_mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
		_mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);

		if (event.type == SDL_MOUSEMOTION) {
			type = JMT_MOVED;
		} else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				type = JMT_PRESSED;
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				type = JMT_RELEASED;
			}

			if (event.button.button == SDL_BUTTON_LEFT) {
				button = JMB_BUTTON1;
			} else if (event.button.button == SDL_BUTTON_MIDDLE) {
				button = JMB_BUTTON2;
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				button = JMB_BUTTON3;
			}

			_click_count = event.button.clicks;

			if (type == JMT_PRESSED) {
				/*
				if ((jcommon::Date::CurrentTimeMillis()-_last_keypress) < 200L) {
					_click_count = _click_count + 1;
				} else {
					_click_count = 1;
				}
			
				_last_keypress = jcommon::Date::CurrentTimeMillis();
				*/

				mouse_z = _click_count;
			}
		} else if (event.type == SDL_MOUSEWHEEL) {
			type = JMT_ROTATED;
			mouse_z = event.motion.y;
		}

		uint32_t state = SDL_GetMouseState(NULL, NULL);
		
		if ((state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		}

		if ((state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		}

		if ((state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		// void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
		// SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			SDL_SetWindowGrab(_window, SDL_TRUE);
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			SDL_SetWindowGrab(_window, SDL_FALSE);
		}

		_dispatcher->PostEvent(new MouseEvent(this, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	}
}

void NativeHandler::Run()
{
	MainLoop();
}

}

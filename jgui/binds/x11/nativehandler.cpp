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

#include <X11/extensions/Xrandr.h>

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

static jkeyevent_symbol_t TranslateToNativeKeySymbol(KeySym symbol)
{
	switch (symbol) {
		/*
		case XK_Shift_L:
			return JKS_LShift;
		case XK_Shift_R:
			return JKS_RShift;
		case XK_Control_L:
			return JKS_LControl;
		case XK_Control_R:
			return JKS_RControl;
		case XK_Alt_L:
			return JKS_LAlt;
		case XK_Alt_R:
			return JKS_RAlt;
		case XK_Super_L:
			return JKS_LSystem;
		case XK_Super_R:
			return JKS_RSystem;
		case XK_Menu:
			return JKS_Menu;
		*/
		case XK_Escape:
			return JKS_ESCAPE;
		case XK_semicolon:
			return JKS_SEMICOLON;
		case XK_KP_Divide:
		case XK_slash:
			return JKS_SLASH;
		case XK_equal:
			return JKS_EQUALS_SIGN;
		case XK_KP_Subtract:
		case XK_hyphen:
		case XK_minus:
			return JKS_MINUS_SIGN;
		case XK_bracketleft:
			return JKS_SQUARE_BRACKET_LEFT;
		case XK_bracketright:
			return JKS_SQUARE_BRACKET_RIGHT;
		case XK_comma:
			return JKS_COMMA;
		case XK_KP_Decimal:
		case XK_period:
			return JKS_PERIOD;
		case XK_dead_acute:
			return JKS_APOSTROPHE;
		case XK_backslash:
			return JKS_BACKSLASH;
		case XK_dead_grave:
			return JKS_TILDE;
		case XK_space:
			return JKS_SPACE;
		case XK_KP_Enter:
		case XK_Return:
			return JKS_ENTER;
		case XK_BackSpace:
			return JKS_BACKSPACE;
		case XK_Tab:
			return JKS_TAB;
		case XK_Prior:
			return JKS_PAGE_UP;
		case XK_Next:
			return JKS_PAGE_DOWN;
		case XK_KP_End:
		case XK_End:
			return JKS_END;
		case XK_KP_Home:
		case XK_Home:
			return JKS_HOME;
		case XK_KP_Insert:
		case XK_Insert:
			return JKS_INSERT;
		case XK_KP_Delete:
		case XK_Delete:
			return JKS_DELETE;
		case XK_KP_Add:
		case XK_plus:
			return JKS_PLUS_SIGN;
		case XK_KP_Multiply:
			return JKS_STAR;
		case XK_Pause:
			return JKS_PAUSE;
		case XK_F1:
			return JKS_F1;
		case XK_F2:
			return JKS_F2;
		case XK_F3:
			return JKS_F3;
		case XK_F4:
			return JKS_F4;
		case XK_F5:
			return JKS_F5;
		case XK_F6:
			return JKS_F6;
		case XK_F7:
			return JKS_F7;
		case XK_F8:
			return JKS_F8;
		case XK_F9:
			return JKS_F9;
		case XK_F10:
			return JKS_F10;
		case XK_F11:
			return JKS_F11;
		case XK_F12:
			return JKS_F12;
		case XK_KP_Left:
		case XK_Left:
			return JKS_CURSOR_LEFT;
		case XK_KP_Right:
		case XK_Right:
			return JKS_CURSOR_RIGHT;
		case XK_KP_Up:
		case XK_Up:
			return JKS_CURSOR_UP;
		case XK_KP_Down:
		case XK_Down:
			return JKS_CURSOR_DOWN;
		case XK_KP_0:
		case XK_0:
			return JKS_0;
		case XK_KP_1:
		case XK_1:
			return JKS_1;
		case XK_KP_2:
		case XK_2:
			return JKS_2;
		case XK_KP_3:
		case XK_3:
			return JKS_3;
		case XK_KP_4:
		case XK_4:
			return JKS_4;
		case XK_KP_5:
		case XK_5:
			return JKS_5;
		case XK_KP_6:
		case XK_6:
			return JKS_6;
		case XK_KP_7:
		case XK_7:
			return JKS_7;
		case XK_KP_8:
		case XK_8:
			return JKS_8;
		case XK_KP_9:
		case XK_9:
			return JKS_9;
		case XK_A:
			return JKS_A;
		case XK_B:
			return JKS_B;
		case XK_C:
			return JKS_C;
		case XK_D:
			return JKS_D;
		case XK_E:
			return JKS_E;
		case XK_F:
			return JKS_F;
		case XK_G:
			return JKS_G;
		case XK_H:
			return JKS_H;
		case XK_I:
			return JKS_I;
		case XK_J:
			return JKS_J;
		case XK_K:
			return JKS_K;
		case XK_L:
			return JKS_L;
		case XK_M:
			return JKS_M;
		case XK_N:
			return JKS_N;
		case XK_O:
			return JKS_O;
		case XK_P:
			return JKS_P;
		case XK_Q:
			return JKS_Q;
		case XK_R:
			return JKS_R;
		case XK_S:
			return JKS_S;
		case XK_T:
			return JKS_T;
		case XK_U:
			return JKS_U;
		case XK_V:
			return JKS_V;
		case XK_X:
			return JKS_X;
		case XK_W:
			return JKS_W;
		case XK_Y:
			return JKS_Y;
		case XK_Z:
			return JKS_Z;
		case XK_a:
			return JKS_a;
		case XK_b:
			return JKS_b;
		case XK_c:
			return JKS_c;
		case XK_d:
			return JKS_d;
		case XK_e:
			return JKS_e;
		case XK_f:
			return JKS_f;
		case XK_g:
			return JKS_g;
		case XK_h:
			return JKS_h;
		case XK_i:
			return JKS_i;
		case XK_j:
			return JKS_j;
		case XK_k:
			return JKS_k;
		case XK_l:
			return JKS_l;
		case XK_m:
			return JKS_m;
		case XK_n:
			return JKS_n;
		case XK_o:
			return JKS_o;
		case XK_p:
			return JKS_p;
		case XK_q:
			return JKS_q;
		case XK_r:
			return JKS_r;
		case XK_s:
			return JKS_s;
		case XK_t:
			return JKS_t;
		case XK_u:
			return JKS_u;
		case XK_v:
			return JKS_v;
		case XK_x:
			return JKS_x;
		case XK_w:
			return JKS_w;
		case XK_y:
			return JKS_y;
		case XK_z:
			return JKS_z;
		case XK_Print:
			return JKS_PRINT;
		case XK_Break:
			return JKS_BREAK;
		case XK_exclam:
			return JKS_EXCLAMATION_MARK;
		case XK_quotedbl:
			return JKS_QUOTATION;
		case XK_numbersign:
			return JKS_NUMBER_SIGN;
		case XK_dollar:
			return JKS_DOLLAR_SIGN;
		case XK_percent:
			return JKS_PERCENT_SIGN;
		case XK_ampersand:
			return JKS_AMPERSAND;
		case XK_apostrophe:
			return JKS_APOSTROPHE;
		case XK_parenleft:
			return JKS_PARENTHESIS_LEFT;
		case XK_parenright:
			return JKS_PARENTHESIS_RIGHT;
		case XK_asterisk:
			return JKS_STAR;
		case XK_less:
			return JKS_LESS_THAN_SIGN;
		case XK_greater:
			return JKS_GREATER_THAN_SIGN;
		case XK_question:
			return JKS_QUESTION_MARK;
		case XK_at:
			return JKS_AT;
		case XK_asciicircum:
			return JKS_CIRCUMFLEX_ACCENT;
		case XK_grave:
			return JKS_GRAVE_ACCENT;
		case XK_bar:
			return JKS_VERTICAL_BAR;  
		case XK_braceleft:
			return JKS_CURLY_BRACKET_LEFT;
		case XK_braceright:
			return JKS_CURLY_BRACKET_RIGHT;
		case XK_asciitilde:
			return JKS_TILDE;
		case XK_underscore:
			return JKS_UNDERSCORE;
		case XK_acute:
			return JKS_ACUTE_ACCENT;
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
	_window = 0;
	_graphics = NULL;
	_is_running = false;
	_is_initialized = false;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = 0LL;
	_click_count = 1;
	_last_key_release_event.type = -1;
	_key_repeat = true;

	InternalInitialize();
}

NativeHandler::~NativeHandler()
{
}

/*
bool NativeInputManager::GrabKeyEvents(bool b)
{
	if (b == true) {
		XGrabKeyboard(_display, _window->_window, False, GrabModeAsync, GrabModeAsync, CurrentTime);

		return true;
	} else {
		XUngrabKeyboard(_display, CurrentTime);

		return true;
	}

	return false;
}

bool NativeInputManager::GrabMouseEvents(bool b)
{
	if (b == true) {
		XGrabPointer(_display, _window->_window, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | EnterWindowMask | LeaveWindowMask, GrabModeAsync, GrabModeAsync, root_window, None, CurrentTime);

		return true;
	} else {
		XUngrabPointer(_display, CurrentTime);

		return true;
	}

	return false;
}
*/

void NativeHandler::InternalInitialize()
{
	if (_is_initialized == true) {
		return;
	}

	// Open a connection with the X server
	_display = XOpenDisplay(NULL);

	if (_display == NULL) {
		throw jcommon::RuntimeException("Unable to connect with X server");
	}

	int screen = DefaultScreen(_display);

	_size.width = _screen.width = DisplayWidth(_display, screen);
	_size.height = _screen.height = DisplayHeight(_display, screen);

	XInitThreads();

	InternalInitCursors();

	_dispatcher = new InputEventDispatcher(this);

	_dispatcher->Start();

	_is_initialized = true;
}

void NativeHandler::InternalRelease()
{
	InternalReleaseCursors();

	// Close the connection with the X server
	XCloseDisplay(_display);

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
	
	XUndefineCursor(_display, XRootWindow(_display, DefaultScreen(_display)));

	if (_hidden_cursor) {
		XFreeCursor(_display, _hidden_cursor);
	}
}

// Filter the events received by windows (only allow those matching a specific window)
static Bool check_x11_event(Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast<Window >(userData);
}

void NativeHandler::MainLoop()
{
	XSetWindowAttributes attr;

	attr.event_mask = 0;
	attr.override_redirect = False;

	int screen = DefaultScreen(_display);

	_window = XCreateWindow(
			_display, 
			XRootWindow(_display, screen), 
			_location.x, 
			_location.y, 
			_size.width, 
			_size.height, 
			0, 
			DefaultDepth(_display, screen), 
			InputOutput, 
			DefaultVisual(_display, screen), 
			CWEventMask | CWOverrideRedirect, 
			&attr
	);

	if (_window == 0) {
		std::cout << "Unable to create a window" << std::endl;

		return;
	}

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(_display, "_MOTIF_WM_HINTS", False);

	if (WMHintsAtom) {
		static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
		static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

		enum mwm_decor_t {
			MWM_DECOR_ALL         = 1 << 0,
			MWM_DECOR_BORDER      = 1 << 1,
			MWM_DECOR_RESIZEH     = 1 << 2,
			MWM_DECOR_TITLE       = 1 << 3,
			MWM_DECOR_MENU        = 1 << 4,
			MWM_DECOR_MINIMIZE    = 1 << 5,
			MWM_DECOR_MAXIMIZE    = 1 << 6
		};

		enum mwm_func_t {
			MWM_FUNC_ALL          = 1 << 0,
			MWM_FUNC_RESIZE       = 1 << 1,
			MWM_FUNC_MOVE         = 1 << 2,
			MWM_FUNC_MINIMIZE     = 1 << 3,
			MWM_FUNC_MAXIMIZE     = 1 << 4,
			MWM_FUNC_CLOSE        = 1 << 5
		};

		struct WMHints {
			unsigned long Flags;
			unsigned long Functions;
			unsigned long Decorations;
			long          InputMode;
			unsigned long State;
		};

		WMHints hints;

		hints.Flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
		hints.Decorations = True;
		hints.Functions   = MWM_FUNC_MOVE | MWM_FUNC_RESIZE | MWM_FUNC_CLOSE;

		if (_is_undecorated == true) {
			hints.Decorations = False;
		}

		const unsigned char *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(_display, _window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}

	/*
	// This is a hack to force some windows managers to disable resizing
	XSizeHints sizeHints;

	sizeHints.flags = PMinSize | PMaxSize;
	sizeHints.min_width = sizeHints.max_width  = width;
	sizeHints.min_height = sizeHints.max_height = height;

	XSetWMNormalHints(_display, _window, &sizeHints); 
	*/

	NativeGraphics *native_graphics = new NativeGraphics(this, &_window, NULL, JPF_ARGB, _size.width, _size.height);

	_graphics = native_graphics;

	XMapWindow(_display, _window);

	XSelectInput(
			_display, 
			_window, 
			ExposureMask | EnterNotify | LeaveNotify | KeyPress | 
			KeyRelease | ButtonPress | ButtonRelease | MotionNotify | 
			PointerMotionMask | StructureNotifyMask | SubstructureNotifyMask
	);

	XEvent event;

	_is_running = true;

	_init_sem.Notify();

	int ox, oy;

	ox = _size.width;
	oy = _size.height;

	do {
		while (XCheckIfEvent(_display, &event, &check_x11_event, reinterpret_cast<XPointer>(_window))) {
			if (event.type == Expose) {
				native_graphics->InternalFlip();
			} else {
				InternalEventHandler(event);
			}
		}
	
		if (_need_destroy == true) {
			_need_destroy = false;

			if (_is_fullscreen_enabled == true) {
				ox = _size.width;
				oy = _size.height;
			
				_size.width = _screen.width;
				_size.height = _screen.height;
			
				XMoveResizeWindow(_display, _window, _location.x, _location.y, _size.width, _size.height);

				_graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);

				XEvent xev;

				Atom wm_state = XInternAtom(_display, "_NET_WM_STATE", False);
				Atom fullscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);

				memset(&xev, 0, sizeof(xev));

				xev.type = ClientMessage;
				xev.xclient.window = _window;
				xev.xclient.message_type = wm_state;
				xev.xclient.format = 32;
				xev.xclient.data.l[0] = 1;
				xev.xclient.data.l[1] = fullscreen;
				xev.xclient.data.l[2] = 0;

				XSendEvent(_display, XRootWindow(_display, DefaultScreen(_display)), False, SubstructureNotifyMask, &xev);
			} else {
				_size.width = ox;
				_size.height = oy;
				
				XMoveResizeWindow(_display, _window, _location.x, _location.y, _size.width, _size.height);

				_graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
			}
		}
	} while (_is_visible == true);

	// TODO:: destroy fullscreen ?

	XUnmapWindow(_display, _window);
	XDestroyWindow(_display, _window);
	XFlush(_display);
	XSync(_display, False);

	_window = 0;
}

void NativeHandler::SetFullScreenEnabled(bool b)
{
	if (_is_fullscreen_enabled == b) {
		return;
	}

	_is_fullscreen_enabled = b;
	
	_need_destroy = true;
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

	if (_is_visible == true) {
		// _window->setTitle(_title.c_str());
	}
}

void NativeHandler::SetOpacity(int i)
{
}

void NativeHandler::SetUndecorated(bool b)
{
	_is_undecorated = b;

	if (_is_visible == false) {
		return;
	}

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(_display, "_MOTIF_WM_HINTS", False);

	if (WMHintsAtom) {
		static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
		static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

		enum mwm_decor_t {
			MWM_DECOR_ALL         = 1 << 0,
			MWM_DECOR_BORDER      = 1 << 1,
			MWM_DECOR_RESIZEH     = 1 << 2,
			MWM_DECOR_TITLE       = 1 << 3,
			MWM_DECOR_MENU        = 1 << 4,
			MWM_DECOR_MINIMIZE    = 1 << 5,
			MWM_DECOR_MAXIMIZE    = 1 << 6
		};

		enum mwm_func_t {
			MWM_FUNC_ALL          = 1 << 0,
			MWM_FUNC_RESIZE       = 1 << 1,
			MWM_FUNC_MOVE         = 1 << 2,
			MWM_FUNC_MINIMIZE     = 1 << 3,
			MWM_FUNC_MAXIMIZE     = 1 << 4,
			MWM_FUNC_CLOSE        = 1 << 5
		};

		struct WMHints {
			unsigned long Flags;
			unsigned long Functions;
			unsigned long Decorations;
			long          InputMode;
			unsigned long State;
		};

		WMHints hints;

		hints.Flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
		hints.Decorations = True;
		hints.Functions   = MWM_FUNC_MOVE | MWM_FUNC_CLOSE;

		if (_is_undecorated == true) {
			hints.Decorations = False;
		}

		const unsigned char *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(_display, _window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}
}

bool NativeHandler::IsUndecorated()
{
	return _is_undecorated;
}

void NativeHandler::SetVerticalSyncEnabled(bool b)
{
	jgui::Application::SetVerticalSyncEnabled(b);
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
		
	XMoveResizeWindow(_display, _window, _location.x, _location.y, _size.width, _size.height);

	_graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
}

void NativeHandler::SetLocation(int x, int y)
{
	Application::SetLocation(x, y);
	
	if (_is_visible == false) {
		return;
	}

	XMoveWindow(_display, _window, _location.x, _location.y);
}

void NativeHandler::SetSize(int width, int height)
{
	Application::SetSize(width, height);
	
	if (_is_visible == false) {
		return;
	}

	XResizeWindow(_display, _window, _size.width, _size.height);

	_graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
}

void NativeHandler::Move(int x, int y)
{
	Application::Move(x, y);
	
	if (_is_visible == false) {
		return;
	}

	XMoveWindow(_display, _window, _location.x, _location.y);
}

void NativeHandler::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > _screen.width) {
		x = _screen.width;
	}

	if (y > _screen.height) {
		y = _screen.height;
	}

	XWarpPointer(_display, None, _window, 0, 0, 0, 0, x, y);
	
	// XWarpPointer(_display, None, XRootWindow(_display, DefaultScreen(_display)), 0, 0, 0, 0, x, y);
	
	XFlush(_display);
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	::Window child_return;

	XTranslateCoordinates(_display, _window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &p.x, &p.y, &child_return);

	return p;
}

void NativeHandler::SetCursorEnabled(bool b)
{
	jgui::Application::SetCursorEnabled(b);

	if (_is_visible == false) {
		return;
	}

	// XDefineCursor(_display, _window, _is_cursor_enabled);
	// XFlush(_display);
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

	// XChangeWindowAttributes() this funciontion change attributes like cursor
	//
	
	/*
	if ((void *)shape == NULL) {
		return;
	}

	uint32_t *data = NULL;

	jsize_t t = shape->GetSize();
	
	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == NULL) {
		return;
	}

	// Create the icon pixmap
	int screen = DefaultScreen(_display);
	Visual *visual = DefaultVisual(_display, screen);
	unsigned int depth = DefaultDepth(_display, screen);
	XImage *image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char *)data, t.width, t.height, 32, 0);
	::Window root_window = XRootWindow(_display, screen);

	if (image == NULL) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(_display, RootWindow(_display, screen), t.width, t.height, depth);
	GC gc = XCreateGC(_display, pixmap, 0, NULL);
	
	XPutImage(_display, pixmap, gc, image, 0, 0, 0, 0, t.width, t.height);

	XColor color;

	color.flags = DoRed | DoGreen | DoBlue;
	color.red = 0;
	color.green = 0;
	color.blue = 0;

	Cursor cursor = XCreatePixmapCursor(_display, pixmap, pixmap, &color, &color, 0, 0);

	// XUndefineCursor(_display, root_window);
	XDefineCursor(_display, root_window, cursor);
	XSync(_display, root_window);
	XFreePixmap(_display, pixmap);

	delete [] data;
	*/
}

Display * NativeHandler::GetDisplay()
{
	return _display;
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

void NativeHandler::InternalEventHandler(XEvent event)
{
	// This function implements a workaround to properly discard repeated key events when necessary. 
	// The problem is that the system's key events policy doesn't match SFML's one: X server will 
	// generate both repeated KeyPress and KeyRelease events when maintaining a key down, while SFML 
	// only wants repeated KeyPress events. Thus, we have to:
	//   - Discard duplicated KeyRelease events when EnableKeyRepeat is true
	//   - Discard both duplicated KeyPress and KeyRelease events when EnableKeyRepeat is false

	// Detect repeated key events
	if ((event.type == KeyPress) || (event.type == KeyRelease)) {
		if (event.xkey.keycode < 256) {
			// To detect if it is a repeated key event, we check the current state of the key.
			// - If the state is "down", KeyReleased events must obviously be discarded.
			// - KeyPress events are a little bit harder to handle: they depend on the EnableKeyRepeat state,
			//   and we need to properly forward the first one.
			char keys[32];
			
			XQueryKeymap(_display, keys);

			if (keys[event.xkey.keycode / 8] & (1 << (event.xkey.keycode % 8))) {
				// KeyRelease event + key down = repeated event --> discard
				if (event.type == KeyRelease) {
					_last_key_release_event = event;
					
					return;
				}

				// KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
				if ((event.type == KeyPress) && !_key_repeat &&
						(_last_key_release_event.xkey.keycode == event.xkey.keycode) && 
						(_last_key_release_event.xkey.time == event.xkey.time)) {
					return;
				}
			}
		}
	}

	NativeGraphics *graphics = dynamic_cast<NativeGraphics *>(_graphics);

	if (event.type == DestroyNotify) {
		printf("Event:: DestroyNotify\n");

		SetVisible(false);
		
		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSED));
	} else if (event.type == MapNotify) {
		// printf("Event:: MapNotify\n");

		// INFO:: avoid any draw before MapNotify's event
	} else if (event.type == ExposureMask) {
		printf("Event:: ExposureMask\n");
	} else if (event.type == EnterNotify) {
		printf("Event:: EnterNotify\n");
		
		SetCursor(GetCursor());

		DispatchWidgetEvent(new WidgetEvent(this, JWET_ENTERED));
	} else if (event.type == LeaveNotify) {
		printf("Event:: LeaveNotify\n");
		
		SetCursor(JCS_DEFAULT);

		DispatchWidgetEvent(new WidgetEvent(this, JWET_LEAVED));
	} else if (event.type == FocusIn) {
		// printf("Event:: FocusIn\n");

		/*
		if (m_inputContext) {
			XSetICFocus(m_inputContext);
		}

		Event e;
		
		e.type = Event::GainedFocus;
		
		pushEvent(e);
		*/
		
		DispatchWidgetEvent(new WidgetEvent(this, JWET_OPENED));
	} else if (event.type == FocusOut) {
		// printf("Event:: FocusOut\n");

		/*
		if (m_inputContext) {
			XUnsetICFocus(m_inputContext);
		}

		Event e;

		e.type = Event::LostFocus;
		
		pushEvent(e);
		*/
	} else if (event.type == ConfigureNotify) {
		_size.width = event.xconfigure.width;
		_size.height = event.xconfigure.height;

		graphics->SetNativeSurface(&_window, _size.width, _size.height);
		graphics->ReleaseFlip();

		Repaint();

		DispatchWidgetEvent(new WidgetEvent(this, JWET_RESIZED));
	} else if (event.type == ClientMessage) {
		// printf("Event:: ClientMessage\n");

		/*
		if ((windowEvent.xclient.format == 32) && (windowEvent.xclient.data.l[0]) == static_cast<long>(m_atomClose)) {
			Event e;
			
			e.type = Event::Closed;
			
			pushEvent(e);
		}
		*/
	} else if (event.type == KeyPress || event.type == KeyRelease) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if (event.xkey.state & ShiftMask) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		}

		if (event.xkey.state & ControlMask) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		}

		if (event.xkey.state & Mod1Mask) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == KeyPress) {
			type = JKT_PRESSED;

			// TODO:: grab pointer events
		} else if (event.type == KeyRelease) {
			type = JKT_RELEASED;
			
			// TODO:: ungrab pointer events
		}

		static XComposeStatus keyboard;

		char buffer[32];
		KeySym sym;

		XLookupString(&event.xkey, buffer, sizeof(buffer), &sym, &keyboard);

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(sym);

		_dispatcher->PostEvent(new KeyEvent(this, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == ButtonPress || event.type == ButtonRelease || event.type == MotionNotify) {
		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == MotionNotify) {
			type = JMT_MOVED;
		
			_mouse_x = event.xmotion.x;
			_mouse_y = event.xmotion.y;
		} else if (event.type == ButtonPress || event.type == ButtonRelease) {
			if (event.type == ButtonPress) {
				type = JMT_PRESSED;
			} else if (event.type == ButtonRelease) {
				type = JMT_RELEASED;
			}

			_mouse_x = event.xbutton.x;
			_mouse_y = event.xbutton.y;

			if (event.xbutton.button == Button1) {
				button = JMB_BUTTON1;
			} else if (event.xbutton.button == Button2) {
				button = JMB_BUTTON2;
			} else if (event.xbutton.button == Button3) {
				button = JMB_BUTTON3;
			} else if (event.xbutton.button == Button4) {
				if (type == JMT_RELEASED) {
					return;
				}

				type = JMT_ROTATED;
				button = JMB_WHEEL;
				mouse_z = -1;
			} else if (event.xbutton.button == Button5) {
				if (type == JMT_RELEASED) {
					return;
				}

				type = JMT_ROTATED;
				button = JMB_WHEEL;
				mouse_z = 1;
			}
		
			if (type == JMT_PRESSED) {
				if ((jcommon::Date::CurrentTimeMillis()-_last_keypress) < 200L) {
					_click_count = _click_count + 1;
				} else {
					_click_count = 1;
				}

				_last_keypress = jcommon::Date::CurrentTimeMillis();

				mouse_z = _click_count;
			}
		}

		if (event.xbutton.state & Button1) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		}

		if (event.xbutton.state & Button2) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		}

		if (event.xbutton.state & Button3) {
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

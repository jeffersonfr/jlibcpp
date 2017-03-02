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

#define SFML_ENGINE_DEFAULT_FRAME_RATE	120

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

static jkeyevent_symbol_t TranslateToNativeKeySymbol(sf::Keyboard::Key symbol)
{
	switch (symbol) {
		case sf::Keyboard::Return:
			return JKS_ENTER; // JKS_RETURN;
		case sf::Keyboard::BackSpace:
			return JKS_BACKSPACE;
		case sf::Keyboard::Tab:
			return JKS_TAB;
		// case sf::Keyboard::CANCEL:
		//	return JKS_CANCEL;
		case sf::Keyboard::Escape:
			return JKS_ESCAPE;
		case sf::Keyboard::Space:
			return JKS_SPACE;
		// case sf::Keyboard::EXCLAIM:
		//	return JKS_EXCLAMATION_MARK;
		// case sf::Keyboard::QUOTEDBL:
		//	return JKS_QUOTATION;
		// case sf::Keyboard::HASH:
		//	return JKS_NUMBER_SIGN;
		// case sf::Keyboard::DOLLAR:
		//	return JKS_DOLLAR_SIGN;
		// case sf::Keyboard::PERCENT_SIGN:
		//	return JKS_PERCENT_SIGN;
		// case sf::Keyboard::AMPERSAND:   
		//	return JKS_AMPERSAND;
		case sf::Keyboard::Quote:
			return JKS_APOSTROPHE;
		// case sf::Keyboard::LEFTPAREN:
		//	return JKS_PARENTHESIS_LEFT;
		// case sf::Keyboard::RIGHTPAREN:
		//	return JKS_PARENTHESIS_RIGHT;
		case sf::Keyboard::Multiply:
			return JKS_STAR;
		case sf::Keyboard::Add:
			return JKS_PLUS_SIGN;
		case sf::Keyboard::Comma:   
			return JKS_COMMA;
		case sf::Keyboard::Dash:
		case sf::Keyboard::Subtract:
			return JKS_MINUS_SIGN;
		case sf::Keyboard::Period:  
			return JKS_PERIOD;
		case sf::Keyboard::Divide:
		case sf::Keyboard::Slash:
			return JKS_SLASH;
		case sf::Keyboard::Numpad0: 
		case sf::Keyboard::Num0: 
			return JKS_0;
		case sf::Keyboard::Numpad1: 
		case sf::Keyboard::Num1:
			return JKS_1;
		case sf::Keyboard::Numpad2: 
		case sf::Keyboard::Num2:
			return JKS_2;
		case sf::Keyboard::Numpad3: 
		case sf::Keyboard::Num3:
			return JKS_3;
		case sf::Keyboard::Numpad4: 
		case sf::Keyboard::Num4:
			return JKS_4;
		case sf::Keyboard::Numpad5: 
		case sf::Keyboard::Num5:
			return JKS_5;
		case sf::Keyboard::Numpad6: 
		case sf::Keyboard::Num6:
			return JKS_6;
		case sf::Keyboard::Numpad7: 
		case sf::Keyboard::Num7:
			return JKS_7;
		case sf::Keyboard::Numpad8: 
		case sf::Keyboard::Num8:
			return JKS_8;
		case sf::Keyboard::Numpad9: 
		case sf::Keyboard::Num9:
			return JKS_9;
		// case sf::Keyboard::COLON:
		//	return JKS_COLON;
		case sf::Keyboard::SemiColon:
			return JKS_SEMICOLON;
		// case sf::Keyboard::LESS:
		// 	return JKS_LESS_THAN_SIGN;
		case sf::Keyboard::Equal: 
			return JKS_EQUALS_SIGN;
		// case sf::Keyboard::GREATER:
		//	return JKS_GREATER_THAN_SIGN;
		// case sf::Keyboard::QUESTION:   
		//	return JKS_QUESTION_MARK;
		// case sf::Keyboard::AT:
		//	return JKS_AT;
		case sf::Keyboard::A:
			return JKS_a;
		case sf::Keyboard::B:
			return JKS_b;
		case sf::Keyboard::C:
			return JKS_c;
		case sf::Keyboard::D:
			return JKS_d;
		case sf::Keyboard::E:
			return JKS_e;
		case sf::Keyboard::F:
			return JKS_f;
		case sf::Keyboard::G:
			return JKS_g;
		case sf::Keyboard::H:
			return JKS_h;
		case sf::Keyboard::I:
			return JKS_i;
		case sf::Keyboard::J:
			return JKS_j;
		case sf::Keyboard::K:
			return JKS_k;
		case sf::Keyboard::L:
			return JKS_l;
		case sf::Keyboard::M:
			return JKS_m;
		case sf::Keyboard::N:
			return JKS_n;
		case sf::Keyboard::O:
			return JKS_o;
		case sf::Keyboard::P:
			return JKS_p;
		case sf::Keyboard::Q:
			return JKS_q;
		case sf::Keyboard::R:
			return JKS_r;
		case sf::Keyboard::S:
			return JKS_s;
		case sf::Keyboard::T:
			return JKS_t;
		case sf::Keyboard::U:
			return JKS_u;
		case sf::Keyboard::V:
			return JKS_v;
		case sf::Keyboard::W:
			return JKS_w;
		case sf::Keyboard::X:
			return JKS_x;
		case sf::Keyboard::Y:
			return JKS_y;
		case sf::Keyboard::Z:
			return JKS_z;
		case sf::Keyboard::LBracket:
			return JKS_SQUARE_BRACKET_LEFT;
		case sf::Keyboard::BackSlash:   
			return JKS_BACKSLASH;
		case sf::Keyboard::RBracket:
			return JKS_SQUARE_BRACKET_RIGHT;
		// case sf::Keyboard::CARET:
		//	return JKS_CIRCUMFLEX_ACCENT;
		// case sf::Keyboard::UNDERSCORE:    
		//	return JKS_UNDERSCORE;
		// case sf::Keyboard::BACKQUOTE:
		//	return JKS_GRAVE_ACCENT;
		// case sf::Keyboard::CURLY_BRACKET_LEFT:
		//	return JKS_CURLY_BRACKET_LEFT;
		// case sf::Keyboard::VERTICAL_BAR:  
		//	return JKS_VERTICAL_BAR;
		// case sf::Keyboard::CURLY_BRACKET_RIGHT:
		//	return JKS_CURLY_BRACKET_RIGHT;
		case sf::Keyboard::Tilde:  
			return JKS_TILDE;
		case sf::Keyboard::Delete:
			return JKS_DELETE;
		case sf::Keyboard::Left:
			return JKS_CURSOR_LEFT;
		case sf::Keyboard::Right:
			return JKS_CURSOR_RIGHT;
		case sf::Keyboard::Up:
			return JKS_CURSOR_UP;
		case sf::Keyboard::Down:
			return JKS_CURSOR_DOWN;
		case sf::Keyboard::Insert:  
			return JKS_INSERT;
		case sf::Keyboard::Home:     
			return JKS_HOME;
		case sf::Keyboard::End:
			return JKS_END;
		case sf::Keyboard::PageUp:
			return JKS_PAGE_UP;
		case sf::Keyboard::PageDown:
			return JKS_PAGE_DOWN;
		// case sf::Keyboard::PRINT:   
		//	return JKS_PRINT;
		case sf::Keyboard::Pause:
			return JKS_PAUSE;
		// case sf::Keyboard::RED:
		//	return JKS_RED;
		// case sf::Keyboard::GREEN:
		//	return JKS_GREEN;
		// case sf::Keyboard::YELLOW:
		//	return JKS_YELLOW;
		// case sf::Keyboard::BLUE:
		//	return JKS_BLUE;
		case sf::Keyboard::F1:
			return JKS_F1;
		case sf::Keyboard::F2:
			return JKS_F2;
		case sf::Keyboard::F3:
			return JKS_F3;
		case sf::Keyboard::F4:
			return JKS_F4;
		case sf::Keyboard::F5:
			return JKS_F5;
		case sf::Keyboard::F6:     
			return JKS_F6;
		case sf::Keyboard::F7:    
			return JKS_F7;
		case sf::Keyboard::F8:   
			return JKS_F8;
		case sf::Keyboard::F9:  
			return JKS_F9;
		case sf::Keyboard::F10: 
			return JKS_F10;
		case sf::Keyboard::F11:
			return JKS_F11;
		case sf::Keyboard::F12:
			return JKS_F12;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			return JKS_SHIFT;
		case sf::Keyboard::LControl:
		case sf::Keyboard::RControl:
			return JKS_CONTROL;
		case sf::Keyboard::LAlt:
		case sf::Keyboard::RAlt:
			return JKS_ALT;
		// case sf::Keyboard::ALTGR:
		//	return JKS_ALTGR;
		// case sf::Keyboard::LMETA:
		// case sf::Keyboard::RMETA:
		//	return JKS_META;
		// case sf::Keyboard::LSUPER:
		// case sf::Keyboard::RSUPER:
		//	return JKS_SUPER;
		case sf::Keyboard::RSystem:
			return JKS_HYPER;
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

	XInitThreads();

	sf::VideoMode display = sf::VideoMode::getDesktopMode();

	_size.width = _screen.width = display.width;
	_size.height = _screen.height = display.height;

	InternalInitCursors();

	_dispatcher = new InputEventDispatcher(this);

	_dispatcher->Start();

	_is_initialized = true;
}

void NativeHandler::InternalRelease()
{
	InternalReleaseCursors();

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
	int flags = (int)(sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

	if (_is_undecorated == true) {
		flags = (int)(0); // sf::Style::None);
	}

	_window = new sf::RenderWindow(sf::VideoMode(_size.width, _size.height), _title.c_str(), flags);

	_window->requestFocus();
	_window->setVerticalSyncEnabled(_is_vertical_sync_enabled);
	_window->setFramerateLimit(SFML_ENGINE_DEFAULT_FRAME_RATE);

	NativeGraphics *native_graphics = new NativeGraphics(_window, NULL, JPF_ARGB, _size.width, _size.height);

	_graphics = native_graphics;

	_is_running = true;

	int ox, oy;

	ox = _size.width;
	oy = _size.height;

	_window->setActive(false);

	_init_sem.Notify();

	while (_window->isOpen() == true) {
		if (_is_running == false) {
			break;
		}

		sf::Event event;

		// while (window->waitEvent(event)) {
		while (_window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				break;
			} else {
				InternalEventHandler(event);
			}
		}

		if (_need_destroy == true) {
			_need_destroy = false;

			_window->close();
			_window = NULL;

			int flags = (int)(sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

			if (_is_undecorated == true) {
				flags = (int)(0); // sf::Style::None;
			}

			if (_is_fullscreen_enabled == true) {
				sf::VideoMode mode = sf::VideoMode::getDesktopMode();

				flags = (int)(sf::Style::Fullscreen);

				ox = _size.width;
				oy = _size.height;
			
				_size.width = mode.width;
				_size.height = mode.height;
			
				_window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), _title.c_str(), flags);
			} else {
				_size.width = ox;
				_size.height = oy;
				
				_window = new sf::RenderWindow(sf::VideoMode(_size.width, _size.height), _title.c_str(), flags);
			}

			_window->requestFocus();
			_window->setVerticalSyncEnabled(_is_vertical_sync_enabled);
			_window->setFramerateLimit(SFML_ENGINE_DEFAULT_FRAME_RATE);

			native_graphics->SetNativeSurface(_window, _size.width, _size.height);
		}
	}

	delete _graphics;
	_graphics = NULL;

	_window->close();
	_window = NULL;
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
		_window->setTitle(_title.c_str());
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

	_need_destroy = true;
}

bool NativeHandler::IsUndecorated()
{
	return _is_undecorated;
}

void NativeHandler::SetVerticalSyncEnabled(bool b)
{
	jgui::Application::SetVerticalSyncEnabled(b);

	if (_is_visible == false) {
		return;
	}

	_window->setFramerateLimit(SFML_ENGINE_DEFAULT_FRAME_RATE);
	_window->setVerticalSyncEnabled(_is_vertical_sync_enabled);
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
		
	_window->setPosition(sf::Vector2i(_location.x, _location.y));
	_window->setSize(sf::Vector2u(_size.width, _size.height));
	_graphics->SetNativeSurface((void *)_window, _size.width, _size.height);
}

void NativeHandler::SetLocation(int x, int y)
{
	Application::SetLocation(x, y);
	
	if (_is_visible == false) {
		return;
	}

	_window->setPosition(sf::Vector2i(_location.x, _location.y));
}

void NativeHandler::SetSize(int width, int height)
{
	Application::SetSize(width, height);
	
	if (_is_visible == false) {
		return;
	}

	_window->setSize(sf::Vector2u(_size.width, _size.height));
	_graphics->SetNativeSurface((void *)_window, _size.width, _size.height);
}

void NativeHandler::Move(int x, int y)
{
	Application::Move(x, y);
	
	if (_is_visible == false) {
		return;
	}

	_window->setPosition(sf::Vector2i(_location.x, _location.y));
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

	sf::Mouse::setPosition(sf::Vector2i(x, y));
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	sf::Vector2i pos = sf::Mouse::getPosition();

	p.x = pos.x;
	p.y = pos.y;

	return p;
}

void NativeHandler::SetCursorEnabled(bool b)
{
	jgui::Application::SetCursorEnabled(b);

	if (_is_visible == false) {
		return;
	}

	_window->setMouseCursorVisible(_is_cursor_enabled);
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

void NativeHandler::InternalEventHandler(sf::Event event)
{
	if (event.type == sf::Event::MouseEntered) {
		SetCursor(GetCursor());

		DispatchWidgetEvent(new WidgetEvent(this, JWET_ENTERED));
	} else if (event.type == sf::Event::MouseLeft) {
		SetCursor(JCS_DEFAULT);

		DispatchWidgetEvent(new WidgetEvent(this, JWET_LEAVED));
	} else if (event.type == sf::Event::Closed) {
		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSED));
	} else if (event.type == sf::Event::Resized) {
		_size.width = event.size.width;
		_size.height = event.size.height;

		_graphics->SetNativeSurface((void *)_window, _size.width, _size.height);
		
		Repaint();

		DispatchWidgetEvent(new WidgetEvent(this, JWET_RESIZED));
	} else if (event.type == sf::Event::LostFocus) {
	} else if (event.type == sf::Event::GainedFocus) {
		DispatchWidgetEvent(new WidgetEvent(this, JWET_OPENED));
	} else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		bool control = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
		bool alt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

		if (shift == true) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		}

		if (control == true) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		}

		if (alt == true) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == sf::Event::KeyPressed) {
			type = JKT_PRESSED;

			// TODO:: grab pointer events
		} else if (event.type == sf::Event::KeyReleased) {
			type = JKT_RELEASED;
			
			// TODO:: ungrab pointer events
		}

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.code);

		_dispatcher->PostEvent(new KeyEvent(_window, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (
			event.type == sf::Event::MouseMoved || 
			event.type == sf::Event::MouseButtonPressed || 
			event.type == sf::Event::MouseButtonReleased ||
			event.type == sf::Event::MouseWheelMoved ||
			event.type == sf::Event::MouseWheelScrolled) {
		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == sf::Event::MouseMoved) {
			type = JMT_MOVED;
		
			_mouse_x = event.mouseMove.x;
			_mouse_y = event.mouseMove.y;
		} else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
			if (event.type == sf::Event::MouseButtonPressed) {
				type = JMT_PRESSED;
			} else if (event.type == sf::Event::MouseButtonReleased) {
				type = JMT_RELEASED;
			}

			_mouse_x = event.mouseButton.x;
			_mouse_y = event.mouseButton.y;

			if (event.mouseButton.button == sf::Mouse::Left) {
				button = JMB_BUTTON1;
			} else if (event.mouseButton.button == sf::Mouse::Middle) {
				button = JMB_BUTTON2;
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				button = JMB_BUTTON3;
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
		} else if (event.type == sf::Event::MouseWheelMoved || event.type == sf::Event::MouseWheelScrolled) {
			type = JMT_ROTATED;

			if (event.type == sf::Event::MouseWheelMoved) {
				_mouse_x = event.mouseWheel.x;
				_mouse_y = event.mouseWheel.y;
				mouse_z = event.mouseWheel.delta;
			} else if (event.type == sf::Event::MouseWheelScrolled) {
				_mouse_x = event.mouseWheelScroll.x;
				_mouse_y = event.mouseWheelScroll.y;
				mouse_z = event.mouseWheelScroll.delta;
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		// AddEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
		_dispatcher->PostEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	} else if (event.type == sf::Event::TouchBegan) {
	} else if (event.type == sf::Event::TouchMoved) {
	} else if (event.type == sf::Event::TouchEnded) {
	}
}

void NativeHandler::Run()
{
	MainLoop();
}

}

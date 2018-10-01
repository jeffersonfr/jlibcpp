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
#include "binds/include/nativeapplication.h"
#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <X11/Xlib.h>

namespace jgui {

/** \brief */
static int _window = 0;
/** \brief */
static jgui::Image *_icon = nullptr;
/** \brief */
static std::chrono::time_point<std::chrono::steady_clock> _last_keypress;
/** \brief */
static int _mouse_x;
/** \brief */
static int _mouse_y;
/** \brief */
static int _click_count;
/** \brief */
static Window *g_window = nullptr;
/** \brief */
static std::string _title;
/** \brief */
static bool _visible = true;
/** \brief */
static bool _fullscreen = false;
/** \brief */
static jgui::jregion_t _previous_bounds;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
	switch (symbol) {
		case '\r':
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		//case SDLK_BACKSPACE:
		//	return jevent::JKS_BACKSPACE;
		case '\t':
			return jevent::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jevent::JKS_CANCEL;
		case 27:
			return jevent::JKS_ESCAPE;
    case ' ':
			return jevent::JKS_SPACE;
		case '!':
			return jevent::JKS_EXCLAMATION_MARK;
		case '"':
			return jevent::JKS_QUOTATION;
		case '#':
			return jevent::JKS_NUMBER_SIGN;
		case '$':
			return jevent::JKS_DOLLAR_SIGN;
		case '%':
			return jevent::JKS_PERCENT_SIGN;
		case '&':   
			return jevent::JKS_AMPERSAND;
		case '\'':
			return jevent::JKS_APOSTROPHE;
		case '(':
			return jevent::JKS_PARENTHESIS_LEFT;
		case ')':
			return jevent::JKS_PARENTHESIS_RIGHT;
		case '*':
			return jevent::JKS_STAR;
		case '+':
			return jevent::JKS_PLUS_SIGN;
		case ',':   
			return jevent::JKS_COMMA;
		case '-':
			return jevent::JKS_MINUS_SIGN;
		case '.':  
			return jevent::JKS_PERIOD;
		case '/':
			return jevent::JKS_SLASH;
		case '0':     
			return jevent::JKS_0;
		case '1':
			return jevent::JKS_1;
		case '2':
			return jevent::JKS_2;
		case '3':
			return jevent::JKS_3;
		case '4':
			return jevent::JKS_4;
		case '5':
			return jevent::JKS_5;
		case '6':
			return jevent::JKS_6;
		case '7':
			return jevent::JKS_7;
		case '8':
			return jevent::JKS_8;
		case '9':
			return jevent::JKS_9;
    case ':':
			return jevent::JKS_COLON;
		case ';':
			return jevent::JKS_SEMICOLON;
		case '<':
			return jevent::JKS_LESS_THAN_SIGN;
		case '=': 
			return jevent::JKS_EQUALS_SIGN;
		case '>':
			return jevent::JKS_GREATER_THAN_SIGN;
		case '?':   
			return jevent::JKS_QUESTION_MARK;
		case '@':
			return jevent::JKS_AT;
		case 'A':
			return jevent::JKS_A;
		case 'B':
			return jevent::JKS_B;
		case 'C':
			return jevent::JKS_C;
		case 'D':
			return jevent::JKS_D;
		case 'E':
			return jevent::JKS_E;
		case 'F':
			return jevent::JKS_F;
		case 'G':
			return jevent::JKS_G;
		case 'H':
			return jevent::JKS_H;
		case 'I':
			return jevent::JKS_I;
		case 'J':
			return jevent::JKS_J;
		case 'K':
			return jevent::JKS_K;
		case 'L':
			return jevent::JKS_L;
		case 'M':
			return jevent::JKS_M;
		case 'N':
			return jevent::JKS_N;
		case 'O':
			return jevent::JKS_O;
		case 'P':
			return jevent::JKS_P;
		case 'Q':
			return jevent::JKS_Q;
		case 'R':
			return jevent::JKS_R;
		case 'S':
			return jevent::JKS_S;
		case 'T':
			return jevent::JKS_T;
		case 'U':
			return jevent::JKS_U;
		case 'V':
			return jevent::JKS_V;
		case 'W':
			return jevent::JKS_W;
		case 'X':
			return jevent::JKS_X;
		case 'Y':
			return jevent::JKS_Y;
		case 'Z':
			return jevent::JKS_Z;
		case '[':
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case '\\':   
			return jevent::JKS_BACKSLASH;
		case ']':
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case '^':
			return jevent::JKS_CIRCUMFLEX_ACCENT;
		case '_':    
			return jevent::JKS_UNDERSCORE;
		case '`':
			return jevent::JKS_GRAVE_ACCENT;
		case 'a':       
			return jevent::JKS_a;
		case 'b':
			return jevent::JKS_b;
		case 'c':
			return jevent::JKS_c;
		case 'd':
			return jevent::JKS_d;
		case 'e':
			return jevent::JKS_e;
		case 'f':
			return jevent::JKS_f;
		case 'g':
			return jevent::JKS_g;
		case 'h':
			return jevent::JKS_h;
		case 'i':
			return jevent::JKS_i;
		case 'j':
			return jevent::JKS_j;
		case 'k':
			return jevent::JKS_k;
		case 'l':
			return jevent::JKS_l;
		case 'm':
			return jevent::JKS_m;
		case 'n':
			return jevent::JKS_n;
		case 'o':
			return jevent::JKS_o;
		case 'p':
			return jevent::JKS_p;
		case 'q':
			return jevent::JKS_q;
		case 'r':
			return jevent::JKS_r;
		case 's':
			return jevent::JKS_s;
		case 't':
			return jevent::JKS_t;
		case 'u':
			return jevent::JKS_u;
		case 'v':
			return jevent::JKS_v;
		case 'w':
			return jevent::JKS_w;
		case 'x':
			return jevent::JKS_x;
		case 'y':
			return jevent::JKS_y;
		case 'z':
			return jevent::JKS_z;
		case '{':
			return jevent::JKS_CURLY_BRACKET_LEFT;
		case '|':  
			return jevent::JKS_VERTICAL_BAR;
		case '}':
			return jevent::JKS_CURLY_BRACKET_RIGHT;
		case '~':  
			return jevent::JKS_TILDE;
		//case SDLK_DELETE:
		//	return jevent::JKS_DELETE;
		// case SDLK_PRINT:   
		//	return jevent::JKS_PRINT;
		//case SDLK_PAUSE:
		//	return jevent::JKS_PAUSE;
		// case SDLK_RED:
		//	return jevent::JKS_RED;
		// case SDLK_GREEN:
		//	return jevent::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jevent::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jevent::JKS_BLUE;
		//case SDLK_LSHIFT:
		//case SDLK_RSHIFT:
		//	return jevent::JKS_SHIFT;
		//case SDLK_LCTRL:
		//case SDLK_RCTRL:
		//	return jevent::JKS_CONTROL;
		//case SDLK_LALT:
		//case SDLK_RALT:
		//	return jevent::JKS_ALT;
		// case SDLK_ALTGR:
		//	return jevent::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jevent::JKS_META;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return jevent::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jevent::JKS_HYPER;
		default: 
			break;
	}

	return jevent::JKS_UNKNOWN;
}

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbolSpecial(int symbol)
{
	switch (symbol) {
		case GLUT_KEY_LEFT:
			return jevent::JKS_CURSOR_LEFT;
		case GLUT_KEY_RIGHT:
			return jevent::JKS_CURSOR_RIGHT;
		case GLUT_KEY_UP:  
			return jevent::JKS_CURSOR_UP;
		case GLUT_KEY_DOWN:
			return jevent::JKS_CURSOR_DOWN;
		case GLUT_KEY_INSERT:  
	  	return jevent::JKS_INSERT;
	  case GLUT_KEY_HOME:     
			return jevent::JKS_HOME;
		case GLUT_KEY_END:
			return jevent::JKS_END;
		case GLUT_KEY_PAGE_UP:
			return jevent::JKS_PAGE_UP;
		case GLUT_KEY_PAGE_DOWN:
			return jevent::JKS_PAGE_DOWN;
		case GLUT_KEY_F1:
			return jevent::JKS_F1;
		case GLUT_KEY_F2:
			return jevent::JKS_F2;
		case GLUT_KEY_F3:
			return jevent::JKS_F3;
		case GLUT_KEY_F4:
			return jevent::JKS_F4;
		case GLUT_KEY_F5:
			return jevent::JKS_F5;
    case GLUT_KEY_F6:
			return jevent::JKS_F6;
		case GLUT_KEY_F7:
			return jevent::JKS_F7;
		case GLUT_KEY_F8:
			return jevent::JKS_F8;
		case GLUT_KEY_F9:
			return jevent::JKS_F9;
		case GLUT_KEY_F10: 
			return jevent::JKS_F10;
		case GLUT_KEY_F11:
			return jevent::JKS_F11;
		case GLUT_KEY_F12:
			return jevent::JKS_F12;
		default: 
			break;
	}

	return jevent::JKS_UNKNOWN;
}

static jgui::jsize_t _screen = {0, 0};

NativeApplication::NativeApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeApplication");
}

NativeApplication::~NativeApplication()
{
}

void NativeApplication::InternalInit(int argc, char **argv)
{
	XInitThreads();

  glutInit(&argc, argv);
  // glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB | GLUT_MULTISAMPLE);

	_screen.width = glutGet(GLUT_SCREEN_WIDTH);
	_screen.height = glutGet(GLUT_SCREEN_HEIGHT);
}

void NativeApplication::InternalLoop()
{
  glutMainLoop();
  // glutMainLoopEvent(); // single loop event

  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
  glutLeaveMainLoop();
}

void OnDraw()
{
	if (g_window == nullptr || g_window->IsVisible() == false) {
		return;
	}

  jregion_t 
    bounds = g_window->GetVisibleBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_ARGB, bounds.width, bounds.height);
  jgui::Graphics 
    *g = buffer->GetGraphics();
	jpoint_t 
    t = g->Translate();

	g->Reset();
	g->Translate(-t.x, -t.y);
  g->SetClip(0, 0, bounds.width, bounds.height);
	g_window->DoLayout();
  g_window->Paint(g);
	g->Translate(t.x, t.y);

  cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

  if (cairo_surface == nullptr) {
    delete buffer;

    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    delete buffer;

    return;
  }

  GLubyte dst[dh][dw][3];
	uint8_t *src = data;

  for (int i=dh-1; i>=0; i--) {
    for (int j=0; j<dw; j++) {
      dst[i][j][0] = (GLubyte)src[2];
      dst[i][j][1] = (GLubyte)src[1];
      dst[i][j][2] = (GLubyte)src[0];

      src = src + 4;
    }
	}

  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(dw, dh, GL_RGB, GL_UNSIGNED_BYTE, dst);
  
  glFinish();
  glFlush();
  
  glutSwapBuffers();

  g_window->Flush();

  delete buffer;
  buffer = nullptr;

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void OnShape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) -h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

std::map<jevent::jmouseevent_button_t, bool> _mouse_button_state;

void OnMousePress(int button_id, int state, int x, int y)
{
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
  int mouse_z = 0;

  _mouse_x = x;
  _mouse_y = y;

  _mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
  _mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);

  if (state == GLUT_DOWN) {
    type = jevent::JMT_PRESSED;
  } else if (state == GLUT_UP) {
    type = jevent::JMT_RELEASED;
  }

  if (button_id == GLUT_LEFT_BUTTON) {
    button = jevent::JMB_BUTTON1;
  } else if (button_id == GLUT_MIDDLE_BUTTON) {
    button = jevent::JMB_BUTTON2;
  } else if (button_id == GLUT_RIGHT_BUTTON) {
    button = jevent::JMB_BUTTON3;
  }

  if (type == jevent::JMT_PRESSED) {
    _mouse_button_state[button] = true;
  } else {
    _mouse_button_state[button] = false;
  }

  _click_count = 1;

  if (type == jevent::JMT_PRESSED) {
    auto current = std::chrono::steady_clock::now();

    if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - _last_keypress).count()) < 200L) {
      _click_count = _click_count + 1;
    }

    _last_keypress = current;

    mouse_z = _click_count;
  }

  if (_mouse_button_state[jevent::JMB_BUTTON1] == true) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
  }

  if (_mouse_button_state[jevent::JMB_BUTTON2] == true) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
  }

  if (_mouse_button_state[jevent::JMB_BUTTON3] == true) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
  }

  g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
}

void OnMouseMove(int x, int y)
{
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
  int mouse_z = 0;

  _mouse_x = x;
  _mouse_y = y;

  _mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
  _mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);

  type = jevent::JMT_MOVED;

  if (_mouse_button_state[jevent::JMB_BUTTON1] == true) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
  }

  if (_mouse_button_state[jevent::JMB_BUTTON2] == true) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
  }

  if (_mouse_button_state[jevent::JMB_BUTTON3] == true) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
  }

  g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
}

void OnKeyPressRelease(unsigned char key, int x, int y, bool released)
{
  jevent::jkeyevent_type_t type;
  jevent::jkeyevent_modifiers_t mod;

  mod = (jevent::jkeyevent_modifiers_t)(0);

  int m = glutGetModifiers();

  if (m == GLUT_ACTIVE_SHIFT) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
  } else if (m == GLUT_ACTIVE_CTRL) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
  } else if (GLUT_ACTIVE_ALT) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
  // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_SUPER);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_HYPER);
  }

  type = jevent::JKT_UNKNOWN;
  
  if (released == false) {
    type = jevent::JKT_PRESSED;
  } else {
    type = jevent::JKT_RELEASED;
  }

  jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(key);

  g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

void OnKeyPress(unsigned char key, int x, int y)
{
  OnKeyPressRelease(key, x, y, false);
}

void OnKeyRelease(unsigned char key, int x, int y)
{
  OnKeyPressRelease(key, x, y, true);
}

void OnKeyPressReleaseSpecial(int key, int x, int y, bool released)
{
  jevent::jkeyevent_type_t type;
  jevent::jkeyevent_modifiers_t mod;

  mod = (jevent::jkeyevent_modifiers_t)(0);

  int m = glutGetModifiers();

  if (m & GLUT_ACTIVE_SHIFT) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
  } else if (m & GLUT_ACTIVE_CTRL) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
  } else if (m & GLUT_ACTIVE_ALT) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
  // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_SUPER);
  // } else if ((event.key.keysym.mod & ) != 0) {
  //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_HYPER);
  }

  type = jevent::JKT_UNKNOWN;
  
  if (released == false) {
    type = jevent::JKT_PRESSED;
  } else {
    type = jevent::JKT_RELEASED;
  }

  jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbolSpecial(key);

  g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

void OnKeyPressSpecial(int key, int x, int y)
{
  OnKeyPressReleaseSpecial(key, x, y, false);
}

void OnKeyReleaseSpecial(int key, int x, int y)
{
  OnKeyPressReleaseSpecial(key, x, y, true);
}

static bool quitting = false;

void OnTimer(int value)
{
  std::vector<jevent::EventObject *> events = g_window->GrabEvents();

  if (events.size() > 0) {
    jevent::EventObject *event = events.front();

    if (dynamic_cast<jevent::WindowEvent *>(event) != nullptr) {
      jevent::WindowEvent *window_event = dynamic_cast<jevent::WindowEvent *>(event);

      if (window_event->GetType() == jevent::JWET_PAINTED) {
        glutPostRedisplay();
      }
    }

    // INFO:: discard all remaining events
    while (events.size() > 0) {
      jevent::EventObject *event = events.front();

      events.erase(events.begin());

      delete event;
      event = nullptr;
    }
  }

  /*
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_WINDOWEVENT) {
      } else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_OPENED));
      } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
      } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
        InternalPaint();
      } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_MOVED));
      } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        InternalPaint();

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_RESIZED));
      } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
      }
    } else if(event.type == SDL_QUIT) {
      SDL_HideWindow(_window);

      quitting = true;

      g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
    }
  }
  */

  if (quitting == false) {
    glutTimerFunc(10, OnTimer, value);
  }
}

void OnVisibility(int state)
{
  if (state == GLUT_NOT_VISIBLE) {
    _visible = false;
  } else if (state == GLUT_VISIBLE) {
    _visible = true;
  }
}

void OnEntry(int state)
{
  if (state == GLUT_ENTERED) {
    // SDL_CaptureMouse(true);
    // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
    // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

    // SetCursor(GetCursor());

    g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_ENTERED));
  } else if (GLUT_LEFT) {
    // SDL_CaptureMouse(false);
    // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
    // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

    // SetCursor(JCS_DEFAULT);

    g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_LEAVED));
  }
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (_window > 0) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");
	
  _window = 0;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

  glutInitWindowSize(width, height);
  glutInitWindowPosition(x, y);
  
  _window = glutCreateWindow(nullptr);

  if (_window == 0) {
		throw jexception::RuntimeException("Cannot create a window");
  }

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glutDisplayFunc(OnDraw);
  glutReshapeFunc(OnShape);
  glutKeyboardFunc(OnKeyPress);
  glutSpecialFunc(OnKeyPressSpecial);
  glutKeyboardUpFunc(OnKeyRelease);
  glutSpecialUpFunc(OnKeyReleaseSpecial);
  glutMouseFunc(OnMousePress);
  glutMotionFunc(OnMouseMove);
  glutPassiveMotionFunc(OnMouseMove);
  glutTimerFunc(100, OnTimer, 1);
  glutVisibilityFunc(OnVisibility);
  glutEntryFunc(OnEntry);
  glutIdleFunc(nullptr);

  g_window = this;
}

NativeWindow::~NativeWindow()
{
  delete g_window;
  g_window = nullptr;
}

void NativeWindow::ToggleFullScreen()
{
  if (_fullscreen == false) {
    _previous_bounds = GetVisibleBounds();

    glutFullScreen();

    _fullscreen = true;
  } else {
    SetBounds(_previous_bounds.x, _previous_bounds.y, _previous_bounds.width, _previous_bounds.height);
    
    _fullscreen = false;
  }
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == nullptr) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  g_window = parent;

  g_window->SetParent(nullptr);
}

void NativeWindow::SetTitle(std::string title)
{
  _title = title;

  glutSetWindowTitle(title.c_str());
}

std::string NativeWindow::GetTitle()
{
  return _title;
}

void NativeWindow::SetOpacity(float opacity)
{
}

float NativeWindow::GetOpacity()
{
	return 1.0f;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
	if (undecorated == true) {
	} else {
	}
}

bool NativeWindow::IsUndecorated()
{
  return false;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  glutPositionWindow(x, y);
  glutReshapeWindow(width, height);
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
	jgui::jregion_t t;

  t.x = glutGet(GLUT_WINDOW_X);
  t.y = glutGet(GLUT_WINDOW_Y);
  t.width = glutGet(GLUT_WINDOW_WIDTH);
  t.height = glutGet(GLUT_WINDOW_HEIGHT);

	return t;
}
		
void NativeWindow::SetVisible(bool visible)
{
  _visible = visible;

  if (visible == false) {
    glutHideWindow();
  } else {
    glutShowWindow();
  }
}

bool NativeWindow::IsVisible()
{
	return _visible;
}
		
void NativeWindow::SetResizable(bool resizable)
{
}

bool NativeWindow::IsResizable()
{
  return true;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

jcursor_style_t NativeWindow::GetCursor()
{
	int id = glutGet(GLUT_WINDOW_CURSOR);

  if (id == GLUT_CURSOR_INHERIT) {
    return JCS_DEFAULT;
  } else if (id == GLUT_CURSOR_CROSSHAIR) {
    return JCS_CROSSHAIR;
  } else if (GLUT_CURSOR_RIGHT_SIDE) {
    return JCS_EAST;
  } else if (GLUT_CURSOR_LEFT_SIDE) {
    return JCS_WEST;
  } else if (GLUT_CURSOR_TOP_SIDE) {
    return JCS_NORTH;
  } else if (GLUT_CURSOR_BOTTOM_SIDE) {
    return JCS_SOUTH;
  } else if (GLUT_CURSOR_INFO) {
    return JCS_HAND;
  // } else if (GLUT_CURSOR_INHERIT) {
  //  return JCS_MOVE;
  } else if (GLUT_CURSOR_UP_DOWN) {
    return JCS_NS;
  } else if (GLUT_CURSOR_LEFT_RIGHT) {
    return JCS_WE;
  } else if (GLUT_CURSOR_TOP_LEFT_CORNER) {
    return JCS_NW_CORNER;
  } else if (GLUT_CURSOR_TOP_RIGHT_CORNER) {
    return JCS_NE_CORNER;
  } else if (GLUT_CURSOR_BOTTOM_RIGHT_CORNER) {
    return JCS_SW_CORNER;
  } else if (GLUT_CURSOR_BOTTOM_LEFT_CORNER) {
    return JCS_SE_CORNER;
  } else if (GLUT_CURSOR_TEXT) {
    return JCS_TEXT;
  } else if (GLUT_CURSOR_WAIT) {
    return JCS_WAIT;
  }

  return JCS_DEFAULT;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  if (enabled == false) {
    glutSetCursor(GLUT_CURSOR_NONE);
  } else {
    glutSetCursor(GLUT_CURSOR_INHERIT);
  }
}

bool NativeWindow::IsCursorEnabled()
{
	return glutGet(GLUT_WINDOW_CURSOR) != GLUT_CURSOR_NONE;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  int type = GLUT_CURSOR_INHERIT;

  if (style == JCS_DEFAULT) {
    type = GLUT_CURSOR_INHERIT;
  } else if (style == JCS_CROSSHAIR) {
    type = GLUT_CURSOR_CROSSHAIR;
  } else if (style == JCS_EAST) {
    type = GLUT_CURSOR_RIGHT_SIDE;
  } else if (style == JCS_WEST) {
    type = GLUT_CURSOR_LEFT_SIDE;
  } else if (style == JCS_NORTH) {
    type = GLUT_CURSOR_TOP_SIDE;
  } else if (style == JCS_SOUTH) {
    type = GLUT_CURSOR_BOTTOM_SIDE;
  } else if (style == JCS_HAND) {
    type = GLUT_CURSOR_INFO;
  } else if (style == JCS_MOVE) {
    type = GLUT_CURSOR_INHERIT;
  } else if (style == JCS_NS) {
    type = GLUT_CURSOR_UP_DOWN;
  } else if (style == JCS_WE) {
    type = GLUT_CURSOR_LEFT_RIGHT;
  } else if (style == JCS_NW_CORNER) {
    type = GLUT_CURSOR_TOP_LEFT_CORNER;
  } else if (style == JCS_NE_CORNER) {
    type = GLUT_CURSOR_TOP_RIGHT_CORNER;
  } else if (style == JCS_SW_CORNER) {
    type = GLUT_CURSOR_BOTTOM_RIGHT_CORNER;
  } else if (style == JCS_SE_CORNER) {
    type = GLUT_CURSOR_BOTTOM_LEFT_CORNER;
  } else if (style == JCS_TEXT) {
    type = GLUT_CURSOR_TEXT;
  } else if (style == JCS_WAIT) {
    type = GLUT_CURSOR_WAIT;
  }

  glutSetCursor(type);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t *data = nullptr;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == nullptr) {
		return;
	}

	SDL_Surface *surface = nullptr;
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

	if (surface == nullptr) {
		delete [] data;

		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);

	delete [] data;
  */
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jgui::JWR_NONE;
}

void NativeWindow::SetIcon(jgui::Image *image)
{
  _icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return _icon;
}

}

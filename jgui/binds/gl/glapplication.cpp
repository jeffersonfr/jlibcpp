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
#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <X11/Xlib.h>

namespace jgui {

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static int sg_window = 0;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static std::string sg_title;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static jgui::jrect_t<int> sg_previous_bounds;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static Window *sg_jgui_window = nullptr;

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

void Application::Init(int argc, char **argv)
{
	XInitThreads();

  glutInit(&argc, argv);
  // glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB | GLUT_MULTISAMPLE);

	sg_screen.width = glutGet(GLUT_SCREEN_WIDTH);
	sg_screen.height = glutGet(GLUT_SCREEN_HEIGHT);
  
  sg_quitting = false;
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  glutMainLoop();
  // glutMainLoopEvent(); // single loop event

  sg_jgui_window->SetVisible(false);
}

jsize_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  glutLeaveMainLoop();

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

static void OnDraw()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

  jrect_t<int> 
    bounds = sg_jgui_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jgui::jsize_t<int>
      size = sg_back_buffer->GetSize();

    if (size.width != bounds.size.width or size.height != bounds.size.height) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, bounds.size);
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  if (Application::FrameRate(sg_jgui_window->GetFramesPerSecond()) == true) {
    g->Flush();

    sg_jgui_window->Repaint();

    return;
  }

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC_OVER);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  // INFO:: invert the y-axis
  uint32_t 
    *r1 = data,
    *r2 = data + (bounds.size.height - 1)*bounds.size.width;

  for (int i=0; i<bounds.size.height/2; i++) {
    for (int j=0; j<bounds.size.width; j++) {
      uint32_t p = r1[j];

      r1[j] = r2[j];

      r2[j] = p;
    }

    r1 = r1 + bounds.size.width;
    r2 = r2 - bounds.size.width;
	}

  // glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(bounds.size.width, bounds.size.height, GL_BGRA, GL_UNSIGNED_BYTE, data);
  
  if (g->IsVerticalSyncEnabled() == false) {
    glFlush();
  } else {
    glFinish();
  }
  
  glutSwapBuffers();

  sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
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

std::map<jevent::jmouseevent_button_t, bool> sg_mouse_button_state;

void OnMousePress(int button_id, int state, int x, int y)
{
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
  int mouse_z = 0;

  sg_mouse_x = x;
  sg_mouse_y = y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.width-1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.height-1);

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
    sg_mouse_button_state[button] = true;
  } else {
    sg_mouse_button_state[button] = false;
  }

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

void OnMouseMove(int x, int y)
{
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
  int mouse_z = 0;

  sg_mouse_x = x;
  sg_mouse_y = y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.width-1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.height-1);

  type = jevent::JMT_MOVED;

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
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

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
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

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

void OnKeyPressSpecial(int key, int x, int y)
{
  OnKeyPressReleaseSpecial(key, x, y, false);
}

void OnKeyReleaseSpecial(int key, int x, int y)
{
  OnKeyPressReleaseSpecial(key, x, y, true);
}

void OnTimer(int value)
{
  if (sg_repaint.exchange(false) == true) {
    glutPostRedisplay();
  }

  /*
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_WINDOWEVENT) {
      } else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_OPENED));
      } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
      } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
        InternalPaint();
      } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_MOVED));
      } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        InternalPaint();

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_RESIZED));
      } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
      } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
      } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
      }
    } else if(event.type == SDL_QUIT) {
      SDL_HideWindow(sg_window);

      sg_quitting = true;

      sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
    }
  }
  */

  if (sg_quitting == false) {
    glutTimerFunc(10, OnTimer, value);
  }
}

void OnVisibility(int state)
{
  if (state == GLUT_NOT_VISIBLE) {
    sg_visible = false;
  } else if (state == GLUT_VISIBLE) {
    sg_visible = true;
  }
}

void OnEntry(int state)
{
  if (state == GLUT_ENTERED) {
    // SDL_CaptureMouse(true);
    // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
    // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

    // SetCursor(GetCursor());

    sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
  } else if (GLUT_LEFT) {
    // SDL_CaptureMouse(false);
    // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
    // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

    // SetCursor(JCS_DEFAULT);

    sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
  }
}

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::WindowAdapter()
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_window > 0) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");
	
  sg_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jgui_window = parent;

  glutInitWindowSize(bounds.size.width, bounds.size.height);
  glutInitWindowPosition(bounds.point.x, bounds.point.y);
  
  sg_window = glutCreateWindow(nullptr);

  if (sg_window == 0) {
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
}

NativeWindow::~NativeWindow()
{
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_previous_bounds = GetBounds();

    glutFullScreen();

    sg_fullscreen = true;
  } else {
    SetBounds(sg_previous_bounds.point.x, sg_previous_bounds.point.y, sg_previous_bounds.size.width, sg_previous_bounds.size.height);
    
    sg_fullscreen = false;
  }
}

void NativeWindow::SetTitle(std::string title)
{
  sg_title = title;

  glutSetWindowTitle(title.c_str());
}

std::string NativeWindow::GetTitle()
{
  return sg_title;
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

jgui::jrect_t<int> NativeWindow::GetBounds()
{
  return {
    glutGet(GLUT_WINDOW_X),
    glutGet(GLUT_WINDOW_Y),
    glutGet(GLUT_WINDOW_WIDTH),
    glutGet(GLUT_WINDOW_HEIGHT)
  };
}
		
void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == false) {
    glutHideWindow();
  } else {
    glutShowWindow();
  }
}

bool NativeWindow::IsVisible()
{
	return sg_visible;
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

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

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

	jsize_t<int> t = shape->GetSize();
	uint32_t data[t.width*t.height];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.width, t.height});

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
  sg_jgui_icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}

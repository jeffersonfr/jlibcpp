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
#include "jgui/jfont.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <iostream>

#include <QDialog>
#include <QResizeEvent>
#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>

namespace jgui {

/** \brief */
static jgui::Image *_icon = nullptr;
/** \brief */
static Window *g_window = nullptr;
/** \brief */
QApplication *_application = nullptr;
/** \brief */
static QDialog *_handler = nullptr;
/** \brief */
static float _opacity = 1.0f;
/** \brief */
static bool _fullscreen_enabled = false;
/** \brief */
static jcursor_style_t _cursor;
/** \brief */
static bool _undecorated = false;
/** \brief */
static bool _visible = false;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol, bool capital)
{
	switch (symbol) {
		case 16777220:
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		case 16777219:
			return jevent::JKS_BACKSPACE;
		case 16777217:
			return jevent::JKS_TAB;
		// case ALLEGRO_KEY_CANCEL:
		//	return jevent::JKS_CANCEL;
		case 16777216:
			return jevent::JKS_ESCAPE;
		case 32:
			return jevent::JKS_SPACE;
		// case ALLEGRO_KEY_EXCLAIM:
		// 	return jevent::JKS_EXCLAMATION_MARK;
		// case ALLEGRO_KEY_QUOTEDBL:
		// 	return jevent::JKS_QUOTATION;
		// case ALLEGRO_KEY_HASH:
		// 	return jevent::JKS_NUMBER_SIGN;
		// case ALLEGRO_KEY_DOLLAR:
		// 	return jevent::JKS_DOLLAR_SIGN;
		// case ALLEGRO_KEY_PERCENT_SIGN:
		//	return jevent::JKS_PERCENT_SIGN;
		// case ALLEGRO_KEY_AMPERSAND:   
		// 	return jevent::JKS_AMPERSAND;
		case 39:
			return jevent::JKS_APOSTROPHE;
		// case ALLEGRO_KEY_LEFTPAREN:
		// 	return jevent::JKS_PARENTHESIS_LEFT;
		// case ALLEGRO_KEY_RIGHTPAREN:
		// 	return jevent::JKS_PARENTHESIS_RIGHT;
		// case ALLEGRO_KEY_ASTERISK:
		// 	return jevent::JKS_STAR;
		// case ALLEGRO_KEY_PLUS:
		// 	return jevent::JKS_PLUS_SIGN;
		case 44:   
			return jevent::JKS_COMMA;
		case 45:
			return jevent::JKS_MINUS_SIGN;
		case 46:  
		 	return jevent::JKS_PERIOD;
		case 47:
			return jevent::JKS_SLASH;
		case 48:     
			return jevent::JKS_0;
		case 49:
			return jevent::JKS_1;
		case 50:
			return jevent::JKS_2;
		case 51:
			return jevent::JKS_3;
		case 52:
			return jevent::JKS_4;
		case 53:
			return jevent::JKS_5;
		case 54:
			return jevent::JKS_6;
		case 55:
			return jevent::JKS_7;
		case 56:
			return jevent::JKS_8;
		case 57:
			return jevent::JKS_9;
		// case ALLEGRO_KEY_COLON:
		// 	return jevent::JKS_COLON;
		case 59:
			return jevent::JKS_SEMICOLON;
		// case ALLEGRO_KEY_LESS:
		// 	return jevent::JKS_LESS_THAN_SIGN;
		case 61: 
			return jevent::JKS_EQUALS_SIGN;
		// case ALLEGRO_KEY_GREATER:
		// 	return jevent::JKS_GREATER_THAN_SIGN;
		// case ALLEGRO_KEY_QUESTION:   
		// 	return jevent::JKS_QUESTION_MARK;
		// case ALLEGRO_KEY_AT:
		//	return jevent::JKS_AT;
		case 65:
			return (capital == true)?jevent::JKS_A:jevent::JKS_a;
		case 66:
			return (capital == true)?jevent::JKS_B:jevent::JKS_b;
		case 67:
			return (capital == true)?jevent::JKS_C:jevent::JKS_c;
		case 68:
			return (capital == true)?jevent::JKS_D:jevent::JKS_d;
		case 69:
			return (capital == true)?jevent::JKS_E:jevent::JKS_e;
		case 70:
			return (capital == true)?jevent::JKS_F:jevent::JKS_f;
		case 71:
			return (capital == true)?jevent::JKS_G:jevent::JKS_g;
		case 72:
			return (capital == true)?jevent::JKS_H:jevent::JKS_h;
		case 73:
			return (capital == true)?jevent::JKS_I:jevent::JKS_i;
		case 74:
			return (capital == true)?jevent::JKS_J:jevent::JKS_j;
		case 75:
			return (capital == true)?jevent::JKS_K:jevent::JKS_k;
		case 76:
			return (capital == true)?jevent::JKS_L:jevent::JKS_l;
		case 77:
			return (capital == true)?jevent::JKS_M:jevent::JKS_m;
		case 78:
			return (capital == true)?jevent::JKS_N:jevent::JKS_n;
		case 79:
			return (capital == true)?jevent::JKS_O:jevent::JKS_o;
		case 80:
			return (capital == true)?jevent::JKS_P:jevent::JKS_p;
		case 81:
			return (capital == true)?jevent::JKS_Q:jevent::JKS_q;
		case 82:
			return (capital == true)?jevent::JKS_R:jevent::JKS_r;
		case 83:
			return (capital == true)?jevent::JKS_S:jevent::JKS_s;
		case 84:
			return (capital == true)?jevent::JKS_T:jevent::JKS_t;
		case 85:
			return (capital == true)?jevent::JKS_U:jevent::JKS_u;
		case 86:
			return (capital == true)?jevent::JKS_V:jevent::JKS_v;
		case 87:
			return (capital == true)?jevent::JKS_W:jevent::JKS_w;
		case 88:
			return (capital == true)?jevent::JKS_X:jevent::JKS_x;
		case 89:
			return (capital == true)?jevent::JKS_Y:jevent::JKS_y;
		case 90:
			return (capital == true)?jevent::JKS_Z:jevent::JKS_z;
		case 91:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case 92:   
			return jevent::JKS_BACKSLASH;
		case 93:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		// case ALLEGRO_KEY_CARET:
		// 	return jevent::JKS_CIRCUMFLEX_ACCENT;
		// case ALLEGRO_KEY_UNDERSCORE:    
		// 	return jevent::JKS_UNDERSCORE;
		// case ALLEGRO_KEY_BACKQUOTE:
		//	return jevent::JKS_GRAVE_ACCENT;
		// case ALLEGRO_KEY_CURLY_BRACKET_LEFT:
		//	return jevent::JKS_CURLY_BRACKET_LEFT;
		// case ALLEGRO_KEY_VERTICAL_BAR:  
		// 	return jevent::JKS_VERTICAL_BAR;
		// case ALLEGRO_KEY_CURLY_BRACKET_RIGHT:
		// 	return jevent::JKS_CURLY_BRACKET_RIGHT;
		// case ALLEGRO_KEY_TILDE:  
		//	return jevent::JKS_TILDE;
		case 16777223:
			return jevent::JKS_DELETE;
		case 16777234:
			return jevent::JKS_CURSOR_LEFT;
		case 16777236:
			return jevent::JKS_CURSOR_RIGHT;
		case 16777235:  
			return jevent::JKS_CURSOR_UP;
		case 16777237:
			return jevent::JKS_CURSOR_DOWN;
		case 16777222:  
			return jevent::JKS_INSERT;
		case 16777232:     
			return jevent::JKS_HOME;
		case 16777233:
			return jevent::JKS_END;
		case 16777238:
			return jevent::JKS_PAGE_UP;
		case 16777239:
			return jevent::JKS_PAGE_DOWN;
		// case ALLEGRO_KEY_PRINTSCREEN:   
		//	return jevent::JKS_PRINT;
		case 16777224:
			return jevent::JKS_PAUSE;
		// case ALLEGRO_KEY_RED:
		// 	return jevent::JKS_RED;
		// case ALLEGRO_KEY_GREEN:
		// 	return jevent::JKS_GREEN;
		// case ALLEGRO_KEY_YELLOW:
		// 	return jevent::JKS_YELLOW;
		// case ALLEGRO_KEY_BLUE:
		// 	return jevent::JKS_BLUE;
		case 16777264:
		 	return jevent::JKS_F1;
		case 16777265:
		 	return jevent::JKS_F2;
		case 16777266:
			return jevent::JKS_F3;
		case 16777267:
			return jevent::JKS_F4;
		case 16777268:
			return jevent::JKS_F5;
		case 16777269:
			return jevent::JKS_F6;
		case 16777270:
		 	return jevent::JKS_F7;
		case 16777271:
			return jevent::JKS_F8;
		case 16777272:
			return jevent::JKS_F9;
		case 16777273:
		 	return jevent::JKS_F10;
		case 16777274:
			return jevent::JKS_F11;
		case 16777275:
		 	return jevent::JKS_F12;
    case 16777248:
		 	return jevent::JKS_SHIFT;
		case 16777249:
		 	return jevent::JKS_CONTROL;
		case 16777251:
		 	return jevent::JKS_ALT;
		// case ALLEGRO_KEY_ALTGR:
		//	return jevent::JKS_ALTGR;
		// case ALLEGRO_KEY_LMETA:
		// case ALLEGRO_KEY_RMETA:
		// 	return jevent::JKS_META;
		// case ALLEGRO_KEY_LSUPER:
		// case ALLEGRO_KEY_RSUPER:
		// 	return jevent::JKS_SUPER;
		// case ALLEGRO_KEY_HYPER:
		// 	return jevent::JKS_HYPER;
		default: 
			break;
	}

	return jevent::JKS_UNKNOWN;
}

static bool quitting = false;

class QTWindowRender : public QDialog {

  protected:
    virtual bool event(QEvent *event) override
    {
      if (event->type() == QEvent::ActivationChange) {
          // printf("ActivationEvent\n");
      } else if (event->type() == QEvent::ApplicationStateChange) {
          printf("ApplicationStateEvent\n");;
      } else if (event->type() == QEvent::Close) {
          printf("CloseEvent\n");
          
          quitting = true;
      } else if (event->type() == QEvent::Enter) {
          printf("EnterEvent\n");
      } else if (event->type() == QEvent::Leave) {
          printf("LeaveEvent\n");
      // } else if (event->type() == QEvent::Expose) {
      // } else if (event->type() == QEvent::UpdateRequest) {
      // } else if (event->type() == QEvent::Paint) {
      } else if (event->type() == QEvent::Move) {
          printf("MoveEvent\n");
      } else if (event->type() == QEvent::Resize) {
          printf("ResizeEvent\n");
      } else if (event->type() == QEvent::Show) {
          printf("ShowEvent\n");
      } else if (event->type() == QEvent::Hide) {
          printf("HideEvent\n");
      } else if (event->type() == QEvent::FocusIn) {
          printf("FocusInEvent\n");
      } else if (event->type() == QEvent::FocusOut) {
          printf("FocusOutEvent\n");
      } else if (
          event->type() == QEvent::KeyPress or
          event->type() == QEvent::KeyRelease) {
        QKeyEvent *e = dynamic_cast<QKeyEvent *>(event);

        /*
        std::string info;

        if (event->type() == QEvent::KeyPress) {
          info = "KeyPress";
        } else if (event->type() == QEvent::KeyRelease) {
          info = "KeyRelease";
        }

        printf("%s:: count:[%d], key:[%d], modifiers:[%x]\n", 
            info.c_str(), e->count(), e->key(), e->modifiers()); 
        */

        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = jevent::JKM_NONE;

        if (e->modifiers() & Qt::ShiftModifier) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if (e->modifiers() & Qt::ControlModifier) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if (e->modifiers() & Qt::AltModifier) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        } else if (e->modifiers() & Qt::MetaModifier) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
        }

        type = (jevent::jkeyevent_type_t)(0);

        if (event->type() == QEvent::KeyPress) {
          type = jevent::JKT_PRESSED;
        } else if (event->type() == QEvent::KeyRelease) {
          type = jevent::JKT_RELEASED;
        }

        int shift = e->modifiers() & Qt::ShiftModifier;
        int capslock = false;

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->key(), (shift != 0 && capslock == 0) || (shift == 0 && capslock != 0));

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (
          event->type() == QEvent::MouseButtonDblClick or
          event->type() == QEvent::MouseButtonPress or
          event->type() == QEvent::MouseButtonRelease or
          event->type() == QEvent::MouseMove) {
        QMouseEvent *e = dynamic_cast<QMouseEvent *>(event);

        /*
        std::string info;

        if (event->type() == QEvent::MouseButtonDblClick) {
          info = "MouseDoubleClick";
        } else if (event->type() == QEvent::MouseButtonPress) {
          info = "MousePress";
        } else if (event->type() == QEvent::MouseButtonRelease) {
          info = "MouseRelease";
        } else if (event->type() == QEvent::MouseMove) {
          info = "MouseMove";
        } else if (event->type() == QEvent::Wheel) {
          info = "MouseWheel";
        }

        printf("%s:: button:[%x], buttons:[%x], global:[%d, %d], local:[%d, %d]\n", 
            info.c_str(), e->button(), e->buttons(), e->globalX(), e->globalY(), e->x(), e->y()); 
        */

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_MOVED;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        if (e->buttons() & Qt::LeftButton) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if (e->buttons() & Qt::MidButton) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if (e->buttons() & Qt::RightButton) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        mouse_z = 1;

        if (event->type() == QEvent::MouseButtonDblClick) {
          type = jevent::JMT_PRESSED;

          mouse_z = 2;
        } else if (event->type() == QEvent::MouseButtonPress) {
          type = jevent::JMT_PRESSED;
        } else if (event->type() == QEvent::MouseButtonRelease) {
          type = jevent::JMT_RELEASED;
        }

        if (e->button() == Qt::LeftButton) {
          button = jevent::JMB_BUTTON1;
        } else if (e->button() == Qt::MidButton) {
          button = jevent::JMB_BUTTON2;
        } else if (e->button() == Qt::RightButton) {
          button = jevent::JMB_BUTTON3;
        }

        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, mouse_x, mouse_y));
      } else if (event->type() == QEvent::Wheel) {
        QWheelEvent *e = dynamic_cast<QWheelEvent *>(event);

        /*
        printf("MouseWheel:: buttons:[%x], global:[%d, %d], local:[%d, %d]\n", 
            e->buttons(), e->globalX(), e->globalY(), e->x(), e->y()); 
        */

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_ROTATED;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        if (e->buttons() & Qt::LeftButton) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if (e->buttons() & Qt::MidButton) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if (e->buttons() & Qt::RightButton) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        QPoint degrees = e->angleDelta();

        if (degrees.y() > 0) {
          mouse_z = 1;
        } else {
          mouse_z = -1;
        }

        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, mouse_x, mouse_y));
      } else if (event->type() == QEvent::WindowActivate) {
          // printf("WindowActivateEvent\n");
      } else if (event->type() == QEvent::WindowDeactivate) {
          // printf("WindowDeactivateEvent\n");
      } else if (event->type() == QEvent::WindowStateChange) {
          printf("WindowStateEvent\n");
      } else {
          return QDialog::event(event);
      }

      return true;
    }

  public:
    explicit QTWindowRender(): 
      QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint)
    {
      setWindowFlags(Qt::Window);
    }

    virtual ~QTWindowRender() 
    {
    }

    virtual void paintEvent(QPaintEvent* event)
    {
      // QDialog::paintEvent(event);

      QPainter painter(this);

      if (g_window == nullptr || g_window->IsVisible() == false) {
        return;
      }

      // NativeWindow 
      //   *handler = reinterpret_cast<NativeWindow *>(user_data);
      jregion_t 
        bounds = g_window->GetBounds();
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
      int stride = cairo_image_surface_get_stride(cairo_surface);

      uint8_t *data = cairo_image_surface_get_data(cairo_surface);

      if (data == nullptr) {
        delete buffer;

        return;
      }

      QImage image(data, dw, dh, stride, QImage::Format_RGB32);
      QPixmap pixmap = QPixmap::fromImage(image);

      // painter.beginNativePainting();
      painter.drawPixmap(0, 0, pixmap);
      // painter.endNativePainting();

      g_window->Flush();

      delete buffer;
      buffer = nullptr;

      g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
    }

};

static jgui::jsize_t _screen = {0, 0};

NativeApplication::NativeApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeApplication");
}

NativeApplication::~NativeApplication()
{
  delete g_window;
  g_window = nullptr;
}

void NativeApplication::InternalInit(int argc, char **argv)
{
  int i = argc;

  _application = new QApplication(i, argv);

  QRect 
    geometry = QApplication::desktop()->screenGeometry();

	_screen.width = geometry.width();
	_screen.height = geometry.height();
}

void NativeApplication::InternalPaint()
{
}

void NativeApplication::InternalLoop()
{
  quitting = false;

  _handler->show();
  _handler->activateWindow();

  do {
    std::vector<jevent::EventObject *> events = g_window->GrabEvents();

    if (events.size() > 0) {
      jevent::EventObject *event = events.front();

      if (dynamic_cast<jevent::WindowEvent *>(event) != nullptr) {
        jevent::WindowEvent *window_event = dynamic_cast<jevent::WindowEvent *>(event);

        if (window_event->GetType() == jevent::JWET_PAINTED) {
          _handler->repaint();
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

    _application->processEvents();
    
    std::this_thread::yield();
  } while (quitting == false);

  g_window->GrabEvents();

  g_window->SetVisible(false);
}

void NativeApplication::InternalQuit()
{
  QCoreApplication::quit();
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (g_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  _handler = new QTWindowRender();

  _handler->setMouseTracking(true);
  _handler->resize(width, height);
  _handler->move(x, y);

  _visible = true;
}

NativeWindow::~NativeWindow()
{
  delete _handler;
  _handler = nullptr;
}

QByteArray geometry;

void NativeWindow::ToggleFullScreen()
{
    const QString 
      session = QString(getenv("DESKTOP_SESSION")).toLower();

	if (_fullscreen_enabled == false) {
    _fullscreen_enabled = true;
    
    geometry = _handler->saveGeometry();

    if (session == "ubuntu") {
      _handler->setFixedSize({_screen.width, _screen.height});
      _handler->setWindowFlags(Qt::FramelessWindowHint);
      _handler->setWindowState(_handler->windowState() | Qt::WindowFullScreen);
      _handler->show();
      _handler->activateWindow();
    } else {
      _handler->showFullScreen();
    }
	} else {
    _fullscreen_enabled = false;
    
    if (session == "ubuntu") {
        _handler->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        _handler->setMinimumSize(0, 0);
        _handler->restoreGeometry(geometry);
        _handler->setWindowFlags(Qt::Dialog);
        _handler->show();
        _handler->activateWindow();
    } else {
        _handler->showNormal();
    }

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
	_handler->setWindowTitle(title.c_str());
}

std::string NativeWindow::GetTitle()
{
	return _handler->windowTitle().toStdString();
}

void NativeWindow::SetOpacity(float opacity)
{
	_opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return _opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
  _undecorated = undecorated;

  if (undecorated == true) {
    _handler->setWindowFlags(Qt::CustomizeWindowHint);
    _handler->setWindowFlags(Qt::FramelessWindowHint);
  } else {
    _handler->setWindowFlags(Qt::WindowTitleHint);
    _handler->setWindowFlags(Qt::WindowMinimizeButtonHint);
    _handler->setWindowFlags(Qt::WindowMaximizeButtonHint);
    _handler->setWindowFlags(Qt::WindowCloseButtonHint);
  }
}

bool NativeWindow::IsUndecorated()
{
  return _undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  _handler->resize(width, height);
  _handler->move(x, y);
}

jgui::jregion_t NativeWindow::GetBounds()
{
  QSize 
    size = _handler->size();
  QPoint
    location = _handler->pos();

  return {
    .x = location.x(),
    .y = location.y(),
    .width = size.width(),
    .height = size.height()
  };
}

void NativeWindow::SetResizable(bool resizable)
{
}

bool NativeWindow::IsResizable()
{
  return false;
}

void NativeWindow::SetCursorLocation(int x, int y)
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

  QCursor cursor = _handler->cursor();

  cursor.setPos(QPoint(x, y));
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

  QCursor 
    cursor = _handler->cursor();
  QPoint
    location = cursor.pos();

  t.x = location.x();
  t.y = location.y();

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  if (_visible == visible) {
    return;
  }

  _visible = visible;

  // _handler->setVisible(_visible);

  if (_visible == false) {
    _handler->hide();
    // _handler->deactivateWindow();
  } else {
    _handler->show();
    _handler->activateWindow();
  }
}

bool NativeWindow::IsVisible()
{
  return _visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return _cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  QCursor cursor = _handler->cursor();

  if (enabled == false) {
    cursor.setShape(Qt::BlankCursor);
  } else {
    SetCursor(_cursor);
  }
}

bool NativeWindow::IsCursorEnabled()
{
  return _handler->cursor().shape() != Qt::BlankCursor;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  Qt::CursorShape type = Qt::ArrowCursor;

  if (style == JCS_DEFAULT) {
    type = Qt::ArrowCursor;
  } else if (style == JCS_CROSSHAIR) {
    type = Qt::CrossCursor;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
    type = Qt::UpArrowCursor;
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = Qt::OpenHandCursor;
  } else if (style == JCS_MOVE) {
    type = Qt::SizeAllCursor;
  } else if (style == JCS_NS) {
    type = Qt::SizeVerCursor;
  } else if (style == JCS_WE) {
    type = Qt::SizeHorCursor;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = Qt::IBeamCursor;
  } else if (style == JCS_WAIT) {
    type = Qt::BusyCursor;
  }

  QCursor cursor;

  cursor.setShape(type);

  _handler->setCursor(cursor);

  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{

	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t *data = nullptr;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == nullptr) {
		return;
	}
  
  QImage image((uint8_t *)data, t.width, t.height, t.width*4, QImage::Format_RGB32);
  QPixmap pixmap = QPixmap::fromImage(image);
  QCursor cursor_default = QCursor(pixmap, hotx, hoty);

  _handler->setCursor(cursor_default);
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

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
#include "jgui/jfont.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

#include <QDialog>
#include <QResizeEvent>
#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>
#include <QScreen>

namespace jgui {

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static QApplication *sg_application = nullptr;
/** \brief */
static QDialog *sg_handler = nullptr;
/** \brief */
static QByteArray sg_geometry;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static bool sg_visible = false;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcursor_style_t sg_jgui_cursor;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static Window *sg_jgui_window = nullptr;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
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
			return jevent::JKS_a;
		case 66:
			return jevent::JKS_b;
		case 67:
			return jevent::JKS_c;
		case 68:
			return jevent::JKS_d;
		case 69:
			return jevent::JKS_e;
		case 70:
			return jevent::JKS_f;
		case 71:
			return jevent::JKS_g;
		case 72:
			return jevent::JKS_h;
		case 73:
			return jevent::JKS_i;
		case 74:
			return jevent::JKS_j;
		case 75:
			return jevent::JKS_k;
		case 76:
			return jevent::JKS_l;
		case 77:
			return jevent::JKS_m;
		case 78:
			return jevent::JKS_n;
		case 79:
			return jevent::JKS_o;
		case 80:
			return jevent::JKS_p;
		case 81:
			return jevent::JKS_q;
		case 82:
			return jevent::JKS_r;
		case 83:
			return jevent::JKS_s;
		case 84:
			return jevent::JKS_t;
		case 85:
			return jevent::JKS_u;
		case 86:
			return jevent::JKS_v;
		case 87:
			return jevent::JKS_w;
		case 88:
			return jevent::JKS_x;
		case 89:
			return jevent::JKS_y;
		case 90:
			return jevent::JKS_z;
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
          
          sg_quitting = true;
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

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->key());

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
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

        static jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_MOVED;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        if (e->button() == Qt::LeftButton) {
          button = jevent::JMB_BUTTON1;
        } else if (e->button() == Qt::MidButton) {
          button = jevent::JMB_BUTTON2;
        } else if (e->button() == Qt::RightButton) {
          button = jevent::JMB_BUTTON3;
        }

        if (event->type() == QEvent::MouseButtonDblClick) {
          type = jevent::JMT_PRESSED;
          buttons = (jevent::jmouseevent_button_t)(buttons | button);
        } else if (event->type() == QEvent::MouseButtonPress) {
          type = jevent::JMT_PRESSED;
          buttons = (jevent::jmouseevent_button_t)(buttons & ~button);
        } else if (event->type() == QEvent::MouseButtonRelease) {
          type = jevent::JMT_RELEASED;
          buttons = (jevent::jmouseevent_button_t)(buttons & ~button);
        }

        if (sg_jgui_window->GetEventManager()->IsAutoGrab() == true && buttons != jevent::JMB_NONE) {
          sg_handler->grabMouse();
        } else {
          sg_handler->releaseMouse();
        }

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, buttons, {mouse_x, mouse_y}, mouse_z));
      } else if (event->type() == QEvent::Wheel) {
        QWheelEvent *e = dynamic_cast<QWheelEvent *>(event);

        /*
        printf("MouseWheel:: buttons:[%x], global:[%d, %d], local:[%d, %d]\n", 
            e->buttons(), e->globalX(), e->globalY(), e->x(), e->y()); 
        */

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_ROTATED;

        int mouse_x = e->x();
        int mouse_y = e->y();
        int mouse_z = 0;

        QPoint degrees = e->angleDelta();

        if (degrees.y() > 0) {
          mouse_z = 1;
        } else {
          mouse_z = -1;
        }

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {mouse_x, mouse_y}, mouse_z));
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

      if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
        return;
      }

      // NativeWindow 
      //   *handler = reinterpret_cast<NativeWindow *>(user_data);
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
        sg_jgui_window->Repaint();
      } else {
        g->Reset();
        g->SetCompositeFlags(jgui::JCF_SRC_OVER);

        sg_jgui_window->DoLayout();
        sg_jgui_window->Paint(g);
      }

      g->Flush();

      uint8_t *data = sg_back_buffer->LockData();

      QImage image(data, bounds.size.width, bounds.size.height, bounds.size.width*4, QImage::Format_RGB32);
      QPixmap pixmap = QPixmap::fromImage(image);

      // painter.beginNativePainting();
      painter.drawPixmap(0, 0, pixmap);
      // painter.endNativePainting();

      sg_back_buffer->UnlockData();

      sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
    }

};

void Application::Init(int argc, char **argv)
{
  static int argc0 = 1;
  static char *argv0[2] = {
    (char *)"app", nullptr
  };

  sg_application = new QApplication(argc0, argv0);

  QList<QScreen *> screens = QGuiApplication::screens();
  QRect geometry = screens.front()->geometry();

	sg_screen.width = geometry.width();
	sg_screen.height = geometry.height();
  
  sg_quitting = false;
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  sg_handler->show();
  // sg_handler->activateWindow();

  do {
    if (sg_repaint.exchange(false) == true) {
      sg_handler->repaint();
    }

    sg_application->processEvents();

    std::this_thread::yield();
  } while (sg_quitting == false);

  sg_jgui_window->SetVisible(false);
}

jsize_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::WindowAdapter()
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_jgui_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_window = parent;

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  sg_handler = new QTWindowRender();

  sg_handler->setVisible(true);
  sg_handler->setMouseTracking(true);
  sg_handler->setGeometry(bounds.point.x, bounds.point.y, bounds.size.width, bounds.size.height);

  sg_visible = true;
}

NativeWindow::~NativeWindow()
{
  QCoreApplication::quit();
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
    const QString 
      session = QString(getenv("DESKTOP_SESSION")).toLower();

	if (sg_fullscreen == false) {
    sg_fullscreen = true;
    
    sg_geometry = sg_handler->saveGeometry();

    if (session == "ubuntu") {
      sg_handler->setFixedSize({sg_screen.width, sg_screen.height});
      sg_handler->setWindowFlags(Qt::FramelessWindowHint);
      sg_handler->setWindowState(sg_handler->windowState() | Qt::WindowFullScreen);
      sg_handler->show();
      sg_handler->activateWindow();
    } else {
      sg_handler->showFullScreen();
    }
	} else {
    sg_fullscreen = false;
    
    if (session == "ubuntu") {
        sg_handler->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        sg_handler->setMinimumSize(0, 0);
        sg_handler->restoreGeometry(sg_geometry);
        sg_handler->setWindowFlags(Qt::Dialog);
        sg_handler->show();
        sg_handler->activateWindow();
    } else {
        sg_handler->showNormal();
    }
	}
}

void NativeWindow::SetTitle(std::string title)
{
	sg_handler->setWindowTitle(title.c_str());
}

std::string NativeWindow::GetTitle()
{
	return sg_handler->windowTitle().toStdString();
}

void NativeWindow::SetOpacity(float opacity)
{
	sg_opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return sg_opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
  sg_undecorated = undecorated;

  if (undecorated == true) {
    sg_handler->setWindowFlags(Qt::CustomizeWindowHint);
    sg_handler->setWindowFlags(Qt::FramelessWindowHint);
  } else {
    sg_handler->setWindowFlags(Qt::WindowTitleHint);
    sg_handler->setWindowFlags(Qt::WindowMinimizeButtonHint);
    sg_handler->setWindowFlags(Qt::WindowMaximizeButtonHint);
    sg_handler->setWindowFlags(Qt::WindowCloseButtonHint);
  }
}

bool NativeWindow::IsUndecorated()
{
  return sg_undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  sg_handler->resize(width, height);
  sg_handler->move(x, y);
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
  QSize 
    size = sg_handler->size();
  QPoint
    location = sg_handler->pos();

  return {
    location.x(),
    location.y(),
    size.width(),
    size.height()
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

	if (x > sg_screen.width) {
		x = sg_screen.width;
	}

	if (y > sg_screen.height) {
		y = sg_screen.height;
	}

  QCursor cursor = sg_handler->cursor();

  cursor.setPos(QPoint(x, y));
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

  QCursor 
    cursor = sg_handler->cursor();
  QPoint
    location = cursor.pos();

  t.x = location.x();
  t.y = location.y();

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  if (sg_visible == visible) {
    return;
  }

  sg_visible = visible;

  // sg_handler->setVisible(sg_visible);

  if (sg_visible == false) {
    sg_handler->hide();
    // sg_handler->deactivateWindow();
  } else {
    sg_handler->show();
    sg_handler->activateWindow();
  }
}

bool NativeWindow::IsVisible()
{
  return sg_visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  QCursor cursor = sg_handler->cursor();

  if (enabled == false) {
    cursor.setShape(Qt::BlankCursor);
  } else {
    SetCursor(sg_jgui_cursor);
  }
}

bool NativeWindow::IsCursorEnabled()
{
  return sg_handler->cursor().shape() != Qt::BlankCursor;
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

  sg_handler->setCursor(cursor);

  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{

	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t<int> t = shape->GetSize();
	uint32_t data[t.width*t.height];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.width, t.height});

	if (data == nullptr) {
		return;
	}
  
  QImage image((uint8_t *)data, t.width, t.height, t.width*4, QImage::Format_RGB32);
  QPixmap pixmap = QPixmap::fromImage(image);
  QCursor cursor_default = QCursor(pixmap, hotx, hoty);

  sg_handler->setCursor(cursor_default);
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

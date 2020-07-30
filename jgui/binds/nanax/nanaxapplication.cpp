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

#include <nana/gui.hpp>
#include <nana/gui/screen.hpp>
#include <nana/paint/pixel_buffer.hpp>

namespace jgui {

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static nana::form *fm = nullptr;
/** \brief */
static nana::drawing *dw = nullptr;
/** \brief */
static std::mutex sg_paint_mutex;
/** \brief */
static std::condition_variable sg_paint_condition;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static jgui::jrect_t<int> sg_last_bounds = {0, 0, 0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
  switch (symbol) {
    case 13:
      return jevent::JKS_ENTER; // jevent::JKS_RETURN;
    case 8:
      return jevent::JKS_BACKSPACE;
    case 9:
      return jevent::JKS_TAB;
    // case ALLEGRO_KEY_CANCEL:
    //  return jevent::JKS_CANCEL;
    case 27:
      return jevent::JKS_ESCAPE;
    case 32:
      return jevent::JKS_SPACE;
    // case 39:
    //  return jevent::JKS_APOSTHROFE;
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
    case 59:
      return jevent::JKS_SEMICOLON;
    case 61: 
      return jevent::JKS_EQUALS_SIGN;
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
    case 96:
      return jevent::JKS_GRAVE_ACCENT;
    case 127:
      return jevent::JKS_DELETE;
    case 37:
      return jevent::JKS_CURSOR_LEFT;
    case 39:
      return jevent::JKS_CURSOR_RIGHT;
    case 38:  
      return jevent::JKS_CURSOR_UP;
    case 40:
      return jevent::JKS_CURSOR_DOWN;
    // case 16777222:  
    //  return jevent::JKS_INSERT;
    // case 16777232:     
    //  return jevent::JKS_HOME;
    // case 16777233:
    //  return jevent::JKS_END;
    // case 16777238:
    //  return jevent::JKS_PAGE_UP;
    // case 16777239:
    //  return jevent::JKS_PAGE_DOWN;
    // case ALLEGRO_KEY_PRINTSCREEN:   
    //  return jevent::JKS_PRINT;
    // case 16777224:
    //  return jevent::JKS_PAUSE;
    // case ALLEGRO_KEY_RED:
    //  return jevent::JKS_RED;
    // case ALLEGRO_KEY_GREEN:
    //  return jevent::JKS_GREEN;
    // case ALLEGRO_KEY_YELLOW:
    //  return jevent::JKS_YELLOW;
    // case ALLEGRO_KEY_BLUE:
    //  return jevent::JKS_BLUE;
    case 65470:
      return jevent::JKS_F1;
    case 65471:
      return jevent::JKS_F2;
    case 65472:
      return jevent::JKS_F3;
    case 65473:
      return jevent::JKS_F4;
    case 65474:
      return jevent::JKS_F5;
    case 65475:
      return jevent::JKS_F6;
    case 65476:
      return jevent::JKS_F7;
    case 65477:
      return jevent::JKS_F8;
    case 65478:
      return jevent::JKS_F9;
    case 65479:
      return jevent::JKS_F10;
    case 65480:
      return jevent::JKS_F11;
    case 65481:
      return jevent::JKS_F12;
    case 16:
      return jevent::JKS_SHIFT;
    case 17:
      return jevent::JKS_CONTROL;
    // case 16777251:
    //  return jevent::JKS_ALT;
    // case ALLEGRO_KEY_ALTGR:
    //  return jevent::JKS_ALTGR;
    // case ALLEGRO_KEY_LMETA:
    // case ALLEGRO_KEY_RMETA:
    //  return jevent::JKS_META;
    // case ALLEGRO_KEY_LSUPER:
    // case ALLEGRO_KEY_RSUPER:
    //  return jevent::JKS_SUPER;
    // case ALLEGRO_KEY_HYPER:
    //  return jevent::JKS_HYPER;
    default: 
      break;
  }

  return jevent::JKS_UNKNOWN;
}

void Application::Init(int argc, char **argv)
{
  nana::screen s = nana::screen();
  const ::nana::rectangle r = s.get_primary().workarea();

  sg_screen.width = r.width;
  sg_screen.height = r.height;
  
  sg_quitting = false;
}

static void InternalPaint()
{
  while (sg_quitting == false) {
    std::unique_lock<std::mutex> lock(sg_paint_mutex);
    
    dw->update();

    sg_paint_condition.wait_for(lock, std::chrono::milliseconds(1));
  }
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_OPENED));

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  std::thread thread = std::thread(InternalPaint);

  ::nana::exec();

  sg_quitting = true;

  thread.join();
  
  sg_jgui_window->SetVisible(false);
}

jsize_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;
  
  fm->close();

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

static void destroy_callback(const nana::arg_destroy &arg)
{
  fm->close();

  sg_quitting = true;

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
}

static void expose_callback(const nana::arg_expose &arg)
{
  dw->update();
}

static void resized_callback(const nana::arg_resized &arg)
{
  dw->update();
          
  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_RESIZED));
}

static void moved_callback(const nana::arg_move &arg)
{
  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_MOVED));
}

static void focus_callback(const nana::arg_mouse &arg)
{
  if (arg.evt_code == nana::event_code::mouse_enter) {
    sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
  } else if (arg.evt_code == nana::event_code::mouse_leave) {
    sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
  }
}

static void mouse_input_callback(const nana::arg_mouse &arg)
{
  static jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;

  int mouse_z = 0;
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;

  sg_mouse_x = arg.pos.x;
  sg_mouse_y = arg.pos.y;

  if (arg.evt_code == nana::event_code::mouse_move) {
    type = jevent::JMT_MOVED;
  } else {
    if (arg.button == nana::mouse::left_button) {
      button = jevent::JMB_BUTTON1;
    } else if (arg.button == nana::mouse::middle_button) {
      button = jevent::JMB_BUTTON2;
    } else if (arg.button == nana::mouse::right_button) {
      button = jevent::JMB_BUTTON3;
    }
  
    if (arg.evt_code == nana::event_code::mouse_down or arg.evt_code == nana::event_code::dbl_click) {
      type = jevent::JMT_PRESSED;
      buttons = (jevent::jmouseevent_button_t)(buttons | button);
    } else if (arg.evt_code == nana::event_code::mouse_up) {
      type = jevent::JMT_RELEASED;
      buttons = (jevent::jmouseevent_button_t)(buttons & ~button);
    }

    mouse_z = 1;
  
    if (arg.evt_code == nana::event_code::dbl_click) {
      mouse_z = 2;
    }
  }

  if (sg_jgui_window->GetEventManager()->IsAutoGrab() == true && buttons != jevent::JMB_NONE) {
    fm->set_capture(true);
  } else {
    fm->release_capture();
  }

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mouse_wheel_input_callback(const nana::arg_wheel &arg)
{
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_ROTATED;

  int mouse_z = 1;

  if (arg.upwards == false) {
    mouse_z = -1;
  }

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void key_input_callback(const nana::arg_keyboard &arg)
{
  jevent::jkeyevent_type_t type;
  jevent::jkeyevent_modifiers_t mod;

  mod = (jevent::jkeyevent_modifiers_t)(0);

  if (arg.shift == true) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
  } else if (arg.ctrl == true) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
  } else if (arg.alt == true) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
  }

  type = jevent::JKT_UNKNOWN;
  
  if (arg.evt_code == nana::event_code::key_press) {
    type = jevent::JKT_PRESSED;
  } else if (arg.evt_code == nana::event_code::key_release) {
    type = jevent::JKT_RELEASED;
  }

  jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(arg.key);

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void paint_callback(const nana::paint::graphics& graph)
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
    sg_jgui_window->Repaint();
  } else {
    g->Reset();
    g->SetCompositeFlags(jgui::JCF_SRC);

    sg_jgui_window->DoLayout();
    sg_jgui_window->Paint(g);
  }

  g->Flush();

  uint8_t *data = sg_back_buffer->LockData();

  nana::paint::pixel_buffer pixbuf{ graph.handle(), nana::rectangle{ graph.size() } };
  // size sz = pixbuf.size();

  for (int i=0; i<bounds.size.height; i++) {
    const uint8_t *src = data + i*bounds.size.width*4;

    pixbuf.fill_row(i, src, bounds.size.width*4, 32);
  }

  // pixbuf.put((unsigned char *)data, bounds.size.width, bounds.size.height, 32, bounds.size.width*4, false);
  pixbuf.paste(graph.handle(), {});

  sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
  
  std::this_thread::yield();
}

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::WindowAdapter()
{
  jcommon::Object::SetClassName("jgui::NativeWindow");

  if (fm != nullptr) {
    throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  sg_mouse_x = 0;
  sg_mouse_y = 0;
  sg_jgui_window = parent;

  fm = new nana::form{nana::API::make_center(bounds.size.width, bounds.size.height)};

  fm->move(bounds.point.x, bounds.point.y);

  dw = new nana::drawing(*fm);

  dw->draw(paint_callback);
  
  // TODO:: cursor, resize, expose
  fm->events().destroy(destroy_callback);
  fm->events().expose(expose_callback);
  fm->events().resized(resized_callback);
  fm->events().move(moved_callback);
  fm->events().mouse_enter(focus_callback);
  fm->events().mouse_leave(focus_callback);

  fm->events().dbl_click(mouse_input_callback);
  fm->events().mouse_move(mouse_input_callback);
  fm->events().mouse_down(mouse_input_callback);
  fm->events().mouse_up(mouse_input_callback);
  fm->events().mouse_wheel(mouse_wheel_input_callback);
  
  fm->events().key_press(key_input_callback);
  fm->events().key_release(key_input_callback);

  fm->show();
}

NativeWindow::~NativeWindow()
{
  fm->close();

  delete fm;
  delete dw;
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_paint_condition.notify_all();
}

void NativeWindow::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_last_bounds = GetBounds();

    fm->size({(nana::size::value_type)sg_screen.width, (nana::size::value_type)sg_screen.height});
    fm->move({0, 0});

    sg_fullscreen = true;
  } else {
    fm->size({(nana::size::value_type)sg_last_bounds.size.width, (nana::size::value_type)sg_last_bounds.size.height});
    fm->move({sg_last_bounds.point.x, sg_last_bounds.point.y});

    sg_fullscreen = false;
  }

}

void NativeWindow::SetTitle(std::string title)
{
  fm->caption(title);
}

std::string NativeWindow::GetTitle()
{
  return fm->caption();
}

void NativeWindow::SetOpacity(float opacity)
{
}

float NativeWindow::GetOpacity()
{
  return 1.0;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
}

bool NativeWindow::IsUndecorated()
{
  return false;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  fm->size({(nana::size::value_type)width, (nana::size::value_type)height});
  fm->move({x, y});
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
  nana::size s = fm->size();
  nana::point p = fm->pos();

  return {
    p.x,
    p.y,
    (int)s.width,
    (int)s.height
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
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
  jpoint_t<int> p {
    .x = sg_mouse_x, 
    .y = sg_mouse_y
  };

  return p;
}

void NativeWindow::SetVisible(bool visible)
{
  if (visible == false) {
    fm->hide();
  } else {
    fm->show();
  }
}

bool NativeWindow::IsVisible()
{
  return fm->visible();
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
}

bool NativeWindow::IsCursorEnabled()
{
  return true;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  nana::cursor type = nana::cursor::arrow;

  if (style == JCS_DEFAULT) {
  } else if (style == JCS_CROSSHAIR) {
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = nana::cursor::hand;
  } else if (style == JCS_MOVE) {
  } else if (style == JCS_NS) {
    type = nana::cursor::size_ns;
  } else if (style == JCS_WE) {
    type = nana::cursor::size_we;
  } else if (style == JCS_NW_CORNER) {
    type = nana::cursor::size_top_left;
  } else if (style == JCS_NE_CORNER) {
    type = nana::cursor::size_top_right;
  } else if (style == JCS_SW_CORNER) {
    type = nana::cursor::size_bottom_left;
  } else if (style == JCS_SE_CORNER) {
    type = nana::cursor::size_bottom_right;
  } else if (style == JCS_TEXT) {
    type = nana::cursor::iterm;
  } else if (style == JCS_WAIT) {
    type = nana::cursor::wait;
  }

  fm->cursor(type);
  
  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
  // TODO::
}

jwindow_rotation_t NativeWindow::GetRotation()
{
  return jgui::JWR_NONE;
}

void NativeWindow::SetIcon(jgui::Image *image)
{
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}

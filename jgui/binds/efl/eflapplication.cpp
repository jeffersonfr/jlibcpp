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

#include <Ecore_Getopt.h>
#include <Elementary.h>

#include <stdio.h>
#include <errno.h>

namespace jgui {

/** \brief */
Evas_Object *sg_window = nullptr;
/** \brief */
Evas_Object *sg_surface = nullptr;
/** \brief */
Evas_Coord sg_mouse_x = 0;
/** \brief */
Evas_Coord sg_mouse_y = 0;
/** \brief */
Eina_Bool _mouse_down = false;
/** \brief */
jgui::jrect_t<int> sg_bounds = {0, 0, 0, 0};
/** \brief */
bool sg_visible = false;
/** \brief */
double _mouse_wheel_x = 0.0;
/** \brief */
double _mouse_wheel_y = 0.0;
/** \brief */
double _mouse_wheel_scale = 1.0;
/** \brief */
Ecore_Timer *_mouse_wheel_timer = nullptr;
/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
jevent::jmouseevent_button_t sg_button_state = jevent::JMB_NONE;
/** \brief */
static bool sg_repaint = false;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(std::string key)
{
  key = jcommon::StringUtils::ToLower(key);

  if (key == "return" or key == "kp_enter") {
    return jevent::JKS_ENTER; // jevent::JKS_RETURN;
  } else if (key == "backspace") {
    return jevent::JKS_BACKSPACE;
  } else if (key == "tab") {
    return jevent::JKS_TAB;
    // } else if (key == "") {
    //	return jevent::JKS_CANCEL;
  } else if (key == "escape") {
    return jevent::JKS_ESCAPE;
  } else if (key == "space") {
    return jevent::JKS_SPACE;
  } else if (key == "exclam") {
    return jevent::JKS_EXCLAMATION_MARK;
  } else if (key == "apostrophe") {
    return jevent::JKS_QUOTATION;
  } else if (key == "numbersign") {
    return jevent::JKS_NUMBER_SIGN;
  } else if (key == "dollar") {
    return jevent::JKS_DOLLAR_SIGN;
  } else if (key == "percent") {
    return jevent::JKS_PERCENT_SIGN;
  } else if (key == "ampersand") {   
    return jevent::JKS_AMPERSAND;
  } else if (key == "apostrophe") {
    return jevent::JKS_APOSTROPHE;
  } else if (key == "parenleft") {
    return jevent::JKS_PARENTHESIS_LEFT;
  } else if (key == "parenright") {
    return jevent::JKS_PARENTHESIS_RIGHT;
  } else if (key == "asterisk") {
    return jevent::JKS_STAR;
  } else if (key == "plus") {
    return jevent::JKS_PLUS_SIGN;
  } else if (key == "comma") {   
    return jevent::JKS_COMMA;
  } else if (key == "minus") {
    return jevent::JKS_MINUS_SIGN;
  } else if (key == "period") {  
    return jevent::JKS_PERIOD;
  } else if (key == "slash") {
    return jevent::JKS_SLASH;
  } else if (key == "0") {     
    return jevent::JKS_0;
  } else if (key == "1") {
    return jevent::JKS_1;
  } else if (key == "2") {
    return jevent::JKS_2;
  } else if (key == "3") {
    return jevent::JKS_3;
  } else if (key == "4") {
    return jevent::JKS_4;
  } else if (key == "5") {
    return jevent::JKS_5;
  } else if (key == "6") {
    return jevent::JKS_6;
  } else if (key == "7") {
    return jevent::JKS_7;
  } else if (key == "8") {
    return jevent::JKS_8;
  } else if (key == "9") {
    return jevent::JKS_9;
  } else if (key == "colon") {
    return jevent::JKS_COLON;
  } else if (key == "semicolon") {
    return jevent::JKS_SEMICOLON;
  } else if (key == "less") {
    return jevent::JKS_LESS_THAN_SIGN;
  } else if (key == "equal") { 
    return jevent::JKS_EQUALS_SIGN;
  } else if (key == "greater") {
    return jevent::JKS_GREATER_THAN_SIGN;
  } else if (key == "question") {   
    return jevent::JKS_QUESTION_MARK;
  } else if (key == "at") {
    return jevent::JKS_AT;
  } else if (key == "A") {
    return jevent::JKS_A;
  } else if (key == "B") {
    return jevent::JKS_B;
  } else if (key == "C") {
    return jevent::JKS_C;
  } else if (key == "D") {
    return jevent::JKS_D;
  } else if (key == "E") {
    return jevent::JKS_E;
  } else if (key == "F") {
    return jevent::JKS_F;
  } else if (key == "G") {
    return jevent::JKS_G;
  } else if (key == "H") {
    return jevent::JKS_H;
  } else if (key == "I") {
    return jevent::JKS_I;
  } else if (key == "J") {
    return jevent::JKS_J;
  } else if (key == "K") {
    return jevent::JKS_K;
  } else if (key == "L") {
    return jevent::JKS_L;
  } else if (key == "M") {
    return jevent::JKS_M;
  } else if (key == "N") {
    return jevent::JKS_N;
  } else if (key == "O") {
    return jevent::JKS_O;
  } else if (key == "P") {
    return jevent::JKS_P;
  } else if (key == "Q") {
    return jevent::JKS_Q;
  } else if (key == "R") {
    return jevent::JKS_R;
  } else if (key == "S") {
    return jevent::JKS_S;
  } else if (key == "T") {
    return jevent::JKS_T;
  } else if (key == "U") {
    return jevent::JKS_U;
  } else if (key == "V") {
    return jevent::JKS_V;
  } else if (key == "W") {
    return jevent::JKS_W;
  } else if (key == "X") {
    return jevent::JKS_X;
  } else if (key == "Y") {
    return jevent::JKS_Y;
  } else if (key == "Z") {
    return jevent::JKS_Z;
  } else if (key == "bracketleft") {
    return jevent::JKS_SQUARE_BRACKET_LEFT;
  } else if (key == "backslash") {
    return jevent::JKS_BACKSLASH;
  } else if (key == "bracketright") {
    return jevent::JKS_SQUARE_BRACKET_RIGHT;
  } else if (key == "asciicircum") {
    return jevent::JKS_CIRCUMFLEX_ACCENT;
  } else if (key == "underscore") {    
    return jevent::JKS_UNDERSCORE;
  } else if (key == "grave") {
    return jevent::JKS_GRAVE_ACCENT;
  } else if (key == "a") {
    return jevent::JKS_a;
  } else if (key == "b") {
    return jevent::JKS_b;
  } else if (key == "c") {
    return jevent::JKS_c;
  } else if (key == "d") {
    return jevent::JKS_d;
  } else if (key == "e") {
    return jevent::JKS_e;
  } else if (key == "f") {
    return jevent::JKS_f;
  } else if (key == "g") {
    return jevent::JKS_g;
  } else if (key == "h") {
    return jevent::JKS_h;
  } else if (key == "i") {
    return jevent::JKS_i;
  } else if (key == "j") {
    return jevent::JKS_j;
  } else if (key == "k") {
    return jevent::JKS_k;
  } else if (key == "l") {
    return jevent::JKS_l;
  } else if (key == "m") {
    return jevent::JKS_m;
  } else if (key == "n") {
    return jevent::JKS_n;
  } else if (key == "o") {
    return jevent::JKS_o;
  } else if (key == "p") {
    return jevent::JKS_p;
  } else if (key == "q") {
    return jevent::JKS_q;
  } else if (key == "r") {
    return jevent::JKS_r;
  } else if (key == "s") {
    return jevent::JKS_s;
  } else if (key == "t") {
    return jevent::JKS_t;
  } else if (key == "u") {
    return jevent::JKS_u;
  } else if (key == "v") {
    return jevent::JKS_v;
  } else if (key == "w") {
    return jevent::JKS_w;
  } else if (key == "x") {
    return jevent::JKS_x;
  } else if (key == "y") {
    return jevent::JKS_y;
  } else if (key == "z") {
    return jevent::JKS_z;
  } else if (key == "braceleft") {
    return jevent::JKS_CURLY_BRACKET_LEFT;
  } else if (key == "bar") {  
    return jevent::JKS_VERTICAL_BAR;
  } else if (key == "braceright") {
    return jevent::JKS_CURLY_BRACKET_RIGHT;
  } else if (key == "asciitilde") {  
    return jevent::JKS_TILDE;
  } else if (key == "delete") {
    return jevent::JKS_DELETE;
  } else if (key == "left") {
    return jevent::JKS_CURSOR_LEFT;
  } else if (key == "right") {
    return jevent::JKS_CURSOR_RIGHT;
  } else if (key == "up") {  
    return jevent::JKS_CURSOR_UP;
  } else if (key == "down") {
    return jevent::JKS_CURSOR_DOWN;
  } else if (key == "insert") {
    return jevent::JKS_INSERT;
  } else if (key == "home" or key == "kp_home") {
    return jevent::JKS_HOME;
  } else if (key == "end" or key == "kp_end") {
    return jevent::JKS_END;
  } else if (key == "prior" or key == "kp_prior") {
    return jevent::JKS_PAGE_UP;
  } else if (key == "next" or key == "kp_next") {
    return jevent::JKS_PAGE_DOWN;
  // } else if (key == "") {   
  //  return jevent::JKS_PRINT;
  } else if (key == "pause") {
    return jevent::JKS_PAUSE;
  //} else if (key == "") {
  //  return jevent::JKS_RED;
  //} else if (key == "") {
  //  return jevent::JKS_GREEN;
  //} else if (key == "") {
  //  return jevent::JKS_YELLOW;
  //} else if (key == "") {
  //  return jevent::JKS_BLUE;
  } else if (key == "f1") {
    return jevent::JKS_F1;
  } else if (key == "f2") {
    return jevent::JKS_F2;
  } else if (key == "f3") {
    return jevent::JKS_F3;
  } else if (key == "f4") {
    return jevent::JKS_F4;
  } else if (key == "f5") {
    return jevent::JKS_F5;
  } else if (key == "f6") {     
    return jevent::JKS_F6;
  } else if (key == "f7") {    
    return jevent::JKS_F7;
  } else if (key == "f8") {   
    return jevent::JKS_F8;
  } else if (key == "f9") {  
    return jevent::JKS_F9;
  } else if (key == "f10") { 
    return jevent::JKS_F10;
  } else if (key == "f11") {
    return jevent::JKS_F11;
  } else if (key == "f12") {
    return jevent::JKS_F12;
  } else if (key == "shift_l" or key == "shift_r") {
    return jevent::JKS_SHIFT;
  } else if (key == "control_l" or key == "control_r") {
    return jevent::JKS_CONTROL;
  } else if (key == "alt_l" or key == "alt_r") {
    return jevent::JKS_ALT;
  } else if (key == "altgr") {
    return jevent::JKS_ALTGR;
  // } else if (key == "") {
  //  return jevent::JKS_META;
  } else if (key == "win") {
    return jevent::JKS_SUPER;
  // } else if (key == "") {
  //  return jevent::JKS_HYPER;
  }

  return jevent::JKS_UNKNOWN;
}

void Application::Init(int argc, char **argv)
{
  // CHANGE:: initializing ecore_evas just to get screen size
  if (!ecore_evas_init()) {
    throw jexception::RuntimeException("Unable to init ecore evas");
  }

  static Ecore_Evas 
    *ee = ecore_evas_new(NULL, 0, 0, 0, 0, NULL);

  if (!ee) {
    throw jexception::RuntimeException("Unable to create a ecore evas object");
  }

  int w, h;
  
  ecore_evas_screen_geometry_get(ee, nullptr, nullptr, &w, &h);
  ecore_evas_free(ee);

  _elm_startup_time = ecore_time_unix_get();
  
  elm_init(argc, argv);

  sg_screen.width = w;
  sg_screen.height = h;

  sg_quitting = false;
}

static void InternalPaint(Evas_Object *content)
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
  g->SetCompositeFlags(jgui::JCF_SRC);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();
  
  uint32_t *src = (uint32_t *)sg_back_buffer->LockData();

  Evas_Coord ww, wh;
  uint32_t *dst, *pixel;

  evas_object_geometry_get(content, NULL, NULL, &ww, &wh);
  dst = (unsigned int*)evas_object_image_data_get(sg_surface, EINA_TRUE);
  pixel = dst;

  for (int j=0; j<wh; j++) {
    for (int i=0; i<ww; i++) {
      *pixel++ = *src++;
    }
  }

  evas_object_image_data_set(sg_surface, dst);
  evas_object_image_data_update_add(sg_surface, 0, 0, ww, wh);

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

static void render_layout(Evas_Object *content, bool paint)
{
  Evas_Coord wx, wy, ww, wh;

  evas_object_geometry_get(sg_window, &wx, &wy, &ww, &wh);
  evas_object_image_size_set(sg_surface, ww, wh);

  sg_bounds = jgui::jrect_t<int>{
    wx, wy, ww, wh
  };

  if (paint == true) {
    InternalPaint(content);
  }
}

void fractally_render_refresh(Evas_Object *content)
{
  Evas_Coord ww, wh;
  unsigned int *data, *pixel;
  int x, y;

  evas_object_geometry_get(content, NULL, NULL, &ww, &wh);
  data = (unsigned int*)evas_object_image_data_get(sg_surface, EINA_TRUE);
  pixel = data;

  for (y = 0; y < wh; y++)
    for (x = 0; x < ww; x++) {
      *pixel++ = random()%0xffffffff;
    }
  evas_object_image_data_set(sg_surface, data);
  evas_object_image_data_update_add(sg_surface, 0, 0, ww, wh);
}

static Eina_Bool InternalPaintTick(void *data)
{
  if (sg_quitting == true) {
     return ECORE_CALLBACK_CANCEL;
  }

   Evas_Object *window = (Evas_Object*)data;

   if (sg_repaint == true) {
     sg_repaint = false;

     InternalPaint(window);
   }

   return ECORE_CALLBACK_RENEW;
}

void Application::Loop()
{
  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  ecore_timer_add(0.0, InternalPaintTick, sg_window);

  elm_run();

  sg_quitting = true;
  sg_visible = false;

  sg_jgui_window->SetVisible(false);
}

void Application::Quit()
{
  sg_quitting = true;

  elm_exit();

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

static void mousedown_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  // grabbing
  // evas_object_pointer_mode_set(sg_window, EVAS_OBJECT_POINTER_MODE_AUTOGRAB); // EVAS_OBJECT_POINTER_MODE_NOGRAB

  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_PRESSED;

  if (ev->button == 1) {
    button = jevent::JMB_BUTTON1;
  } else if (ev->button == 2) {
    button = jevent::JMB_BUTTON2;
  } else if (ev->button == 3) {
    button = jevent::JMB_BUTTON3;
  }

  mouse_z = 1;

  if (ev->flags == EVAS_BUTTON_DOUBLE_CLICK) {
    mouse_z = 2;
  } else if (ev->flags == EVAS_BUTTON_TRIPLE_CLICK) {
    mouse_z = 3;
  }

  sg_button_state = (jevent::jmouseevent_button_t)(sg_button_state | button);

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mouseup_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_RELEASED;

  if (ev->button == 1) {
    button = jevent::JMB_BUTTON1;
  } else if (ev->button == 2) {
    button = jevent::JMB_BUTTON2;
  } else if (ev->button == 3) {
    button = jevent::JMB_BUTTON3;
  }

  mouse_z = 1;

  sg_button_state = (jevent::jmouseevent_button_t)(sg_button_state & ~button);

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mousemove_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)einfo;

  int mouse_z = 0;
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_MOVED;

  sg_mouse_x = ev->cur.canvas.x;
  sg_mouse_y = ev->cur.canvas.y;

  sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.width - 1);
  sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.height - 1);

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void mousewheel_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*)einfo;
  // const Evas_Modifier *mods = evas_key_modifier_get(evas);

  int mouse_z = 0;
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_ROTATED;

  mouse_z = ev->z;

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, sg_button_state, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

static void keydown_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)einfo;
  const Evas_Modifier *mods = evas_key_modifier_get(evas);
  
  jevent::jkeyevent_type_t type = jevent::JKT_PRESSED;
  jevent::jkeyevent_modifiers_t mod;

  mod = (jevent::jkeyevent_modifiers_t)(0);

  if (evas_key_modifier_is_set(mods, "Control")) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
  } else if (evas_key_modifier_is_set(mods, "Shift")) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
  } else if (evas_key_modifier_is_set(mods, "Alt")) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
  }

  jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev->key);

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void keyup_callback(void *data EINA_UNUSED, Evas *evas, Evas_Object *o EINA_UNUSED, void *einfo)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)einfo;
  const Evas_Modifier *mods = evas_key_modifier_get(evas);
  
  jevent::jkeyevent_type_t type = jevent::JKT_RELEASED;
  jevent::jkeyevent_modifiers_t mod;

  mod = (jevent::jkeyevent_modifiers_t)(0);

  if (evas_key_modifier_is_set(mods, "Control")) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
  } else if (evas_key_modifier_is_set(mods, "Shift")) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
  } else if (evas_key_modifier_is_set(mods, "Alt")) {
    mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
  }

  jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev->key);

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void window_resize_callback(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
  render_layout(obj, true);
}

static void window_delete_callback(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
  sg_quitting = true;

  elm_exit();
}

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::WindowAdapter()
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_bounds = bounds;
  sg_jgui_window = parent;

  Evas_Object *content = nullptr;

  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

  sg_window = elm_win_util_standard_add("main", "Main");
  
  if (sg_window == nullptr) {
    throw jexception::RuntimeException("Unable to create window");
  }

  elm_win_focus_highlight_enabled_set(sg_window, EINA_TRUE);
  
  evas_object_resize(sg_window, bounds.size.width * elm_config_scale_get(), bounds.size.height * elm_config_scale_get());

  evas_object_smart_callback_add(sg_window, "delete,request", window_delete_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_RESIZE, window_resize_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_KEY_DOWN, keydown_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_KEY_UP, keyup_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_DOWN, mousedown_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_UP, mouseup_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_MOVE, mousemove_callback, NULL);
  evas_object_event_callback_add(sg_window, EVAS_CALLBACK_MOUSE_WHEEL, mousewheel_callback, NULL);

  content = evas_object_rectangle_add(sg_window);

  elm_win_resize_object_add(sg_window, content);
  evas_object_show(content);

  // INFO:: init surface
  Evas_Coord ww, wh;

  evas_object_geometry_get(sg_window, NULL, NULL, &ww, &wh);
  evas_object_hide(content);

  sg_surface = evas_object_image_add(evas_object_evas_get(content));

  evas_object_image_alpha_set(sg_surface, EINA_FALSE);
  evas_object_image_source_set(sg_surface, content);

  evas_object_image_filled_set(sg_surface, EINA_TRUE);
  evas_object_show(sg_surface);

  elm_win_resize_object_add(sg_window, sg_surface);
  evas_object_image_size_set(sg_surface, ww, wh);

  render_layout(sg_surface, false);

  evas_object_show(sg_window);

  render_layout(sg_window, false);

  sg_visible = true;
}

NativeWindow::~NativeWindow()
{
  elm_shutdown();
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint = true;
}

void NativeWindow::ToggleFullScreen()
{
}

void NativeWindow::SetTitle(std::string title)
{
}

std::string NativeWindow::GetTitle()
{
	return std::string();
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
  evas_object_resize(sg_window, width, height);
  evas_object_move(sg_window, x, y);
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
  return sg_bounds;
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

jgui::jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t p {
    .x = 0,
    .y = 0
  };

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
  if (visible == IsVisible()) {
    return;
  }

  sg_visible = visible;

  if (visible == true) {
    evas_object_show(sg_window);
  } else {
    evas_object_hide(sg_window);
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
}

bool NativeWindow::IsCursorEnabled()
{
	return true;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t 
    t = shape->GetSize();
	uint32_t 
    data[t.width*t.height];

	shape->GetGraphics()->GetRGBArray(data, {{0, 0}, {t.width, t.height}});

  /*
	SDL_Surface 
    *surface = SDL_CreateRGBSurfaceFrom(data, t.width, t.height, 32, t.width*4, 0, 0, 0, 0);

	if (surface == nullptr) {
		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);
  */
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
  if (image == nullptr) {
    return;
  }

  /*
  cairo_surface_t 
    *cairo_surface = cairo_get_target(image->GetGraphics()->GetCairoContext());

  sg_jgui_icon = image;

  if (cairo_surface == nullptr) {
    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    return;
  }
  */

  /*
  SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(data, dw, dh, 32, dw*4, 0, 0, 0, 0);

  if (nullptr == icon) {
    return;
  }

  SDL_SetWindowIcon(sg_window, icon);
  SDL_FreeSurface(icon);
  */
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}

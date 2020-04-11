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
#ifndef J_KEYEVENT_H
#define J_KEYEVENT_H

#include "jevent/jeventobject.h"

namespace jevent {

/**
 * \brief
 *
 */
enum jkeyevent_type_t {
  JKT_UNKNOWN,
  JKT_PRESSED,
  JKT_RELEASED,
  JKT_TYPED
};

/**
 * \brief
 *
 */
enum jkeyevent_modifiers_t {
  JKM_NONE = 0x0000,
  JKM_SHIFT = 0x0001,
  JKM_CONTROL = 0x0002,
  JKM_ALT = 0x0004,
  JKM_ALTGR = 0x0008,
  JKM_META = 0x0010,
  JKM_SUPER = 0x0020,
  JKM_HYPER = 0x0040,
  JKM_CAPS_LOCK = 0x0080,
  JKM_NUM_LOCK = 0x0100,
  JKM_SCROLL_LOCK = 0x0200
};

/**
 * \brief
 *
 */
enum jkeyevent_symbol_t {
  JKS_UNKNOWN,
  JKS_EXIT,
  JKS_BACKSPACE,
  JKS_TAB,
  JKS_ENTER,
  JKS_CANCEL,
  JKS_ESCAPE,
  JKS_SPACE,
  JKS_EXCLAMATION_MARK,
  JKS_QUOTATION,
  JKS_NUMBER_SIGN,
  JKS_DOLLAR_SIGN,
  JKS_PERCENT_SIGN,
  JKS_AMPERSAND,
  JKS_APOSTROPHE,
  JKS_PARENTHESIS_LEFT,
  JKS_PARENTHESIS_RIGHT,
  JKS_STAR,
  JKS_SHARP,
  JKS_PLUS_SIGN,
  JKS_COMMA,
  JKS_MINUS_SIGN,
  JKS_PERIOD,
  JKS_SLASH,
  JKS_0,
  JKS_1,
  JKS_2,
  JKS_3,   
  JKS_4,
  JKS_5,
  JKS_6,
  JKS_7,
  JKS_8,
  JKS_9,

  JKS_COLON,
  JKS_SEMICOLON,
  JKS_LESS_THAN_SIGN,
  JKS_EQUALS_SIGN,
  JKS_GREATER_THAN_SIGN, 
  JKS_QUESTION_MARK,
  JKS_AT,

  JKS_A,
  JKS_B,
  JKS_C,
  JKS_D,
  JKS_E,
  JKS_F,
  JKS_G,
  JKS_H,
  JKS_I,         
  JKS_J,       
  JKS_K,     
  JKS_L,   
  JKS_M, 
  JKS_N,
  JKS_O,
  JKS_P,
  JKS_Q,
  JKS_R,
  JKS_S,
  JKS_T,
  JKS_U,
  JKS_V,
  JKS_W,
  JKS_X,            
  JKS_Y,          
  JKS_Z,        

  JKS_a,      
  JKS_b,    
  JKS_c,  
  JKS_d,
  JKS_e,
  JKS_f,
  JKS_g,
  JKS_h,
  JKS_i,
  JKS_j,
  JKS_k,              
  JKS_l,            
  JKS_m,          
  JKS_n,        
  JKS_o,      
  JKS_p,    
  JKS_q,  
  JKS_r,
  JKS_s,
  JKS_t,
  JKS_u,
  JKS_v,
  JKS_w,
  JKS_x,
  JKS_y,
  JKS_z,

  JKS_CAPITAL_CEDILlA,
  JKS_SMALL_CEDILLA,

  JKS_SQUARE_BRACKET_LEFT,
  JKS_BACKSLASH,
  JKS_SQUARE_BRACKET_RIGHT,
  JKS_CIRCUMFLEX_ACCENT,
  JKS_UNDERSCORE,
  JKS_ACUTE_ACCENT,
  JKS_GRAVE_ACCENT,
  JKS_CURLY_BRACKET_LEFT,    
  JKS_VERTICAL_BAR,  
  JKS_CURLY_BRACKET_RIGHT,
  JKS_TILDE,

  JKS_DELETE,
  JKS_INSERT,
  JKS_HOME,
  JKS_END,
  JKS_PAGE_UP,
  JKS_PAGE_DOWN,
  JKS_PRINT,
  JKS_PAUSE,
  JKS_BREAK,
  JKS_SLEEP,
  JKS_SUSPEND,
  JKS_HIBERNATE,

  JKS_CURSOR_LEFT,
  JKS_CURSOR_RIGHT,
  JKS_CURSOR_UP,
  JKS_CURSOR_DOWN,

  JKS_CURSOR_LEFT_UP,
  JKS_CURSOR_LEFT_DOWN,  
  JKS_CURSOR_UP_RIGHT,
  JKS_CURSOR_DOWN_RIGHT,

  JKS_RED,
  JKS_GREEN,
  JKS_YELLOW,
  JKS_BLUE,

  JKS_F1, 
  JKS_F2,
  JKS_F3,
  JKS_F4,
  JKS_F5,
  JKS_F6,          
  JKS_F7,        
  JKS_F8,      
  JKS_F9,    
  JKS_F10,  
  JKS_F11,
  JKS_F12,

  JKS_SHIFT,
  JKS_CONTROL,    
  JKS_ALT,  
  JKS_ALTGR,
  JKS_META,
  JKS_SUPER,
  JKS_HYPER,

  JKS_POWER,
  JKS_MENU,
  JKS_FILE,
  JKS_INFO,
  JKS_BACK,
  JKS_GUIDE,

  JKS_CHANNEL_UP,
  JKS_CHANNEL_DOWN,

  JKS_VOLUME_UP,
  JKS_VOLUME_DOWN,

  JKS_PLAY,
  JKS_STOP,
  JKS_EJECT,
  JKS_REWIND,
  JKS_RECORD,
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class KeyEvent : public jevent::EventObject {

  private:
    /** \brief */
    int _code;
    /** \brief */
    jkeyevent_symbol_t _symbol;
    /** \brief */
    jkeyevent_type_t _type;
    /** \brief */
    jkeyevent_modifiers_t _mod;

  public:
    /**
     * \brief
     *
     */
    KeyEvent(void *source, jkeyevent_type_t type, jkeyevent_modifiers_t mod, int code, jkeyevent_symbol_t symbol);
    
    /**
     * \brief
     *
     */
    virtual ~KeyEvent();

    /**
     * \brief
     *
     */
    static int GetCodeFromSymbol(jkeyevent_symbol_t symbol);

    /**
     * \brief
     *
     */
    virtual int GetKeyCode();
    
    /**
     * \brief
     *
     */
    virtual jkeyevent_symbol_t GetSymbol();
    
    /**
     * \brief
     *
     */
    virtual jkeyevent_type_t GetType();
    
    /**
     * \brief
     *
     */
    virtual jkeyevent_modifiers_t GetModifiers();

};

}

#endif


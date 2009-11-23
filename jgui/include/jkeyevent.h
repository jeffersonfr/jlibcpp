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

#include "jeventobject.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jkey_type_t {
	JKEY_PRESSED,
	JKEY_RELEASED,
	JKEY_TYPED
};

/**
 * \brief
 *
 */
enum jinput_modifiers_t {
	JMOD_SHIFT				= 0x0001,
	JMOD_CONTROL			= 0x0002,
	JMOD_ALT					= 0x0004,
	JMOD_ALTGR				= 0x0008,
	JMOD_META					= 0x0010,
	JMOD_SUPER				= 0x0020,
	JMOD_HYPER				= 0x0040,
	JMOD_CAPS_LOCK		= 0x0080,
	JMOD_NUM_LOCK			= 0x0100,
	JMOD_SCROLL_LOCK	= 0x0200
};

/**
 * \brief
 *
 */
enum jkey_symbol_t {
  JKEY_UNKNOWN,
	JKEY_EXIT,
	JKEY_BACKSPACE,
	JKEY_TAB,
	JKEY_ENTER,
	JKEY_CANCEL,
	JKEY_ESCAPE,
	JKEY_SPACE,
	JKEY_EXCLAMATION_MARK,
	JKEY_QUOTATION,
	JKEY_NUMBER_SIGN,
	JKEY_DOLLAR_SIGN,
	JKEY_PERCENT_SIGN,
	JKEY_AMPERSAND,
	JKEY_APOSTROPHE,
	JKEY_PARENTHESIS_LEFT,
	JKEY_PARENTHESIS_RIGHT,
	JKEY_STAR,
	JKEY_SHARP,
	JKEY_PLUS_SIGN,
	JKEY_COMMA,
	JKEY_MINUS_SIGN,
	JKEY_PERIOD,
	JKEY_SLASH,
	JKEY_0,
	JKEY_1,
	JKEY_2,
	JKEY_3,   
	JKEY_4,
	JKEY_5,
	JKEY_6,
	JKEY_7,
	JKEY_8,
	JKEY_9,

	JKEY_COLON,
	JKEY_SEMICOLON,
	JKEY_LESS_THAN_SIGN,
	JKEY_EQUALS_SIGN,
	JKEY_GREATER_THAN_SIGN, 
	JKEY_QUESTION_MARK,
	JKEY_AT,

	JKEY_A,
	JKEY_B,
	JKEY_C,
	JKEY_D,
	JKEY_E,
	JKEY_F,
	JKEY_G,
	JKEY_H,
	JKEY_I,         
	JKEY_J,       
	JKEY_K,     
	JKEY_L,   
	JKEY_M, 
	JKEY_N,
	JKEY_O,
	JKEY_P,
	JKEY_Q,
	JKEY_R,
	JKEY_S,
	JKEY_T,
	JKEY_U,
	JKEY_V,
	JKEY_W,
	JKEY_X,            
	JKEY_Y,          
	JKEY_Z,        

	JKEY_a,      
	JKEY_b,    
	JKEY_c,  
	JKEY_d,
	JKEY_e,
	JKEY_f,
	JKEY_g,
	JKEY_h,
	JKEY_i,
	JKEY_j,
	JKEY_k,              
	JKEY_l,            
	JKEY_m,          
	JKEY_n,        
	JKEY_o,      
	JKEY_p,    
	JKEY_q,  
	JKEY_r,
	JKEY_s,
	JKEY_t,
	JKEY_u,
	JKEY_v,
	JKEY_w,
	JKEY_x,
	JKEY_y,
	JKEY_z,

	JKEY_SQUARE_BRACKET_LEFT,
	JKEY_BACKSLASH,
	JKEY_SQUARE_BRACKET_RIGHT,
	JKEY_CIRCUMFLEX_ACCENT,
	JKEY_UNDERSCORE,
	JKEY_GRAVE_ACCENT,
	JKEY_CURLY_BRACKET_LEFT,    
	JKEY_VERTICAL_BAR,  
	JKEY_CURLY_BRACKET_RIGHT,
	JKEY_TILDE,

	JKEY_DELETE,
	JKEY_INSERT,
	JKEY_HOME,
	JKEY_END,
	JKEY_PAGE_UP,
	JKEY_PAGE_DOWN,
	JKEY_PRINT,
	JKEY_PAUSE,

	JKEY_CURSOR_LEFT,
	JKEY_CURSOR_RIGHT,
	JKEY_CURSOR_UP,
	JKEY_CURSOR_DOWN,

	JKEY_CURSOR_LEFT_UP,
	JKEY_CURSOR_LEFT_DOWN,  
	JKEY_CURSOR_UP_RIGHT,
	JKEY_CURSOR_DOWN_RIGHT,

	JKEY_RED,
	JKEY_GREEN,
	JKEY_YELLOW,
	JKEY_BLUE,

	JKEY_F1, 
	JKEY_F2,
	JKEY_F3,
	JKEY_F4,
	JKEY_F5,
	JKEY_F6,          
	JKEY_F7,        
	JKEY_F8,      
	JKEY_F9,    
	JKEY_F10,  
	JKEY_F11,
	JKEY_F12,

	JKEY_SHIFT,
	JKEY_CONTROL,    
	JKEY_ALT,  
	JKEY_ALTGR,
	JKEY_META,
	JKEY_SUPER,
	JKEY_HYPER,

	JKEY_POWER,
	JKEY_MENU,
	JKEY_FILE,
	JKEY_INFO,
	JKEY_BACK,
	JKEY_GUIDE,

	JKEY_CHANNEL_UP,
	JKEY_CHANNEL_DOWN,

	JKEY_VOLUME_UP,
	JKEY_VOLUME_DOWN,

	JKEY_PLAY,
	JKEY_STOP,
	JKEY_EJECT,
	JKEY_REWIND,
	JKEY_RECORD,
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class KeyEvent : public jcommon::EventObject{

	private:
		int _code;
		jkey_symbol_t _symbol;
		jkey_type_t _type;
		jinput_modifiers_t _mod;

	public:
		/**
		 * \brief
		 *
		 */
		KeyEvent(void *source, jkey_type_t type, jinput_modifiers_t mod, int code, jkey_symbol_t symbol);
		
		/**
		 * \brief
		 *
		 */
		virtual ~KeyEvent();

		/**
		 * \brief
		 *
		 */
		int GetKeyCode();
		
		/**
		 * \brief
		 *
		 */
		jkey_symbol_t GetSymbol();
		
		/**
		 * \brief
		 *
		 */
		jkey_type_t GetType();
		
		/**
		 * \brief
		 *
		 */
		jinput_modifiers_t GetModifiers();

};

}

#endif


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
#ifndef MINES_H_
#define MINES_H_

#include "slidewindow.h"

#include "jframe.h"
#include "joffscreenimage.h"

#include <string>
#include <iostream>
#include <list>
using namespace std;

#include <directfb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace mines {

class Mines : public jgui::Frame, public jgui::FrameInputListener{

		enum block_type_t {
				CLOSED_BLOCK = 0,
				BOMB_BLOCK = -1,
				BLANK_BLOCK = -2,
				MARKED_BLOCK = -3,
				NUMBER_BLOCK = -4
		};

		enum block_state_t {
				EMPTY_STATE = -1,
				MARKED_STATE = -2,
				UNAVAILABLE_STATE = -255
		};

		struct block_t {
				block_type_t type;
				// block_state_t state;
				int state;
		};

		private:
				jthread::Mutex mines_mutex;

				jgui::Graphics *graphics;
				int bx,
					by,
					bwidth,
					bheight;
				int size,
					max_rows,
					max_cols,
					current_row,
					current_col,
					max_bombs,
					hide_bombs,
					game_state;
				block_t *board;
				SlideWindow *slide;
				jgui::OffScreenImage *prefetch1,
					*prefetch2;

		public:
				Mines(int x, int y);
				virtual ~Mines();

				virtual void Paint(jgui::Graphics *g);

				int DrawBlock(int row, int col, block_type_t type, int value = 0, bool update = true);
				void InitializeFlags();
				void SetupBoard();
				void Expose(int row, int col);
				int GetResult();
				void UpdateBoard(int flag);

				virtual void InputChanged(jgui::KeyEvent *event);

};

}

#endif 


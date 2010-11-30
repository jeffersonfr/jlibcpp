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

#include "jframe.h"

namespace mines {

class Mines : public jgui::Frame, public jgui::FrameInputListener{

	enum game_status_t {
		NONE,
		WIN,
		LOSE
	};

	enum block_type_t {
		BOMB_BLOCK,
		SAFE_BLOCK
	};

	enum block_state_t {
		CLOSED_BLOCK,
		OPENED_BLOCK,
		MARKED_BLOCK
	};

	struct block_t {
		block_type_t type;
		block_state_t state;
		int value;
	};

	private:
	jthread::Mutex mines_mutex;
	jgui::OffScreenImage *small_bomb,
		*huge_bomb,
		*flag,
		*smile_face,
		*dead_face;
	block_t *board;
	int size,
			max_rows,
			max_cols,
			current_row,
			current_col,
			max_bombs,
			hide_bombs;
	game_status_t game_state;

	public:
	Mines(int x, int y);
	virtual ~Mines();

	virtual void Paint(jgui::Graphics *g);

	void InitializeFlags();
	void SetupBoard();
	void Expose(int row, int col);
	game_status_t GetResult();

	virtual void InputChanged(jgui::KeyEvent *event);

};

}

#endif 


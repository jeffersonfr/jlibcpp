/***************************************************************************
 *   Copyright (C) 2005 _insets.top Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published _insets.top  *
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
#include "mines.h"
#include "jmessagedialogbox.h"
#include "jsystem.h"

namespace mines {

Mines::Mines(int x, int y):
	jgui::Frame("Minas", 150, 100, 920, 820)
{
	_insets.left = 20;

	size = 60;
	max_rows = 10;
	max_cols = 10;
	max_bombs = 20;

	board = new block_t[max_rows*max_cols+10]; // 10 _insets.toptes para overflow

	small_bomb = jgui::Image::CreateImage(size, size);
	huge_bomb = jgui::Image::CreateImage(4*size, 4*size);
	flag = jgui::Image::CreateImage(size, size);
	smile_face = jgui::Image::CreateImage(4*size, 4*size);
	dead_face = jgui::Image::CreateImage(4*size, 4*size);

	small_bomb->GetGraphics()->DrawImage("images/bomb.png", 0, 0, size, size);
	huge_bomb->GetGraphics()->DrawImage("images/bomb.png", 0, 0, 4*size, 4*size);
	flag->GetGraphics()->DrawImage("images/flag.png", 0, 0, size, size);
	smile_face->GetGraphics()->DrawImage("images/smile_face.png", 0, 0, 4*size, 4*size);
	dead_face->GetGraphics()->DrawImage("images/dead_face.png", 0, 0, 4*size, 4*size);

	SetupBoard();

	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", "Novo Jogo");
	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/yellow_icon.png", "Flag");
	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/green_icon.png", "Ajuda");

	Frame::RegisterInputListener(this);
}

Mines::~Mines() 
{
	jthread::AutoLock lock(&mines_mutex);

	delete board;

	delete small_bomb;
	delete huge_bomb;
	delete flag;
	delete smile_face;
	delete dead_face;
}

void Mines::Paint(jgui::Graphics *g)
{
	jgui::Frame::Paint(g);

	int delta = 4;

	g->SetFont(_font);

	for (int i=0; i<max_cols; i++) {
		for (int j=0; j<max_rows; j++) {
			block_t block = board[j*max_cols+i];

			g->SetColor(0x80, 0x80, 0x80, 0xff);

			if (block.state == CLOSED_BLOCK) {
				g->FillRectangle(_insets.left+i*(size+delta), _insets.top+j*(size+delta), size, size);
			} else if (block.state == OPENED_BLOCK) {
				g->SetColor(0xd0, 0xd0, 0xd0, 0xff);
				g->FillRectangle(_insets.left+i*(size+delta), _insets.top+j*(size+delta), size, size);

				if (block.type == BOMB_BLOCK) {
					g->FillRectangle(_insets.left+i*(size+delta), _insets.top+j*(size+delta), size, size);
					g->DrawImage(small_bomb, _insets.left+i*(size+delta)+2, _insets.top+j*(size+delta)+2);
				} else {
					if (block.value != 0) {
						char str[] = {block.value+'0', '\0'};

						g->SetColor(0xff, 0x00, 0x00, 0xff);
						g->DrawString(str, _insets.left+i*(size+delta), _insets.top+j*(size+delta), size, size, jgui::CENTER_HALIGN, jgui::CENTER_VALIGN);
					}
				}
			} else if (block.state == MARKED_BLOCK) {
				g->FillRectangle(_insets.left+i*(size+delta), _insets.top+j*(size+delta), size, size);
				g->DrawImage(flag, _insets.left+i*(size+delta)+2, _insets.top+j*(size+delta)+2);
			}
		}
	}

	g->SetColor(0x00, 0x00, 0x00, 0xff);

	if (GetResult() != LOSE) {
		g->DrawString("Parabens", GetWidth()-190, 100);
		g->DrawImage(smile_face, GetWidth()-210, 180, 160, 140);
	} else {
		g->DrawString("Perdeu", GetWidth()-160, 100);
		g->DrawImage(dead_face, GetWidth()-210, 180, 160, 140);
	}

	int x = GetWidth()-210,
			y = 400,
			w = 160,
			h = 140;
	char tmp[255];

	sprintf(tmp, "%02d", hide_bombs);

	g->DrawImage(huge_bomb, x, y, w, h);

	g->SetColor(0x00, 0x00, 0x00, 0xff);
	g->FillRectangle(x, y+h+10, w, _font->GetHeight());
	g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
	g->DrawString(tmp, x+10, y+h+10);
				
	g->SetColor(0x20, 0x40, 0xa0, 0x80);
	g->FillRectangle(_insets.left+current_col*(size+delta), _insets.top+current_row*(size+delta), size, size);
}

void Mines::InputChanged(jgui::KeyEvent *event)
{
	jthread::AutoLock lock(&mines_mutex);

	if (event->GetType() != jgui::JKEY_PRESSED) {
		return;
	}

	if (GetResult() == NONE) {
		if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT) {
			if (current_col < max_cols-1) {
				current_col++;
			}
		} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
			if (current_col > 0) {
				current_col--;
			}
		} else if (event->GetSymbol() == jgui::JKEY_CURSOR_UP) {
			if (current_row > 0) {
				current_row--;
			}
		} else if (event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
			if (current_row < max_rows-1) {
				current_row++;
			}
		} else if (event->GetSymbol() == jgui::JKEY_ENTER) {
			block_t *block = &board[current_row*max_cols+current_col];

			// INFO:: just open a block
			if (block->type == SAFE_BLOCK) {
				if (block->state == MARKED_BLOCK) {
					hide_bombs++;

					block->state = CLOSED_BLOCK;
				}

				if (block->state == CLOSED_BLOCK) {
					Expose(current_row, current_col);
				}
			} else if (block->type == BOMB_BLOCK) {
				game_state = LOSE;
			}

			int count = 0;

			for (int i=0; i<max_rows*max_cols; i++) {
				if (board[i].state == CLOSED_BLOCK || board[i].state == MARKED_BLOCK) {
					count++;
				}
			}

			if (count == max_bombs) {
				game_state = WIN;
			}
		} else if (event->GetSymbol() == jgui::JKEY_RED || event->GetSymbol() == jgui::JKEY_F1) {
		} else if (event->GetSymbol() == jgui::JKEY_GREEN || event->GetSymbol() == jgui::JKEY_F2) {
			jgui::MessageDialogBox dialog("Ajuda", "O jogo termina quando todos os blocos que n\xe3o possuem minas s\xe3o revelados. Utilize as bandeirinhas para ajudar a identificar as minas escondidas.", GetX()+300, GetY()+200);

			dialog.Show();
		} else if (event->GetSymbol() == jgui::JKEY_YELLOW || event->GetSymbol() == jgui::JKEY_F3) {
			block_t *block = &board[current_row*max_cols+current_col];

			if (block->state == CLOSED_BLOCK) {
				block->state = MARKED_BLOCK;

				hide_bombs--;
			} else if (block->state == MARKED_BLOCK) {
				block->state = CLOSED_BLOCK;

				hide_bombs++;
			}
		} else if (event->GetSymbol() == jgui::JKEY_BLUE || event->GetSymbol() == jgui::JKEY_F4) {
			SetupBoard();
		}

		if (GetResult() == WIN) {
			int rx, ry;

			for (int i=0; i<max_rows*max_cols; i++) {
				ry = i/max_cols;
				rx = i%max_cols;

				if (board[i].type == BOMB_BLOCK) {
					board[i].state = MARKED_BLOCK;
				}
			}
		} else if (GetResult() == LOSE) {
			int rx, ry;

			for (int i=0; i<max_rows*max_cols; i++) {
				ry = i/max_cols;
				rx = i%max_cols;

				if (board[i].type == BOMB_BLOCK) {
					board[i].state = OPENED_BLOCK;
				}
			}
		}

		Repaint();
	} else if (event->GetSymbol() == jgui::JKEY_BLUE || event->GetSymbol() == jgui::JKEY_F4) {
		SetupBoard();
	}
}

void Mines::InitializeFlags()
{
	current_row = 0;
	current_col = 0;
	hide_bombs = max_bombs;
	game_state = NONE;
}

void Mines::SetupBoard()
{
	InitializeFlags();

	for (int i=0; i<max_rows*max_cols; i++) {
		board[i].type = SAFE_BLOCK;
		board[i].state = CLOSED_BLOCK;
		board[i].value = 0;
	}

	int bombs = max_bombs;

	while (bombs > 0) {
		int rx = rand() % max_cols,
				ry = rand() % max_rows;

		if (board[ry*max_cols+rx].type != BOMB_BLOCK) {
			board[ry*max_cols+rx].type = BOMB_BLOCK;

			bombs--;
		}
	}

	for (int i=0; i<max_rows*max_cols; i++) {
		int ry = i/max_cols,
				rx = i%max_cols;

		if (board[i].type != BOMB_BLOCK) {
			int count = 0;

			if (rx > 0) {
				count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
			}

			if (rx < (max_cols-1)) {
				count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
			}

			if (ry > 0) {
				count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;

				if (rx > 0) {
					count += (board[(ry-1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
				}

				if (rx < (max_cols-1)) {
					count += (board[(ry-1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
				}
			}

			if (ry < (max_rows-1)) {
				count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;

				if (rx > 0) {
					count += (board[(ry+1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
				}

				if (rx < (max_cols-1)) {
					count += (board[(ry+1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
				}
			}

			if (count > 0) {
				board[i].value = count;
			}
		}
	}

	Repaint();
}

void Mines::Expose(int row, int col)
{
	if (col < 0 || col > (max_cols-1) || row < 0 || row > (max_rows-1)) {
		return;
	}

	block_t *block = &board[row*max_cols+col];

	if (block->type == SAFE_BLOCK && block->state == CLOSED_BLOCK) {
		block->state = OPENED_BLOCK;

		if (block->value != 0) {
			return;
		}

		Expose(row+0, col-1);
		Expose(row+0, col+1);
		Expose(row+1, col-1);
		Expose(row+1, col+0);
		Expose(row+1, col+1);
		Expose(row-1, col-1);
		Expose(row-1, col+0);
		Expose(row-1, col+1);
	}
}

Mines::game_status_t Mines::GetResult()
{
	return game_state;
}

}

int main()
{
	srand((int)time(NULL));

	mines::Mines app(100, 100);

	app.Show();

	return 0;
}


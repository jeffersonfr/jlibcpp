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
#include "mines.h"
#include "jmessagedialog.h"

namespace mines {

Mines::Mines(int x, int y):
		jgui::Frame("Minas", 150, 100, 920, 820)
{
	bx = 20;
	by = 100;

	size = 60;
	max_rows = 10;
	max_cols = 10;
	max_bombs = 20;

	graphics = NULL;

	board = new block_t[max_rows*max_cols+10]; // 10 bytes para overflow

	slide = new SlideWindow(bx+x, by+y, size, size);

	slide->SetInputEnabled(false);
	slide->Show(false);

	label = new jgui::Label("0", _width-170, _height-240, 90, 40);

	label->SetAlign(jgui::CENTER_ALIGN);
	label->SetTruncated(false);

	prefetch1 = new jgui::OffScreenImage(size, size);
	prefetch2 = new jgui::OffScreenImage(size, size);
	
	prefetch1->GetGraphics()->DrawImage("icons/bomb2.png", 0, 0, size, size);
	prefetch2->GetGraphics()->DrawImage("icons/flag1.png", 0, 0, size, size);

	Add(label);

	AddSubtitle("icons/blue_icon.png", "Novo Jogo");
	AddSubtitle("icons/yellow_icon.png", "Flag");
	AddSubtitle("icons/green_icon.png", "Ajuda");

	Frame::RegisterInputListener(this);
}

Mines::~Mines() 
{
	jthread::AutoLock lock(&mines_mutex);

	delete board;
	delete label;
	delete slide;
}

void Mines::Paint(jgui::Graphics *g)
{
	jgui::Frame::Paint(g);

	graphics = GetGraphics();

	SetupBoard();

	slide->RaiseToTop();
}

void Mines::InputChanged(jgui::KeyEvent *event)
{
	jthread::AutoLock lock(&mines_mutex);

	if (event->GetType() != jgui::JKEY_PRESSED) {
		return;
	}

	if (GetResult() == 0) {
		int delta = 5;

		if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT) {
			if (current_col < max_cols-1) {
				current_col++;
				slide->SetPosition(bx+_x+current_col*(size+delta), by+_y+current_row*(size+delta));
			}
		} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
			if (current_col > 0) {
				current_col--;
				slide->SetPosition(bx+_x+current_col*(size+delta), by+_y+current_row*(size+delta));
			}
		} else if (event->GetSymbol() == jgui::JKEY_CURSOR_UP) {
			if (current_row > 0) {
				current_row--;
				slide->SetPosition(bx+_x+current_col*(size+delta), by+_y+current_row*(size+delta));
			}
		} else if (event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
			if (current_row < max_rows-1) {
				current_row++;
				slide->SetPosition(bx+_x+current_col*(size+delta), by+_y+current_row*(size+delta));
			}
		} else if (event->GetSymbol() == jgui::JKEY_ENTER) {
			UpdateBoard(1);
		} else if (event->GetSymbol() == jgui::JKEY_RED || event->GetSymbol() == jgui::JKEY_F1) {
		} else if (event->GetSymbol() == jgui::JKEY_GREEN || event->GetSymbol() == jgui::JKEY_F2) {
			jgui::MessageDialog dialog("Ajuda", "O jogo termina quando todos os blocos que n\xe3o possuem minas s\xe3o revelados. Utilize as bandeirinhas para ajudar na identificac\xe3o das minas escondidas.", _x+300, _y+200);

			dialog.Show();
		} else if (event->GetSymbol() == jgui::JKEY_YELLOW || event->GetSymbol() == jgui::JKEY_F3) {
			UpdateBoard(2);
		} else if (event->GetSymbol() == jgui::JKEY_BLUE || event->GetSymbol() == jgui::JKEY_F4) {
			Repaint();
		}

		if (GetResult() != 0) {
			if (GetResult() == 1) {
				graphics->SetColor(0x00, 0x00, 0x00, 0xff);
				graphics->DrawString("Parabens", _width-190, 100);
				graphics->DrawImage("icons/flag2.png", _width-190, 180, 160, 140);

				Flip();

				int rx, ry;

				for (int i=0; i<max_rows*max_cols; i++) {
					ry = i/max_cols;
					rx = i%max_cols;

					if (board[i].type == BOMB_BLOCK) {
						DrawBlock(ry, rx, MARKED_BLOCK);
					}
				}
			} else if (GetResult() == 2) {
				graphics->SetColor(0x00, 0x00, 0x00, 0xff);
				graphics->DrawString("Perdeu", _width-160, 100);
				graphics->DrawImage("icons/flag3.png", _width-190, 180, 160, 140);

				Flip();

				int rx, ry;

				for (int i=0; i<max_rows*max_cols; i++) {
					ry = i/max_cols;
					rx = i%max_cols;

					if (board[i].type == BOMB_BLOCK) {
						DrawBlock(ry, rx, BOMB_BLOCK);
					}
				}
			}
		}
	} else {
		if (event->GetSymbol() == jgui::JKEY_BLUE || event->GetSymbol() == jgui::JKEY_F4) {
			Repaint();
		}
	}

}

int Mines::DrawBlock(int row, int col, block_type_t type, int value, bool update)
{
	int delta = 5;

	if (graphics == NULL) {
		return -1;
	}

	if (type == CLOSED_BLOCK) {
		graphics->SetColor(0x80, 0x80, 0x80, 0xff);
		graphics->FillRectangle(bx+col*(size+delta), by+row*(size+delta), size, size);
	} else if (type == BLANK_BLOCK) {
		graphics->SetColor(0xd0, 0xd0, 0xd0, 0xff);
		graphics->FillRectangle(bx+col*(size+delta), by+row*(size+delta), size, size);
	} else if (type == NUMBER_BLOCK) {
		graphics->SetColor(0xd0, 0xd0, 0xd0, 0xff);
		graphics->FillRectangle(bx+col*(size+delta), by+row*(size+delta), size, size);

		if (value > 0 && value < 8) {
			char c[2];

			c[0] = (char)(value + '0');
			c[1] = '\0';

			graphics->SetColor(0xff, 0x00, 0x00, 0xff);
			graphics->DrawStringJustified((const char *)&c, bx+col*(size+delta), by+row*(size+delta), size, size, jgui::CENTER_ALIGN);
		}
	} else if (type == BOMB_BLOCK) {
		graphics->SetColor(0xc0, 0xc0, 0xc0, 0xff);
		graphics->FillRectangle(bx+col*(size+delta), by+row*(size+delta), size, size);

		graphics->DrawImage(prefetch1, bx+col*(size+delta)+2, by+row*(size+delta)+2, prefetch1->GetWidth(), prefetch1->GetHeight());
	} else if (type == MARKED_BLOCK) {
		graphics->SetColor(0xc0, 0xc0, 0xc0, 0xff);
		graphics->FillRectangle(bx+col*(size+delta), by+row*(size+delta), size, size);
		graphics->DrawImage(prefetch2, bx+col*(size+delta)+2, by+row*(size+delta)+2, prefetch2->GetWidth(), prefetch2->GetHeight());
	}

	if (update == true) {
		graphics->Flip(bx+col*(size+delta), by+row*(size+delta), size, size);
	}

	return 0;
}

void Mines::InitializeFlags()
{
	current_row = 0;
	current_col = 0;
	hide_bombs = max_bombs;
	game_state = 0;
}

void Mines::SetupBoard()
{
	slide->SetPosition(bx+_x, by+_y);

	if (graphics == NULL) {
		return;
	}

	graphics->DrawImage("icons/flag2.png", _width-190, 180, 160, 140);
	graphics->DrawImage("icons/bomb2.png", _width-180, 400, 160, 140);

	InitializeFlags();

	for (int i=0; i<max_rows*max_cols; i++) {
		board[i].type = CLOSED_BLOCK;
		board[i].state = EMPTY_STATE;
	}

	// INFO:: preenche as celulas com bombas
	int bombs = max_bombs;
	int rx, 
		ry,
		index;

	srand((int)time(NULL));

	while (bombs > 0) {
		rx = rand() % max_cols;
		ry = rand() % max_rows;

		index = ry*max_cols+rx;
		if (board[index].type != BOMB_BLOCK) {
			board[index].type = BOMB_BLOCK;

			bombs--;
		}
	}

	// INFO:: identifica o numero de bombas ao redor de cada celula
	int count;

	for (int i=0; i<max_rows*max_cols; i++) {
		ry = i/max_cols;
		rx = i%max_cols;

		if (rx > 0 && rx < (max_cols-1) && ry > 0 && (ry < max_rows-1)) {
			// INFO:: preenche o interior do tabuleiro
			if (board[i].type != BOMB_BLOCK) {
				count = 0;

				count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
				count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
				count += (board[(ry-1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
				count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
				count += (board[(ry-1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
				count += (board[(ry+1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
				count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
				count += (board[(ry+1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;

				if (count > 0) {
					board[i].type = NUMBER_BLOCK;
					board[i].state = count;
				}
			}
		} else if (rx == 0) {
			// INFO:: preenche a lateral esquerda
			if (ry == 0) {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			} else if (ry == (max_rows-1)) {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			} else {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			}
		} else if (rx == (max_cols-1)) {
			// INFO:: preenche a lateral direita
			if (ry == 0) {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			} else if (ry == (max_rows-1)) {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			} else {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			}
		} else {
			// INFO:: preenche a parte superior e inferior do tabuleiro
			if (ry == 0) {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			} else if (ry == (max_rows-1)) {
				if (board[i].type != BOMB_BLOCK) {
					count = 0;

					count += (board[(ry+0)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry+0)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx-1)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+0)].type == BOMB_BLOCK)?1:0;
					count += (board[(ry-1)*max_cols+(rx+1)].type == BOMB_BLOCK)?1:0;

					if (count > 0) {
						board[i].type = NUMBER_BLOCK;
						board[i].state = count;
					}
				}
			}
		}
	}

	for (int i=0; i<max_cols; i++) {
		for (int j=0; j<max_rows; j++) {
			DrawBlock(j, i, CLOSED_BLOCK, 0, false);
		}
	}

	UpdateBoard(-1);

	Flip();
}

void Mines::Expose(int row, int col)
{
	if (col < 0 || col > (max_cols-1) || row < 0 || row > (max_rows-1)) {
		return;
	}

	int t = row*max_cols+col;

	if (board[t].state != UNAVAILABLE_STATE) {
		if (board[t].state == MARKED_STATE) {
			hide_bombs++;
		}

		if (board[t].type == NUMBER_BLOCK) {
			DrawBlock(row, col, NUMBER_BLOCK, board[t].state);
			board[t].state = UNAVAILABLE_STATE;
		} else if (board[t].type == CLOSED_BLOCK) {
			DrawBlock(row, col, BLANK_BLOCK);
			board[t].state = UNAVAILABLE_STATE;

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
}

int Mines::GetResult()
{
	// 0[nothing], 1[win], 2[lose]
	return game_state;
}

void Mines::UpdateBoard(int flag)
{
	int v = current_row*max_cols+current_col;

	if (board[v].state == UNAVAILABLE_STATE) {
		return;
	}

	if (flag == 1) {
		// INFO:: just open a block
		if (board[v].type == CLOSED_BLOCK || board[v].type == BLANK_BLOCK) {
			Expose(current_row, current_col);
		} else if (board[v].type == BOMB_BLOCK) {
			DrawBlock(current_row, current_col, BOMB_BLOCK);
			game_state = 2;
		} else if (board[v].type == NUMBER_BLOCK) {
			if (board[v].state == MARKED_STATE) {
				hide_bombs++;
			}

			DrawBlock(current_row, current_col, NUMBER_BLOCK, board[v].state);
			board[current_row*max_cols+current_col].state = UNAVAILABLE_STATE;
		}

		// TODO:: verificar vitoria
		int count = 0;
		for (int i=0; i<max_rows*max_cols; i++) {
			if (board[i].state != UNAVAILABLE_STATE) {
				count++;
			}
		}

		if (count == max_bombs) {
			game_state = 1;
		}
	} else if (flag == 2) {
		if (board[v].type == CLOSED_BLOCK || board[v].type == BOMB_BLOCK || board[v].type == NUMBER_BLOCK) {
			if (board[v].state == EMPTY_STATE || board[v].state > 0) {
				DrawBlock(current_row, current_col, MARKED_BLOCK);
				board[v].state = MARKED_STATE;

				hide_bombs--;
			} else if (board[v].state == MARKED_STATE) {
				DrawBlock(current_row, current_col, CLOSED_BLOCK);
				board[v].state = EMPTY_STATE;

				hide_bombs++;
			}
		}
	}

	// INFO:: show number of marked bombs
	char tmp[255];

	sprintf(tmp, "%02d", hide_bombs);

	label->SetText(tmp);
 
	// Flip();
}

}

int main()
{
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/comic.ttf", 0, 28));
	
	mines::Mines app(100, 100);

	app.Show();

	return 0;
}

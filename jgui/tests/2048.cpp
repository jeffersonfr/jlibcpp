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

/**
 * \author RosettaCode
 *
 * \address http://rosettacode.org/wiki/2048
 *
 */

#include "jframe.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <map>

#include <stdlib.h>
#include <time.h>

enum move_direction_t { 
	UP, 
	DOWN, 
	LEFT, 
	RIGHT 
};

class Tile {

	public:
		int val;
		bool blocked;

	public:
		Tile() {
			val = 0;
			blocked = false;
		}

};

class Game2048 : public jgui::Frame {
	
	private:
		std::map<int, jgui::Image *> _images;
		Tile board[4][4];
		int score;
		bool win;
		bool done;
		bool moved;

	public:
		Game2048():
			jgui::Frame("2048", 100, 100, 400, 400)
		{
			done = false;
			win = false;
			moved = true;
			score = 0;
			
			for (int i=2; i<=8192;) {
				std::ostringstream o;

				o << i;

				_images[i] = jgui::Image::CreateImage("images/2048/tux-n" + o.str() + ".png");

				i = i*2;
			}

			addTile(); 
		}

		virtual ~Game2048()
		{
			for (int i=0; i<(int)_images.size(); i++) {
				jgui::Image *image = _images[i];

				delete image;
			}

			_images.clear();
		}

	private:
		virtual void Paint(jgui::Graphics *g) 
		{
			jgui::Frame::Paint(g);

			jgui::jinsets_t t = GetInsets();

			if (done) {
				std::string s = "Game Over!";

				if (win) {
					s = "You've made it!";
				}

				g->DrawString(s, t.left, GetHeight()-t.bottom-16);
			} else {
				std::ostringstream o;

				o << "score:: " << score;

				g->DrawString(o.str(), t.left, GetHeight()-t.bottom-16);
			}

			int gap = 4;
			int bw = (GetWidth()-t.left-t.right-4*gap)/4;
			int bh = (GetHeight()-t.top-t.bottom-4*gap-16)/4;

			g->SetColor(jgui::Color::Black);

			for (int i=0; i<4; i++) {
				for (int j=0; j<4; j++) {
					g->DrawRectangle(t.left+i*(bw+gap), t.top+j*(bh+gap), bw, bh);
				}
			}

			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (board[x][y].val > 0) {
						std::ostringstream o;
						
						g->DrawImage(_images[board[x][y].val], x*(bw+gap)+t.left, y*(bh+gap)+t.top, bw, bh);
					}
				}
			}
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (jgui::Frame::KeyPressed(event) == true) {
				return true;
			}

			if (done) {
				Release();

				return true;
			}

			moved = false; 
			
			if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
				move(LEFT);
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
				move(RIGHT);
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_UP) {
				move(UP);
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_DOWN) {
				move(DOWN);
			}
			
			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					board[x][y].blocked = false;
				}
			}

			addTile();

			Repaint();

			return true;
		}

		void addTile()
		{
			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (!board[x][y].val) {
						int a, b;

						do { 
							a = rand() % 4; b = rand() % 4; 
						} while (board[a][b].val);

						int s = rand() % 100;

						if (s > 89) 
							board[a][b].val = 4;
						else 
							board[a][b].val = 2;

						if (canMove()) 
							return;
					}
				}
			}

			done = true;
		}

		bool canMove()
		{
			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (!board[x][y].val) {
						return true;
					}
				}
			}

			for (int y=0; y<4; y++) {
				for (int x=0; x<4; x++) {
					if (testAdd(x+1, y, board[x][y].val)) {
						return true;
					}

					if (testAdd(x-1, y, board[x][y].val)) {
						return true;
					}

					if (testAdd(x, y+1, board[x][y].val)) {
						return true;
					}

					if (testAdd(x, y-1, board[x][y].val)) {
						return true;
					}
				}
			}

			return false;
		}

		bool testAdd(int x, int y, int v)
		{
			if (x < 0 || x > 3 || y < 0 || y > 3) {
				return false;
			}

			return board[x][y].val == v;
		}

		void moveVert( int x, int y, int d )
		{
			if (board[x][y+d].val && board[x][y+d].val == board[x][y].val && !board[x][y].blocked && !board[x][y+d].blocked) {
				board[x][y].val = 0;
				board[x][y + d].val *= 2;
				score += board[x][y + d].val;
				board[x][y + d].blocked = true;
				moved = true;
			} else if (!board[x][y+d].val && board[x][y].val) {
				board[x][y + d].val = board[x][y].val;
				board[x][y].val = 0;
				moved = true;
			}
		
			if (d > 0) { 
				if (y+d < 3) {
					moveVert(x, y+d, 1); 
				}
			} else { 
				if (y+d > 0) {
					moveVert(x, y+d, -1); 
				}
			}
		}
		
		void moveHori( int x, int y, int d )
		{
			if (board[x+d][y].val && board[x+d][y].val == board[x][y].val && !board[x][y].blocked && !board[x+d][y].blocked) {
				board[x][y].val = 0;
				board[x + d][y].val *= 2;
				score += board[x + d][y].val;
				board[x + d][y].blocked = true;
				moved = true;
			} else if (!board[x+d][y].val && board[x][y].val) {
				board[x + d][y].val = board[x][y].val;
				board[x][y].val = 0;
				moved = true;
			}

			if (d > 0) { 
				if (x+d < 3) {
					moveHori( x + d, y,  1 ); 
				}
			} else { 
				if (x+d > 0) {
					moveHori(x+d, y, -1); 
				}
			}
		}

		void move(move_direction_t d)
		{
			switch(d) {
				case UP:
					for (int x=0; x<4; x++) {
						int y = 1;

						while (y < 4) { 
							if (board[x][y].val) {
								moveVert(x, y, -1);
							}

							y++;
						}
					}
					break;
				case DOWN:
					for (int x=0; x<4; x++) {
						int y = 2;

						while (y >= 0) { 
							if (board[x][y].val) {
								moveVert(x, y, 1);
							}

							y--;
						}
					}
					break;
				case LEFT:
					for (int y=0; y<4; y++) {
						int x = 1;

						while (x < 4) { 
							if (board[x][y].val) {
								moveHori(x, y, -1);
							}

							x++;
						}
					}
					break;
				case RIGHT:
					for (int y=0; y<4; y++) {
						int x = 2;

						while (x >= 0) { 
							if (board[x][y].val) {
								moveHori(x, y, 1);
							}

							x--;
						}
					}
			}
		}

};
int main( int argc, char* argv[] )
{
	srand(static_cast<int>(time(NULL)));

	Game2048 g; 
	
	g.Show(true);

	return 0;
}

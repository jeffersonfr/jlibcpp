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
#include "jpanel.h"
#include "jframe.h"
#include "jfont.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define ENABLE_GUI	1
#define DRAW_ANTS	0

#define FONT "./fonts/font.ttf"

struct path_t {
	int x;
	int y;
	int phr;
	int delta_phr;
	int index;
	int value;
};

#if ENABLE_GUI == 0
class Main{
#else 
class Main : public jgui::Frame{
#endif

	private:
		static const int K_INTERACTIONS = 1000;
		static const int K_ANTS = 80;
		static const double BETA_PHR = 0.35;

		static const double D_ACS = 0.85;
		static const int Q_ACS = 0xf00;

		static const int MAX_COLS = 80;
		static const int MAX_ROWS = 40;
		
		static const int BLOCK_WIDTH = 16;
		static const int BLOCK_HEIGHT = 16;
		
		static const int BLOCK_GAP = 4;
		
		static const int TRY_BETTER = 5;

		jgui::Font *fresult;
		path_t *board,
			   *solution[MAX_COLS],
			   *try_solutions;
		int best,
			max_value,
			min_value,
			min_global,
			max_global,
			average_global;

	public:
#if ENABLE_GUI == 0
		Main(int x, int y)
#else
		Main(int x, int y):
			jgui::Frame("Ant Colony", x, y, MAX_COLS*(BLOCK_WIDTH+BLOCK_GAP)+20, MAX_ROWS*(BLOCK_HEIGHT+BLOCK_GAP)+60+50)
#endif
		{
#if ENABLE_GUI == 1
			_insets.left = 10;
			_insets.right = 10;
			_insets.top = 60;

			fresult = new jgui::Font(FONT, 0, 24);

			SetFont(fresult);
#endif

			board = new path_t[MAX_COLS*MAX_ROWS];

			try_solutions = new path_t[K_ANTS*MAX_COLS];
			
			srand(time(NULL));

			max_value = 0;
			min_value = 9999;

			for (int i=0; i<MAX_COLS*MAX_ROWS; i++) {
				board[i].x = i%MAX_COLS;
				board[i].y = i/MAX_COLS;
				board[i].index = i;
				board[i].value = rand()%MAX_ROWS;
				board[i].phr = 0;
				board[i].delta_phr = 0;

				if (board[i].value > max_value) {
					max_value = board[i].value;
				}

				if (board[i].value < min_value) {
					min_value = board[i].value;
				}
			}

			best = 0;

			for (int i=0; i<MAX_COLS; i++) {
				solution[i] = &board[i];

				best += solution[i]->value;
			}
			
			for (int j=0; j<K_ANTS; j++) {
				for (int i=1; i<MAX_COLS; i++) {
					try_solutions[j*MAX_COLS+i] = *solution[i];
				}
			}

			path_t *current;
			int rmin = 9999,
				rmax = 0;

			min_global = 0;
			max_global = 0;

			for (int i=1; i<MAX_COLS; i++) {
				for (int j=0; j<MAX_ROWS; j++) {
					current = &board[j*MAX_COLS+i];

					if (current->value < rmin) {
						rmin = current->value;
					}

					if (current->value > rmax) {
						rmax = current->value;
					}

					min_global += rmin;
					max_global += rmax;
				}
			}

			average_global = (max_global-min_global)/2;
		}

		virtual ~Main()
		{
		}

		void Init()
		{
			path_t *temp[MAX_COLS];
			int r,
				count;

			// max interactions
#if ENABLE_GUI == 0
			for (int k=0; k<K_INTERACTIONS; k++) {
#else
			for (int k=0; k<K_INTERACTIONS && GetLastKeyCode()!=jgui::JKEY_ESCAPE; k++) {
#endif
				// k ants
				for (int j=0; j<K_ANTS; j++) {
					count = 0;

					for (int i=0; i<MAX_COLS; i++) {
						// follow path of pherom
						int l,
							phr_row[MAX_ROWS],
							count_probability = 0;

						for (l=0; l<MAX_ROWS; l++) {
							phr_row[l] = board[l*MAX_COLS+i].phr+1;
							count_probability += phr_row[l];
						}

						r = rand()%count_probability;
						count_probability = 0;

						for (l=0; l<MAX_ROWS; l++) {
							count_probability += phr_row[l];

							if (count_probability >= r) {
								break;
							}
						}

						r = l;

						// intensificando a qualidade das solucoes
						for (int t=0; t<TRY_BETTER; t++) {
							l = rand()%MAX_ROWS;

							if (board[r*MAX_COLS+i].value > board[l*MAX_COLS+i].value) {
								r = l;
							}
						}

						// fill solution
						temp[i] = &board[r*MAX_COLS+i];
						count += temp[i]->value+1;

						try_solutions[j*MAX_COLS+i] = *temp[i];
					}

					// leave delta phr
					int delta_phr = (int)(D_ACS*(Q_ACS/count));

					for (int i=0; i<MAX_COLS; i++) {
						temp[i]->delta_phr += delta_phr;
					}

					// compare the solutions
					if (count < best) {
						printf("Solution:: [%d => %d]\n", best, count);

						best = count;

						for (int l=0; l<MAX_COLS; l++) {
							solution[l] = temp[l];
						}
					}
				}
				
				// update pherony
				for (int i1=0; i1<MAX_COLS*MAX_ROWS; i1++) {
					if (board[i1].delta_phr != 0) {
						board[i1].phr = (int)(BETA_PHR*board[i1].phr + board[i1].delta_phr);

						if (board[i1].phr > 0xff) {
							board[i1].phr = 0xff;
						}

						board[i1].delta_phr = 0;
					}
				}
				// end update

#if ENABLE_GUI == 1
				Repaint();
#endif
			}

			// print best solution
			printf("Best Solution::\n");

			for (int i=0; i<MAX_COLS; i++) {
				printf("%d ", solution[i]->value);
			}

			printf("\n");
		}

#if ENABLE_GUI == 1
		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			g->SetFont(jgui::Graphics::GetDefaultFont());
			
			int dx = _insets.left,
				dy = _insets.top;
			char tmp[255];

			for (int i=0; i<MAX_COLS*MAX_ROWS; i++) {
				sprintf(tmp, "%d", board[i].value);

				g->SetColor(board[i].phr, 0x00, 0x00, 0xff);
				g->FillRectangle(dx+board[i].x*(BLOCK_WIDTH+BLOCK_GAP), dy+board[i].y*(BLOCK_HEIGHT+BLOCK_GAP), BLOCK_WIDTH, BLOCK_HEIGHT);
				g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
				g->DrawString(tmp, dx+board[i].x*(BLOCK_WIDTH+BLOCK_GAP), dy+board[i].y*(BLOCK_HEIGHT+BLOCK_GAP));
			}
			
#if DRAW_ANTS == 1
			g->SetColor(0x00, 0xf0, 0x00, 0xff);

			for (int j=0; j<K_ANTS; j++) {
				for (int i=1; i<MAX_COLS; i++) {
					g->DrawLine(dx+try_solutions[j*MAX_COLS+i-1].x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
						dy+try_solutions[j*MAX_COLS+i-1].y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2, 
						dx+try_solutions[j*MAX_COLS+i].x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
						dy+try_solutions[j*MAX_COLS+i].y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2);
				}
			}
#endif

			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

			for (int i=1; i<MAX_COLS; i++) {
				g->DrawLine(dx+solution[i-1]->x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
					dy+solution[i-1]->y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2, 
					dx+solution[i]->x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
					dy+solution[i]->y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2);
			}

			g->SetFont(fresult);
			g->SetColor(0x00, 0x00, 0x00, 0xff);

			sprintf(tmp, "Current Solution [%d]", best);

			g->DrawString(tmp, _insets.left, _height-50);
		}
#endif

};

int main(int argc, char **argv)
{
#if ENABLE_GUI == 1
	jgui::Graphics::SetDefaultFont(new jgui::Font(FONT, 0, 12));
#endif

	Main main(0, 0);

#if ENABLE_GUI == 1
	main.Show(false);
#endif

	main.Init();

	return 0;
}

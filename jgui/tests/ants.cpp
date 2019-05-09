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
#include "jgui/japplication.h"
#include "jgui/jbufferedimage.h"
#include "jgui/jwindow.h"

#include <iostream>
#include <mutex>

#define ENABLE_GUI	1
#define DRAW_ANTS		0

#define K_INTERACTIONS  1000
#define K_ANTS          80
#define BETA_PHR        0.35
#define D_ACS           0.85
#define Q_ACS           0xf00
#define MAX_COLS        90
#define MAX_ROWS        45
#define BLOCK_WIDTH     12
#define BLOCK_HEIGHT    12
#define BLOCK_GAP       2
#define TRY_BETTER      5

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
class Main : public jgui::Window {
#endif

	private:
#if ENABLE_GUI == 1
		jgui::Image 
      *foffscreen;
		jgui::Font 
      *fweights;
    std::mutex
      _mutex;
#endif
		path_t 
      *board,
		  *solution[MAX_COLS],
		  *try_solutions;
		int 
      best,
			max_value,
			min_value,
			min_global,
			max_global,
			average_global;

	public:
#if ENABLE_GUI == 0
		Main(int r)
#else
		Main(int r):
			jgui::Window(/* "Ant Colony", */ 0, 0, MAX_COLS*(BLOCK_WIDTH+BLOCK_GAP), MAX_ROWS*(BLOCK_HEIGHT+BLOCK_GAP))
#endif
		{
#if ENABLE_GUI == 1
			foffscreen = nullptr;

      fweights = new jgui::Font("default", jgui::JFA_NORMAL, 8);
#endif

			board = new path_t[MAX_COLS*MAX_ROWS];

			try_solutions = new path_t[K_ANTS*MAX_COLS];
			
			std::cout << "random seed:: " << r << std::endl;

			srand(time(nullptr));

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
      _mutex.unlock();
		}

		virtual void ShowApp()
		{
			path_t 
        *temp[MAX_COLS];
			int 
        r,
				count;

			// max interactions
			for (int k=0; k<K_INTERACTIONS; k++) {
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
						std::cout << "Solution:: [" << best << " => " << count << "]" << std::endl;

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
				if (IsHidden() == true) {
          break;
				}
					
        _mutex.lock();

        Repaint();
#endif
			}

			// print best solution
			std::cout << "Best Solution::" << std::endl;

			for (int i=0; i<MAX_COLS; i++) {
				std::cout << solution[i]->value << " ";
			}

			delete [] board;
      board = nullptr;

			delete [] try_solutions;
      try_solutions = nullptr;

#if ENABLE_GUI == 1
			delete fweights;
      fweights = nullptr;

      delete foffscreen;
      foffscreen = nullptr;
#endif
      
			std::cout << std::endl;

      jgui::Application::Quit();
		}

#if ENABLE_GUI == 1
		virtual void Paint(jgui::Graphics *g)
		{
      if (board == nullptr) {
        _mutex.unlock();

        return;
      }

			jgui::jsize_t
				size = GetSize();
			char tmp[255];

			if (foffscreen == nullptr) {
				foffscreen = new jgui::BufferedImage(jgui::JPF_RGB32, size.width, size.height);
			
				jgui::Graphics *goff = foffscreen->GetGraphics();

				sprintf(tmp, "Current Solution [%d]", best);

				jgui::Window::Paint(goff);

				goff->SetColor(0x00, 0x00, 0x00, 0xff);
				goff->DrawString(tmp, 0, size.height - 50);

				goff->SetFont(fweights);

				for (int i=0; i<MAX_COLS*MAX_ROWS; i++) {
					sprintf(tmp, "%d", board[i].value);

					goff->SetColor(0x00, 0x00, 0x00, 0xff);
					goff->FillRectangle(board[i].x*(BLOCK_WIDTH+BLOCK_GAP), board[i].y*(BLOCK_HEIGHT+BLOCK_GAP), BLOCK_WIDTH, BLOCK_HEIGHT);
					goff->SetColor(0xf0, 0xf0, 0xf0, 0xff);
					goff->DrawString(tmp, board[i].x*(BLOCK_WIDTH+BLOCK_GAP), board[i].y*(BLOCK_HEIGHT+BLOCK_GAP));
				}
			}

			g->DrawImage(foffscreen, 0, 0);

			for (int i=0; i<MAX_COLS*MAX_ROWS; i++) {
				sprintf(tmp, "%d", board[i].value);

				g->SetColor(board[i].phr, 0x00, 0x00, 0x80);
				g->FillRectangle(board[i].x*(BLOCK_WIDTH+BLOCK_GAP), board[i].y*(BLOCK_HEIGHT+BLOCK_GAP), BLOCK_WIDTH, BLOCK_HEIGHT);
			}

#if DRAW_ANTS == 1
			g->SetColor(0x00, 0xf0, 0x00);

			for (int j=0; j<K_ANTS; j++) {
				for (int i=1; i<MAX_COLS; i++) {
					g->DrawLine(try_solutions[j*MAX_COLS+i-1].x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
						try_solutions[j*MAX_COLS+i-1].y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2, 
						try_solutions[j*MAX_COLS+i].x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
						try_solutions[j*MAX_COLS+i].y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2);
				}
			}
#endif

			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

			for (int i=1; i<MAX_COLS; i++) {
				g->DrawLine(solution[i-1]->x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
					solution[i-1]->y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2, 
					solution[i]->x*(BLOCK_WIDTH+BLOCK_GAP)+BLOCK_WIDTH/2, 
					solution[i]->y*(BLOCK_HEIGHT+BLOCK_GAP)+BLOCK_HEIGHT/2);
			}

      _mutex.unlock();
		}
#endif

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app(time(nullptr));
	
#if ENABLE_GUI == 1
	app.SetTitle("Ants");
  app.Exec();
#endif

	jgui::Application::Loop();

	return 0;
}

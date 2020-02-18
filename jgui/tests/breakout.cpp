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
/*
 * \brief Original code by Brian Postma
 *
 */
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"

int	borderwidth = 5;
int	batwidth = 50;
int	ballsize = 5;
int	batheight = 5;
int	scoreheight = 20;
int	screendelay = 300;
int	brickwidth = 15;
int	brickheight = 8;
int	brickspace = 2;
int	numlines = 4;
int	startline = 32;
int	delay = 10;

double myrandom()
{
	return ((double)(random()%1000))/1000.0;
}

class Breakout : public jgui::Window {

	private:
		jgui::Image 
      *off;
		jgui::Graphics 
      *goff;
		int	player1score,
	  	ballx,
      bally,
      batpos,
      batdpos,
      balldx ,
      balldy,
      dxval,
      ballsleft,
      count,
      bricksperline;
		bool ingame,
		  showtitle,
		  *showbrick;

	public:
		Breakout():
			jgui::Window(/*"BreakOut", */ {720, 480})
		{
      SetFramesPerSecond(60);

			off = nullptr;
			goff = nullptr;

			player1score = 0;
			ballx = 0;
			bally = 0;
			batpos = 0;
			batdpos = 0;
			balldx = 0;
			balldy = 0;
			dxval = 0;
			ballsleft = 0;
			count = 0;
			ingame = false;
			showtitle = true;

			bricksperline = (720 - 2*borderwidth)/(brickwidth + brickspace);

			showbrick = new bool[bricksperline*numlines];

			GameInit();
		}

		virtual ~Breakout()
		{
      delete off;

      delete [] showbrick;
		}

		void GameInit()
		{
      jgui::jsize_t<int>
        size = GetSize();

			batpos = (size.width-batwidth)/2;
			ballx = (size.width-ballsize)/2;
			bally = (size.height-ballsize-scoreheight-2*borderwidth);
			player1score = 0;
			ballsleft = 3;
			dxval = 2;

			if (myrandom() < 0.5) {
				balldx = dxval;
			} else {
				balldx = -dxval;
			}

			balldy = -dxval;
			count = screendelay;
			batdpos = 0;

			InitBricks();
		}

		void InitBricks()
		{
			for (int i=0; i<numlines*bricksperline; i++)
				showbrick[i] = true;
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyPressed(event) == true) {
				return true;
			}

			if (ingame) {
				if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
					batdpos = -3;
				}

				if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
					batdpos = 3;
				}
			} else {
				if (event->GetSymbol() == jevent::JKS_SPACE) {
					ingame = true;

					GameInit();
				}
			}

			return true;
		}

		/*
		virtual bool KeyReleased(jgui::KeyEvent *event)
		{
			if (jgui::Window::KeyReleased(event) == true) {
				return true;
			}

			batdpos = 0;

			return true;
		}
		*/

		virtual void Paint(jgui::Graphics *g)
		{
      jgui::jsize_t<int>
        size = GetSize();

			if (goff == nullptr && size.width > 0 && size.height > 0) {
				off = new jgui::BufferedImage(jgui::JPF_RGB32, size);

				goff = off->GetGraphics();
			}

			if (goff == nullptr || off == nullptr) {
				return;
			}

			goff->SetColor(GetTheme().GetIntegerParam("bg"));
			goff->FillRectangle({0, 0, size.width, size.height});

			if (ingame) {
				PlayGame();
			} else {
				ShowIntroScreen();
			}

			g->DrawImage(off, jgui::jpoint_t<int>{0, 0});

      Repaint();
		}


		void PlayGame()
		{
			MoveBall();
			CheckBat();
			CheckBricks();
			DrawPlayField();
			DrawBricks();
			ShowScore();
		}

		void ShowIntroScreen()
		{
			MoveBall();
			CheckBat();
			CheckBricks();
			BatDummyMove();
			DrawPlayField();
			DrawBricks();
			ShowScore();
	
			goff->SetColor({0x60, 0x80, 0xff, 0xff});

			if (--count <= 0) { 
				count = screendelay; 
				showtitle = !showtitle;
			}
			
			jgui::Font *font = GetTheme().GetFont("widget");
			jgui::jsize_t<int> size = GetSize();

			if (font != nullptr) {
        size.width = size.width - font->GetStringWidth("Pressione SPACE para iniciar");

				goff->DrawString("Pressione SPACE para iniciar", jgui::jpoint_t<int>{size.width/2, size.height/2});
			}
		}

		void DrawBricks()
		{
			int i,
					j,
					colordelta = 255/(numlines-1);
			bool nobricks = true;

			for (j=0; j<numlines; j++) {
				for (i=0; i<bricksperline; i++) {
					if (showbrick[j*bricksperline+i]) {
						nobricks = false;

						goff->SetColor({0xff, j*colordelta, 0xff-j*colordelta, 0xff});
						goff->FillRectangle({borderwidth+i*(brickwidth+brickspace), startline+j*(brickheight+brickspace), brickwidth, brickheight});
					}
				}
			}

			if (nobricks) {
				InitBricks();
				
				if (ingame) {
					player1score += 100;
				}
			}
		}

		void DrawPlayField()
		{
      jgui::jsize_t<int>
        size = GetSize();

			goff->SetColor({0xff, 0xff, 0xff, 0xff});
			goff->FillRectangle({0, 0, size.width, borderwidth});
			goff->FillRectangle({0, 0, borderwidth, size.height});
			goff->FillRectangle({size.width-borderwidth, 0, borderwidth, size.height});
			goff->FillRectangle({0, size.height-borderwidth, size.width, borderwidth});
			goff->FillRectangle({batpos, size.height-2*borderwidth-scoreheight, batwidth,batheight}); // bat
			goff->FillRectangle({ballx, bally, ballsize, ballsize}); // ball
		}

		void ShowScore()
		{
			jgui::Font 
        *font = GetTheme().GetFont("widget");
      jgui::jsize_t<int>
        size = GetSize();

			if (font == nullptr) {
				return;
			}

			char tmp[255];

			goff->SetFont(font);

			goff->SetColor({0xff, 0xff, 0xff, 0xff});

			sprintf(tmp, "Score: %d", player1score); 
			goff->DrawString(tmp, jgui::jpoint_t<int>{borderwidth, borderwidth});
			sprintf(tmp, "Balls left: %d", ballsleft); 
			goff->DrawString(tmp, jgui::jpoint_t<int>{size.width-borderwidth-font->GetStringWidth(tmp), borderwidth});
		}

		void MoveBall()
		{
      jgui::jsize_t<int>
        size = GetSize();

			ballx += balldx;
			bally += balldy;

			if (bally <= borderwidth) {
				balldy = -balldy;
				bally = borderwidth;
			}

			if (bally >= (size.height-ballsize-scoreheight)) {
				if (ingame) {
					ballsleft--;
					
					if (ballsleft <= 0) {
						ingame = false;
					}
				}

				ballx = batpos+(batwidth-ballsize)/2;
				bally = startline+numlines*(brickheight+brickspace);
				balldy = dxval;
				balldx = 0;
			}

			if (ballx >= (size.width-borderwidth-ballsize)) {
				balldx = -balldx;
				ballx = size.width-borderwidth-ballsize;
			}

			if (ballx <= borderwidth) {
				balldx = -balldx;
				ballx = borderwidth;
			}
		}

		void BatDummyMove()
		{
			if (ballx < (batpos+2)) {
				batpos -= 3;
			} else if (ballx>(batpos+batwidth-3)) {
				batpos += 3;
			}
		}

		void CheckBat()
		{
      jgui::jsize_t<int>
        size = GetSize();

			batpos += batdpos;

			if (batpos < borderwidth) {
				batpos = borderwidth;
			} else if (batpos > (size.width-borderwidth-batwidth)) {
				batpos = (size.width-borderwidth-batwidth);
			}

			if (bally >= (size.height-scoreheight-2*borderwidth-ballsize) && bally < (size.height-scoreheight-2*borderwidth) && (ballx+ballsize) >= batpos && ballx <= (batpos+batwidth)) {
				bally = size.height-scoreheight-ballsize-borderwidth*2;
				balldy = -dxval;
				balldx = CheckBatBounce(balldx,ballx-batpos);
			}
		}

		int CheckBatBounce(int dy, int delta)
		{
			int i = -ballsize, 
					j = 0,
					sign = 0,
					stepsize = (ballsize+batwidth)/8;

			if (dy > 0) {
				sign = 1;
			} else {
				sign = -1;
			}

			while (i < batwidth && delta > i) {
				i += stepsize;
				j++;
			}

			switch(j) {
				case 0:
				case 1:
					return -4;
				case 2:
					return -3;
				case 7:
					return 3;
				case 3:
				case 6:
					return sign*2;
				case 4:
				case 5:
					return sign*1;
				default:
					return 4;
			}
		}

		void CheckBricks()
		{
			int i,
					j,
					x,
					y,
					xspeed = balldx;

			if (xspeed < 0) {
				xspeed=-xspeed;
			}

			int ydir = balldy;

			if (bally < (startline-ballsize) || bally > (startline+numlines*(brickspace+brickheight))) {
				return;
			}

			for (j=0; j<numlines; j++) {
				for (i=0; i<bricksperline; i++) {
					if (showbrick[j*bricksperline+i]) {
						y = startline+j*(brickspace+brickheight);
						x = borderwidth+i*(brickspace+brickwidth);
						
						if (bally >= (y-ballsize) && bally < (y+brickheight) && ballx >= (x-ballsize) && ballx < (x+brickwidth)) {
							showbrick[j*bricksperline+i] = false;

							if (ingame) {
								player1score+=(numlines-j);
							}

							// Where did we hit the brick
							if (ballx >= (x-ballsize) && ballx <= (x-ballsize+3)) { // leftside
								balldx = -xspeed;
							} else if (ballx <= (x+brickwidth-1) && ballx >= (x+brickwidth-4)) { // rightside
								balldx = xspeed;
							}

							balldy = -ydir;
						}
					}
				}
			}
		}

};

int main(int argc, char **argv) 
{
	jgui::Application::Init(argc, argv);

	Breakout app;

	app.SetTitle("Breakout");
	app.Exec();

	jgui::Application::Loop();

	return 0;
}

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

#include "jframe.h"
#include "jimage.h"
#include "jdate.h"
#include "jthread.h"

int	borderwidth = 5,
		batwidth = 20,
		ballsize = 5,
		batheight = 5,
		scoreheight = 20,
		screendelay = 300,
		brickwidth = 15,
		brickheight = 8,
		brickspace = 2,
		numlines = 4,
		startline = 32,
		delay = 10;

double myrandom()
{
	return ((double)(random()%1000))/1000.0;
}

class Breakout : public jgui::Frame, public jthread::Thread {

	private:
		jgui::Image *off;
		jgui::Graphics *goff;
		int		player1score,
					ballx,
					bally,
					batpos,
					batdpos,
					balldx, 
					balldy,
					dxval,
					ballsleft,
					count,
					bricksperline;
		bool	ingame,
					showtitle;
		bool	*showbrick;

	public:
		Breakout():
			jgui::Frame("BreakOut", 100, 100, 720, 480)
		{
			SetDefaultExitEnabled(false);
			SetMoveEnabled(true);

			off = NULL;
			goff = NULL;

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

			bricksperline = (_size.width-2*borderwidth)/(brickwidth+brickspace);

			showbrick = new bool[bricksperline*numlines];

			SetSize(bricksperline*(brickwidth+brickspace)+(2*borderwidth), _size.height);

			GameInit();

			Start();
		}

		void GameInit()
		{
			batpos = (_size.width-batwidth)/2;
			ballx = (_size.width-ballsize)/2;
			bally = (_size.height-ballsize-scoreheight-2*borderwidth);
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

		virtual void InputReceived(jgui::KeyEvent *event)
		{
			if (event->GetType() == jgui::JKT_RELEASED) {
				batdpos = 0;

				return;
			}

			if (ingame) {
				if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
					batdpos = -3;
				}

				if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
					batdpos = 3;
				}

				if (event->GetSymbol() == jgui::JKS_ESCAPE) {
					ingame = false;
				}

			} else {
				if (event->GetSymbol() == jgui::JKS_SPACE) {
					ingame = true;

					GameInit();
				}
			}

		}

		virtual void Paint(jgui::Graphics *g)
		{
			if (goff == NULL && _size.width > 0 && _size.height > 0) {
				off = jgui::Image::CreateImage(_size.width, _size.height);

				goff = off->GetGraphics();
			}

			if (goff == NULL || off == NULL) {
				return;
			}

			goff->SetColor(GetBackgroundColor());
			goff->FillRectangle(0, 0, _size.width, _size.height);

			if (ingame) {
				PlayGame();
			} else {
				ShowIntroScreen();
			}

			g->DrawImage(off, 0, 0);
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
	
			goff->SetColor(0x60, 0x80, 0xff, 0xff);

			if (--count <= 0) { 
				count = screendelay; 
				showtitle = !showtitle;
			}
			
			goff->DrawString("Pressione SPACE para iniciar", (GetWidth()-_font->GetStringWidth("Pressione SPACE para iniciar"))/2, GetHeight()/2);
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

						goff->SetColor(0xff, j*colordelta, 0xff-j*colordelta, 0xff);
						goff->FillRectangle(borderwidth+i*(brickwidth+brickspace), startline+j*(brickheight+brickspace), brickwidth, brickheight);
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
			goff->SetColor(0xff, 0xff, 0xff, 0xff);
			goff->FillRectangle(0, 0,_size.width, borderwidth);
			goff->FillRectangle(0, 0,borderwidth, _size.height);
			goff->FillRectangle(_size.width-borderwidth, 0, borderwidth, _size.height);
			goff->FillRectangle(0, _size.height-borderwidth,_size.width, borderwidth);
			goff->FillRectangle(batpos,_size.height-2*borderwidth-scoreheight, batwidth,batheight); // bat
			goff->FillRectangle(ballx, bally, ballsize, ballsize); // ball
		}

		void ShowScore()
		{
			char tmp[255];

			goff->SetFont(_font);

			goff->SetColor(0xff, 0xff, 0xff, 0xff);

			sprintf(tmp, "Score: %d", player1score); 
			goff->DrawString(tmp, borderwidth, borderwidth);
			sprintf(tmp, "Balls left: %d", ballsleft); 
			goff->DrawString(tmp, _size.width-borderwidth-_font->GetStringWidth(tmp), borderwidth);
		}

		void MoveBall()
		{
			ballx += balldx;
			bally += balldy;

			if (bally <= borderwidth) {
				balldy = -balldy;
				bally = borderwidth;
			}

			if (bally >= (_size.height-ballsize-scoreheight)) {
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

			if (ballx >= (_size.width-borderwidth-ballsize)) {
				balldx = -balldx;
				ballx = _size.width-borderwidth-ballsize;
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
			batpos += batdpos;

			if (batpos < borderwidth) {
				batpos = borderwidth;
			} else if (batpos > (_size.width-borderwidth-batwidth)) {
				batpos = (_size.width-borderwidth-batwidth);
			}

			if (bally >= (_size.height-scoreheight-2*borderwidth-ballsize) && bally < (_size.height-scoreheight-2*borderwidth) && (ballx+ballsize) >= batpos && ballx <= (batpos+batwidth)) {
				bally = _size.height-scoreheight-ballsize-borderwidth*2;
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

		virtual void Run()
		{
			uint64_t startTime = jcommon::Date::CurrentTimeMillis();
				
			while(true) {
				Repaint();
				
				// Delay depending on how far we are behind.
				startTime += delay;
				
				if ((int64_t)(startTime-jcommon::Date::CurrentTimeMillis()) > 0) {
					jthread::Thread::MSleep(startTime-jcommon::Date::CurrentTimeMillis());
				}
			}
		}

};

int main(int argc, char **argv) 
{
	Breakout breakout;

	breakout.Show();

	return 0;
}

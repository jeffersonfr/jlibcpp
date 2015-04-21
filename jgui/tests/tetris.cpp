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
#include "jframe.h"

#define X_BLOCKS	10
#define Y_BLOCKS	20

int items[] = {
	0,0,   -1,0,  0,-1, -1,-1,  // cube, normal
	0,0,   -1,0,  0,1,  -1,1,   // rotated 90 degrees
	0,0,   1,0,   0,1,   1,1,   // rotated 180 degrees
	0,0,   1,0,   0,-1,  1,-1,  // rotated 270 degrees

	0,0,   0,-1,  0,-2, 0,-3,   // straight line
	0,0,  -1,0,  -2,0, -3,0,
	0,0,   0,1,   0,2,  0,3, 
	0,0,   1,0,   2,0,  3,0,

	0,0,   1,0,   0,-1, -1,-1,  // stupid block
	0,0,   0,-1,  -1,0, -1, 1,
	0,0,  -1,0,   0,1,  1,1,
	0,0,   0,1,   1,0,  1,-1,

	0,0,   -1,0,  0,-1,  1,-1,  // stupid block 2
	0,0,   0,1,   -1,0,  -1,-1,
	0,0,   1,0,   0,1,   -1,1,
	0,0,   0,-1,  1,0,   1,1,

	0,0,   1,0,   -1,0,  0,-1,  // another one
	0,0,   0,1,   0,-1,  -1,0,
	0,0,   0,1,   -1,0,  1,0,
	0,0,   1,0,   0,-1,  0,1,

	0,0,   0,-1,  1,-1,  0,1,	// hook one
	0,0,   -1,0,  -1,-1, 1,0,
	0,0,   -1,1,  0,1,   0,-1,
	0,0,   -1,0,  1,0,   1,1,

	0,0,   0,1,  0,-1,  -1,-1,	// hook two
	0,0,   1,0, -1,0,   -1,1,
	0,0,   0,-1,  0,1,  1,1,
	0,0,   -1,0, 1,0,   1,-1
};

int checks[] = { 
	-1,1, 0,1, -1,1,  0,1,      // check cube, normal
	-1,2, 0,2, -1,2,  0,2,      // check, rotated 90 degrees
	0,2,  1,2,  0,2,  1,2,      // check, rotated 180 degrees
	0,1,  1,1,  0,1,  1,1,      // check, rotated 270 degrees

	0,1,  0,1,  0,1,  0,1,
	0,1, -1,1, -2,1, -3,1,
	0,4,  0,4,  0,4,  0,4,      // straight line
	0,1,  1,1,  2,1,  3,1,

	0,1,  -1,0, 1,1,  0,1,		// stupid block one
	0,1,  -1,2, 0,1, -1,2,
	0,2,  1,2,  -1,2, 0,2,
	0,2,  1,1,  0,2,  1,1,

	-1,1,  0,1, 1,0,  1,0,		// stupid block two
	-1,1,  0,2, 0,2,  -1,1,
	-1,2,  0,2, 1,1,  1,1,
	0,1,   1,2,  0,1,  1,2,

	-1,1,  0,1,  1,1, 1,1,		// block three
	-1,1,  0,2,  0,2, -1,1,
	-1,1,  0,2,  1,1, 1,1,
	0,2,   1,1,  0,2, 1,1,

	0,2,   1,0,  1,0, 0,2,		// hook one
	-1,1,  0,1,  1,1, 1,1,
	-1,2,  0,2,  0,2, -1,2,
	-1,1,  0,1,  1,2, 1,2,

	-1,0,  0,2,  0,2, -1,0,		// hook two
	-1,2,  0,1,  1,1,  1,1,
	0,2,   1,2,  1,2,  0,2,
	-1,1,  0,1,  1,1,  1,1
};

class Tetris : public jgui::Frame, public jthread::Thread{

	public:
		jgui::Graphics *goff;
		jgui::Image *ii;
		jgui::Font *largefont;
		jgui::Font *smallfont;
		uint32_t blocks[7];
		uint32_t textcolor1;
		uint32_t textcolor2;
		uint32_t barcolor;
		uint32_t background;
		bool ingame;
		bool showtitle;
		int screendata[X_BLOCKS][Y_BLOCKS];
		int	xblocks;
		int	yblocks;
		int	blocksize;
		int	width;
		int	height;
		int	maxcolors;
		int	barwidth;
		int	score;
		int	emptyline;
		int	objectx;
		int	objecty;
		int	objectdx;
		int	objecttype;
		int	objectcolor;
		int	objectrotation;
		int	objectrotationd;
		int	objectptr;
		int	checkptr;
		int	itemcount;
		int	itemrotlen;
		int	itemlen;
		int	count;
		int	maxcount;
		int	curcount;
		int	fast;
		int	screendelay;
		int	screencount;
		jgui::jregion_t d;
		bool _running;

	public:

		Tetris():
			jgui::Frame("Tetris", 0, 0, 1920, 1080)
		{
			_running = true;

			smallfont = jgui::Font::CreateFont("default", jgui::JFA_NORMAL, 20);
			largefont = jgui::Font::CreateFont("default", jgui::JFA_NORMAL, 28);

			goff = NULL;
			ii = NULL;
			largefont = NULL;
			smallfont = NULL;
			textcolor1 = 0xff6080ff;
			textcolor2 = 0xffffa040;
			barcolor = 0xff80ff40;
			background = 0xff000000;
			ingame = false;
			showtitle = true;
			xblocks = X_BLOCKS;
			yblocks = Y_BLOCKS;
			blocksize = 32;
			width = xblocks*blocksize;
			height = yblocks*blocksize;
			maxcolors = 6;
			barwidth = 8;
			score = 0;
			emptyline = 0;
			objectx = 0;
			objecty = 0;
			objectdx = 0;
			objecttype = 0;
			objectcolor = 0;
			objectrotation = 0;
			objectrotationd = 0;
			objectptr = 0;
			checkptr = 0;
			itemcount = 7;
			itemrotlen = 8;
			itemlen = itemrotlen*4;
			count = 0;
			maxcount = 5;
			curcount = 0;
			fast = false;
			screendelay = 40;
			screencount = 40;

			blocks[0] = background;
			blocks[1] = 0xffff0000;
			blocks[2] = 0xff00ff00;
			blocks[3] = 0xff0000ff;
			blocks[4] = 0xffffff00;
			blocks[5] = 0xffff00ff;
			blocks[6] = 0xff00ffff;

			// resize(width+2*barwidth,height+30);

			d.x = GetX();
			d.y = GetY();
			d.width = GetWidth();
			d.height = GetHeight();

			SetBackgroundColor((background>>0x10)&0xff, (background>>0x08)&0xff, (background>>0x00)&0xff, (background>>0x18)&0xff);
			SetUndecorated(true);
			SetSize(width+2*barwidth, height+2*barwidth);
		}

		virtual ~Tetris()
		{
			_running = false;

			WaitThread();
		}

		void init()
		{
			ingame=true;
			gameInit();

			Start();
		}

		void gameInit()
		{
			int i,j;

			for (i=0; i<xblocks; i++) {
				for (j=0; j<yblocks; j++) {
					screendata[i][j]=0;
				}
			}

			score=0;
			emptyline=-1;

			newObject();
			fast=false;
			curcount=maxcount;
		}


		void newObject()
		{
			int i,
				y;

			fast = false;

			objectx=xblocks/2-1;
			objectdx=0;
			objecty=0;
			objecttype=(int)(0.0+(itemcount * (rand()/(RAND_MAX+1.0))));
			if (objecttype>=itemcount)
				objecttype=itemcount-1;

			objectptr=(int)(objecttype*itemlen);
			checkptr=(int)(objecttype*itemlen);

			objectcolor=(int)(0.0+((maxcolors+1) * (rand()/(RAND_MAX+1.0))));
			if (objectcolor>maxcolors)
				objectcolor=maxcolors;
			objectrotation=0;
			count=maxcount;

			// check if game has ended
			for (i=0; i<4; i++)
			{
				y=items[objectptr+i*2+1];
				if (y>=0 && screendata[objectx+items[objectptr+i*2]][y]!=0)
				{
					ingame=false;
					showtitle=true;
				}
			}
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (jgui::Frame::KeyPressed(event) == true) {
				return true;
			}

			if (ingame) {
				if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
					objectdx = -1;
				} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
					objectdx = +1;
				} else if (event->GetSymbol() == jgui::JKS_CURSOR_UP) {
					objectrotationd = +1;
				} else if (event->GetSymbol() == jgui::JKS_CURSOR_DOWN) {
					fast = true;
				} else if (event->GetSymbol() == jgui::JKS_ESCAPE) {
					ingame = false;
				}
			} else {
				if (event->GetSymbol() == jgui::JKS_S) {
					ingame=true;
					gameInit();
				}
			}

			return true;
		}

		virtual bool KeyReleased(jgui::KeyEvent *event)
		{
			if (jgui::Frame::KeyReleased(event) == true) {
				return true;
			}

			fast = false;

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			if (goff == NULL && d.width>0 && d.height>0) {
				ii = jgui::Image::CreateImage(d.width, d.height);

				goff = ii->GetGraphics();
			}

			if (goff==NULL || ii==NULL) {
				return;
			}

			goff->SetColor(background);
			goff->FillRectangle(0, 0, d.width, d.height);

			if (ingame)
				playGame();
			else
				showIntro();
			
			showScore();

			g->DrawImage(ii, 0, 0);
		}

		void playGame()
		{
			bool bottomreached = false,
				 stillscrolling = false;

			if (emptyline<0) {
				bottomreached=drawObject();
			} else {
				scrollDown();
				stillscrolling=true;
			}
			drawBars();
			drawBlocks();
			if (stillscrolling || bottomreached) {
				checkFull();
			}
		}

		void showIntro()
		{
			std::string s;

			drawBars();
			drawBlocks();

			if (showtitle) {
				goff->SetFont(largefont);
				s = "Tetris";
				goff->SetColor(textcolor1);
				// goff->DrawString(s, barwidth+(width-largefont->GetStringWidth(s))/2-2, height/2-22);
				goff->SetColor(0xffffffff);
				// goff->DrawString(s,barwidth+(width-largefont->GetStringWidth(s)) / 2, height/2 - 20);

				goff->SetFont(smallfont);
				s = "(c)2001 by Brian Postma";
				goff->SetColor(textcolor2);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2 -1,height/2 + 9);
				goff->SetColor(0xffffffff);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2,height/2 + 10);

				s = "b.postma@hetnet.nl";
				goff->SetColor(textcolor2);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2 -1,height/2 + 29);
				goff->SetColor(0xffffffff);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2,height/2 + 30);
			} else {
				goff->SetFont(smallfont);
				s = "'S' to start game";
				goff->SetColor(textcolor1);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2 -1,height/2 - 31);
				goff->SetColor(0xffffffff);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2,height/2 - 30);

				s = "Use cursor left+right to move";
				goff->SetColor(textcolor2);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2 -1,height/2 - 11);
				goff->SetColor(0xffffffff);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2,height/2 - 10);

				s = "Use cursor up to rotate";
				goff->SetColor(textcolor2);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2 -1,height/2 + 9);
				goff->SetColor(0xffffffff);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2,height/2 + 10);

				s="Use cursor down to drop";
				goff->SetColor(textcolor2);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2 -1,height/2 + 29);
				goff->SetColor(0xffffffff);
				// goff->DrawString(s,barwidth+(width-smallfont->GetStringWidth(s))/2,height/2 + 30);
			}
			screencount--;
			if (screencount<=0) { 
				screencount=screendelay; showtitle=!showtitle; 
			}
		}

		void drawBars()
		{
			goff->SetColor(barcolor);
			goff->FillRectangle(0,0,barwidth,blocksize*yblocks);
			goff->FillRectangle(barwidth+blocksize*xblocks,0,barwidth,blocksize*yblocks);
			goff->FillRectangle(0,blocksize*yblocks,xblocks*blocksize+2*barwidth,barwidth);
		}

		bool drawObject()
		{
			int	i;
			bool	bottomreached=false;
			int		x,y,checkx,checky;

			// clear old
			for (i=0; i<4; i++) {
				x=objectx+items[objectptr+i*2+objectrotation*itemrotlen];
				y=objecty+items[objectptr+i*2+objectrotation*itemrotlen+1];
				checkx=objectx+checks[objectptr+i*2+objectrotation*itemrotlen];
				checky=objecty+checks[objectptr+i*2+objectrotation*itemrotlen+1];
				if (y>=0)
					screendata[x][y]=0;
				if (screendata[checkx][checky]!=0)
					bottomreached=true;

			}

			if (!bottomreached) {
				count--;
				if (count<=0 || fast) {
					objecty++;
					count=curcount;
				}
				checkRotation();
				objectdx=0;
				objectrotationd=0;
			}

			// draw new
			for (i=0; i<4; i++) {
				x=objectx+items[objectptr+i*2+objectrotation*itemrotlen];
				y=objecty+items[objectptr+i*2+objectrotation*itemrotlen+1];

				if (y>=0)
					screendata[x][y]=objectcolor;
				if (y>=(yblocks-1))
					bottomreached=true;
			}
			if (bottomreached) {
				score++;
				newObject();
			}
			return bottomreached;
		}

		void checkRotation()
		{
			int dummyx,
				dummyrot,
				x,
				y,
				i;
			bool cando = true;

			dummyrot=(objectrotation+objectrotationd)%4;
			dummyx=objectx+objectdx;

			// make sure the part doesn't rotate of the playscreen
			for (i=0; i<4; i++) {
				x=dummyx+items[objectptr+i*2+dummyrot*itemrotlen];
				if (x>=xblocks)
					dummyx-=(x-xblocks+1);
				else if (x<0)
					dummyx-=x;
			}

			for (i=0; (i<4 && cando); i++) {
				x=dummyx+items[objectptr+i*2+dummyrot*itemrotlen];
				y=objecty+items[objectptr+i*2+dummyrot*itemrotlen+1];
				if (y>=0)
					cando=cando&&(screendata[x][y]==0);
				if (y>=yblocks || x<0 || x>=xblocks)
					cando=false;
			}

			if (cando) {
				objectrotation = dummyrot;
				objectx = dummyx;
			}
		}

		void drawBlocks()
		{
			int x,
				y;

			for (x=0; x<xblocks; x++) {
				for (y=0; y<yblocks; y++) {
					goff->SetColor(blocks[screendata[x][y]]);
					goff->DrawRectangle(x*blocksize+barwidth,y*blocksize,blocksize-1,blocksize-1);
					goff->FillRectangle(x*blocksize+barwidth+3,y*blocksize+3,blocksize-6,blocksize-6);
				}
			}
		}

		void checkFull()
		{
			int	x,y;
			bool found=false;

			for (y=yblocks-1; ( y>=0 && !found); y--) {
				found=true;
				for (x=0; x<xblocks; x++) {
					if (screendata[x][y]==0)
						found=false;
				}
				if (found) {
					score+=10;

					// increase speed when you've got a lot of points
					if (score>800)
						curcount=1;
					else if (score>600)
						curcount=2;
					else if (score>400)
						curcount=3;
					else if (score>200)
						curcount=4;

					for (x=0; x<xblocks; x++)
					{
						screendata[x][y]=0;
					}
					emptyline=y;
				}
			}
		}

		void scrollDown()
		{
			int x,y;

			for (y=emptyline; y>0; y--) {
				for (x=0; x<xblocks; x++) {
					screendata[x][y]=screendata[x][y-1];
				}
			}
			for (x=0; x<xblocks; x++) {
				screendata[x][0]=0;
			}
			emptyline=-1;
		}


		void showScore()
		{
			std::string s;

			goff->SetFont(smallfont);
			goff->SetColor(0xffffffff);

			s="Score: " + score;

			goff->DrawString(s,width/2-40,(yblocks+1)*blocksize+10);
		}

		void Run()
		{
			// uint64_t starttime;

			while(_running) {
				// starttime = (jcommon::Date::CurrentTimeMillis()+10LL);

				Repaint();

				usleep(50000);
				// usleep(1000*((long long)jcommon::Date::CurrentTimeMillis()-(long long)starttime));
			}
		}
};

int main()
{
	Tetris t;

	t.init();
	t.Show(true);

	return 0;
}


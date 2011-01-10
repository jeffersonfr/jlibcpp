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

struct dimension_t {
	int x;
	int y;
	int w;
	int h;
};

int level1data[] = { 
	0x13,0x1a,0x1a,0x16,0x09,0x0c,0x13,0x1a,0x16,0x09,0x0c,0x13,0x1a,0x1a,0x16,
	0x25,0x0b,0x0e,0x11,0x1a,0x1a,0x14,0x0f,0x11,0x1a,0x1a,0x14,0x0b,0x0e,0x25,
	0x11,0x1a,0x1a,0x14,0x0b,0x06,0x11,0x1a,0x14,0x03,0x0e,0x11,0x1a,0x1a,0x14,
	0x15,0x03,0x06,0x19,0x16,0x05,0x15,0x07,0x15,0x05,0x13,0x1c,0x03,0x06,0x15,
	0x15,0x09,0x08,0x0e,0x15,0x0d,0x15,0x05,0x15,0x0d,0x15,0x0b,0x08,0x0c,0x15,
	0x19,0x12,0x1a,0x12,0x18,0x12,0x1c,0x05,0x19,0x12,0x18,0x12,0x1a,0x12,0x1c,
	0x06,0x15,0x07,0x15,0x07,0x15,0x0b,0x08,0x0e,0x15,0x07,0x15,0x07,0x15,0x03,
	0x04,0x15,0x05,0x15,0x05,0x15,0x0b,0x0a,0x0e,0x15,0x05,0x15,0x05,0x15,0x01,
	0x0c,0x15,0x0d,0x15,0x0d,0x15,0x0b,0x0a,0x0e,0x15,0x0d,0x15,0x0d,0x15,0x09,
	0x13,0x18,0x1a,0x18,0x1a,0x10,0x1a,0x12,0x1a,0x10,0x1a,0x18,0x1a,0x18,0x16,
	0x15,0x03,0x02,0x02,0x06,0x15,0x0f,0x15,0x0f,0x15,0x03,0x02,0x02,0x06,0x15,
	0x15,0x09,0x08,0x08,0x04,0x11,0x1a,0x08,0x1a,0x14,0x01,0x08,0x08,0x0c,0x15,
	0x11,0x1a,0x1a,0x16,0x0d,0x15,0x0b,0x02,0x0e,0x15,0x0d,0x13,0x1a,0x1a,0x14,
	0x25,0x0b,0x0e,0x11,0x1a,0x18,0x16,0x0d,0x13,0x18,0x1a,0x14,0x0b,0x0e,0x25,
	0x19,0x1a,0x1a,0x1c,0x03,0x06,0x19,0x1a,0x1c,0x03,0x06,0x19,0x1a,0x1a,0x1c
};

int	validspeeds[] = { 
	1, 2, 3, 4, 6, 8 
};

class PacMan : public jgui::Frame, public jthread::Thread, public jgui::FrameInputListener {

	private:

	jgui::Graphics *goff;
	jgui::OffScreenImage *ii;
	
	dimension_t d;
	
	uint32_t dotcolor,
			 mazecolor,
			 bigdotcolor,
			 dbigdotcolor;
	int	screendelay;
	int blocksize;
	int nrofblocks;
	int scrsize;
	int	animdelay;
	int pacanimdelay;
	int ghostanimcount;
	int pacmananimcount;
	int maxghosts;
	int	pacmanspeed;
	int animcount;
	int pacanimcount;
	int pacanimdir;
	int count;
	int ghostanimpos;
	int pacmananimpos;
	int nrofghosts;
	int pacsleft, 
		score,
		deathcounter,
		*dx, 
		*dy,
		*ghostx, 
		*ghosty, 
		*ghostdx, 
		*ghostdy, 
		*ghostspeed;
	int pacmanx, 
		pacmany, 
		pacmandx, 
		pacmandy,
		reqdx, 
		reqdy, 
		viewdx, 
		viewdy,
		scaredcount, 
		scaredtime,
		maxscaredtime,
		minscaredtime,
		maxspeed,
		currentspeed,
		*screendata;
	bool ingame,
		 showtitle,
		 scared,
		 dying,
		 flag;
	jgui::OffScreenImage *ghost_bmp,
		*pacman_bmp;

	public:

	PacMan():
		jgui::Frame("PacMan", 0, 0, 1, 1)
	{
		SetUndecorated(true);

		goff = NULL;
		ii = NULL;
	
		d.x = 0;
		d.y = 0;
		d.w = 0;
		d.h = 0;

		ingame = false;
		showtitle = true;
		scared = false;
		dying = false;

		mazecolor = 0xff000000;
		dotcolor = 0xffc0c000;
		bigdotcolor = 192;
		dbigdotcolor = -2;

		screendelay = 120;
		blocksize = 48;
		nrofblocks = 15;
		scrsize = nrofblocks*blocksize;
		animdelay = 8;
		pacanimdelay = 2;
		ghostanimcount = 8;
		pacmananimcount = 3;
		maxghosts = 12;
		pacmanspeed = 4;

		animcount = animdelay;
		pacanimcount = pacanimdelay;
		pacanimdir = 1;
		count = screendelay;
		ghostanimpos = 0;
		pacmananimpos = 0;
		nrofghosts = 6;
		pacsleft = 0;
		score = 0;
		deathcounter = 0;
		dx = NULL;
		dy = NULL;
		ghostx = NULL;
		ghosty = NULL;
		ghostdx = NULL;
		ghostdy = NULL;
		ghostspeed = NULL;

		ghost_bmp = NULL;
		pacman_bmp = NULL;

		pacmanx = 0;
		pacmany = 0;
		pacmandx = 0;
		pacmandy = 0;

		reqdx = 0;
		reqdy = 0;
		viewdx = 0;
		viewdy = 0;

		scaredcount = 0;
		scaredtime = 0;
		maxscaredtime = 240;
		minscaredtime = 40;
	
		maxspeed = 0;
		currentspeed = 0;
		
		screendata = NULL;
	
		flag = true;

		SetDefaultExitEnabled(false);
		SetSize(scrsize+4, scrsize+nrofblocks+blocksize+10);

		Frame::RegisterInputListener(this);
	}
	
	virtual ~PacMan()
	{
		flag = false;
	}

	void Init()
	{
		jgui::Graphics *g;
		
		GetImages();

		screendata = new int[nrofblocks*nrofblocks];

		g = GetGraphics();
		
		d.x = GetX();
		d.y = GetY();
		d.w = GetWidth();
		d.h = GetHeight();

		SetBackgroundColor(0x00, 0x00, 0x00, 0xff);
		
		ghostx = new int[maxghosts];
		ghostdx = new int[maxghosts];
		ghosty = new int[maxghosts];
		ghostdy = new int[maxghosts];
		ghostspeed = new int[maxghosts];
		dx = new int[4];
		dy = new int[4];
		
		GameInit();
	}

	void GameInit()
	{
		pacsleft=3;
		score=0;
		scaredtime=maxscaredtime;
		LevelInit();
		nrofghosts=6;
		currentspeed=3;
		scaredtime=maxscaredtime;
	}

	void LevelInit()
	{
		int  i;

		for (i=0; i<nrofblocks*nrofblocks; i++)
			screendata[i]=level1data[i];

		LevelContinue();
	}

	void LevelContinue()
	{
		int i,
			dx=1,
			random;

		for (i=0; i<nrofghosts; i++) {
			ghosty[i]=7*blocksize;
			ghostx[i]=7*blocksize;
			ghostdy[i]=0;
			ghostdx[i]=dx;
			dx=-dx;
			random = (int)(0+((currentspeed+1)*(rand()/(RAND_MAX+1.0))));

			if (random>currentspeed) {
				random=currentspeed;
			}

			ghostspeed[i]=validspeeds[random];
		}

		screendata[7*nrofblocks+6]=10;
		screendata[7*nrofblocks+8]=10;
		pacmanx=7*blocksize;
		pacmany=11*blocksize;
		pacmandx=0;
		pacmandy=0;
		reqdx=0;
		reqdy=0;
		viewdx=-1;
		viewdy=0;
		dying=false;
		scared=false;
	}

	jgui::OffScreenImage * GetImage(std::string image, int w, int h)
	{
		jgui::OffScreenImage *o = new jgui::OffScreenImage(w, h);

		o->GetGraphics()->DrawImage(image, 0, 0, w, h);

		return o;
	}

	void GetImages()
	{
		int w = blocksize,
			h = blocksize;

		ghost_bmp = GetImage("icons/ghosts.png", 8*w, 6*h);
		pacman_bmp = GetImage("icons/pacman.png", 3*w, 5*h);
	}

	virtual void InputChanged(jgui::KeyEvent *event)
	{
		if (event->GetType() == jgui::JKEY_PRESSED) {
			if (event->GetSymbol() == jgui::JKEY_ESCAPE) {
				flag = false;

				jgui::Frame::Release();

				return;
			}

			if (ingame) {
				if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
					reqdx=-1;
					reqdy=0;
				} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT) {
					reqdx=1;
					reqdy=0;
				} else if (event->GetSymbol() == jgui::JKEY_CURSOR_UP) {
					reqdx=0;
					reqdy=-1;
				} else if (event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
					reqdx=0;
					reqdy=1;
				} else if (event->GetSymbol() == jgui::JKEY_ESCAPE) {
					ingame=false;
				}
			} else {
				if (event->GetSymbol() == jgui::JKEY_s) {
					ingame=true;
					GameInit();
				}
			}
		} else if (event->GetType() == jgui::JKEY_RELEASED) {
			if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT || 
				event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT || 
				event->GetSymbol() == jgui::JKEY_CURSOR_UP || 
				event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
				/*
				reqdx=0;
				reqdy=0;
				*/
			}
		}
	}

	virtual void Paint(jgui::Graphics *g)
	{
		Frame::Paint(g);

		std::string s;

		if (goff == NULL && d.w>0 && d.h>0) {
			ii = new jgui::OffScreenImage(d.w, d.h);

			goff = ii->GetGraphics();
		}

		if (goff==NULL || ii==NULL) {
			return;
		}

		goff->SetColor(0x00, 0x00, 0x00, 0xff);
		goff->FillRectangle(0, 0, d.w, d.h);

		DrawMaze();
		DrawScore();
		DoAnim();

		// CHANGE:: demo ou game
		ingame = true;

		if (ingame) {
			PlayGame();
		} else {
			PlayDemo();
		}

		g->DrawImage(ii, 2, 2);
	}

	void DoAnim()
	{
		animcount--;
		if (animcount<=0)
		{
			animcount=animdelay;
			ghostanimpos++;
			if (ghostanimpos>=ghostanimcount)
				ghostanimpos=0;
		}
		pacanimcount--;
		if (pacanimcount<=0)
		{
			pacanimcount=pacanimdelay;
			pacmananimpos=pacmananimpos+pacanimdir;
			if (pacmananimpos==(pacmananimcount-1) ||  pacmananimpos==0)
				pacanimdir=-pacanimdir;
		}
	}

	void PlayGame()
	{
		if (dying) {
			Death();
		} else {
			CheckScared();
			MovePacMan();
			DrawPacMan();
			MoveGhosts();
			CheckMaze();
		} 
	}

	void PlayDemo()
	{
		CheckScared();
		MoveGhosts();
		// ShowIntroScreen();
	}

	void Death()
	{
		int k,
			x_index,
			y_index;

		deathcounter--;
		k=(deathcounter&15)/4;

		if (k == 0) {
			x_index = 2;
			y_index = 4;
		} else if (k == 1) {
			x_index = 2;
			y_index = 1;
		} else if (k == 2) {
			x_index = 2;
			y_index = 2;
		} else {
			x_index = 2;
			y_index = 3;
		}

		x_index = x_index*blocksize;
		y_index = y_index*blocksize;

		goff->DrawImage(pacman_bmp, x_index, y_index, blocksize, blocksize, pacmanx+1, pacmany+1, blocksize, blocksize);

		if (deathcounter==0) {
			pacsleft--;
			if (pacsleft==0)
				ingame=false;
			LevelContinue();
		}
	}

	void MoveGhosts()
	{
		int i,
			pos,
			count;

		if (ghostx == NULL || ghosty == NULL) {
			return;
		}

		for (i=0; i<nrofghosts; i++) {
			if (ghostx[i]%blocksize==0 && ghosty[i]%blocksize==0) {
				pos=ghostx[i]/blocksize+nrofblocks*(int)(ghosty[i]/blocksize);

				count=0;
				if ((screendata[pos]&1)==0 && ghostdx[i]!=1) {
					dx[count]=-1;
					dy[count]=0;
					count++;
				}
				if ((screendata[pos]&2)==0 && ghostdy[i]!=1) {
					dx[count]=0;
					dy[count]=-1;
					count++;
				}
				if ((screendata[pos]&4)==0 && ghostdx[i]!=-1) {
					dx[count]=1;
					dy[count]=0;
					count++;
				}
				if ((screendata[pos]&8)==0 && ghostdy[i]!=-1) {
					dx[count]=0;
					dy[count]=1;
					count++;
				}
				if (count==0) {
					if ((screendata[pos]&15)==15) {
						ghostdx[i]=0;
						ghostdy[i]=0;
					} else {
						ghostdx[i]=-ghostdx[i];
						ghostdy[i]=-ghostdy[i];
					}
				} else {
					count = (int)(0+(count*(rand()/(RAND_MAX+1.0))));

					if (count>3) {
						count=3;
					}

					ghostdx[i]=dx[count];
					ghostdy[i]=dy[count];
				}
			}

			ghostx[i]=ghostx[i]+(ghostdx[i]*ghostspeed[i]);
			ghosty[i]=ghosty[i]+(ghostdy[i]*ghostspeed[i]);
			
			DrawGhost(ghostx[i]+1,ghosty[i]+1);

			if (pacmanx>(ghostx[i]-12) && pacmanx<(ghostx[i]+12) &&	pacmany>(ghosty[i]-12) && pacmany<(ghosty[i]+12) && ingame) {
				if (scared) {
					score+=10;
					ghostx[i]=7*blocksize;
					ghosty[i]=7*blocksize;
				} else {
					dying=true;
					deathcounter=64;
				}
			}
		}
	}

	void DrawGhost(int x, int y)
	{
		int size,
			x_index,
			y_index;

		size = ghost_bmp->GetWidth()/8;

		x_index = ghostanimpos;

		if (scared == false) {
			// CHANGE:: 0, 1, 2, 3
			y_index = 0;
		} else {
			y_index = 4;
		}

		x_index = x_index*blocksize;
		y_index = y_index*blocksize;

		goff->DrawImage(ghost_bmp, x_index, y_index, blocksize, blocksize, x, y, blocksize, blocksize);
	}

	void MovePacMan()
	{
		int pos,
			ch;

		if (reqdx==-pacmandx && reqdy==-pacmandy) {
			pacmandx=reqdx;
			pacmandy=reqdy;
			viewdx=pacmandx;
			viewdy=pacmandy;
		}
		if (pacmanx%blocksize==0 && pacmany%blocksize==0) {
			pos=pacmanx/blocksize+nrofblocks*(int)(pacmany/blocksize);
			ch=screendata[pos];
			if ((ch&16)!=0) {
				screendata[pos]=(int)(ch&15);
				score++;
			}
			if ((ch&32)!=0) {
				scared=true;
				scaredcount=scaredtime;
				screendata[pos]=(int)(ch&15);
				score+=5;
			}

			if (reqdx!=0 || reqdy!=0) {
				if (!( (reqdx==-1 && reqdy==0 && (ch&1)!=0) || (reqdx==1 && reqdy==0 && (ch&4)!=0) ||
							(reqdx==0 && reqdy==-1 && (ch&2)!=0) ||	(reqdx==0 && reqdy==1 && (ch&8)!=0))) {
					pacmandx=reqdx;
					pacmandy=reqdy;
					viewdx=pacmandx;
					viewdy=pacmandy;
				}
			}

			// Check for standstill
			if ( (pacmandx==-1 && pacmandy==0 && (ch&1)!=0) || (pacmandx==1 && pacmandy==0 && (ch&4)!=0) ||
					(pacmandx==0 && pacmandy==-1 && (ch&2)!=0) || (pacmandx==0 && pacmandy==1 && (ch&8)!=0)) {
				pacmandx=0;
				pacmandy=0;
			}
		}
		pacmanx=pacmanx+pacmanspeed*pacmandx;
		pacmany=pacmany+pacmanspeed*pacmandy;
	}

	void DrawPacMan()
	{
		int x_index,
			y_index;

		x_index = pacmananimpos;

		if (viewdx == -1) {
			y_index = 3;
		} else if (viewdx == 1) {
			y_index = 1;
		} else if (viewdy == -1) {
			y_index = 4;
		} else {
			y_index = 2;
		}

		x_index = x_index*blocksize;
		y_index = y_index*blocksize;

		goff->DrawImage(pacman_bmp, x_index, y_index, blocksize, blocksize, pacmanx, pacmany, blocksize, blocksize);
	}

	void DrawMaze()
	{
		int i=0;
		int x,y;

		bigdotcolor=bigdotcolor+dbigdotcolor;

		if (bigdotcolor<=64 || bigdotcolor>=192)
			dbigdotcolor=-dbigdotcolor;

		for (y=0; y<scrsize; y+=blocksize) {
			for (x=0; x<scrsize; x+=blocksize) {
				goff->SetColor((mazecolor>>0x10)&0xff, (mazecolor>>0x08)&0xff, (mazecolor>>0x00)&0xff, (mazecolor>>0x18)&0xff);
				if ((screendata[i]&1)!=0) {
					goff->DrawLine(x, y, x, y+blocksize-1);
				}
				if ((screendata[i]&2)!=0) {
					goff->DrawLine(x, y, x+blocksize-1, y);
				}
				if ((screendata[i]&4)!=0) {
					goff->DrawLine(x+blocksize-1,y,x+blocksize-1,y+blocksize-1);
				}
				if ((screendata[i]&8)!=0) {
					goff->DrawLine(x,y+blocksize-1,x+blocksize-1,y+blocksize-1);
				}
				if ((screendata[i]&16)!=0) {
					goff->SetColor((dotcolor>>0x10)&0xff, (dotcolor>>0x08)&0xff, (dotcolor>>0x00)&0xff, (dotcolor>>0x18)&0xff);
					goff->FillRectangle(x+(blocksize-2)/2, y+(blocksize-2)/2, 2, 2);
				}
				if ((screendata[i]&32)!=0) {
					goff->SetColor(224, 224-bigdotcolor, bigdotcolor, 0xff);
					goff->FillRectangle(x+(blocksize-8)/2, y+(blocksize-8)/2, 8, 8);
				}
				i++;
			}
		}
	}

	void DrawScore()
	{
		char tmp[255];
		int i;

		sprintf(tmp, "Score: %d", score);
		
		goff->SetFont(GetFont());
		goff->SetColor(96, 128, 255, 0xff);
		goff->DrawString(tmp, GetWidth()-180, scrsize+16);

		for (i=0; i<pacsleft; i++) {
			goff->DrawImage(pacman_bmp, 1*blocksize, 1*blocksize, blocksize, blocksize, i*(blocksize+4)+8, scrsize+10, blocksize, blocksize);
		}
	}

	void CheckScared()
	{
		scaredcount--;
		if (scaredcount<=0)
			scared=false;

		if (scared && scaredcount>=30)
			mazecolor = 0xffc020ff;
		else
			mazecolor = 0xff20c0ff;

		if (scared) {
			screendata[7*nrofblocks+6]=11;
			screendata[7*nrofblocks+8]=14;
		} else {
			screendata[7*nrofblocks+6]=10;
			screendata[7*nrofblocks+8]=10;
		}
	}

	void CheckMaze()
	{
		int i=0;
		bool finished=true;

		while (i<nrofblocks*nrofblocks && finished) {
			if ((screendata[i]&48)!=0)
				finished=false;
			i++;
		}
		if (finished) {
			score+=50;
			DrawScore();
			usleep(2000*1000);
			if (nrofghosts < maxghosts)
				nrofghosts++; 
			if (currentspeed<maxspeed)
				currentspeed++;
			scaredtime=scaredtime-20;
			if (scaredtime<minscaredtime)
				scaredtime=minscaredtime;
			LevelInit();
		}
	}

	virtual void Run()
	{
		uint64_t starttime;

		while(flag) {
			starttime = (jcommon::Date::CurrentTimeMillis()+10LL);

			Repaint();

			usleep(5000);
			// INFO:: as vezes trava:: usleep(1000*((long long)jcommon::Date::CurrentTimeMillis()-(long long)starttime));
		}
	}
};

int main()
{
	PacMan p;

	p.Show(false);
	p.Init();
	p.Run();

	return 0;
}



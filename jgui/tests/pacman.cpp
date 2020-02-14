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
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"

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

class PacMan : public jgui::Window {

	private:
  	jgui::Graphics 
      *goff;
	  jgui::Image 
      *ii;
  	jgui::Theme 
      _theme;
	  jgui::jrect_t<int>
      d;
	uint32_t 
		dotcolor,
		mazecolor,
		bigdotcolor,
		dbigdotcolor;
	int	
    screendelay,
    blocksize,
    nrofblocks,
    scrsize,
    animdelay,
    pacanimdelay,
    ghostanimcount,
    pacmananimcount,
    maxghosts,
    pacmanspeed,
    animcount,
    pacanimcount,
    pacanimdir,
    count,
    ghostanimpos,
    pacmananimpos,
    nrofghosts,
    pacsleft, 
		score,
		deathcounter,
		*dx, 
		*dy,
		*ghostx, 
		*ghosty, 
		*ghostdx, 
		*ghostdy, 
		*ghostspeed,
		pacmanx, 
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
	bool 
		ingame,
		showtitle,
		scared,
		dying,
		flag;
	jgui::Image 
		*ghost_bmp,
		*pacman_bmp;

	public:

	PacMan():
		jgui::Window(jgui::jsize_t<int>{720, 720})
	{
		goff = nullptr;
		ii = nullptr;
	
		d = {0, 0, 0, 0};

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
		dx = nullptr;
		dy = nullptr;
		ghostx = nullptr;
		ghosty = nullptr;
		ghostdx = nullptr;
		ghostdy = nullptr;
		ghostspeed = nullptr;

		ghost_bmp = nullptr;
		pacman_bmp = nullptr;

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
		
		screendata = nullptr;
	
		flag = true;

		_theme.SetIntegerParam("window.bg", 0xff000000);

		// SetDefaultExitEnabled(false);
		SetSize(scrsize + 4, scrsize + nrofblocks + blocksize + 10);
		
    Init();
	}
	
	virtual ~PacMan()
	{
		flag = false;

    delete [] screendata;
		
    delete ghost_bmp;
		delete pacman_bmp;

    delete ii;
		
    delete [] ghostx;
		delete [] ghostdx;
		delete [] ghosty;
		delete [] ghostdy;
		delete [] ghostspeed;
		delete [] dx;
		delete [] dy;
	}

	void Init()
	{
		jgui::jrect_t
			region = GetVisibleBounds();

		GetImages();

		screendata = new int[nrofblocks*nrofblocks];

		d = region;

		SetTheme(&_theme);
		
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

		for (i=0; i<nrofblocks*nrofblocks; i++) {
			screendata[i]=level1data[i];
		}

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

	jgui::Image * GetImage(std::string file, int w, int h)
	{
		jgui::Image *image = new jgui::BufferedImage(file);
		jgui::Image *scale = image->Scale({w, h});

		delete image;
		
		return scale;
	}

	void GetImages()
	{
		int w = blocksize,
			h = blocksize;

		ghost_bmp = GetImage("images/ghosts.png", 8*w, 6*h);
		pacman_bmp = GetImage("images/pacman.png", 3*w, 5*h);
	}

	virtual bool KeyPressed(jevent::KeyEvent *event)
	{
		if (jgui::Window::KeyPressed(event) == true) {
			return true;
		}

		if (event->GetSymbol() == jevent::JKS_ESCAPE) {
			flag = false;

			// TODO:: SetVisible(jgui::Frame::Release();

			return false;
		}

		if (ingame) {
			if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				reqdx=-1;
				reqdy=0;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				reqdx=1;
				reqdy=0;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				reqdx=0;
				reqdy=-1;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				reqdx=0;
				reqdy=1;
			} else if (event->GetSymbol() == jevent::JKS_ESCAPE) {
				ingame=false;
			}
		} else {
			if (event->GetSymbol() == jevent::JKS_s) {
				ingame=true;
				GameInit();
			}
		}

		return true;
	}

	virtual bool KeyReleased(jevent::KeyEvent *event)
	{
		if (jgui::Window::KeyReleased(event) == true) {
			return true;
		}

		if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT || 
			event->GetSymbol() == jevent::JKS_CURSOR_RIGHT || 
			event->GetSymbol() == jevent::JKS_CURSOR_UP || 
			event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
			// reqdx=0;
			// reqdy=0;
		}

		return true;
	}

  void Framerate(int fps)
  {
    static auto begin = std::chrono::steady_clock::now();
    static int index = 0;

    std::chrono::time_point<std::chrono::steady_clock> timestamp = begin + std::chrono::milliseconds(index++*(1000/fps));
    std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
    std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - current);

    if (diff.count() < 0) {
      return;
    }

    std::this_thread::sleep_for(diff);
  }

	virtual void Paint(jgui::Graphics *g)
	{
		Window::Paint(g);

		std::string s;

		if (goff == nullptr && d.size.width > 0 && d.size.height > 0) {
			ii = new jgui::BufferedImage(jgui::JPF_RGB32, d.size);

			goff = ii->GetGraphics();
		}

		if (goff == nullptr || ii == nullptr) {
			return;
		}

		goff->SetColor({0x00, 0x00, 0x00, 0xff});
		goff->FillRectangle({0, 0, d.size.width, d.size.height});

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

		g->DrawImage(ii, jgui::jpoint_t<int>{2, 2});

    Framerate(60);

    Repaint();
	}

	void DoAnim()
	{
		animcount--;

		if (animcount<=0) {
			animcount=animdelay;
			ghostanimpos++;
			
			if (ghostanimpos>=ghostanimcount) {
				ghostanimpos=0;
			}
		}
		
		pacanimcount--;

		if (pacanimcount<=0) {
			pacanimcount=pacanimdelay;
			pacmananimpos=pacmananimpos+pacanimdir;
			
			if (pacmananimpos==(pacmananimcount-1) ||  pacmananimpos==0) {
				pacanimdir=-pacanimdir;
			}
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

		goff->DrawImage(pacman_bmp, {x_index, y_index, blocksize, blocksize}, {pacmanx+1, pacmany+1, blocksize, blocksize});

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

		if (ghostx == nullptr || ghosty == nullptr) {
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
		int // size,
			x_index,
			y_index;

		// size = ghost_bmp->GetWidth()/8;

		x_index = ghostanimpos;

		if (scared == false) {
			// CHANGE:: 0, 1, 2, 3
			y_index = 0;
		} else {
			y_index = 4;
		}

		x_index = x_index*blocksize;
		y_index = y_index*blocksize;

		goff->DrawImage(ghost_bmp, {x_index, y_index, blocksize, blocksize}, {x, y, blocksize, blocksize});
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

		goff->DrawImage(pacman_bmp, {x_index, y_index, blocksize, blocksize}, {pacmanx, pacmany, blocksize, blocksize});
	}

	void DrawMaze()
	{
		int i=0;
		int x,y;

		bigdotcolor=bigdotcolor+dbigdotcolor;

		if (bigdotcolor<=64 || bigdotcolor>=192) {
			dbigdotcolor=-dbigdotcolor;
		}

		for (y=0; y<scrsize; y+=blocksize) {
			for (x=0; x<scrsize; x+=blocksize) {
				goff->SetColor(mazecolor);

				if ((screendata[i]&1)!=0) {
					goff->DrawLine({x, y}, {x, y+blocksize-1});
				}

				if ((screendata[i]&2)!=0) {
					goff->DrawLine({x, y}, {x+blocksize-1, y});
				}

				if ((screendata[i]&4)!=0) {
					goff->DrawLine({x+blocksize-1, y}, {x+blocksize-1, y+blocksize-1});
				}

				if ((screendata[i]&8)!=0) {
					goff->DrawLine({x, y+blocksize-1}, {x+blocksize-1, y+blocksize-1});
				}

				if ((screendata[i]&16)!=0) {
					goff->SetColor(dotcolor);
					goff->FillRectangle({x+(blocksize-2)/2, y+(blocksize-2)/2, 2, 2});
				}

				if ((screendata[i]&32)!=0) {
					goff->SetColor({224, (int)(224-bigdotcolor), (int)(bigdotcolor), 0xff});
					goff->FillRectangle({x+(blocksize-8)/2, y+(blocksize-8)/2, 8, 8});
				}

				i++;
			}
		}
	}

	void DrawScore()
	{
		jgui::jsize_t
			size = GetSize();
		int 
			i;
		char 
			tmp[255];

		sprintf(tmp, "Score: %d", score);
		
		goff->SetFont(GetTheme()->GetFont("window"));
		goff->SetColor({96, 128, 255, 0xff});
		goff->DrawString(tmp, jgui::jpoint_t<int>{size.width - 180, scrsize + 16});

		for (i=0; i<pacsleft; i++) {
			goff->DrawImage(pacman_bmp, {1*blocksize, 1*blocksize, blocksize, blocksize}, {i*(blocksize+4)+8, scrsize+10, blocksize, blocksize});
		}
	}

	void CheckScared()
	{
		scaredcount--;

		if (scaredcount<=0) {
			scared=false;
		}

		if (scared && scaredcount>=30) {
			mazecolor = 0xffc020ff;
		} else {
			mazecolor = 0xff20c0ff;
		}

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
			if ((screendata[i]&48)!=0) {
				finished = false;
			}

			i++;
		}

		if (finished) {
			score+=50;
			DrawScore();
        
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			
			if (nrofghosts < maxghosts) {
				nrofghosts++;
			}
			
			if (currentspeed<maxspeed) {
				currentspeed++;
			}
			
			scaredtime=scaredtime-20;
			
			if (scaredtime<minscaredtime) {
				scaredtime=minscaredtime;
			}
			
			LevelInit();
		}
	}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	PacMan app;

	app.SetTitle("Pacman");
	app.Exec();
	
  jgui::Application::Loop();

	return 0;
}



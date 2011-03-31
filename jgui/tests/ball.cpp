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
 * \brief Original code from balldrop applet.
 *
 */

#include "jframe.h"
#include "jimage.h"
#include "jmath.h"
#include "jdate.h"

double slowdown = .75,
			 getout = 1;

int numrows = 8,
		numcolumns = 20,
		numballs = 10,
		delay = 10,
		topspace = 30,
		sidespace = 20; 

double myrandom()
{
	return ((double)(random()%1000))/1000.0;
}

class Ball {

	public:
		double r,
					 x,
					 y,
					 vx,
					 vy,
					 vlen;
		int w,
				h;
		jgui::Image *_image;

	public:
		Ball(double tx, double ty) 
		{
			_image = NULL;

			r = 0;
			x = tx;
			y = ty;
			w = -1;
			h = -1;
			vx = 0;
			vy = 0;
			vlen = 0;
		}

		void Move() 
		{
			x += vx;
			y += vy;
		}

		void Accelerate(double ax, double ay) 
		{
			vx += ax;
			vy += ay;
			vlen = sqrt(vx*vx+vy*vy);
		}

		bool Boink(int pinx,int piny,double pinr) 
		{
			double dx,
						 dy,
						 dist;

			dx = x-(double)pinx;
			dy = y-(double)piny;  

			dist = sqrt(dx*dx+dy*dy);

			if (dist <= pinr+r) {
				double tvx,tvy,proj;

				tvx = vx/vlen;
				tvy = vy/vlen;

				dx /= dist;
				dy /= dist;

				proj=tvx*dx+tvy*dy;

				tvx -= 2.0*proj*dx;
				tvy -= 2.0*proj*dy;

				vx = tvx*vlen;
				vy = tvy*vlen;

				do {
					x += vx;
					y += vy;

					dx=x-(double)pinx;
					dy=y-(double)piny;  

					dist = sqrt(dx*dx+dy*dy);
				} while(dist <= pinr+r);


				vx *= slowdown;
				vy *= slowdown;

				vlen = sqrt(vx*vx+vy*vy);

				return true;
			}

			return false;
		}

		void SetImage(jgui::Image *image) 
		{
			_image = image;
			w = _image->GetWidth();
			h = _image->GetHeight();
			r = w/2.0;
		}

		void Draw(jgui::Graphics *g) 
		{
			if (_image != NULL)
				g->DrawImage(_image, (int)x-w/2, (int)y-h/2);
		}

		void Update(jgui::Image *img, int x, int y, int width, int height) 
		{
			if (img == _image) {
				w = width;
				h = height;
				r = w/2;
			}
		}

};

class BallDrop : public jgui::Frame, public jthread::Thread {

	private:
		std::vector<Ball *> _balls;
		jgui::jsize_t offDimension,
			backDimension;
		jgui::Image *offImage,
			*backImage,
			*pin,
			*ball;
		double pinr;
		int ballw,
				ballh,
				pinw,
				pinh,
				numracks,
				*rackheight,
				*rackdel;

	public:
		BallDrop():
			jgui::Frame("Ball Drop", 0, 0, 720, 480)
		{
			int w = 16,
					h = 16;

			ball = jgui::Image::CreateImage(w/2, h/2);
			pin = jgui::Image::CreateImage(w, h);

			ball->GetGraphics()->DrawImage("images/smallball.png", 0, 0, ball->GetWidth(), ball->GetHeight());
			pin->GetGraphics()->DrawImage("images/smallpin.png", 0, 0, pin->GetWidth(), pin->GetHeight());

			pinw = pin->GetWidth();
			pinh = pin->GetHeight();
			pinr = pinw/2.0;
			ballw = ball->GetWidth();
			ballh = ball->GetHeight();

			for(int i=0; i<numballs; ++i) {
				Ball *ball = new Ball(_size.width/2,0);

				ball->Accelerate((myrandom()-.5)*.5, 0);

				_balls.push_back(ball);
			}

			Start();
		}

		virtual void Run() 
		{
			uint64_t startTime = jcommon::Date::CurrentTimeMillis();

			for (std::vector<Ball *>::iterator i=_balls.begin(); i!=_balls.end(); i++) {
				(*i)->SetImage(ball);
			}

			// Allocate rack information
			numracks = _size.width/ballw;
			rackheight = new int[numracks];
			rackdel = new int[numracks];
			for(int i=0; i<numracks; i++) {
				rackheight[i] = 0;
				rackdel[i] = 0;
			}

			while (true) {
				UpdateBalls();

				Repaint();

				// Delay depending on how far we are behind.
				startTime += delay;

				if ((int64_t)(startTime-jcommon::Date::CurrentTimeMillis()) > 0) {
					jthread::Thread::MSleep(startTime-jcommon::Date::CurrentTimeMillis());
				}
			}
		}

		void Paint(jgui::Graphics *g) 
		{
			// Create the background image if necessary
			if ((backImage == NULL) || (_size.width != backDimension.width) || (_size.height != backDimension.height) ) {
				jgui::Graphics *backGraphics;

				backDimension = _size;
				backImage = jgui::Image::CreateImage(_size.width, _size.height);
				backGraphics = backImage->GetGraphics();

				// Erase the previous image.
				backGraphics->SetColor(GetBackgroundColor());
				backGraphics->FillRectangle(0, 0, _size.width, _size.height);
				backGraphics->SetColor(0x00, 0x00, 0x00, 0xff);

				//Paint the frame into the image.
				PaintBackground(backGraphics);
			} else {
				UpdateRack(backImage->GetGraphics());
			}

			if ( (offImage == NULL) || (_size.width != offDimension.width) || (_size.height != offDimension.height) ) {
				offDimension = _size;
				offImage = jgui::Image::CreateImage(_size.width, _size.height);
			}

			offImage->GetGraphics()->DrawImage(backImage, 0, 0);

			for (std::vector<Ball *>::iterator i=_balls.begin(); i!=_balls.end(); i++) {
				(*i)->Draw(offImage->GetGraphics());
			}

			g->DrawImage(offImage, 0, 0);
		}

		void PaintBackground(jgui::Graphics *g) {
			double scale,
						 x,
						 y,
						 oy;
			int i,
					j,
					pinx,
					piny;

			// Draw the pins

			for(i=0;i<numrows;++i) {  // Rows
				piny=i*(_size.height-2*topspace)/(numrows*2)+topspace;
				scale=(double)(_size.width-2*sidespace)/(double)numcolumns;

				for(j=0;j<numcolumns;++j) { // Columns
					pinx=(int)((j+(i%2)/2.0)*scale)+sidespace;
					g->DrawImage(pin, pinx-pinw/2, piny-pinh/2);
				}
			}

			//  Draw density curve

			scale=(double)_size.width/numracks;
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			oy=0;
			for(i=0;i<=numracks;++i) {
				x=numracks/2;        
				y=_size.height*(1.0-.5*exp(-(double)(i-x)*(i-x)/(2*81)));
				if(i>0) {
					g->DrawLine((int)((i-1)*scale),(int)oy,(int)(i*scale),(int)y);
				}
				oy=y;
			}

			// Draw 'racks'

			for(i=0;i<numracks;++i) {
				for(j=0;j<rackheight[i];++j) {
					pinx=i*ballw;
					piny=_size.height-(j+1)*ballh;
					g->DrawImage(ball, pinx, piny);            
				}
			}
		}

		void UpdateRack(jgui::Graphics *g) 
		{
			int i,
					pinx,
					piny;

			for(i=0;i<numracks;++i) {
				while(rackdel[i]>0) {
					++rackheight[i];
					pinx=i*ballw;
					piny=_size.height-rackheight[i]*ballh;
					g->DrawImage(ball, pinx, piny);            
					--rackdel[i];
				}
			}
		}

		void UpdateBalls() 
		{
			double scale;
			int j,
					k,
					pinx,
					piny,
					rack,
					bottomy,
					fr,
					lr,
					fc,
					lc;

			bottomy=_size.height;

			for (std::vector<Ball *>::iterator i=_balls.begin(); i!=_balls.end(); i++) {
				Ball *aBall = (*i);

				aBall->Move();

				if(aBall->y > bottomy-aBall->r) {
					if(aBall->vlen > .25) {
						aBall->vx = 0;
						aBall->vy = -aBall->vy*.25;
						aBall->y = bottomy-aBall->r;
					} else {
						rack=(int)(aBall->x/ballw);

						if(rack>=0 && rack<numracks) {
							++rackdel[rack];
						}

						aBall->x=_size.width/2;
						aBall->y=aBall->r;

						aBall->vx=(myrandom()-.5)/3;
						aBall->vy=0;
					}
				} else {
					k=(int)((aBall->y-topspace)*2*numrows/(_size.height-2*topspace));

					fr= k<=0 ? 0 : k-1;
					lr= k>=numrows-1 ? numrows-1 : k+1;

					for (int i=fr; i<=lr; i++) {  // Rows
						piny=i*(_size.height-2*topspace)/(2*numrows)+topspace;

						scale=(double)(_size.width-2*sidespace)/(double)numcolumns;

						k=(int)((aBall->x-sidespace)/scale-(i%2)/2.0);

						fc= k<=0 ? 0 : k-1;
						lc= k>=numcolumns-1 ? numcolumns-1 : k+1;

						for (j=fc;j<=lc;++j) { // Columns
							pinx=(int)((j+(i%2)/2.0)*scale+sidespace);

							aBall->Boink(pinx,piny,pinr);
						}
					}
				}


				aBall->Accelerate(0,.075);
			}
		}

};

int main(int argc, char **argv)
{
	BallDrop ball;

	ball.Show();

	return 0;
}

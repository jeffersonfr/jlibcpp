#include "jframe.h"
#include "jimage.h"
#include "jthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vector>
#include <string>
#include <exception>

class Neuron {
	public:
	double x,y;
	double wx,wy;
	int update,choose;
	
	Neuron(double x,double y)
	{
		this->x = x;
		this->y = y;
		
		this->wx = random()%100;
		this->wy = random()%100;
		
		update = 0;
		choose = 0;
	}
	
	double dist(Neuron *c)
	{
		double dx = x - c->x;
		double dy = y - c->y;
		
		return sqrt(dx*dx + dy*dy);
		
	}
	
	double wdist(Neuron *c)
	{
		double dx = wx - c->wx;
		double dy = wy - c->wy;
		
		return sqrt(dx*dx + dy*dy);
	}

};

double	COUNTRY = 1.00;

class som : public jgui::Frame, public jthread::Thread {

	public:
		enum {
			bkC = 0xff000090,
			bk2C = 0xff000050,
			lnC = 0xffff0000,
			ln2C = 0xffcccc00,
			fgC = 0xffffffff
		};
	
	public:
		jgui::Image *offscreen;
		int	W,
				H,
				NGEONEURON,
				imagewidth,
				imageheight,
				counter;
		double px,
					 py,
					 **r,
					 theta, 
					 phi, 
					 momentum;
		bool please_stop;
		Neuron **gn;

	public:
		som():
			jgui::Frame("SOM", 10, 10, 400, 400),
			jthread::Thread() 
		{
			kohonenInit();

			SetResizeEnabled(true);
		}

		virtual ~som()
		{
			please_stop = true;

			WaitThread();
		}

		void kohonenInit()
		{
			please_stop = false;
			theta = 0.5;
			phi   = 0.5;
			momentum = 0.999;

			// tamanho da grade
			W = 10;
			H = 10;

			NGEONEURON = W * H;

			gn = new Neuron*[NGEONEURON];
			for(int x = 0; x<W; x++)
				for(int y = 0; y<H; y++){
					gn[x*W+y] = new Neuron((double)x/(double)(W-1), (double)y/(double)(H-1));
				}

			r = new double*[NGEONEURON];

			for (int i=0; i<NGEONEURON; i++) {
				r[i] = new double[NGEONEURON];
			}

			makeR(theta);        

			counter = 0;
		}

		void makeR(double th)
		{
			for(int i=0; i<NGEONEURON; i++){
				r[i][i]= 1.0;
				for(int j=i+1; j<NGEONEURON; j++){
					r[i][j] = exp( -1.0 * ( gn[i]->dist(gn[j])*gn[i]->dist(gn[j]) )/(2.0*th*th));
					r[j][i] = r[i][j];
				}
			}
		}

		void Run() {
			double x1,
						 x2,
						 mindist;
			int j;

			while(!please_stop) {
				counter++;

				// CHOSE A RANDOM PATTERN
				x1 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));
				x2 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));

				while ((x1*x1+x2*x2) > 1.0){
					x1 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));
					x2 = (COUNTRY*((double)rand()/((double)(RAND_MAX))));
				}

				px = x1;
				py = x2;

				// SEARCH FOR MINIMAL
				mindist = 100000.0;
				j = -1;
				for(int i=0; i<NGEONEURON;i++){
					double d = (x1 - gn[i]->wx)*(x1 - gn[i]->wx) + (x2 - gn[i]->wy)*(x2 - gn[i]->wy);
					if(d < mindist){
						mindist = d;
						j = i;
					}
				}

				gn[j]->update++;

				// UPDATE WEIGHTS
				for(int i=0; i<NGEONEURON;i++){
					gn[i]->wx += (phi * r[i][j] * (x1 - gn[i]->wx));
					gn[i]->wy += (phi * r[i][j] * (x2 - gn[i]->wy));
				}

				// DECREASE LEARNING PARAMETERS
				phi *= momentum;
				theta *= momentum;

				// RE-COMPUTE r MATRIX
				makeR(theta);

				Repaint();
			}
		}

		int toXReal(double val)
		{
			int w = _size.width;

			return (int)(val *((double)w-50.0) / COUNTRY +25.0);
		}

		int toYReal(double val)
		{
			int h = _size.height;

			return (int)(val *((double)h-50.0) / COUNTRY +25.0);
		}

		void paintLeft(jgui::Graphics *g) 
		{
			int w = _size.width, 
					h = _size.height;

			// CLEAR ALL
			g->SetColor(bkC);
			g->FillRectangle(0, 0, w, h);
			// DRAW GRID
			g->SetColor(bk2C);
			for(double i=0; i<=COUNTRY; i+=(COUNTRY/20.0)){
				g->DrawLine(toXReal(0.0),toYReal(i),toXReal(COUNTRY),toYReal(i));
				g->DrawLine(toXReal(i),toYReal(0.0),toXReal(i),toYReal(COUNTRY));
			}

			//DRAW PATH
			g->SetColor(lnC);

			for(int x=0; x<(W-1); x++)
				for(int y=0; y<(H-1); y++){
					g->DrawCircle( toXReal(gn[x*W+y]->wx)-2, toYReal(gn[x*W+y]->wy)-2, 4);
					g->DrawLine( toXReal(gn[x*W+y]->wx),toYReal(gn[x*W+y]->wy),toXReal(gn[(x+1)*W+y]->wx),toYReal(gn[(x+1)*W+y]->wy));
					g->DrawLine( toXReal(gn[x*W+y]->wx),toYReal(gn[x*W+y]->wy),toXReal(gn[x*W+y+1]->wx),toYReal(gn[x*W+y+1]->wy));
				}
			for(int x=0; x<(W-1); x++){
				g->DrawCircle( toXReal(gn[x*W+H-1]->wx)-2, toYReal(gn[x*W+H-1]->wy)-2,4);
				g->DrawLine( toXReal(gn[x*W+H-1]->wx),toYReal(gn[x*W+H-1]->wy),toXReal(gn[(x+1)*W+H-1]->wx),toYReal(gn[(x+1)*W+H-1]->wy));
			}
			for(int y=0; y<(H-1); y++){
				g->DrawCircle( toXReal(gn[(W-1)*W+y]->wx)-2, toYReal(gn[(W-1)*W+y]->wy)-2,4);
				g->DrawLine( toXReal(gn[(W-1)*W+y]->wx),toYReal(gn[(W-1)*W+y]->wy),toXReal(gn[(W-1)*W+y+1]->wx),toYReal(gn[(W-1)*W+y+1]->wy));
			}            
			g->DrawCircle( toXReal(gn[(W-1)*W+H-1]->wx)-2, toYReal(gn[(W-1)*W+H-1]->wy)-2,4);

			g->SetColor(fgC);
			g->DrawLine( toXReal(0.0), toYReal(0.0),toXReal(COUNTRY),toYReal(0.0));
			g->DrawLine( toXReal(0.0), toYReal(0.0),toXReal(0.0),toYReal(COUNTRY));

			double dx = toXReal(0.0),
						 dy = toYReal(0.0);

			g->DrawArc(dx, dy, _size.width-2*dx, _size.height-2*dy, 3*M_PI_2,  2*M_PI);
			
			g->DrawCircle( toXReal(px)-3, toYReal(py)-3, 6);
		}    

		virtual void Paint(jgui::Graphics *g) 
		{
			int w = _size.width, 
					h = _size.height;

			g->SetColor(bkC);
			g->FillRectangle(0, 0, _size.width, _size.height);

			if ((offscreen == NULL) || ((imagewidth != w) || (imageheight != h))) {
				offscreen = jgui::Image::CreateImage(w, h);
				imagewidth = w;
				imageheight = h;
			}

			jgui::Graphics *goff = offscreen->GetGraphics();

			paintLeft(goff);
			g->DrawImage(offscreen, 0, 0);

			// CLEAR ALL
			g->SetColor(bkC);
			g->FillRectangle(w/2+30,0,w/2+130, 20);
			g->SetColor(fgC);
		}

};

int main()
{
	srand(time(NULL));

	som n;

	n.kohonenInit();
	n.Start();
	n.Show(true);

	return 0;
}


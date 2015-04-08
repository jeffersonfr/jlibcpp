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
#include "jfileinputstream.h"
#include "jindexedimage.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <values.h>

#define FILENAME "images/tux-zombie.png"

class Quantization {
 
	private:
		double **_network;
		double *_bias;
		double *_freq;
		double _gamma;
		double _beta;
		double _betagamma;
		uint32_t *_pixels;
		int **_colormap;
		int *_netindex;
		int _ncycles; // no. of learning cycles
		int _netsize; // number of colours used
		int _specials; // number of reserved colours used
		int _bgColour; // reserved background colour
		int _cutnetsize;
		int _maxnetpos;
		int _initrad; // for 256 cols, radius starts at 32
		int _radiusbiasshift;
		int _radiusbias;
		int _initBiasRadius;
		int _radiusdec; // factor of 1/30 each cycle
		int _alphabiasshift; // alpha starts at 1
		int _initalpha; // biased by 10 bits
		int _prime1;
		int _prime2;
		int _prime3;
		int _prime4;
		int _maxprime;
		int _samplefac;
		int _width;
		int _height;

	private:
		void SetPixels (uint32_t *data, int width, int height) {
			if (width*height < _maxprime) {
				throw new jcommon::RuntimeException("Image is too small");
			}

			_pixels = data;
			_width = width;
			_height = height;
		}

		void AlterSingle(double alpha, int i, double b, double g, double r) {
			double *n = _network[i];
			
			n[0] -= (alpha*(n[0] - b));
			n[1] -= (alpha*(n[1] - g));
			n[2] -= (alpha*(n[2] - r));
		}

		void AlterNeigh(double alpha, int rad, int i, double b, double g, double r) {
			int lo = i-rad;   
			
			if (lo < _specials-1) {
				lo = _specials-1;
			}

			int hi = i+rad;   
			
			if (hi > _netsize) {
				hi = _netsize;
			}

			int j = i+1;
			int k = i-1;
			int q = 0;

			while ((j<hi) || (k>lo)) {
				double a = (alpha * (rad*rad - q*q)) / (rad*rad);
				
				q ++;
				
				if (j<hi) {
					double *p = _network[j];
				
					p[0] -= (a*(p[0] - b));
					p[1] -= (a*(p[1] - g));
					p[2] -= (a*(p[2] - r));
					j++;
				}

				if (k>lo) {
					double *p = _network[k];

					p[0] -= (a*(p[0] - b));
					p[1] -= (a*(p[1] - g));
					p[2] -= (a*(p[2] - r));
					k--;
				}
			}
		}

		int Contest(double b, double g, double r) {
			double bestd = FLT_MAX;
			double bestbiasd = bestd;
			int bestpos = -1;
			int bestbiaspos = bestpos;

			for (int i=_specials; i<_netsize; i++) {
				double *n = _network[i];
				double dist = n[0] - b;   
				
				if (dist<0) {
					dist = -dist;
				}

				double a = n[1] - g;   
				
				if (a<0) {
					a = -a;
				}
				
				dist += a;
				a = n[2] - r;   
				
				if (a<0) {
					a = -a;
				}

				dist += a;
				
				if (dist<bestd) {
					bestd=dist; 
					bestpos=i;
				}

				double biasdist = dist - _bias[i];
				
				if (biasdist<bestbiasd) {
					bestbiasd=biasdist; 
					bestbiaspos=i;
				}
				
				_freq[i] -= _beta * _freq[i];
				_bias[i] += _betagamma * _freq[i];
			}

			_freq[bestpos] += _beta;
			_bias[bestpos] -= _betagamma;
			
			return bestbiaspos;
		}

		int SpecialFind(double b, double g, double r) {
			for (int i=0; i<_specials; i++) {
				double *n = _network[i];
			
				if (n[0] == b && n[1] == g && n[2] == r) {
					return i;
				}
			}

			return -1;
		}

		void Learn() {
			int biasRadius = _initBiasRadius;
			int alphadec = 30 + ((_samplefac-1)/3);
			int lengthcount = _width*_height;
			int samplepixels = lengthcount / _samplefac;
			int delta = samplepixels / _ncycles;
			int alpha = _initalpha;
			int rad = biasRadius >> _radiusbiasshift;
			int i = 0;
			
			if (rad <= 1) {
				rad = 0;
			}

			int step = 0;
			int pos = 0;

			if ((lengthcount%_prime1) != 0) {
				step = _prime1;
			} else {
				if ((lengthcount%_prime2) !=0) {
					step = _prime2;
				} else {
					if ((lengthcount%_prime3) !=0) {
						step = _prime3;
					} else {
						step = _prime4;
					}
				}
			}

			i = 0;
			
			while (i < samplepixels) {
				uint32_t p = _pixels[pos];
				int red = (p >> 0x10) & 0xff;
				int green = (p >> 0x08) & 0xff;
				int blue = (p >> 0x00) & 0xff;

				double b = blue;
				double g = green;
				double r = red;

				if (i == 0) {   // remember background colour
					_network[_bgColour][0] = b;
					_network[_bgColour][1] = g;
					_network[_bgColour][2] = r;
				}

				int j = SpecialFind(b, g, r);
			
				j = j < 0 ? Contest(b, g, r) : j;

				if (j >= _specials) {   // don't learn for specials
					double a = (1.0 * alpha) / _initalpha;
				
					AlterSingle(a, j, b, g, r);
					
					if (rad > 0) {
						AlterNeigh(a, rad, j, b, g, r);   // alter neighbours
					}
				}

				pos += step;
				
				while (pos >= lengthcount) {
					pos -= lengthcount;
				}

				i++;
				
				if (i%delta == 0) {	
					alpha -= alpha / alphadec;
					biasRadius -= biasRadius / _radiusdec;
					rad = biasRadius >> _radiusbiasshift;
				
					if (rad <= 1) {
						rad = 0;
					}
				}
			}
		}

		void Fix() {
			for (int i=0; i<_netsize; i++) {
				for (int j=0; j<3; j++) {
					int x = (int) (0.5 + _network[i][j]);
			
					if (x < 0) {
						x = 0;
					}

					if (x > 255) {
						x = 255;
					}

					_colormap[i][j] = x;
				}

				_colormap[i][3] = i;
			}
		}

		void PaletteBuild() {
			int previouscol = 0;
			int startpos = 0;

			for (int i=0; i<_netsize; i++) {
				int *p = _colormap[i];
				int *q = NULL;
				int smallpos = i;
				int smallval = p[1];

				// find smallest in i..netsize-1
				for (int j=i+1; j<_netsize; j++) {
					q = _colormap[j];
	
					if (q[1] < smallval) {		// index on g
						smallpos = j;
						smallval = q[1];	// index on g
					}
				}

				q = _colormap[smallpos];
				
				if (i != smallpos) {
					int j = q[0];   q[0] = p[0];   p[0] = j;
					
					j = q[1];   
					q[1] = p[1];   
					p[1] = j;
					
					j = q[2];   
					q[2] = p[2];   
					p[2] = j;
					
					j = q[3];   
					q[3] = p[3];   
					p[3] = j;
				}

				if (smallval != previouscol) {
					_netindex[previouscol] = (startpos+i)>>1;
					
					for (int j=previouscol+1; j<smallval; j++) {
						_netindex[j] = i;
					}

					previouscol = smallval;
					startpos = i;
				}
			}

			_netindex[previouscol] = (startpos+_maxnetpos)>>1;
			
			for (int j=previouscol+1; j<256; j++) {
				_netindex[j] = _maxnetpos; // really 256
			}
		}

		int QuickColorSearch(int b, int g, int r) {
			int bestd = 1000;		// biggest possible dist is 256*3
			int best = -1;
			
			for (int i=0; i<_netsize; i++) {
				int *p = _colormap[i];
				int dist = p[1] - g;
			
				if (dist<0) {
					dist = -dist;
				}

				int a = p[0] - b;   
				
				if (a<0) {
					a = -a;
				}

				dist += a;
				a = p[2] - r;   
				
				if (a<0) {
					a = -a;
				}

				dist += a;
				
				if (dist<bestd) {
					bestd=dist; 
					best=i;
				}
			}

			return best;
		}

		int ColorSearch(int b, int g, int r) {
			int bestd = 1000;		// biggest possible dist is 256*3
			int best = -1;
			int i = _netindex[g];	// index on g
			int j = i-1;		// start at netindex[g] and work outwards

			while ((i < _netsize) || (j >= 0)) {
				if (i < _netsize) {
					int *p = _colormap[i];
					int dist = p[1] - g;		// inx key
				
					if (dist >= bestd) {
						i = _netsize;	// stop iter
					} else {
						if (dist < 0) {
							dist = -dist;
						}

						int a = p[0] - b;  
						
						if (a<0) {
							a = -a;
						}

						dist += a;
						
						if (dist < bestd) {
							a = p[2] - r;   
							
							if (a < 0) {
								a = -a;
							}

							dist += a;
							
							if (dist<bestd) {
								bestd=dist; 
								best=i;
							}
						}

						i++;
					}
				}

				if (j>=0) {
					int *p = _colormap[j];
					int dist = g - p[1]; // inx key - reverse dif
				
					if (dist >= bestd) {
						j = -1; // stop iter
					} else {
						if (dist < 0) {
							dist = -dist;
						}

						int a = p[0] - b; 
						
						if (a<0) {
							a = -a;
						}

						dist += a;
						
						if (dist<bestd) {
							a = p[2] - r;
							
							if (a<0) {
								a = -a;
							}

							dist += a;
							
							if (dist<bestd) {
								bestd=dist; 
								best=j;
							}
						}

						j--;
					}
				}
			}

			return best;
		}

		void SetUpArrays() {
			_network[0][0] = 0.0;	// black
			_network[0][1] = 0.0;
			_network[0][2] = 0.0;

			_network[1][0] = 255.0;	// white
			_network[1][1] = 255.0;
			_network[1][2] = 255.0;

			for (int i=0; i<_specials; i++) {
				_freq[i] = 1.0 / _netsize;
				_bias[i] = 0.0;
			}

			for (int i=_specials; i<_netsize; i++) {
				double *p = _network[i];
				
				p[0] = (255.0 * (i-_specials)) / _cutnetsize;
				p[1] = (255.0 * (i-_specials)) / _cutnetsize;
				p[2] = (255.0 * (i-_specials)) / _cutnetsize;

				_freq[i] = 1.0 / _netsize;
				_bias[i] = 0.0;
			}
		}    	

	public:
    Quantization (uint32_t *pixels, int width, int height, int colors, int sample) {
			if (sample < 1 || sample > 30) {
				throw jcommon::RuntimeException("Sample must be 1..30");
			}

			if (colors < 2) {
				colors = 2;
			}

			if (colors > 255) {
				colors = 255;
			}

			if (sample < 1) {
				sample = 1;
			}

			if (sample > 30) {
				sample = 30;
			}

			_netsize = colors + 1;
			_network = NULL;
			_bias = NULL;
			_freq = NULL;
			_gamma = 1024.0;
			_beta = 1.0/1024.0;
			_betagamma = _beta * _gamma;
			_pixels = NULL;
			_colormap = NULL;
			_netindex = NULL;
			_ncycles	=	100;
			_specials = 3;
			_bgColour = _specials - 1;
			_cutnetsize = _netsize - _specials;
			_maxnetpos = _netsize - 1;
			_initrad = _netsize/8;
			_radiusbiasshift = 6;
			_radiusbias = 1 << _radiusbiasshift;
			_initBiasRadius = _initrad*_radiusbias;
			_radiusdec = 30;
			_alphabiasshift = 10; 
			_initalpha = 1 << _alphabiasshift;
			_prime1 = 499;
			_prime2 = 491;
			_prime3 = 487;
			_prime4 = 503;
			_maxprime = _prime4;
			_samplefac = 0;
			_width = -1;
			_height = -1;

			_netindex = new int[256];
			_bias = new double[_netsize];
			_freq = new double[_netsize];

			_network = new double*[_netsize];

			for (int i=0; i<_netsize; i++) {
				_network[i] = new double[3];
			}

			_colormap = new int*[_netsize];

			for (int i=0; i<_netsize; i++) {
				_colormap[i] = new int[4];
			}

			_samplefac = sample;
			
			SetPixels(pixels, width, height);
			SetUpArrays();
		}

		void Start() {
			Learn();
			Fix();
			PaletteBuild();
		}

		int GetColorCount() {
			return _netsize;
		}

		uint32_t GetColor(int i) {
			if (i < 0 || i >= _netsize) {
				return 0;
			}
			
			int b = _colormap[i][0] & 0xff;
			int g = _colormap[i][1] & 0xff;
			int r = _colormap[i][2] & 0xff;
			
			return (uint32_t)(0xff000000 | (r << 0x10) | (g << 0x08) | (b << 0x00));
		}

		uint32_t Convert(uint32_t color) {
			int r = (color >> 0x10) & 0xff;
			int g = (color >> 0x08) & 0xff;
			int b = (color >> 0x00) & 0xff;
			int i = ColorSearch(b, g, r);
			int bb = _colormap[i][0] & 0xff;
			int gg = _colormap[i][1] & 0xff;
			int rr = _colormap[i][2] & 0xff;

			return (uint32_t)(0xff000000 | (rr << 0x10) | (gg << 0x08) | (bb << 0x00));
		}

		int Lookup(uint32_t color) {
			int r = (color >> 0x10) & 0xff;
			int g = (color >> 0x08) & 0xff;
			int b = (color >> 0x00) & 0xff;
			
			return ColorSearch(b, g, r);
		}

		int Lookup(int r, int g, int b) {
			return ColorSearch (r, g, b);
		}

};

class Picture : public jgui::Component {

	protected:
		jgui::Image *_image;
		std::string _title;

	public:
		Picture(int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_image = NULL;
		}

		virtual ~Picture()
		{
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

			if (IsFontSet() == true) {
				int height = _font->GetSize();

				g->SetColor(jgui::Color::White);
				g->DrawString(_title, 0, 0);
				g->DrawImage(_image, 0, height, GetWidth(), GetHeight()-height);
			}
		}

};

class FilePathImageTest : public Picture {

	private:

	public:
		FilePathImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			_image = jgui::Image::CreateImage(FILENAME);
			_title = "File Path";
		}

		virtual ~FilePathImageTest()
		{
		}

};

class FileImageTest : public Picture {

	private:

	public:
		FileImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jio::File file(FILENAME);

			_image = jgui::Image::CreateImage(&file);
			_title = "File Object";
		}

		virtual ~FileImageTest()
		{
		}

};

class InputStreamImageTest : public Picture {

	private:

	public:
		InputStreamImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jio::FileInputStream fis(FILENAME);

			_image = jgui::Image::CreateImage(&fis);
			_title = "InputStream";
		}

		virtual ~InputStreamImageTest()
		{
		}

};

class CopyImageTest : public Picture {

	private:

	public:
		CopyImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jgui::Image *image = jgui::Image::CreateImage(FILENAME);

			_image = jgui::Image::CreateImage(image);
			_title = "Clone Image";

			delete image;
		}

		virtual ~CopyImageTest()
		{
		}

};

class BufferedImageTest : public Picture {

	private:

	public:
		BufferedImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			int iw,
					ih;

			jgui::Image::GetImageSize(FILENAME, &iw, &ih);

			_image = jgui::Image::CreateImage(iw, ih);

			if (_image->GetGraphics() != NULL) {
				_image->GetGraphics()->DrawImage(FILENAME, 0, 0, _image->GetWidth(), _image->GetHeight());
			}

			_title = "Buffered Image";
		}

		virtual ~BufferedImageTest()
		{
		}

};

class RGBImageTest : public Picture {

	private:

	public:
		RGBImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jgui::Image *image = jgui::Image::CreateImage(FILENAME);
			jgui::jsize_t scale = image->GetGraphics()->GetWorkingScreenSize();;

			uint32_t *rgb = NULL;

			image->GetRGB(&rgb, 0, 0, image->GetWidth(), image->GetHeight());

			_image = jgui::Image::CreateImage(rgb, image->GetWidth(), image->GetHeight(), scale.width, scale.height);

			_title = "RGB Image";
		}

		virtual ~RGBImageTest()
		{
		}

};

class RawImageTest : public Picture {

	private:

	public:
		RawImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jio::File file(FILENAME);

			char *raw = new char[file.GetSize()];
			int64_t packet = 64*1024,
							length,
							count = 0;

			while ((length = file.Read(raw+(int)count, packet)) > 0) {
				count = count + length;
			}
 
			_image = jgui::Image::CreateImage(raw, file.GetSize());
			_title = "Raw Image";

			delete [] raw;
		}

		virtual ~RawImageTest()
		{
		}

};

class IndexedImageTest : public Picture {

	private:

	public:
		IndexedImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jgui::Image *image = jgui::Image::CreateImage(FILENAME);

			if (image != NULL) {
				int size = image->GetWidth()*image->GetHeight();
				uint32_t *pixels = new uint32_t[size];
				uint32_t *rgb = NULL;

				image->GetRGB(&rgb, 0, 0, image->GetWidth(), image->GetHeight());

    		Quantization q(rgb, image->GetWidth(), image->GetHeight(), 255, 1.0);

				q.Start();

				for (int i=0; i<size; i++) {
					pixels[i] = q.Convert(rgb[i]);
				}

				_image = jgui::IndexedImage::Pack(pixels, image->GetWidth(), image->GetHeight());
			}

			_title = "Indexed Image";
		}

		virtual ~IndexedImageTest()
		{
		}

};

class Test : public Picture {

	private:

	public:
		Test(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			_image = NULL;
			_title = "Image";
		}

		virtual ~Test()
		{
		}

};

class Main : public jgui::Frame{

	private:

	public:
		Main():
			jgui::Frame("Image Create Test", 0, 0, 1920, 1080)
		{
			int w = 300,
					h = 300,
					gapx = 50,
					gapy = 50,
					dx = (1920-4*w-3*gapx)/2,
					dy = (1080-2*h-1*gapy)/2;

			Add(new FilePathImageTest(dx+0*(w+gapx), dy+0*(h+gapy), w, h));
			Add(new FileImageTest(dx+1*(w+gapx), dy+0*(h+gapy), w, h));
			Add(new InputStreamImageTest(dx+2*(w+gapx), dy+0*(h+gapy), w, h));
			Add(new CopyImageTest(dx+3*(w+gapx), dy+0*(h+gapy), w, h));
		
			Add(new BufferedImageTest(dx+0*(w+gapx), dy+1*(h+gapy), w, h));
			Add(new RGBImageTest(dx+1*(w+gapx), dy+1*(h+gapy), w, h));
			Add(new RawImageTest(dx+2*(w+gapx), dy+1*(h+gapy), w, h));
			Add(new IndexedImageTest(dx+3*(w+gapx), dy+1*(h+gapy), w, h));
		}

		virtual ~Main()
		{
			while (GetComponents().size() > 0) {
				Picture *pic = (Picture *)(*GetComponents().begin());

				GetComponents().erase(GetComponents().begin());

				delete pic;
			}
		}

};

int main(int argc, char **argv)
{
	Main main;

	main.Show(true);

	return 0;
}

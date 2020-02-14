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
#include "jgui/jindexedimage.h"
#include "jgui/jbufferedimage.h"
#include "jio/jfileinputstream.h"
#include "jio/jmemoryinputstream.h"
#include "jexception/jruntimeexception.h"

#include <values.h>

#define FILENAME "images/tux-zombie.png"

class Quantization {
 
	private:
		double 
      **_network,
  		*_bias,
	  	*_freq,
		  _gamma,
  		_beta,
	  	_betagamma;
		uint32_t 
      *_pixels;
		int 
      **_colormap,
      *_netindex,
      _ncycles, // no. of learning cycles
      _netsize, // number of colours used
      _specials, // number of reserved colours used
      _bgColour, // reserved background colour
      _cutnetsize,
      _maxnetpos,
      _initrad, // for 256 cols, radius starts at 32
      _radiusbiasshift,
      _radiusbias,
      _initBiasRadius,
      _radiusdec, // factor of 1/30 each cycle
      _alphabiasshift, // alpha starts at 1
      _initalpha, // biased by 10 bits
      _prime1,
      _prime2,
      _prime3,
      _prime4,
      _maxprime,
      _samplefac,
      _width,
      _height;

	private:
		void SetPixels (uint32_t *data, int width, int height) {
			if (width*height < _maxprime) {
				throw new jexception::RuntimeException("Image is too small");
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
				int *q = nullptr;
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
				throw jexception::RuntimeException("Sample must be 1..30");
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
			_network = nullptr;
			_bias = nullptr;
			_freq = nullptr;
			_gamma = 1024.0;
			_beta = 1.0/1024.0;
			_betagamma = _beta * _gamma;
			_pixels = nullptr;
			_colormap = nullptr;
			_netindex = nullptr;
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

    virtual ~Quantization()
    {
      delete [] _netindex;
      delete [] _bias;
      delete [] _freq;

			for (int i=0; i<_netsize; i++) {
				delete [] _network[i];
			}
      
      delete [] _network;
			
			for (int i=0; i<_netsize; i++) {
				delete [] _colormap[i];
			}
      
      delete [] _colormap;
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
			jgui::Component({x, y, w, h})
		{
			_image = nullptr;
		}

		virtual ~Picture()
		{
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

			jgui::Theme 
				*theme = GetTheme();
			jgui::Font 
				*font = theme->GetFont("component.font");
			jgui::jsize_t
				size = GetSize();
			int 
        height = font->GetSize();

			if (font != nullptr) {

				g->SetColor(jgui::jcolor_name_t::White);
				g->SetFont(font);
				g->DrawString(_title, jgui::jpoint_t<int>{0, 0});
      }
			
      g->DrawImage(_image, {0, height, size.width, size.height - height});
		}

};

class FilePathImageTest : public Picture {

	private:

	public:
		FilePathImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			_image = new jgui::BufferedImage(FILENAME);
			_title = "File Path";
		}

		virtual ~FilePathImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class FileImageTest : public Picture {

	private:

	public:
		FileImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jio::File *file = jio::File::OpenFile(FILENAME);

			_image = new jgui::BufferedImage(file->GetPath());
			_title = "File Object";

			delete file;
		}

		virtual ~FileImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class InputStreamImageTest : public Picture {

	private:

	public:
		InputStreamImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jio::FileInputStream fis(FILENAME);

			_image = new jgui::BufferedImage(&fis);
			_title = "InputStream";
		}

		virtual ~InputStreamImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class CopyImageTest : public Picture {

	private:

	public:
		CopyImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jgui::Image *image = new jgui::BufferedImage(FILENAME);

			_image = dynamic_cast<jgui::Image *>(image->Clone());
			_title = "Clone Image";

			delete image;
		}

		virtual ~CopyImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class BufferedImageTest : public Picture {

	private:

	public:
		BufferedImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jgui::Image *image = new jgui::BufferedImage(FILENAME);

			jgui::jsize_t
				size = image->GetSize();

			_image = new jgui::BufferedImage(jgui::JPF_RGB32, size);

			_image->GetGraphics()->DrawImage(image, jgui::jpoint_t<int>{0, 0});

			delete image;

			_title = "Buffered Image";
		}

		virtual ~BufferedImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class RGBImageTest : public Picture {

	private:

	public:
		RGBImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jgui::Image *image = new jgui::BufferedImage(FILENAME);

			jgui::jsize_t
				size = image->GetSize();
			uint32_t 
				rgb[size.width*size.height];

			image->GetRGBArray(rgb, {0, 0, size.width, size.height});

      delete image;
      image = nullptr;

			_image = new jgui::BufferedImage(jgui::JPF_RGB32, size);

      _image->GetGraphics()->SetCompositeFlags(jgui::JCF_SRC);
      _image->GetGraphics()->SetRGBArray(rgb, {0, 0, size.width, size.height});

			_title = "RGB Image";
		}

		virtual ~RGBImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class RawImageTest : public Picture {

	private:

	public:
		RawImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jio::File *file = jio::File::OpenFile(FILENAME);

			char *raw = new char[file->GetSize()];
			int64_t 
			  packet = 64*1024,
				length,
				count = 0;

			while ((length = file->Read(raw+(int)count, packet)) > 0) {
				count = count + length;
			}
 
      jio::MemoryInputStream is((const uint8_t *)raw, file->GetSize());

			_image = new jgui::BufferedImage(&is);
			_title = "Raw Image";

			delete file;
			delete [] raw;
		}

		virtual ~RawImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class IndexedImageTest : public Picture {

	private:

	public:
		IndexedImageTest(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			jgui::Image *image = new jgui::BufferedImage(FILENAME);

			jgui::jsize_t
				size = image->GetSize();
			int 
				length = size.width*size.height;
			uint32_t 
				rgb[size.width*size.height],
				*pixels = new uint32_t[length];

			image->GetRGBArray(rgb, {0, 0, size.width, size.height});

      delete image;
      image = nullptr;

   		Quantization q(rgb, size.width, size.height, 255, 1.0);

			q.Start();

			for (int i=0; i<length; i++) {
				pixels[i] = q.Convert(rgb[i]);
			}

			_image = jgui::IndexedImage::Pack(pixels, size);

      delete [] pixels;
      pixels = nullptr;

			_title = "Indexed Image";
		}

		virtual ~IndexedImageTest()
		{
      delete _image;
      _image = nullptr;
		}

};

class Test : public Picture {

	private:

	public:
		Test(int x, int y, int w, int h):
			Picture(x, y, w, h)
		{
			_image = nullptr;
			_title = "Image";
		}

		virtual ~Test()
		{
		}

};

class Main : public jgui::Window {

	private:

	public:
		Main():
			jgui::Window({960, 540})
		{
      jgui::jsize_t
        size = GetSize();
			int 
        gapx = 64,
  			gapy = 64,
	  		w = 180,
		  	h = 180,
			  dx = (size.width-4*w-3*gapx)/2,
			  dy = (size.height-2*h-1*gapy)/2;

			gapx = gapx/2;
			gapy = gapy/3;

			Add(new FilePathImageTest(dx+0*(w+gapx)+gapx, dy+0*(h+gapy)+gapy, w, h));
			Add(new FileImageTest(dx+1*(w+gapx)+gapx, dy+0*(h+gapy)+gapy, w, h));
			Add(new InputStreamImageTest(dx+2*(w+gapx)+gapx, dy+0*(h+gapy)+gapy, w, h));
			Add(new CopyImageTest(dx+3*(w+gapx)+gapx, dy+0*(h+gapy)+gapy, w, h));
		
			Add(new BufferedImageTest(dx+0*(w+gapx)+gapx, dy+1*(h+gapy)+gapy, w, h));
			Add(new RGBImageTest(dx+1*(w+gapx)+gapx, dy+1*(h+gapy)+gapy, w, h));
			Add(new RawImageTest(dx+2*(w+gapx)+gapx, dy+1*(h+gapy)+gapy, w, h));
			Add(new IndexedImageTest(dx+3*(w+gapx)+gapx, dy+1*(h+gapy)+gapy, w, h));
		}

		virtual ~Main()
		{
			while (GetComponents().size() > 0) {
        jgui::Component *c = *GetComponents().begin();

				Remove(c);

				delete c;
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app;

	app.SetTitle("Draw Image");

	jgui::Application::Loop();

	return 0;
}

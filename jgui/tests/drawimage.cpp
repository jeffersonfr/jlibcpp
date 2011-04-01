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

#include "quantization.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define FILENAME "images/tux-zombie.png"

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
				int height = _font->GetHeight();

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

			uint32_t *rgb = NULL;

			image->GetRGB(&rgb, 0, 0, image->GetWidth(), image->GetHeight());

			_image = jgui::Image::CreateImage(rgb, image->GetWidth(), image->GetHeight());

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

			uint8_t *raw = new uint8_t[file.GetSize()];
			int64_t packet = 64*1024,
							length,
							count = 0;

			while ((length = file.Read((char *)(raw+(int)count), packet)) > 0) {
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
				uint32_t *pix = new uint32_t[size];
				uint32_t *rgb = NULL;
				uint32_t *palette = NULL;

				image->GetRGB(&rgb, 0, 0, image->GetWidth(), image->GetHeight());

				Cube *cube = Quantization::GetInstance()->quantizeImage(rgb, image->GetWidth()*image->GetHeight(), 255);

				cube->GetPalette(&palette); 

				for (int i=0; i<size; i++) {
					pix[i] = palette[rgb[i]];
				}

				delete cube;

				_image = jgui::IndexedImage::Pack(pix, image->GetWidth(), image->GetHeight());
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

	main.Show();

	return 0;
}

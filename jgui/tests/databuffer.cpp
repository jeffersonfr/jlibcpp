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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

IDirectFB *dfb = NULL;
IDirectFBSurface *primary = NULL;
int screen_width, 
		screen_height;

DIRenderCallbackResult render_callback( DFBRectangle *rect, void *ctx )
{
	int               width;
	int               height;
	IDirectFBSurface *image = (IDirectFBSurface*) ctx;

	image->GetSize(image, &width, &height);
	primary->Blit(primary, image, rect, (screen_width-width)/2+rect->x, (screen_height-height)/2+rect->y);
	
	DFBRegion rgn;

	rgn.x1 = (screen_width-width)/2+rect->x;
	rgn.y1 = (screen_height-height)/2+rect->y;
	rgn.x2 = (screen_width-width)/2+rect->x+width;
	rgn.y2 = (screen_height-height)/2+rect->y+1;

	primary->Flip(primary, &rgn, DSFLIP_NONE);
	
	return DIRCR_OK;
}

class TestFileStreamed : public jthread::Thread{

	private:
		IDirectFBDataBuffer *buffer;
		std::string filename;

	public:
		TestFileStreamed(std::string filename)
		{
			this->filename = filename;
		}

		virtual ~TestFileStreamed()
		{
		}

		void Load()
		{
			DFBSurfaceDescription     sdsc;
			IDirectFBImageProvider   *image_provider = NULL;
			IDirectFBSurface         *image;

			primary->Clear(primary, 0, 0, 0, 0);

			// create a streamed data buffer
			dfb->CreateDataBuffer(dfb, NULL, &buffer);

			// create thread that will feed the buffer
			Start();

			if (buffer->CreateImageProvider(buffer, &image_provider) != DFB_OK) {
				return;
			}

			image_provider->GetSurfaceDescription(image_provider, &sdsc);

			std::cout << "\nImage size: " << sdsc.width << "x" << sdsc.height << std::endl;

			dfb->CreateSurface(dfb, &sdsc, &image);

			// image_provider->SetRenderCallback( image_provider, render_callback, image );
			image_provider->RenderTo(image_provider, image, NULL);

			primary->Blit(primary, image, NULL, (screen_width-(int)sdsc.width)/2, (screen_height-(int)sdsc.height)/2);
			primary->Flip(primary, NULL, DSFLIP_NONE);

			image->Release( image );
			if (image_provider)
				image_provider->Release(image_provider);

			buffer->Release( buffer );
		}

		virtual void Run()
		{
			int                  fd,
													 total = 0;
			unsigned int         len   = 0;
			char                 data[8192];
			struct stat          stat;
			DFBRectangle         rect;
			
			fd = open(filename.c_str(), O_RDONLY);
			if (fd < 0) {
				return;
			}

			if (fstat( fd, &stat ) < 0) {
				close( fd );
				return;
			}

			rect.x = 0;
			rect.y = screen_height - 10;
			rect.w = screen_width;
			rect.h = 5;

			while (1) {
				usleep( ((rand()%1000000) + 2000000) / (stat.st_size >> 10) );

				// actually limit the amount of data in buffer to 64 Kb
				buffer->GetLength(buffer, &len);
				if (len >= 64*1024)
					continue;

				// put some data with variing length
				len = read( fd, data, (rand()%8192) + 1 );
				if (len <= 0) {
					buffer->Finish(buffer);
					break;
				}

				buffer->PutData(buffer, data, len);

				total += len;

				buffer->GetLength(buffer, &len);
			}

			close( fd );
		}

};

int main(int argc, char **argv)
{
	if (argc == 1) {
		std::cout << "usage:: " << argv[0] << " <file>" << std::endl;

		return 1;
	}

	std::string filename = (const char *)argv[1];

	dfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

	jgui::Window *window = new jgui::Window(0, 0, 1920, 1080);

	window->Show(false);
	primary = (IDirectFBSurface *)window->GetGraphics()->GetNativeSurface();

	screen_width = jgui::GFXHandler::GetInstance()->GetScreenWidth();
	screen_height = jgui::GFXHandler::GetInstance()->GetScreenHeight();

	TestFileStreamed test(filename);

	test.Load();

	sleep(10);

	jgui::GFXHandler::GetInstance()->Release();

	return 0;
}


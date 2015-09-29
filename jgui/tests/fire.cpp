#include "jwindow.h"

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

uint32_t *fire = NULL;
uint32_t *buffer = NULL; //this is the buffer to be drawn to the screen
uint32_t palette[256]; //this will contain the color palette

int main(int argc, char *argv[])
{
	jgui::Window window;
	jgui::Image *image = jgui::Image::CreateImage(jgui::JPF_ARGB, window.GetWidth()/6, window.GetHeight()/6);
	jgui::jsize_t size = image->GetSize();

	window.Show(false);

	fire = new uint32_t[size.width*size.height];
	buffer = new uint32_t[size.width*size.height];

  // make sure the fire buffer is zero in the beginning
	for(int x = 0; x < size.width; x++) {
		for(int y = 0; y < size.height; y++) {
			fire[y*size.width+x] = 0;
		}
	}
  
  // generate the palette
  for(int x = 0; x < 256; x++) {
		double h = (x/8.0)/255.0;
		double s = 255.0/255.0;
		double l = std::min(255.0, (x * 2)/255.0);
		int r, g, b;

    jgui::Color::HSBtoRGB(h, s, l, &r, &g, &b);

		palette[x] = (0xff << 0x18) | (r << 0x10) | (g << 0x08) | (b << 0x00);
  }
  
  //start the loop (one frame per loop)
  while(true) {
    //randomize the bottom row of the fire buffer
    for(int x = 0; x < size.width; x++) {
			fire[(size.height - 1)*size.width + x] = abs(32768 + rand()) % 256;
		}
    
		//do the fire calculations for every pixel, from top to bottom
    for(int y = 0; y < size.height-1; y++) {
			for(int x = 0; x < size.width-1; x++) {
				fire[y*size.width + x] = ((
						fire[(y+1)*size.width + (x-1+size.width)%size.width] + 
						fire[(y+1)*size.width + (x)%size.width] + 
						fire[(y+1)*size.width + (x+1)%size.width] + 
						fire[((y+2) % size.height)*size.width + (x)%size.width]
						) * 32) / 129;
				// fire[y*size.width + x]= ((fire[(y + 1)*size.width + (x-1)] + fire[(y+2)*size.width + x] + fire[(y + 1)*size.width + (x + 1)] + fire[(y + 3)*size.width + x]) * 64) / 257;
			}
		}

    //set the drawing buffer to the fire buffer, using the palette colors
		for(int x = 0; x < size.width; x++) {
			for(int y = 0; y < size.height-1; y++) {
				buffer[y*size.width + x] = palette[fire[y*size.width+x]];
			}
		}
    
		image->GetGraphics()->SetRGBArray(buffer, 0, 0, size.width, size.height);
		window.GetGraphics()->DrawImage(image, 0, 0, window.GetWidth(), window.GetHeight());
		window.GetGraphics()->Flip();
  }
}

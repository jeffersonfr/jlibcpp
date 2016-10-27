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
/*
   (c) Copyright 2012-2013  DirectFB integrated media GmbH
   (c) Copyright 2001-2013  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Shimokawa <andi@directfb.org>,
              Marek Pikarski <mass@directfb.org>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include "Stdafx.h"
#include "giflightplayer.h"
#include "nativeimage.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvolumecontrol.h"
#include "jaudioconfigurationcontrol.h"
#include "jmediaexception.h"
#include "jgfxhandler.h"
#include "jinputstream.h"
#include "jcondition.h"
#include "jfileinputstream.h"
#include "jsemaphoretimeoutexception.h"
#include "jdebug.h"

#if defined(DIRECTFB_NODEPS_UI)
#include <directfb.h>
#else
#include <cairo.h>
#endif

#define MAXCOLORMAPSIZE 256

#define CM_RED   0
#define CM_GREEN 1
#define CM_BLUE  2

#define MAX_LWZ_BITS 12

#define INTERLACE     0x40
#define LOCALCOLORMAP 0x80

#define BitSet(byte, bit) (((byte) & (bit)) == (bit))

#define LM_to_uint(a,b) (((b)<<8)|(a))

namespace jmedia {

namespace giflightplayer {

struct AnimatedGIFData {
	jio::InputStream   *stream;
	jthread::Thread    *thread;
	jthread::Mutex     lock;
	jthread::Condition cond;

	uint32_t  *image;

	char      Version[4];
	uint32_t  Width;
	uint32_t  Height;
	uint8_t   ColorMap[3][MAXCOLORMAPSIZE];
	uint32_t  BitPixel;
	uint32_t  ColorResolution;
	uint32_t  Background;
	uint32_t  AspectRatio;

	int       transparent;
	uint32_t  delayTime;
	int       inputFlag;
	int       disposal;

	uint8_t   buf[280];
	int       curbit, lastbit, done, last_byte;

	int       fresh;
	int       code_size, set_code_size;
	int       max_code, max_code_size;
	int       firstcode, oldcode;
	int       clear_code, end_code;
	int       table[2][(1 << MAX_LWZ_BITS)];
	int       stack[(1<<(MAX_LWZ_BITS))*2], *sp;

	int       ZeroDataBlock;
};

static int FetchData(jio::InputStream *stream, void *data, uint32_t len)
{
	do {
		uint32_t read = 0;

		read = stream->Read((char *)data, len);

		if (read <= 0) {
			return -1;
		}

		data = (char *)data + read;
		len  = len - read;
	} while (len);

	return 0;
}

static int ReadColorMap(jio::InputStream *stream, int number, uint8_t buf[3][MAXCOLORMAPSIZE])
{
	uint8_t rgb[3*number];
	int i;

	if (FetchData(stream, rgb, sizeof(rgb))) {
		JDEBUG(JINFO, "bad colormap");

		return -1;
	}

	for (i=0; i<number; ++i) {
		buf[CM_RED][i] = rgb[i*3+0];
		buf[CM_GREEN][i] = rgb[i*3+1];
		buf[CM_BLUE][i] = rgb[i*3+2];
	}

	return 0;
}

static int GetDataBlock(AnimatedGIFData *data, uint8_t *buf)
{
	unsigned char count;

	if (FetchData(data->stream, &count, 1)) {
		JDEBUG(JINFO, "error in getting DataBlock size");

		return -1;
	}

	data->ZeroDataBlock = (count == 0);

	if ((count != 0) && FetchData(data->stream, buf, count)) {
		JDEBUG(JINFO, "error in reading DataBlock");

		return -1;
	}

	return count;
}

static int GetCode(AnimatedGIFData *data, int code_size, int flag)
{
	int i, j, ret;
	unsigned char count;

	if (flag) {
		data->curbit = 0;
		data->lastbit = 0;
		data->done = false;
		return 0;
	}

	if ( (data->curbit+code_size) >= data->lastbit) {
		if (data->done) {
			if (data->curbit >= data->lastbit) {
				JDEBUG(JINFO, "ran off the end of my bits");
			}
			return -1;
		}
		
		data->buf[0] = data->buf[data->last_byte-2];
		data->buf[1] = data->buf[data->last_byte-1];

		if ((count = GetDataBlock(data, &data->buf[2])) == 0) {
			data->done = true;
		}

		data->last_byte = 2 + count;
		data->curbit = (data->curbit - data->lastbit) + 16;
		data->lastbit = (2+count) * 8;
	}

	ret = 0;
	
	for (i = data->curbit, j = 0; j < code_size; ++i, ++j) {
		ret |= ((data->buf[ i / 8 ] & (1 << (i % 8))) != 0) << j;
	}
	
	data->curbit += code_size;

	return ret;
}

static int DoExtension( AnimatedGIFData *data, int label )
{
	unsigned char buf[256] = { 0 };
	char *str;

	switch (label) {
		case 0x01:              // Plain Text Extension 
			str = (char *)"Plain Text Extension";
			break;
		case 0xff:              // Application Extension 
			str = (char *)"Application Extension";
			break;
		case 0xfe:              // Comment Extension 
			str = (char *)"Comment Extension";
			while (GetDataBlock(data, (uint8_t*) buf) != 0)
				JDEBUG(JINFO, "gif comment: %s", buf);
			return false;
		case 0xf9:              // Graphic Control Extension 
			str = (char *)"Graphic Control Extension";
			(void) GetDataBlock(data, (uint8_t*) buf);
			data->disposal  = (buf[0] >> 2) & 0x7;
			data->inputFlag = (buf[0] >> 1) & 0x1;
			
			if (LM_to_uint( buf[1], buf[2] )) {
				data->delayTime = LM_to_uint( buf[1], buf[2] ) * 10000;
			}

			if ((buf[0] & 0x1) != 0) {
				data->transparent = buf[3];
			} else {
				data->transparent = -1;
			}

			while (GetDataBlock(data, (uint8_t*) buf) != 0);

			return false;
		default:
			str = (char*) buf;
			snprintf(str, 256, "UNKNOWN (0x%02x)", label);
			break;
	}

	JDEBUG(JINFO, "got a '%s' extension", str );

	while (GetDataBlock(data, (uint8_t*)buf) != 0);

	return 0;
}

static int LWZReadByte( AnimatedGIFData *data, int flag, int input_code_size )
{
	int i, code, incode;

	if (flag) {
		data->set_code_size = input_code_size;
		data->code_size = data->set_code_size+1;
		data->clear_code = 1 << data->set_code_size ;
		data->end_code = data->clear_code + 1;
		data->max_code_size = 2*data->clear_code;
		data->max_code = data->clear_code+2;

		GetCode(data, 0, true);

		data->fresh = true;

		for (i = 0; i < data->clear_code; ++i) {
			data->table[0][i] = 0;
			data->table[1][i] = i;
		}
		
		for (; i < (1<<MAX_LWZ_BITS); ++i) {
			data->table[0][i] = data->table[1][0] = 0;
		}
		
		data->sp = data->stack;

		return 0;
	} else if (data->fresh) {
		data->fresh = false;

		do {
			data->firstcode = data->oldcode = GetCode( data, data->code_size, false );
		} while (data->firstcode == data->clear_code);

		return data->firstcode;
	}

	if (data->sp > data->stack) {
		return *--data->sp;
	}

	while ((code = GetCode( data, data->code_size, false )) >= 0) {
		if (code == data->clear_code) {
			for (i = 0; i < data->clear_code; ++i) {
				data->table[0][i] = 0;
				data->table[1][i] = i;
			}
			
			for (; i < (1<<MAX_LWZ_BITS); ++i) {
				data->table[0][i] = data->table[1][i] = 0;
			}
			
			data->code_size = data->set_code_size+1;
			data->max_code_size = 2*data->clear_code;
			data->max_code = data->clear_code+2;
			data->sp = data->stack;
			data->firstcode = data->oldcode = GetCode( data, data->code_size, false );

			return data->firstcode;
		} else if (code == data->end_code) {
			int count;
			uint8_t buf[260];

			if (data->ZeroDataBlock) {
				return -2;
			}

			while ((count = GetDataBlock(data, buf)) > 0);

			if (count != 0) {
				JDEBUG(JINFO, "missing EOD in data stream (common occurence)");
			}

			return -2;
		}

		incode = code;

		if (code >= data->max_code) {
			*data->sp++ = data->firstcode;
			code = data->oldcode;
		}

		while (code >= data->clear_code) {
			*data->sp++ = data->table[1][code];

			if (code == data->table[0][code]) {
				JDEBUG(JINFO, "circular table entry BIG ERROR");
			}

			code = data->table[0][code];
		}

		*data->sp++ = data->firstcode = data->table[1][code];

		if ((code = data->max_code) <(1<<MAX_LWZ_BITS)) {
			data->table[0][code] = data->oldcode;
			data->table[1][code] = data->firstcode;
			++data->max_code;
			
			if ((data->max_code >= data->max_code_size) && (data->max_code_size < (1<<MAX_LWZ_BITS))) {
				data->max_code_size *= 2;
				++data->code_size;
			}
		}

		data->oldcode = incode;

		if (data->sp > data->stack) {
			return *--data->sp;
		}
	}
	return code;
}

static int ReadImage( AnimatedGIFData *data, int left, int top, int width, int height, uint8_t cmap[3][MAXCOLORMAPSIZE], bool interlace, bool ignore )
{
	int v, xpos = 0, ypos = 0, pass = 0;
	uint32_t *image, *dst;
	uint8_t c;

	//  Initialize the decompression routines
	if (FetchData( data->stream, &c, 1 )) {
		JDEBUG(JINFO, "EOF / read error on image data");
	}

	if (LWZReadByte( data, true, c ) < 0) {
		JDEBUG(JINFO, "error reading image");
	}

	// If this is an "uninteresting picture" ignore it.
	if (ignore) {
		JDEBUG(JINFO, "skipping image...");

		while (LWZReadByte(data, false, c) >= 0);

		return 0;
	}

	switch (data->disposal) {
		case 2:
			JDEBUG(JINFO, "restoring to background color...");
			memset( data->image, 0, data->Width * data->Height * 4 );
			break;
		case 3:
			JDEBUG(JINFO, "restoring to previous frame is unsupported");
			break;
		default:
			break;
	}

	dst = image = data->image + (top * data->Width + left);

	JDEBUG(JINFO, "reading %dx%d at %dx%d %sGIF image", width, height, left, top, interlace ? " interlaced " : "" );

	while ((v = LWZReadByte( data, false, c )) >= 0 ) {
		if (v != data->transparent) {
			dst[xpos] = (0xFF000000 | cmap[CM_RED][v] << 16 | cmap[CM_GREEN][v] << 8 | cmap[CM_BLUE][v]);
		}

		++xpos;

		if (xpos == width) {
			xpos = 0;
			if (interlace) {
				switch (pass) {
					case 0:
					case 1:
						ypos += 8;
						break;
					case 2:
						ypos += 4;
						break;
					case 3:
						ypos += 2;
						break;
				}

				if (ypos >= height) {
					++pass;
					switch (pass) {
						case 1:
							ypos = 4;
							break;
						case 2:
							ypos = 2;
							break;
						case 3:
							ypos = 1;
							break;
						default:
							goto fini;
					}
				}
			} else {
				++ypos;
			}

			dst = image + ypos * data->Width;
		}

		if (ypos >= height) {
			break;
		}
	}

fini:
	if (LWZReadByte( data, false, c ) >= 0) {
		JDEBUG(JINFO, "too much input data, ignoring extra...");
		//while (LWZReadByte( data, false, c ) >= 0);
	}

	return 0;
}

static void GIFReset( AnimatedGIFData *data )
{
	data->transparent = -1;
	data->delayTime   = 1000000; // default: 1s
	data->inputFlag   = -1;
	data->disposal    = 0;

	if (data->image) {
		memset(data->image, 0, data->Width*data->Height*4);
	}
}

static int GIFReadHeader( AnimatedGIFData *data )
{
	uint8_t buf[7];
	int ret;

	ret = FetchData( data->stream, buf, 6 );
	if (ret) {
		JDEBUG(JINFO, "error reading header");

		return ret;
	}

	if (memcmp( buf, "GIF", 3 )) {
		JDEBUG(JINFO, "bad magic");

		return -1;
	}

	memcpy( data->Version, &buf[3], 3 );
	data->Version[3] = '\0';

	ret = FetchData( data->stream, buf, 7 );
	if (ret) {
		JDEBUG(JINFO, "error reading screen descriptor");

		return ret;
	}

	data->Width           = LM_to_uint( buf[0], buf[1] );
	data->Height          = LM_to_uint( buf[2], buf[3] );
	data->BitPixel        = 2 << (buf[4] & 0x07);
	data->ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
	data->Background      = buf[5];
	data->AspectRatio     = buf[6];

	if (data->AspectRatio) {
		data->AspectRatio = ((data->AspectRatio + 15) << 8) >> 6;
	} else {
		data->AspectRatio = (data->Width << 8) / data->Height;
	}

	if (BitSet(buf[4], LOCALCOLORMAP)) { // Global Colormap
		if (ReadColorMap( data->stream, data->BitPixel, data->ColorMap )) {
			JDEBUG(JINFO, "error reading global colormap");

			return -1;
		}
	}

	return 0;
}

static int GIFReadFrame(AnimatedGIFData *data)
{
	int top, left, width, height;
	uint8_t localColorMap[3][MAXCOLORMAPSIZE];
	bool useGlobalColormap;
	uint8_t buf[16], c;

	data->curbit = data->lastbit = data->done = data->last_byte = 0;

	data->fresh = data->code_size = 
		data->set_code_size = data->max_code = 
		data->max_code_size = data->firstcode = 
		data->oldcode = data->clear_code = data->end_code = 0;

	for (;;) {
		int ret;

		ret = FetchData( data->stream, &c, 1);
		if (ret) {
			JDEBUG(JINFO, "EOF / read error on image data" );

			return -1;
		}

		if (c == ';') { // GIF terminator
			return -1;
		}

		if (c == '!') { // Extension
			if (FetchData( data->stream, &c, 1)) {
				JDEBUG(JINFO, "EOF / read error on extention function code");

				return -1;
			}

			DoExtension( data, c );

			continue;
		} 

		if (c != ',') { // Not a valid start character
			// JDEBUG(JINFO, "bogus character 0x%02x, ignoring", (int)c);

			continue;
		}

		ret = FetchData(data->stream, buf, 9);
		if (ret) {
			JDEBUG(JINFO, "couldn't read left/top/width/height");

			return ret;
		}

		left = LM_to_uint( buf[0], buf[1] );
		top = LM_to_uint( buf[2], buf[3] );
		width = LM_to_uint( buf[4], buf[5] );
		height = LM_to_uint( buf[6], buf[7] );

		useGlobalColormap = !BitSet( buf[8], LOCALCOLORMAP );

		if (!useGlobalColormap) {
			int bitPixel = 2 << (buf[8] & 0x07);

			if (ReadColorMap( data->stream, bitPixel, localColorMap )) {
				JDEBUG(JINFO, "error reading local colormap");
			}
		}

		if (ReadImage(data, left, top, width, height, (useGlobalColormap?data->ColorMap:localColorMap), BitSet(buf[8], INTERLACE), 0)) {
			JDEBUG(JINFO, "error reading image");

			return -1;
		}

		break;
	}

	return 0;
}

class PlayerComponentImpl : public jgui::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		jgui::Image *_image;
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		jgui::jregion_t _src;
		/** \brief */
		jgui::jregion_t _dst;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		PlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_image = NULL;
			_player = player;
			
			_frame_size.width = w;
			_frame_size.height = h;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			_dst.x = 0;
			_dst.y = 0;
			_dst.width = w;
			_dst.height = h;

			SetVisible(true);
		}

		virtual ~PlayerComponentImpl()
		{
			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_mutex.Unlock();
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(uint32_t *data)
		{
#if defined(DIRECTFB_NODEPS_UI)
			IDirectFBSurface *frame;
			DFBSurfaceDescription desc;
			int sw = _frame_size.width;
			int sh = _frame_size.height;

			desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_PREALLOCATED);
			desc.caps = (DFBSurfaceCapabilities)(DSCAPS_NONE);
			desc.width = sw;
			desc.height = sh;
			desc.pixelformat = DSPF_ARGB;
			desc.preallocated[0].data = (uint8_t *)data;
			desc.preallocated[0].pitch = sw*4;

			IDirectFB *directfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

			if (directfb->CreateSurface(directfb, &desc, &frame) == DFB_OK) {
				_mutex.Lock();

				if (_image != NULL) {
					delete _image;
					_image = NULL;
				}

				_image = new jgui::NativeImage(frame, jgui::JPF_ARGB, sw, sh);

				_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, _image));

				_mutex.Unlock();

				Run();
			}
#else
			int sw = _frame_size.width;
			int sh = _frame_size.height;

			cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
					(uint8_t *)data, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
			cairo_t *cairo_context = cairo_create(cairo_surface);

			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_image = new jgui::NativeImage(cairo_context, jgui::JPF_ARGB, sw, sh);

			_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, _image));

			cairo_surface_flush(cairo_surface);
			cairo_surface_destroy(cairo_surface);

			_mutex.Unlock();

			Run();
#endif
		}

		virtual void Run()
		{
			if (IsVisible() != false) {
				Repaint();
			}
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

			_mutex.Lock();

			g->DrawImage(_image, _src.x, _src.y, _src.width, _src.height, 0, 0, _size.width, _size.height);
				
			_mutex.Unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		GIFLightPlayer *_player;

	public:
		VideoSizeControlImpl(GIFLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			PlayerComponentImpl *impl = dynamic_cast<PlayerComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			PlayerComponentImpl *impl = dynamic_cast<PlayerComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<PlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<PlayerComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

}

GIFLightPlayer::GIFLightPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_is_playing = false;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	_provider = NULL;
	
	jio::FileInputStream *stream = new jio::FileInputStream(file);

	giflightplayer::AnimatedGIFData *data = new giflightplayer::AnimatedGIFData;

	data->thread = NULL;
	data->stream = stream;
	data->image = NULL;
	data->Width = -1;
	data->Height = -1;
	data->BitPixel = 0;
	data->ColorResolution = 0;
	data->Background = 0;
	data->AspectRatio = 0;
	data->transparent = 0;
	data->delayTime = 0;
	data->inputFlag = 0;
	data->disposal = 0;
	data->curbit = 0;
	data->lastbit = 0;
	data->done = 0;
	data->last_byte = 0;
	data->fresh = 0;
	data->code_size = 0;
	data->set_code_size = 0;
	data->max_code = 0;
	data->max_code_size = 0;
	data->firstcode = 0;
	data->oldcode = 0;
	data->clear_code = 0;
	data->end_code = 0;
	data->ZeroDataBlock = 0;

	GIFReset(data);

	if (GIFReadHeader(data) != 0) {
		delete data;

		throw MediaException("Unable to process gif header");
	}

	data->image = new uint32_t[data->Width*data->Height];

	_controls.push_back(new giflightplayer::VideoSizeControlImpl(this));

	_component = new giflightplayer::PlayerComponentImpl(this, 0, 0, data->Width, data->Height);
	
	_provider = data;
}

GIFLightPlayer::~GIFLightPlayer()
{
	Close();
	
	delete _component;
	_component = NULL;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();

	giflightplayer::AnimatedGIFData *data = (giflightplayer::AnimatedGIFData *)_provider;

	delete data;
}

void GIFLightPlayer::Run()
{
	giflightplayer::AnimatedGIFData *data = (giflightplayer::AnimatedGIFData *)_provider;

	while (_is_playing == true) {
		bool skip = false;

		data->lock.Lock();

		if (GIFReadFrame(data) != 0) { 
			GIFReset(data);

			if (_is_loop == true) {
				skip = true;

				data->stream->Reset();
				
				if (GIFReadHeader(data) != 0) {
					break;
				}
			} else {
				DispatchPlayerEvent(new PlayerEvent(this, JPE_FINISHED));

				data->lock.Unlock();

				break;
			}
		}

		if (skip == false) {
			dynamic_cast<giflightplayer::PlayerComponentImpl *>(_component)->UpdateComponent(data->image);

			try {
				if (_decode_rate == 0) {
					data->cond.Wait(&data->lock);
				} else {
					uint64_t us;

					us = data->delayTime;

					if (_decode_rate != 1.0) {
						us = ((double)us / _decode_rate + 0.5);
					}

					data->cond.Wait(us, &data->lock);
				}
			} catch (jthread::SemaphoreTimeoutException &e) {
			}
		}

		data->lock.Unlock();
	}
}

void GIFLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		if (IsRunning() == false) {
			_is_playing = true;

			Start();
		}
	}
}

void GIFLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void GIFLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;
		
		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void GIFLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	_is_playing = false;

	if (IsRunning() == true) {
		WaitThread();
	}

	if (_has_video == true) {
		_component->Repaint();
	}

	_is_paused = false;
}

void GIFLightPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_playing = false;
	_is_closed = true;
	
	giflightplayer::AnimatedGIFData *data = (giflightplayer::AnimatedGIFData *)_provider;

	data->cond.Notify();

	if (IsRunning() == true) {
		WaitThread();
	}

	if (data->stream != NULL) {
		data->stream->Close();
		delete data->stream;
	}

	if (data->image != NULL) {
		delete [] data->image;
	}

	delete data;

	_provider = NULL;
}

void GIFLightPlayer::SetCurrentTime(uint64_t time)
{
}

uint64_t GIFLightPlayer::GetCurrentTime()
{
	return -1LL;
}

uint64_t GIFLightPlayer::GetMediaTime()
{
	return -1LL;
}

void GIFLightPlayer::SetLoop(bool b)
{
	jthread::AutoLock lock(&_mutex);

	_is_loop = b;
}

bool GIFLightPlayer::IsLoop()
{
	return _is_loop;
}

void GIFLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		giflightplayer::AnimatedGIFData *data = (giflightplayer::AnimatedGIFData *)_provider;
		
		_is_paused = false;
			
		data->cond.Notify();
	}
}

double GIFLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	return _decode_rate;
}

jgui::Component * GIFLightPlayer::GetVisualComponent()
{
	return _component;
}

}

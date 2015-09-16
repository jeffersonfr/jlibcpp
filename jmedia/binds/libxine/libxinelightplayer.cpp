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
#include "Stdafx.h"
#include "libxinelightplayer.h"
#include "nativeimage.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvideodevicecontrol.h"
#include "jvolumecontrol.h"
#include "jmediaexception.h"
#include "jgfxhandler.h"
#include "jcolorconversion.h"

#if defined(DIRECTFB_NODEPS_UI)
#include <directfb.h>
#else
#include <cairo.h>
#endif

namespace jmedia {

namespace libxinelightplayer {

class LibXineLightComponentImpl : public jgui::Component, jthread::Thread {

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
		uint32_t *_buffer;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		LibXineLightComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_buffer = NULL;
			_image = NULL;
			_player = player;
			
			_frame_size.width = w;
			_frame_size.height = h;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			SetVisible(true);
		}

		virtual ~LibXineLightComponentImpl()
		{
			if (IsRunning() == true) {
				WaitThread();
			}

			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_mutex.Unlock();

			if (_buffer != NULL) {
				delete [] _buffer;
				_buffer = NULL;
			}
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(int format, int width, int height, double aspect, void *data0, void *data1, void *data2)
		{
			if (width <= 0 || height <= 0) {
				return;
			}

			if (_buffer == NULL) {
				_frame_size.width = _src.width = width;
				_frame_size.height = _src.height = height;

				_buffer = new uint32_t[width*height];
			}

			if (format == XINE_VORAW_YV12) {
				ColorConversion::GetRGB32FromYV12((uint8_t **)&data0, (uint8_t **)&data1, (uint8_t **)&data2, (uint32_t **)&_buffer, width, height);
			} else if (format == XINE_VORAW_YUY2) {
				ColorConversion::GetRGB32FromYUYV((uint8_t **)&data0, (uint32_t **)&_buffer, width, height);
			} else if (format == XINE_VORAW_RGB) {
				ColorConversion::GetRGB32FromRGB24((uint8_t **)&data0, (uint32_t **)&_buffer, width, height);
			} 

#if defined(DIRECTFB_NODEPS_UI)
			if (IsRunning() == true) {
				WaitThread();
			}

			int sw = width;
			int sh = height;

			IDirectFBSurface *frame;
			DFBSurfaceDescription desc;

			desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_PREALLOCATED);
			desc.caps = (DFBSurfaceCapabilities)(DSCAPS_NONE);
			desc.width = sw;
			desc.height = sh;
			desc.pixelformat = DSPF_ARGB;
			desc.preallocated[0].data = _buffer;
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

				Start();
			}
#else
			if (IsRunning() == true) {
				WaitThread();
			}

			int sw = width;
			int sh = height;

			cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
					(uint8_t *)_buffer, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
			cairo_t *cairo_context = cairo_create(cairo_surface);

			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_image = new jgui::NativeImage(cairo_context, jgui::JPF_RGB24, sw, sh);

			_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, _image));

			cairo_surface_flush(cairo_surface);
			cairo_surface_destroy(cairo_surface);

			_mutex.Unlock();

			Start();
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

class VolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		LibXineLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		VolumeControlImpl(LibXineLightPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;
			_is_muted = false;
		}

		virtual ~VolumeControlImpl()
		{
		}

		virtual int GetLevel()
		{
			jthread::AutoLock lock(&_player->_mutex);

			int level = 0;

			if (_player->_stream != NULL) {
				level = xine_get_param(_player->_stream, XINE_PARAM_AUDIO_VOLUME);
				
				// int amp = xine_get_param(_player->_stream, XINE_PARAM_AUDIO_AMP_LEVEL);

				// level = (int)(100.0*((float)level/100.0 * (float)amp/100.0));
			}

			return level;
		}

		virtual void SetLevel(int level)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_stream != NULL) {
				_level = (level < 0)?0:(level > 100)?100:level;

				if (_level != 0) {
					_is_muted = true;
				} else {
					_is_muted = false;
				}

				/*
				level = (level < 0)?0:(level > 200)?200:level;

				int vol, amp;

				if (level > 100) {
					vol = 100;
					amp = level;
				} else {
					vol = level;
					amp = 100;
				}

				xine_set_param( data->stream, XINE_PARAM_AUDIO_VOLUME, vol );
				xine_set_param( data->stream, XINE_PARAM_AUDIO_AMP_LEVEL, amp );
				*/

				xine_set_param(_player->_stream, XINE_PARAM_AUDIO_VOLUME, level);
				xine_set_param(_player->_stream, XINE_PARAM_AUDIO_MUTE, (_is_muted == true)?1:0);
			}
		}
		
		virtual bool IsMute()
		{
			return _is_muted;
		}

		virtual void SetMute(bool b)
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			_is_muted = b;
			
			if (_player->_stream != NULL) {
				xine_set_param(_player->_stream, XINE_PARAM_AUDIO_MUTE, (_is_muted == true)?1:0);
			}
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		LibXineLightPlayer *_player;

	public:
		VideoSizeControlImpl(LibXineLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			LibXineLightComponentImpl *impl = dynamic_cast<LibXineLightComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			LibXineLightComponentImpl *impl = dynamic_cast<LibXineLightComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<LibXineLightComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<LibXineLightComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		LibXineLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		VideoFormatControlImpl(LibXineLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~VideoFormatControlImpl()
		{
		}

		virtual void SetContentMode(jvideo_mode_t t)
		{
			_video_mode = t;
		}

		virtual void SetVideoFormatHD(jhd_video_format_t vf)
		{
			_hd_video_format = vf;
		}

		virtual void SetVideoFormatSD(jsd_video_format_t vf)
		{
			_sd_video_format = vf;
		}

		virtual jaspect_ratio_t GetAspectRatio()
		{
			jthread::AutoLock lock(&_player->_mutex);

			double aspect = _player->_aspect;

			if (aspect == (1.0/1.0)) {
				return LAR_1x1;
			} else if (aspect == (3.0/2.0)) {
				return LAR_3x2;
			} else if (aspect == (4.0/3.0)) {
				return LAR_4x3;
			} else if (aspect == (16.0/9.0)) {
				return LAR_16x9;
			}

			return LAR_16x9;
		}

		virtual double GetFramesPerSecond()
		{
			jthread::AutoLock lock(&_player->_mutex);

			return _player->_frames_per_second;
		}

		virtual jvideo_mode_t GetContentMode()
		{
			return LVM_FULL;
		}

		virtual jhd_video_format_t GetVideoFormatHD()
		{
			return LHVF_1080p;
		}

		virtual jsd_video_format_t GetVideoFormatSD()
		{
			return LSVF_PAL_M;
		}

};

class VideoDeviceControlImpl : public VideoDeviceControl {
	
	private:
		LibXineLightPlayer *_player;
		std::map<jvideo_control_t, int> _default_values;

	public:
		VideoDeviceControlImpl(LibXineLightPlayer *player):
			VideoDeviceControl()
		{
			_player = player;

			_controls.push_back(JVC_CONTRAST);
			_controls.push_back(JVC_SATURATION);
			_controls.push_back(JVC_HUE);
			_controls.push_back(JVC_BRIGHTNESS);
			_controls.push_back(JVC_GAMMA);
		
			for (std::vector<jvideo_control_t>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
				_default_values[*i] = GetValue(*i);
			}
		}

		virtual ~VideoDeviceControlImpl()
		{
		}

		virtual int GetValue(jvideo_control_t id)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_stream != NULL) {
				if (HasControl(id) == true) {
					int control = -1;

					if (id == JVC_CONTRAST) {
						control = XINE_PARAM_VO_CONTRAST;
					} else if (id == JVC_SATURATION) {
						control = XINE_PARAM_VO_SATURATION;
					} else if (id == JVC_HUE) {
						control = XINE_PARAM_VO_HUE;
					} else if (id == JVC_BRIGHTNESS) {
						control = XINE_PARAM_VO_BRIGHTNESS;
					} else if (id == JVC_GAMMA) {
						control = XINE_PARAM_VO_GAMMA;
					}
					
					return xine_get_param(_player->_stream, control);
				}
			}
				
			return 0;
		}

		virtual bool SetValue(jvideo_control_t id, int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_stream != NULL) {
				if (HasControl(id) == true) {
					int control = -1;

					if (id == JVC_CONTRAST) {
						control = XINE_PARAM_VO_CONTRAST;
					} else if (id == JVC_SATURATION) {
						control = XINE_PARAM_VO_SATURATION;
					} else if (id == JVC_HUE) {
						control = XINE_PARAM_VO_HUE;
					} else if (id == JVC_BRIGHTNESS) {
						control = XINE_PARAM_VO_BRIGHTNESS;
					} else if (id == JVC_GAMMA) {
						control = XINE_PARAM_VO_GAMMA;
					}
					
					xine_set_param(_player->_stream, control, value);

					return true;
				}
			}

			return false;
		}

		virtual void Reset(jvideo_control_t id)
		{
			SetValue(id, _default_values[id]);
		}

};

static void render_callback(void *data, int format, int width, int height, double aspect, void *data0, void *data1, void *data2)
{
	reinterpret_cast<LibXineLightComponentImpl *>(data)->UpdateComponent(format, width, height, aspect, data0, data1, data2);
}

static void overlay_callback(void *user_data, int num_ovl, raw_overlay_t *overlays_array)
{
}

static void events_callback(void *data, const xine_event_t *event) 
{
	LibXineLightPlayer *player = reinterpret_cast<LibXineLightPlayer *>(data);

	if (event->type == XINE_EVENT_UI_PLAYBACK_FINISHED) {
		player->DispatchPlayerEvent(new PlayerEvent(player, JPE_FINISHED));

		if (player->IsLoop() == true) {
			player->Play();
		} else {
			player->Stop();
		}
  }
}
  
}

LibXineLightPlayer::LibXineLightPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_is_loop = false;
	_decode_rate = 1.0;
	_frames_per_second = 0.0;
	
	_component = new libxinelightplayer::LibXineLightComponentImpl(this, 0, 0, -1, -1);

	raw_visual_t t;

	memset(&t, 0, sizeof(t));

	t.supported_formats = (int)(XINE_VORAW_YV12 | XINE_VORAW_YUY2 | XINE_VORAW_RGB);
	t.raw_output_cb = libxinelightplayer::render_callback;
	t.raw_overlay_cb = libxinelightplayer::overlay_callback;
	t.user_data = _component;

  _xine = xine_new();
	
	char configfile[2048];
	sprintf(configfile, "%s%s", xine_get_homedir(), "/.xine/config");
	xine_config_load(_xine, configfile);
  
	xine_init(_xine);
 
	_post = NULL;
  _ao_port = xine_open_audio_driver(_xine, "auto", NULL);
  _vo_port = xine_open_video_driver(_xine , "auto", XINE_VISUAL_TYPE_RAW, &t);
  _stream = xine_stream_new(_xine, _ao_port, _vo_port);
  _event_queue = xine_event_new_queue(_stream);
  
	xine_event_create_listener_thread(_event_queue, libxinelightplayer::events_callback, this);

  if (xine_open(_stream, _file.c_str()) == 0) {
		xine_close(_stream);
		xine_event_dispose_queue(_event_queue);
		xine_dispose(_stream);
		xine_close_audio_driver(_xine, _ao_port);  
		xine_close_video_driver(_xine, _vo_port);  
		xine_exit(_xine);

		throw jmedia::MediaException("Unable to open the media file");
  }

	xine_set_param(_stream, XINE_PARAM_VERBOSITY, 1);
	xine_set_param(_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, -1);

	_media_info.title = std::string(xine_get_meta_info(_stream, XINE_META_INFO_TITLE)?:"");
	_media_info.author = std::string(xine_get_meta_info(_stream, XINE_META_INFO_ARTIST) ?:"");
	_media_info.album = std::string(xine_get_meta_info(_stream, XINE_META_INFO_ALBUM)?:"");
	_media_info.genre = std::string(xine_get_meta_info(_stream, XINE_META_INFO_GENRE)?:"");
	_media_info.comments = std::string(xine_get_meta_info(_stream, XINE_META_INFO_COMMENT)?:"");
	_media_info.date = std::string(xine_get_meta_info(_stream, XINE_META_INFO_YEAR)?:"");

	if (xine_get_stream_info(_stream, XINE_STREAM_INFO_HAS_AUDIO)) {
		_has_audio = true;

		_controls.push_back(new libxinelightplayer::VolumeControlImpl(this));
	}

	if (xine_get_stream_info(_stream, XINE_STREAM_INFO_HAS_VIDEO)) {
		_has_video = true;
		_aspect = xine_get_stream_info(_stream, XINE_STREAM_INFO_VIDEO_RATIO)/10000.0;
		_frames_per_second = xine_get_stream_info(_stream, XINE_STREAM_INFO_FRAME_DURATION);

		if (_frames_per_second) {
			_frames_per_second = 90000.0/_frames_per_second;
		}

		_controls.push_back(new libxinelightplayer::VideoSizeControlImpl(this));
		_controls.push_back(new libxinelightplayer::VideoFormatControlImpl(this));
		_controls.push_back(new libxinelightplayer::VideoDeviceControlImpl(this));
	}

	if (_has_video == false && _has_audio == true) {
		const char *const *post_list;
		const char *post_plugin;
		xine_post_out_t *audio_source;

		post_list = xine_list_post_plugins_typed(_xine, XINE_POST_TYPE_AUDIO_VISUALIZATION);
		post_plugin = xine_config_register_string(_xine, "gui.post_audio_plugin", post_list[0], "Audio visualization plugin", NULL, 0, NULL, NULL);
		_post = xine_post_init(_xine, post_plugin, 0, &_ao_port, &_vo_port);

		if (_post) {
			audio_source = xine_get_audio_source(_stream);
			xine_post_wire_audio_port(audio_source, _post->audio_input[0]);
		}
	}
}

LibXineLightPlayer::~LibXineLightPlayer()
{
	Close();
	
	delete _component;
	_component = NULL;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();
}

void LibXineLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _stream != NULL) {
		int speed = xine_get_param(_stream, XINE_PARAM_SPEED);

		xine_play(_stream, 0, 0);
		xine_set_param(_stream, XINE_PARAM_SPEED, speed);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_STARTED));
	}
}

void LibXineLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void LibXineLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;
		
		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void LibXineLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	if (_stream != NULL) {
		xine_stop(_stream);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void LibXineLightPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_stream != NULL) {
		xine_close(_stream);
		xine_event_dispose_queue(_event_queue);
		xine_dispose(_stream);
		xine_close_audio_driver(_xine, _ao_port);  
		xine_close_video_driver(_xine, _vo_port);  
		
		if (_post != NULL) {
			xine_post_dispose(_xine, _post);
		}

		xine_exit(_xine);
	}
}

void LibXineLightPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_stream != NULL) {
		if (xine_get_stream_info(_stream, XINE_STREAM_INFO_SEEKABLE) == 0) {
			return;
		}

		int speed = xine_get_param(_stream, XINE_PARAM_SPEED);

		xine_play(_stream, 0, time);

		xine_set_param(_stream, XINE_PARAM_SPEED, speed);

		usleep(100);
	}
}

uint64_t LibXineLightPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	uint64_t time = 0LL;

	if (_stream != NULL) {
		int pos = 0;

		for (int i=0; i<5; i++) {
			if (xine_get_pos_length(_stream, NULL, &pos, NULL)) {
				break;
			}

			usleep(1000);
		}

		time = pos;
	}

	return time;
}

uint64_t LibXineLightPlayer::GetMediaTime()
{
	uint64_t time = 0LL;

	if (_stream != NULL) {
		int length = 0;

		xine_get_pos_length(_stream, NULL, NULL, &length);

		time = (uint64_t)length;
	}

	return time;
}

void LibXineLightPlayer::SetLoop(bool b)
{
	jthread::AutoLock lock(&_mutex);

	_is_loop = b;
}

bool LibXineLightPlayer::IsLoop()
{
	return _is_loop;
}

void LibXineLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	if (_stream != NULL) {
		xine_set_param(_stream, XINE_PARAM_SPEED, rate*XINE_SPEED_NORMAL+0.5);
	}
}

double LibXineLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_stream != NULL) {
		int speed;

		speed = xine_get_param(_stream, XINE_PARAM_SPEED);

		rate = (double)speed/ (double)XINE_SPEED_NORMAL;
	}

	return rate;
}

jgui::Component * LibXineLightPlayer::GetVisualComponent()
{
	return _component;
}

}

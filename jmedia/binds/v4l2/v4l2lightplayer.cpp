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
#include "jmedia/binds/v4l2/include/v4l2lightplayer.h"
#include "jmedia/binds/v4l2/include/videocontrol.h"
#include "jmedia/binds/v4l2/include/videograbber.h"
#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvideodevicecontrol.h"
#include "jmedia/jcolorconversion.h"
#include "jgui/jbufferedimage.h"
#include "jexception/jmediaexception.h"
#include "jexception/jcontrolexception.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <cairo.h>

namespace jmedia {

class V4l2PlayerComponentImpl : public jgui::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		jgui::Image *_image;
		/** \brief */
    std::mutex  _mutex;
		/** \brief */
		jgui::jregion_t _src;
		/** \brief */
		uint32_t *_buffer;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		V4l2PlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_buffer = nullptr;
			_image = nullptr;
			_player = player;
			
			_frame_size.width = w;
			_frame_size.height = h;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			SetVisible(true);
		}

		virtual ~V4l2PlayerComponentImpl()
		{
			if (_image != nullptr) {
				delete _image;
				_image = nullptr;
			}

			if (_buffer != nullptr) {
				delete [] _buffer;
				_buffer = nullptr;
			}
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void Reset()
		{
			_frame_size.width = _src.width = -1;
			_frame_size.height = _src.height = -1;

			delete [] _buffer;
			_buffer = nullptr;
		}

		virtual void UpdateComponent(const uint8_t *buffer, int width, int height, jgui::jpixelformat_t format)
		{
			if (width <= 0 || height <= 0) {
				return;
			}

			if (_buffer == nullptr) {
				_frame_size.width = _src.width = width;
				_frame_size.height = _src.height = height;

				_buffer = new uint32_t[width*height];
			}

			if (format == jgui::JPF_UYVY) {
				ColorConversion::GetRGB32FromYUYV((uint8_t **)&buffer, (uint32_t **)&_buffer, width, height);
			} else if (format == jgui::JPF_RGB24) {
				ColorConversion::GetRGB32FromRGB24((uint8_t **)&buffer, (uint32_t **)&_buffer, width, height);
			} else if (format == jgui::JPF_RGB32) {
				memcpy(_buffer, buffer, width*height*4);
			}

			int sw = width;
			int sh = height;

			cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
					(uint8_t *)_buffer, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
			cairo_t *cairo_context = cairo_create(cairo_surface);

			_mutex.lock();

			if (_image != nullptr) {
				delete _image;
				_image = nullptr;
			}

			_image = new jgui::BufferedImage(cairo_context);

			_player->DispatchFrameGrabberEvent(new jevent::FrameGrabberEvent(_image, jevent::JFE_GRABBED));

			cairo_surface_flush(cairo_surface);
			cairo_surface_destroy(cairo_surface);

			_mutex.unlock();

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

      jgui::jsize_t
        size = GetSize();

			_mutex.lock();

			g->DrawImage(_image, _src.x, _src.y, _src.width, _src.height, 0, 0, size.width, size.height);
				
			_mutex.unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class V4l2VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		V4L2LightPlayer *_player;

	public:
		V4l2VideoSizeControlImpl(V4L2LightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~V4l2VideoSizeControlImpl()
		{
		}

		virtual void SetSize(int w, int h)
		{
			V4l2PlayerComponentImpl 
        *impl = dynamic_cast<V4l2PlayerComponentImpl *>(_player->_component);
			VideoGrabber 
        *grabber = _player->_grabber;

      impl->_mutex.lock();

			grabber->Stop();
			impl->Reset();
			grabber->Open();
			grabber->Configure(w, h);
			grabber->GetVideoControl()->Reset();
      
      impl->_mutex.unlock();
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			V4l2PlayerComponentImpl *impl = dynamic_cast<V4l2PlayerComponentImpl *>(_player->_component);

      impl->_mutex.lock();
			
			/*
			VideoGrabber *grabber = _player->_grabber;

			grabber->Stop();

			impl->Reset();

			grabber->Open();
			grabber->Configure(w, h);
			grabber->GetVideoControl()->Reset();
			*/
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = -1;
			impl->_src.height = -1;

      impl->_mutex.unlock();
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			V4l2PlayerComponentImpl *impl = dynamic_cast<V4l2PlayerComponentImpl *>(_player->_component);

      impl->_mutex.lock();

			impl->SetBounds(x, y, w, h);
      
      impl->_mutex.unlock();
		}

		virtual jgui::jsize_t GetSize()
		{
			return dynamic_cast<V4l2PlayerComponentImpl *>(_player->_component)->GetPreferredSize();
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<V4l2PlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<V4l2PlayerComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class V4l2VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		V4L2LightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		V4l2VideoFormatControlImpl(V4L2LightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~V4l2VideoFormatControlImpl()
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
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				return control->GetFramesPerSecond();
			}

			return 0.0;
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

class V4l2VideoDeviceControlImpl : public VideoDeviceControl {
	
	private:
		V4L2LightPlayer *_player;

	public:
		V4l2VideoDeviceControlImpl(V4L2LightPlayer *player):
			VideoDeviceControl()
		{
			_player = player;

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();
				std::vector<jmedia::jvideo_control_t> controls = control->GetControls();

				for (std::vector<jmedia::jvideo_control_t>::iterator i=controls.begin(); i!=controls.end(); i++) {
					_controls.push_back(*i);
				}
			}
		}

		virtual ~V4l2VideoDeviceControlImpl()
		{
		}

		virtual int GetValue(jvideo_control_t id)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(id) == true) {
					return control->GetValue(id);
				}
			}

			return 0;
		}

		virtual bool SetValue(jvideo_control_t id, int value)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(id) == true) {
					control->SetValue(id, value);

					return true;
				}
			}

			return false;
		}

		virtual void Reset(jvideo_control_t id)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

			if (_player->_grabber != nullptr) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(id) == true) {
					control->Reset(id);
				}
			}
		}

};

V4L2LightPlayer::V4L2LightPlayer(jnetwork::URL url):
	jmedia::Player()
{
	_file = url.GetPath();
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = true;
	_aspect = 1.0;
	_media_time = 0LL;
	_is_loop = false;
	_decode_rate = 1.0;
	_frame_rate = 0.0;
	_component = nullptr;
	
  if (_file.empty() == true) {
    _file = "/dev/video0";
  }

  _grabber = new VideoGrabber(this, _file);

	jgui::jsize_t size;
	
	size.width = 640;
	size.height = 480;

	_grabber->Open();
	_grabber->Configure(size.width, size.height);
	_grabber->GetVideoControl()->Reset();

	_controls.push_back(new V4l2VideoSizeControlImpl(this));
	_controls.push_back(new V4l2VideoFormatControlImpl(this));
	_controls.push_back(new V4l2VideoDeviceControlImpl(this));
	
	_component = new V4l2PlayerComponentImpl(this, 0, 0, -1, -1);
}

V4L2LightPlayer::~V4L2LightPlayer()
{
	Close();
	
	delete _component;
	_component = nullptr;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();
}

void V4L2LightPlayer::ProcessFrame(const uint8_t *buffer, int width, int height, jgui::jpixelformat_t format)
{
	dynamic_cast<V4l2PlayerComponentImpl *>(_component)->UpdateComponent(buffer, width, height, format);
}

void V4L2LightPlayer::Play()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _grabber != nullptr) {
		_grabber->Start();
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_STARTED));
	}
}

void V4L2LightPlayer::Pause()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _grabber != nullptr) {
		_is_paused = true;
		
		_grabber->Pause();
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_PAUSED));
	}
}

void V4L2LightPlayer::Resume()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == true && _grabber != nullptr) {
		_is_paused = false;
		
		_grabber->Resume();
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_RESUMED));
	}
}

void V4L2LightPlayer::Stop()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_grabber != nullptr) {
		_grabber->Stop();

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void V4L2LightPlayer::Close()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_grabber != nullptr) {
		delete _grabber;
		_grabber = nullptr;
	}
}

void V4L2LightPlayer::SetCurrentTime(uint64_t time)
{
}

uint64_t V4L2LightPlayer::GetCurrentTime()
{
	return 0LL;
}

uint64_t V4L2LightPlayer::GetMediaTime()
{
	return 0LL;
}

void V4L2LightPlayer::SetLoop(bool b)
{
}

bool V4L2LightPlayer::IsLoop()
{
	return false;
}

void V4L2LightPlayer::SetDecodeRate(double rate)
{
}

double V4L2LightPlayer::GetDecodeRate()
{
	return 0.0;
}

jgui::Component * V4L2LightPlayer::GetVisualComponent()
{
	return _component;
}

}


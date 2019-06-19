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
#include "ilistlightplayer.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvolumecontrol.h"
#include "jmedia/jaudioconfigurationcontrol.h"
#include "jgui/jbufferedimage.h"
#include "jio/jinputstream.h"
#include "jio/jfile.h"
#include "jio/jfileinputstream.h"
#include "jexception/jcontrolexception.h"
#include "jexception/jmediaexception.h"
#include "jexception/jsemaphoretimeoutexception.h"

#include <algorithm>
#include <iostream>

#include <cairo.h>

namespace jmedia {

class IlistPlayerComponentImpl : public jgui::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		jgui::Image *_image;
		/** \brief */
    std::mutex _mutex;
		/** \brief */
		jgui::jrect_t<int> _src;
		/** \brief */
		jgui::jrect_t<int> _dst;
		/** \brief */
		jgui::jsize_t<int> _frame_size;

	public:
		IlistPlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_image = nullptr;
			_player = player;
			
			_frame_size.width = w;
			_frame_size.height = h;

			_src = {
        0, 0, w, h
      };

			_dst = {
        0, 0, w, h
      };

			SetVisible(true);
		}

		virtual ~IlistPlayerComponentImpl()
		{
			if (_image != nullptr) {
				delete _image;
				_image = nullptr;
			}
		}

		virtual jgui::jsize_t<int> GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(jgui::Image *frame)
		{
			jgui::jsize_t<int> isize = frame->GetSize();

			if (_frame_size.width != isize.width || _frame_size.height != isize.height) {
				if (_frame_size.width < 0 || _frame_size.height < 0) {
					_src.size = isize;
				}

				_frame_size = isize;
			}

			_player->DispatchFrameGrabberEvent(new jevent::FrameGrabberEvent(frame, jevent::JFE_GRABBED));

			_mutex.lock();

			_image = frame;

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

      jgui::jsize_t<int>
        size = GetSize();

      if (_src.point.x == 0 and _src.point.y == 0 and _src.size.width == _frame_size.width and _src.size.height == _frame_size.height) {
			  g->DrawImage(_image, {0, 0, size.width, size.height});
      } else {
			  g->DrawImage(_image, _src, {0, 0, size.width, size.height});
      }
				
      delete _image;
      _image = nullptr;

			_mutex.unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class IlistVideoSizeControlImpl : public VideoSizeControl {
	
	private:
		ImageListLightPlayer *_player;

	public:
		IlistVideoSizeControlImpl(ImageListLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~IlistVideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			IlistPlayerComponentImpl *impl = dynamic_cast<IlistPlayerComponentImpl *>(_player->_component);

      impl->_mutex.lock();

			impl->_src = {
        x, y, w, h
      };

      impl->_mutex.unlock();
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			IlistPlayerComponentImpl *impl = dynamic_cast<IlistPlayerComponentImpl *>(_player->_component);

      impl->_mutex.lock();

			impl->SetBounds(x, y, w, h);
      
      impl->_mutex.unlock();
		}

		virtual jgui::jrect_t<int> GetSource()
		{
			return dynamic_cast<IlistPlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jrect_t<int> GetDestination()
		{
			return dynamic_cast<IlistPlayerComponentImpl *>(_player->_component)->GetBounds();
		}

};

ImageListLightPlayer::ImageListLightPlayer(jnetwork::URL url):
	jmedia::Player()
{
	_directory = url.GetPath();
	_is_paused = false;
	_is_playing = false;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	_frame_index = 0;
	
	jio::File *file = jio::File::OpenDirectory(_directory);

	if (file == nullptr) {
		throw jexception::RuntimeException("Unable to open the media directory");
	}

	std::vector<std::string> files;

	if (file->ListFiles(&files) == false) {
		throw jexception::RuntimeException("Unable to list files in the local directory");
	}

	if (files.size() == 0) {
		throw jexception::RuntimeException("There is no file in the directory");
	}

	for (std::vector<std::string>::iterator i=files.begin(); i!=files.end(); i++) {
		std::string file = (*i);

		if (file.size() > 3 && file[0] != '.') {
		  _image_list.push_back(_directory + "/" + file);

      /*
			jio::File *tmp = jio::File::OpenFile(_directory + "/" + file);

			if (tmp != nullptr) {
				_image_list.push_back(_directory + "/" + file);
			}

			delete tmp;
      */
		}
	}

	delete file;

	std::sort(_image_list.begin(), _image_list.end(), [](std::string a, std::string b) {
    if (a < b) {
      return true;
    }

    return false;
  });

	_controls.push_back(new IlistVideoSizeControlImpl(this));

	_component = new IlistPlayerComponentImpl(this, 0, 0, -1, -1);
}

ImageListLightPlayer::~ImageListLightPlayer()
{
	Close();
	
	delete _component;
	_component = nullptr;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();
	
  // TODO:: delete images of list
}

void ImageListLightPlayer::ResetFrames()
{
	_frame_index = 0;
}

jgui::Image * ImageListLightPlayer::GetFrame()
{
  std::string file = _image_list[_frame_index++];

  if (_frame_index >= (int)_image_list.size()) {
    _frame_index = 0;
  }

  return new jgui::BufferedImage(file);
}

void ImageListLightPlayer::Run()
{
	jgui::Image *frame;

	while (_is_playing == true) {
    std::unique_lock<std::mutex> lock(_mutex);

		frame = GetFrame();

		if (frame == nullptr) { 
			ResetFrames();

			if (_is_loop == false) {
				DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_FINISHED));

				break;
			}
		}

		dynamic_cast<IlistPlayerComponentImpl *>(_component)->UpdateComponent(frame);

		try {
			if (_decode_rate == 0) {
				_condition.wait(lock);
			} else {
				uint64_t us;

				us = 1000000; // 1.0 frame/sec

				if (_decode_rate != 1.0) {
					us = ((double)us / _decode_rate + 0.);
				}

        printf("::: %ld\n", us);
        _condition.wait_for(lock, std::chrono::microseconds(us));
			}
		} catch (jexception::SemaphoreTimeoutException &e) {
		}
	}
}

void ImageListLightPlayer::Play()
{
  _mutex.lock();

	if (_is_paused == false) {
		if (_is_playing == false) {
			_is_playing = true;

      _thread = std::thread(&ImageListLightPlayer::Run, this);
		}
	}
  
  _mutex.unlock();
}

void ImageListLightPlayer::Pause()
{
  _mutex.lock();

	if (_is_paused == false) {
		_is_paused = true;
		
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_PAUSED));
	}
  
  _mutex.unlock();
}

void ImageListLightPlayer::Resume()
{
  _mutex.lock();

	if (_is_paused == true) {
		_is_paused = false;
		
		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_RESUMED));
	}
  
  _mutex.unlock();
}

void ImageListLightPlayer::Stop()
{
  if (_is_playing == true) {
    _is_playing = false;

    _thread.join();
  }

	if (_has_video == true) {
		_component->Repaint();
	}

	_is_paused = false;
}

void ImageListLightPlayer::Close()
{
  Stop();

	_condition.notify_all();

	_is_closed = true;
}

void ImageListLightPlayer::SetCurrentTime(uint64_t time)
{
}

uint64_t ImageListLightPlayer::GetCurrentTime()
{
	return -1LL;
}

uint64_t ImageListLightPlayer::GetMediaTime()
{
	return -1LL;
}

void ImageListLightPlayer::SetLoop(bool b)
{
	_is_loop = b;
}

bool ImageListLightPlayer::IsLoop()
{
	return _is_loop;
}

void ImageListLightPlayer::SetDecodeRate(double rate)
{
	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
			
		_condition.notify_one();
	}
}

double ImageListLightPlayer::GetDecodeRate()
{
	return _decode_rate;
}

jgui::Component * ImageListLightPlayer::GetVisualComponent()
{
	return _component;
}

}

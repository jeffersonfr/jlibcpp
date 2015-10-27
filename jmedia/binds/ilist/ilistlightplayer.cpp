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
#include "ilistlightplayer.h"
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
#include "jfile.h"

#if defined(DIRECTFB_NODEPS_UI)
#include <directfb.h>
#else
#include <cairo.h>
#endif

namespace jmedia {

namespace imagelistlightplayer {

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

		virtual void UpdateComponent(jgui::Image *frame)
		{
			jgui::jsize_t isize = frame->GetSize();

			if (_frame_size.width != isize.width || _frame_size.height != isize.height) {
				if (_frame_size.width < 0 || _frame_size.height < 0) {
					_src.width = isize.width;
					_src.height = isize.height;
				}

				_frame_size.width = isize.width;
				_frame_size.height = isize.height;
			}

			_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, frame));

			_mutex.Unlock();

			_image = frame;

			Run();

			_image = NULL;
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
		ImageListLightPlayer *_player;

	public:
		VideoSizeControlImpl(ImageListLightPlayer *player):
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

struct ascending_sort {
	bool operator()(std::string a, std::string b)
	{
		if (a < b) {
			return true;
		}

		return false;
	}
};

}

ImageListLightPlayer::ImageListLightPlayer(std::string directory):
	jmedia::Player()
{
	_directory = directory;
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

	if (file == NULL) {
		throw jcommon::RuntimeException("Unable to open the media directory");
	}

	std::vector<std::string> files;

	if (file->ListFiles(&files) == false) {
		throw jcommon::RuntimeException("Unable to list files in the local directory");
	}

	if (files.size() == 0) {
		throw jcommon::RuntimeException("There is no file in the directory");
	}

	for (std::vector<std::string>::iterator i=files.begin(); i!=files.end(); i++) {
		std::string file = (*i);

		if (file.size() > 3 && file[0] != '.') {
			jio::File *tmp = jio::File::OpenFile(file);

			if (tmp != NULL) {
				_image_list.push_back(_directory + "/" + file);
			}

			delete tmp;
		}
	}

	delete file;

	std::sort(_image_list.begin(), _image_list.end(), imagelistlightplayer::ascending_sort());

	_controls.push_back(new imagelistlightplayer::VideoSizeControlImpl(this));

	_component = new imagelistlightplayer::PlayerComponentImpl(this, 0, 0, -1, -1);
}

ImageListLightPlayer::~ImageListLightPlayer()
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

void ImageListLightPlayer::ResetFrames()
{
	_frame_index = 0;
}

jgui::Image * ImageListLightPlayer::GetFrame()
{
	for (int i=_frame_index; i<(int)_image_list.size(); i++) {
		std::string file = _image_list[i];

		_frame_index = i + 1;

		if (_frame_index >= (int)_image_list.size()) {
			_frame_index = 0;
		}

		return jgui::Image::CreateImage(file);
	}

	return NULL;
}

void ImageListLightPlayer::Run()
{
	jgui::Image *frame;

	while (_is_playing == true) {
		_mutex.Lock();

		frame = GetFrame();

		if (frame == NULL) { 
			ResetFrames();

			if (_is_loop == false) {
				DispatchPlayerEvent(new PlayerEvent(this, JPE_FINISHED));

				_mutex.Unlock();

				break;
			}
		}

		dynamic_cast<imagelistlightplayer::PlayerComponentImpl *>(_component)->UpdateComponent(frame);

		try {
			if (_decode_rate == 0) {
				_sem.Wait(&_mutex);
			} else {
				uint64_t us;

				us = 1000000; // 1.0 frame/sec

				if (_decode_rate != 1.0) {
					us = ((double)us / _decode_rate + 0.);
				}

				_sem.Wait(us, &_mutex);
			}
		} catch (jthread::SemaphoreTimeoutException &e) {
		}

		_mutex.Unlock();
	}
}

void ImageListLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		if (IsRunning() == false) {
			_is_playing = true;

			Start();
		}
	}
}

void ImageListLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void ImageListLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;
		
		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void ImageListLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	_is_playing = false;

	WaitThread();

	if (_has_video == true) {
		_component->Repaint();
	}

	_is_paused = false;
}

void ImageListLightPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_playing = false;
	_is_closed = true;
	
	_sem.Notify();

	WaitThread();

	// TODO:: loop to dele images of list
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
	jthread::AutoLock lock(&_mutex);

	_is_loop = b;
}

bool ImageListLightPlayer::IsLoop()
{
	return _is_loop;
}

void ImageListLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
			
		_sem.Notify();
	}
}

double ImageListLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	return _decode_rate;
}

jgui::Component * ImageListLightPlayer::GetVisualComponent()
{
	return _component;
}

}

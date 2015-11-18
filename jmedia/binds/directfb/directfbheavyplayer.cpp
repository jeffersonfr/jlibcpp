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
#include "directfbheavyplayer.h"
#include "nativeimage.h"
#include "nativegraphics.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvideodevicecontrol.h"
#include "jaudioconfigurationcontrol.h"
#include "jvolumecontrol.h"
#include "jmediaexception.h"
#include "jimage.h"
#include "jwindow.h"
#include "jgfxhandler.h"

#include <directfb.h>

namespace jmedia {

namespace directfbheavyplayer {

class VideoOverlayImpl : public jgui::Component, jthread::Thread {

	public:
		/** \brief */
		IDirectFBWindow *_window;
		/** \brief */
		IDirectFBSurface *_surface;
		/** \brief */
		Player *_player;
		/** \brief */
		jgui::Image *_image;
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		jgui::jregion_t _src;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		VideoOverlayImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h),
			_mutex(jthread::JMT_RECURSIVE)
		{
			_player = player;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			_frame_size.width = w;
			_frame_size.height = h;

			jgui::GFXHandler *handler = jgui::GFXHandler::GetInstance();
			IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

			DFBWindowDescription desc;

			desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_SURFACE_CAPS); // | DWDESC_STACKING);
			desc.caps   = (DFBWindowCapabilities)(DWCAPS_NODECORATION);
			desc.pixelformat = DSPF_RGB32;
			desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_FLIPPING | DSCAPS_DOUBLE);
			desc.options = (DFBWindowOptions)(DWOP_SCALE);
			desc.stacking = DWSC_UPPER;
			desc.posx   = _location.x;
			desc.posy   = _location.y;
			desc.width  = _size.width;
			desc.height = _size.height;

			/*
			desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING);
			desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);
			desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_PREMULTIPLIED | DSCAPS_FLIPPING | DSCAPS_DOUBLE);
			desc.pixelformat = DSPF_ARGB;
			desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);
			desc.stacking = DWSC_UPPER;
			desc.posx   = _location.x;
			desc.posy   = _location.y;
			desc.width  = _size.width;
			desc.height = _size.height;
			*/

			IDirectFBDisplayLayer *layer;

			if (engine->GetDisplayLayer(engine, (DFBDisplayLayerID)(DLID_PRIMARY), &layer) != DFB_OK) {
				throw jcommon::RuntimeException("Problem to get the device layer");
			} 

			if (layer->CreateWindow(layer, &desc, &_window) != DFB_OK) {
				throw jcommon::RuntimeException("Cannot create a window");
			}

			if (_window->GetSurface(_window, &_surface) != DFB_OK) {
				_window->Release(_window);

				throw jcommon::RuntimeException("Cannot get a window's surface");
			}

			// Add ghost option (behave like an overlay)
			// _window->SetOptions(_window, (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE)); // | DWOP_GHOST));
			// Move window to upper stacking class
			// _window->SetStackingClass(_window, DWSC_UPPER);
			// Make it the top most window
			// _window->RaiseToTop(_window);
			_window->SetOpacity(_window, 0x00);
			// _surface->SetRenderOptions(_surface, DSRO_ALL);
			// _window->DisableEvents(_window, (DFBWindowEventType)(DWET_BUTTONDOWN | DWET_BUTTONUP | DWET_MOTION));

			_surface->SetDrawingFlags(_surface, (DFBSurfaceDrawingFlags)(DSDRAW_BLEND));
			_surface->SetBlittingFlags(_surface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
			_surface->SetPorterDuff(_surface, (DFBSurfacePorterDuffRule)(DSPD_NONE));

			_surface->Clear(_surface, 0x00, 0x00, 0x00, 0x00);
			_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_FLUSH));
			_surface->Clear(_surface, 0x00, 0x00, 0x00, 0x00);

			// SetVisible(true);
		}

		virtual ~VideoOverlayImpl()
		{
			if (IsRunning() == true) {
				WaitThread();
			}
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent()
		{
			RaiseToTop();

#if defined(DIRECTFB_NODEPS_UI)
			if (IsRunning() == true) {
				WaitThread();
			}

			if (_surface != NULL) {
				void *ptr;
				int pitch;
				int sw,
						sh;

				_surface->GetSize(_surface, &sw, &sh);
				_surface->Lock(_surface, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &ptr, &pitch);
				
				IDirectFBSurface *frame;
				DFBSurfaceDescription desc;

				desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_PREALLOCATED);
				desc.caps = (DFBSurfaceCapabilities)(DSCAPS_NONE);
				desc.width = sw;
				desc.height = sh;
				desc.pixelformat = DSPF_ARGB;
				desc.preallocated[0].data = ptr;
				desc.preallocated[0].pitch = pitch;

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
					
					_surface->Unlock(_surface);
					_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)0);
				} else {
					_surface->Unlock(_surface);
				}
			}
#else
			if (IsRunning() == true) {
				WaitThread();
			}

			if (_surface != NULL) {
				void *ptr;
				int pitch;
				int sw,
						sh;

				_surface->GetSize(_surface, &sw, &sh);
				_surface->Lock(_surface, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &ptr, &pitch);

				cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
						(uint8_t *)ptr, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
				cairo_t *cairo_context = cairo_create(cairo_surface);

				jgui::NativeImage *image = new jgui::NativeImage(cairo_context, jgui::JPF_ARGB, sw, sh);

				_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, image));

				cairo_surface_flush(cairo_surface);
				cairo_surface_destroy(cairo_surface);

				_surface->Unlock(_surface);
			}

			Start();
#endif
		}

		virtual void Move(int x, int y)
		{
			SetLocation(x, y);
		}
		
		virtual void SetBounds(int x, int y, int width, int height)
		{
			jgui::Component::SetBounds(x, y, width, height);

			_window->SetBounds(_window, x, y, width, height);
			_window->ResizeSurface(_window, width, height);
		}
		
		virtual void SetLocation(int x, int y)
		{
			SetLocation(x, y);

			_window->MoveTo(_window, x, y);
		}
		
		virtual void SetSize(int width, int height)
		{
			jgui::Component::SetSize(width, height);

			_window->Resize(_window, width, height);
			_window->ResizeSurface(_window, width, height);
		}
		
		virtual void SetVisible(bool visible)
		{
			jgui::Component::SetVisible(visible);

			if (IsVisible() == true) {
				_window->SetOpacity(_window, 0xff);
			} else {
				_window->SetOpacity(_window, 0x00);
			}
		}

		virtual void RaiseToTop()
		{
			jgui::Container *parent = GetTopLevelAncestor();
			
			if (parent != NULL) {
				jgui::Window *window = dynamic_cast<jgui::Window *>(parent);

				if (window != NULL) {
					IDirectFBWindow *native = (IDirectFBWindow *)window->GetNativeWindow();

					if (native != NULL) {
						_window->PutAtop(_window, native);
					}
				}
			} else {
				_window->RaiseToTop(_window);
			}
		}
		
		virtual void LowerToBottom()
		{
			jgui::Container *parent = GetTopLevelAncestor();
			
			if (parent != NULL) {
				jgui::Window *window = dynamic_cast<jgui::Window *>(parent);

				if (window != NULL) {
					IDirectFBWindow *native = (IDirectFBWindow *)window->GetNativeWindow();

					if (native != NULL) {
						_window->PutAtop(_window, native);
					}
				}
			} else {
				_window->LowerToBottom(_window);
			}
		}

		virtual void Run()
		{
			_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class VolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		DirectFBHeavyPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		VolumeControlImpl(DirectFBHeavyPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;
			_is_muted = false;

			SetLevel(100);
		}

		virtual ~VolumeControlImpl()
		{
		}

		virtual int GetLevel()
		{
			jthread::AutoLock lock(&_player->_mutex);

			float level = 0.0f;

			if (_player->_provider != NULL) {
				_player->_provider->GetVolume(_player->_provider, &level);
			}

			return (int)(level*100.0f);
		}

		virtual void SetLevel(int level)
		{
			jthread::AutoLock lock(&_player->_mutex);

			_level = level;

			if (_level <= 0) {
				_level = 0;
				_is_muted = true;
			} else {
				if (_level > 100) {
					_level = 100;
				}

				_is_muted = false;
			}

			if (_player->_provider != NULL) {
				_player->_provider->SetVolume(_player->_provider, _level/100.0f);
			}
		}
		
		virtual bool IsMute()
		{
			return _is_muted;
		}

		virtual void SetMute(bool b)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_is_muted == b) {
				return;
			}

			if (_is_muted == false) {
				float level = _level/100.0f;

				if (_player->_provider != NULL) {
					_player->_provider->SetVolume(_player->_provider, level);
				}
			} else {
				if (_player->_provider != NULL) {
					_player->_provider->SetVolume(_player->_provider, 0.0f);
				}
			}
		}

};

class AudioConfigurationControlImpl : public AudioConfigurationControl {
	
	private:
		/** \brief */
		DirectFBHeavyPlayer *_player;
		/** \brief */
		int64_t _audio_delay;

	public:
		AudioConfigurationControlImpl(DirectFBHeavyPlayer *player):
			AudioConfigurationControl()
		{
			_player = player;
			_audio_delay = 0LL;
		}

		virtual ~AudioConfigurationControlImpl()
		{
		}

		virtual void SetAudioDelay(int64_t delay)
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			if (_player->_provider != NULL) {
				if (_player->_provider->SetAudioDelay(_player->_provider, delay) == DFB_OK) {
					_audio_delay = delay;
				}
			}
		}

		virtual int64_t GetAudioDelay()
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			return _audio_delay;
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		DirectFBHeavyPlayer *_player;

	public:
		VideoSizeControlImpl(DirectFBHeavyPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			VideoOverlayImpl *impl = dynamic_cast<VideoOverlayImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			VideoOverlayImpl *impl = dynamic_cast<VideoOverlayImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<VideoOverlayImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<VideoOverlayImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		DirectFBHeavyPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		VideoFormatControlImpl(DirectFBHeavyPlayer *player):
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
		DirectFBHeavyPlayer *_player;
		std::map<jvideo_control_t, int> _default_values;

	public:
		VideoDeviceControlImpl(DirectFBHeavyPlayer *player):
			VideoDeviceControl()
		{
			_player = player;
			
			_controls.push_back(JVC_CONTRAST);
			_controls.push_back(JVC_SATURATION);
			_controls.push_back(JVC_HUE);
			_controls.push_back(JVC_BRIGHTNESS);

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

			if (_player->_provider != NULL) {
				if (HasControl(id) == true) {
					DFBColorAdjustment adj;

					if (id == JVC_CONTRAST) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_CONTRAST);
					} else if (id == JVC_SATURATION) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_SATURATION);
					} else if (id == JVC_HUE) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_HUE);
					} else if (id == JVC_BRIGHTNESS) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_BRIGHTNESS);
					}
					
					_player->_provider->GetColorAdjustment(_player->_provider, &adj);

					if (id == JVC_CONTRAST) {
						return adj.contrast;
					} else if (id == JVC_SATURATION) {
						return adj.saturation;
					} else if (id == JVC_HUE) {
						return adj.hue;
					} else if (id == JVC_BRIGHTNESS) {
						return adj.brightness;
					}
				}
			}
				
			return 0;
		}

		virtual bool SetValue(jvideo_control_t id, int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				if (HasControl(id) == true) {
					DFBColorAdjustment adj;

					adj.contrast = value;

					if (id == JVC_CONTRAST) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_CONTRAST);
					} else if (id == JVC_SATURATION) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_SATURATION);
					} else if (id == JVC_HUE) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_HUE);
					} else if (id == JVC_BRIGHTNESS) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_BRIGHTNESS);
					}
					
					_player->_provider->SetColorAdjustment(_player->_provider, &adj);

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

}

DirectFBHeavyPlayer::DirectFBHeavyPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_decode_rate = 1.0;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_component = NULL;
	_frames_per_second = 0.0;

	IDirectFB *directfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();
	
	if (directfb->CreateVideoProvider(directfb, _file.c_str(), &_provider) != DFB_OK) {
		_provider = NULL;

		throw jmedia::MediaException("Media format not supported");
	}
		
	DFBSurfaceDescription sdsc;
	DFBStreamDescription mdsc;
	
	_provider->SetPlaybackFlags(_provider, DVPLAY_NOFX);
	_provider->GetSurfaceDescription(_provider, &sdsc);
	_provider->GetStreamDescription(_provider, &mdsc);
	_provider->CreateEventBuffer(_provider, &_events);

	_aspect = (double)sdsc.width/(double)sdsc.height;
	
	char tmp[256];

	sprintf(tmp, "%d", mdsc.year);

	_media_info.title = std::string(mdsc.title);
	_media_info.author = std::string(mdsc.author);
	_media_info.album = std::string(mdsc.album);
	_media_info.genre = std::string(mdsc.genre);
	_media_info.comments = std::string(mdsc.comment);
	_media_info.date = std::string(tmp);

	if (mdsc.caps & DVSCAPS_AUDIO) {
		_has_audio = true;
	
		_controls.push_back(new directfbheavyplayer::VolumeControlImpl(this));
		_controls.push_back(new directfbheavyplayer::AudioConfigurationControlImpl(this));
	}

	if (mdsc.caps & DVSCAPS_VIDEO) {
		_has_video = true;
		_aspect = mdsc.video.aspect;
		_frames_per_second = mdsc.video.framerate;

		_controls.push_back(new directfbheavyplayer::VideoSizeControlImpl(this));
		_controls.push_back(new directfbheavyplayer::VideoFormatControlImpl(this));
		_controls.push_back(new directfbheavyplayer::VideoDeviceControlImpl(this));
	}

	_component = new directfbheavyplayer::VideoOverlayImpl(this, 0, 0, sdsc.width, sdsc.height);

	Start();
}

DirectFBHeavyPlayer::~DirectFBHeavyPlayer()
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

void DirectFBHeavyPlayer::Callback(void *ctx)
{
	reinterpret_cast<directfbheavyplayer::VideoOverlayImpl *>(ctx)->UpdateComponent();
}
		
void DirectFBHeavyPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		directfbheavyplayer::VideoOverlayImpl *component = dynamic_cast<directfbheavyplayer::VideoOverlayImpl *>(_component);
		IDirectFBSurface *surface = (IDirectFBSurface *)component->_surface;

		if (_has_video == true) {
			_provider->PlayTo(_provider, surface, NULL, DirectFBHeavyPlayer::Callback, (void *)_component);
		} else {
			_provider->PlayTo(_provider, surface, NULL, NULL, NULL);
		}

		// usleep(500000);
	}
}

void DirectFBHeavyPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void DirectFBHeavyPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;

		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void DirectFBHeavyPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		_provider->Stop(_provider);

		_is_paused = false;
	}
}

void DirectFBHeavyPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	WaitThread();

	if (_provider != NULL) {
		// INFO:: cause crashes when release and create players
		//_events->Release(_events);
		_events = NULL;

		_provider->Release(_provider);
		_provider = NULL;
	}
}

void DirectFBHeavyPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		_provider->SeekTo(_provider, (double)time/1000.0);
	}
}

uint64_t DirectFBHeavyPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetPos(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

uint64_t DirectFBHeavyPlayer::GetMediaTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetLength(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

void DirectFBHeavyPlayer::SetLoop(bool b)
{
	jthread::AutoLock lock(&_mutex);

	_is_loop = b;

	if (_provider != NULL) {
		if (_is_loop == false) {
			_provider->SetPlaybackFlags(_provider, DVPLAY_NOFX);
		} else {
			_provider->SetPlaybackFlags(_provider, DVPLAY_LOOPING);
		}
	}
}

bool DirectFBHeavyPlayer::IsLoop()
{
	return _is_loop;
}

void DirectFBHeavyPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	if (rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != NULL) {
		_provider->SetSpeed(_provider, rate);
	}
}

double DirectFBHeavyPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_provider != NULL) {
		_provider->GetSpeed(_provider, &rate);
	}

	return rate;
}

jgui::Component * DirectFBHeavyPlayer::GetVisualComponent()
{
	return _component;
}

void DirectFBHeavyPlayer::Run()
{
	while (_is_closed == false) {
		_events->WaitForEventWithTimeout(_events, 0, 100);

		while (_events->HasEvent(_events) == DFB_OK) {
			DFBVideoProviderEvent event;

			_events->GetEvent(_events, DFB_EVENT(&event));

			if (event.clazz == DFEC_VIDEOPROVIDER) {
				if (event.type == DVPET_STARTED) {
					DispatchPlayerEvent(new PlayerEvent(this, JPE_STARTED));
				} else if (event.type == DVPET_STOPPED) {
					DispatchPlayerEvent(new PlayerEvent(this, JPE_STOPPED));
				} else if (event.type == DVPET_FINISHED) {
					DispatchPlayerEvent(new PlayerEvent(this, JPE_FINISHED));
				}
			}
		}
	}
}

}

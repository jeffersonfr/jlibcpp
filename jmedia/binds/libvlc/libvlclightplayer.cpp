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
#include "libvlclightplayer.h"
#include "nativeimage.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvolumecontrol.h"
#include "jaudioconfigurationcontrol.h"
#include "jmediaexception.h"
#include "jgfxhandler.h"

#if defined(DIRECTFB_NODEPS_UI)
#include <directfb.h>
#else
#include <cairo.h>
#endif

namespace jmedia {

namespace libvlclightplayer {

static libvlc_event_type_t mi_events[] = {
	// libvlc_MediaMetaChanged,	
	// libvlc_MediaSubItemAdded,	
	// libvlc_MediaDurationChanged,
	// libvlc_MediaParsedChanged,
	// libvlc_MediaFreed,
	// libvlc_MediaStateChanged,	
	// libvlc_MediaSubItemTreeAdded,	
	// libvlc_MediaPlayerMediaChanged,
	// libvlc_MediaPlayerNothingSpecial,
	// libvlc_MediaPlayerOpening,	
	// libvlc_MediaPlayerBuffering,
	libvlc_MediaPlayerPlaying,	
	libvlc_MediaPlayerPaused,	
	libvlc_MediaPlayerStopped,	
	// libvlc_MediaPlayerForward,	
	// libvlc_MediaPlayerBackward,	
	libvlc_MediaPlayerEndReached,
	// libvlc_MediaPlayerEncounteredError,	
	// libvlc_MediaPlayerTimeChanged,
	// libvlc_MediaPlayerPositionChanged,
	// libvlc_MediaPlayerSeekableChanged,
	// libvlc_MediaPlayerPausableChanged,
	// libvlc_MediaPlayerTitleChanged,	
	// libvlc_MediaPlayerSnapshotTaken,
	// libvlc_MediaPlayerLengthChanged,
	// libvlc_MediaPlayerVout,
	// libvlc_MediaPlayerScrambledChanged,
	// libvlc_MediaPlayerESAdded,	
	// libvlc_MediaPlayerESDeleted,	
	// libvlc_MediaPlayerESSelected,
	// libvlc_MediaPlayerCorked,	
	// libvlc_MediaPlayerUncorked,
	// libvlc_MediaPlayerMuted,	
	// libvlc_MediaPlayerUnmuted,
	// libvlc_MediaPlayerAudioVolume,	
	// libvlc_MediaPlayerAudioDevice,
	// libvlc_MediaListItemAdded,	
	// libvlc_MediaListWillAddItem,	
	// libvlc_MediaListItemDeleted,
	// libvlc_MediaListWillDeleteItem,	
	// libvlc_MediaListEndReached,	
	// libvlc_MediaListViewItemAdded,
	// libvlc_MediaListViewWillAddItem,	
	// libvlc_MediaListViewItemDeleted,	
	// libvlc_MediaListViewWillDeleteItem,	
	// libvlc_MediaListPlayerPlayed,
	// libvlc_MediaListPlayerNextItemSet,	
	// libvlc_MediaListPlayerStopped,	
	// libvlc_MediaDiscovererStarted,	
	// libvlc_MediaDiscovererEnded,	
	// libvlc_VlmMediaAdded,	
	// libvlc_VlmMediaRemoved,
	// libvlc_VlmMediaChanged,
	// libvlc_VlmMediaInstanceStarted,	
	// libvlc_VlmMediaInstanceStopped,
	// libvlc_VlmMediaInstanceStatusInit,	
	// libvlc_VlmMediaInstanceStatusOpening,	
	// libvlc_VlmMediaInstanceStatusPlaying,	
	// libvlc_VlmMediaInstanceStatusPause,
	// libvlc_VlmMediaInstanceStatusEnd,	
	// libvlc_VlmMediaInstanceStatusError,
};
	
static int mi_events_len = sizeof(mi_events)/sizeof(*mi_events);

class LibVLCLightComponentImpl : public jgui::Component, jthread::Thread {

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
		uint32_t **_buffer;
		/** \brief */
		int _buffer_index;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		LibVLCLightComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_buffer = new uint32_t*[2];
			
			_buffer[0] = new uint32_t[w*h];
			_buffer[1] = new uint32_t[w*h];

			_buffer_index = 0;

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

		virtual ~LibVLCLightComponentImpl()
		{
			if (IsRunning() == true) {
				WaitThread();
			}

			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			if (_buffer != NULL) {
				delete [] _buffer[0];
				delete [] _buffer[1];

				delete _buffer;
				_buffer = NULL;
			}

			_mutex.Unlock();
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent()
		{
#if defined(DIRECTFB_NODEPS_UI)
			if (IsRunning() == true) {
				WaitThread();
			}

			int sw = _frame_size.width;
			int sh = _frame_size.height;

			IDirectFBSurface *frame;
			DFBSurfaceDescription desc;

			desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_PREALLOCATED);
			desc.caps = (DFBSurfaceCapabilities)(DSCAPS_NONE);
			desc.width = sw;
			desc.height = sh;
			desc.pixelformat = DSPF_ARGB;
			desc.preallocated[0].data = (uint8_t *)_buffer[(_buffer_index+1)%2];
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

			int sw = _frame_size.width;
			int sh = _frame_size.height;

			cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
					(uint8_t *)_buffer[(_buffer_index+1)%2], CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
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

			g->DrawImage(_image, _src.x, _src.y, _src.width, _src.height, _location.x, _location.y, _size.width, _size.height);
				
			_mutex.Unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

static void * LockMediaSurface(void *data, void **p_pixels)
{
	LibVLCLightComponentImpl *cmp = reinterpret_cast<LibVLCLightComponentImpl *>(data);
	
	(*p_pixels) = cmp->_buffer[cmp->_buffer_index];

	cmp->_buffer_index = (cmp->_buffer_index + 1)%2;

	return NULL; 
}

static void UnlockMediaSurface(void *data, void *id, void *const *p_pixels)
{
}

static void DisplayMediaSurface(void *data, void *id)
{
	reinterpret_cast<LibVLCLightComponentImpl *>(data)->UpdateComponent();
}

static void MediaEventsCallback(const libvlc_event_t *event, void *data)
{
	LibVLCLightPlayer	*player = reinterpret_cast<LibVLCLightPlayer *>(data);

	if (event->type == libvlc_MediaMetaChanged) {
	//} else if (event->type == libvlc_MediaSubItemAdded) {
	//} else if (event->type == libvlc_MediaDurationChanged) {
	//} else if (event->type == libvlc_MediaParsedChanged) {
	//} else if (event->type == libvlc_MediaFreed) {
	//} else if (event->type == libvlc_MediaStateChanged) {
	//} else if (event->type == libvlc_MediaSubItemTreeAdded) {
	//} else if (event->type == libvlc_MediaPlayerMediaChanged) {
	//} else if (event->type == libvlc_MediaPlayerNothingSpecial) {
	//} else if (event->type == libvlc_MediaPlayerOpening) {
	//} else if (event->type == libvlc_MediaPlayerBuffering) {
	} else if (event->type == libvlc_MediaPlayerPlaying) {
		player->DispatchPlayerEvent(new PlayerEvent(player, JPE_STARTED));
	// } else if (event->type == libvlc_MediaPlayerPaused) {
	} else if (event->type == libvlc_MediaPlayerStopped) {
		player->DispatchPlayerEvent(new PlayerEvent(player, JPE_STOPPED));
	//} else if (event->type == libvlc_MediaPlayerForward) {
	//} else if (event->type == libvlc_MediaPlayerBackward) {
	} else if (event->type == libvlc_MediaPlayerEndReached) {
		player->Start();
	// } else if (event->type == libvlc_MediaPlayerEncounteredError) {
	// } else if (event->type == libvlc_MediaPlayerTimeChanged) {
	// } else if (event->type == libvlc_MediaPlayerPositionChanged) {
	// } else if (event->type == libvlc_MediaPlayerSeekableChanged) {
	// } else if (event->type == libvlc_MediaPlayerPausableChanged) {
	// } else if (event->type == libvlc_MediaPlayerTitleChanged) {
	// } else if (event->type == libvlc_MediaPlayerSnapshotTaken) {
	// } else if (event->type == libvlc_MediaPlayerLengthChanged) {
	// } else if (event->type == libvlc_MediaPlayerVout) {
	// } else if (event->type == libvlc_MediaPlayerScrambledChanged) {
	// } else if (event->type == libvlc_MediaPlayerESAdded) {
	// } else if (event->type == libvlc_MediaPlayerESDeleted) {
	// } else if (event->type == libvlc_MediaPlayerESSelected) {
	// } else if (event->type == libvlc_MediaPlayerCorked) {
	// } else if (event->type == libvlc_MediaPlayerUncorked) {
	// } else if (event->type == libvlc_MediaPlayerMuted) {
	// } else if (event->type == libvlc_MediaPlayerUnmuted) {
	// } else if (event->type == libvlc_MediaPlayerAudioVolume) {
	// } else if (event->type == libvlc_MediaPlayerAudioDevice) {
	// } else if (event->type == libvlc_MediaListItemAdded) {
	// } else if (event->type == libvlc_MediaListWillAddItem) {
	// } else if (event->type == libvlc_MediaListItemDeleted) {
	// } else if (event->type == libvlc_MediaListWillDeleteItem) {
	// } else if (event->type == libvlc_MediaListEndReached) {
	// } else if (event->type == libvlc_MediaListViewItemAdded) {
	// } else if (event->type == libvlc_MediaListViewWillAddItem) {
	// } else if (event->type == libvlc_MediaListViewItemDeleted) {
	// } else if (event->type == libvlc_MediaListViewWillDeleteItem) {
	// } else if (event->type == libvlc_MediaListPlayerPlayed) {
	// } else if (event->type == libvlc_MediaListPlayerNextItemSet) {
	// } else if (event->type == libvlc_MediaListPlayerStopped) {
	// } else if (event->type == libvlc_MediaDiscovererStarted) {
	// } else if (event->type == libvlc_MediaDiscovererEnded) {
	// } else if (event->type == libvlc_VlmMediaAdded) {
	// } else if (event->type == libvlc_VlmMediaRemoved) {
	// } else if (event->type == libvlc_VlmMediaChanged) {
	// } else if (event->type == libvlc_VlmMediaInstanceStarted) {
	// } else if (event->type == libvlc_VlmMediaInstanceStopped) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusInit) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusOpening) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusPlaying) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusPause) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusEnd) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusError) {
	}
}

class VolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		LibVLCLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		VolumeControlImpl(LibVLCLightPlayer *player):
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

			if (_player->_provider != NULL) {
				level = libvlc_audio_get_volume(_player->_provider);
			}

			return level;
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
				libvlc_audio_set_mute(_player->_provider, (_is_muted == true)?1:0);
				libvlc_audio_set_volume(_player->_provider, _level);
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
			
			if (_player->_provider != NULL) {
				libvlc_audio_set_mute(_player->_provider, (_is_muted == true)?1:0);
			}
		}

};

class AudioConfigurationControlImpl : public AudioConfigurationControl {
	
	private:
		/** \brief */
		LibVLCLightPlayer *_player;

	public:
		AudioConfigurationControlImpl(LibVLCLightPlayer *player):
			AudioConfigurationControl()
		{
			_player = player;
		}

		virtual ~AudioConfigurationControlImpl()
		{
		}

		virtual void SetAudioMode(jaudio_config_mode_t)
		{
		}

		virtual jaudio_config_mode_t GetHDMIAudioMode()
		{
			return AudioConfigurationControl::GetHDMIAudioMode();
		}

		virtual void SetSPDIFPCM(bool pcm)
		{
		}

		virtual bool IsSPDIFPCM()
		{
			return AudioConfigurationControl::IsSPDIFPCM();
		}

		virtual void SetAudioDelay(int64_t delay)
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			if (_player->_provider != NULL) {
				 libvlc_audio_set_delay(_player->_provider, delay);
			}
		}

		virtual int64_t GetAudioDelay()
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			int64_t delay = 0LL;

			if (_player->_provider != NULL) {
				 delay = libvlc_audio_get_delay(_player->_provider);
			}

			return delay;
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		LibVLCLightPlayer *_player;

	public:
		VideoSizeControlImpl(LibVLCLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			LibVLCLightComponentImpl *impl = dynamic_cast<LibVLCLightComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			LibVLCLightComponentImpl *impl = dynamic_cast<LibVLCLightComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<LibVLCLightComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			LibVLCLightComponentImpl *impl = dynamic_cast<LibVLCLightComponentImpl *>(_player->_component);

			jgui::jregion_t t;

			t.x = impl->GetX();
			t.y = impl->GetY();
			t.width = impl->GetWidth();
			t.height = impl->GetHeight();

			return t;
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		LibVLCLightPlayer *_player;
		jaspect_ratio_t _aspect_ratio;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		VideoFormatControlImpl(LibVLCLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_aspect_ratio = LAR_16x9;
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~VideoFormatControlImpl()
		{
		}

		virtual void SetAspectRatio(jaspect_ratio_t t)
		{
			_aspect_ratio = t;
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

}

LibVLCLightPlayer::LibVLCLightPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	
	char const *vlc_argv[] = {
		"--vout=dummy"
	};
	int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

	libvlc_media_t *media;

	_engine = libvlc_new(vlc_argc, vlc_argv);
	media = libvlc_media_new_path(_engine, file.c_str());
	_provider = libvlc_media_player_new_from_media(media);

	libvlc_media_parse(media);

 	libvlc_media_track_t **tracks = NULL;
	int count = libvlc_media_tracks_get(media, &tracks);

	for (int i=0; i<count; i++) {
		libvlc_media_track_t *t = tracks[i];

		if (t->i_type == libvlc_track_audio) {
			if (_has_audio == false) {
				_has_audio = true;

				_controls.push_back(new libvlclightplayer::VolumeControlImpl(this));
				_controls.push_back(new libvlclightplayer::AudioConfigurationControlImpl(this));
			}
		} else if (t->i_type == libvlc_track_video) {
			if (_has_video == false) {
				_has_video = true;
				
				_controls.push_back(new libvlclightplayer::VideoSizeControlImpl(this));
				_controls.push_back(new libvlclightplayer::VideoFormatControlImpl(this));
			}
		}
	}

	libvlc_media_tracks_release(tracks, count);

	uint32_t iw = 1;
	uint32_t ih = 1;

	if (_has_video == true) {
		if (libvlc_video_get_size(_provider, 0, &iw, &ih) != 0 || iw <= 0 || ih <= 0) {
			// CHANGE:: fix this section (currently needed to wait the finshing of tracks loading)
			libvlc_audio_set_mute(_provider, 1);
			libvlc_media_player_play(_provider);
			libvlc_media_player_pause(_provider);

			// waits 5 seconds to get video size
			for (int i=0; i<50; i++) {
				libvlc_video_get_size(_provider, 0, &iw, &ih);

				if (iw > 0 && ih > 0) {
					break;
				}

				usleep(100000);
			}

			libvlc_media_player_stop(_provider);
			libvlc_audio_set_mute(_provider, 0);

			if (iw <= 0 || ih <= 0) {
				libvlc_media_player_release(_provider);
				libvlc_release(_engine);

				throw jcommon::RuntimeException("Cannot retrive the size of media content");
			}
		}

		_aspect = (double)(iw/(double)ih);
	}

	_media_time = (uint64_t)libvlc_media_get_duration(media);
	// _frame_per_sec = libvlc_media_player_get_fps(_provider);

	_component = new libvlclightplayer::LibVLCLightComponentImpl(this, 0, 0, iw, ih);

	libvlc_video_set_format(_provider, "RV32", iw, ih, iw*4);
	libvlc_video_set_callbacks(
			_provider, 
			libvlclightplayer::LockMediaSurface, 
			libvlclightplayer::UnlockMediaSurface, 
			libvlclightplayer::DisplayMediaSurface, 
			_component);

	_media_info.title = std::string(libvlc_media_get_meta(media, libvlc_meta_Title)?:"");
	_media_info.author = std::string(libvlc_media_get_meta(media, libvlc_meta_Artist)?:"");
	_media_info.album = std::string(libvlc_media_get_meta(media, libvlc_meta_Album)?:"");
	_media_info.genre = std::string(libvlc_media_get_meta(media, libvlc_meta_Genre)?:"");
	_media_info.comments = std::string(libvlc_media_get_meta(media, libvlc_meta_Description)?:"");
	_media_info.date = std::string(libvlc_media_get_meta(media, libvlc_meta_Date)?:"");
	
	/*
	libvlc_meta_Copyright
	libvlc_meta_TrackNumber
	libvlc_meta_Rating
	libvlc_meta_Date
	libvlc_meta_Setting
	libvlc_meta_URL
	libvlc_meta_Language
	libvlc_meta_NowPlaying
	libvlc_meta_Publisher
	libvlc_meta_EncodedBy
	libvlc_meta_ArtworkURL
	libvlc_meta_TrackID
	libvlc_meta_TrackTotal
	libvlc_meta_Director
	libvlc_meta_Season
	libvlc_meta_Episode
	libvlc_meta_ShowName
	libvlc_meta_Actors
	libvlc_meta_AlbumArtist
	libvlc_meta_DiscNumber 
	*/

	libvlc_media_release(media);
	
	_event_manager = libvlc_media_player_event_manager(_provider);

	for (int i=0; i<libvlclightplayer::mi_events_len; i++) {
		libvlc_event_attach(_event_manager, libvlclightplayer::mi_events[i], libvlclightplayer::MediaEventsCallback, this);
	}
}

LibVLCLightPlayer::~LibVLCLightPlayer()
{
	if (IsRunning() == true) {
		WaitThread();
	}

	for (int i=0; i<libvlclightplayer::mi_events_len; i++) {
		libvlc_event_detach(_event_manager, libvlclightplayer::mi_events[i], libvlclightplayer::MediaEventsCallback, this);
	}

	Close();
	
	delete _component;
	_component = NULL;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();
}

void LibVLCLightPlayer::Run()
{
	if (IsLoop() == true) {
		Stop();
		Play();
	} else {
		Stop();

		DispatchPlayerEvent(new PlayerEvent(this, JPE_FINISHED));
	}
}

void LibVLCLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		libvlc_media_player_play(_provider);
	}
}

void LibVLCLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		_is_paused = true;

		if (libvlc_media_player_can_pause(_provider) == true) {
			libvlc_media_player_set_pause(_provider, 1);
		}
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void LibVLCLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true && _provider != NULL) {
		_is_paused = false;

		libvlc_media_player_set_pause(_provider, 0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void LibVLCLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		libvlc_media_player_stop(_provider);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void LibVLCLightPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_provider != NULL) {
		// INFO:: cause crashes when release and create players
		// _events->Release(_events);
		// _events = NULL;

		libvlc_media_player_release(_provider);
		_provider = NULL;

		libvlc_release(_engine);
		_engine = NULL;
	}
}

void LibVLCLightPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		if (libvlc_media_player_is_seekable(_provider) == true) {
			libvlc_media_player_set_time(_provider, (libvlc_time_t)time);
		}
	}
}

uint64_t LibVLCLightPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	uint64_t time = 0LL;

	if (_provider != NULL) {
		time = (uint64_t)libvlc_media_player_get_time(_provider);
	}

	return time;
}

uint64_t LibVLCLightPlayer::GetMediaTime()
{
	return _media_time;
}

void LibVLCLightPlayer::SetLoop(bool b)
{
	jthread::AutoLock lock(&_mutex);

	_is_loop = b;
}

bool LibVLCLightPlayer::IsLoop()
{
	return _is_loop;
}

void LibVLCLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != NULL) {
		libvlc_media_player_set_rate(_provider, (float)rate);
	}
}

double LibVLCLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_provider != NULL) {
		rate = (double)libvlc_media_player_get_rate(_provider);
	}

	return rate;
}

jgui::Component * LibVLCLightPlayer::GetVisualComponent()
{
	return _component;
}

}

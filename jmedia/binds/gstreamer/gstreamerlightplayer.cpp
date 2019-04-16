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
#include "gstreamerlightplayer.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvolumecontrol.h"
#include "jmedia/jaudioconfigurationcontrol.h"
#include "jmedia/jvideodevicecontrol.h"
#include "jgui/jbufferedimage.h"
#include "jexception/jmediaexception.h"
#include "jexception/jcontrolexception.h"

#include <cairo.h>

namespace jmedia {

class GStreamerPlayerComponentImpl : public jgui::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		cairo_surface_t *_surface;
		/** \brief */
    std::mutex _mutex;
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
		GStreamerPlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_buffer = new uint32_t*[2];
			
			_buffer[0] = new uint32_t[w*h];
			_buffer[1] = new uint32_t[w*h];

			_buffer_index = 0;

			_surface = nullptr;
			_player = player;
			
			_frame_size.width = -1;
			_frame_size.height = -1;

			_src.x = 0;
			_src.y = 0;
			_src.width = -1;
			_src.height = -1;

			_dst.x = 0;
			_dst.y = 0;
			_dst.width = w;
			_dst.height = h;

			SetVisible(true);
		}

		virtual ~GStreamerPlayerComponentImpl()
		{
      if (_surface != nullptr) {
        cairo_surface_destroy(_surface);
      }

			if (_buffer != nullptr) {
				delete [] _buffer[0];
				delete [] _buffer[1];

				delete _buffer;
				_buffer = nullptr;
			}
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(uint8_t *data, int width, int height)
		{
			_mutex.lock();

      if (_frame_size.width < 0 or _frame_size.height < 0) {
        delete [] _buffer[0];
        delete [] _buffer[1];
			
        _buffer[0] = new uint32_t[width*height];
			  _buffer[1] = new uint32_t[width*height];

			  _frame_size.width = width;
			  _frame_size.height = height;
			
        if (_src.width < 0 or _src.height < 0) {
			    _src.width = width;
			    _src.height = height;
        }
      }

			int sw = _frame_size.width;
			int sh = _frame_size.height;

      memcpy(_buffer[_buffer_index], data, width*height*4);

      _buffer_index = (_buffer_index + 1)%2;

			_surface = cairo_image_surface_create_for_data(
					(uint8_t *)_buffer[(_buffer_index+1)%2], CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
			
      _mutex.unlock();

      Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

      jgui::jsize_t
        size = GetSize();

			_mutex.lock();

      if (_surface == nullptr) {
        _mutex.unlock();

        return;
      }

      if (_frame_size.width < 0 or _frame_size.height < 0) {
        return;
      }

			cairo_t *context = cairo_create(_surface);
      jgui::Image *image = new jgui::BufferedImage(context);

			_player->DispatchFrameGrabberEvent(new jevent::FrameGrabberEvent(image, jevent::JFE_GRABBED));

			cairo_surface_mark_dirty(_surface);

			g->DrawImage(image, _src.x, _src.y, _src.width, _src.height, 0, 0, size.width, size.height);

      delete image;
      image = nullptr;

			cairo_surface_destroy(_surface);
      _surface = nullptr;

			_mutex.unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class GStreamerVideoDeviceControlImpl : public VideoDeviceControl {
	
	private:
		GStreamerLightPlayer *_player;
		std::map<jvideo_control_t, int> _default_values;

  private:
    float GetColorParam(const gchar *channel_name, GstColorBalance *cb) 
    {
      const GList *channels, *l;
      GstColorBalanceChannel *channel = NULL;

      // Retrieve the list of channels and locate the requested one
      channels = gst_color_balance_list_channels (cb);

      for (l = channels; l != NULL; l = l->next) {
        GstColorBalanceChannel *tmp = (GstColorBalanceChannel *)l->data;

        if (g_strrstr (tmp->label, channel_name)) {
          channel = tmp;

          break;
        }
      }

      if (!channel) {
        return 0.0f;
      }

      // Change the channel's value
      float param1 = (channel->max_value - channel->min_value);
      float param2 = (channel->max_value - gst_color_balance_get_value(cb, channel));

      return param2/param1;
    }

    void SetColorParam(const gchar *channel_name, float value, GstColorBalance *cb) 
    {
      const GList *channels, *l;
      GstColorBalanceChannel *channel = NULL;

      // Retrieve the list of channels and locate the requested one
      channels = gst_color_balance_list_channels (cb);

      for (l = channels; l != NULL; l = l->next) {
        GstColorBalanceChannel *tmp = (GstColorBalanceChannel *)l->data;

        if (g_strrstr (tmp->label, channel_name)) {
          channel = tmp;

          break;
        }
      }

      if (!channel) {
        return;
      }

      // Change the channel's value
      float param = gint((channel->max_value - channel->min_value) * value);

      // value = gst_color_balance_get_value(cb, channel);

      if (param > channel->max_value) {
        param = channel->max_value;
      }

      if (param < channel->min_value) {
        param = channel->min_value;
      }

      gst_color_balance_set_value(cb, channel, gint(param));
    }

	public:
		GStreamerVideoDeviceControlImpl(GStreamerLightPlayer *player):
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

		virtual ~GStreamerVideoDeviceControlImpl()
		{
		}

		virtual int GetValue(jvideo_control_t id)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

      if (HasControl(id) == true) {
        if (id == JVC_CONTRAST) {
          return (int)(GetColorParam("CONTRAST", GST_COLOR_BALANCE(_player->_pipeline))*100.0f);
        } else if (id == JVC_SATURATION) {
          return (int)(GetColorParam("SATURATION", GST_COLOR_BALANCE(_player->_pipeline))*100.0f);
        } else if (id == JVC_HUE) {
          return (int)(GetColorParam("HUE", GST_COLOR_BALANCE(_player->_pipeline))*100.0f);
        } else if (id == JVC_BRIGHTNESS) {
          return (int)(GetColorParam("BRIGHTNESS", GST_COLOR_BALANCE(_player->_pipeline))*100.0f);
        }
      }

			return 0;
		}

		virtual bool SetValue(jvideo_control_t id, int value)
		{
      std::unique_lock<std::mutex> lock(_player->_mutex);

      if (HasControl(id) == true) {
        if (id == JVC_CONTRAST) {
          SetColorParam("CONTRAST", value/100.0f, GST_COLOR_BALANCE(_player->_pipeline));
        } else if (id == JVC_SATURATION) {
          SetColorParam("SATURATION", value/100.0f, GST_COLOR_BALANCE(_player->_pipeline));
        } else if (id == JVC_HUE) {
          SetColorParam("HUE", value/100.0f, GST_COLOR_BALANCE(_player->_pipeline));
        } else if (id == JVC_BRIGHTNESS) {
          SetColorParam("BRIGHTNESS", value/100.0f, GST_COLOR_BALANCE(_player->_pipeline));
        }

        return true;
      }

			return false;
		}

		virtual void Reset(jvideo_control_t id)
		{
			SetValue(id, _default_values[id]);
		}

};

class GStreamerVideoSizeControlImpl : public VideoSizeControl {
	
	private:
		GStreamerLightPlayer *_player;

	public:
		GStreamerVideoSizeControlImpl(GStreamerLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~GStreamerVideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			GStreamerPlayerComponentImpl *impl = dynamic_cast<GStreamerPlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			GStreamerPlayerComponentImpl *impl = dynamic_cast<GStreamerPlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<GStreamerPlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<GStreamerPlayerComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class GStreamerVideoFormatControlImpl : public VideoFormatControl {
	
	private:
		GStreamerLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		GStreamerVideoFormatControlImpl(GStreamerLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~GStreamerVideoFormatControlImpl()
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

static GstFlowReturn SampleCallback(GstAppSink *appsink, gpointer data)
{
  GStreamerLightPlayer *player = (GStreamerLightPlayer *)data;

  GstSample *sample;
  GstVideoFrame v_frame;
  GstVideoInfo v_info;
  GstBuffer *buf;
  GstCaps *caps;
  guint8 *pixels;

  sample = gst_app_sink_pull_sample(appsink);
  g_assert_nonnull(sample);

  buf = gst_sample_get_buffer(sample);
  g_assert_nonnull(buf);

  caps = gst_sample_get_caps(sample);
  g_assert_nonnull(caps);

  g_assert(gst_video_info_from_caps(&v_info, caps));
  g_assert(gst_video_frame_map(&v_frame, &v_info, buf, GST_MAP_READ));

  pixels = (guint8 *) GST_VIDEO_FRAME_PLANE_DATA(&v_frame, 0);

  int w = GST_VIDEO_FRAME_WIDTH(&v_frame);
  int h = GST_VIDEO_FRAME_HEIGHT(&v_frame);

	static_cast<GStreamerPlayerComponentImpl *>(player->_component)->UpdateComponent(pixels, w, h);

  gst_video_frame_unmap(&v_frame);
  gst_sample_unref(sample);

  return GST_FLOW_OK;
}

GStreamerLightPlayer::GStreamerLightPlayer(jnetwork::URL url):
	jmedia::Player()
{
	_file = url.GetPath();
	_is_loop = false;
	_is_closed = false;
	_has_audio = true;
	_has_video = true;
	_aspect = 1.0;
	_frames_per_second = 0.0;
  _component = nullptr;
  _decode_rate = 1.0;

  GstElement *_bin = nullptr;
  GstElement *_filter = nullptr;
  GstElement *_color = nullptr;
  GstElement *_sink = nullptr;

  _pipeline = nullptr;

  gst_init(nullptr, nullptr);

  _pipeline = gst_element_factory_make("playbin", NULL);
  g_assert_nonnull(_pipeline);

  g_object_set(G_OBJECT(_pipeline), "uri", url.What().c_str(), NULL);

  _bin = gst_bin_new(NULL);
  g_assert_nonnull(_bin);

  _filter = gst_element_factory_make("capsfilter", NULL);
  g_assert_nonnull(_filter);

  GstStructure *st = gst_structure_new_empty("video/x-raw");
  gst_structure_set(st, "format", G_TYPE_STRING, "BGRA", NULL); //"width", G_TYPE_INT, width, "height", G_TYPE_INT, height, NULL);

  GstCaps *caps = gst_caps_new_full(st, NULL);
  g_assert_nonnull(caps);

  g_object_set(_filter, "caps", caps, NULL);

  _color = gst_element_factory_make ("videobalance", NULL);
  g_assert_nonnull(_color);

  _sink = gst_element_factory_make("appsink", NULL);
  g_assert_nonnull(_sink);

  g_assert(gst_bin_add(GST_BIN(_bin), _filter));
  g_assert(gst_bin_add(GST_BIN(_bin), _color));
  g_assert(gst_bin_add(GST_BIN(_bin), _sink));

  g_assert(gst_element_link(_filter, _color));
  g_assert(gst_element_link(_color, _sink));

  GstPad *pad = gst_element_get_static_pad(_filter, "sink");
  gst_element_add_pad(_bin, gst_ghost_pad_new("sink", pad));

  g_object_set(G_OBJECT(_pipeline), "video-sink", _bin, NULL);

  _callbacks.eos = nullptr;
  _callbacks.new_preroll = nullptr;
  _callbacks.new_sample = SampleCallback;

  gst_app_sink_set_callbacks(GST_APP_SINK(_sink), &_callbacks, this, NULL);

  gst_caps_unref(caps);
  gst_object_unref(pad);

  _is_closed = false;

  _events_thread = std::thread(&GStreamerLightPlayer::Run, this);
    
  _component = new GStreamerPlayerComponentImpl(this, 0, 0, 192, 108);
		
  _controls.push_back(new GStreamerVideoDeviceControlImpl (this));
  _controls.push_back(new GStreamerVideoSizeControlImpl (this));
  _controls.push_back(new GStreamerVideoFormatControlImpl (this));
}

GStreamerLightPlayer::~GStreamerLightPlayer()
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

void GStreamerLightPlayer::Run()
{
  GstBus *bus = gst_element_get_bus (_pipeline);

  do {
    GstMessage *msg = gst_bus_timed_pop_filtered (bus, 100 * GST_MSECOND, 
        (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_CLOCK_LOST));

    if (msg != NULL) {
      if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
        // _is_closed = true;
      } else if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_EOS) {
        // DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_FINISHED));
        
        if (IsLoop() == true) {
          Play();
        }
      } else if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_CLOCK_LOST) {
        gst_element_set_state (_pipeline, GST_STATE_PAUSED);
        gst_element_set_state (_pipeline, GST_STATE_PLAYING);
      } else if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_STATE_CHANGED) {
        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(_pipeline)) {
          GstState old_state, new_state, pending_state;

          gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);

          if (new_state == GST_STATE_PLAYING) {
            if (old_state == GST_STATE_PAUSED) {
              DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_RESUMED));
            } else {
              DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_STARTED));
            }
          } else if (new_state == GST_STATE_PAUSED) {
            DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_PAUSED));
          } else if (new_state == GST_STATE_READY) {
            DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_STOPPED));
          }
        }
      }
      
      gst_message_unref (msg);
    }
  } while (!_is_closed);

  gst_object_unref (bus);
}

void GStreamerLightPlayer::Play()
{
  if (_is_closed == true) {
    return;
  }
}

void GStreamerLightPlayer::Pause()
{
  if (_is_closed == true) {
    return;
  }

  gst_element_set_state(_pipeline, GST_STATE_PAUSED);
}

void GStreamerLightPlayer::Resume()
{
  if (_is_closed == true) {
    return;
  }

  gst_element_set_state(_pipeline, GST_STATE_PLAYING);
}

void GStreamerLightPlayer::Stop()
{
  if (_is_closed == true) {
    return;
  }

  gst_element_set_state(_pipeline, GST_STATE_NULL);

  if (_has_video == true) {
    _component->Repaint();
  }
}

void GStreamerLightPlayer::Close()
{
	if (_is_closed == true) {
		return;
	}

  _is_closed = true;

  _events_thread.join();

  gst_element_set_state(_pipeline, GST_STATE_NULL);
  gst_object_unref(_pipeline);
  gst_deinit();

  if (_surface != nullptr) {
    cairo_surface_destroy(_surface);
  }
}

void GStreamerLightPlayer::SetCurrentTime(uint64_t ms)
{
  if (_is_closed == true) {
    return;
  }

  gst_element_seek_simple(_pipeline, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), ms * GST_MSECOND);
}

uint64_t GStreamerLightPlayer::GetCurrentTime()
{
  if (_is_closed == true) {
    return -1LL;
  }

  gint64 current = -1;

  if (!gst_element_query_position (_pipeline, GST_FORMAT_TIME, &current)) {
    return -1LL;
  }

  return (int64_t)(current/1000000LL);
}

uint64_t GStreamerLightPlayer::GetMediaTime()
{
  if (_is_closed == true) {
    return -1LL;
  }

  gint64 duration = -1;

  if (!gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &duration)) {
    return -1LL;
  }

  return (int64_t)(duration/1000000LL);
}

void GStreamerLightPlayer::SetLoop(bool b)
{
	_is_loop = b;
}

bool GStreamerLightPlayer::IsLoop()
{
	return _is_loop;
}

void GStreamerLightPlayer::SetDecodeRate(double rate)
{
  _decode_rate = rate;

  gint64 position;
  GstFormat format = GST_FORMAT_TIME;
  GstEvent *seek_event;

  if (!gst_element_query_position(_pipeline, format, &position)) {
    return;
  }

  GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);

  if (_decode_rate > 0) {
    seek_event = gst_event_new_seek (_decode_rate, GST_FORMAT_TIME, flags, GST_SEEK_TYPE_SET, position, GST_SEEK_TYPE_NONE, 0);
  } else {
    seek_event = gst_event_new_seek (_decode_rate, GST_FORMAT_TIME, flags, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, position);
  }

  GstElement *video_sink = nullptr;

  g_object_get(_pipeline, "video-sink", &video_sink, NULL);

  gst_element_send_event(video_sink, seek_event);
}

double GStreamerLightPlayer::GetDecodeRate()
{
  return _decode_rate;
}

jgui::Component * GStreamerLightPlayer::GetVisualComponent()
{
	return _component;
}

}

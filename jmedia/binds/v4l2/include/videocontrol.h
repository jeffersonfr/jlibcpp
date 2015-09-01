#ifndef __VIDEOCONTROL_PHOTOBOOTH_H
#define __VIDEOCONTROL_PHOTOBOOTH_H

#include "jthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

enum video_control_t {
	BRIGHTNESS_CONTROL,
	CONTRAST_CONTROL,
	SATURATION_CONTROL,
	HUE_CONTROL,
	GAMMA_CONTROL,
	FOCUS_AUTO_CONTROL,
	ZOOM_ABSOLUTE_CONTROL 
};

struct video_query_control_t {
	video_control_t id;
	int v4l_id;
	int value;
	int default_value;
	int step;
	int minimum;
	int maximum;
};

class VideoControl {

	private:
		std::vector<video_query_control_t> _query_controls;
		int _handler;

	private:
		void EnumerateControls();

	public:
		VideoControl(int handler);

		virtual ~VideoControl();

		virtual std::vector<video_control_t> GetControls();
		
		virtual bool HasControl(video_control_t id);

		virtual int GetValue(video_control_t id);

		virtual bool SetValue(video_control_t id, int value);

		virtual void Reset();
		
		virtual void Reset(video_control_t id);

};

#endif

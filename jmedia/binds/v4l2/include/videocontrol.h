#ifndef __VIDEOCONTROL_PHOTOBOOTH_H
#define __VIDEOCONTROL_PHOTOBOOTH_H

#include "jvideodevicecontrol.h"
#include "jthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

struct video_query_control_t {
	jmedia::jvideo_control_t id;
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

		virtual double GetFramesPerSecond();

		virtual std::vector<jmedia::jvideo_control_t> GetControls();
		
		virtual bool HasControl(jmedia::jvideo_control_t id);

		virtual int GetValue(jmedia::jvideo_control_t id);

		virtual bool SetValue(jmedia::jvideo_control_t id, int value);

		virtual void Reset(jmedia::jvideo_control_t id);

		virtual void Reset();
		
};

#endif

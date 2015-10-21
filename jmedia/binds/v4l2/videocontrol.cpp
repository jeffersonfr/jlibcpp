#include "videocontrol.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct v4l2_queryctrl queryctrl;

VideoControl::VideoControl(int handler)
{
	_handler = handler;

	CLEAR(queryctrl);

	for (queryctrl.id = V4L2_CID_BASE; queryctrl.id < V4L2_CID_LASTP1; queryctrl.id++) {
		if (0 == ioctl (_handler, VIDIOC_QUERYCTRL, &queryctrl)) {
			if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
				continue;
			}

			EnumerateControls();
		}
	}

	for (queryctrl.id = V4L2_CID_CAMERA_CLASS_BASE; queryctrl.id < V4L2_CID_CAMERA_CLASS_BASE + 32; queryctrl.id++) {
		if (0 == ioctl (_handler, VIDIOC_QUERYCTRL, &queryctrl)) {
			if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
				continue;
			}

			EnumerateControls();
		}
	}

	for (queryctrl.id = V4L2_CID_PRIVATE_BASE;; queryctrl.id++) {
		if (0 == ioctl (_handler, VIDIOC_QUERYCTRL, &queryctrl)) {
			if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
				continue;
			}

			EnumerateControls();
		} else {
			if (errno == EINVAL) {
				break;
			}
		}
	}
}

VideoControl::~VideoControl()
{
}

void VideoControl::EnumerateControls()
{
	jmedia::jvideo_control_t id = jmedia::JVC_UNKNOWN;

	if (queryctrl.id == V4L2_CID_CONTRAST) {
		id = jmedia::JVC_CONTRAST;
	} else if (queryctrl.id == V4L2_CID_BRIGHTNESS) {
		id = jmedia::JVC_BRIGHTNESS;
	} else if (queryctrl.id == V4L2_CID_SATURATION) {
		id = jmedia::JVC_SATURATION;
	} else if (queryctrl.id == V4L2_CID_HUE) {
		id = jmedia::JVC_HUE;
	} else if (queryctrl.id == V4L2_CID_GAMMA) {
		id = jmedia::JVC_GAMMA;
	} else if (queryctrl.id == V4L2_CID_SHARPNESS) {
		id = jmedia::JVC_SHARPNESS;
	} else if (queryctrl.id == V4L2_CID_FOCUS_AUTO) {
		id = jmedia::JVC_AUTO_FOCUS;
	} else if (queryctrl.id == V4L2_CID_ZOOM_ABSOLUTE) {
		id = jmedia::JVC_ZOOM;
	} else if (queryctrl.id == V4L2_CID_HFLIP) {
		id = jmedia::JVC_HFLIP;
	} else if (queryctrl.id == V4L2_CID_VFLIP) {
		id = jmedia::JVC_VFLIP;
	} else if (queryctrl.id == V4L2_CID_BACKLIGHT_COMPENSATION) {
		id = jmedia::JVC_BACKLIGHT;
	} else if (queryctrl.id == V4L2_CID_EXPOSURE) {
		// id = jmedia::JVC_AUTO_EXPOSURE;
	} else if (queryctrl.id == V4L2_CID_EXPOSURE_AUTO_PRIORITY) {
		id = jmedia::JVC_AUTO_EXPOSURE;
	} else if (queryctrl.id == V4L2_CID_EXPOSURE_AUTO) {
		// id = jmedia::JVC_AUTO_EXPOSURE;
	} else {
		return;
	}

	struct video_query_control_t t;

	t.id = id;
	t.v4l_id = queryctrl.id;
	t.value = (100*(queryctrl.default_value-queryctrl.minimum))/(queryctrl.maximum-queryctrl.minimum); // 0-100
	t.default_value = queryctrl.default_value;
	t.step = queryctrl.step;
	t.minimum = queryctrl.minimum;
	t.maximum = queryctrl.maximum;

	_query_controls.push_back(t);
}

double VideoControl::GetFramesPerSecond()
{
	struct v4l2_streamparm fps;

	memset(&fps, 0, sizeof(fps));

	fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	return (double)fps.parm.capture.timeperframe.numerator/(double)fps.parm.capture.timeperframe.denominator;
}

std::vector<jmedia::jvideo_control_t> VideoControl::GetControls()
{
	std::vector<jmedia::jvideo_control_t> controls;

	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		controls.push_back((*i).id);
	}

	return controls;
}

bool VideoControl::HasControl(jmedia::jvideo_control_t id)
{
	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		if ((*i).id == id) {
			return true;
		}
	}

	return false;
}

int VideoControl::GetValue(jmedia::jvideo_control_t id)
{
	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		struct video_query_control_t t = (*i);

		if (t.id == id) {
			return t.value;
		}
	}

	return -1;
}

bool VideoControl::SetValue(jmedia::jvideo_control_t id, int value)
{
	if (value < 0) {
		value = 0;
	}

	if (value > 100) {
		value = 100;
	}

	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		struct video_query_control_t t = (*i);

		if (t.id != id) {
			continue;
		}

		if (t.value == value) {
			return true;
		}

		struct v4l2_control control;

		CLEAR(control);

		control.id = t.v4l_id;
		control.value = t.minimum+(value*(t.maximum-t.minimum))/100;

		if (-1 == ioctl (_handler, VIDIOC_S_CTRL, &control) && errno != ERANGE) {
			perror ("VIDIOC_S_CTRL");

			return false;
		}

		(*i).value = value;

		return true;
	}

	return false;
}

void VideoControl::Reset(jmedia::jvideo_control_t id)
{
	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		struct video_query_control_t t = (*i);

		if (t.id != id) {
			continue;
		}

		struct v4l2_control control;

		CLEAR(control);

		control.id = t.v4l_id;
		control.value = t.default_value;

		if (-1 == ioctl (_handler, VIDIOC_S_CTRL, &control) && errno != ERANGE) {
			perror ("VIDIOC_S_CTRL");
		}

		(*i).value = (100*(t.default_value-t.minimum))/(t.maximum-t.minimum);

		break;
	}
}

void VideoControl::Reset()
{
	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		struct video_query_control_t t = (*i);

		struct v4l2_control control;

		CLEAR(control);

		control.id = t.v4l_id;
		control.value = t.default_value;

		if (-1 == ioctl (_handler, VIDIOC_S_CTRL, &control) && errno != ERANGE) {
			perror ("VIDIOC_S_CTRL");
		}
		
		(*i).value = (100*(t.default_value-t.minimum))/(t.maximum-t.minimum);
	}
}


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
	video_control_t id;

	if (queryctrl.id == V4L2_CID_BRIGHTNESS) {
		id = BRIGHTNESS_CONTROL;
	} else if (queryctrl.id == V4L2_CID_CONTRAST) {
		id = CONTRAST_CONTROL;
	} else if (queryctrl.id == V4L2_CID_SATURATION) {
		id = SATURATION_CONTROL;
	} else if (queryctrl.id == V4L2_CID_HUE) {
		id = HUE_CONTROL;
	} else if (queryctrl.id == V4L2_CID_GAMMA) {
		id = GAMMA_CONTROL;
	} else if (queryctrl.id == V4L2_CID_FOCUS_AUTO) {
		id = FOCUS_AUTO_CONTROL;
	} else if (queryctrl.id == V4L2_CID_ZOOM_ABSOLUTE) {
		id = ZOOM_ABSOLUTE_CONTROL;
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

std::vector<video_control_t> VideoControl::GetControls()
{
	std::vector<video_control_t> controls;

	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		controls.push_back((*i).id);
	}

	return controls;
}

bool VideoControl::HasControl(video_control_t id)
{
	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		if ((*i).id == id) {
			return true;
		}
	}

	return false;
}

int VideoControl::GetValue(video_control_t id)
{
	for (std::vector<video_query_control_t>::iterator i=_query_controls.begin(); i!=_query_controls.end(); i++) {
		struct video_query_control_t t = (*i);

		if (t.id == id) {
			return t.value;
		}
	}

	return -1;
}

bool VideoControl::SetValue(video_control_t id, int value)
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

void VideoControl::Reset(video_control_t id)
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


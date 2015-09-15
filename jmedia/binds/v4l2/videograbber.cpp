#include "videograbber.h"
#include "videodev2.h"

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

#define AUTO_EXPOSURE_ENABLED	1

#define CLEAR(x) memset(&(x), 0, sizeof(x))

VideoGrabber::VideoGrabber(V4LFrameListener *listener, std::string device)
{
	_video_control = NULL;
	_listener = listener;
	_device = device;
	_method = IO_METHOD_MMAP;
	_handler = -1;
	_buffers = NULL;
	_n_buffers = 0;
	_xres = 0;
	_yres = 0;
	_running = false;
}

VideoGrabber::~VideoGrabber()
{
}

int xioctl(int fh, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

void VideoGrabber::InitBuffer(unsigned int buffer_size)
{
	_buffers = (buffer *)calloc(1, sizeof(*_buffers));

	if (!_buffers) {
		ExceptionHandler("Out of memory");
	}

	_buffers[0].length = buffer_size;
	_buffers[0].start = malloc(buffer_size);

	if (!_buffers[0].start) {
		ExceptionHandler("Out of memory");
	}
}

void VideoGrabber::InitSharedMemory()
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(_handler, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			ExceptionHandler(_device + " does not support memory mapping");
		} else {
			ExceptionHandler("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		ExceptionHandler("Insufficient buffer memory on " + _device);
	}

	_buffers = (buffer *)calloc(req.count, sizeof(*_buffers));

	if (!_buffers) {
		ExceptionHandler("Out of memory");
	}

	for (_n_buffers = 0; _n_buffers < req.count; ++_n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = _n_buffers;

		if (-1 == xioctl(_handler, VIDIOC_QUERYBUF, &buf))
			ExceptionHandler("VIDIOC_QUERYBUF");

		_buffers[_n_buffers].length = buf.length;
		_buffers[_n_buffers].start =
			mmap(NULL /* start anywhere */,
					buf.length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					_handler, buf.m.offset);

		if (MAP_FAILED == _buffers[_n_buffers].start)
			ExceptionHandler("mmap");
	}
}

void VideoGrabber::InitUserPtr(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(_handler, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			ExceptionHandler(_device + " does not support user pointer i/o");
		} else {
			ExceptionHandler("VIDIOC_REQBUFS");
		}
	}

	_buffers = (buffer *)calloc(4, sizeof(*_buffers));

	if (!_buffers) {
		ExceptionHandler("Out of memory");
	}

	for (_n_buffers = 0; _n_buffers < 4; ++_n_buffers) {
		_buffers[_n_buffers].length = buffer_size;
		_buffers[_n_buffers].start = malloc(buffer_size);

		if (!_buffers[_n_buffers].start) {
			ExceptionHandler("Out of memory");
		}
	}
}

void VideoGrabber::Configure(int width, int height)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1 == xioctl(_handler, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			ExceptionHandler(_device + " is no V4L2 device");
		} else {
			ExceptionHandler("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		ExceptionHandler(_device + " is no video capture device");
	}

	switch (_method) {
		case IO_METHOD_READ:
			if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
				ExceptionHandler(_device + " does not support read i/o");
			}
			break;

		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
				ExceptionHandler(_device + " does not support streaming i/o");
			}
			break;
	}

	// Select video input, video standard and tune here
	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl(_handler, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; // reset to default

		if (-1 == xioctl(_handler, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
				case EINVAL:
					// cropping not supported
					break;
				default:
					// Errors ignored
					break;
			}
		}
	} else {
		// Errors ignored
	}

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (width > 0 && height > 0) {
		fmt.fmt.pix.width       = width;
		fmt.fmt.pix.height      = height;
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

		if (-1 == xioctl(_handler, VIDIOC_S_FMT, &fmt)) {
			ExceptionHandler("VIDIOC_S_FMT");
		}

		if (-1 == xioctl(_handler, VIDIOC_G_FMT, &fmt)) {
			ExceptionHandler("VIDIOC_G_FMT");
		}

		// note VIDIOC_S_FMT may change width and height
	} else {
		// Preserve original settings as set by v4l2-ctl for example
		if (-1 == xioctl(_handler, VIDIOC_G_FMT, &fmt)) {
			ExceptionHandler("VIDIOC_G_FMT");
		}
	}

	if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
		_pixelformat = jgui::JPF_UYVY;
	} else if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB24) {
		_pixelformat = jgui::JPF_RGB24;
	} else if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB32) {
		_pixelformat = jgui::JPF_RGB32;
	} else {
		printf("[PIXEL FORMAT] 0x%8x\n", fmt.fmt.pix.pixelformat);

		ExceptionHandler("Not implemented to this pixel format");
	}

	_xres = fmt.fmt.pix.width;
	_yres = fmt.fmt.pix.height;

	printf("camera.mode:: [%d, %d] -> [%d, %d]\n", width, height, _xres, _yres);

	// buggy driver paranoia
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min) {
		fmt.fmt.pix.bytesperline = min;
	}
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min) {
		fmt.fmt.pix.sizeimage = min;
	}

	// disable auto-exposure
	struct v4l2_control control;

	memset(&control, 0, sizeof(control));

	// control.id = V4L2_CID_EXPOSURE_AUTO;
	control.id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
	control.value = AUTO_EXPOSURE_ENABLED;

	if (ioctl(_handler, VIDIOC_S_CTRL, &control) < 0) {
		printf("Couldn't set auto exposure !\n");
	}

	// get frame rate
	struct v4l2_streamparm fps;

	memset(&fps, 0, sizeof(fps));

	fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (ioctl(_handler, VIDIOC_G_PARM, &fps) >= 0) {
		printf("Frame Rate:: %d/%d\n", fps.parm.capture.timeperframe.numerator, fps.parm.capture.timeperframe.denominator);
	}

	fps.parm.capture.timeperframe.numerator = 1;
	fps.parm.capture.timeperframe.denominator = 30;

	if (ioctl(_handler, VIDIOC_S_PARM, &fps) < 0) {
		printf("Couldn't set v4l fps!\n");
	}

	switch (_method) {
		case IO_METHOD_READ:
			InitBuffer(fmt.fmt.pix.sizeimage);
			break;

		case IO_METHOD_MMAP:
			InitSharedMemory();
			break;

		case IO_METHOD_USERPTR:
			InitUserPtr(fmt.fmt.pix.sizeimage);
			break;
	}
}

void VideoGrabber::ReleaseDevice()
{
	unsigned int i;

	switch (_method) {
		case IO_METHOD_READ:
			free(_buffers[0].start);
			break;

		case IO_METHOD_MMAP:
			for (i = 0; i < _n_buffers; ++i)
				if (-1 == munmap(_buffers[i].start, _buffers[i].length))
					ExceptionHandler("munmap");
			break;

		case IO_METHOD_USERPTR:
			for (i = 0; i < _n_buffers; ++i)
				free(_buffers[i].start);
			break;
	}

	free(_buffers);

	if (-1 == close(_handler))
		ExceptionHandler("close");

	_handler = -1;
}

int VideoGrabber::GetFrame()
{
	struct v4l2_buffer buf;
	unsigned int i;

	switch (_method) {
		case IO_METHOD_READ:
			if (-1 == read(_handler, _buffers[0].start, _buffers[0].length)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						// could ignore EIO, see spec. fall through

					default:
						ExceptionHandler("read");
				}
			}

			if (_listener != NULL) {
				_listener->ProcessFrame((const uint8_t *)_buffers[0].start, _xres, _yres, _pixelformat);
				// _listener->ProcessFrame((const uint8_t *)buffers[0].start, buffers[0].length, _pixelformat);
			}
			break;

		case IO_METHOD_MMAP:
			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;

			if (-1 == xioctl(_handler, VIDIOC_DQBUF, &buf)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						// could ignore EIO, see spec. fall through

					default:
						ExceptionHandler("VIDIOC_DQBUF");
				}
			}

			if (buf.index >= _n_buffers) {
				ExceptionHandler("Buffer index is out of bounds");
			}

			if (_listener != NULL) {
				_listener->ProcessFrame((const uint8_t *)_buffers[buf.index].start, _xres, _yres, _pixelformat);
				// _listener->ProcessFrame((const uint8_t *)buffers[buf.index].start, buf.bytesused, _pixelformat);
			}

			if (-1 == xioctl(_handler, VIDIOC_QBUF, &buf))
				ExceptionHandler("VIDIOC_QBUF");
			break;

		case IO_METHOD_USERPTR:
			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;

			if (-1 == xioctl(_handler, VIDIOC_DQBUF, &buf)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						// could ignore EIO, see spec. fall through

					default:
						ExceptionHandler("VIDIOC_DQBUF");
				}
			}

			for (i = 0; i < _n_buffers; ++i)
				if (buf.m.userptr == (unsigned long)_buffers[i].start
						&& buf.length == _buffers[i].length)
					break;

			if (i >= _n_buffers) {
				ExceptionHandler("Buffer index is out of bounds");
			}

			if (_listener != NULL) {
				_listener->ProcessFrame((const uint8_t *)buf.m.userptr, _xres, _yres, _pixelformat);
				// _listener->ProcessFrame((const uint8_t *)buf.m.userptr, buf.bytesused, _pixelformat);
			}

			if (-1 == xioctl(_handler, VIDIOC_QBUF, &buf))
				ExceptionHandler("VIDIOC_QBUF");
			break;
	}

	return 1;
}

void VideoGrabber::ExceptionHandler(std::string msg)
{
	throw jcommon::RuntimeException(msg + ": " + strerror(errno));
}

void VideoGrabber::Open()
{
	struct stat st;

	if (-1 == stat(_device.c_str(), &st)) {
		ExceptionHandler("Cannot identify '" + _device + "': " + strerror(errno));
	}

	if (!S_ISCHR(st.st_mode)) {
		ExceptionHandler(_device + " is no device");
	}

	_handler = open(_device.c_str(), O_RDWR | O_NONBLOCK, 0);

	if (-1 == _handler) {
		ExceptionHandler("Cannot open '" + _device + "': " + strerror(errno));
	}

	_video_control = new VideoControl(_handler);
}

void VideoGrabber::Start()
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (_method) {
		case IO_METHOD_READ:
			// do nothing
			break;

		case IO_METHOD_MMAP:
			for (i = 0; i < _n_buffers; ++i) {
				struct v4l2_buffer buf;

				CLEAR(buf);
				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_MMAP;
				buf.index = i;

				if (-1 == xioctl(_handler, VIDIOC_QBUF, &buf))
					ExceptionHandler("VIDIOC_QBUF");
			}
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl(_handler, VIDIOC_STREAMON, &type))
				ExceptionHandler("VIDIOC_STREAMON");
			break;

		case IO_METHOD_USERPTR:
			for (i = 0; i < _n_buffers; ++i) {
				struct v4l2_buffer buf;

				CLEAR(buf);
				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_USERPTR;
				buf.index = i;
				buf.m.userptr = (unsigned long)_buffers[i].start;
				buf.length = _buffers[i].length;

				if (-1 == xioctl(_handler, VIDIOC_QBUF, &buf))
					ExceptionHandler("VIDIOC_QBUF");
			}
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl(_handler, VIDIOC_STREAMON, &type))
				ExceptionHandler("VIDIOC_STREAMON");
			break;
	}

	_running = true;

	jthread::Thread::Start();
}

void VideoGrabber::Pause()
{
	if (_running == true) {
		_running = false;

		jthread::Thread::WaitThread();
	}
}

void VideoGrabber::Resume()
{
	if (_running == false) {
		_running = true;

		jthread::Thread::Start();
	}
}

void VideoGrabber::Stop()
{
	_running = false;

	jthread::Thread::WaitThread();

	enum v4l2_buf_type type;

	switch (_method) {
		case IO_METHOD_READ:
			/* Nothing to do. */
			break;

		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl(_handler, VIDIOC_STREAMOFF, &type))
				ExceptionHandler("VIDIOC_STREAMOFF");
			break;
	}

	ReleaseDevice();
}

VideoControl * VideoGrabber::GetVideoControl()
{
	return _video_control;
}

void VideoGrabber::Run()
{
	while (_running) {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(_handler, &fds);

			// timeout
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			r = select(_handler + 1, &fds, NULL, NULL, &tv);

			if (-1 == r) {
				if (EINTR == errno) {
					continue;
				}

				ExceptionHandler("select");
			}

			if (0 == r) {
				ExceptionHandler("Select timeout");
			}

			if (GetFrame()) {
				break;
			}

			// EAGAIN - continue select loop
		}
	}
}


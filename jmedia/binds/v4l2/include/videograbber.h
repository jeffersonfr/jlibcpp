#ifndef __VIDEOGRABBER_PHOTOBOOTH_H
#define __VIDEOGRABBER_PHOTOBOOTH_H

#include "videocontrol.h"
#include "jimage.h"
#include "jthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

enum jcapture_method_t {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
};

struct buffer {
	void *start;
	size_t length;
};

class V4LFrameListener {

	protected:
		V4LFrameListener()
		{
		}

	public:

		virtual ~V4LFrameListener()
		{
		}

		virtual void ProcessFrame(const uint8_t *buffer, int width, int height, jgui::jpixelformat_t format)
		{
		}
};

class VideoGrabber : public jthread::Thread {

	private:
		/** \brief */
		VideoControl *_video_control;
		/** \brief */
		V4LFrameListener *_listener;
		/** \brief */
		std::string _device;
		/** \brief */
		jcapture_method_t _method;
		/** \brief */
		int _handler;
		/** \brief */
		struct buffer *buffers;
		/** \brief */
		unsigned int n_buffers;
		/** \brief */
		int out_buf;
		/** \brief */
		int xres;
		/** \brief */
		int yres;
		/** \brief */
		bool _running;
		/** \brief */
		jgui::jpixelformat_t _pixelformat;

	private:
		/**
		 * \brief
		 *
		 */
		void InitBuffer(unsigned int buffer_size);

		/**
		 * \brief
		 *
		 */
		void InitSharedMemory();

		/**
		 * \brief
		 *
		 */
		void InitUserPtr(unsigned int buffer_size);

		/**
		 * \brief
		 *
		 */
		void ReleaseDevice();

		/**
		 * \brief
		 *
		 */
		int GetFrame();

	public:
		/**
		 * \brief
		 *
		 */
		VideoGrabber(V4LFrameListener *listener, std::string device);

		/**
		 * \brief
		 *
		 */
		virtual ~VideoGrabber();

		/**
		 * \brief
		 *
		 */
		virtual void ExceptionHandler(std::string msg);

		/**
		 * \brief
		 *
		 */
		virtual void Open();
		
		/**
		 * \brief
		 *
		 */
		virtual void Configure(int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual void Start();

		/**
		 * \brief
		 *
		 */
		virtual void Pause();

		/**
		 * \brief
		 *
		 */
		virtual void Resume();

		/**
		 * \brief
		 *
		 */
		virtual void Stop();

		/**
		 * \brief
		 *
		 */
		virtual VideoControl * GetVideoControl();

		/**
		 * \brief
		 *
		 */
		virtual void Run();

};

#endif

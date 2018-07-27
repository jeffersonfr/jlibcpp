#ifndef J_VIDEOGRABBER_H
#define J_VIDEOGRABBER_H

#include "jgui/jimage.h"

#include <thread>

namespace jmedia {

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

class VideoControl;

class VideoGrabber {

	private:
		/** \brief */
		VideoControl *_video_control;
		/** \brief */
		V4LFrameListener *_listener;
		/** \brief */
    std::thread _thread;
		/** \brief */
		struct buffer *_buffers;
		/** \brief */
		std::string _device;
		/** \brief */
		uint32_t _n_buffers;
		/** \brief */
		int _handler;
		/** \brief */
		int _xres;
		/** \brief */
		int _yres;
		/** \brief */
		bool _running;
		/** \brief */
		jcapture_method_t _method;
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

}

#endif

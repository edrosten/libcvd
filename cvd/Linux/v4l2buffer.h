//-*- c++ -*-
#ifndef __CVD_V4L2BUFFER_H
#define __CVD_V4L2BUFFER_H

#include <string>

#include <cvd/videobuffer.h>
#include <cvd/Linux/v4l2frame.h>
#include <linux/videodev.h>    // v4l2 - video for linux 2

#define V4L2BUFFERS 3

namespace CVD {

/// Which buffer block method shall I use?
enum V4L2BufferBlockMethod{
  V4L2BBMselect,
  V4L2BBMsleep,
  V4L2BBMchew
};

/*
enum V4L2BufferFrameOrder{
  MostRecent,
  InOrder,
};*/

namespace Exceptions
{
	/// %Exceptions specific to V4L2Buffer
	/// @ingroup gException
	namespace V4L2Buffer
	{
		/// Base class for all V4L2 exceptions
		struct All: public CVD::Exceptions::VideoBuffer::All{};
		/// Error opening the device
		struct DeviceOpen: public All {DeviceOpen(std::string dev); ///< Construct from the device name
		};
		/// Error setting up the device
		struct DeviceSetup: public All {DeviceSetup(std::string dev, std::string action);  ///< Construct from the device string and an error string
		};
		/// Error in a put_frame() call
		struct PutFrame: public All {PutFrame(std::string dev); ///< Construct from the device name
		};
		/// Error in a get_frame() call
		struct GetFrame: public All {GetFrame(std::string dev); ///< Construct from the device name
		};
	}
}


/// A live video buffer from a the framegrabber (using the Video for Linux 2 API).
/// This provides 8-bit greyscale video frames of type CVD::V4L2Frame and throws exceptions
/// of type CVD::Exceptions::V4L2Buffer
/// @ingroup gVideoBuffer
class V4L2Buffer : public VideoBuffer<unsigned char> 
{
	public:
		/// Construct a video buffer
		/// @param devname The device name
		/// @param fields
		/// @param block Which buffer block method to use
		/// @param numbufs How many buffers?
		V4L2Buffer(const char *devname, bool fields, V4L2BufferBlockMethod block, int numbufs=V4L2BUFFERS);
		~V4L2Buffer();

		virtual ImageRef size() 
		{
			return my_image_size;
		}

		virtual V4L2Frame* get_frame(); 
		/// Tell the buffer that you are finished with this frame. Overloaded version of VideoBuffer<T>::put_frame()
		virtual void put_frame(VideoFrame<unsigned char>* f);
		/// Tell the buffer that you are finished with this frame. Overloaded version of VideoBuffer<T>::put_frame()
		virtual void put_frame(V4L2Frame *f);
		virtual bool frame_pending();

		virtual double frame_rate() 
		{
			return my_frame_rate;
		}

	private:
		std::string device;
		int my_dropped_frames;
		int my_prev_frame_no;
		int num_buffers;
		bool i_am_using_fields;
		double my_frame_rate;
		ImageRef my_image_size;
		V4L2BufferBlockMethod my_block_method;
		struct v4l2_buffer* m_sv4l2Buffer;
		void** m_pvVideoBuffer;
		int m_nVideoFileDesc;
		//struct v4l2_performance m_sv4l2Performance;

		int my_fd;

		V4L2Buffer( V4L2Buffer& copyof );
		int operator = ( V4L2Buffer& copyof );

};

}

#endif

















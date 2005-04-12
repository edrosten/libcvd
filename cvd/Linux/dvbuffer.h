// -*- c++ -*-
#ifndef __DVBUFFER_H
#define __DVBUFFER_H

#include <cvd/videobuffer.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <vector>

namespace CVD {

namespace DC
{
	template<class C> struct cam_type
	{
		static const int mode = Error__type_not_valid_for_camera___Use_byte_or_rgb_of_byte;
		static const int fps  = Error__type_not_valid_for_camera___Use_byte_or_rgb_of_byte;
	};
	
	
	template<> struct cam_type<byte>
	{
		static const int mode = MODE_640x480_MONO;
		static const double fps  = 30;
	};
	
	template<> struct cam_type<Rgb<byte> >
	{
		static const int mode = MODE_640x480_RGB;
		static const double  fps  = 30;
	};

	struct raw_frame
	{
		unsigned char* data;
		double timestamp;
		int buffer;
	};

	class RawDCVideo
	{
		public:
			RawDCVideo(int camera_no, int num_dma_buffers, int bright, int exposure, int mode, double frame_rate);
			~RawDCVideo();

			ImageRef size();
			VideoFrame<byte>* get_frame();
			void put_frame(VideoFrame<byte>* f);
			bool frame_pending();


			void set_shutter(unsigned int);
			unsigned int get_shutter();

			void set_iris(unsigned int);
			unsigned int get_iris();

			void set_gain(unsigned int);
			unsigned int get_gain();

			void set_exposure(unsigned int);
			unsigned int get_exposure();

			void set_brightness(unsigned int);
			unsigned int get_brightness();

			double frame_rate();

			
			raw1394handle_t& handle();
			nodeid_t&		 node();

		private:

			// components needed for the DMA based video capture
			int my_channel;
			unsigned char* my_ring_buffer;
			int my_frame_size;
			int my_num_buffers;
			// int my_most_recent_frame; // the most recently filled buffer
			int my_fd; // fd of the mmaped dma ring buffer
			raw1394handle_t my_handle;
			nodeid_t * my_camera_nodes; // member variable I guess unless we can make it be released
			nodeid_t my_node;
			ImageRef my_size;

			std::vector<int> my_frame_sequence;
			int my_next_frame;
			int my_last_in_sequence;
			double true_fps;
	};
		
}

template<class T> class DVBuffer2: public VideoBuffer<T>, public DC::RawDCVideo
{
	public:
		DVBuffer2(int cam_no, int num_dma_buffers, int bright=-1, int exposure=-1, double fps=DC::cam_type<T>::fps)
		:RawDCVideo(cam_no, num_dma_buffers, bright, exposure, DC::cam_type<T>::mode, fps)
		{
		}

		virtual ~DVBuffer2()
		{
		}

		double frame_rate()
		{
			return RawDCVideo::frame_rate();
		}

		virtual ImageRef size()
		{
			return RawDCVideo::size();
		}

		virtual VideoFrame<T>* get_frame()
		{
			return reinterpret_cast<VideoFrame<T>*>(RawDCVideo::get_frame());
		}
		
		virtual void put_frame(VideoFrame<T>* f)
		{
			RawDCVideo::put_frame(reinterpret_cast<VideoFrame<byte>*>(f));
		}

		virtual bool frame_pending()
		{
			return RawDCVideo::frame_pending();
		}
	
		virtual void seek_to(double){}
};

typedef DVBuffer2<byte> DVBuffer;

}

#endif

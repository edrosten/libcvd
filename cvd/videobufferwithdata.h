/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef CVD_VIDEOBUFFERWITHDATA_H
#define CVD_VIDEOBUFFERWITHDATA_H

#include <cvd/videobuffer.h>
#include <memory>

namespace CVD {

/// Certain video buffers, especially the decorator classes, and buffers
/// such as ServerPushJpegBuffer have additional data 
/// with the same lifetime as the buffer. This is a tool to allow management of
/// this data. This class manages a video buffer and some data concurrently.
/// @param T The pixel type of the video frames
/// @ingroup gVideoBuffer
template <class T, class D> 
class VideoBufferWithData: public VideoBuffer<T> 
{
	public: 
		VideoBufferWithData(std::auto_ptr<VideoBuffer<T> >& buf_, std::auto_ptr<D>& d)
		:VideoBuffer<T>(buf_->type()), buf(buf_),extra_data(d)
		{}

		ImageRef size()
		{
			return buf->size();
		}

		virtual RawVideoBuffer* source_buffer()
		{
			return buf.get();
		}

		VideoFrame<T>* get_frame()
		{
			return buf->get_frame();
		}

		void put_frame(VideoFrame<T>* f)
		{
			buf->put_frame(f);
		}	

		bool frame_pending()
		{
			return buf->frame_pending();
		}	

		void flush()
		{
			return buf->flush();
		}	

		double frame_rate()
		{
			return buf->frame_rate();
		}

		double seek_to()
		{
			return buf->seek_to();
		}


	private:
		std::auto_ptr<VideoBuffer<T> > buf;
	public:
		std::auto_ptr<D> extra_data;
};

}

#endif

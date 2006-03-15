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
#ifndef _SGI_VIDEO_H
#define _SGI_VIDEO_H

#include <dmedia/dmedia.h>
#include <dmedia/vl.h>
#include <dmedia/dm_buffer.h>
#include <dmedia/dm_params.h>
//#include <cvd_image.h>
#include <cvd/videoframe.h>


namespace CVD
{

/// Internal O2Buffer helpers
namespace SGI
{
	/// Set this dodgy flag for debugging
	extern int debug;
	/// Set this dodgy flag for debugging
	extern int debug_sgi_events;
	
	/// Get current time since the epoch in SGI-time
	unsigned long long 	current_time();
	/// Divisor to turn current_time() in to seconds
	extern const unsigned long long	time_div;
	
	/// Internal (non type-safe) class used by O2Buffer to do the actual interfacing with the
	/// SGI video hardware. 
	class RawSGIVideo
	{
		public:
			
			/// Construct SGI video grabber
			/// @param use_frames Should the grabber use frames (true) or fields (false)
			RawSGIVideo(bool use_frames=false);
			~RawSGIVideo();
			

			///	Start capturing
			void 	start();
			/// Stop capturing
			void 	stop();

			/// Get data pointer to the next frame
			unsigned char*	next_frame();
			
			/// Width of images returned by the buffer
			int		x_size() const;
			
			/// Height of images returned by the buffer
			int		y_size() const ;
			
			/// Number of bytes in a frame 
			int		frame_size() const;

			/// Frames per second of capture
			float	frame_rate() const ;

			/// Sequence number of frame captured
			int		frame_number() const;

			/// Pointer to most recently retrieved frame
			unsigned char*	current_frame() const;	
			
			/// Time at which frame was captured
			unsigned long long  frame_time() const;


		private:

			int			my_frame_number;
			int			my_frame_size;
			int			my_x_size;
			int			my_y_size;
			float		my_frame_rate;
			unsigned char*		my_current_frame;
			unsigned long long  my_timestamp;
		
			int			frame_number_div;
			int			frame_no;
		

			VLServer	svr;
			VLPath		vid_in;
			VLNode		source, drain;
			DMbuffer	buf[2];				
			int			locked_buffer;
			DMbufferpool		buff;
			bool		started;	
	};
			
}

}
#endif

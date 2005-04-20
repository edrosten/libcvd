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
	extern int debug;
	extern int debug_sgi_events;

	unsigned long long 	current_time();
	extern const unsigned long long	time_div;
	
	/// Internal (non type-safe) class used by O2Buffer to do the actual interfacing with the
	/// SGI video hardware. 
	class RawSGIVideo
	{
		public:
			
			RawSGIVideo(bool=false);
			~RawSGIVideo();

			void 	start();
			void 	stop();

			unsigned char*	next_frame();

			int		x_size() const;
			int		y_size() const ;
			int		frame_size() const;
			float	frame_rate() const ;
			int		frame_number() const;

			unsigned char*	current_frame() const;	

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

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
#ifndef CVD_KERNEL_VIDEO1394_H
#define CVD_KERNEL_VIDEO1394_H

//Header file which contains stuff which is 100% compatible with 
//drivers/ieee1394/video1394.h
//without including any GPL code.


#include <sys/ioctl.h>
#include <sys/time.h>

#define SYNC_FRAMES      1

struct cvd_video1394_mmap 
{
	int channel_number;
	unsigned int syncronization_tag;
	unsigned int num_buffers;
	unsigned int buffer_size;
	unsigned int size_of_packet;
	unsigned int frames_per_second;
	unsigned int syt_time_offset;
	unsigned int capture_flags;		
};

struct cvd_video1394_wait 
{
	unsigned int channel_number;
	unsigned int buffer;
	struct timeval time;				//Buffer filled up at this time
};


//Define the ioctls

#if CVD_KERNEL_MAJOR==2 
	#if CVD_KERNEL_MINOR==6
		#define LISTEN_CHANNEL		 	_IOWR('#', 0x10, struct cvd_video1394_mmap)
		#define UNLISTEN_CHANNEL		_IOW ('#', 0x11, int)
		#define LISTEN_QUEUE_BUFFER	 	_IOW ('#', 0x12, struct cvd_video1394_wait)
		#define LISTEN_WAIT_BUFFER	 	_IOWR('#', 0x13, struct cvd_video1394_wait)
		#define LISTEN_POLL_BUFFER	 	_IOWR('#', 0x18, struct cvd_video1394_wait)
	#elif CVD_KERNEL_MINOR==4
		#define LISTEN_CHANNEL  		0
		#define UNLISTEN_CHANNEL  		1
		#define LISTEN_QUEUE_BUFFER  	2
		#define LISTEN_WAIT_BUFFER  	3
		#define LISTEN_POLL_BUFFER  	8	 
	#else
		#define UNKNOWN_KERNEL
	#endif
#else 
	#define UNKNOWN_KERNEL
#endif

#endif

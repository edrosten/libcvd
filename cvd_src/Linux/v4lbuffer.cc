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
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cvd/Linux/v4lbuffer.h>
#include <cvd/timer.h>

#include <vector>
using namespace std;

namespace CVD { // CVD

Exceptions::V4LBuffer::DeviceOpen::DeviceOpen(string device)
{
    what = "V4LBuffer: failed to open \""+device+ "\": " + strerror(errno);
}

Exceptions::V4LBuffer::DeviceSetup::DeviceSetup(string device, string action)
{
    what = "V4LBuffer: \""+action + "\" ioctl failed on " + device + ": " +strerror(errno);
}

Exceptions::V4LBuffer::PutFrame::PutFrame(string device, string msg)
{
    what = "V4LBuffer: PutFrame on " + device + " failed: " + msg;
}

Exceptions::V4LBuffer::GetFrame::GetFrame(string device, string msg)
{
    what = "V4LBuffer: GetFrame on " + device + " failed: " + msg;
}

namespace V4L { // V4L

    struct V4L2Client::State {
	struct Frame {
	    void* data;
	    size_t length;
	};
	int fd;
	v4l2_format format;
	vector<Frame> frames;
	double frameRate;
	v4l2_buffer refbuf;
    };

  V4L2Client::V4L2Client(int fd, unsigned int fmt, ImageRef size, int input, bool fields, int frames_per_second)
    {
	state = 0;

	struct v4l2_capability caps;
	if (0 != ioctl(fd, VIDIOC_QUERYCAP, &caps))
	    throw string("VIDIOC_QUERYCAP failed");
	
	if (strcmp((const char*)caps.driver, "bttv")==0) {
	    v4l2_std_id stdId=V4L2_STD_PAL;
	    if(ioctl(fd, VIDIOC_S_STD, &stdId ))
		throw string("VIDIOC_S_STD");
	}
	
	if (input != -1) {
	    struct v4l2_input v4l2Input;
	    v4l2Input.index=input; 
	    if (0 != ioctl(fd, VIDIOC_S_INPUT, &v4l2Input))
		throw string("VIDIOC_S_INPUT");
	}

	// Get / Set capture format.
	struct v4l2_format format;
	format.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	if (0 != ioctl(fd, VIDIOC_G_FMT, &format))
	    throw string("VIDIOC_G_FMT");
	
	format.fmt.pix.width = size.x;
	format.fmt.pix.height = size.y;
	format.fmt.pix.pixelformat = fmt;
	format.fmt.pix.field = fields ? V4L2_FIELD_ALTERNATE : V4L2_FIELD_ANY;
	
	if (0 != ioctl(fd, VIDIOC_S_FMT, &format))
	    throw string("VIDIOC_S_FMT");

	if (0 != ioctl(fd, VIDIOC_G_FMT, &format))
	    throw string("VIDIOC_G_FMT");
	
	if (fmt != format.fmt.pix.pixelformat)
	    throw string("Requested format not supported");
	
	struct v4l2_requestbuffers reqbufs;
	reqbufs.count = 10;
	reqbufs.memory = V4L2_MEMORY_MMAP;
	reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (0 != ioctl(fd,VIDIOC_REQBUFS,&reqbufs))
	    throw string("VIDIOC_REQBUFS");

	num_bufs = reqbufs.count;
	
	if (reqbufs.count < 2)
	    throw string("Insufficient buffers available");
	vector<State::Frame> frames(reqbufs.count);
	struct v4l2_buffer refbuf;
	for (size_t i=0; i<frames.size(); i++) {
	    struct v4l2_buffer buffer;
	    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	    buffer.memory = V4L2_MEMORY_MMAP;
	    buffer.index = i;
	    if (0 != ioctl(fd, VIDIOC_QUERYBUF, &buffer))
		throw string("VIDIOC_QUERYBUF");
	    if (i == 0)
		refbuf = buffer;
	    frames[i].data = mmap(0,buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buffer.m.offset);
	    if (frames[i].data == MAP_FAILED) {
		frames[i].data = mmap(0,buffer.length, PROT_READ, MAP_SHARED, fd, buffer.m.offset);
		if (frames[i].data == MAP_FAILED)
		    throw string("mmap failed");
	    }
	    frames[i].length = buffer.length;
	    if (0 != ioctl(fd, VIDIOC_QBUF, &buffer))
		throw string("VIDIOC_QBUF");
	}

	// Do we want to manually set FPS?
	if(frames_per_second != 0) 
	  {
	    v4l2_streamparm streamparams;
	    streamparams.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	    if (0 != ioctl(fd, VIDIOC_G_PARM, &streamparams))
	      throw string("VIDIOC_G_PARM");
	    
	    // Check if the device has the capability to set a frame-rate.
	    if(streamparams.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
	      {
		streamparams.parm.capture.timeperframe.denominator = frames_per_second;
		streamparams.parm.capture.timeperframe.numerator = 1;
		if (0 != ioctl(fd, VIDIOC_S_PARM, &streamparams))
		  throw string("VIDIOC_S_PARM");
	      }
	  }

	if (0 != ioctl(fd, VIDIOC_STREAMON, &reqbufs.type))
	    throw string("STREAMON");
	state = new State;
	state->fd = fd;
	state->format = format;
	state->frames = frames;	
	state->frameRate = 0;
	state->refbuf = refbuf;
    }
    V4L2Client::~V4L2Client() {
	if (state == 0)
	    return;
	for (size_t i=0; i<state->frames.size(); i++) {
	    if (0 != munmap(state->frames[i].data, state->frames[i].length))
		throw string("munmap failed");
	}
	close(state->fd);
	delete state;
    }

    ImageRef V4L2Client::getSize()
    {
	return ImageRef(state->format.fmt.pix.width, state->format.fmt.pix.height);
    }
    
    V4L2Client::Buffer V4L2Client::getFrame()
    {
	struct v4l2_buffer buffer = state->refbuf;
	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;
	int err = ioctl(state->fd, VIDIOC_DQBUF, &buffer);
	while (err != 0 && errno == EAGAIN) {
	    usleep(10);
	    err = ioctl(state->fd, VIDIOC_DQBUF, &buffer);
	}
	if (err != 0)
	    throw string("VIDIOC_DQBUF");

	Buffer ret;
	ret.id = buffer.index;
	ret.data = static_cast<unsigned char*>(state->frames[buffer.index].data);
	ret.when = timer.conv_ntime(buffer.timestamp);
	return ret;
    }
    
    void V4L2Client::releaseFrame(int id)
    {
	struct v4l2_buffer buffer = state->refbuf;
	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;
	buffer.index = id;
	
	if (0 != ioctl(state->fd, VIDIOC_QBUF, &buffer))
	    throw string("VIDIOC_QBUF");
    }

    double V4L2Client::getRate() {
	return state->frameRate;
    }

    bool V4L2Client::pendingFrame() {
	fd_set fdsetRead;
	fd_set fdsetOther;
	struct timeval tv;
 
        FD_ZERO(&fdsetRead);
	FD_SET(state->fd,&fdsetRead);
	FD_ZERO(&fdsetOther);
	tv.tv_sec=0;
	tv.tv_usec=0;
	if (select(state->fd+1,&fdsetRead,&fdsetOther,&fdsetOther,&tv)>0)
		return true;
	return false;
    }

};

};

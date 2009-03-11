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
//-*- c++ -*-
#ifndef CVD_V4LBUFFER_H
#define CVD_V4LBUFFER_H

#include <cvd/config.h>

#include <vector>

#ifdef CVD_INTERNAL_HAVE_STRANGE_V4L2
	#include <videodevx/videodev.h>
#else
	#include <linux/videodev.h>
#endif



#include <cvd/videobuffer.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/rgb8.h>
#include <cvd/timer.h>
#include <cvd/colourspaces.h>
#include <fcntl.h>

namespace CVD {

namespace Exceptions
{
    /// @ingroup gException
    namespace V4LBuffer
    {
        /// @ingroup gException
        struct All: public CVD::Exceptions::VideoBuffer::All
	{
	};
        /// Error opening the device
	/// @ingroup gException
	struct DeviceOpen: public All {DeviceOpen(std::string dev); ///< Construct from the device name
	};
        /// Error setting up the device
	/// @ingroup gException
	struct DeviceSetup: public All {DeviceSetup(std::string dev, std::string action);  ///< Construct from the device string and an error string
	};
        /// Error in a put_frame() call
	/// @ingroup gException
	struct PutFrame: public All {PutFrame(std::string dev, std::string msg); ///< Construct from the device name
	};
        /// Error in a get_frame() call
	/// @ingroup gException
	struct GetFrame: public All {GetFrame(std::string dev, std::string msg); ///< Construct from the device name
        };
    }
}

namespace V4L
{
#ifndef DOXYGEN_IGNORE_INTERNAL
    template<class C> struct format;

    template<> struct format<byte>
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_GREY;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_GREY;
    };
	
    #ifdef V4L2_PIX_FMT_SBGGR8
    template<> struct format<bayer_grbg>
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_SBGGR8;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_RAW;
    };
    template<> struct format<bayer_bggr>
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_SBGGR8;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_RAW;
    };
    #endif

    template<> struct format<yuv422>
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_YUYV;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_YUV422;
    };

    template<> struct format<vuy422>
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_UYVY;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_UYVY;
    };

    template<> struct format<yuv420p>
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_YUV420;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_YUV420P;
    };

    template<> struct format<Rgb<byte> >
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_RGB24;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_RGB24;
    };

    template<> struct format<Rgb8>
    {
		static const unsigned int v4l2_fmt = V4L2_PIX_FMT_RGB32;
        static const unsigned int v4l1_palette = VIDEO_PALETTE_RGB32;
    };
#endif
    
    class V4L2Client
    {
    public:
	struct Buffer {
	    int id;
	    unsigned char* data;
	    double when;
	};
	V4L2Client(int fd, unsigned int fmt, ImageRef size, int input, bool fields, int frame_per_second, bool verbose);
	ImageRef getSize();
	Buffer getFrame();
	void releaseFrame(int id);
	double getRate();
	bool pendingFrame();
	virtual ~V4L2Client();
	int num_buffers()
	{
		return num_bufs;
	}
    private:
	int num_bufs;
	struct State; 
	State* state;
    };
    
    class V4L1Client;
    
};


/// A live video buffer which uses the Video for Linux 2 (V4L2) API.
/// A replacement for the (deprecated?) V4L2Buffer
/// @ingroup gVideoBuffer
template <class T> class V4LBuffer : public VideoBuffer<T>
{
public:
 V4LBuffer(const std::string & dev, ImageRef size, int input=-1, bool fields=false, int frames_per_second=0, bool verbose=0) 
 :VideoBuffer<T>(VideoBufferType::Flushable), 
  devname(dev)
    {
	int device = open(devname.c_str(), O_RDWR | O_NONBLOCK);
	if (device == -1)
	    throw Exceptions::V4LBuffer::DeviceOpen(dev);
	try {
	  v4l2 = new V4L::V4L2Client(device, V4L::format<T>::v4l2_fmt, size, input, fields,frames_per_second, verbose);
	}
	catch (std::string& s) {
	    v4l2 = 0;
	    throw Exceptions::V4LBuffer::DeviceSetup(devname, "V4L2: "+s);
	}
	v4l1 = 0;
    }
	virtual ImageRef size() { return v4l2 ? v4l2->getSize() : ImageRef(-1,-1); }
    virtual VideoFrame<T> * get_frame() {
	if (v4l2) {
	    try {
		V4L::V4L2Client::Buffer buffer = v4l2->getFrame();
		return new V4LFrame(buffer.id, buffer.when, buffer.data, v4l2->getSize());
	    } catch (std::string& s) {
		throw Exceptions::V4LBuffer::GetFrame(devname, "V4L2: "+s);
	    }
	} else {
	    throw Exceptions::V4LBuffer::GetFrame(devname, "V4L1 not yet supported by V4LBuffer");
	}
    }
    virtual void put_frame(VideoFrame<T>* f) {
	V4LFrame* vf = dynamic_cast<V4LFrame*>(f);
	if (vf == 0)
	    throw Exceptions::V4LBuffer::PutFrame(devname, "Invalid VideoFrame");
	int id = vf->id;
	delete vf;
	if (v4l2) {
	    try {
		v4l2->releaseFrame(id);
	    } catch (std::string& s) {
		throw Exceptions::V4LBuffer::PutFrame(devname, "V4L2: "+s);
	    }
	} else {
	    throw Exceptions::V4LBuffer::PutFrame(devname, "V4L1 not yet supported by V4LBuffer");
	}
    }
    virtual bool frame_pending() { return v4l2->pendingFrame(); }
    virtual double frame_rate() { return v4l2 ? v4l2->getRate() : 0.0; }
    virtual ~V4LBuffer() {
	if (v4l2)
	    delete v4l2;
    }

	int num_buffers()
	{
	if (v4l2) {
		return v4l2->num_buffers();
	} else {
	    throw Exceptions::V4LBuffer::PutFrame(devname, "V4L1 not yet supported by V4LBuffer");
	}
	}

    const std::string & device_name() const { return devname; }
    
 private:
    struct V4LFrame : public VideoFrame<T> {
	V4LFrame(int i, double t, void* data, ImageRef size) : VideoFrame<T>(t,reinterpret_cast<T*>(data),size), id(i) {}
	int id;
	friend class V4LBuffer<T>;
    };
    std::string devname;
    V4L::V4L2Client* v4l2;
    V4L::V4L1Client* v4l1;
    V4LBuffer( V4LBuffer& copyof ) {}
    void operator=( V4LBuffer& copyof ) {}
};

};
#endif

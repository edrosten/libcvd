#include <sstream>
#include <algorithm>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "cvd/Linux/v4l1buffer.h"
#include "cvd/timer.h"

using namespace std;

namespace CVD {

Exceptions::V4L1Buffer::DeviceOpen::DeviceOpen(string device)
{
    what = "V4L1Buffer: failed to open \""+device+ "\": " + strerror(errno);
}

Exceptions::V4L1Buffer::DeviceSetup::DeviceSetup(string device, string action)
{
    what = "V4L1Buffer: \""+action + "\" ioctl failed on " + device + ": " +strerror(errno);
}

Exceptions::V4L1Buffer::PutFrame::PutFrame(string device, unsigned int buffer)
{
    std::ostringstream o;
    o << buffer;
    what = "V4L1Buffer: Enabling frame " + o.str() + " for capture failed on " + device;
}

Exceptions::V4L1Buffer::GetFrame::GetFrame(string device, unsigned int buffer)
{
    std::ostringstream o;
    o << buffer;
    what = "V4L1Buffer: Syncing to frame " + o.str() + " failed on " + device;
}

namespace V4L1 {

unsigned int getPaletteDepth(unsigned int p) {
    switch (p) {
    case VIDEO_PALETTE_YUV422: return 16;
    case VIDEO_PALETTE_RGB24: return 24;
    case VIDEO_PALETTE_YUV420P: return 12;
    case VIDEO_PALETTE_RAW: return 8;
    case VIDEO_PALETTE_GREY: return 8;
    default: return 0;
    }
}

RawV4L1::RawV4L1(const std::string & dev, unsigned int mode, const ImageRef& size)
    : deviceName( dev ), myDevice(-1), mySize(640,480), myPalette(mode),
      myBrightness(0.5), myWhiteness(0.5), myContrast(0.5), myHue(0.5), mySaturation(0.5),
      myBpp(16)
{
    myDevice = open(dev.c_str(), O_RDWR);
    if (myDevice == -1)
        throw Exceptions::V4L1Buffer::DeviceOpen(deviceName);
    struct video_capability caps;
    if (ioctl(myDevice, VIDIOCGCAP, & caps) != 0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "get capabilities");
	

	
	if(size.x != 0)
	{
		struct video_window window;
		if (ioctl (myDevice, VIDIOCGWIN, &window) != 0)
			throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "get window");

		window.width  = size.x;
		window.height = size.y;
		if (ioctl (myDevice, VIDIOCSWIN, &window) != 0)
			throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "set window");
	}

    struct video_mbuf mbuf;
    if (ioctl(myDevice, VIDIOCGMBUF, &mbuf) != 0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "get memory buffer info");

	mmaped_len = mbuf.size;
    mmaped_memory = mmap(0, mbuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, myDevice, 0);
    if (mmaped_memory == MAP_FAILED)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "mmap()");

	unsigned char* theMap = static_cast<unsigned char*>(mmaped_memory);

    myFrameBuf.resize(mbuf.frames);
    myFrameBufState.resize(myFrameBuf.size(), false);
    myNextRetrieveBuf = 0;
    for (unsigned int i=0;i<myFrameBuf.size();++i)
    {
        myFrameBuf[i]=theMap+mbuf.offsets[i];
    }
    retrieveSettings();
    set_palette( mode );
    commitSettings();

    // start capturing into all buffers
    for(unsigned int i=0; i < myFrameBuf.size(); i++)
        captureFrame(i);

    myNextRetrieveBuf = myFrameBuf.size() - 1;
}

RawV4L1::~RawV4L1()
{
    if (myDevice != -1)
	{
        close(myDevice);
		munmap(mmaped_memory, mmaped_len);
	}
}

void RawV4L1::retrieveSettings()
{
    struct video_window win;
    if(ioctl(myDevice, VIDIOCGWIN, &win) != 0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "retrieve videow_window");
    mySize = ImageRef(win.width, win.height);

    struct video_picture pic;
    if(ioctl(myDevice, VIDIOCGPICT, &pic) != 0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "retrieve video_picture");
    myBrightness = pic.brightness/65535.0;
    myWhiteness = pic.whiteness/65535.0;
    mySaturation = pic.colour/65535.0;
    myContrast = pic.contrast/65535.0;
    myHue = pic.hue/65535.0;
    myBpp = pic.depth;
    myPalette = pic.palette;
}

void RawV4L1::commitSettings()
{
    struct video_window win;
    if(ioctl(myDevice, VIDIOCGWIN, &win) !=0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "get video_window");
    win.x=win.y=0;
    win.width=mySize.x;
    win.height=mySize.y;
    if(ioctl(myDevice, VIDIOCSWIN, &win) !=0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "set video_window");

    struct video_picture pic;
    if(ioctl(myDevice, VIDIOCGPICT, &pic) !=0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "get video_picture");

	again:
    pic.brightness=(unsigned short)(myBrightness*65535+0.5);
    pic.whiteness=(unsigned short)(myWhiteness*65535+0.5);
    pic.colour=(unsigned short)(mySaturation*65535+0.5);
    pic.contrast=(unsigned short)(myContrast*65535+0.5);
    pic.hue=(unsigned short)(myHue*65535+0.5);
    pic.depth=myBpp;
    pic.palette=myPalette;
    if(ioctl(myDevice, VIDIOCSPICT, &pic) !=0)
	{
		if(myPalette == VIDEO_PALETTE_GREY)
		{
			myPalette = VIDEO_PALETTE_YUV420P;
			goto again;
		}
		else
			throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "set video_picture");
	}

    retrieveSettings();
}

const ImageRef& RawV4L1::get_size() const { return mySize; }

void RawV4L1::set_size(const ImageRef& size) { mySize = size; }

void RawV4L1::set_brightness(double brightness) { myBrightness = brightness; }

void RawV4L1::set_whiteness(double whiteness) { myWhiteness = whiteness; }

void RawV4L1::set_hue(double hue) { myHue = hue; }

void RawV4L1::set_contrast(double contrast) { myContrast = contrast; }

void RawV4L1::set_saturation(double saturation) { mySaturation = saturation; }

void RawV4L1::set_auto_exp(bool on){
    int val = on;
    if( val != autoexp ){
        autoexp = val;
        ioctl(myDevice, (VIDIOCCAPTURE), &autoexp);
    }
}

bool RawV4L1::get_auto_exp(void){
    return autoexp;
}

void RawV4L1::set_palette(unsigned int palette) {
    myPalette = palette;
    myBpp = getPaletteDepth(palette);
    if( myBpp == 0)
        throw Exceptions::V4L1Buffer::DeviceSetup(deviceName, "compute palette depth, unknow palette");
}

void RawV4L1::captureFrame( unsigned int buffer )
{
    if (myFrameBufState[buffer])
        return;
    struct video_mmap mm;
    mm.frame = buffer;
    mm.width = mySize.x;
    mm.height = mySize.y;
    mm.format = myPalette;
    if (ioctl(myDevice, VIDIOCMCAPTURE, &mm) != 0)
        throw Exceptions::V4L1Buffer::PutFrame(deviceName, buffer);
}

unsigned char* RawV4L1::get_frame()
{
    unsigned int nextBuffer = myNextRetrieveBuf;
    do {
        nextBuffer = (nextBuffer+1) % myFrameBuf.size();
    }
    while( myFrameBufState[myNextRetrieveBuf] && nextBuffer != myNextRetrieveBuf);
    if( myNextRetrieveBuf == nextBuffer )
        throw Exceptions::V4L1Buffer::GetFrame(deviceName, myNextRetrieveBuf);
    myNextRetrieveBuf = nextBuffer;
    int err = ioctl(myDevice, VIDIOCSYNC, &myNextRetrieveBuf);
    while( err < 0 && errno == EINTR )
    {
        err = ioctl(myDevice, VIDIOCSYNC, &myNextRetrieveBuf);
        cout << ".";
        cout.flush();
    }
    if(err < 0 )
        throw Exceptions::V4L1Buffer::GetFrame(deviceName, myNextRetrieveBuf);
    unsigned char* ret = (unsigned char*)myFrameBuf[myNextRetrieveBuf];
    myFrameBufState[myNextRetrieveBuf] = true;
    return ret;
}

void RawV4L1::put_frame(unsigned char * data)
{
    vector<unsigned char *>::iterator t = find(myFrameBuf.begin(), myFrameBuf.end(), data);
    if( t != myFrameBuf.end())
    {
        unsigned int number = t - myFrameBuf.begin();
        myFrameBufState[number] = false;
        captureFrame(number);
    }
}

double RawV4L1::frame_rate()
{
    return 30.0;
}

bool RawV4L1::frame_pending()
{
    fd_set fdsetRead;
    fd_set fdsetOther;
    struct timeval tv;

    FD_ZERO(&fdsetRead);
    FD_SET(myDevice,&fdsetRead);
    FD_ZERO(&fdsetOther);
    tv.tv_sec=0;
    tv.tv_usec=0;
    if (select(myDevice+1,&fdsetRead,&fdsetOther,&fdsetOther,&tv)>0)
        return true;
    return false;
}

};

};

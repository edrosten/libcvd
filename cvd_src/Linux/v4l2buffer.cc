#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <errno.h>
#include <string.h>

#include "cvd/Linux/v4l2buffer.h"
#include "cvd/timer.h"

#include <sys/types.h>   // included for open()
#include <sys/stat.h>    // included for open()
#include <fcntl.h>       // included for open()
#include <unistd.h>      // included for close();
#include <sys/ioctl.h>   
#include <unistd.h>      // included for mmap
#include <sys/mman.h>    // included for mmap

using namespace std;

namespace CVD {

Exceptions::V4L2Buffer::DeviceOpen::DeviceOpen(string device, const char* err)
{
	what = "V4L2Buffer: failed to open \""+device+ "\": " + err;
}

Exceptions::V4L2Buffer::DeviceSetup::DeviceSetup(string device, string action)
{
	what = "V4L2Buffer: \""+action + "\" ioctl failed on " + device;
}

Exceptions::V4L2Buffer::PutFrame::PutFrame(string device)
{
	what = "V4L2Buffer: Requeuing buffer in put_frame failed (putting back a frame twice?) on " + device;
}

Exceptions::V4L2Buffer::GetFrame::GetFrame(string device)
{
	what = "V4L2Buffer: Dequeueing buffer in get frame failed on " + device;
}


V4L2Buffer::V4L2Buffer(const char *devname, bool fields, V4L2BufferBlockMethod block)
{
	device = devname;
	my_frame_rate=0;
	my_block_method=block;
	i_am_using_fields=fields;


	// Open the device.
	m_nVideoFileDesc=open(devname,O_RDONLY);

	if(m_nVideoFileDesc == 0)
		throw Exceptions::V4L2Buffer::DeviceOpen(devname, strerror(errno));
	 
	// Get device capabilites::
	struct v4l2_capability sv4l2Capability;
	ioctl(m_nVideoFileDesc, VIDIOC_QUERYCAP, &sv4l2Capability);

	cerr << "  V4L2Buffer: Device name:"<< sv4l2Capability.name <<endl;
	cerr << "  V4L2Buffer: (If that was garbled then you've not go the right modules loaded.) "<<endl;
	cerr << "  V4L2Buffer: Streaming flags: "<< (sv4l2Capability.flags&V4L2_FLAG_STREAMING)<<endl;

	// Change a few of the card's settings to our liking:
	struct v4l2_control sv4l2Control;

	// Enable "Low Colour Removal"
	sv4l2Control.id=V4L2_CID_PRIVATE_BASE + 8;   sv4l2Control.value=1;
	if(ioctl(m_nVideoFileDesc, VIDIOC_S_CTRL, &sv4l2Control))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Enable Low Colour Removal");

	// Disable "Chroma AGC"
	sv4l2Control.id=V4L2_CID_PRIVATE_BASE + 7;   sv4l2Control.value=0;
	if(ioctl(m_nVideoFileDesc, VIDIOC_S_CTRL, &sv4l2Control))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Disable Chroma AGC");
		

	// Disable "Luma Notch Filter";
	sv4l2Control.id=V4L2_CID_PRIVATE_BASE + 4;   sv4l2Control.value=0;
	if(ioctl(m_nVideoFileDesc, VIDIOC_S_CTRL, &sv4l2Control))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Disable Luma Notch Filter");


	// Enable "Raw TS";
	//sv4l2Control.id=V4L2_CID_PRIVATE_BASE + 17;   sv4l2Control.value=1;
	//ssert(!ioctl(m_nVideoFileDesc, VIDIOC_S_CTRL, &sv4l2Control));

	// Get / Set capture format.
	struct v4l2_format sv4l2Format;
	sv4l2Format.type=V4L2_BUF_TYPE_CAPTURE;

	if(ioctl(m_nVideoFileDesc, VIDIOC_G_FMT, &sv4l2Format))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Get capture format");

	cerr	<<"  V4L2Buffer: Current capture format: "<<sv4l2Format.fmt.pix.width<<
			"x"<<sv4l2Format.fmt.pix.height<<
			"x"<<sv4l2Format.fmt.pix.depth<<
			"bpp, image size:"<<sv4l2Format.fmt.pix.sizeimage<<endl;

	// ******************************************* CAPTURE FORMAT******
	sv4l2Format.fmt.pix.width=768;                                
	sv4l2Format.fmt.pix.depth=8;                                 
	sv4l2Format.fmt.pix.pixelformat=V4L2_PIX_FMT_GREY;          
	if(i_am_using_fields)                                      
	{
		sv4l2Format.fmt.pix.flags|=V4L2_FMT_FLAG_TOPFIELD;    
		sv4l2Format.fmt.pix.flags|=V4L2_FMT_FLAG_BOTFIELD;   
		sv4l2Format.fmt.pix.flags^=(sv4l2Format.fmt.pix.flags&V4L2_FMT_FLAG_INTERLACED);
		sv4l2Format.fmt.pix.height=576/2;                   
	};                                                     
	if(!i_am_using_fields)
	{
	  sv4l2Format.fmt.pix.height=576;                     
	};

	// ****************************************************************

	if(ioctl(m_nVideoFileDesc, VIDIOC_S_FMT, &sv4l2Format))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Set capture format");

	cerr<<"  V4L2Buffer: New capture format: "<<sv4l2Format.fmt.pix.width<<
	"x"<<sv4l2Format.fmt.pix.height<<
	"x"<<sv4l2Format.fmt.pix.depth<<
	"bpp, image size:"<<sv4l2Format.fmt.pix.sizeimage<<endl;
	my_image_size.x=sv4l2Format.fmt.pix.width;
	my_image_size.y=sv4l2Format.fmt.pix.height;

	// Select video input
	struct v4l2_input sv4l2Input;
	sv4l2Input.index=1;  // This is the composite input
	if(ioctl(m_nVideoFileDesc, VIDIOC_S_INPUT, &sv4l2Input))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Select composite input");

	// Set up the streaming buffer request
	struct v4l2_requestbuffers sv4l2RequestBuffers;
	sv4l2RequestBuffers.count=3;
	sv4l2RequestBuffers.type=V4L2_BUF_TYPE_CAPTURE; //|V4L2_BUF_ATTR_DEVICEMEM;
	cerr<<"  V4L2Buffer: Request buffer of count "<<sv4l2RequestBuffers.count<<" and type "<<sv4l2RequestBuffers.type<<".."<<endl;
	if(ioctl(m_nVideoFileDesc,VIDIOC_REQBUFS,&sv4l2RequestBuffers))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Request capture buffers");
	cerr<<"  V4L2Buffer: Granted buffer of count "<<sv4l2RequestBuffers.count<<" and type "<<sv4l2RequestBuffers.type<<".."<<endl;

	// Set up the streaming buffers, and mmap them
	for(int ii=0;ii<V4L2BUFFERS;ii++) 
	{
		m_sv4l2Buffer[ii].index=ii;
		m_sv4l2Buffer[ii].type=sv4l2RequestBuffers.type;  

		if(ioctl(m_nVideoFileDesc,VIDIOC_QUERYBUF,&m_sv4l2Buffer[ii]))
			throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Query streaming buffer");
			
		m_pvVideoBuffer[ii]=mmap(0,m_sv4l2Buffer[ii].length,PROT_READ,MAP_SHARED,m_nVideoFileDesc, m_sv4l2Buffer[ii].offset);

		if(ioctl(m_nVideoFileDesc,VIDIOC_QBUF,&m_sv4l2Buffer[ii]))
			throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Queue streaming buffer");
			
	}

	// Begin the data transfer
	if(ioctl(m_nVideoFileDesc,VIDIOC_STREAMON, &m_sv4l2Buffer[0].type))
		throw Exceptions::V4L2Buffer::DeviceSetup(devname, "Begin streaming (is device already in use?)");
		
}

V4L2Buffer::~V4L2Buffer()
{
	if(ioctl(m_nVideoFileDesc,VIDIOC_STREAMOFF, &m_sv4l2Buffer[0].type))
		throw Exceptions::V4L2Buffer::DeviceSetup(device, "End streaming");

	for(int ii=0;ii<V4L2BUFFERS;ii++)
		munmap(m_pvVideoBuffer[ii], m_sv4l2Buffer[ii].length);

	close(m_nVideoFileDesc);
}  

void V4L2Buffer::put_frame(V4L2Frame *f)
{
  // Requeue
  struct v4l2_buffer buffer;
  buffer.type=m_sv4l2Buffer[0].type;
  buffer.index=f->my_index;

  if(ioctl(m_nVideoFileDesc,VIDIOC_QBUF,&buffer))
		throw Exceptions::V4L2Buffer::PutFrame(device);

  // Delete the frame
  delete f;
}

void V4L2Buffer::put_frame(VideoFrame<unsigned char> *f)
{
  put_frame((V4L2Frame *)f);
}

bool V4L2Buffer::frame_pending(){
  fd_set fdsetRead;
  fd_set fdsetOther;
  struct timeval tv;
 
  FD_SET(m_nVideoFileDesc,&fdsetRead);
  FD_ZERO(&fdsetOther);
  tv.tv_sec=0;
  tv.tv_usec=0;
  if (select(m_nVideoFileDesc+1,&fdsetRead,&fdsetOther,&fdsetOther,&tv)>0)  
    return true;
  return false;
}

V4L2Frame* V4L2Buffer::get_frame(){

  fd_set fdsetRead;
  fd_set fdsetOther;
  struct v4l2_buffer buffer;
  V4L2Frame *frame;
 
  buffer.type=m_sv4l2Buffer[0].type;
 
  // Block until one is ready and dequeue
  switch (my_block_method) {
  case V4L2BBMselect:
    FD_SET(m_nVideoFileDesc,&fdsetRead);
    FD_ZERO(&fdsetOther);
    select(m_nVideoFileDesc+1,&fdsetRead,&fdsetOther,&fdsetOther,NULL);  

    if(ioctl(m_nVideoFileDesc,VIDIOC_DQBUF,&buffer))
		throw Exceptions::V4L2Buffer::GetFrame(device);

    break;
  case V4L2BBMsleep:
    while(ioctl(m_nVideoFileDesc,VIDIOC_DQBUF,&buffer)) {
      usleep(10);
    }
    break;
  case V4L2BBMchew:
    while(ioctl(m_nVideoFileDesc,VIDIOC_DQBUF,&buffer)) {
    }
    break;
  }

  // Create a new frame with the data
  frame=new V4L2Frame(timer.conv_ntime(buffer.timestamp),my_image_size,buffer.index,(unsigned char *)m_pvVideoBuffer[buffer.index], buffer.flags&V4L2_BUF_FLAG_BOTFIELD);

  return frame;
}

}



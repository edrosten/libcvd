/**************************************************************************
**       Title: grab one gray image using libdc1394
**    $RCSfile: dvbuffer.cc,v $
**   $Revision: 1.1.1.1 $$Name:  $
**       $Date: 2005/01/26 16:17:45 $
**   Copyright: LGPL $Author: er258 $
** Description:
**
**    Get one gray image using libdc1394 and store it as portable gray map
**    (pgm). Based on 'samplegrab' from Chris Urmson 
**
**-------------------------------------------------------------------------
**
**  $Log: dvbuffer.cc,v $
**  Revision 1.1.1.1  2005/01/26 16:17:45  er258
**  Entering in to CVS
**
**  Revision 1.1.1.1  2005/01/25 18:59:22  er258
**  Entering libCVD in to CVS
**
**  Revision 1.3  2001/10/16 09:14:14  ronneber
**  - added more meaningful error message, when no raw1394 handle could be get
**  - does not exit anymore, when RawDCVideo has no trigger
**
**  Revision 1.2  2001/09/14 08:10:41  ronneber
**  - some cosmetic changes
**
**  Revision 1.1  2001/07/24 13:50:59  ronneber
**  - simple test programs to demonstrate the use of libdc1394 (based
**    on 'samplegrab' of Chris Urmson
**
**
**************************************************************************/

#include <stdio.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <stdlib.h>


#if KERNEL_MAJOR==2 
	#if KERNEL_MINOR==6
		#include "cvd_src/Linux/kernel-video1394-2.6.h"
	#elif KERNEL_MINOR==4
		#include "cvd_src/Linux/kernel-video1394-2.4.h"
	#else
		#define UNKNOWN_KERNEL
	#endif
#else 
	#define UNKNOWN_KERNEL
#endif


#ifdef UNKNOWN_KERNEL
	#error I dont know how to compile DC stuff for this kernel version!
#endif

#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

//#include <videodisplay.h>
#include "cvd/image.h"
#include "cvd/Linux/dvbuffer.h"
#include "cvd/Linux/dvframe.h"

using namespace std;
using namespace CVD;

#define MAX_NUM_PORTS 8


/*
  These arrays define how many image quadlets there
  are in a packet given a mode and a frame rate
  This is defined in the 1394 digital RawDCVideo spec 
*/
const int quadlets_per_packet_format_0[42] = 
{
     -1,  -1,  15,  30,  60,  -1,
     -1,  20,  40,  80, 160,  -1,
     -1,  60, 120, 240, 480,  -1,
     -1,  80, 160, 320, 640,  -1,
     -1, 120, 240, 480, 960,  -1,
     -1,  40,  80, 160, 320, 640,
     -1,  80, 160, 320, 640,  -1
};

const int quadlets_per_packet_format_1[48] = 
{
     -1, 125, 250, 500, 1000,   -1,
     -1,  -1, 375, 750,   -1,   -1,
     -1,  -1, 125, 250,  500, 1000,
     96, 192, 384, 768,   -1,   -1,
    144, 288, 576,  -1,   -1,   -1,
     48,  96, 192, 384,  768,   -1,
     -1, 125, 250, 500, 1000,   -1,
     96, 192, 384, 768,   -1,   -1
};

const int quadlets_per_packet_format_2[48] = 
{
    160, 320,  640,   -1, -1, -1,
    240, 480,  960,   -1, -1, -1,
     80, 160,  320,  640, -1, -1,
    250, 500, 1000,   -1, -1, -1,
    375, 750,   -1,   -1, -1, -1,
    125, 250,  500, 1000, -1, -1,
    160, 320,  640,   -1, -1, -1,
    250, 500, 1000,   -1, -1, -1
};
  

/**********************/
/* Internal functions */
/**********************/

static int _dc1394_get_wh_from_format(int format, int mode, int *w, int *h) 
{

    switch(format) 
    {
    case FORMAT_VGA_NONCOMPRESSED:
        switch(mode) 
        {
        case MODE_160x120_YUV444:
            *w = 160;*h=120;
            return DC1394_SUCCESS;
        case MODE_320x240_YUV422:
            *w = 320;*h=240;
            return DC1394_SUCCESS;
        case MODE_640x480_YUV411:
        case MODE_640x480_YUV422:
        case MODE_640x480_RGB:
        case MODE_640x480_MONO:
        case MODE_640x480_MONO16:
            *w =640;*h=480;
            return DC1394_SUCCESS;
        default:
            return DC1394_FAILURE;
        }
    case FORMAT_SVGA_NONCOMPRESSED_1:
        switch(mode) 
        {
        case MODE_800x600_YUV422:
        case MODE_800x600_RGB:
        case MODE_800x600_MONO:
        case MODE_800x600_MONO16:
            *w=800;*h=600;
            return DC1394_SUCCESS;
        case MODE_1024x768_YUV422:
        case MODE_1024x768_RGB:
        case MODE_1024x768_MONO:
        case MODE_1024x768_MONO16:
            *w=1024;*h=768;
            return DC1394_SUCCESS;
        default:
            return DC1394_FAILURE;
        }
    case FORMAT_SVGA_NONCOMPRESSED_2:
        switch(mode) 
        {
        case MODE_1280x960_YUV422:
        case MODE_1280x960_RGB:
        case MODE_1280x960_MONO:
        case MODE_1280x960_MONO16:
            *w=1280;*h=960;
            return DC1394_SUCCESS;
        case MODE_1600x1200_YUV422:
        case MODE_1600x1200_RGB:
        case MODE_1600x1200_MONO:
        case MODE_1600x1200_MONO16:
            *w=1600;*h=1200;
            return DC1394_SUCCESS;
        default:
            return DC1394_FAILURE;
        }
    default:
        return DC1394_FAILURE;
    }

}

/**********************************************************
 _dc1394_quadlets_from_format

 This routine reports the number of quadlets that make up a 
 frame given the format and mode
***********************************************************/
static int _dc1394_quadlets_from_format(int format, int mode) 
{

    switch (format) 
    {
    case FORMAT_VGA_NONCOMPRESSED:

        switch(mode) 
        {
        case MODE_160x120_YUV444:
            return 14400;   //160x120*3/4
        case MODE_320x240_YUV422:
            return 38400;   //320x240/2
        case MODE_640x480_YUV411:
            return 115200;  //640x480x12/32
        case MODE_640x480_YUV422:
            return 153600;  //640x480/2
        case MODE_640x480_RGB:
            return 230400;  //640x480x3/4
        case MODE_640x480_MONO:
            return 76800;   //640x480/4
        case MODE_640x480_MONO16:
            return 153600;  //640x480/2
        default:
            fprintf(stderr,"(%s) Improper mode specified: %d\n", __FILE__, mode);
            break;
        }

        break;
    case FORMAT_SVGA_NONCOMPRESSED_1: 

        switch(mode) 
        {
        case MODE_800x600_YUV422:
            return 240000;  //800x600/2
        case MODE_800x600_RGB:
            return 360000;  //800x600x3/4
        case MODE_800x600_MONO:
            return 120000;  //800x600/4
        case MODE_1024x768_YUV422:
            return 393216;  //1024x768/2
        case MODE_1024x768_RGB:
            return 589824;  //1024x768x3/4
        case MODE_1024x768_MONO:
            return 196608;  //1024x768/4
        case MODE_800x600_MONO16:
            return 240000;  //800x600/2
        case MODE_1024x768_MONO16:
            return 393216;  //1024x768/2
        default:
            fprintf(stderr,"(%s) Improper mode specified: %d\n", __FILE__, mode);
            break;
        }

        break;
    case FORMAT_SVGA_NONCOMPRESSED_2:

        switch (mode) 
        {
        case MODE_1280x960_YUV422:
            return 614400;  //1280x960/2
        case MODE_1280x960_RGB:
            return 921600;  //1280x960x3/4
        case MODE_1280x960_MONO:
            return 307200;  //1280x960/4
        case MODE_1600x1200_YUV422:
            return 960000;  //1600x1200/2
        case MODE_1600x1200_RGB:
            return 1440000; //1600x1200x3/4
        case MODE_1600x1200_MONO:
            return 480000;  //1600x1200/4
        case MODE_1280x960_MONO16:
            return 614400;  //1280x960/2
        case MODE_1600x1200_MONO16:
            return 960000;  //1600x1200/2
        default:
            fprintf(stderr,"(%s) Improper mode specified: %d\n", __FILE__, mode);
            break;
        }

        break;
    case FORMAT_STILL_IMAGE:
        fprintf(stderr,"(%s) Don't know how many quadlets per frame for "
               "FORMAT_STILL_IMAGE mode:%d\n", __FILE__, mode);
        break;
    case FORMAT_SCALABLE_IMAGE_SIZE:
        fprintf(stderr,"(%s) Don't know how many quadlets per frame for "
               "FORMAT_SCALABLE_IMAGE mode:%d\n", __FILE__, mode);
        break;
    default:
        fprintf(stderr,"(%s) Improper format specified: %d\n", __FILE__, format);
        break;
    }

    return -1;
}



/*****************************************************
 dc1394_dma_release_camera

 This releases memory that was mapped by
 dc1394_dma_setup_camera
*****************************************************/
void tom_dc1394_dma_release_camera(raw1394handle_t handle, const unsigned char* ring_buffer, int buffer_size, int dma_fd){
  if (ring_buffer){
    munmap((void*)ring_buffer,buffer_size);
  }
  
  while (close(dma_fd) != 0) {
    cerr << "waiting for dma_fd to close" << endl;
    sleep(1);
  }
}
 
CVD::DC::RawDCVideo::RawDCVideo(int camera_no, int num_dma_buffers, int bright, int exposure, int mode, int frame_rate)
{

  int channel = camera_no;
  int format = FORMAT_VGA_NONCOMPRESSED;
  //int mode = MODE_640x480_MONO;
  int speed = SPEED_400;
  const char* dma_device_file = "/dev/video1394/0";


  int camera_quadlets_per_frame; // construction variable
  int numNodes; // local variable
  int numCameras; // local variable
  int *myPort = new int;
  int port=0;

  my_frame_sequence.resize(num_dma_buffers);
  for(int i=0; i<num_dma_buffers-1; i++){
    my_frame_sequence[i]=i+1;
  }
  my_frame_sequence[num_dma_buffers-1]=-1;

  my_next_frame=0;
  my_last_in_sequence=num_dma_buffers-1;

  /*-----------------------------------------------------------------------
   *  Open ohci and asign handle to it
   *-----------------------------------------------------------------------*/
#ifdef LIBRAW1394_OLD
  if (!(my_handle= raw1394_get_handle()))
#else
  if (!(my_handle= raw1394_new_handle()))
#endif
  {
    cerr << "Couldn't get raw1394 handle!" << endl;
    my_handle=0;
  }

  if (raw1394_set_port(my_handle, port) < 0) {
    if (my_handle != NULL) {
      raw1394_destroy_handle(my_handle);
      cerr << "Couldn't raw1394_set_port!" << endl;
      my_handle=0;
    }
  }

  *myPort = port;
  raw1394_set_userdata( my_handle, myPort );

  if (my_handle==NULL)
  {
    fprintf(stderr, "Unable to aquire a raw1394 handle\n\n"
             "Please check \n"
	     "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
	     "  - if you have read/write access to /dev/raw1394\n\n");
    exit(1);
  }
  
  /*-----------------------------------------------------------------------
   *  get the RawDCVideo nodes and describe them as we find them
   *-----------------------------------------------------------------------*/
  numNodes = raw1394_get_nodecount(my_handle);
  my_camera_nodes = dc1394_get_camera_nodes(my_handle,&numCameras,1); // keep this one too
  fflush(stdout);
  if (numCameras<=camera_no)
  {
    fprintf( stderr, "not enough cameras found - only found %d :(\n", numCameras);
    raw1394_destroy_handle(my_handle);
    exit(1);
  }
  fprintf(stderr,"found %d cameras\n",numCameras);

  /*-----------------------------------------------------------------------
   *  to prevent the iso-transfer bug from raw1394 system, check if
   *  RawDCVideo is highest node. For details see 
   *  http://linux1394.sourceforge.net/faq.html#DCbusmgmt
   *  and
   *  http://sourceforge.net/tracker/index.php?func=detail&aid=435107&group_id=8157&atid=108157
   *-----------------------------------------------------------------------*/
  if( my_camera_nodes[camera_no] == numNodes-1)
  {
    fprintf( stderr, "\n"
             "Sorry, your RawDCVideo is the highest numbered node\n"
             "of the bus, and has therefore become the root node.\n"
             "The root node is responsible for maintaining \n"
             "the timing of isochronous transactions on the IEEE \n"
             "1394 bus.  However, if the root node is not cycle master \n"
             "capable (it doesn't have to be), then isochronous \n"
             "transactions will not work.  The host controller card is \n"
             "cycle master capable, however, most cameras are not.\n"
             "\n"
             "The quick solution is to add the parameter \n"
             "attempt_root=1 when loading the OHCI driver as a \n"
             "module.  So please do (as root):\n"
             "\n"
             "   rmmod ohci1394\n"
             "   insmod ohci1394 attempt_root=1\n"
             "\n"
			 "A quicker solution is to unplug the camera and plug it back in again.\n"
			 "\n"
             "for more information see the FAQ at \n"
             "http://linux1394.sourceforge.net/faq.html#DCbusmgmt\n"
             "\n");
    exit( 1);
  }
  
  /*-----------------------------------------------------------------------
   *  setup capture
   *-----------------------------------------------------------------------*/

  my_node = my_camera_nodes[camera_no];

 
  if( format == FORMAT_SCALABLE_IMAGE_SIZE) {
    fprintf( stderr, "Wrong function: Use dc1394_dma_setup_format7_capture()\n");
    exit(1);
  }

  if (dc1394_set_iso_channel_and_speed(my_handle,my_node,channel,speed) != DC1394_SUCCESS) {
    cerr << "dc1394_set_iso_channel_and_speed failed" << endl;
    raw1394_destroy_handle(my_handle);
    exit(1);
  }

  if (dc1394_set_video_format(my_handle,my_node,format) != DC1394_SUCCESS) {
    cerr << "dc1394_set_video_format failed" << endl;
    raw1394_destroy_handle(my_handle);
    exit(1);
  }

  if (dc1394_set_video_mode(my_handle, my_node,mode) != DC1394_SUCCESS) {
    cerr << "dc1394_set_video_mode failed" << endl;
    raw1394_destroy_handle(my_handle);
    exit(1);
  }

  if (dc1394_set_video_framerate(my_handle,my_node,frame_rate) != DC1394_SUCCESS) {
    cerr << "dc1394_set_video_framerate failed" << endl;
    raw1394_destroy_handle(my_handle);
    exit(1);
  }

  my_channel= channel;
  
  if((camera_quadlets_per_frame= _dc1394_quadlets_from_format(format, mode)) < 0){
    cerr << "_dc1394_quadlets_from_format failed" << endl;
    raw1394_destroy_handle(my_handle);
    exit(1);
  }
  
  // if (_dc1394_get_wh_from_format(format,mode,&(camera_frame_width), &(camera_frame_height)) == DC1394_FAILURE) {
  if (_dc1394_get_wh_from_format(format,mode,&(my_size.x), &(my_size.y)) == DC1394_FAILURE) {
    cerr << "_dc1394_get_wh_from_format failed" << endl;
    raw1394_destroy_handle(my_handle);
    exit(1);
  }

  // dma setup
  struct video1394_mmap vmmap;
  struct video1394_wait vwait;
  
  if ( (my_fd = open(dma_device_file,O_RDONLY)) < 0 ) {
    cerr << "unable to open video1394 device " << dma_device_file << endl;
    tom_dc1394_dma_release_camera(my_handle,my_ring_buffer, my_frame_size*my_num_buffers, my_fd);
    raw1394_destroy_handle(my_handle);
    exit(1);
  }

  vmmap.sync_tag= 1;
  vmmap.nb_buffers= num_dma_buffers;
  vmmap.flags= VIDEO1394_SYNC_FRAMES;
  vmmap.buf_size= camera_quadlets_per_frame * 4; //number of bytes needed
  vmmap.channel= channel;

  /* tell the video1394 system that we want to listen to the given channel */
  if (ioctl(my_fd, VIDEO1394_LISTEN_CHANNEL, &vmmap) < 0) {
    cerr << "VIDEO1394_LISTEN_CHANNEL ioctl failed!" << endl;
    tom_dc1394_dma_release_camera(my_handle,my_ring_buffer, my_frame_size*my_num_buffers, my_fd);
    raw1394_destroy_handle(my_handle);
    exit(1);
  }
  
  my_frame_size= vmmap.buf_size;
  my_num_buffers= vmmap.nb_buffers;
  vwait.channel= channel;
  
  /* QUEUE the buffers */
  for (unsigned int i= 0; i < vmmap.nb_buffers; i++) {
    vwait.buffer= i;
    
    if (ioctl(my_fd,VIDEO1394_LISTEN_QUEUE_BUFFER,&vwait) < 0) {
      cerr << "VIDEO1394_LISTEN_QUEUE_BUFFER ioctl failed!" << endl;
      ioctl(my_fd, VIDEO1394_UNLISTEN_CHANNEL, &(vwait.channel));
      tom_dc1394_dma_release_camera(my_handle,my_ring_buffer, my_frame_size*my_num_buffers, my_fd);
      raw1394_destroy_handle(my_handle);
      exit(1);
    }  
  }
    
  my_ring_buffer= (unsigned char*) mmap(0, vmmap.nb_buffers * vmmap.buf_size,
					PROT_READ,MAP_SHARED, my_fd, 0);

  // make sure the ring buffer was allocated
  if (my_ring_buffer == (unsigned char*)(-1)) {
    cerr << "mmap failed!" << endl;
    ioctl(my_fd, VIDEO1394_UNLISTEN_CHANNEL, &vmmap.channel);
    tom_dc1394_dma_release_camera(my_handle,my_ring_buffer, my_frame_size*my_num_buffers, my_fd);
    raw1394_destroy_handle(my_handle);
    exit(1);
  }

  // camera_dma_buffer_size= vmmap.buf_size * vmmap.nb_buffers;

  // set trigger mode
  if( dc1394_set_trigger_mode(my_handle, my_node, TRIGGER_MODE_0)
      != DC1394_SUCCESS)
  {
    fprintf( stderr, "unable to set RawDCVideo trigger mode\n");
  }
  
  // auto brightness and exposure
  if(bright==-1){
    dc1394_auto_on_off(my_handle,my_node,FEATURE_BRIGHTNESS,1); 
  } else {
    dc1394_auto_on_off(my_handle,my_node,FEATURE_BRIGHTNESS,0); 
    //dc1394_set_brightness(bright);
  }

  if(exposure==-1){
    dc1394_auto_on_off(my_handle,my_node,FEATURE_EXPOSURE,1);
  } else {
    dc1394_auto_on_off(my_handle,my_node,FEATURE_EXPOSURE,0);
    //dc1394_set_exposure(exposure);
  }

  //dc1394_auto_on_off(my_handle,my_node,FEATURE_EXPOSURE,0);
  //dc1394_set_exposure(my_handle,my_node,200);

  /*-----------------------------------------------------------------------
   *  have the RawDCVideo start sending us data
   *-----------------------------------------------------------------------*/

  // this call is OK
  if (dc1394_start_iso_transmission(my_handle,my_node)!=DC1394_SUCCESS) {
    fprintf( stderr, "unable to start RawDCVideo iso transmission\n");
    tom_dc1394_dma_release_camera(my_handle,my_ring_buffer, my_frame_size*my_num_buffers, my_fd);
    raw1394_destroy_handle(my_handle);
    exit(1);
  } else {
    fprintf( stderr, "started iso transmisssion\n");
  }
}

VideoFrame<byte>* CVD::DC::RawDCVideo::get_frame(){
  struct video1394_wait vwait;

  vwait.channel = my_channel;

  // check that there actually is a free slot!
  if(my_next_frame==-1){
    return 0;
  }


  // get the first frame
  vwait.buffer = my_next_frame;
  
  if (int retval=ioctl(my_fd, VIDEO1394_LISTEN_WAIT_BUFFER, &vwait) != 0) {
    cerr << " VIDEO1394_LISTEN_WAIT_BUFFER ioctl failed with value" << retval << endl;
  }

  // find the most recent frame
  for(;;){
    int next_frame = my_frame_sequence[my_next_frame];

    // we'd better not run off the end of the sequence
    if(next_frame==-1){
      break;
    }

    vwait.buffer = next_frame;
    if (ioctl(my_fd, VIDEO1394_LISTEN_POLL_BUFFER, &vwait) != 0) {
      break;
    }
    

    vwait.buffer = my_next_frame;
    if (ioctl(my_fd, VIDEO1394_LISTEN_QUEUE_BUFFER, &vwait) < 0) {
      cerr << " VIDEO1394_LISTEN_QUEUE_BUFFER failed" << endl;
    }
    my_frame_sequence[my_last_in_sequence]=my_next_frame;
    my_frame_sequence[my_next_frame]=-1;
    my_last_in_sequence = my_next_frame;
    my_next_frame = next_frame;    
  }
  
  DVFrame* frame = new DVFrame(my_size, vwait.filltime, my_next_frame,
			       my_ring_buffer + my_next_frame * my_frame_size );

  my_next_frame = my_frame_sequence[my_next_frame];

  return frame;
}
  
void CVD::DC::RawDCVideo::put_frame(VideoFrame<byte>* f){
  DVFrame* frame = (DVFrame*) f;

  struct video1394_wait vwait;


  // requeue the last buffer
  vwait.channel = my_channel;
  vwait.buffer = frame->my_buffer;
  if (ioctl(my_fd, VIDEO1394_LISTEN_QUEUE_BUFFER, &vwait) < 0) {
    cerr << " VIDEO1394_LISTEN_QUEUE_BUFFER failed to queue buffer " << vwait.buffer << endl;
  }
  my_frame_sequence[my_last_in_sequence] = frame->my_buffer;
  my_frame_sequence[frame->my_buffer]=-1;
  my_last_in_sequence = frame->my_buffer;

  if(my_next_frame==-1){
    my_next_frame = my_last_in_sequence;
  }

  delete frame;
}



CVD::DC::RawDCVideo::~RawDCVideo(){
  /*-----------------------------------------------------------------------
   *  Stop data transmission
   *-----------------------------------------------------------------------*/
  if (dc1394_stop_iso_transmission(my_handle,my_node)!=DC1394_SUCCESS) 
  {
    fprintf(stderr,"couldn't stop the RawDCVideo?\n");
  } else {
    fprintf(stderr, "stopped the RawDCVideo\n");
  }

  /*-----------------------------------------------------------------------
   *  Close RawDCVideo
   *-----------------------------------------------------------------------*/
  tom_dc1394_dma_release_camera(my_handle,my_ring_buffer, my_frame_size*my_num_buffers, my_fd);
  dc1394_camera_off(my_handle, my_node);    // GKMOD: Turn camera off
  raw1394_destroy_handle(my_handle);
}

ImageRef CVD::DC::RawDCVideo::size(){return my_size;}
bool CVD::DC::RawDCVideo::frame_pending(){return true;}
//void CVD::DC::RawDCVideo::seek_to(unsigned long long int t){}


void CVD::DC::RawDCVideo::set_shutter(unsigned int s)
{
	if(dc1394_set_shutter(my_handle, my_node, s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't set shutter.\n");
}

unsigned int CVD::DC::RawDCVideo::get_shutter()
{
	unsigned int s;
	if(dc1394_get_shutter(my_handle, my_node, &s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't get shutter.\n");
	return s;
}

void CVD::DC::RawDCVideo::set_iris(unsigned int s)
{
	if(dc1394_set_iris(my_handle, my_node, s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't set iris.\n");
}

unsigned int CVD::DC::RawDCVideo::get_iris()
{
	unsigned int s;
	if(dc1394_get_iris(my_handle, my_node, &s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't get iris.\n");
	return s;
}

void CVD::DC::RawDCVideo::set_gain(unsigned int s)
{
	if(dc1394_set_gain(my_handle, my_node, s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't set gain.\n");
}

unsigned int CVD::DC::RawDCVideo::get_gain()
{
	unsigned int s;
	if(dc1394_get_gain(my_handle, my_node, &s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't get gain.\n");
	return s;
}


void CVD::DC::RawDCVideo::set_exposure(unsigned int s)
{
	if(dc1394_set_exposure(my_handle, my_node, s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't set exposure.\n");
}

unsigned int CVD::DC::RawDCVideo::get_exposure()
{
	unsigned int s;
	if(dc1394_get_exposure(my_handle, my_node, &s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't get exposure.\n");
	return s;
}


void CVD::DC::RawDCVideo::set_brightness(unsigned int s)
{
	if(dc1394_set_brightness(my_handle, my_node, s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't set brightness.\n");
}

unsigned int CVD::DC::RawDCVideo::get_brightness()
{
	unsigned int s;
	if(dc1394_get_brightness(my_handle, my_node, &s) != DC1394_SUCCESS)
		fprintf(stderr, "couldn't get brightness.\n");
	return s;
}

raw1394handle_t& CVD::DC::RawDCVideo::handle()
{
	return my_handle;
}

nodeid_t& CVD::DC::RawDCVideo::node()
{
	return my_node;
}

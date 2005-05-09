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
#include <iostream>

#include <dmedia/dmedia.h>
#include <dmedia/vl.h>
#include <dmedia/dm_buffer.h>
#include <dmedia/dm_params.h>

#include <cvd/IRIX/sgi-video.h>


using namespace std;

namespace CVD
{

namespace SGI
{
	int debug=0;
	int debug_sgi_events=0;
	const unsigned long long	time_div = 1000000000;


	unsigned long long 	current_time()
	{
		unsigned long long t;
		dmGetUST(&t);

		return t;
	}


void testv(bool  t, char* s)
{
	if(!t)
	{
		if(debug)cerr << "Success for: " << s << endl;
		return;
	}

	cerr << "Error: ";

	vlPerror(s);
	exit(1);
}
void test(bool  t, char* s)
{
	if(!t)
	{
		if(debug)cerr << "Success for: " << s << endl;
		return;
	}
	
	cerr << "Error. Faulure for:" << s << endl;
	exit(1);
}


//#define RB_SIZE 10


typedef long long copy_t;

RawSGIVideo::RawSGIVideo(bool use_frame)
{
	VLControlValue	v;			//A large union of different types.
	DMparams*		dm_params;
	DMstatus		ds;
	int				i;


	svr=vlOpenVideo("");
	test(svr==NULL, "open default video source.");
	

	//Attempt to set up the screen as a video drain... 
	//Since we're using a memory drain, we need to set up buffers. See later...
	drain=vlGetNode(svr, VL_DRN, VL_MEM, VL_ANY);
	testv(drain==-1, "get memory drain node");

	//Get the video dource node at the server:
	source = vlGetNode(svr, VL_SRC, VL_VIDEO, VL_ANY);
	testv(source == -1., "get [default] video source node");

	/* Set up a path from the source to (the video) to the destination
	(the memory). The path only has 2 nodes, the start and the end.*/
	vid_in = vlCreatePath(svr, VL_ANY, source, drain);
	testv(vid_in == -1, "create path from video to memory");

	i = vlSetupPaths(svr, (VLPathList)&vid_in, 1, VL_SHARE, VL_SHARE);
	testv(i == -1, "Setting up path");

	/*Parameters can be read or set by manipulating "Controls" */

	//Set various parameters...
	v.intVal = VL_PACKING_RGB_8;
	i=vlSetControl(svr, vid_in, VL_ANY, VL_PACKING, &v);
	testv(i==-1, "setting format to Y");


	if(!use_frame)
	{
		v.intVal = VL_CAPTURE_FIELDS;
		i=vlSetControl(svr, vid_in, drain, VL_CAP_TYPE, &v);
		testv(i==-1, "setting field capture");

		frame_number_div=1;
	}
	else
		frame_number_div=2;
	
	
	//Get the screen size:
	vlGetControl(svr, vid_in, drain, VL_SIZE, &v);

	cerr << "Screen size is: (" << v.xyVal.x << ", " << v.xyVal.y << ")\n";
	
	my_x_size = v.xyVal.x;
	my_y_size = v.xyVal.y;

	my_frame_size = vlGetTransferSize(svr, vid_in);
	
	cerr << "Data size is " << my_frame_size << " bytes. Is this right?\n";

	i = vlGetControl(svr, vid_in, drain, VL_RATE, &v);
	testv(i == -1, "querying frame rate");

	my_frame_rate = v.fractVal.numerator / (float) v.fractVal.denominator;

	cerr << "Rate is " <<  v.fractVal.numerator << "/" <<v.fractVal.denominator
	     <<" = "<< my_frame_rate << endl;
	
	
	/* Now set up the digital media. This is done by getting a list of
	parameters set by it and pasing them to more setup functions.*/

	ds = dmParamsCreate(&dm_params);
	test(ds == DM_FAILURE, "creating the dmedia parameter list");
	
	//Set up the pool of buffers.
	ds=dmBufferSetPoolDefaults(dm_params, 2, my_frame_size, DM_TRUE, DM_TRUE);
	test(ds==DM_FAILURE, "setting pool sefaults");

	//Find out which pool parameters are needed for the video
	i = vlDMPoolGetParams(svr, vid_in, drain, dm_params);
	test(i == DM_FAILURE, "querying path for pool parameters");

	//Now create the pool of buffers
	ds = dmBufferCreatePool(dm_params, &buff);
	test(ds == DM_FAILURE, "creating pool of buffers");

	//We have the buffers and the video memory sink. Now connect them:
	i = vlDMPoolRegister(svr, vid_in, drain, buff);
	test(i == DM_FAILURE, "registering buffer pool with the memory drain");

	
	//The transfer is event driven:
	//When it says frame, it really means field.
	i = vlSelectEvents(svr, vid_in, VLTransferCompleteMask 		|
									//VLOddVerticalRetraceMask	|
									//VLEvenVerticalRetraceMask	|
									VLFrameVerticalRetraceMask	|
									VLTransferFailedMask    	);
	testv(i == -1, "vl event selection");



	/* Now the system is set up, so do the rest of the initialisation */


	locked_buffer = -1;
		
	frame_no = 0;
	my_frame_number = 0;
	started=0;	

}
	

void RawSGIVideo::start()
{
	int i;
	VLTransferDescriptor xferdesc;

	if(started) return;

	started=1;
	
	//This is the type of transfer we want:
	xferdesc.mode = VL_TRANSFER_MODE_CONTINUOUS;	//A single (discrete) xfer
	xferdesc.count = 0;								//One frame...
	xferdesc.delay = 0;								//Start on the next frame
	xferdesc.trigger=  VLTriggerImmediate;			//Now, Damn it, NOW!!!!!

	i = vlBeginTransfer(svr, vid_in, 1, &xferdesc);
	testv(i==-1, "starting transfer");

}

void RawSGIVideo::stop()
{
	int i;

	if(!started) return;
	
	started = 0;
	
	i = vlEndTransfer(svr, vid_in);
	testv(i==-1, "ending transfer");
}


RawSGIVideo::~RawSGIVideo()
{
	if(locked_buffer != -1)
		dmBufferFree(buf[locked_buffer]);

	vlEndTransfer(svr, vid_in);
	vlDMPoolDeregister(svr, vid_in, source, buff);
	vlDestroyPath(svr, vid_in);
	vlCloseVideo(svr);
	delete	buf;
}


int RawSGIVideo::x_size() const
{
	return my_x_size;
}

int RawSGIVideo::y_size() const
{
	return my_y_size;
}

int RawSGIVideo::frame_size() const
{
	return my_frame_size;
}

int RawSGIVideo::frame_number() const
{
	return my_frame_number / frame_number_div;
}

float RawSGIVideo::frame_rate() const
{
	return my_frame_rate;
}

unsigned char* RawSGIVideo::current_frame() const
{
	return my_current_frame;
}


unsigned long long  RawSGIVideo::frame_time() const
{
	return my_timestamp;
}



unsigned char* RawSGIVideo::next_frame()
{
	unsigned char* ret_buff = NULL;
	VLEvent	event;
	USTMSCpair	time_data;

	while(vlEventRecv(svr, vid_in, &event) == DM_SUCCESS)
	{
		
		if(debug_sgi_events)
			cerr << "--- Event: " << vlEventToName(event.reason) << "\n";
		switch(event.reason)
		{
			case VLTransferComplete:
				
				//Save the frame number for this frame
				my_frame_number = frame_no;

				
				//put the frame in to the free buffer
				vlEventToDMBuffer(&event, &buf[!locked_buffer]);
				ret_buff = (unsigned char*)dmBufferMapData(buf[!locked_buffer]);

				my_current_frame = ret_buff;

				//Free the old locked buffer
				//This ensures that there is always a buffer for a transer to
				//go in to.
				if(locked_buffer != -1)
					dmBufferFree(buf[locked_buffer]);

				if(!locked_buffer)
					locked_buffer = 1;
				else
					locked_buffer = 0;
					


				//Get the timestamp

				vlGetUSTMSCPair(svr, vid_in, source, VL_ANY, drain, &time_data);

				my_timestamp = time_data.ust;

				
				break;

			case VLFrameVerticalRetrace:
				frame_no++;
				break;

			case VLTransferFailed:
				cerr << "Video transfer failed\n";
				exit(1);
				break;
		}

	}

	return ret_buff;
}

}

}

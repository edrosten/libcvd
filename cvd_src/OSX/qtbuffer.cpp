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
/*
 *  qtbuffer.cpp
 *  libcvd
 *
 *  FIXME:
 *  - check how to set video input properties such as brightness, saturation, etc
 *  - support other video formats (potentially from other hardware) as well
 */

// build for carbon
#define TARGET_API_MAC_CARBON 1

#if __APPLE_CC__
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#else
#include <ConditionalMacros.h>
#include <QuickTimeComponents.h>
#endif

#undef check

#include <iostream>

using namespace std;

#include <cvd/image_ref.h>
#include <cvd/OSX/qtbuffer.h>

using namespace CVD;
using namespace QT;

Exceptions::QTBUFFER::DeviceOpen::DeviceOpen(string msg)
{
    what = "QTBuffer: failed to open video sequence grabber: "+msg;
}

class CVD::QT::RawQTPimpl {
public:
	int number;
	ImageRef mySize;

	Rect 				bounds;		// bounds rect
    SeqGrabComponent 	seqGrab;	// sequence grabber
	SGChannel			chanVideo;	// video channel
	TimeValue 		 	lastTime;   // time stamp of last frame, FIXME figure out how that is a real timestamp
	unsigned char *		lastFrame;  // pointer to the frame data of the last frame received
	long				length;     // length of the frame data
	bool				newFrame;   // do we have a new frame since the last get_frame call?
	
	static bool isInitialized;  // flag to init Carbon and QuickTime

	static pascal OSErr GrabDataProc(SGChannel c, Ptr p, long len, long *offset, long chRefCon, TimeValue time, short writeType, long refCon);
};

bool RawQTPimpl::isInitialized = false;

pascal OSErr RawQTPimpl::GrabDataProc(SGChannel /* c */, Ptr p, long len, long * /* offset */, long /* chRefCon */, TimeValue time, short /* writeType */, long refCon){
	ComponentResult err = noErr;
	RawQTPimpl * self = (RawQTPimpl *)refCon;
	self->lastFrame = (unsigned char *) p;
	self->length = len;
	self->lastTime = time;
	self->newFrame = true;
	return err;
}

RawQT::RawQT(const ImageRef & size, unsigned int /* mode */, unsigned int num, bool showSettingsDialog, bool verbose) : pimpl(NULL) 
{
	if( !RawQTPimpl::isInitialized ){
		EnterMovies();
		RawQTPimpl::isInitialized = true;
	}

	pimpl = new RawQTPimpl;
	pimpl->bounds.left = 0;
	pimpl->bounds.top = 0;
	pimpl->bounds.right = size.x;
	pimpl->bounds.bottom = size.y;
	pimpl->lastFrame = NULL;
	pimpl->newFrame = false;
	
	pimpl->seqGrab = OpenDefaultComponent(SeqGrabComponentType, 0);
	if( pimpl->seqGrab == NULL ){
		throw Exceptions::QTBUFFER::DeviceOpen("Error opening SequenceGrabber");
	}
	
	OSErr err = noErr;
	
	err = SGInitialize(pimpl->seqGrab);
	if(err != noErr){
		throw Exceptions::QTBUFFER::DeviceOpen("SGInitialize returned error");
	}
	err = SGSetGWorld(pimpl->seqGrab,  GetWindowPort(NULL), NULL);
	if(err != noErr){
		throw Exceptions::QTBUFFER::DeviceOpen("SGSetGWorld returned error");
	}
	// specify the destination data reference for a record operation
	// tell it we're not making a movie
	// if the flag seqGrabDontMakeMovie is used, the sequence grabber still calls
	// your data function, but does not write any data to the movie file
	// writeType will always be set to seqGrabWriteAppend
	err = SGSetDataRef(pimpl->seqGrab, 0, 0, seqGrabDontMakeMovie);
	if(err != noErr){
		throw Exceptions::QTBUFFER::DeviceOpen("SGSetDataRef returned error");
	}
	
	err = SGNewChannel(pimpl->seqGrab, VideoMediaType, &pimpl->chanVideo);
	if(err != noErr){
		throw Exceptions::QTBUFFER::DeviceOpen("SGNewChannel returned error");
	}
	
	if(showSettingsDialog)
	  {
	    err = SGSettingsDialog(pimpl->seqGrab, pimpl->chanVideo, 0, NULL, 0L, NULL, 0); 
	    if(err != noErr)
	      throw Exceptions::QTBUFFER::DeviceOpen("SGSettingsDialog returned an error");
	  }

	try {
		SGDeviceList devices;
		err = SGGetChannelDeviceList(pimpl->chanVideo, 0, &devices);
		if(err != noErr)
			throw Exceptions::QTBUFFER::DeviceOpen("SGGetChannelDeviceList returned an error");
        
		if(verbose)
			cout << "QTBuffer available devices:\n";
		vector<int> allowedDevices;
		for(int i = 0; i < (*devices)->count; ++i){
			if((*devices)->entry[i].flags == 0){
				allowedDevices.push_back(i);
				if(verbose)
					cout << "\t" << (*devices)->entry[i].name << endl;
			}
		}
		if(num >= allowedDevices.size())
			throw Exceptions::QTBUFFER::DeviceOpen("Given device number is not available");
    
		err = SGSetChannelDevice(pimpl->chanVideo, (*devices)->entry[allowedDevices[num]].name);
		if(err != noErr)
			throw Exceptions::QTBUFFER::DeviceOpen("SGSetChannelDevice returned an error");
		SGDisposeDeviceList(pimpl->chanVideo, devices);

		err = SGSetChannelBounds(pimpl->chanVideo, &pimpl->bounds);
		if (err != noErr)
			throw Exceptions::QTBUFFER::DeviceOpen("SGSetChannelBounds returned error");

		// set usage for new video channel to avoid playthrough
		// note we don't set seqGrabPlayDuringRecord
		err = SGSetChannelUsage(pimpl->chanVideo, seqGrabRecord);
		if( err != noErr)
			throw CVD::Exceptions::QTBUFFER::DeviceOpen("SGSetChannelUsage returned error");

		err = SGSetDataProc(pimpl->seqGrab, NewSGDataUPP(RawQTPimpl::GrabDataProc), (long)pimpl);
		if(err != noErr)
			throw Exceptions::QTBUFFER::DeviceOpen("SGSetDataProc returned error");
		
		// lights...camera...
		err = SGPrepare(pimpl->seqGrab, false, true);	
		// ...action
		err = SGStartRecord(pimpl->seqGrab);
		
	        // What format are the images?
		ImageDescriptionHandle imageDesc = (ImageDescriptionHandle)NewHandle(0);
		err = SGGetChannelSampleDescription(pimpl->chanVideo, (Handle)imageDesc);
		if(err != noErr)
		  throw Exceptions::QTBUFFER::DeviceOpen("SGGetChannelSampleDescription returned error");

		// Convert pascal string to stl string..
		for(char i=1; i<=(**imageDesc).name[0]; i++)
			frame_format_string = frame_format_string + (char) (**imageDesc).name[i];
		if(verbose)
			cout << "QTBuffer video format is " << frame_format_string << "\t" << (**imageDesc).width << "," << (**imageDesc).height << "," << (**imageDesc).depth << endl;
	}
	catch(Exceptions::QTBUFFER::DeviceOpen){
		// clean up on failure
		SGDisposeChannel(pimpl->seqGrab, pimpl->chanVideo);
		pimpl->chanVideo = NULL;
		throw;
	}
}

string RawQT::get_frame_format_string()
{
	return frame_format_string;
}

RawQT::~RawQT(){
	if(pimpl){
		if (pimpl->seqGrab)
    		CloseComponent(pimpl->seqGrab);
		delete pimpl;
		pimpl = NULL;
	}
}

ImageRef RawQT::get_size() const {
	return ImageRef(pimpl->bounds.right, pimpl->bounds.bottom);
}

unsigned char* RawQT::get_frame(){
	while(pimpl->newFrame == false){
		OSErr err = SGIdle(pimpl->seqGrab);
		if (err) {
			SGStop(pimpl->seqGrab);
			SGStartRecord(pimpl->seqGrab);
		}
	}
	pimpl->newFrame = false;
	return pimpl->lastFrame;
}

void RawQT::put_frame( unsigned char * ){
	// do nothing here because we cannot protect the frames anyway
}

double RawQT::frame_rate(){
	// FIXME actually determine framerate from VideoDigitizer or something
	return 30;
}

bool RawQT::frame_pending(){
	OSErr err = SGIdle(pimpl->seqGrab);
	if (err) {
		SGStop(pimpl->seqGrab);
		SGStartRecord(pimpl->seqGrab);
	}
	return pimpl->newFrame;
}

// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include "video_capture.h"
#include <cvd/byte.h>
//#include <png.h>
#include <cvd/rgb.h>
#include <cvd/image.h>
#include <cvd/thread.h>
#include <cvd/localvideoframe.h>
#include <cvd/image_ref.h>


//#include <Hello.h>

using namespace std;
//using namespace Demo;

void console_callback(const vcapture_frame_format* format,
		const vcapture_frame* frame,
		unsigned long long int timestamp,
		void* userdata) {
	printf("count=%d\n",frame->count);
	vcapture_frame_format gFormat = *format;
	//gBuffer.resize(format->height * format->bytes_per_row);
	//gBuffer = new CVD::byte[format->height * format->bytes_per_row];
	//gBuffer = new CVD::Rgb<CVD::byte>[format->height*format->width];
	CVD::ImageRef image_ref(format->width, format->height);
	//CVD::BasicImage< CVD::byte > image(gBuffer, image_ref);
	//CVD::Image<CVD::Rgb<CVD::byte> > rgb_im;
	//CVD::Image<CVD::Rgb<CVD::byte> > rgb_im(gBuffer, image_ref);
	CVD::Image<CVD::Rgb<CVD::byte> > image(image_ref);
	memcpy(image.data(), frame->planes[0], format->height * format->bytes_per_row);

	CVD::LocalVideoFrame<CVD::Rgb<CVD::byte> > cvd_frame((double) timestamp, image);

	std::cerr << "timestamp = " << cvd_frame.timestamp() << " " << timestamp << std::endl;

	//CVD::BasicImage<CVD::Rgb<CVD::byte> > im(gBuffer, image_ref);
	//CVD::VideoFrame<CVD::Rgb<CVD::byte> > frame(0.0, gBuffer);
	CVD::ImageRef iref(320, 240);
	//CVD::byte b = image[iref];
	//  std::cerr << (unsigned int) b << std::endl;
	//std::cerr << "image " << image.pos[iref] << std::endl;
	//CVD::BasicImage<unsigned char> i;
	//CVD::BasicImage<CVD::Rgb<CVD::byte> > image;//(gBuffer, image_ref);
	CVD::Rgb<CVD::byte> rgb;
	rgb = image[320][240];
	std::cout << "pixel is (" << ((unsigned int) rgb.red) << ", " << ((unsigned int) rgb.green) << ", " << ((unsigned int) rgb.blue) << ")" << std::endl;

	//  memcpy(&(gBuffer[0]), frame->planes[0], gBuffer.size());
	//memcpy(&(gBuffer[0]), frame->planes[0], format->height * format->bytes_per_row);
	//  if (gBuffer.size() && gFormat.type == VC_TYPE_RGB) {
	if (gFormat.type == VC_TYPE_RGB) {
		printf("got rgb\n");
		//std::cout << "pixel is (" << ((unsigned int) r) << ", " << ((unsigned int) g) << ", " << ((unsigned int) b) << ")" << std::endl;
	}
}


class QTKitIceClient : public Ice::Application
{
public:

    QTKitIceClient();

    virtual int run(int, char*[]);

private:
};

int
main(int argc, char* argv[])
{
    QTKitIceClient app;
    return app.main(argc, argv, "config.client");
}

QTKitIceClient::QTKitIceClient() :
    //
    // Since this is an interactive demo we don't want any signal
    // handling.
    //
    Ice::Application(Ice::NoSignalHandling)
{
}

int
QTKitIceClient::run(int argc, char* argv[])
{
	std::cout << "QTKitIceClient" << std::endl;
	std::cerr << "starting run" << std::endl;
	vcapture v = vcapture_alloc();
	std::cerr << "QTKitFrameGrabber constructor" << std::endl;
	vcapture_frame_format format;
	memset(&format, 0, sizeof(format));
	format.type = VC_TYPE_RGB;
	vcapture_set_requested_format(v, &format);
	//vcapture_set_callback(v, callback, this);
	vcapture_set_callback(v, console_callback, this);
	int n = vcapture_device_count(v);
	std::cerr << "n=" << n << std::endl;
	static int i=0;
	for (int i=0; i<n; ++i) {
		printf("device %d is '%s'\n", i, vcapture_device_name(v, i));
	}
	std::cerr << "vcapture_set_device" << std::endl;
	int status = vcapture_set_device(v, 0);
	std::cerr << "status = " << status << std::endl;
	std::cerr << "device set" << std::endl;
	if (status == 0) {
		printf("set device!\n");
		status = vcapture_start(v);
		if (status == 0) {
			printf("started!\n");
			for (int j=0; j<50; j++) {
				std::cerr << "j=" << j << std::endl;
				vcapture_poll(1000);
			}
			vcapture_stop(v);
		}
	}
	vcapture_free(v);
//    HelloPrx twoway = HelloPrx::checkedCast(
//        communicator()->propertyToProxy("Hello.Proxy")->ice_twoway()->ice_timeout(-1)->ice_secure(false));
//    if(!twoway)
//    {
//        cerr << argv[0] << ": invalid proxy" << endl;
//        return EXIT_FAILURE;
//    }
//    HelloPrx oneway = twoway->ice_oneway();

	return EXIT_SUCCESS;
}

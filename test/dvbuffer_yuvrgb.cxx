#include <cvd/Linux/dvbuffer.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/colourspace.h>
#include <cvd/Linux/dvbuffer.h>


using namespace CVD;

typedef  Rgb<byte>pix;
#define  DATA_TYPE GL_RGB

class cs: public VideoBuffer<Rgb<byte> >, public DC::RawDCVideo
{
	public:
		cs()
		:RawDCVideo(0, 3, -1, -1, MODE_640x480_YUV411, FRAMERATE_30)
		{
		}

		virtual ~cs()
		{
		}

		virtual ImageRef size()
		{
			return RawDCVideo::size();
		}

		virtual VideoFrame<pix>* get_frame()
		{
			return reinterpret_cast<VideoFrame<pix>*>(RawDCVideo::get_frame());
		}
		
		virtual void put_frame(VideoFrame<pix>* f)
		{
			RawDCVideo::put_frame(reinterpret_cast<VideoFrame<byte>*>(f));
		}

		virtual bool frame_pending()
		{
			return RawDCVideo::frame_pending();
		}
	
		virtual void seek_to(unsigned long long){}
};

VideoBuffer<pix>* get_vbuf()
{
	return new cs();
}

#define HACK 	VideoFrame<pix>* vvf = vbuf->get_frame();\
				Image<pix> vfrgb(vvf->size()), *vf;\
				ColourSpace::yuv411_to_rgb((byte*)vvf->data(), vvf->totalsize(), (byte*)vfrgb.data());\
				vf = &vfrgb;\
				vbuf->put_frame(vvf);
				
				
					

#include "test/videoprog.cxx"

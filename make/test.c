#ifdef test_dv
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#endif

#ifdef test_x11
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#endif

#ifdef test_jpeg
#include <stdio.h>
#include <jpeglib.h>
#endif

#ifdef test_ffmpeg
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#endif

#ifdef test_tiff
#include <tiffio.h>
#endif

int main()
{

#ifdef test_ffmpeg
// Check for version 0.4.9-pre1 or higher
// (Since that version introduces this function, which I use)
    AVPacket packet;
	AVFormatContext ctx;
   	av_read_frame(&ctx, &packet);
#endif
	
	#ifdef test_tiff
		TIFF* tiff;
		uint32* a;
		TIFFReadRGBAImageOriented(tiff, 0, 0, a, 0);
	#endif

}

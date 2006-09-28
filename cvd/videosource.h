#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <cvd/config.h>

#include <cvd/diskbuffer2.h>
#include <cvd/readaheadvideobuffer.h>

#if CVD_HAVE_FFMPEG
#include <cvd/videofilebuffer.h>
#endif

#if CVD_HAVE_V4L2BUFFER
#include <cvd/Linux/v4lbuffer.h>
#endif

#if CVD_HAVE_DVBUFFER
#include <cvd/Linux/dvbuffer.h>
#endif

namespace CVD {
    struct ParseException : public Exceptions::All
    {
	ParseException(const std::string& what_) { what = what_; }
    };
    
    struct VideoSourceException : public Exceptions::All
    {
	VideoSourceException(const std::string& what_) { what = what_; }
    };

    struct VideoSource 
    {
	std::string protocol;
	std::string identifier;
	typedef std::vector<std::pair<std::string,std::string> > option_list;
	option_list options;
    };

    std::ostream& operator<<(std::ostream& out, const VideoSource& vs);

    void parse(std::istream& in, VideoSource& vs);
    
    template <class T> VideoBuffer<T>* makeDiskBuffer2(const std::vector<std::string>& files, double fps, VideoBufferFlags::OnEndOfBuffer eob)
    {
	return new DiskBuffer2<T>(files, fps, eob);    
    }

    void get_files_options(const VideoSource& vs, int& fps, int& ra_frames, VideoBufferFlags::OnEndOfBuffer& eob);
    
#if CVD_HAVE_V4L2BUFFER
    template <class T> VideoBuffer<T>* makeV4LBuffer(const std::string& dev, const ImageRef& size, int input, bool interlaced)
    {
	throw VideoSourceException("V4LBuffer cannot handle types other than byte, bayer, yuv422, Rgb<byte>");
    }

    template <> VideoBuffer<byte>* makeV4LBuffer(const std::string& dev, const ImageRef& size, int input, bool interlaced);
    template <> VideoBuffer<bayer>* makeV4LBuffer(const std::string& dev, const ImageRef& size, int input, bool interlaced);
    template <> VideoBuffer<yuv422>* makeV4LBuffer(const std::string& dev, const ImageRef& size, int input, bool interlaced);
    template <> VideoBuffer<Rgb<byte> >* makeV4LBuffer(const std::string& dev, const ImageRef& size, int input, bool interlaced);

    void get_v4l2_options(const VideoSource& vs, ImageRef& size, int& input, bool& interlaced);

#endif


#if CVD_HAVE_FFMPEG    
    template <class T> VideoBuffer<T>* makeVideoFileBuffer(const std::string& file, VideoBufferFlags::OnEndOfBuffer eob)
    {
	throw VideoSourceException("VideoFileBuffer cannot handle types other than byte, Rgb<byte>");
    }
    
    template <> VideoBuffer<byte>* makeVideoFileBuffer(const std::string& file, VideoBufferFlags::OnEndOfBuffer eob);
    template <> VideoBuffer<Rgb<byte> >* makeVideoFileBuffer(const std::string& file, VideoBufferFlags::OnEndOfBuffer eob);

    void get_file_options(const VideoSource& vs, int& ra_frames, VideoBufferFlags::OnEndOfBuffer& eob);

#endif

#if CVD_HAVE_DVBUFFER
    template <class T> VideoBuffer<T>* makeDVBuffer2(int cam, int dmabufs, int bright, int exposure, int fps)
    {
	throw VideoSourceException("DVBuffer2 cannot handle types other than byte, Rgb<byte>");
    }
    
    template <> VideoBuffer<byte>* makeDVBuffer2(int cam, int dmabufs, int bright, int exposure, int fps);
    template <> VideoBuffer<Rgb<byte> >* makeDVBuffer2(int cam, int dmabufs, int bright, int exposure, int fps);

    void get_dc1394_options(const VideoSource& vs, int& dma_bufs, int& bright, int& exposure, int& fps);

#endif

    template <class T> VideoBuffer<T>* open_video_source(const VideoSource& vs)
    {
	if (vs.protocol == "files") {
	    int fps, ra_frames=0;
	    VideoBufferFlags::OnEndOfBuffer eob;
	    get_files_options(vs, fps, ra_frames, eob);
	    VideoBuffer<T>* vb = makeDiskBuffer2<T>(globlist(vs.identifier), fps, eob);
	    if (ra_frames)
		vb = new ReadAheadVideoBuffer<T>(*vb, ra_frames);
	    return vb;
	}
#if CVD_HAVE_V4L2BUFFER
	else if (vs.protocol == "v4l2") {
	    ImageRef size;
	    int input;
	    bool interlaced;
	    get_v4l2_options(vs, size, input, interlaced);
	    return makeV4LBuffer<T>(vs.identifier, size, input, interlaced);	
	} 
#endif
#if CVD_HAVE_DVBUFFER
	else if (vs.protocol == "dc1394") {
	    int cam_no = atoi(vs.identifier.c_str());
	    int dma_bufs, bright, exposure, fps;
	    get_dc1394_options(vs, dma_bufs, bright, exposure, fps);
	    return makeDVBuffer2<T>(cam_no, dma_bufs, bright, exposure, fps);
	} 
#endif
#if CVD_HAVE_FFMPEG
	else if (vs.protocol == "file") {
	    int ra_frames = 0;
	    VideoBufferFlags::OnEndOfBuffer eob;
	    get_file_options(vs, ra_frames, eob);
	    VideoBuffer<T>* vb = makeVideoFileBuffer<T>(vs.identifier, eob);
	    if (ra_frames)
		vb = new ReadAheadVideoBuffer<T>(*vb, ra_frames);
	    return vb;
	} 
#endif
	else
	    throw VideoSourceException("undefined video source protocol: '" + vs.protocol + "'\n\t valid protocols: "
#if CVD_HAVE_FFMPEG
				       "file, "
#endif
#if CVD_HAVE_V4L2BUFFER
				       "v4l2, "
#endif
#if CVD_HAVE_DVBUFFER
				       "dc1394, "
#endif
				       "files");
    }
    
    template <class T> VideoBuffer<T>* open_video_source(std::istream& in)
    {
	VideoSource vs;
	parse(in, vs);
	return open_video_source<T>(vs);
    }
    
    template <class T> VideoBuffer<T>* open_video_source(const std::string& src)
    {
	std::istringstream in(src);
	return open_video_source<T>(in);
    }    
}

#endif

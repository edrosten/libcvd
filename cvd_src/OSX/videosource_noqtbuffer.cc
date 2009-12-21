#include <cvd/videosource.h>
#include <cvd/OSX/qtbuffer.h>
namespace CVD{

	template <> VideoBuffer<vuy422> * makeQTBuffer( const ImageRef & /*size*/, int /*input*/, bool /*showsettings*/, bool /*verbose*/)
	{
		throw VideoSourceException("QTBuffer is not compiled in to libcvd.");
	}

	template <> VideoBuffer<yuv422> * makeQTBuffer( const ImageRef & /*size*/, int /*input*/, bool /*showsettings*/, bool /*verbose*/)
	{
		throw VideoSourceException("QTBuffer is not compiled in to libcvd.");
	}
	
	template <> VideoBuffer<Rgb<byte> > * makeQTBuffer( const ImageRef & /*size*/, int /*input*/, bool /*showsettings*/, bool /*verbose*/)
	{
		throw VideoSourceException("QTBuffer is not compiled in to libcvd.");
	}
}

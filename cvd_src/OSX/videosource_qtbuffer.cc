#include <cvd/videosource.h>
#include <cvd/OSX/qtbuffer.h>
namespace CVD{

	template <> VideoBuffer<vuy422> * makeQTBuffer( const ImageRef & size, int input, bool showsettings, bool verbose)
	{
		return new CVD::QTBuffer<vuy422>(size, input, showsettings, verbose);
	}

	template <> VideoBuffer<yuv422> * makeQTBuffer( const ImageRef & size, int input, bool showsettings, bool verbose)
	{
		return new CVD::QTBuffer<yuv422>(size, input, showsettings, verbose);
	}

	template <> VideoBuffer<Rgb<byte> > * makeQTBuffer( const ImageRef & size, int input, bool showsettings, bool verbose)
	{
		return new CVD::QTBuffer<Rgb<byte> >(size, input, showsettings, verbose);
	}

}

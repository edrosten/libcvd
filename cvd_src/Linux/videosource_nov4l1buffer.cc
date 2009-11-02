#include <cvd/videosource.h>

namespace CVD{

	template <> VideoBuffer<byte>* makeV4L1Buffer(const std::string& , const ImageRef&)
	{
		throw VideoSourceException("V4L1Buffer is not compiled in to libcvd.");
	}

	template <> VideoBuffer<yuv422>* makeV4L1Buffer(const std::string& , const ImageRef&)
	{
		throw VideoSourceException("V4L1Buffer is not compiled in to libcvd.");
	}
	template <> VideoBuffer<Rgb<byte> >* makeV4L1Buffer(const std::string& , const ImageRef&)
	{
		throw VideoSourceException("V4L1Buffer is not compiled in to libcvd.");
	}

	template <> VideoBuffer<bayer>* makeV4L1Buffer(const std::string& , const ImageRef&)
	{
		throw VideoSourceException("V4L1Buffer is not compiled in to libcvd.");
	}
}

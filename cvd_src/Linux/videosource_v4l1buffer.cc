#include <cvd/videosource.h>
#include <cvd/Linux/v4l1buffer.h>

namespace CVD{
	template <> VideoBuffer<byte>* makeV4L1Buffer(const std::string& dev, const ImageRef& size)
	{
		return new V4L1Buffer<byte>(dev, size);
	}

	template <> VideoBuffer<yuv422>* makeV4L1Buffer(const std::string& dev, const ImageRef& size)
	{
		return new V4L1Buffer<yuv422>(dev, size);
	}
	template <> VideoBuffer<Rgb<byte> >* makeV4L1Buffer(const std::string& dev, const ImageRef& size)
	{
		return new V4L1Buffer<Rgb<byte> >(dev, size);
	}

	template <> VideoBuffer<bayer>* makeV4L1Buffer(const std::string& dev, const ImageRef& size)
	{
		return new V4L1Buffer<bayer>(dev, size);
	}

}

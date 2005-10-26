#include <cvd/vision.h>
#include <cvd/config.h>

using namespace std;

namespace CVD {

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
void halfSample(const BasicImage<byte>& in, BasicImage<byte>& out)
{   

	if( (in.size()/2) != out.size())
        throw Exceptions::Vision::IncompatibleImageSizes("halfSample");
    Internal::halfsample(in.data(), out.data(), in.size().x, in.size().y);
}
#endif

#if defined(CVD_HAVE_SSE) && defined(CVD_HAVE_CPU_i686)
void gradient(const BasicImage<byte>& im, BasicImage<float[2]>& out)
{ 
	if( im.size() != out.size())
        throw Exceptions::Vision::IncompatibleImageSizes("gradient");
    Internal::byte_to_float_gradient(im.data(), out.data(), im.size().x, im.size().y);
    zeroBorders(out);
}
#endif

#if defined(CVD_HAVE_SSE2) && defined(CVD_HAVE_CPU_i686)
void gradient(const BasicImage<byte>& im, BasicImage<double[2]>& out)
{
	if( im.size() != out.size())
        throw Exceptions::Vision::IncompatibleImageSizes("gradient");
    Internal::byte_to_double_gradient(im.data(), out.data(), im.size().x, im.size().y);
    zeroBorders(out);
}
#endif

};

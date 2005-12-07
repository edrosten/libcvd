#include <cvd/vision.h>
#include <cvd/config.h>
// internal functions used by CVD vision algorithm implementations
#include <cvd/internal/assembly.h>

using namespace std;

namespace CVD {

#if defined(CVD_HAVE_MMXEXT) && defined(CVD_HAVE_CPU_i686)
void halfSample(const BasicImage<byte>& in, BasicImage<byte>& out)
{   
  if( (in.size()/2) != out.size())
    throw Exceptions::Vision::IncompatibleImageSizes("halfSample");
  if (!is_aligned<8>(in.data()) || !is_aligned<8>(out.data()) || (in.size().x % 8 != 0))
    halfSample<byte>(in, out);
  else
    Internal::Assembly::halfsample(in.data(), out.data(), in.size().x, in.size().y);
}
#endif


};

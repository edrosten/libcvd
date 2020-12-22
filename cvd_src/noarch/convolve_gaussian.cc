#include <cvd/convolution.h>
using namespace std;

namespace CVD
{

// Try to choose the fastest method
void convolveGaussianIIR(const BasicImage<float>& I, BasicImage<float>& out, double sigma, double sigmas)
{
	int ksize = (int)ceil(sigma * sigmas);
	if(ksize > 6)
	{
		double b[3];
		compute_van_vliet_b(sigma, b);
		van_vliet_blur(b, I, out);
	}
	else
		convolveGaussian(I, out, sigma, sigmas);
}
template<>
void convolveGaussian<Convolution::Clamp,float,float>(const BasicImage<float>& I, BasicImage<float>& out, double sigma, double sigmas)
{
	Internal::convolveGaussianGeneric<Convolution::Clamp, float, float>(I, out, sigma, sigmas);
}
}

#include "cvd/convolution.h"

namespace CVD
{
	void convolveGaussian(const BasicImage<float>& I, BasicImage<float>& out, double sigma, double sigmas)
	{
		convolveGaussian<float>(I, out, sigma, sigmas);
	}
}

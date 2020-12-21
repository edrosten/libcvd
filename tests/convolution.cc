#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>


#include <cvd/convolution.h>
#include <cvd/image.h>

using namespace CVD;

int main(int, char**)
{
	Image<double> img(ImageRef(2, 5));
	Image<double> out(img.size());
	convolveGaussian(img, out, 1.0);
}

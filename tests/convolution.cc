

#include <cvd/convolution.h>
#include <cvd/image.h>

using namespace CVD;

int main(int, char**)
{
	Image<float> img(ImageRef(2, 5));
	Image<float> out(img.size());
	convolveGaussian(img, out, 1.0);
}

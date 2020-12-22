
#ifndef CVD_CONVOLUTION_H_
#define CVD_CONVOLUTION_H_

#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

#include <cvd/config.h>
#include <cvd/exceptions.h>
#include <cvd/image.h>
#include <cvd/internal/pixel_operations.h>
#include <cvd/utility.h>

namespace CVD
{

template <class T>
void convolveGaussian5_1(BasicImage<T>& I)
{
	int w = I.size().x;
	int h = I.size().y;
	int i, j;
	for(j = 0; j < w; j++)
	{
		T* src = I.data() + j;
		T* end = src + w * (h - 4);
		while(src != end)
		{
			T sum = (T)(0.0544887 * (src[0] + src[4 * w])
			    + 0.2442010 * (src[w] + src[3 * w])
			    + 0.4026200 * src[2 * w]);
			*(src) = sum;
			src += w;
		}
	}
	for(i = h - 5; i >= 0; i--)
	{
		T* src = I.data() + i * w;
		T* end = src + w - 4;
		while(src != end)
		{
			T sum = (T)(0.0544887 * (src[0] + src[4])
			    + 0.2442010 * (src[1] + src[3])
			    + 0.4026200 * src[2]);
			*(src + 2 * w + 2) = sum;
			++src;
		}
	}
}

namespace Exceptions
{

	/// %Exceptions specific to vision algorithms
	/// @ingroup gException
	namespace Convolution
	{
		/// Base class for all Image_IO exceptions
		/// @ingroup gException
		struct All : public CVD::Exceptions::All
		{
			using CVD::Exceptions::All::All;
		};

		/// Input images have incompatible dimensions
		/// @ingroup gException
		struct IncompatibleImageSizes : public All
		{
			IncompatibleImageSizes(const std::string& function)
			    : All("Incompatible image sizes in " + function)
			{
			}
		};

		/// Input images have incompatible dimensions
		/// @ingroup gException
		struct OddSizedKernelRequired : public All
		{
			OddSizedKernelRequired(const std::string& function)
			    : All("Odd sized kernel required in " + function) {};
		};
	}
}
//void convolveGaussian5_1(BasicImage<byte>& I);

/// convolves an image with a box of given size.
/// @param I input image, modified in place
/// @param hwin window size, this is half of the box size
/// @ingroup gVision
template <class T>
void convolveWithBox(const BasicImage<T>& I, BasicImage<T>& J, ImageRef hwin)
{
	typedef typename Pixel::traits<T>::wider_type sum_type;
	if(I.size() != J.size())
	{
		throw Exceptions::Convolution::IncompatibleImageSizes("convolveWithBox");
	}
	int w = I.size().x;
	int h = I.size().y;
	ImageRef win = 2 * hwin + ImageRef(1, 1);
	const double factor = 1.0 / (win.x * win.y);
	std::vector<sum_type> buffer(w * win.y);
	std::vector<sum_type> sums_v(w);
	sum_type* sums = &sums_v[0];
	sum_type* next_row = &buffer[0];
	sum_type* oldest_row = &buffer[0];
	zeroPixels(sums, w);
	const T* input = I.data();
	T* output = J[hwin.y] - hwin.x;
	for(int i = 0; i < h; i++)
	{
		sum_type hsum = sum_type();
		const T* back = input;
		int j;
		for(j = 0; j < win.x - 1; j++)
			hsum += input[j];
		for(; j < w; j++)
		{
			hsum += input[j];
			next_row[j] = hsum;
			sums[j] += hsum;
			hsum -= *(back++);
		}
		if(i >= win.y - 1)
		{
			assign_multiple(sums + win.x - 1, factor, output + win.x - 1, w - win.x + 1);
			differences(sums + win.x - 1, oldest_row + win.x - 1, sums + win.x - 1, w - win.x + 1);
			output += w;
			oldest_row += w;
			if(oldest_row == &buffer[0] + w * win.y)
				oldest_row = &buffer[0];
		}
		input += w;
		next_row += w;
		if(next_row == &buffer[0] + w * win.y)
			next_row = &buffer[0];
	}
}

template <class T>
inline void convolveWithBox(const BasicImage<T>& I, BasicImage<T>& J, int hwin)
{
	convolveWithBox(I, J, ImageRef(hwin, hwin));
}

template <class T>
inline void convolveWithBox(BasicImage<T>& I, int hwin)
{
	convolveWithBox(I, I, hwin);
}

template <class T>
inline void convolveWithBox(BasicImage<T>& I, ImageRef hwin)
{
	convolveWithBox(I, I, hwin);
}





template<class T>
constexpr std::vector<T> normalizedGaussianHalfKernel(double sigma, double sigmas){
	int radius = (int)ceil(sigmas * sigma);

	std::vector<T> kernel(radius+1);
	kernel[0] = 1;
	T sum = 1;

	for(int i=1; i <= radius; i++){
		kernel[i] = static_cast<T>(std::exp(-i*i/(2*sigma*sigma)));
		sum += 2*kernel[i];
	}

	for(T& k: kernel)
		k /= sum;
	
	return kernel;
}


namespace Internal {

constexpr inline int positive_modulo(int value, int m) {
    int mod = value % (int)m;
    if (value < 0) {
        mod += m;
    }
    return mod;
}

///101 mirroring is:
//  dcb|abcde|dcb
// For more larger ranges it's
//    abcb|abc|babc
// abcdcb|abcd|cbabcb
//
// The repeating unit is of
// length n + (n-2) 
// abcdcb|abcdcb|abcdcb|
// which gives the index into the larger pattern as: i1 = positive_modulo(index, 2*n-2);
constexpr inline int mirror_index_101(int i, int n){
	int large_i = positive_modulo(i, 2*n-2);
	return large_i - std::max(0, large_i-(n-1))*2;
}

constexpr inline int clamp_index(int i, int range){
	return std::max(0,std::min(i, range-1));
}

constexpr inline int unchecked_index(int i, int){
	return i;
}

/* Generic thing-doer class (for template reasons) for performing a single instance
   of convolution. It is generic over both the indexable object and remapping of
   indices.
*/
template<typename Output, typename Sum, int(*index)(int,int)>
struct ConvolveOne {
	template<typename Indexable, typename Kernel>
	static constexpr Output op(const Indexable& row, const int position, const int size, const Kernel& kernel){
		assert(position >= 0);
		assert(position < size);

		Sum sum = kernel[0] * row[position];
		for(int i=1; i < static_cast<int>(kernel.size()); i++)
			sum += static_cast<Sum>(kernel[i]*(row[index(i+position, size)] + row[index(-i+position, size)]));
		return static_cast<Output>(sum);
	}
};


template<typename Output, typename Sum> using ConvolveCentre = ConvolveOne<Output, Sum, unchecked_index>;
}

namespace Convolution{
template<typename Output, typename Sum> using Mirror101 = Internal::ConvolveOne<Output, Sum, Internal::mirror_index_101>;
template<typename Output, typename Sum> using Clamp = Internal::ConvolveOne<Output, Sum, Internal::clamp_index>;
}


namespace Internal{
// Struct to allow indexing of a column of an image
template<typename C> struct Column{
	
	public:
		constexpr Column(const BasicImage<C>& image, int column)
		:data(&image[0][column]), stride(image.row_stride())
		{
		}

		constexpr const C& operator[](int i) const{
			return data[i*stride];
		}
	
	private:
		const C* data;
		int stride;
};



template<typename Sum, template<class,class>class ConvolveEdge, typename Output, typename Input, typename Kernel>
constexpr void convolveSymmetricRow(Output* row_buffer, const Input* row, const int width, const Kernel& half_kernel){
	const int kernel_centre = static_cast<int>(half_kernel.size()-1);

	for(int column = 0; column < std::min(kernel_centre, width); column++)
		row_buffer[column] = ConvolveEdge<Output, Sum>::op(row, column, width, half_kernel);

	for(int column = kernel_centre; column < width - kernel_centre; column++)
		row_buffer[column] = ConvolveCentre<Output, Sum>::op(row, column, width, half_kernel);
	
	if(width > kernel_centre)
		for(int column = width - kernel_centre; column < width; column++)
			row_buffer[column] = ConvolveEdge<Output, Sum>::op(row, column, width, half_kernel);
}



template<typename Input, typename Output, class SumType, template<class,class>class ConvolveEdge, class Kernel>
constexpr void convolveSeperableSymmetric(const BasicImage<Input>& in, BasicImage<Output>& out, const Kernel& half_kernel){
	assert(out.size() == in.size());
	
	//The algorithm performs a single pass top to bottom. The row convolution results are stored in
	//a buffer big enough to do a single left-to-right sweep of vertical convolutions
	//FIXME!! make this a rolling buffer?
	Image<SumType> row_buffer{in.size()};
	row_buffer.fill(0);
	
	const int kernel_centre = static_cast<int>(half_kernel.size()-1);

	for(int row=0; row < in.size().y; row++){
		convolveSymmetricRow<SumType, ConvolveEdge>(row_buffer[row], in[row], in.size().x, half_kernel);

		//Once the buffer is full enough, start performing the horizontal sweeps of the vertical
		//convolution
		int output_row = row - kernel_centre;
		if(output_row >= 0) {
			for(int column=0; column < in.size().x; column++){
				if(output_row < kernel_centre)
					out[output_row][column] = ConvolveEdge<Output,SumType>::op(Column{row_buffer, column}, output_row, in.size().y, half_kernel);
				else
					out[output_row][column] = ConvolveCentre<Output,SumType>::op(Column{row_buffer, column}, output_row, in.size().y, half_kernel);
			}
		}
	}
	
	//Finish off the convolution
	for(int row=std::max(0, in.size().y-kernel_centre); row < in.size().y; row++)
		for(int column=0; column < in.size().x; column++)
			out[row][column] = ConvolveEdge<Output,SumType>::op(Column{row_buffer, column}, row, in.size().y, half_kernel);

}

// This is separated off so that test /intergration code can forcibly invoke
// the non specialised code path
template<template<class,class>class ConvolveEdge, typename Input, typename Output>
void convolveGaussianGeneric(const BasicImage<Input>& in, BasicImage<Output>& out, double sigma, double sigmas){
	
	if(in.size() != out.size())
		throw Exceptions::Convolution::IncompatibleImageSizes("ConvolveGaussian");

	if(sigma == 0 || sigmas == 0){
		std::copy(in.begin(), in.end(), out.begin());
		return;
	}

	typedef typename Pixel::traits<typename Pixel::Component<Input>::type>::float_type sum_component_type;
	typedef typename Pixel::traits<Input>::float_type SumType;

	std::vector<sum_component_type> half_kernel = normalizedGaussianHalfKernel<sum_component_type>(sigma, sigmas);

	if(half_kernel.size() == 1){
		std::copy(in.begin(), in.end(), out.begin());
		return;
	}
	Internal::convolveSeperableSymmetric<Input, Output, SumType, ConvolveEdge>(in, out, half_kernel);
}

}

template<template<class,class>class ConvolveEdge= Convolution::Clamp, typename Input, typename Output>
void convolveGaussian(const BasicImage<Input>& in, BasicImage<Output>& out, double sigma, double sigmas=3.0){
	Internal::convolveGaussianGeneric<ConvolveEdge>(in, out, sigma, sigmas);
}

void convolveGaussianIIR(const BasicImage<float>& I, BasicImage<float>& out, double sigma, double sigmas = 3.0);


template<>
void convolveGaussian<Convolution::Clamp,float,float>(const BasicImage<float>& I, BasicImage<float>& out, double sigma, double sigmas);


void compute_van_vliet_b(double sigma, double b[]);
void compute_triggs_M(const double b[], double M[][3]);
void van_vliet_blur(const double b[], const BasicImage<float> in, BasicImage<float> out);





} // namespace CVD

#endif

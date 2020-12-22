#define CVD_IMAGE_DEBUG
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cvd/image.h>
#include <cvd/convolution.h>
#include <cvd/internal/pixel_operations.h>
#include <cvd/opencv.h>

#include <cmath>
#include <random>

using namespace std;

namespace CVD{


template<class T>
std::vector<T> normalizedGaussianHalfKernel(double sigma, double sigmas){
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

inline int positive_modulo(int value, int m) {
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
inline int mirror_index_101(int i, int n){
	int large_i = positive_modulo(i, 2*n-2);
	return large_i - max(0, large_i-(n-1))*2;
}

inline int clamp_index(int i, int range){
	return max(0,min(i, range-1));
}

inline int unchecked_index(int i, int){
	return i;
}

/* Generic thing-doer class (for template reasons) for performing a single instance
   of convolution. It is generic over both the indexable object and remapping of
   indices.
*/
template<typename Output, typename Sum, int(*index)(int,int)>
struct ConvolveOne {
	template<typename Indexable, typename Kernel>
	static Output op(const Indexable& row, const int position, const int size, const Kernel& kernel){
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
		Column(const BasicImage<C>& image, int column)
		:data(&image[0][column]), stride(image.row_stride())
		{
		}

		const C& operator[](int i) const{
			return data[i*stride];
		}
	
	private:
		const C* data;
		int stride;
};



template<typename Sum, template<class,class>class ConvolveEdge, typename Output, typename Input, typename Kernel>
void convolveSymmetricRow(Output* row_buffer, const Input* row, const int width, const Kernel& half_kernel){
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
void convolveSeperableSymmetric(const BasicImage<Input>& in, BasicImage<Output>& out, const Kernel& half_kernel){
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

}

template<typename Input, typename Output=Input, template<class,class>class ConvolveEdge = Convolution::Clamp>
void convolveGaussian2(const BasicImage<Input>& in, BasicImage<Output>& out, double sigma, double sigmas=3.0){
	if(sigma == 0 || sigmas == 0){
		out.copy_from(in);
		return;
	}

	typedef typename Pixel::traits<typename Pixel::Component<Input>::type>::float_type sum_component_type;
	typedef typename Pixel::traits<Input>::float_type SumType;

	std::vector<sum_component_type> half_kernel = normalizedGaussianHalfKernel<sum_component_type>(sigma, sigmas);

	if(half_kernel.size() == 1){
		out.copy_from(in);
		return;
	}
	Internal::convolveSeperableSymmetric<Output, Input, SumType, ConvolveEdge>(in, out, half_kernel);
}

}

using namespace CVD;

TEST_CASE("gaussian window"){
	std::vector<double> k = normalizedGaussianHalfKernel<double>(2.5, 4.5);
	double sum = std::accumulate(k.begin()+1, k.end(), 0.0)*2 + k[0];
	REQUIRE(sum == Approx(1));
}


TEST_CASE("mirror 101"){
	
	const std::string in="abcd";
	//  Repeating unit ------------------vvvv--------------
	const std::string out = "abcdcbabcdcbabcdcbabcdcbabcdcb";
	
	string s;
	for(int i=-12; i <18; i++)
		s += in[Internal::mirror_index_101(i, 4)];

	REQUIRE(s == out);
}

#include <iomanip>


template<class C>
void print(const Image<C>& im){
	
	for(int r=0; r < im.size().y; r++){

		for(int c=0; c < im.size().x; c++)
			cout << setprecision(4) << setw(8) << im[r][c] << " ";
		cout << endl;
	}
	cout << endl;
}


template<class T>
double absdiff(const BasicImage<T>& out, const BasicImage<T>& out_cv){
	double sum=0;
	for(int r=0; r < out.size().y; r++)
		for(int c=0; c < out.size().x; c++)
			sum += std::abs(out[r][c] - out_cv[r][c]);
	return sum;
}	



TEST_CASE("double to double"){
	std::mt19937 eng;
	std::normal_distribution<> unit_normal(0, 1);
	std::uniform_int_distribution random_image_size(2, 100);
	std::uniform_real_distribution random_sigma(.1, 20.);
	std::uniform_real_distribution random_sigmas(1., 4.);
	
	
	SECTION("101 mirroring, large kernel"){
		for(int i=0; i < 100;){

			int s = random_image_size(eng);
			ImageRef im_size{s,s};

			Image<double> in(im_size), out(im_size, -1e99), out_opencv(im_size, 0);
			for(double& d: in)
				d = unit_normal(eng);

			double sigma=random_sigma(eng);
			double sigmas=random_sigmas(eng);
			int radius = (int)ceil(sigmas * sigma);
			int ksize = 2*radius+1;

			if(ksize <= s)
				continue;

			convolveGaussian2<double,double,Convolution::Mirror101>(in, out, sigma, sigmas);
			cv::GaussianBlur(toMat(in), toMat(out_opencv), cv::Size(ksize,ksize), sigma, 0, cv::BORDER_REFLECT_101);
			REQUIRE(absdiff(out, out_opencv)  < 1e-8);
			i++;
		}
	}

	SECTION("101 mirroring, small kernel"){
		for(int i=0; i < 100;){

			int s = random_image_size(eng);
			ImageRef im_size{s,s};

			Image<double> in(im_size), out(im_size, -1e99), out_opencv(im_size, 0);
			for(double& d: in)
				d = unit_normal(eng);

			double sigma=random_sigma(eng);
			double sigmas=random_sigmas(eng);
			int radius = (int)ceil(sigmas * sigma);
			int ksize = 2*radius+1;

			if(ksize > s)
				continue;

			convolveGaussian2<double,double,Convolution::Mirror101>(in, out, sigma, sigmas);
			cv::GaussianBlur(toMat(in), toMat(out_opencv), cv::Size(ksize,ksize), sigma, 0, cv::BORDER_REFLECT_101);
			REQUIRE(absdiff(out, out_opencv)  < 1e-8);
			i++;
		}
	}


	SECTION("Clamping, large kernel"){
		for(int i=0; i < 100;){

			int s = random_image_size(eng);
			ImageRef im_size{s,s};

			Image<double> in(im_size), out(im_size, -1e99), out_opencv(im_size, 0);
			for(double& d: in)
				d = unit_normal(eng);

			double sigma=random_sigma(eng);
			double sigmas=random_sigmas(eng);
			int radius = (int)ceil(sigmas * sigma);
			int ksize = 2*radius+1;

			if(ksize <= s)
				continue;

			convolveGaussian2<double,double,Convolution::Clamp>(in, out, sigma, sigmas);
			cv::GaussianBlur(toMat(in), toMat(out_opencv), cv::Size(ksize,ksize), sigma, 0, cv::BORDER_REPLICATE);
			REQUIRE(absdiff(out, out_opencv)  < 1e-8);
			i++;
		}
	}

	SECTION("Clamping, small kernel"){
		for(int i=0; i < 100;){

			int s = random_image_size(eng);
			ImageRef im_size{s,s};

			Image<double> in(im_size), out(im_size, -1e99), out_opencv(im_size, 0);
			for(double& d: in)
				d = unit_normal(eng);

			double sigma=random_sigma(eng);
			double sigmas=random_sigmas(eng);
			int radius = (int)ceil(sigmas * sigma);
			int ksize = 2*radius+1;

			if(ksize > s)
				continue;

			convolveGaussian2<double,double,Convolution::Clamp>(in, out, sigma, sigmas);
			cv::GaussianBlur(toMat(in), toMat(out_opencv), cv::Size(ksize,ksize), sigma, 0, cv::BORDER_REPLICATE);
			REQUIRE(absdiff(out, out_opencv)  < 1e-8);
			i++;
		}
	}
}




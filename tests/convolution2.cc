#define CVD_IMAGE_DEBUG
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cvd/image.h>
#include <cvd/convolution.h>
#include <cvd/internal/pixel_operations.h>
#include <cvd/opencv.h>

#include <cmath>
#include <random>

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
	
	std::string s;
	for(int i=-12; i <18; i++)
		s += in[Internal::mirror_index_101(i, 4)];

	REQUIRE(s == out);
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

			convolveGaussian<Convolution::Mirror101>(in, out, sigma, sigmas);
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

			convolveGaussian<Convolution::Mirror101>(in, out, sigma, sigmas);
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

			convolveGaussian<Convolution::Clamp>(in, out, sigma, sigmas);
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

			convolveGaussian<Convolution::Clamp>(in, out, sigma, sigmas);
			cv::GaussianBlur(toMat(in), toMat(out_opencv), cv::Size(ksize,ksize), sigma, 0, cv::BORDER_REPLICATE);
			REQUIRE(absdiff(out, out_opencv)  < 1e-8);
			i++;
		}
	}
}


TEST_CASE("float to float"){
	std::mt19937 eng;
	std::normal_distribution<float> unit_normal(0, 1);
	std::uniform_int_distribution random_image_size(2, 100);
	std::uniform_real_distribution random_sigma(.1, 20.);
	std::uniform_real_distribution random_sigmas(1., 4.);

	SECTION("Clamping, large kernel"){
		for(int i=0; i < 100;){

			int s = random_image_size(eng);
			ImageRef im_size{s,s};

			Image<float> in(im_size), out(im_size, -1e10), out_opencv(im_size, 0);
			for(float& d: in)
				d = unit_normal(eng);

			double sigma=random_sigma(eng);
			double sigmas=random_sigmas(eng);
			int radius = (int)ceil(sigmas * sigma);
			int ksize = 2*radius+1;

			if(ksize <= s)
				continue;

			convolveGaussian<Convolution::Clamp>(in, out, sigma, sigmas);
			cv::GaussianBlur(toMat(in), toMat(out_opencv), cv::Size(ksize,ksize), sigma, 0, cv::BORDER_REPLICATE);
			REQUIRE(absdiff(out, out_opencv)  < 1e-3);
			i++;
		}
	}

	SECTION("Clamping, small kernel"){
		for(int i=0; i < 100;){

			int s = random_image_size(eng);
			ImageRef im_size{s,s};

			Image<float> in(im_size), out(im_size, -1e10), out_opencv(im_size, 0);
			for(float& d: in)
				d = unit_normal(eng);

			double sigma=random_sigma(eng);
			double sigmas=random_sigmas(eng);
			int radius = (int)ceil(sigmas * sigma);
			int ksize = 2*radius+1;

			if(ksize > s)
				continue;

			convolveGaussian<Convolution::Clamp>(in, out, sigma, sigmas);
			cv::GaussianBlur(toMat(in), toMat(out_opencv), cv::Size(ksize,ksize), sigma, 0, cv::BORDER_REPLICATE);
			REQUIRE(absdiff(out, out_opencv)  < 1e-3);
			i++;
		}
	}
}


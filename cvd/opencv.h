#ifndef LIBCVD_OPENCV_H
#define LIBCVD_OPENCV_H

#include <cvd/byte.h>
#include <cvd/image.h>
#include <cvd/rgb.h>
#include <cvd/rgba.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace CVD
{
namespace Internal
{

	template <class C>
	struct opencv_traits
	{
		static const int depth = cv::DataDepth<C>::value;
		static const int channels = 1;
	};

	template <class C>
	struct opencv_traits<Rgb<C>> : public opencv_traits<C>
	{
		static const int channels = 3;
	};

	template <class C>
	struct opencv_traits<Rgba<C>> : public opencv_traits<C>
	{
		static const int channels = 4;
	};

	template <class C>
	struct opencv_type
	{
		static const int type = CV_MAKETYPE(opencv_traits<C>::depth, opencv_traits<C>::channels);
	};
};

template <class T>
cv::Mat toMat(const CVD::BasicImage<T>& img)
{
	return cv::Mat(img.size().y, img.size().x, Internal::opencv_type<T>::type, (void*)img.data(), img.row_stride() * sizeof(T));
}

template <class T>
void equalizeHist(const CVD::BasicImage<T>& in, CVD::BasicImage<T>& out)
{
	equalizeHist(toMat(in), toMat(out));
}

namespace OpenCV
{
	struct ContourHierarchy
	{
		int next_index;
		int previous_index;
		int first_child_index;
		int parent_index;
	};

	namespace Internal
	{
		void convert_hierarchy(const std::vector<cv::Vec4i>& cv_hierarchy, std::vector<ContourHierarchy>& hierarchy)
		{
			hierarchy.reserve(cv_hierarchy.size());
			std::transform(
			    cv_hierarchy.begin(),
			    cv_hierarchy.end(),
			    std::back_inserter(hierarchy),
			    [](const cv::Vec4i& p) { return ContourHierarchy { p[0], p[1], p[2], p[3] }; });
		}
	}

	template <typename T>
	void resize(const CVD::BasicImage<T>& in, CVD::BasicImage<T>& out, int interpolation = cv::INTER_LINEAR)
	{
		cv::resize(toMat(in), toMat(out), cv::Size(out.size().x, out.size().y), 0, 0, interpolation);
	}
}
}
#endif

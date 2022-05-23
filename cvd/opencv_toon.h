#ifndef LIBCVD_OPENCV_TOON_H
#define LIBCVD_OPENCV_TOON_H

#include "opencv.h"

#include <opencv2/imgproc.hpp>

#include <TooN/TooN.h>

namespace CVD
{
namespace OpenCV
{
	namespace Internal
	{
		void convert_contours(const std::vector<std::vector<cv::Point2i>>& cv_contours, std::vector<std::vector<TooN::Vector<2, int>>>& contours)
		{
			for(const auto& cv_contour : cv_contours)
			{
				auto& contour = contours.emplace_back();
				contour.reserve(cv_contour.size());
				std::transform(
				    cv_contour.begin(),
				    cv_contour.end(),
				    std::back_inserter(contour),
				    [](const cv::Point2i& p) { return TooN::makeVector(p.x, p.y); });
			}
		}
	}

	template <typename T, typename U>
	void warpAffine(const CVD::BasicImage<T>& in, CVD::BasicImage<T>& out, const TooN::Matrix<2, 3, U>& M, int flags = cv::INTER_LINEAR, int borderMode = cv::BORDER_CONSTANT, const T& borderValue = T())
	{
		cv::Mat m_mat(2, 3, CVD::Internal::opencv_type<U>::type, const_cast<U*>(&M(0, 0)));
		cv::warpAffine(toMat(in), toMat(out), m_mat, cv::Size(out.size().x, out.size().y), flags, borderMode, borderValue);
	}

	template <typename T, typename U>
	void warpPerspective(const CVD::BasicImage<T>& in, CVD::BasicImage<T>& out, const TooN::Matrix<3, 3, U>& M, int flags = cv::INTER_LINEAR, int borderMode = cv::BORDER_CONSTANT, const T& borderValue = T())
	{
		cv::Mat m_mat(3, 3, CVD::Internal::opencv_type<U>::type, const_cast<U*>(&M(0, 0)));
		cv::warpPerspective(toMat(in), toMat(out), m_mat, cv::Size(out.size().x, out.size().y), flags, borderMode, borderValue);
	}

	inline void findContours(const CVD::BasicImage<std::uint8_t>& in, std::vector<std::vector<TooN::Vector<2, int>>>& contours, cv::RetrievalModes mode, cv::ContourApproximationModes method, const TooN::Vector<2, int>& offset = TooN::makeVector(0, 0))
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		cv::findContours(toMat(in), cv_contours, mode, method, { offset[0], offset[1] });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
	}

	inline void findContours(const CVD::BasicImage<std::int32_t>& in, std::vector<std::vector<TooN::Vector<2, int>>>& contours, cv::ContourApproximationModes method, const TooN::Vector<2, int>& offset = TooN::makeVector(0, 0))
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		cv::findContours(toMat(in), cv_contours, cv::RETR_FLOODFILL, method, { offset[0], offset[1] });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
	}

	inline void findContours(const CVD::BasicImage<std::uint8_t>& in, std::vector<std::vector<TooN::Vector<2, int>>>& contours, std::vector<ContourHierarchy>& hierarchy, cv::RetrievalModes mode, cv::ContourApproximationModes method, const TooN::Vector<2, int>& offset = TooN::makeVector(0, 0))
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		std::vector<cv::Vec4i> cv_hierarchy;
		cv::findContours(toMat(in), cv_contours, cv_hierarchy, mode, method, { offset[0], offset[1] });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
		CVD::OpenCV::Internal::convert_hierarchy(cv_hierarchy, hierarchy);
	}

	inline void findContours(const CVD::BasicImage<std::int32_t>& in, std::vector<std::vector<TooN::Vector<2, int>>>& contours, std::vector<ContourHierarchy>& hierarchy, cv::ContourApproximationModes method, const TooN::Vector<2, int>& offset = TooN::makeVector(0, 0))
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		std::vector<cv::Vec4i> cv_hierarchy;
		cv::findContours(toMat(in), cv_contours, cv_hierarchy, cv::RETR_FLOODFILL, method, { offset[0], offset[1] });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
		CVD::OpenCV::Internal::convert_hierarchy(cv_hierarchy, hierarchy);
	}
}
}

#endif

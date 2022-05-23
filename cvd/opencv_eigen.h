#ifndef LIBCVD_OPENCV_EIGEN_H
#define LIBCVD_OPENCV_EIGEN_H

#include "opencv.h"

#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <vector>

namespace CVD
{
namespace OpenCV
{
	namespace Internal
	{
		void convert_contours(const std::vector<std::vector<cv::Point2i>>& cv_contours, std::vector<std::vector<Eigen::Vector2i>>& contours)
		{
			for(const auto& cv_contour : cv_contours)
			{
				auto& contour = contours.emplace_back();
				contour.reserve(cv_contour.size());
				std::transform(
				    cv_contour.begin(),
				    cv_contour.end(),
				    std::back_inserter(contour),
				    [](const cv::Point2i& p) { return Eigen::Vector2i(p.x, p.y); });
			}
		}
	}

	template <typename T, typename U, int Options, int MaxRows, int MaxCols>
	void warpAffine(const CVD::BasicImage<T>& in, CVD::BasicImage<T>& out, const Eigen::Matrix<U, 2, 3, Options, MaxRows, MaxCols>& M, int flags = cv::INTER_LINEAR, int borderMode = cv::BORDER_CONSTANT, const T& borderValue = T())
	{
		cv::Mat m_mat;
		cv::eigen2cv(M, m_mat);
		cv::warpAffine(toMat(in), toMat(out), m_mat, cv::Size(out.size().x, out.size().y), flags, borderMode, borderValue);
	}

	template <typename T, typename U, int Options, int MaxRows, int MaxCols>
	void warpPerspective(const CVD::BasicImage<T>& in, CVD::BasicImage<T>& out, const Eigen::Matrix<U, 3, 3, Options, MaxRows, MaxCols>& M, int flags = cv::INTER_LINEAR, int borderMode = cv::BORDER_CONSTANT, const T& borderValue = T())
	{
		cv::Mat m_mat;
		cv::eigen2cv(M, m_mat);
		cv::warpPerspective(toMat(in), toMat(out), m_mat, cv::Size(out.size().x, out.size().y), flags, borderMode, borderValue);
	}

	inline void findContours(const CVD::BasicImage<std::uint8_t>& in, std::vector<std::vector<Eigen::Vector2i>>& contours, cv::RetrievalModes mode, cv::ContourApproximationModes method, const Eigen::Vector2i& offset = { 0, 0 })
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		cv::findContours(toMat(in), cv_contours, mode, method, { offset.x(), offset.y() });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
	}

	inline void findContours(const CVD::BasicImage<std::int32_t>& in, std::vector<std::vector<Eigen::Vector2i>>& contours, cv::ContourApproximationModes method, const Eigen::Vector2i& offset = { 0, 0 })
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		cv::findContours(toMat(in), cv_contours, cv::RETR_FLOODFILL, method, { offset.x(), offset.y() });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
	}

	inline void findContours(const CVD::BasicImage<std::uint8_t>& in, std::vector<std::vector<Eigen::Vector2i>>& contours, std::vector<ContourHierarchy>& hierarchy, cv::RetrievalModes mode, cv::ContourApproximationModes method, const Eigen::Vector2i& offset = { 0, 0 })
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		std::vector<cv::Vec4i> cv_hierarchy;
		cv::findContours(toMat(in), cv_contours, cv_hierarchy, mode, method, { offset.x(), offset.y() });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
		CVD::OpenCV::Internal::convert_hierarchy(cv_hierarchy, hierarchy);
	}

	inline void findContours(const CVD::BasicImage<std::int32_t>& in, std::vector<std::vector<Eigen::Vector2i>>& contours, std::vector<ContourHierarchy>& hierarchy, cv::ContourApproximationModes method, const Eigen::Vector2i& offset = { 0, 0 })
	{
		std::vector<std::vector<cv::Point2i>> cv_contours;
		std::vector<cv::Vec4i> cv_hierarchy;
		cv::findContours(toMat(in), cv_contours, cv_hierarchy, cv::RETR_FLOODFILL, method, { offset.x(), offset.y() });
		CVD::OpenCV::Internal::convert_contours(cv_contours, contours);
		CVD::OpenCV::Internal::convert_hierarchy(cv_hierarchy, hierarchy);
	}
}
}

#endif

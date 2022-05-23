/*
	This file is part of the CVD Library.

	Copyright (C) 2022 The Authors

	This library is free software, see LICENSE file for details
*/
#include "test_utility.h"

#include <cvd/opencv.h>
#include <cvd/opencv_eigen.h>
#include <cvd/opencv_toon.h>

#include <cstdlib>
#include <iostream>

namespace
{
using CVD::Testing::assert_equal;
using CVD::Testing::assert_image_equal;
using CVD::Testing::assert_vector_equal;
using CVD::Testing::init;

template <typename T, int Rows, int Cols, int Options>
TooN::Matrix<Rows, Cols, T> ToTooN(const Eigen::Matrix<T, Rows, Cols, Options>& m)
{
	if constexpr(Options & Eigen::RowMajor)
	{
		return TooN::Matrix<Rows, Cols, T, TooN::Reference::RowMajor>(const_cast<T*>(m.data()));
	}
	else
	{
		return TooN::Matrix<Rows, Cols, T, TooN::Reference::ColMajor>(const_cast<T*>(m.data()));
	}
}
}

int main()
{
	CVD::Image<float> in = init<float>({ { 1, 2 }, { 3, 4 } });
	CVD::Image<float> out({ 4, 4 });
	Eigen::Matrix<double, 2, 3> transformation;
	transformation << 0, -1, 2,
	    1, 0, 1;
	CVD::OpenCV::warpAffine(in, out, transformation, cv::INTER_NEAREST);
	assert_image_equal(init<float>({
	                       { 0, 0, 0, 0 },
	                       { 0, 3, 1, 0 },
	                       { 0, 4, 2, 0 },
	                       { 0, 0, 0, 0 },
	                   }),
	    out);

	CVD::Image<float> toon_out({ 4, 4 });
	double values[6] = { 0, -1, 2, 1, 0, 1 };
	TooN::Matrix<2, 3> toon_transform = ToTooN(transformation);
	CVD::OpenCV::warpAffine(in, toon_out, toon_transform, cv::INTER_NEAREST);
	assert_image_equal(init<float>({
	                       { 0, 0, 0, 0 },
	                       { 0, 3, 1, 0 },
	                       { 0, 4, 2, 0 },
	                       { 0, 0, 0, 0 },
	                   }),
	    toon_out);

	CVD::Image<std::uint8_t> mask = init<std::uint8_t>({
	    { 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 1, 1, 0, 0 },
	    { 0, 1, 1, 1, 1, 0 },
	    { 0, 1, 1, 1, 1, 0 },
	    { 0, 0, 1, 1, 0, 0 },
	    { 0, 0, 0, 0, 0, 0 },
	});
	std::vector<std::vector<Eigen::Vector2i>> contours;
	CVD::OpenCV::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	assert_equal(static_cast<size_t>(1), contours.size());
	assert_vector_equal(std::vector<Eigen::Vector2i> { { 2, 1 }, { 1, 2 }, { 1, 3 }, { 2, 4 }, { 3, 4 }, { 4, 3 }, { 4, 2 }, { 3, 1 } }, contours[0]);

	std::vector<std::vector<TooN::Vector<2, int>>> toon_contours;
	CVD::OpenCV::findContours(mask, toon_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	assert_equal(static_cast<size_t>(1), toon_contours.size());
	assert_vector_equal(std::vector<TooN::Vector<2, int>> {
	                        TooN::makeVector(2, 1),
	                        TooN::makeVector(1, 2),
	                        TooN::makeVector(1, 3),
	                        TooN::makeVector(2, 4),
	                        TooN::makeVector(3, 4),
	                        TooN::makeVector(4, 3),
	                        TooN::makeVector(4, 2),
	                        TooN::makeVector(3, 1) },
	    toon_contours[0]);
}

#ifndef LIBCVD_OPENCV_H
#define LIBCVD_OPENCV_H

#include <cvd/image.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace CVD {

  template <class T>
  cv::Mat to_opencv(const CVD::BasicImage<T> &img) {
    return cv::Mat(img.size().x, img.size().y, cv::DataDepth<T>::value, (void*)img.data());
  }

  template <class T>
  void equalizeHist(const CVD::BasicImage<T> &in, CVD::BasicImage<T> &out) {
    cv::Mat in_(to_opencv(in));
    cv::Mat out_(to_opencv(out));
    equalizeHist(in_, out_);
  }

}
#endif

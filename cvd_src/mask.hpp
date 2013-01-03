#ifndef GRANTA_MASK_HPP
#define GRANTA_MASK_HPP

#include <iostream>
#include <fstream>
#include <cvd/image.h>

namespace Granta {

  using namespace CVD;

  template <class T>
  void read_mask(std::istream &in, Image <T> &out) {
    int width;
    int height;
    in >> width;
    in >> height;
    if (out.size().x != width  || out.size().y != height) {
      out.resize(ImageRef(height, width));
    }
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        int i;
        in >> i;
        out[x][y] = i;
      }
    }
  }

  template <class T>
  void read_mask(const std::string &filename, Image <T> &out) {
    std::ifstream in(filename.c_str());
    read_mask(in, out);
  }


  template <class T>
  ImageRef compute_centroid(const BasicImage <T> &mask, const double threshold) {
    int x_sum(0);
    int y_sum(0);
    int count(0);
    for (int y = 0; y < mask.size().y; y++) {
      for (int x = 0; x < mask.size().x; x++) {
        if (mask[y][x] > threshold) {
          x_sum += x;
          y_sum += y;
          count++;
        }
      }
    }
    return ImageRef(x_sum / count, y_sum / count);
  }
}

#endif

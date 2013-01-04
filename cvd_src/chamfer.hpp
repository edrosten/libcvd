#ifndef GRANTA_CHAMFER_HPP
#define GRANTA_CHAMFER_HPP

#include <cmath>
#include <vector>
#include <cvd/image.h>
#include <cvd/image_io.h>
#include <cvd/vector_image_ref.h>
#include <TooN/TooN.h>
#include <iostream>

#include "edge.hpp"

namespace Granta {

  using namespace std;
  using namespace CVD;

  /*
    Represents a contour fragment for the object model.
   */
  class ContourTemplate {
  private:
    std::vector<TooN::Vector<2> > edgels;
    std::vector<float> directions;

  public:
    ContourTemplate() { }

    ContourTemplate(std::vector<TooN::Vector<2> > &edgels, std::vector<float> &directions) : edgels(edgels), directions(directions) {
      GRANTA_ASSERT(edgels.size() == directions.size());
    }

    void add_edgel(const TooN::Vector<2> &edgel, const double dir = 0) {
      edgels.push_back(edgel);
      directions.push_back(dir);
    }

    size_t size() const {
      GRANTA_ASSERT(edgels.size() == directions.size());
      return edgels.size();
    }

    TooN::Vector<2> &get_edgel(const size_t i) {
      return edgels[i];
    }

    const TooN::Vector<2> &get_edgel(const size_t i) const {
      return edgels[i];
    }

    float &get_direction(const size_t i) {
      return directions[i];
    }

    const float &get_direction(const size_t i) const {
      return directions[i];
    }

    /*
      Computes all the edgels of all the contours in a masked image
      using a Canny edge detector. The edgels are rescaled [0,1] but
      the aspect ratio of the bounding box that encloses all edgels is
      preserved. Finally, all edgels are shifted so that their centroid
      lies at (0,0), making the final scale [-1,1]
     */
    template <class T>
    void compute_whole(const Image<T> &image, const Image<byte> &mask) {
      Image <T> mag(image.size());
      const double pi = std::atan(1.0)*4;
      canny2(image, mag);
      img_save(mag, "Test-compute-canny.txt");
      img_save(mask, "Test-compute-mask.png");
      int max_x(0);
      int max_y(0);
      int min_x(image.size().x-1);
      int min_y(image.size().y-1);
      int sum_x(0);
      int sum_y(0);
      int n(0);
      for (int y = 1; y < image.size().y-1; y++) {
        for (int x = 1; x < image.size().x-1; x++) {
          n++;
          if (mask[y][x] && mag[y][x] > 0) {
            const double ygrad = ((image[y-1][x-1] + 2 * image[y-1][x] + image[y-1][x+1]) -
                                  (image[y+1][x-1] + 2 * image[y+1][x] + image[y+1][x+1]));
            const double xgrad = ((image[y-1][x-1] + 2 * image[y][x-1] + image[y+1][x-1]) -
                                  (image[y-1][x+1] + 2 * image[y][x+1] + image[y+1][x+1]));
            const double dir = std::fmod(std::atan2(xgrad, ygrad)+2*pi, pi);
            //cout << xgrad << " " << ygrad << " " << dir;
            TooN::Vector<2> v = TooN::makeVector((double)x, (double)y);
            edgels.push_back(v);
            directions.push_back(dir);
            if (x > max_x) {
              max_x = x;
            }
            if (y > max_y) {
              max_y = y;
            }
            if (x < min_x) {
              min_x = x;
            }
            if (y < min_y) {
              min_y = y;
            }
          }
        }
      }
      // Rescale to [-1,1] centered at centroid.
      const double range_x = max_x - min_x;
      const double range_y = max_y - min_y;
      const double range = (range_x > range_y) ? range_x : range_y; // Preserve the aspect ratio.
      const size_t npts(edgels.size());
      //#if 0
      TooN::Vector<2> minv = TooN::makeVector(min_x, min_y);
      TooN::Vector<2> centroid = TooN::makeVector(0, 0);
      centroid = centroid / npts;
      centroid = (centroid - minv) / range;
      for (size_t i = 0; i < npts; i++) {
        edgels[i] = 2.0 * (edgels[i] - minv) / range;
        centroid += edgels[i];
      }
      centroid /= npts;
      for (size_t i = 0; i < npts; i++) {
        edgels[i] -= centroid;
      }
      cout << "Centroid: " << centroid;
      //#endif
    }

    /*
      Computes the chamfer distance of a template edge/contour given a binary feature. A pre-computed
      distance transform must be provided.

      @param DTE         The Euclidean distance transform of the feature map.
      @param chamfer_out An image to store the chamfer distances for each feature pixel.
      @param prop        The proportion of pixels to uniformly sample from.
      @param scale       The scaling factor of the template (template edgels are [0,1] normalized).
     */
    template <class T>
    void compute_chamfer_distance(const Image <T> &DTE, Image <T> &chamfer_out,
                                  const double prop, const double scale) {
      GRANTA_ASSERT(edgels.size() > 0);
      const double nedgels = static_cast<double>(edgels.size());
      const double nsampled_edgels = (nedgels * prop);
      const int step_size = static_cast<int>(nedgels/(nsampled_edgels));
      for (int y = 0; y < DTE.size().y; y++) {
        for (int x = 0; x < DTE.size().x; x++) {
          TooN::Vector<2> v(vec(ImageRef(x, y)));
          double total = 0.0;
          for (int k = 0; k < edgels.size(); y += step_size) {
            TooN::Vector<2> scaled_edgel(edgels[k] * scale);
            ImageRef vt_plus_v = ir_rounded(scaled_edgel + v);
            total += DTE[vt_plus_v];
          }
          total /= nsampled_edgels;
          chamfer_out[y][x] = total;
        }
      }
    }
  };

  ostream &operator <<(ostream &out, const ContourTemplate &contour) {
    out << contour.size() << endl;
    for (size_t i = 0; i < contour.size(); i++) {
      out << contour.get_edgel(i) << " ";
      out << contour.get_direction(i) << " ";
    }
    out << endl;
    return out;
  }

  istream &operator >>(istream &in, ContourTemplate &contour) {
    int num_contours(0);
    in >> num_contours;
    for (size_t i = 0; i < contour.size(); i++) {
      TooN::Vector<2> v;
      double dir;
      in >> v;
      in >> dir;
      contour.add_edgel(v, dir);
    }
    return in;
  }
}

#endif

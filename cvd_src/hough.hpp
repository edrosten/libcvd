#ifndef GRANTA_HOUGH_HPP
#define GRANTA_HOUGH_HPP

#include <cvd/image.h>
#include <cvd/convolution.h>
#include <cmath>
#include <queue>
#include "defs.hpp"
#include "debug.hpp"
#include "mask.hpp"
#include <cvd/timer.h>
#include <cvd/vector_image_ref.h>
#include <TooN/TooN.h>

namespace Granta {

  struct HoughResult {
    double scale;
    double rotation;
    double strength;
  };

  template <class T>
  std::pair<T, ImageRef> largest_local_maxima(const BasicImage<T> &img) {
    T best_val(0);
    ImageRef best_ref(0,0);
    T best_best(0);
    for(int y=1; y < img.size().y-1; y++) {
      for(int x=1; x < img.size().x-1; x++) {
        const T &val(img[y][x]);
        if (val > best_best) {
          best_best = val;
        }
        if (val > best_val
            && val > img[y-1][x-1]
            && val > img[y-1][x]
            && val > img[y-1][x+1]
            && val > img[y][x-1]
            && val > img[y][x+1]
            && val > img[y+1][x-1]
            && val > img[y+1][x]
            && val > img[y+1][x+1]) {
          best_val = val;
          best_ref.x = x;
          best_ref.y = y;
        }
      }
    }
    //cout << best_best << endl;
    return make_pair(best_val, best_ref);
  }

#if 0
  struct SpatialVote {
    SpatialVote(double x, double y) : translation(x, y) {}
    Vector<2> translation;
  };

  template <class VoteType>
  class GHT1D {

  public:
    GHT1D(int num_bins,
          double feature_min,
          double feature_max) : votes(num_bins),
                                feature_min(feature_min),
                                feature_max(feature_max),
                                feature_range(feature_max-feature_min) {
      if (feature_min > feature_max) {
        throw std::string("Error: feature_min < feature_max is required");
      }
    }

    inline int get_bin_number(double feature_value) const {
      const double normalized_zero_one = ((feature_value - feature_min) / feature_range);
      return std::min(bins.size(), std::max(0, normalized_zero_one * (int)bins.size()));
    }

    inline std::vector<VoteType> &get_bin(double feature_value) {
      return bins[get_bin_number(feature_value)];
    }

    inline const std::vector<VoteType> &get_bin(double feature_value) const {
      return bins[get_bin_number(feature_value)];
    }

    inline void add_to_bin(double feature_value, const HoughVote &v) {
      bins[feature_value].push_back(v);
    };

    template <class TrainFeatureFunctor>
    void train(const TrainFeatureFunctor &fun) {
      const ImageRef size = fun.size();
      for(int y=1; y < size.y-1; y++) {
        for(int x=1; x < size.x-1; x++) {
          const Vector<2> v(x, y);
          if (fun.good(v)) {
            const double feature = fun.get_feature(v);
            add_to_bin(feature, fun.get_vote(v));
          }
        }
      }
    }

    template <class T> 
    void test() {

    }

  private:
    std::vector <std::vector<HoughVote> > hough_space;
    double feature_min;
    double feature_max;
    double feature_range;
    std::vector <double> half_gaussian;
  };
#endif

  class GHT {
  public:
    GHT(int num_bins = 9) {
      resize(num_bins);
    }
    
    inline std::vector<ImageRef> &get_bin_by_angle(double angle_in_degrees) {
      return bins[(angle_in_degrees / 360) * bins.size()];
    }

    inline std::vector<ImageRef> &get_bin(size_t i) {
      return bins[i];
    }

    inline const std::vector<ImageRef> &get_bin(size_t i) const {
      return bins[i];
    }

    inline const double get_bin_lower_angle(size_t i) const {
      return get_bin_width() * i;
    }

    inline const double get_bin_upper_angle(size_t i) const {
      return get_bin_width() * (i + 1);
    }

    inline const double get_bin_center_angle(size_t i) const {
      return get_bin_width() * ((double)i + 0.5);
    }

    inline const double get_bin_width() const {
      return 360.0 / (double)bins.size();
    }
    
    template <class T, class Q>
    std::pair<double, double> get_chamfer_distance(CVD::Image<T> &dt,
                                CVD::Image<ImageRef> &adt,
                                CVD::Image<Q> &grad_dir,
                                CVD::Image<bool> &unvisited,
                                ImageRef center, const double scale_factor = 1.0,
                                const double min_chamfer = 15.00) {
      const double pi = std::atan(1.0)*4;
      size_t num_edgels(0);
      double chamfer = 0.0;
      double total_orient_cost = 0.0;
      int total_visited = 0;
      const double bin_width = get_bin_width();
      ImageRef null_ir(0, 0);
      for (size_t i = 0; i < bins.size(); i++) {
        num_edgels += bins[i].size();
        const double bin_center = fmod(get_bin_center_angle(i), 360.0);
        const double bin_center2 = fmod(get_bin_center_angle(i) + 180.0, 360.0);
        const double theta_ij(1.0 / bins[i].size());
        size_t bin_size(bins[i].size());
        for (size_t j = 0; j < bin_size; j++) {
          TooN::Vector<2> jump(-vec(bins[i][j]));          
          jump *= scale_factor;
          ImageRef vote_pt(ir_rounded(jump) + center);
          if (dt.in_image(vote_pt)) {
            if (adt[vote_pt] != null_ir) {
              const ImageRef &q(adt[vote_pt]);
              const double grad_dir_pixel = fmod(grad_dir[q]/pi * 180.0 + 180.0, 360.0);
              //cout << "[" << grad_dir_pixel << "," << bin_center << "]";
              const float cc(std::min((float)min_chamfer, dt[vote_pt]));
              const double orient_dist
                = std::min(std::abs(bin_center - grad_dir_pixel),
                           std::abs(bin_center2 - grad_dir_pixel));
              //std::cerr << vote_pt << cc;
              //if (unvisited[q]) {
                chamfer += cc;
                total_orient_cost += orient_dist;
                total_visited++;
                //}
                //unvisited[q] = false;
            }
          }
        }
      }
      //cout << "orient cost: " << total_orient_cost;
      chamfer = (chamfer / (double)total_visited); // + (total_orient_cost / (double) num_edgels);
      total_orient_cost = total_orient_cost / (double)total_visited;
      //cout << "chamfer: " << chamfer << " ";
      return make_pair(chamfer, total_orient_cost);
    }

    void random_sample(double prop) {
      for (size_t i = 0; i < bins.size(); i++) {
        std::vector<ImageRef> &bin(bins[i]);
        std::vector<size_t> s(bin.size(), 0);
        for (size_t j = 0; j < bin.size(); j++) {
          s[j] = j;
        }
        std::random_shuffle(s.begin(), s.end());
        size_t newsize = std::min(bin.size(), std::max((size_t)1, (size_t)(prop * (double)bin.size())));
        std::vector<ImageRef> newbin(newsize);
        for (size_t j = 0; j < newsize; j++) {
          newbin[j] = bin[s[j]];
        }
        bins[i] = newbin;
      }
    }

    void resize(size_t new_size) {
      bins.resize(0);
      bins.resize(new_size);
    }
    
    inline size_t size() const {
      return bins.size();
    }

    void train(const BasicImage<float> &in_img,
               const double canny_sigma,
               const double canny_low,
               const double canny_high) {
      Image<float> canny_img(in_img.size());
      Image<float> grad_dir(in_img.size());
      Image<byte> mask(in_img.size());
      canny2(in_img, canny_img, canny_sigma, canny_low, canny_high);
      std::fill(mask.begin(), mask.end(), 1);
      ImageRef centroid(compute_centroid(mask, canny_low));
      compute_gradient_direction_at(in_img, canny_img, canny_low, grad_dir);
      compute(grad_dir, canny_img, mask, centroid, canny_low);
    }

    template <class T>
    void compute(const BasicImage <T> &grad_dir, const BasicImage <T> &mag, const BasicImage <byte> &mask,
                 const ImageRef &centroid, const double threshold = 0) {
      const double pi = std::atan(1.0)*4;
      for(int y=1; y < grad_dir.size().y-1; y++) {
        for(int x=1; x < grad_dir.size().x-1; x++) {
          if (mask[y][x] && mag[y][x] > threshold) {
            const double dir = fmod(grad_dir[y][x]/pi * 180.0 + 180.0, 360.0);
            //cout << dir << " ";
            std::vector <ImageRef> &bin(get_bin_by_angle(dir));
            bin.push_back(ImageRef(centroid.x-x, centroid.y-y));
          }
        }
      }
    }

    template <class T, class Q>
    double vote(const BasicImage <T> &grad_dir, const BasicImage <T> &mag, BasicImage <Q> &vote_map,
                const double threshold = 0, const double scale_factor = 1.0, double rotation = 0.0) {
      const double pi = std::atan(1.0)*4;
      double max_vote = 0.0;
      for(int y=1; y < grad_dir.size().y-1; y++) {
        for(int x=1; x < grad_dir.size().x-1; x++) {
          if (mag[y][x] > threshold) {
            const double dir = fmod(grad_dir[y][x]/pi * 180.0 + 180.0 + rotation, 360.0);
            //cout << dir << " ";
            //const double dir = fmod((grad_dir[y][x]/pi) * 180.0 + 180.0, 180.0);
            const ImageRef pt(x, y);
            //const double dir = grad_dir[y][x];
            std::vector<ImageRef> &bin(get_bin_by_angle(dir));
            const double theta_ij(1.0 / bin.size());
            for (size_t i = 0; i < bin.size(); i++) {
              TooN::Vector<2> jump(vec(bin[i]));
              jump *= scale_factor;
              ImageRef vote_pt(ir_rounded(jump) + pt);
              //cout << vote_pt;
              if (vote_map.in_image(vote_pt)) {
                //vote_map[vote_pt] += mag[pt];//log(mag[pt]+1.0);
                vote_map[vote_pt] += theta_ij;
                if (vote_map[vote_pt] > max_vote) {
                  max_vote = vote_map[vote_pt];
                }
              }
            }
          }
        }
      }
      return max_vote;
    }

    template <class T, class Q>
    void inverse_vote(const ImageRef &winning_pt,
                      const BasicImage <T> &grad_dir,
                      const BasicImage <T> &mag,
                      BasicImage <Q> &evidence_map,
                      const double threshold = 0,
                      const double scale_factor = 1.0,
                      const double rotation = 0.0) {
      const double pi = std::atan(1.0)*4;
      for(int y=1; y < grad_dir.size().y-1; y++) {
        for(int x=1; x < grad_dir.size().x-1; x++) {
          if (mag[y][x] > threshold) {
            const double dir = fmod(grad_dir[y][x]/pi * 180.0 + 180.0 + rotation, 360.0);
            //cout << dir << " ";
            //const double dir = fmod((grad_dir[y][x]/pi) * 180.0 + 180.0, 180.0);
            const ImageRef pt(x, y);
            //const double dir = grad_dir[y][x];
            std::vector<ImageRef> &bin(get_bin_by_angle(dir));
            const double theta_ij(1.0 / bin.size());
            for (size_t i = 0; i < bin.size(); i++) {
              TooN::Vector<2> jump(vec(bin[i]));
              if (scale_factor != 1.0) {
                jump *= scale_factor;
              }
              ImageRef vote_pt(ir_rounded(jump) + pt);
              int dist = (vote_pt.x - winning_pt.x) * (vote_pt.x - winning_pt.x)
                + (vote_pt.y - winning_pt.y) * (vote_pt.y - winning_pt.y);
              //cout << vote_pt;
              if (dist <= 49) {
                if (evidence_map.in_image(pt)) {
                  evidence_map[pt] += theta_ij; //mag[pt];//log(mag[pt]+1.0);
                }
              }
            }
          }
        }
      }
    }


    template <class T, class Q>
    HoughResult vote_scale_search(const BasicImage <T> &grad_dir, const BasicImage <T> &mag, BasicImage <Q> &vote_map,
                             const double threshold = 0, const double smooth_sigma = 0, const double rotation = 0.0) {
      HoughResult best;
      best.scale = 1.0;
      best.strength = 0.0;
      best.rotation = rotation;
      for(double scale = 0.25; scale <= 1.50; scale += 0.05) {
        std::fill(vote_map.begin(), vote_map.end(), 0.0);
        vote(grad_dir, mag, vote_map, threshold, scale, rotation);
        if (smooth_sigma > 0.0) {
          Image <Q> smooth_map(vote_map.size());
          convolveGaussian(vote_map, smooth_map, smooth_sigma);
          std::copy(smooth_map.begin(), smooth_map.end(), vote_map.begin());
        }
        double max_vote = *std::max_element(vote_map.begin(), vote_map.end());
        if (max_vote >= best.strength) {
          best.scale = scale;
          best.strength = max_vote;
        }
      }
      std::fill(vote_map.begin(), vote_map.end(), 0.0);
      vote(grad_dir, mag, vote_map, threshold, best.scale, rotation);
      if (smooth_sigma > 0.0) {
        Image <float> smooth_map(vote_map.size());
        convolveGaussian(vote_map, smooth_map, smooth_sigma);
        std::copy(smooth_map.begin(), smooth_map.end(), vote_map.begin());
      }
      return best;
    }

    template <class T, class Q>
    HoughResult vote_scale_rotation_search(const BasicImage <T> &grad_dir, const BasicImage <T> &mag, BasicImage <Q> &vote_map,
                                      const double threshold = 0, const double smooth_sigma = 0) {
      HoughResult best;
      best.scale = 1.0;
      best.rotation = 0.0;
      best.strength = 0.0;
      for(double rotation = -10.0; rotation <= 10.0; rotation += 5.0) {
        HoughResult result = vote_scale_search(grad_dir, mag, vote_map, threshold, smooth_sigma, rotation);
        if (result.strength > best.strength) {
          best = result;
        }
      }
      std::fill(vote_map.begin(), vote_map.end(), 0);
      vote(grad_dir, mag, vote_map, threshold, best.scale, best.rotation);
      return best;
    }

#if 0
    template <class T, class Q>
    void vote_orientation_search(const BasicImage <T> &grad_dir, const BasicImage <T> &mag, BasicImage <Q> &vote_map,
                                 const double threshold = 0) {
      double best_angle = 0.0;
      double best_vote = 0.0;
      for(double angle = 0.00; angle <= 3.00; angle += 0.05) {
        std::fill(vote_map.begin(), vote_map.end(), 0.0);
        vote(grad_dir, mag, vote_map, threshold, scale);
        double max_vote = *std::max_element(vote_map.begin(), vote_map.end());
        if (max_vote > best_vote) {
          best_scale = scale;
          best_vote = max_vote;
        }
      }
      vote(grad_dir, mag, vote_map, threshold, best_scale);
    }
#endif

    void compute_bounding_box() {
      ImageRef umin = ImageRef(10000, 10000);
      ImageRef umax = ImageRef(0, 0);
      for (size_t i = 0; i < bins.size(); i++) {
        const std::vector<ImageRef> &bin(bins[i]);
        for (size_t j = 0; j < bin.size(); j++) {
          if (bin[j].x < umin.x) {
            umin.x = bin[j].x;
          }
          if (bin[j].y < umin.y) {
            umin.y = bin[j].y;
          }
          if (bin[j].x > umax.x) {
            umax.x = bin[j].x;
          }
          if (bin[j].y > umax.y) {
            umax.y = bin[j].y;
          }
        }
      }
      if (umin.x == 10000) {
        umin.x = 0;
      }
      if (umin.y == 10000) {
        umin.y = 0;
      }
      ul = umin;
      lr = umax;
    }
    
    ImageRef get_umin(const ImageRef &center, double scale) {
      return ir_rounded(vec(center - ul * scale));
    }

    ImageRef get_umax(const ImageRef &center, double scale) {
      return ir_rounded(vec(center - lr * scale));
    }

  private:
    ImageRef ul, lr;
    std::vector<std::vector<ImageRef> > bins;
  };

  std::ostream &operator<<(std::ostream &out, const GHT &ght) {
    out << ght.size() << endl;
    for (size_t i = 0; i < ght.size(); i++) {
      size_t nvals(ght.get_bin(i).size());
      out << nvals;
      for (size_t j = 0; j < nvals; j++) {
        out << ght.get_bin(i)[j];
      }
      out << endl;
    }
    return out;
  }

  std::istream &operator>>(std::istream &in, GHT &ght) {
    int nbins;
    in >> nbins;
    ght.resize(nbins);
    for (size_t i = 0; i < ght.size(); i++) {
      size_t nvals;
      in >> nvals;
      for (size_t j = 0; j < nvals; j++) {
        ImageRef ref;
        in >> ref;
        ght.get_bin(i).push_back(ref);
      }
    }
    return in;
  }
}
#endif

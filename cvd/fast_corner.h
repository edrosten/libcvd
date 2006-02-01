#ifndef CVD_FAST_CORNER_H
#define CVD_FAST_CORNER_H

#include <vector>
#include <utility>

#include <cvd/byte.h>
#include <cvd/image.h>
#include <cvd/cpu_hacks.h>

namespace CVD
{
	/** Perform a FAST feature detect on an image. The FAST feature detector
	    is described in  Fusing Points and Lines for High Performance Tracking,
	    E. Rosten and T. Drummond. If you use this in published work, please 
		cite paper:
	   
\verbatim
@inproceedings{rosten_2005_tracking,
	title       =    "Fusing points and lines for high performance tracking.",
	author      =    "Edward Rosten and Tom Drummond",
	year        =    "2005",     
	month       =    "October",     
	pages       =    "1508--1511",     
	volume      =    "2",     
	booktitle   =    "IEEE International Conference on Computer Vision",     
	notes       =    "Oral presentation" 
}
\endverbatim

		This performs 12 point FAST corner detection using the algorithm as originally described.
	   
	    @param im 		The input image
	    @param corners	The resulting container of corner locations
	    @param barrier	Corner detection threshold
	    @ingroup	gVision
	**/
	void fast_corner_detect(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);
  
  /** Perform non-maximal suppression on a set of FAST features. This cleans up
  areas where there are multiple adjacent features, using a computed score
  function to leave only the 'best' features. This function is typically called
  immediately after a call to fast_corner_detect() (or one of its variants).
  @param im The image used to generate the FAST features
  @param corners The FAST features previously detected (e.g. by calling
    fast_corner_detect())
  @param  barrier The barrier used to calculate the score, which should be the
    same as that passed to fast_corner_detect()
  @param non_max_corners Vector to be filled with the new list of
    non-maximally-suppressed corners
  @ingroup  gVision
  */
	void fast_nonmax(
   const BasicImage<byte>& im, const std::vector<ImageRef>& corners,
   int barrier, std::vector<ImageRef>& nonmax_corners);

  /** Perform non-maximal suppression on a set of FAST features, also returning
  the score for each remaining corner. This function cleans up areas where
  there are multiple adjacent features, using a computed score function to leave
  only the 'best' features. This function is typically called immediately after
  a call to fast_corner_detect() (or one of its variants).
  @param im The image used to generate the FAST features
  @param corners The FAST features previously detected (e.g. by calling
    fast_corner_detect())
  @param  barrier The barrier used to calculate the score, which should be the
    same as that passed to fast_corner_detect()
  @param non_max_corners Vector to be filled with the new list of
    non-maximally-suppressed corners, and their scores.
  <code>non_maxcorners[i].first</code> gives the location and 
  <code>non_maxcorners[i].second</code> gives the score (higher is better).
  @ingroup  gVision
  */
	void fast_nonmax_with_scores(
   const BasicImage<byte>& im, const std::vector<ImageRef>& corners,
   int barrier, std::vector<std::pair<ImageRef,int> >& nonmax_corners);
	
	int corner_score(const BasicImage<byte>& im, ImageRef c, const int *pointer_dir, int barrier);
	
	/// Perform tree based 9 point FAST feature detection. This is the fastest detector and
	/// preliminary results suggest that it is also the best. 
	/// If you use this, please cite the paper given in @ref fast_corner_detect
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_9(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);

	/// Perform tree based 10 point FAST feature detection
	/// If you use this, please cite the paper given in @ref fast_corner_detect
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_10(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);

	/// Perform tree based 11 point FAST feature detection
	/// If you use this, please cite the paper given in @ref fast_corner_detect
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_11(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);

	/// Perform tree based 12 point FAST feature detection
	/// If you use this, please cite the paper given in @ref fast_corner_detect
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_12(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);

	
	/// The 16 offsets from the centre pixel used in FAST feature detection.
	///
	/// @ingroup gVision
	extern const ImageRef fast_pixel_ring[16];
}

#endif

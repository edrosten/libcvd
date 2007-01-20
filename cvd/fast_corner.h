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

		You probably don't want to use this feature detector. The 9 point FAST detector 
		is better. See @ref fast_corner_detect_9
	   
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
	@param corners The FAST features previously detected (e.g. by calling fast_corner_detect())
	@param  barrier The barrier used to calculate the score, which should be the same as that passed to fast_corner_detect()
	@param nonmax_corners Vector to be filled with the new list of non-maximally-suppressed corners
	@ingroup  gVision
	*/
	void fast_nonmax( const BasicImage<byte>& im, const std::vector<ImageRef>& corners, int barrier, std::vector<ImageRef>& nonmax_corners);

	/** Perform non-maximal suppression on a set of FAST features, also returning
	the score for each remaining corner. This function cleans up areas where
	there are multiple adjacent features, using a computed score function to leave
	only the 'best' features. This function is typically called immediately after
	a call to fast_corner_detect() (or one of its variants).
	
	@param im The image used to generate the FAST features
	@param corners The FAST features previously detected (e.g. by calling fast_corner_detect())
	@param barrier The barrier used to calculate the score, which should be the same as that passed to fast_corner_detect()
	@param nonmax_corners Vector to be filled with the new list of
	       non-maximally-suppressed corners, and their scores.
     	   <code>non_maxcorners[i].first</code> gives the location and 
	       <code>non_maxcorners[i].second</code> gives the score (higher is better).
	
	@ingroup  gVision
	*/
	void fast_nonmax_with_scores( const BasicImage<byte>& im, const std::vector<ImageRef>& corners, int barrier, std::vector<std::pair<ImageRef,int> >& nonmax_corners);

	/** Return the score for the corner as used by fast_nonmax
		Note that this is not the same score as used in @ref fast_score .

		NB: This function is DEPRECATED!

		@param im The image.
		@param c The location in the image to be scored
		@param pointer_dir Integer offsets for each of the 16 pixels in the ring from the centre.
		@param barrier Corner detection threshold
	*/
	int corner_score(const BasicImage<byte>& im, ImageRef c, const int *pointer_dir, int barrier);

	/// Perform tree based 7 point FAST feature detection. This is more like an edge detector.
	/// If you use this, please cite the paper given in @ref fast_corner_detect_9
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_7(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);

	/// Perform tree based 8 point FAST feature detection. This is more like an edge detector.
	/// If you use this, please cite the paper given in @ref fast_corner_detect_9
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_8(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);
	
	
	/** Perform tree based 9 point FAST feature detection as described in:
	    Machine Learning for High Speed Corner Detection, E. Rosten and T. Drummond.
		Results show that this is both the fastest and the best of the detectors.
		If you use this in published work, please cite:
	
\verbatim
@inproceedings{rosten2006machine,
	title       =    "Machine Learning for High Speed Corner Detection",
	author      =    "Edward Rosten and Tom Drummond",
	year        =    "2006",     
	month       =    "May",     
	booktitle   =    "9th European Conference on Computer Vision",
	notes       =    "To appear"
}
\endverbatim

	    @param im 		The input image
	    @param corners	The resulting container of corner locations
	    @param barrier	Corner detection threshold
	    @ingroup	gVision
	**/
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
	/// If you use this, please cite the paper given in @ref fast_corner_detect_9
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_11(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);

	/// Perform tree based 12 point FAST feature detection
	/// If you use this, please cite the paper given in @ref fast_corner_detect_9
	///
	/// @param im 		The input image
	/// @param corners	The resulting container of corner locations
	/// @param barrier	Corner detection threshold
	/// @ingroup	gVision
	void fast_corner_detect_12(const BasicImage<byte>& im, std::vector<ImageRef>& corners, int barrier);


#if (CVD_HAVE_EMMINTRIN && CVD_HAVE_SSE2)
	void faster_corner_detect_12(const BasicImage<byte>& I, std::vector<ImageRef>& corners, int barrier);
#endif


	/// Score corners detected by FAST as the maximum barrier at which the corner is still a corner.
	///	
	///	@param	im		The input image
	///	@param corners	Detected corners
	///	@param ret		The returned corner scores
	///	@param barrier  Do not check for scores below this (can be negative)
	/// @param Num		Score for Num-point corner detection
	/// @ingroup		gVision
	template<int Num> void fast_score(const BasicImage<byte> im, const std::vector<ImageRef> corners, std::vector<int>& ret, int barrier);


	/// The 16 offsets from the centre pixel used in FAST feature detection.
	///
	/// @ingroup gVision
	extern const ImageRef fast_pixel_ring[16];
}

#endif

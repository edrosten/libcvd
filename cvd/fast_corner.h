#ifndef CVD_FAST_CORNER_H
#define CVD_FAST_CORNER_H

#include <vector>
#include <utility>

#include <cvd/byte.h>
#include <cvd/image.h>
#include <cvd/cpu_hacks.h>

namespace CVD
{
	/** Perform a FAST feature setect on an image. The FAST feature detector
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
	void fast_nonmax(const BasicImage<byte>& im, const std::vector<ImageRef>& corners, int barrier, std::vector<ImageRef>& nonmax_corners);
	void fast_nonmax_with_scores(const BasicImage<byte>& im, const std::vector<ImageRef>& corners, int barrier, std::vector<std::pair<ImageRef,int> >& nonmax_corners);
	
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

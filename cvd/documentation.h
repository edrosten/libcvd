///////////////////////////////////////////////////////
// General Doxygen documentation
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// The main title page
/**
@mainpage

\section Information
This C++ library is designed to be easy to use and portable for fast video saving, loading and display.
It supports OpenGL and X Windows.
It is released under the GPL License.

\section Features
\subsection Language

    - Modern C++ design
    - Extensive use of exceptions
    - OpenGL wrappers 

\subsection Imaging

    - Type-safe easy to use images
    - Loading and saving from a variety of types: jpg, pnm, tiff(soon!!)
    - Image grabbing from video sources:
          - V4L2 devices
          - Firewire cameras
          - SGI O2
          - avi and mpegs files (all codecs supported by ffmpeg)
          - list of images
          - raw image streams(soon!!!) 
    - Flexible loading and saving of images
    - Colorspace conversions
    - Various image processing tools
    - %Camera calibration support: Linear, Cubic and Quintic 

\subsection Mathematical

    - Lie Group(S03, SE3) algebra
    - Iterative and reweighted least-squares 

\subsection Portability

    - Linux
    - SGI O2
    - Mac OS X
    - FreeBSD
    - Cygwin(painful)

Not all features are support on all operating systems (ie. video grabbing). 

 
*/

///////////////////////////////////////////////////////
// Modules classifying classes and functions

/// @defgroup gImage Image storage and manipulation
/// Basic image functionality. The CVD image classes provide fast and
/// flexible access to images.

/// @defgroup gImageIO Image loading and saving, and format conversion
/// Functions to support saving and loading of BasicImage and Image 
/// to and from streams. Supports a few common file formats (autodetecting on loading).
/// Also functions for perfoming type conversion as necessary.

/// @defgroup gVideo Video devices and video files
/// Classes and functions to manage video streams and present them as images.

/// @defgroup gVideoBuffer Video buffers
/// @ingroup gVideo
/// All classes and functions relating to video buffers (as opposed to video frames)

/// @defgroup gVideoFrame Video frames
/// @ingroup gVideo
/// All classes and functions relating to video frames (as opposed to video buffers)

/// @defgroup gException Exceptions 
/// Exceptions generated and thrown by CVD classes and functions

/// @defgroup gGraphics Computer graphics
/// Classes and functions to support miscellaneous pixel operations

/// @defgroup gVision Computer Vision
/// Functions and classes to support common computer vision concepts and operations

/// @defgroup gGL GL helper functions and classes.
/// Overloaded versions of GL functions to use CVD classes and datatypes, and other
/// helpful GL classes and functions.

/// @defgroup gMaths Mathematical operations
/// Useful mathematical classes and functions

/// @defgroup gLinAlg Linear Algebra
/// Classes and functions for common Linear Algebra concepts and operations

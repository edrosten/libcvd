/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
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
It is released under the LGPL License.

\section Features
\subsection Language

    - Modern C++ design
	- Optimized assembly code
    - Extensive use of exceptions
    - OpenGL wrappers 

\subsection Imaging

    - Type-safe easy to use images
    - Flexible loading and saving from a variety of types:
		- Native
			- PNM   
			- BMP
			- ASCII text
			- FITS (reading only)
			- PS   (saving only)
			- EPS  (saving only)
		- External libraries required
			- JPEG
			- TIFF 
			- PNG
		- 1/8/16/32 bit integer and 32/64 bit floating point images in greyscale, RGB and RGBA.
		- Optimum bit depth and colour depth selected automatically
    - Image grabbing from video sources:
		  - Linux
		  		- Video for Linux devices
          		- Video for Linux 2 devices
         	 	- Firewire cameras
                - Firewire over USB cameras produced by PointGrey - see http://www.ptgrey.com/products/fireflymv/index.asp \n
                  Use DVBuffer3 for that and make sure to set an image resolution
		  - IRIX
          		- DMedia video capture
		  - OSX
		  		- QuickTime video capture
		  - All UNIX platforms
		        - Live capture from HTTP server push JPEG cameras.
		  - All platforms
          		- AVI and MPEG file (all codecs supported by ffmpeg)
          		- list of images
				- Server push multipart JPEG streams.
		  - Convenient run-time selection using a URL-like syntax
    - Colorspace conversions on images and video streams
    - Various image processing tools
	      - FAST corner detection
		  - Harris/Shi-Tomasi corner detection
		  - Connect components analysis
		  - Image interpolation and resampling
		  - Convolutions
		  - Drawing in to images
		  - Flipping, pasting, etc
		  - Interpolation, warping and resampling
		  - Integral images
    - %Camera calibration support: Linear, Cubic, Quintic and Harris
		  - Program to calibrate cameras from video

\subsection Mathematical

    - Lie Group(S03, SE3) algebra
    - Iterative and reweighted least-squares 
	- Random numbers

\section Portability
  LibCVD will compile on any reasonable unixy environment, with a reasonable
  C++ compiler (e.g. gcc version >= 3.1) and GNU Make.  Additionally, libCVD
  supports both normal and cross compilers.  It is known to work on:
  
  -Well tested (current):
	- Linux: x86, x86-64
	- Linux: ARM LPC3180, XScale (cross compile)
	- uCLinux: Blackfin  (cross compile)
	- FreeBSD: x86
	- OpenBSD: XScale
	- Mac OS X: x86, PPC
		- Supports the OSX build environment including:
			- Frameworks
			- .dylib libraries
	- Cygwin: x86
	- MinGW: x86 (native and cross compile)
	- Win32: Visual Studio 2005

  -Has worked on (current status unknown):
	- Linux: PPC
	- Solaris: SPARC
	- IRIX SGI O2: MIPS


\section Compiling

The normal system works:
@code
	./configure
	make 
	make install
@endcode

libCVD fully supports parallel builds (<code>make -j2</code> for instance).

\subsection slBugs Library bugs/issues
	There are a few known library bugs which prevent the libraries working with libCVD

	- TooN: If you have gcc >= 4.2.0 you need a version of TooN at least as recent as SNAPSHOT_20080725.


\subsection scBugs Compiler bugs
	There are a few known compiler bugs which affect libCVD on various platforms.

	- ppc-darwin (Mac OS X, on PPC)
		- Does not support gcc 3.3 (20030304) due to possible compiler bug.
		- Workaround: none. Install a more recent compiler.
	- arm-linux (cross compiler running on i686-pc-linux-gnu)
		- Internal compiler error with: gcc version 3.4.0 20040409 (CodeSourcery ARM Q1 2004) on:
			- cvd_src/videosource.cc
			- pnm_src/jpeg.cc
		- Remove <code>cvd_src/videosource.o</code> from <code>Makefile</code> and use <code>--disable-jpeg</code>
		- Compile files with -O2 instead of -O3
*/

///////////////////////////////////////////////////////
// Modules classifying classes and functions

/// @defgroup gImage Image storage and manipulation
/// Basic image functionality. The %CVD image classes provide fast and
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
/// Exceptions generated and thrown by %CVD classes and functions

/// @defgroup gGraphics Computer graphics
/// Classes and functions to support miscellaneous pixel operations

/// @defgroup gVision Computer Vision
/// Functions and classes to support common computer vision concepts and operations

/// @defgroup gGL GL helper functions and classes.
/// Overloaded versions of GL functions to use %CVD classes and datatypes, and
/// other helpful GL classes and functions.

/// @defgroup gMaths Mathematical operations
/// Useful mathematical classes and functions

/// @defgroup gLinAlg Linear Algebra
/// Classes and functions for common Linear Algebra concepts and operations

/// @defgroup gCPP General C++ and system helper functions
/// Classes and functions for writing better code


/// @namespace CVD
/// All classes and functions are within the CVD namespace


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
    - Flexible loading and saving from a variety of types: jpg, pnm, tiff(loading only)
    - Image grabbing from video sources:
		  - Linux
		  		- Video for Linux devices
          		- Video for Linux 2 devices
         	 	- Firewire cameras
		  - IRIX
          		- dmedia video capture
		  - All platforms
          		- avi and mpegs files (all codecs supported by ffmpeg)
          		- list of images
          		- raw image streams(soon!!!) 
    - Colorspace conversions on images and video streams
    - Various image processing tools
    - %Camera calibration support: Linear, Cubic and Quintic 

\subsection Mathematical

    - Lie Group(S03, SE3) algebra
    - Iterative and reweighted least-squares 

\subsection Portability

    - Linux
    - IRIX (SGI O2)
    - FreeBSD
    - Mac OS X
		- Supports the OS X build environment including
			- Frameworks
			- .dylib libraries
		- Currently does not support gcc 3.3 (20030304) due to possible compiler bug.
    - Cygwin(painful)
		- Does not properly support the cygwin build environment. Manual intervention is needed.
	- Probably any other unix with a modern C++ compiler and a reasonably standard build environment (gmake is required)

 
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


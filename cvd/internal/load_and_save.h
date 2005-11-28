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
#ifndef CVD_LOAD_AND_SAVE_H
#define CVD_LOAD_AND_SAVE_H

#include <cvd/internal/disk_image.h>
namespace CVD {

	namespace Exceptions
	{
		/// %Exceptions specific to image loading and saving
		/// @ingroup gException
		namespace Image_IO
		{
			/// Base class for all Image_IO exceptions
			/// @ingroup gException
			struct All: public CVD::Exceptions::All
			{};

			/// This image type is not supported
			/// @ingroup gException
			struct UnsupportedImageType: public All
			{
				UnsupportedImageType();
			};

			/// The file ended before the image
			/// @ingroup gException
			struct EofBeforeImage: public All
			{
				EofBeforeImage();
			};

			/// The ifstream which the file is being read from is not open
			/// @ingroup gException
			struct IfstreamNotOpen: public All
			{
				IfstreamNotOpen();
			};

			/// The image was incorrect
			/// @ingroup gException
			struct MalformedImage: public All
			{
				MalformedImage(const std::string &); ///< Construct from a message string
			};

			/// The loaded image is not the right size
			/// @ingroup gException
			struct ImageSizeMismatch: public All
			{
				ImageSizeMismatch(const ImageRef& src, const ImageRef& dest); ///< Construct from the two sizes
			};

			/// Error writing the image
			/// @ingroup gException
			struct WriteError: public All
			{
				WriteError(const std::string& err); ///< Construct from a message string
			};

			/// Cannot seek in this stream
			/// @ingroup gException
			struct UnseekableIstream: public All
			{
				UnseekableIstream(const std::string& type); ///< Construct from a message string
			};

			/// Type mismatch reading the image (image data is either 8- or 16-bit, and it must be the same in the file)
			/// @ingroup gException
			struct ReadTypeMismatch: public All
			{
				ReadTypeMismatch(bool read8); ///< Constructor is passed <code>true</code> if it was trying to read 8-bit data
			};
			
			/// An error occurred in one of the helper libraries
			/// @ingroup gException
			struct InternalLibraryError: public All
			{
				InternalLibraryError(const std::string& lib, const std::string err); ///< Construct from the library name and the error string
			};

			/// This image subtype is not supported
			/// @ingroup gException
			struct UnsupportedImageSubType: public All
			{
				UnsupportedImageSubType(const std::string &, const std::string&); ///< Construct from a subtype string and an error string
			};

			/// Error in opening file
			/// @ingroup gException
			struct OpenError: public All
			{
				OpenError(const std::string&, const std::string&, int); ///< Construct from the filename and the error number
			};


		}
	}




}

#endif

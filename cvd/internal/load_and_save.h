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

#include <cvd/exceptions.h>
#include <cvd/image_convert.h>
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/internal/name_CVD_rgb_types.h>

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
				ReadTypeMismatch(const std::string& available, const std::string& requested);
			};

			/// Type mismatch reading the image (image data is either 8- or 16-bit, and it must be the same in the file)
			/// @ingroup gException
			struct WriteTypeMismatch: public All
			{
				WriteTypeMismatch(const std::string& available, const std::string& requested);
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


	namespace Internal
	{
		template<class C, int i> struct save_default_
		{
			static const bool use_16bit=1;
		};
		  
		template<class C> struct save_default_<C,1>
		{
			static const bool use_16bit=(CVD::Pixel::traits<typename CVD::Pixel::Component<C>::type>::bits_used) > 8;
		};
		  
		template<class C> struct save_default
		{
			static const bool use_16bit = save_default_<C, CVD::Pixel::traits<typename CVD::Pixel::Component<C>::type>::integral>::use_16bit;
		};


		////////////////////////////////////////////////////////////////////////////////	
		//Mechanisms for generic image loading
		//
		// Image readers are duck-types and must provide the following:
		// typedef  Types         Typelist containing types which can be loaded
		// string   datatype()    Stringified name of datatype on disk
		// string   name()        Name of the reader (JPEG, TIFF, etc)
		// ImageRef size()        size()
		// void get_raw_pixel_line(T) Where T is available for everything in Types
		// Constructor accepting istream;

		
		//Basic typelist.
		struct Head{};
		template<class A, class B> struct TypeList
		{
			typedef A Type;
			typedef B Next;
		};

		

		////////////////////////////////////////////////////////////////////////////////
		//
		// Read data and process if necessary. 
		// In the case where the in-memory and on-disk datatypes match, no processing 
		// is performed.
		template<class PixelType, class DiskPixelType, class ImageLoader> struct read_and_maybe_process
		{
			static void exec(SubImage<PixelType>& im, ImageLoader& r)
			{
				Image<DiskPixelType> rowbuf(ImageRef(r.size().x, 1));

				for(int row = 0; row < r.size().y; row++)
				{
					r.get_raw_pixel_line(rowbuf.data());
					Pixel::ConvertPixels<DiskPixelType, PixelType>::convert(rowbuf.data(), im[row], r.size().x);
				}
			}
		};

		template<class PixelType, class ImageLoader> struct read_and_maybe_process<PixelType, PixelType, ImageLoader>
		{
			static void exec(SubImage<PixelType>& im, ImageLoader& r)
			{
				for(int row = 0; row < r.size().y; row++)
					r.get_raw_pixel_line(im[row]);
			}
		};


		////////////////////////////////////////////////////////////////////////////////	
		//
		// Iterate over the typelist, and decide which type to load. 
		//
		template<class PixelType, class ImageLoader, class List > struct Reader
		{	
			static void read(SubImage<PixelType>& im, ImageLoader& r)
			{
				if(r.datatype() == PNM::type_name<typename List::Type>::name())
				{
					read_and_maybe_process<PixelType, typename List::Type, ImageLoader>::exec(im, r);
				}
				else
					Reader<PixelType, ImageLoader, typename List::Next>::read(im, r);
			}
		};

		template<class PixelType, class ImageLoader> struct Reader<PixelType, ImageLoader, Head>
		{
			static void read(SubImage<PixelType>&, ImageLoader& r)
			{	
				throw Exceptions::Image_IO::UnsupportedImageSubType(r.name(), r.datatype() + " not yet supported");
			}
		};

		
		////////////////////////////////////////////////////////////////////////////////	
		//
		// Driver functions for loading images.
		//

		template<class T, class ImageLoader> void readImage(SubImage<T>& im, ImageLoader& r)
		{
			Reader<T, ImageLoader, typename ImageLoader::Types>::read(im, r);
		}

		template <class T, class ImageLoader> void readImage(SubImage<T>& im, std::istream& in)
		{
			ImageLoader loader(in);
			ImageRef size = loader.size();
			if (im.size() != size)
				throw Exceptions::Image_IO::ImageSizeMismatch(size, im.size());

			readImage(im, loader);
		}

		template <class T, class ImageLoader> void readImage(Image<T>& im, std::istream& in)
		{
		  ImageLoader loader(in);
		  im.resize(loader.size());
		  readImage(im, loader);
		}

		////////////////////////////////////////////////////////////////////////////////	
		//
		// Functions for attempting to choose an image type based on the datatype.
		// The template code provides information detailing the ideal image type.
		// The writer object is initialized with this information, and returns the type
		// that it will write.
		//
		// Unlike loading, where the type on disk is known only dynamically, the type
		// being saved and any necessary conversions can be deduced statically.
		//
		// The writer objects provide an interface with the following parts:
		//
		// template<class C> Outgoing::type              For a given incoming pixel type C, this is the outgoing type
		// constructor(ostream&, ImageRef, string type)  Construct an image writer for a given type
		// void write_raw_pixel_line(T*)                 Write pixels of type T.

	
		////////////////////////////////////////////////////////////////////////////////
		//
		// Select an outgoing type, convert if necessary and then save.
		//
		template<class Pixel, class ImageWriter, class OutgoingPixel> struct maybe_process_and_write
		{	
			static void write(std::ostream& os, const SubImage<Pixel>& im)
			{
				ImageWriter w(os, im.size(), CVD::PNM::type_name<OutgoingPixel>::name());
				Image<OutgoingPixel> row(ImageRef(im.size().x, 1));

				for(int r=0; r < im.size().y; r++)
				{
					CVD::Pixel::ConvertPixels<Pixel, OutgoingPixel>::convert(im[r], row.data(), im.size().x);
					w.write_raw_pixel_line(row.data());
				}
			}
		};

		template<class Pixel, class ImageWriter> struct maybe_process_and_write<Pixel, ImageWriter, Pixel>
		{	
			static void write(std::ostream& os, const SubImage<Pixel>& im)
			{
				ImageWriter w(os, im.size(), CVD::PNM::type_name<Pixel>::name());
				for(int r=0; r < im.size().y; r++)
					w.write_raw_pixel_line(im[r]);
			}
		};

		template<class Pixel, class Writer> void writeImage(const SubImage<Pixel>& im, std::ostream& o)
		{
			maybe_process_and_write<Pixel, Writer, typename Writer::template Outgoing<Pixel>::type>::write(o, im);
		}

	
	}


}

#endif

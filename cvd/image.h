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
//-*- c++ -*-
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CVD::image.h                                                        //
//                                                                      //
//  Definitions for of template class CVD::Image, fast_image			//
//                                                                      //
//  derived from IPRS_* developed by Tom Drummond                       //
//	Reworked to provide class heirachy and memory managementby E. Rosten//
//                                                                      //
//////////////////////////////////////////////////////////////////////////


/*******************************************************************************

							CVD::Image
	assume typedef ImageRef Ir, from here
	
	
	CLASS HEIRACHY:

	BasicImage		             Access an arbitraty block of data as an image
	  | 
	  +-VideoFrame               } Access blocks of data returned from video
	  |   |                      } capture devices as images. Defined in other
      |   +-V4L2Frame            } files.
	  |   +-O2Frame              } 
	  |   :                  
	  |	  +-LocalVideoFrame	     } Access blocks of data from local sources 
	  |       |                  } as video frames. Since the data is local, 
	  |		  +-DiskBuffer2Frame } these use Image to manage memory. Access
	  |       :                  } to the private image is allowed
	  |       +-etc              }
	  |
	  +-Image		   Images with memory management.

	
	METHODS

	  All images have the following methods (with const versions where 
	  applicable):

	  int 		totalsize()				Number of elements in the image
	  Ir 		size()					Size (x by y) of image
	  bool		in_image(Ir)			Is an ImageRef inside the image
	  T*		data()					block of data the image is in
	  T&		operator[](Ir)			access pixels lika an array with an Ir
	  T*		operator[](int)			return a row of the image
	  Ir		pos()					return position of a pointer

	  BasicImage is constructed from a blobck of data:

	  BasicImage(T*, ImageRef);



	  Image provides images which do reference counting on the data. Multiple
	  images can point to one block of data (so use the same care as with 
	  pointers). Copy constructing is quite fast (a 16 byte copy and an
	  increment) so they can be efficiently used in containers like std::vector.

	  copy_from(BasicImage)	   	Copies from an image, rather than referencing
	  							it.
								
	  make_unique()				Makes the image be the only reference to the 
								data by copying the data if necessary.
	
	  resize()					Resize the image. The image disowns the data 
	  							before resizing so that other images referencing
								it are not affected. data() will be altered.

	  Image(ImageRef)			Create an empty image of this size.

	  Image(Image)				Make this image reference the same block of
	  operator=(Image)			data as the argument.

	  Image(BasicImage)			Copy from a BasicImage. SLOW! 


	USAGE 
		
	  void do_stuff(BasicImage<T>);
	    Defines a function which can work on any kind of image, eg Images,
		VideoFrames, etc.

	  Image<T> foo; 
	  	Creates an image of type T, which can be copied, assigned to, and used 
		in STL containers efficiently.
	

	  Images behave like pointers. Copying an image is like copying a pointer:
	  both the source and destination point to the same chunk of data. To 
	  further the analogy, [] dereferences images.

	MACROS
	  
	  Defining CVD_IMAGE_DEBUG before including this file compiles in bounds
	  checking with operator[]


*******************************************************************************/


#ifndef CVD_IMAGE_H
#define CVD_IMAGE_H

#include <string.h>
#include <cvd/image_ref.h>
#include <cvd/exceptions.h>
#include <string>
#include <cvd/internal/aligned_mem.h>

namespace CVD {

namespace Exceptions {

  /// @ingroup gException
  namespace Image {
      /// Base class for all Image_IO exceptions
        /// @ingroup gException
        struct All: public CVD::Exceptions::All {};

        /// Input images have incompatible dimensions
        /// @ingroup gException
        struct IncompatibleImageSizes : public All {
            IncompatibleImageSizes(const std::string & function)
            {
                what = "Incompatible image sizes in " + function;
            };
        };

        /// Input ImageRef not within image dimensions
        /// @ingroup gException
        struct ImageRefNotInImage : public All {
            ImageRefNotInImage(const std::string & function)
            {
                what = "Input ImageRefs not in image in " + function;
            };
        };
    }
}


#ifdef CVD_IMAGE_DEBUG
	#define CVD_IMAGE_ASSERT(X,Y)  if(!(X)) throw Y()
#else
	#define CVD_IMAGE_ASSERT(X,Y)
#endif

/// Fatal image errors (used for debugging). These are not included in the
/// main CVD::Exceptions namespace since they are fatal errors which are 
/// only thrown if the library is compiled with <code>-D CVD_IMAGE_DEBUG</code>.
/// This compiles in image bounds checking (see CVD::BasicImage::operator[]())
/// and makes image accesses very slow!
/// @ingroup gException
namespace ImageError
{
	/// An attempt was made to access a pixel outside the image. Note that this is
	/// not derived from CVD::Exceptions::All.
	/// @ingroup gException
	class AccessOutsideImage{};
}


namespace ImageUtil
{
	template<class T> inline void memfill(T* data, int n, const T val)
	{
		T* de = data + n;
		for(;data < de; data++)
			*data=val;
	}

	template<> inline void memfill(unsigned char* data, int n, const unsigned char val)
	{
		memset(data, val, n);
	}

	template<> inline void memfill(signed char* data, int n, const signed char val)
	{
		memset(data, val, n);
	}

	template<> inline void memfill(char* data, int n, const char val)
	{
		memset(data, val, n);
	}
}

template<class T> class SubImageIteratorEnd;
template<class T> class SubImage;

template<class T> class SubImageIterator
{
	public:
		void operator++()
		{
			ptr++;
			if(ptr == row_end)
			{
				ptr += row_increment;
				row_end += total_width;

				if(ptr >= end)
					end = NULL;
			}
		}

		void operator++(int)
		{
			operator++();
		}
	
		T* operator->() { return ptr; }
		const T* operator->() const { return ptr; }

		T& operator*(){ return *ptr;}
		const T& operator*() const { return *ptr;}

		bool operator<(const SubImageIterator& s) const { return ptr < s.ptr; }
		bool operator==(const SubImageIterator& s) const { return ptr == s.ptr; }
		bool operator!=(const SubImageIterator& s) const { return ptr != s.ptr; }


		bool operator!=(const SubImageIteratorEnd<T>& s) const
		{
			return end != NULL;
		}

		bool operator<(const SubImageIteratorEnd<T>& s) const
		{
			//It's illegal to iterate _past_ end(), so < is equivalent to !=
			return end != NULL;
		}

		SubImageIterator()
		{}

		SubImageIterator(T* start, int image_width, int row_stride, T* off_end)
		:ptr(start),
		 row_end(start + image_width), 
		 end(off_end), 
		 row_increment(row_stride-image_width), 
		 total_width(row_stride)
		{ }

		SubImageIterator(T* end) :ptr(end){ }

	private:
		T* ptr, *row_end, *end;
		int row_increment, total_width;
};


template<class T> class SubImageIteratorEnd
{
	public:
		SubImageIteratorEnd(SubImage<T>* p)
		:i(p){}

		operator SubImageIterator<T>()
		{
			return i->end();
		}

	private:
		SubImage<T>* i;
};


template<class T> class SubImage
{
	public:
		/// Construct an image from a block of data.
		/// @param data The image data in horizontal scanline order
		/// @param size The size of the image
		/// @param stride The row stride (or width, including the padding)
		SubImage(T* data, const ImageRef& size, int stride)
		:my_data(data),my_size(size),my_stride(stride)
		{
		}

		/// Is this pixel co-ordinate inside the image?
		/// @param ir The co-ordinate to test
		bool in_image(const ImageRef& ir) const
		{
			return ir.x >=0 && ir.y >=0 && ir.x < my_size.x && ir.y < my_size.y;
		}

		/// The image data is not destroyed when a BasicImage is destroyed.
		~SubImage()
		{}

		/// Access a pixel from the image. Bounds checking is only performed if the library is compiled
		/// with <code>-D CVD_IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is 
		/// thrown.
		inline T& operator[](const ImageRef& pos)
		{
			CVD_IMAGE_ASSERT(in_image(pos), ImageError::AccessOutsideImage);
			return (my_data[pos.y*my_stride + pos.x]);
		}
		
		/// Access a pixel from the image. Bounds checking is only performed if the library is compiled
		/// with <code>-D CVD_IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is 
		/// thrown.
		inline const T& operator[](const ImageRef& pos) const 
		{
			CVD_IMAGE_ASSERT(in_image(pos), ImageError::AccessOutsideImage);
			return (my_data[pos.y*my_stride + pos.x]);
		}

        /// Access pointer to pixel row. Returns the pointer to the first element of the passed row.
        /// Allows to use [y][x] on images to access a pixel. Bounds checking is only performed if the library is compiled
        /// with <code>-D CVD_IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is
        /// thrown.
        inline T* operator[](int row)
        {
            CVD_IMAGE_ASSERT(in_image(ImageRef(0,row)), ImageError::AccessOutsideImage);
            return my_data+row*my_stride;
        }

        /// Access pointer to pixel row. Returns the pointer to the first element of the passed row.
        /// Allows to use [y][x] on images to access a pixel. Bounds checking is only performed if the library is compiled
        /// with <code>-D CVD_IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is
        /// thrown.
        inline const T* operator[](int row) const
        {
            CVD_IMAGE_ASSERT(in_image(ImageRef(0,row)), ImageError::AccessOutsideImage);
            return my_data+row*my_size.x;
        }

		/// Given a pointer, this returns the image position as an ImageRef
		inline ImageRef pos(const T* ptr) const
		{
			int diff = ptr - data();
			return ImageRef(diff % my_stride, diff / my_size.x);
		}

		/// Returns the raw image data
		inline const T* data() const
		{
			return my_data;
		}
		
		/// Returns the raw image data
		inline T* data()
		{
			return my_data;
		}

		typedef SubImageIterator<T> iterator;
		
		/// Returns an iterator referencing the first (top-left) pixel in the image
		inline SubImageIterator<T> begin()
		{
			return SubImageIterator<T>(data(), size().x, my_stride, operator[](my_size.y));
		}

		/// Returns an iterator pointing to one past the end of the image
		inline SubImageIterator<T> end()
		{
			return SubImageIterator<T>(operator[](my_size.y));
		}

		/// Returns an object corresponding to end(), which should eliminate a test.
		inline SubImageIteratorEnd<T> fastend()
		{
			return SubImageIteratorEnd<T>(this);
		}


		/// What is the size of this image?
		inline ImageRef size() const
		{
			return my_size;
		}

		/// What is the total number of elements in the image (i.e. <code>size().x * size().y</code>), including padding
		inline int totalsize() const
		{
			return my_stride * my_size.y;
		}

		/// Set all the pixels in the image to zero. This is a relatively fast operation, using <code>memset</code>.
		inline void zero() 
		{
			memset(my_data, 0, totalsize()*sizeof(T));
		}

		/// Set all the pixels in the image to a value. This is a relatively fast operation, using <code>memfill</code>.
		/// @param d The value to write into the image
		inline void fill(const T d)
		{
			ImageUtil::memfill(my_data, totalsize(), d);
		}

		/// Copy constructor
		/// @param copyof The image to copy
		SubImage(const SubImage& copyof)
		{
		  my_size = copyof.my_size;
		  my_data = copyof.my_data;
		  my_stride = copyof.my_stride;
		}
		

		/// Return a sub image
		/// @param start Top left pixel of the sub image
		/// @param size width and  height of the sub image
		SubImage sub_image(const ImageRef& start, const ImageRef& size)
		{
			return SubImage( &operator[](start), size, my_stride);
		}

		/// Return const a sub image
		/// @param start Top left pixel of the sub image
		/// @param size width and  height of the sub image
		const SubImage sub_image(const ImageRef& start, const ImageRef& size) const
		{
			return SubImage( &operator[](start), size, my_stride);
		}

	protected:
		T* my_data;       ///< The raw image data
		ImageRef my_size; ///< The size of the image
		int my_stride;    ///< The row stride

		SubImage()
		{}

};


/// A generic image class to manage a block of data as an image. Provides
/// basic image access such as accessing a particular pixel co-ordinate. 
/// @param T The pixel type for this image. Typically either
/// <code>CVD::byte</code> or <code>CVD::Rgb<CVD::byte> ></code> are used,
/// but images could be constructed of any available type.
/// 
/// A BasicImage does not manage its own data, but provides access to an 
/// arbitrary externally-managed block of data as though it were an image. Use
/// the derived Image class if you want an image which also has its own data.
/// 
/// Loading and saving, format conversion and some copying functionality is
/// provided by external functions rather than as part of this class. See
/// the @ref gImageIO "Image loading and saving, and format conversion" module
/// for documentation of these functions.
/// @ingroup gImage
template<class T> class BasicImage: public SubImage<T>
{
	public:
		/// Construct an image from a block of data.
		/// @param data The image data in horizontal scanline order
		/// @param size The size of the image
		BasicImage(T* data, const ImageRef& size)
		:SubImage<T>(data, size, size.x)
		{
		}

		/// Copy constructor
		/// @param copyof The image to copy
		BasicImage(const BasicImage& copyof)
		{
		  SubImage<T>::my_size = copyof.my_size;
		  SubImage<T>::my_data = copyof.my_data;
		  SubImage<T>::my_stride = copyof.my_stride;
		}
	
		/// The image data is not destroyed when a BasicImage is destroyed.
		~BasicImage()
		{}

		/** A random-access iterator to read or write pixel values from the image.
		This can be incremented, decremented and dereferenced. Incrementing this
		iterator steps through pixels in the usual scanline order. */
		typedef T* iterator;
		/** A random-access iterator to read pixel values from the image.
		This can be incremented, decremented and dereferenced. Incrementing this
		iterator steps through pixels in the usual scanline order. */
		typedef const T* const_iterator;

		/** Returns a const iterator referencing the first (top-left) pixel in the
		image. */
		const_iterator begin() const { return SubImage<T>::my_data; }
		/** Returns an iterator referencing the first (top-left) pixel in the
		image. */
		iterator begin() { return SubImage<T>::my_data; }

		/** Returns a const iterator referencing the <em>pixel immediately
		after</em> the last (bottom-right) pixel in the image. */
		const_iterator end() const { return SubImage<T>::my_data+SubImage<T>::totalsize(); }
		/** Returns an iterator referencing the <em>pixel immediately
		after</em> the last (bottom-right) pixel in the image. */
		iterator end() { return SubImage<T>::my_data+SubImage<T>::totalsize(); }



	protected:
		/// The default constructor does nothing
		BasicImage()
		{}
	private:
		void operator=(const BasicImage&copyof)
		{
			//my_size = copyof.my_size;
			//my_data = copyof.my_data;
		}
};


/// A full image which manages its own data.
/// @param T The pixel type for this image. Typically either
/// <code>CVD::byte</code> or <code>CVD::Rgb<CVD::byte> ></code> are used,
/// but images could be constructed of any available type.
///
/// Images do reference counting on the data, so multiple images can point
/// to one block of data. This means that copying an image is like copying a
/// pointer (so use the same care); to further the analogy, operator[]()
/// dereferences images. Copy constructing is quite fast (a 16-byte copy and
/// an increment), so images can be efficiently passed back in functions or
/// used in containers like std::vector
///
/// Loading and saving, format conversion and some copying functionality is
/// provided by external functions rather than as part of this class. See
/// the @ref gImageIO "Image loading and saving, and format conversion" module
/// for documentation of these functions.
/// @ingroup gImage
template<class T> 
class Image: public BasicImage<T>
{
	public:
		///Copy constructor. This does not copy the data, it just creates a new
		///reference to the image data
		///@param copy The image to copy
		Image(const Image& copy) :
			BasicImage<T>(copy)
		{
			dup_from(&copy);
		}


		///Make a (new) copy of the image, also making a copy of the data
		///@param copy The image to copy
		void copy_from(const BasicImage<T>& copy)
		{
			Image<T> tmp(copy.size());

			*this = tmp;
			
			std::copy(copy.begin(), copy.end(), this->begin());
		}

		///Make this image independent of any copies (i.e. force a copy of the image data).
		void make_unique()
		{
			if(*num_copies > 1)
			{
				Image<T> tmp(*this);
				copy_from(tmp);
			}
		}

		///Assign this image to another one. This does not copy the data, it just creates a new
		///reference to the image data
		///@param copyof The image to copy
		const Image& operator=(const Image& copyof)
		{
			remove();
			dup_from(&copyof);
			return *this;
		}
		
		///Default constructor
		Image()
		{
			dup_from(NULL);
		}

		///Create an empty image of a given size.
		///@param size The size of image to create
		Image(const ImageRef& size)
		{
			num_copies = new int;
			*num_copies = 1;
 			this->my_size = size;
 			this->my_stride = size.x;
			this->my_data = Internal::aligned_mem<T,16>::alloc(this->totalsize());
		}
		
		///Resize the image (destroying the data). The image is resized even if the new size is the same as the old one.
		///@param size The new size of the image
		void resize(const ImageRef& size)
		{
			Image<T> new_im(size);
			*this = new_im;
		}

		///The destructor removes the image data
		~Image()
		{
			remove();
		}

		
	private:
		int* num_copies;			//Reference count.

		inline void remove()		//Get rid of a reference to the data
		{
			if(this->my_data && *num_copies && --(*num_copies) == 0)
			{
			  Internal::aligned_mem<T,16>::release(this->my_data);
			  this->my_data = 0;
			  delete   num_copies;
			  num_copies = 0;
			}
		}

		inline void dup_from(const Image* copyof)  //Duplicate from another image
		{
			if(copyof != NULL && copyof->my_data != NULL)
			{
				this->my_size = copyof->my_size;
				this->my_stride = copyof->my_stride;
				this->my_data = copyof->my_data;
				num_copies = copyof->num_copies;
				(*num_copies)++;
			}
			else
			{
				this->my_size.home();
				this->my_stride=0;
				this->my_data = 0;
				num_copies = 0;
			}
		}
};


} // end namespace
#endif


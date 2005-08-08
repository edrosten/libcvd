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
	  
	  Defining IMAGE_DEBUG before including this file compiles in bounds
	  checking with operator[]


*******************************************************************************/


#ifndef __CVD_IMAGE_H__
#define __CVD_IMAGE_H__

#include <string.h>
#include <cvd/image_ref.h>

namespace CVD {


#ifdef IMAGE_DEBUG
	#define IMAGE_ASSERT(X,Y)  if(!(X)) throw Y()
#else
	#define IMAGE_ASSERT(X,Y)
#endif

/// Fatal image errors (used for debugging). These are not included in the
/// main CVD::Exceptions namespace since they are fatal errors which are 
/// only thrown if the library is compiled with <code>-D IMAGE_DEBUG</code>.
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

/// A generic image class to manage a block of data as an image. Provides
/// basic image access such as accessing a particular pixel co-ordinate. 
/// A BasicImage does not manage its own data, but provides access to an 
/// arbitrary externally-managed block of data as though it were an image. Use
/// the derived Image class if you want an image which also has its own data.
/// @param T The pixel type for this image. Typically either <code>CVD::byte</code> or 
/// <code>CVD::Rgb<CVD::byte> ></code> are used, but images could be constructed of any available type.
/// @ingroup gImage
template<class T> class BasicImage
{
	public:
		/// Construct an image from a block of data.
		/// @param data The image data in horizontal scanline order
		/// @param size The size of the image
		BasicImage(T* data, const ImageRef& size)
		:my_data(data),my_size(size)
		{
		}

		/// Copy constructor
		/// @param copyof The image to copy
		BasicImage(const BasicImage& copyof)
		{
			my_size = copyof.my_size;
			my_data = copyof.my_data;
		}

		/// Is this pixel co-ordinate inside the image?
		/// @param ir The co-ordinate to test
		bool in_image(const ImageRef& ir) const
		{
			return ir.x >=0 && ir.y >=0 && ir.x < my_size.x && ir.y < my_size.y;
		}

		/// The image data is not destroyed when a BasicImage is destroyed.
		~BasicImage()
		{}

		/// Access a pixel from the image. Bounds checking is only performed if the library is compiled
		/// with <code>-D IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is 
		/// thrown.
		inline T& operator[](const ImageRef& pos)
		{
			IMAGE_ASSERT(in_image(pos), ImageError::AccessOutsideImage);
			return (my_data[pos.y*my_size.x + pos.x]);
		}
		
		/// Access a pixel from the image. Bounds checking is only performed if the library is compiled
		/// with <code>-D IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is 
		/// thrown.
		inline const T& operator[](const ImageRef& pos) const 
		{
			IMAGE_ASSERT(in_image(pos), ImageError::AccessOutsideImage);
			return (my_data[pos.y*my_size.x + pos.x]);
		}

        /// Access pointer to pixel row. Returns the pointer to the first element of the passed row.
        /// Allows to use [y][x] on images to access a pixel. Bounds checking is only performed if the library is compiled
        /// with <code>-D IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is
        /// thrown.
        inline T* operator[](int row)
        {
            IMAGE_ASSERT(in_image(ImageRef(0,row)), ImageError::AccessOutsideImage);
            return my_data+row*my_size.x;
        }

        /// Access pointer to pixel row. Returns the pointer to the first element of the passed row.
        /// Allows to use [y][x] on images to access a pixel. Bounds checking is only performed if the library is compiled
        /// with <code>-D IMAGE_DEBUG</code>, in which case an ImageError::AccessOutsideImage exception is
        /// thrown.
        inline const T* operator[](int row) const
        {
            IMAGE_ASSERT(in_image(ImageRef(0,row)), ImageError::AccessOutsideImage);
            return my_data+row*my_size.x;
        }

		/// Given a pointer, this returns the image position as an ImageRef
		inline ImageRef pos(const T* ptr) const
		{
			int diff = ptr - data();
			return ImageRef(diff % my_size.x, diff / my_size.x);
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

		/// What is the size of this image?
		inline ImageRef size() const
		{
			return my_size;
		}

		/// What is the total number of pixels in the image (i.e. <code>size().x * size().y</code>).
		inline int totalsize() const
		{
			return my_size.x * my_size.y;
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


	protected:
		/// The default constructor does nothing
		BasicImage()
		{}

		T* my_data;       ///< The raw image data
		ImageRef my_size; ///< The size of the image

	private:
		void operator=(const BasicImage&);

};


// HELP: Is this still relevant? Should this be part of the 
// Doxygen documnetation? PAS 20/4/05
//Since the images implement copy on write, each write operation has to be 
//checked, which could be slow. Image provides a cast to NonConstImage which
//does not do any checking of this sort on writes, but care must be taken:
//Since multiple images can point to the same set of data, care has to be taken
//with references to the image data. For instance if a pointer or NonConstImage
//is made to refer to the data, and <em>then</em> another Image is made to reference
//the data as well, using the pointer or the NonConstImage for writing will
//alter <em>both</em> images.
//If this situation is encountered, <code>image1=image2.force_copy();</code> can be used
//to force a copy operation, so that image1 and image2 do not reference the 
//same chunk of data.


/// A full image which manages its own data.
/// Images do reference counting on the data, so multiple images can point
/// to one block of data. This means that copying an image is like copying a pointer
/// (so use the same care); to further the analogy, operator[]() dereferences images. 
/// Copy constructing is quite fast (a 16-byte copy and an increment), so images can be
/// efficiently passed back in functions or used in containers like std::vector
/// @param T The pixel type for this image. Typically either <code>CVD::byte</code> or 
/// <code>CVD::Rgb<CVD::byte> ></code> are used, but images could be constructed of any available type.
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

			memcpy(my_data, copy.data(), totalsize()*sizeof(T));
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

/* I don't think we want this. Make it more explicit
		Image(const BasicImage<T>& copy) //This has to be slow, since it might
									  //not be a good idea to keep references
									  //to that data
		{
			dup_from(NULL):
			copy_from(copy);
		}
*/		
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
 			my_size = size;
			my_data  = new T[totalsize()];
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
			if(my_data && *num_copies && --(*num_copies) == 0)
			{
				delete[] my_data;
				my_data = 0;
				delete   num_copies;
				num_copies = 0;
			}
		}

		inline void dup_from(const Image* copyof)  //Duplicate from another image
		{
			if(copyof != NULL && copyof->my_data != NULL)
			{
				my_size = copyof->my_size;
				my_data = copyof->my_data;
				num_copies = copyof->num_copies;
				(*num_copies)++;
			}
			else
			{
				my_size.home();
				my_data = 0;
				num_copies = 0;
			}
		}
};


} // end namespace
#endif


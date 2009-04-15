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
//  Definitions for of template class CVD::Image, fast_image		//
//                                                                      //
//  derived from IPRS_* developed by Tom Drummond                       //
//	Reworked to provide class heirachy and memory managementby E. Rosten//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef CVD_IMAGE_H
#define CVD_IMAGE_H

#include <string.h>
#include <cvd/image_ref.h>
#include <cvd/exceptions.h>
#include <string>
#include <utility>
#include <iterator>
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

#ifndef DOXYGEN_IGNORE_INTERNAL
namespace Internal
{
	template<class C> class ImagePromise
	{};
};
#endif

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
template<class T> class ConstSubImageIteratorEnd;
template<class T> class ConstSubImage;


template<class T> class ConstSubImageIterator
{
	public:
		const ConstSubImageIterator& operator++()
		{
			ptr++;
			if(ptr == row_end)
			{
				ptr += row_increment;
				row_end += total_width;

				if(ptr >= end)
					end = NULL;
			}
			return *this;
		}

		void operator++(int)
		{
			operator++();
		}
	
		const T* operator->() const { return ptr; }
		const T& operator*() const { return *ptr;}

		bool operator<(const ConstSubImageIterator& s) const { return ptr < s.ptr; }
		bool operator==(const ConstSubImageIterator& s) const { return ptr == s.ptr; }
		bool operator!=(const ConstSubImageIterator& s) const { return ptr != s.ptr; }


		bool operator!=(const ConstSubImageIteratorEnd<T>&) const { return end != NULL; }
		bool operator!=(const SubImageIteratorEnd<T>&) const { return end != NULL; }
		//It's illegal to iterate _past_ end(), so < is equivalent to !=
		bool operator<(const ConstSubImageIteratorEnd<T>&) const { return end != NULL; }
		bool operator<(const SubImageIteratorEnd<T>&) const { return end != NULL; }

		
		//Make it look like a standard iterator
		typedef std::forward_iterator_tag iterator_category;
		typedef T value_type;
		typedef ptrdiff_t difference_type;
		typedef const T* pointer;
		typedef const T& reference;



		ConstSubImageIterator()
		{}

		ConstSubImageIterator(const T* start, int image_width, int row_stride, const T* off_end)
		:ptr(const_cast<T*>(start)),
		 row_end(start + image_width), 
		 end(off_end), 
		 row_increment(row_stride-image_width), 
		 total_width(row_stride)
		{ }

		ConstSubImageIterator(const T* end) 
		:ptr(const_cast<T*>(end))
		{ }

	protected:
		T* ptr;
		const T *row_end, *end;
		int row_increment, total_width;
};

template<class T> class SubImageIterator: public ConstSubImageIterator<T>
{
	public:
		SubImageIterator(T* start, int image_width, int row_stride, const T* off_end)
		:ConstSubImageIterator<T>(start, image_width, row_stride, off_end)
		{}
		
		SubImageIterator(T* end) 
		:ConstSubImageIterator<T>(end)
		{ }

		SubImageIterator()
		{}

		typedef T* pointer;
		typedef T& reference;

		T* operator->() { return ConstSubImageIterator<T>::ptr; }
		T& operator*() { return *ConstSubImageIterator<T>::ptr;}
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


template<class T> class ConstSubImageIteratorEnd
{
	public:
		ConstSubImageIteratorEnd(const SubImage<T>* p)
		:i(p){}

		operator ConstSubImageIterator<T>()
		{
			return i->end();
		}

	private:
		const SubImage<T>* i;
};


/// A generic image class to manage a block of arbitrarily padded data as an image. Provides
/// basic image access such as accessing a particular pixel co-ordinate. 
/// @param T The pixel type for this image. Typically either
/// <code>CVD::byte</code> or <code>CVD::Rgb<CVD::byte> ></code> are used,
/// but images could be constructed of any available type.
/// 
/// A BasicImage does not manage its own data, but provides access to an 
/// arbitrary externally-managed block of data as though it were an image. Use
/// the derived Image class if you want an image which also has its own data.
/// @ingroup gImage
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

		/// Is this pixel co-ordinate inside the image, and not too close to the edges?
		/// @param ir The co-ordinate to test
		/// @param border The size of the border: positive points inside the image.
		bool in_image_with_border(const ImageRef& ir, int border) const
		{
			return ir.x >=border && ir.y >=border && ir.x < my_size.x - border && ir.y < my_size.y - border;
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
            return my_data+row*my_stride;
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
		typedef ConstSubImageIterator<T> const_iterator;
		
		/// Returns an iterator referencing the first (top-left) pixel in the image
		inline iterator begin()
		{
			return SubImageIterator<T>(data(), size().x, my_stride, end_ptr());
		}
		/// Returns a const iterator referencing the first (top-left) pixel in the image
		inline const_iterator begin() const
		{
			return ConstSubImageIterator<T>(data(), size().x, my_stride, end_ptr());
		}

		/// Returns an iterator pointing to one past the end of the image
		inline iterator end()
		{
			//Operator [] would always throw here!
			return SubImageIterator<T>(end_ptr());
		}
		/// Returns a const iterator pointing to one past the end of the image
		inline const_iterator end() const
		{
			//Operator [] would always throw here!
			return ConstSubImageIterator<T>(end_ptr());
		}

		/// Returns an object corresponding to end(), which should eliminate a test.
		inline SubImageIteratorEnd<T> fastend()
		{
			return SubImageIteratorEnd<T>(this);
		}
		/// Returns an object corresponding to end() const, which should eliminate a test.
		inline ConstSubImageIteratorEnd<T> fastend() const
		{
			return ConstSubImageIteratorEnd<T>(this);
		}

		inline void copy_from( const SubImage<T> & other ){
			CVD_IMAGE_ASSERT(other.size() == this->size(), Exceptions::Image::IncompatibleImageSizes);
			std::copy(other.begin(), other.end(), this->begin());
		}

		/// What is the size of this image?
		inline ImageRef size() const
		{
			return my_size;
		}

		/// What is the row stride of the image?
		inline int row_stride() const
		{
			return my_stride;
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
			for(int y=0; y < my_size.y; y++)
				ImageUtil::memfill( (*this)[y], my_size.x, d);
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
			CVD_IMAGE_ASSERT(in_image(start), ImageError::AccessOutsideImage);
			CVD_IMAGE_ASSERT(in_image(start + size - ImageRef(1,1)), ImageError::AccessOutsideImage);
			return SubImage( &operator[](start), size, my_stride);
		}

		/// Return const a sub image
		/// @param start Top left pixel of the sub image
		/// @param size width and  height of the sub image
		const SubImage sub_image(const ImageRef& start, const ImageRef& size) const
		{	
			CVD_IMAGE_ASSERT(in_image(start), ImageError::AccessOutsideImage);
			CVD_IMAGE_ASSERT(in_image(start + size - ImageRef(1,1)), ImageError::AccessOutsideImage);

			T*ptr = my_data + start.y * my_stride + start.x;
			return SubImage(ptr, size, my_stride);
		}

		/// Return a reference to a SubImage. Useful for passing anonymous SubImages to functions.
		SubImage& ref()
		{
			return *this;
		}

	protected:
		T* my_data;       ///< The raw image data
		ImageRef my_size; ///< The size of the image
		int my_stride;    ///< The row stride
		
		///Return an off-the-end pointer without ever throwing AccessOutsideImage
		T* end_ptr() { return my_data+my_size.y*my_stride; }

		///Return an off-the-end pointer without ever throwing AccessOutsideImage
		const T* end_ptr() const { return my_data+my_size.y*my_stride; }

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
		:SubImage<T>(copyof)
		{
		}

		void operator=(const BasicImage&copyof)
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
};


/** An input iterator which just returns N copies of the same
    value over and over again. This can be used for construction 
    of containers of images. For intstance the code:
	@code
	vector<Image<float> > foo(3, ImageRef(10,10));
	@endcode
	All elements of <code>foo</code>  point to the same 10x10 image, which is
	probably not the desired behaviour. The desired behaviour can be obtained with 
	the ImageCreationIterator:
	@code
	ImageCreationIterator<ImageRef> begin(10,10), end(3);
	vector<Image<float> > foo(begin, end);
	@endcode
	In this case, <code>foo</code> contains 3 distinct images. 
	
	See also
	  - @ref CreateImagesBegin
	  - @ref CreateImagesEnd
	  - @ref Image<T>::Image(ImageRef)
	  - @ref Image<T>::Image(std::pair<ImageRef, T> ) 
	  - @ref Image<T>::copy_from_me()
**/
 
template<class T> class ImageCreationIterator: public std::iterator<std::input_iterator_tag, T, ptrdiff_t>
{
	public:
		void operator++(int) { num++; }
		void operator++() { num++; }
		bool operator==(const ImageCreationIterator& i){return num == i.num;}
		bool operator!=(const ImageCreationIterator& i){return num != i.num;}
		
		const T& operator*() { return *construct_from_me; }
		
		ImageCreationIterator(const T& data)
		:construct_from_me(&data),num(0){}

		ImageCreationIterator(int i)
		:construct_from_me(0),num(i){}

	private:
		const T* construct_from_me;
		int num;
};


/// Shortcut function for creating an iterator from a bit of data..
/// @param from_me Data to construct from
template<class C> inline ImageCreationIterator<C> CreateImagesBegin(const C& from_me)
{
	return ImageCreationIterator<C>(from_me);
}
/// Shortcut to create an end iterator.
/// The first parameter is used to get the type correct.
/// @param i Number of copies to make
template<class C> inline ImageCreationIterator<C> CreateImagesEnd(const C&, int i)
{
	return ImageCreationIterator<C>(i);
}

/// Shortcut to create an end iterator.
/// @param i Number of copies to make
template<class C> inline ImageCreationIterator<C> CreateImagesEnd(int i)
{
	return ImageCreationIterator<C>(i);
}

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
	private:
		struct CopyPlaceHolder
		{
			const Image* im;
		};

	public:
		///Copy constructor. This does not copy the data, it just creates a new
		///reference to the image data
		///@param copy The image to copy
		Image(const Image& copy) :
			BasicImage<T>(copy)
		{
			dup_from(&copy);
		}


		/**CopyFrom" constructor. If constructed from this, it creates
		   a new copy of the data.  This provides symmetry with @copy_from
		   @ref copy_from_me
		   @param c The (placeholder) image to copy from.
		**/
		Image(const CopyPlaceHolder& c)
		{
			dup_from(NULL);
			copy_from(*(c.im));
		}
		
		///This returns a place holder from which an image can be constructed.
		///On construction, a new copy of the data is made.
		CopyPlaceHolder copy_from_me() const
		{	
			CopyPlaceHolder c = {this};
			return c;
		}


		///Make a (new) copy of the image, also making a copy of the data
		///@param copy The image to copy
		void copy_from(const BasicImage<T>& copy)
		{
            resize(copy.size());
            std::copy(copy.begin(), copy.end(), this->begin());
		}




		///Make a (new) copy of the image, also making a copy of the data
		///@param copy The image to copy
		void copy_from(const SubImage<T>& copy)
		{
			Image<T> tmp(copy.size());
			*this = tmp;
			
			// FIXME: this is currently slow. Use fastend().
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
		
		#ifndef DOXYGEN_IGNORE_INTERNAL
		template<class C> const Image& operator=(Internal::ImagePromise<C> p)
		{
			p.execute(*this);
			return *this;
		}

		template<class C> Image(Internal::ImagePromise<C> p)
		{
			dup_from(NULL);
			p.execute(*this);
		}
		#endif
		
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
            this->my_data = Internal::aligned_alloc<T>(this->totalsize(), 16);
		}

		///Create a filled image of a given size
		///@param size The size of image to create
		///@param val  The value to fill the image with
		Image(const ImageRef& size, const T& val)
		{
			Image<T> tmp(size);
			tmp.fill(val);
			dup_from(&tmp);
		}

		///Create a filled image of a given size
		///@param p std::pair<ImageRef, T> containing the size and fill value.
		///Useful for creating containers of images with ImageCreationIterator
		Image(const std::pair<ImageRef, T>& p)
		{
			Image<T> tmp(p.first);
			tmp.fill(p.second);
			dup_from(&tmp);
		}

		///Resize the image (destroying the data).
		///This does not affect any other images pointing to this data.
		///@param size The new size of the image
		void resize(const ImageRef& size)
		{	
			if(size != BasicImage<T>::my_size || *num_copies > 1)
			{
				Image<T> new_im(size);
				*this = new_im;
			}
		}

		///Resize the image (destroying the data). 
		///This does not affect any other images pointing to this data.
		//The resized image is filled with val.
		///@param size The new size of the image
		///@param val  The value to fill the image with
		void resize(const ImageRef& size, const T& val)
		{
			if(*num_copies > 1 || size != BasicImage<T>::my_size)
			{
				Image<T> new_im(size, val);
				*this = new_im;
			}
				else fill(val);
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
                Internal::aligned_free<T>(this->my_data, this->totalsize());
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

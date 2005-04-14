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

namespace ImageError
{
	class AccessOutsideImage{};
	class NoImage{};
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

template<class T> class BasicImage
{
	public:

		BasicImage(T* data, const ImageRef& i)
		:my_data(data),my_size(i)
		{
		}

		BasicImage(const BasicImage& copyof)
		{
			my_size = copyof.my_size;
			my_data = copyof.my_data;
		}

		bool in_image(const ImageRef& r) const
		{
			return r.x >=0 && r.y >=0 && r.x < my_size.x && r.y < my_size.y;
		}

		~BasicImage()
		{}

		inline T& operator[](const ImageRef& pos)
		{
			IMAGE_ASSERT(in_image(pos), ImageError::AccessOutsideImage);
			return (my_data[pos.y*my_size.x + pos.x]);
		}
		
		inline const T& operator[](const ImageRef& pos) const 
		{
			IMAGE_ASSERT(in_image(pos), ImageError::AccessOutsideImage);
			return (my_data[pos.y*my_size.x + pos.x]);
		}


		inline const T* data() const
		{
			return my_data;
		}
		
		inline T* data()
		{
			return my_data;
		}

		inline ImageRef size() const
		{
			return my_size;
		}

		inline int totalsize() const
		{
			return my_size.x * my_size.y;
		}

		inline void zero() 
		{
			memset(my_data, 0, totalsize()*sizeof(T));
		}

		inline void fill(const T d)
		{
			ImageUtil::memfill(my_data, totalsize(), d);
		}


	protected:
		BasicImage()
		{}

		T* my_data;
		ImageRef my_size;

	private:
		void operator=(const BasicImage&);

};


//This is the full image, where data is owned by the Image system.
//These have 12 bytes of imformation locally and implement copy-on-write, so 
//they can be efficiently passed back from functions, put in vectors, etc.
//
//Since the images implement copy on write, each write operation has to be 
//checked, which could be slow. Image provides a cast to NonConstImage which
//does not do any checking of this sort on writes, but care must be taken:
//
//Since multiple images can point to the same set of data, care has to be taken
//with references to the image data. For instance if a pointer or NonConstImage
//is made to refer to the data, and *then* another Image is made to reference
//the data as well, using the pointer or the NonConstImage for writing will
//alter *both* images.
//
//If this situation is encountered, image1=image2.force_copy(); can be used
//to force a copy operation, so that image1 and image2 do not reference the 
//same chunk of data.
//
//These manage their own memory, so the last one referencing a blobk of memory
//will free it.

template<class T> class Image: public BasicImage<T>
{
	public:

		//Allow copy constructing
		Image(const Image& copy) :
			BasicImage<T>(copy)
		{
			dup_from(&copy);
		}


		void copy_from(const BasicImage<T>& copy)
		{
			Image<T> tmp(copy.size());

			*this = tmp;

			memcpy(my_data, copy.data(), totalsize()*sizeof(T));
		}

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
		//Allow assignment
		const Image& operator=(const Image& copyof)
		{
			remove();
			dup_from(&copyof);
			return *this;
		}
		
		//Basic constructing
		Image()
		{
			dup_from(NULL);
		}

		Image(const ImageRef& ir)
		{
			num_copies = new int;
			*num_copies = 1;
			my_size = ir;
			my_data  = new T[totalsize()];
		}
		
		//Resizing (destroys data)
		void resize(const ImageRef& ir)
		{
			Image<T> new_im(ir);
			*this = new_im;
		}

		//Destruction
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


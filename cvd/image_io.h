#ifndef CVD_IMAGE_IO_H
#define CVD_IMAGE_IO_H

#include <cvd/exceptions.h>
#include <cvd/image_convert.h>
#include <cvd/internal/load_and_save.h>
#include <cvd/internal/name_builtin_types.h>
#include <cvd/internal/name_CVD_rgb_types.h>
#include <memory>

namespace CVD
{
	namespace Exceptions
	{
		namespace Image_IO
		{
			struct All: public CVD::Exceptions::All
			{};

			struct UnsupportedImageType: public All
			{
				UnsupportedImageType();
			};

			struct EofBeforeImage: public All
			{
				EofBeforeImage();
			};

			struct MalformedImage: public All
			{
				MalformedImage(const std::string &);
			};

			struct ImageSizeMismatch: public All
			{
				ImageSizeMismatch(const ImageRef& src, const ImageRef& dest);
			};

			struct WriteError: public All
			{
				WriteError(const std::string& err);
			};

			struct UnseekableIstream: public All
			{
				UnseekableIstream(const std::string& type);
			};

			struct ReadTypeMismatch: public All
			{
				ReadTypeMismatch(bool read8);
			};
			
			struct InternalLibraryError: public All
			{
				InternalLibraryError(const std::string& lib, const std::string err);
			};

			struct UnsupportedImageSubType: public All
			{
				UnsupportedImageSubType(const std::string &, const std::string&);
			};

		}
	}


	////////////////////////////////////////////////////////////////////////////////
	//
	// Image loading
	//

	template<class I> void img_load(Image<I>& im, std::istream& i)
	{
		img_load(im, i, Pixel::CIE);
	}

	template<class I> void img_load(BasicImage<I>& im, std::istream& i)
	{
		img_load(im, i, Pixel::CIE);
	}

	template<class PixelType, class Conversion> void img_load(Image<PixelType>&im, std::istream& i, Conversion cv)
	{
		//Open an image for reading (put it in an exfeption safe container)
		std::auto_ptr<Image_IO::image_in> in(Image_IO::image_factory::in(i));
		
		//Set up the image to be the correct size (and thereby disown any data as well)
		ImageRef size((int)(in->x_size()), (int)(in->y_size()));
		im.resize(size);

		Internal::load_image(*in, im.data(), cv);
	}

	template<class PixelType, class Conversion> void img_load(BasicImage<PixelType>&im, std::istream& i, Conversion cv)
	{
		//Open an image for reading (put it in an exfeption safe container)
		std::auto_ptr<Image_IO::image_in> in(Image_IO::image_factory::in(i));

		ImageRef size((int)(in->x_size()), (int)(in->y_size()));
		
		//Check image size
		if(size != im.size())
			throw CVD::Exceptions::Image_IO::ImageSizeMismatch(im.size(), size);

		Internal::load_image(*in, im.data(), cv);
	}


	////////////////////////////////////////////////////////////////////////////////
	//
	// Image saving
	//
	namespace Internal
	{
		template<class C, int i> struct save_default_
		{
			static const bool use_16bit=1;
		};

		template<class C> struct save_default_<C,1>
		{
			static const bool use_16bit=Pixel::traits<typename Pixel::Component<C>::type>::bits_used > 8;
		};

		template<class C> struct save_default
		{
			static const bool use_16bit = save_default_<C, Pixel::traits<typename Pixel::Component<C>::type>::integral>::use_16bit;
		};
	}
	




	template<class PixelType, class Conversion> void img_save(const BasicImage<PixelType>& im, std::ostream& o, ImageType::ImageType t, Conversion& cv, int channels, bool use_16bit)
	{
		std::string comments;
		
		//I tmp;
		comments = "Saving image from pixel type of " + PNM::type_name<PixelType>::name();

		std::auto_ptr<Image_IO::image_out> out(Image_IO::image_factory::out(o, im.size().x, im.size().y, t, channels, use_16bit, comments));

		Internal::save_image(*out, im.data(), cv);
	}


	template<class PixelType, class Conversion> void img_save(const BasicImage<PixelType>& im, std::ostream& o, ImageType::ImageType t, Conversion& cv)
	{
		img_save(im, o, t, cv, Pixel::Component<PixelType>::count, Internal::save_default<PixelType>::use_16bit);
	}


	template<class PixelType> void img_save(const BasicImage<PixelType>& im, std::ostream& o, ImageType::ImageType t)
	{
		img_save(im, o, t, Pixel::CIE);
	}




	////////////////////////////////////////////////////////////////////////////////
	//
	// Legacy pnm_* functions
	//

	template<class PixelType> void pnm_save(const BasicImage<PixelType>& im, std::ostream& o)
	{
		img_save(im, o, ImageType::PNM);
	}

	template<class PixelType> void pnm_load(BasicImage<PixelType>& im, std::istream& i)
	{
		img_load(im, i);
	}

	template<class PixelType> void pnm_load(Image<PixelType>& im, std::istream& i)
	{
		img_load(im, i);
	}

}
//#include <cvd/internal/pnm/pnm_cvd_image.h>
//#include <cvd/internal/pnm/cvd_rgb.h>



#endif

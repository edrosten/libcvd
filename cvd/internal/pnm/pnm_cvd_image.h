#ifndef CVD_PNM_IMAGE_H
#define CVD_PNM_IMAGE_H

#include <iostream>
#include <limits>
#include <memory>

//We are included from:
//#include <cvd/image_io.h>
#include <cvd/image.h>
#include <cvd/exceptions.h>
#include <cvd/internal/disk_image.h>
#include <cvd/internal/pnm/load_and_save.h>
#include <cvd/internal/pnm/convert_builtin_types.h>

namespace CVD
{

	namespace PNM
	{
		//Best maxval. Overloaded in cvd_rgb.h where this sort of thing is dealt with
		template<class C> struct best_length { static const int val = 1;};
		template<> struct best_length<unsigned char>{static const int val=0;};
		template<> struct best_length<char>{static const int val=0;};
	}

	template<class I> void img_load(Image<I>& im, std::istream& i)
	{
		PNM::convert::weighted_rgb<I> conv(PNM::convert::cie);	
		img_load(im, i, conv);
	}

	template<class I> void img_load(BasicImage<I>& im, std::istream& i)
	{
		PNM::convert::weighted_rgb<I> conv(PNM::convert::cie);	
		img_load(im, i, conv);
	}

	template<class I, class C> void img_load(BasicImage<I>& im, std::istream& i, C cv)
	{
		
		//Open a PNM for reading	
		std::auto_ptr<Image_IO::image_in> in(Image_IO::image_factory::in(i));

		//Set up the CVD::image
		ImageRef size((int)(in->x_size()), (int)(in->y_size()));

		if(size != im.size())
			throw CVD::Exceptions::Image_IO::ImageSizeMismatch(im.size(), size);

		PNM::load_image(*in, im.data(),  cv);
	}
	
	template<class I, class C> void img_load(Image<I>& im, std::istream& i, C cv)
	{
		//Open a PNM for reading	
		std::auto_ptr<Image_IO::image_in> in(Image_IO::image_factory::in(i));

		//Set up the CVD::image
		ImageRef size((int)(in->x_size()), (int)(in->y_size()));
		im.resize(size);

		PNM::load_image(*in, im.data(),  cv);
	}

	template<class I> void img_save(const BasicImage<I>& im, std::ostream& o,  ImageType::ImageType t, int rgb = -1, bool use_2_bytes = PNM::best_length<I>::val)
	{
		PNM::convert::weighted_rgb<I> conv(PNM::convert::cie);	
		img_save(im, o, t, rgb, use_2_bytes, conv);
	}

	template<class I, class C> void img_save(const BasicImage<I>& im, std::ostream& o, ImageType::ImageType t, int rgb,  bool use_2_bytes, C cv)
	{
		//If the type is unspecified, then autodetect it.
		if(rgb == -1)
		{
			//This class is only specialized for the builtin integral types,
			//so a PGM can represent these. Otherwise, the type is probably
			//a more complex type which is best saved as an PPM instead.
			if(std::numeric_limits<I>::is_specialized == true)
				rgb = 0;
			else
				rgb = 1;
		}

		std::string comments;
		
		//I tmp;
		comments = "Saving image from pixel type of " + PNM::type_name<I>::name();

		std::auto_ptr<Image_IO::image_out> out(Image_IO::image_factory::out(o, im.size().x, im.size().y, t, rgb, use_2_bytes, comments));

		PNM::save_image(*out, im.data(), cv);
	}

	//Give pnm_* for compatibility


	template<class I> void pnm_load(Image<I>& im, std::istream& i)
	{
		img_load(im, i);
	}

	template<class I> void pnm_load(BasicImage<I>& im, std::istream& i)
	{
		img_load(im, i);
	}

	template<class I, class C> void pnm_load(BasicImage<I>& im, std::istream& i, C cv)
	{
		img_load(im, i, cv);	
	}
	
	template<class I, class C> void pnm_load(Image<I>& im, std::istream& i, C cv)
	{
		img_load(im, i, cv);
	}

	template<class I> void pnm_save(const BasicImage<I>& im, std::ostream& o, int rgb = -1, bool use_2_bytes = PNM::best_length<I>::val)
	{
		img_save(im, o, ImageType::PNM, rgb, use_2_bytes);
	}

	template<class I, class C> void pnm_save(const BasicImage<I>& im, std::ostream& o, int rgb,  bool use_2_bytes, C cv)
	{
		img_save(im, o, ImageType::PNM, rgb, use_2_bytes, cv);
	}
}

#endif

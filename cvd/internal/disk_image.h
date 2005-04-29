#ifndef DISK_IMAGE_IN_H
#define DISK_IMAGE_IN_H

#include <iostream>
#include <string>

#include <cvd/config.h>

namespace CVD
{	
	namespace ImageType
	{
		enum ImageType
		{
			PNM,
			CVD_IMAGE_HAVE_JPEG  //This is a macro, ending in ,
		};
	}

	namespace Image_IO
	{

		class image_base
		{
			public:
				bool is_2_byte()const {return m_is_2_byte;}
				int channels(){return m_channels;}
				//bool is_rgb() const {return m_is_rgb;}
				long  x_size() const {return xs;}
				long  y_size() const {return ys;}
				long  elements_per_line() const {return xs * m_channels;}
				
			protected:
				long	xs, ys;
				bool 	m_is_2_byte;
				int		m_channels;
		};

		class image_in : public image_base
		{
			public:
				virtual void get_raw_pixel_lines(unsigned char*, unsigned long nlines)=0;
				virtual void get_raw_pixel_lines(unsigned short*, unsigned long nlines)=0;

				virtual ~image_in();

		};

		class image_out: public image_base
		{
			public:
				virtual void write_raw_pixel_lines(const unsigned char*, unsigned long nlines)=0;
				virtual void write_raw_pixel_lines(const unsigned short*, unsigned long nlines)=0;
				virtual ~image_out();
		};
		
		class image_factory
		{
			public:
				static image_in* in(std::istream&);
				static image_out* out(std::ostream&, long xsize, long ysize, ImageType::ImageType type,
									 int try_channels, bool try_2byte, const std::string& c);
		};
	}

}




#endif

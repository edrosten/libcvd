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
#ifndef PNM_GROK_H
#define PNM_GROK_H

#include <iostream>
#include <string>
#include <vector> 
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/image.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
  namespace PNM
  {
    class pnm_in
    {
		public:
		  pnm_in(std::istream&);
		  bool is_2_byte()const {return m_is_2_byte;}
		  int channels(){return m_channels;}
		  long  x_size() const {return xs;}
		  long  y_size() const {return ys;}
		  long  elements_per_line() const {return xs * m_channels;}
		  void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
		  void get_raw_pixel_lines(unsigned short*, unsigned long nlines);

				
		private:
		  std::istream&	i;
		  bool 	is_text;
		  int   type, maxval;
		  int   lines_so_far;
		  void	read_header();
		  bool  can_proc_lines(unsigned long);
		  long	xs, ys;
		  bool 	m_is_2_byte;
		  int	m_channels;
    };

	template <class T, class S, int N> struct PNMReader;

	template <class T, class S> struct PNMReader<T,S,3> 
	{
		typedef Rgb<S> array;
		static void readPixels(BasicImage<T>& im, pnm_in& pnm) 
		{
 			std::vector<array> rowbuf(pnm.x_size());
			for (int r=0; r<pnm.y_size(); r++) 
			{
	  			pnm.get_raw_pixel_lines((S*) &(rowbuf[0]), 1);
	  			Pixel::ConvertPixels<array, T>::convert(&(rowbuf[0]), im[r], pnm.x_size());
			}
		}
	};
  
    template <class T, class S> struct PNMReader<T,S,1> 
	{
      	static void readPixels(BasicImage<T>& im, pnm_in& pnm) 
		{
			std::vector<S> rowbuf(pnm.x_size());
			for (int r=0; r<pnm.y_size(); r++) 
			{
	  			pnm.get_raw_pixel_lines(&(rowbuf[0]), 1);
	  			Pixel::ConvertPixels<S, T>::convert(&(rowbuf[0]), im[r], pnm.x_size());
			}
      	}
    };

    template <> struct PNMReader<Rgb<byte>,byte,3> 
	{
      	static void readPixels(BasicImage<Rgb<byte> >& im, pnm_in& pnm) 
		{
			pnm.get_raw_pixel_lines((byte*)im.data(), pnm.y_size());
      	}
    };

    template <> struct PNMReader<byte,byte,1> 
	{
      	static void readPixels(BasicImage<byte>& im, pnm_in& pnm) 
		{
			pnm.get_raw_pixel_lines(im.data(), pnm.y_size());
      	}
    };

    template <> struct PNMReader<Rgb<unsigned short>,unsigned short,3> 
	{
      	static void readPixels(BasicImage<Rgb<unsigned short> >& im, pnm_in& pnm) 
		{
			pnm.get_raw_pixel_lines((unsigned short*)im.data(), pnm.y_size());
      	}
    };

    template <> struct PNMReader<unsigned short,unsigned short,1> 
	{
      	static void readPixels(BasicImage<unsigned short>& im, pnm_in& pnm) 
		{
			pnm.get_raw_pixel_lines(im.data(), pnm.y_size());
      	}
    };
  
    template <class T> void readPNM(BasicImage<T>& im, pnm_in& pnm)
    {
      	if (pnm.is_2_byte()) 
	  	{
			if (pnm.channels() == 3)
	  			PNMReader<T,unsigned short,3>::readPixels(im, pnm);
			else 
	  			PNMReader<T,unsigned short,1>::readPixels(im, pnm);
		}
      	else 
		{
			if (pnm.channels() == 3)
	  			PNMReader<T,unsigned char,3>::readPixels(im, pnm);
			else
				PNMReader<T,unsigned char,1>::readPixels(im, pnm);
      	}
    }
	
	template <class T> void readPNM(BasicImage<T>&im, std::istream& in)
	{
      pnm_in pnm(in);
	  ImageRef size(pnm.x_size(), pnm.y_size());

	  if(size != im.size())
	    throw Exceptions::Image_IO::ImageSizeMismatch(size, im.size());
	
		readPNM(im, pnm);

	}

	template <class T> void readPNM(Image<T>&im, std::istream& in)
	{
      	pnm_in pnm(in);
      	im.resize(ImageRef(pnm.x_size(), pnm.y_size()));
	  	readPNM(im, pnm);
	}
	////////////////////////////////////////////////////////////////////////////////
	//
	// PNM writing.
	//

	template<int isRgb, int isbyte> struct ComponentMapper      { typedef Rgb<byte> type; };
	template<>                      struct ComponentMapper<1,0> { typedef Rgb<unsigned short> type; };
	template<>                      struct ComponentMapper<0,1> { typedef byte type; };
	template<>                      struct ComponentMapper<0,0> { typedef unsigned short type; };

	class pnm_writer
	{
		public:
			pnm_writer(std::ostream&, ImageRef size, const std::string& type);
			~pnm_writer();

			//void write_raw_pixel_line(const bool*);
			void write_raw_pixel_line(const unsigned char*);
			void write_raw_pixel_line(const unsigned short*);
			void write_raw_pixel_line(const Rgb<unsigned char>*);
			void write_raw_pixel_line(const Rgb<unsigned short>*);

			template<class Incoming> struct Outgoing
			{		 
				typedef typename Pixel::Component<Incoming>::type Element;
				typedef typename ComponentMapper<Pixel::is_Rgb<Incoming>::value,
												 std::numeric_limits<Element>::is_integer &&
												 std::numeric_limits<Element>::digits <= 8>::type type;
			};		
		private:

			template<class P> void sanity_check(const P*);
			void write_shorts(const unsigned short*, int n);

		long row;
		std::ostream& o;
		ImageRef size;
		std::string type;
	};


  }
}
#endif

#ifndef CVD_RGB_TRAITS_H
#define CVD_RGB_TRAITS_H

#include <cvd/rgb.h>
#include <cvd/rgba.h>
#include <cvd/rgb8.h>

namespace CVD
{
	namespace Pixel
	{
		
		template<class P> struct Component<Rgb<P> >
		{
			typedef P type;
			static const unsigned int count = 3;

			
			//This version is much faster, with -funroll-loops
			static const P& get(const Rgb<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<const P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}

			static P& get(Rgb<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<P*>(&pixel)+i);
				// return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}
		};

		template<> struct Component<Rgb8>
		{
			typedef unsigned char type;
			static const unsigned int count = 3;

			static const type& get(const Rgb8& pixel, unsigned int i)
			{
				return *(reinterpret_cast<const unsigned char*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}

			static type& get(Rgb8& pixel, unsigned int i)
			{
				return *(reinterpret_cast<unsigned char*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : pixel.blue);
			}
		};

		template<class P> struct Component<Rgba<P> >
		{
			typedef P type;
			static const unsigned int count = 4;

			static const P& get(const Rgba<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<const P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : (i==2 ?pixel.blue: pixel.alpha));
			}

			static P& get(Rgba<P>& pixel, unsigned int i)
			{
				return *(reinterpret_cast<P*>(&pixel)+i);
				//return i == 0 ? pixel.red : (i==1 ? pixel.green : (i==2 ?pixel.blue: pixel.alpha));
			}
		};

		template<class P, int I> struct Component<P[I]>
		{
			typedef P type;
			static const unsigned int count=I;

			static const P& get(const P pixel[i], unsigned int i)
			{
				return pixel[i];
			}

			static  P& get(P pixel[i], unsigned int i)
			{
				return pixel[i];
			}
		};

	}
}
#endif

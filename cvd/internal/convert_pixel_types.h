#ifndef CVD_CONVERT_PIXEL_TYPES_H
#define CVD_CONVERT_PIXEL_TYPES_H

#include <math.h>

#include <cvd/abs.h>
#include <cvd/internal/scalar_convert.h>
#include <cvd/internal/builtin_components.h>


namespace CVD{namespace Pixel
{
	//All "conversion" classes must have a "void convert_pixel(from, to)"
	//nonstatic templated member.

	//This conversion knows how to convert from any size pixel to any other.
	//The conversion is defined for all combinations of Y, RGB, RGBA
	//Anything else is unknown. 
	//The pix_convert_lum function is used to transcribe between different
	//data types
	template<template<class,class>class ScalarConvert=scalar_convert> struct BasicConversion
	{
		template<class From, class To> void convert_pixel(const From& fr, To& to) const
		{
			c<From, Component<From>::count, To, Component<To>::count>::conv(fr, to, *this);
		}

		public:

		//Don't know how to do a generic conversion
		template<class From, int FCopms, class To, int TComps> struct c
		{
			static void conv(const From& fr, To& to, const BasicConversion&)
			{
				I_dont_know_how_to_perform_this_conversion;
			}
		};	

		//The number of components is the same, so this is simple
		template<class From, int Comps, class To> struct c<From, Comps, To, Comps>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{	
				for(unsigned int i=0; i < Comps; i++)
				{
					c.convert_pixel(Component<From>::get(fr, i), Component<To>::get(to, i));
				}
			}
		};

		//Ah, we know how to do this one!
		template<class From, class To> struct c<From, 1, To, 1>
		{
			static void conv(const From& fr, To& to, const BasicConversion&)
			{
				to = ScalarConvert<To, From>::from(fr);			
			}
		};
		
		//Special cases which we know about

		////////////////////////////////////////////////////////////////////////////////
		//
		// Convery grey to X

		//Grey to rgb
		template<class From, class To> struct c<From, 1, To, 3>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				typename Component<To>::type  tmp;
				c.convert_pixel(fr, tmp);

				Component<To>::get(to, 0) = tmp;
				Component<To>::get(to, 1) = tmp;
				Component<To>::get(to, 2) = tmp;
			}
		};


		//Grey to rgba
		template<class From, class To> struct c<From, 1, To, 4>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				typename Component<To>::type  tmp;
				c.convert_pixel(fr, tmp);

				Component<To>::get(to, 0) = tmp;
				Component<To>::get(to, 1) = tmp;
				Component<To>::get(to, 2) = tmp;
				Component<To>::get(to, 3) = 0; //Opaque
			}
		};
		
		//Grey to grey+alpha
		template<class From, class To> struct c<From, 1, To, 2>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(fr, Component<To>::get(to,0));
				Component<To>::get(to, 1) = 0; //Opaque
			}

		};


		////////////////////////////////////////////////////////////////////////////////
		//
		//  Gonvert grey+alpha to X

		//From GA to G: ignore alpha
		template<class From, class To> struct c<From, 2, To, 1>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(Component<From>::get(fr, 1), to);
			}
		};
		
		//From GA to rgb: duplicate RGA, ignore A
		template<class From, class To> struct c<From, 2, To, 3>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				//Copy Y to r,g,b
				typename Component<To>::type  tmp;
				c.convert_pixel(Component<From>::get(fr, 0), tmp);

				Component<To>::get(to, 0) = tmp;
				Component<To>::get(to, 1) = tmp;
				Component<To>::get(to, 2) = tmp;
			}
		};
		//From GA to rgba: duplicate RGB, copy A
		template<class From, class To> struct c<From, 2, To, 4>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				//Copy Y to r,g,b
				typename Component<To>::type  tmp;
				c.convert_pixel(Component<From>::get(fr, 0), tmp);

				Component<To>::get(to, 0) = tmp;
				Component<To>::get(to, 1) = tmp;
				Component<To>::get(to, 2) = tmp;

				//Copy alpha
				c.convert_pixel(Component<From>::get(fr, 1), Component<To>::get(to,3));
			}
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		// Convert rgb to X

		
		//rgb to grey: pick green
		template<class From, class To> struct c<From, 3, To, 1>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(Component<From>::get(fr, 1), to);
			}
		};

		//rgb to GA: pick green and make opaque
		template<class From, class To> struct c<From, 3, To, 2>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(Component<From>::get(fr, 1), Component<To>::get(to,0));
				Component<To>::get(to, 1) = 0; //Opaque
			}
		};

		//rgb to rgba: make the image opaque
		template<class From, class To> struct c<From, 3, To, 4>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(Component<From>::get(fr, 0), Component<To>::get(to, 0));
				c.convert_pixel(Component<From>::get(fr, 1), Component<To>::get(to, 1));
				c.convert_pixel(Component<From>::get(fr, 2), Component<To>::get(to, 2));
				Component<To>::get(to, 3) = 0;
			}
		};

		////////////////////////////////////////////////////////////////////////////////
		//
		// rgba to X
	
		
		//rgba to grey: pick green, ignore alpha
		template<class From, class To> struct c<From, 4, To, 1>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(Component<From>::get(fr, 1), to);
			}
		};

		//rgba to GA: pick green and alpha
		template<class From, class To> struct c<From, 4, To, 2>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(Component<From>::get(fr, 1), Component<To>::get(to,0));
				c.convert_pixel(Component<From>::get(fr, 3), Component<To>::get(to,1));
			}
		};

		//rgba to rgb: ignore alpha
		template<class From, class To> struct c<From, 4, To, 3>
		{
			static void conv(const From& fr, To& to, const BasicConversion& c)
			{
				c.convert_pixel(Component<From>::get(fr, 0), Component<To>::get(to, 0));
				c.convert_pixel(Component<From>::get(fr, 1), Component<To>::get(to, 1));
				c.convert_pixel(Component<From>::get(fr, 2), Component<To>::get(to, 2));
			}
		};
	};

	////////////////////////////////////////////////////////////////////////////////
	//
	// More complex conversions are defined below
	//
	template<template<class,class> class ScalarConvert=scalar_convert> struct L1Norm: public BasicConversion<ScalarConvert>
	{
		template<class From, class To> void convert_pixel(const From& fr, To& to) const
		{
			n<From, Component<From>::count, To, Component<To>::count>::conv(fr, to, *this);
		}

		template<class F, int Fc, class T, int Tc> struct n:public BasicConversion<ScalarConvert>::c<F,Fc,T,Tc>
		{};

		template<class F, class T> struct n<F, 1, T, 1>
		{
			static void conv(const F& fr, T& to, const L1Norm& c)
			{	
				c.BasicConversion<ScalarConvert>::convert_pixel(fr, to);
				to = abs(to);
			}
		};

		template<class F, int Fc, class T> struct n<F, Fc, T, 1>
		{
			static void conv(const F& fr, T& to, const L1Norm& c)
			{	
				T t;
				to=0;
				for(unsigned int i=0; i < Fc; i++)
				{
					c.convert_pixel(Component<F>::get(fr, i), t);
					to += abs(t);
				}
			}
		};
	};


	template<template<class,class> class ScalarConvert=scalar_convert> struct L2Norm: public BasicConversion<ScalarConvert>
	{
		template<class From, class To> void convert_pixel(const From& fr, To& to) const
		{
			n<From, Component<From>::count, To, Component<To>::count>::conv(fr, to, *this);
		}

		template<class F, int Fc, class T, int Tc> struct n:public BasicConversion<ScalarConvert>::c<F,Fc,T,Tc>
		{};

		template<class F, class T> struct n<F, 1, T, 1>
		{
			static void conv(const F& fr, T& to, const L2Norm& c)
			{	
				c.BasicConversion<ScalarConvert>::convert_pixel(fr, to);
				to = abs(to);
			}
		};
		

		template<class F, int Fc, class T> struct n<F, Fc, T, 1>
		{
			static void conv(const F& fr, T& to, const L2Norm& c)
			{	
				float t, ts=0;
				to=0;
				for(unsigned int i=0; i < Fc; i++)
				{
					c.convert_pixel(Component<F>::get(fr, i), t);
					ts += abs(t)*abs(t);
				}
				c.convert_pixel(sqrt(ts), to);
			}
		};
	};

	template<template<class,class> class ScalarConvert=scalar_convert> struct WeightedRGB: public BasicConversion<ScalarConvert>
	{
		WeightedRGB(float rr, float gg, float bb)
		:r(rr),g(gg),b(bb)
		{}

		template<class From, class To> void convert_pixel(const From& fr, To& to) const
		{
			n<From, Component<From>::count, To, Component<To>::count>::conv(fr, to, *this);
		}

		template<class F, int Fc, class T, int Tc> struct n:public BasicConversion<ScalarConvert>::c<F,Fc,T,Tc>
		{};
		
		//Convert rgb to y using weightings
		template<class F, class T> struct n<F, 3, T, 1>
		{
			static void conv(const F& fr, T& to, const WeightedRGB& c)
			{

				float rr, gg, bb;
				c.convert_pixel(Component<F>::get(fr, 0), rr);
				c.convert_pixel(Component<F>::get(fr, 1), gg);
				c.convert_pixel(Component<F>::get(fr, 2), bb);
				c.convert_pixel((rr*c.r+bb*c.b+gg*c.g), to);
			}
		};

		//Convert rgba to y using weightings, ignoring alpha
		template<class F, class T> struct n<F, 4, T, 1>
		{
			static void conv(const F& fr, T& to, const WeightedRGB& c)
			{

				float rr, gg, bb;
				c.convert_pixel(Component<F>::get(fr, 0), rr);
				c.convert_pixel(Component<F>::get(fr, 1), gg);
				c.convert_pixel(Component<F>::get(fr, 2), bb);
				c.convert_pixel((rr*c.r+bb*c.b+gg*c.g), to);
			}
		};

		private:
			float r, g, b;
	};

}}
#endif

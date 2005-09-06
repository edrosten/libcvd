#ifndef CVD_IMAGE_INTERPOLATE_H
#define CVD_IMAGE_INTERPOLATE_H

#include <TooN/TooN.h>
#include <cvd/internal/pixel_operations.h>
#include <math.h>

namespace CVD
{
	///Classes used to specify the interpolation type
	///for image_interpolate
	///@ingroup gImage
	namespace Interpolate
	{
		/// This does not interpolate: it uses the nearest neighbour.
		///
		/// The sub pixel to be accessed is \f$p = (x,y)\f$. The nearest pixel is
		/// \f$q = ( \operatorname{round}\ x, \operatorname{round}\ y)
		/// The interpolated value, \f$v\f$ is \f$v = I(q)\f$
		class NearestNeighbour{};


		/// This class is for bilinear interpolation.
		///
		/// Define \f$p' = ( \operatorname{floor}\ x, \operatorname{floor}\ y)\f$ and
		/// \f$\delta = p - p'\f$
		///
		/// 4 pixels in a square with \f$p'\f$ in the top left corner are taken:
		/// \f$a = I(p')\f$
		/// \f$b = I(p' + (1,0))\f$
		/// \f$c = I(p' + (0,1))\f$
		///	\f$d = I(p' + (1,1))\f$
		/// 
		/// \f$v = (1-\delta_y)((1-\delta_x)a + \delta_xb) + \delta_y((1-\delta_x)c + \delta_xd)\f$
		class Bilinear{};


		/// This class is for bicubic (not bicubic spline) interpolation.
		///
		/// \f$ v = \sum_{m=-1}^2\sum_{n=-1}^2 I(x' + m, y' + n)r(m - \delta_x)r(\delta_y-n) \f$
		///
		/// where:
		///
		/// r(x) = \frac{1}{6}\left[ p(x+2)^3 - 4p(x+1)^3 + 6p(x)^3 - 4p(x-1)^3 \right]\f$
		///
		/// \f$ p(x) = \left{ \begin{array}{cc}x&x>0\\0&x \le 0\end{array} \right}\f$
		///This algorithm is described in http://astronomy.swin.edu.au/~pbourke/colour/bicubic/
		class Bicubic{};
	};

	#ifdef DOXYGEN_INCLUDE_ONLY_FOR_DOCS
		///This is a generic interpolation class which wraps in image and provides
		///a similar interface for floating point pixel posisions.
		///@param I The interpolation type. See CVD::Interpolate for available types.
		///@param P The pixel type.
		///@ingroup gImage
		template<class I, class P> class image_interpolate
		{
			///Construct the class.
			///@param i The image to be interpolated.
			image_interpolate(const BasicImage<byte>& i);

			///Is this pixel inside the image?
			///@param pos The coordinate to test.
			bool in_image(const TooN::Vector<2>& pos);

			///Access the pixel at pos, with interpolation.
			///Bouds checking is tha same as for CVD::Image.
			///@param pos The pixel to access
			C operator[](const TooN::Vector<2>& pos);

			///Return the minimum value for which in_image returns true.
			TooN::Vector<2> min();
			///Return the first value for which in_image returns false.
			TooN::Vector<2> max();
		};
	#endif



	#ifndef DOXYGEN_IGNORE_INTERNAL

	template<class I, class C> class image_interpolate;

	//Zero order (nearest neighbour)

	template<class C> class image_interpolate<Interpolate::NearestNeighbour, C>
	{
		private:
			const BasicImage<C>& im;

			int round(double d)
			{
				if(d < 0)
					return (int)ceil(d - .5);
				else
					return (int)floor(d + .5);
			}

			ImageRef to_ir(const TooN::Vector<2>& v)
			{
				return ImageRef(round(v[0]), round(v[1]));
			}

	
		public:
			image_interpolate(const BasicImage<byte>& i)
			:im(i)
			{}

			bool in_image(const TooN::Vector<2>& pos)
			{
				return im.in_image(to_ir(pos));
			}

			C operator[](const TooN::Vector<2>& pos)
			{
				return im[to_ir(pos)];
			}

			TooN::Vector<2> min()
			{
				return (TooN::make_Vector, 0, 0);
			}

			TooN::Vector<2> max()
			{
				return ir(im.size());
			}

			
	};

	template<class T> class image_interpolate<Interpolate::Bilinear, T>
	{
		private:
			const BasicImage<T>& im;

			TooN::Vector<2> floor(const TooN::Vector<2>& v)
			{
				return (TooN::make_Vector, ::floor(v[0]), ::floor(v[1]));
			}

		public:
			image_interpolate(const BasicImage<byte>& i)
			:im(i)
			{}

			bool in_image(const TooN::Vector<2>& pos)
			{
				return im.in_image(ir(floor(pos)));
			}

			T operator[](const TooN::Vector<2>& pos)
			{
				TooN::Vector<2> delta =  pos - floor(pos);

				ImageRef p = ir(floor(pos));

				if(delta[0] == 0 && delta[1] == 0)
					return im[p];
				else
				{	
					double x = delta[0];
					double y = delta[1];

					T ret;

					for(unsigned int i=0; i < Pixel::Component<T>::count; i++)
					{
						float a, b, c, d;

						a = Pixel::Component<T>::get(im[p + ImageRef(0,0)], i);
						b = Pixel::Component<T>::get(im[p + ImageRef(1,0)], i);
						c = Pixel::Component<T>::get(im[p + ImageRef(0,1)], i);
						d = Pixel::Component<T>::get(im[p + ImageRef(1,1)], i);
						
						Pixel::Component<T>::get(ret, i) = (typename Pixel::Component<T>::type)  ((a*(1-x) + b*x)*(1-y) + (c*(1-x) + d*x)*y);
					}

					return ret;
				}
			}

			TooN::Vector<2> min()
			{
				return (TooN::make_Vector, 0, 0);
			}

			TooN::Vector<2> max()
			{
				return ir(im.size());
			}

	};


	template<class T> class image_interpolate<Interpolate::Bicubic, T>
	{
		private:
			const BasicImage<T>& im;

			float p(float f)
			{
				return f <0 ? 0 : f;
			}

			float r(float x)
			{
				return (  pow(p(x+2), 3) - 4 * pow(p(x+1),3) + 6 * pow(p(x), 3) - 4* pow(p(x-1),3))/6;
			}

		public:
			image_interpolate(const BasicImage<byte>& i)
			:im(i)
			{}

			bool in_image(const TooN::Vector<2>& pos)
			{
				return pos[0] >= 1 && pos[1] >=1 && pos[0] < im.size().x-2 && pos[1] < im.size().y - 2;
			}

			T operator[](const TooN::Vector<2>& pos)
			{
				int x = (int)floor(pos[0]);
				int y = (int)floor(pos[1]);
				float dx = pos[0] - x;
				float dy = pos[1] - y;

				//Algorithm as described in http://astronomy.swin.edu.au/~pbourke/colour/bicubic/
				T ret;
				for(unsigned int i=0; i < Pixel::Component<T>::count; i++)
				{
					float s=0;

					for(int m = -1; m < 3; m++)
						for(int n = -1; n < 3; n++)
							s += Pixel::Component<T>::get(im[y+n][x+m], i) * r(m - dx) * r(dy-n);
						
					Pixel::Component<T>::get(ret, i)= (typename Pixel::Component<T>::type) s;
				}

				return ret;
			}

			TooN::Vector<2> min()
			{
				return (TooN::make_Vector, 1, 1);
			}

			TooN::Vector<2> max()
			{
				return (TooN::make_Vector, im.size().x - 2, im.size().y - 2);
			}

	};
	#endif

}

#endif

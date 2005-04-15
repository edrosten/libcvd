#ifndef CVD_GL_HELPERS_H
#define CVD_GL_HELPERS_H


#include <cvd/image_ref.h>
#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>
#include <cvd/rgb8.h>
#include <cvd/rgba.h>
#include <numerics.h>
#include <GL/gl.h>

namespace CVD
{
	namespace gl
	{
		template<class C> struct data;

		//Scalar types

		template<> struct data<unsigned char>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_UNSIGNED_BYTE;
		};

		template<> struct data<signed char>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_BYTE;
		};
		
		template<> struct data<unsigned  short>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_UNSIGNED_SHORT;
		};

		template<> struct data<short>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_SHORT;
		};

		template<> struct data<unsigned int>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<int>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_INT;
		};
		
		template<> struct data<float>
		{
			static const int format=GL_LUMINANCE;
			static const int type  =GL_FLOAT;
		};
	

		//Rgb<*> types
	
		template<> struct data<Rgb<unsigned char> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_UNSIGNED_BYTE;
		};

		template<> struct data<Rgb<signed char> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_BYTE;
		};
		
		template<> struct data<Rgb<unsigned  short> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_UNSIGNED_SHORT;
		};

		template<> struct data<Rgb<short> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_SHORT;
		};

		template<> struct data<Rgb<unsigned int> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<Rgb<int> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_INT;
		};
		
		template<> struct data<Rgb<float> >
		{
			static const int format=GL_RGB;
			static const int type  =GL_FLOAT;
		};
		
		//Rgba<*> types
	
		template<> struct data<Rgba<unsigned char> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_BYTE;
		};

		template<> struct data<Rgba<signed char> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_BYTE;
		};
		
		template<> struct data<Rgba<unsigned  short> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_SHORT;
		};

		template<> struct data<Rgba<short> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_SHORT;
		};

		template<> struct data<Rgba<unsigned int> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_INT;
		};

		template<> struct data<Rgba<int> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_INT;
		};
		
		template<> struct data<Rgba<float> >
		{
			static const int format=GL_RGBA;
			static const int type  =GL_FLOAT;
		};
		//Rgb8 type

		template<> struct data<Rgb8>
		{
			static const int format=GL_RGBA;
			static const int type  =GL_UNSIGNED_BYTE;
		};
		
	};

	inline void glVertex(const ImageRef& i)
	{
		glVertex2i(i.x, i.y);
	}

	inline void glVertex(const Vector<2>& v)
	{
		glVertex2d(v[0], v[1]);
	}

	inline void glVertex(const Vector<3>& v)
	{
		glVertex3d(v[0], v[1], v[2]);
	}

	inline void glVertex(const Vector<4>& v)
	{
		glVertex4d(v[0], v[1], v[2], v[3]);
	}





	inline void glColor(const Vector<3>& v)
	{
		glColor3d(v[0], v[1], v[2]);
	}

	inline void glColor(const Vector<4>& v)
	{
		glColor4d(v[0], v[1], v[2], v[3]);
	}

	inline void glColor(const CVD::Rgb<byte>& c)
	{
		glColor3ub(c.red, c.green, c.blue);
	}

	inline void glColor(const CVD::Rgb<float>& c)
	{
		glColor3f(c.red, c.green, c.blue);
	}

	inline void glColor3(const CVD::Rgb8& c)
	{
		glColor3ub(c.red, c.green, c.blue);
	}
	
	inline void glColor4(const CVD::Rgb8& c)
	{
		glColor4ub(c.red, c.green, c.blue, c.dummy);
	}

	inline void glColor(const CVD::Rgba<unsigned char>& c)
	{
		glColor4ub(c.red, c.green, c.blue, c.alpha);
	}

	inline void glColor(const CVD::Rgba<float>& c)
	{
		glColor4f(c.red, c.green, c.blue, c.alpha);
	}

	template<class C> inline void glDrawPixels(const BasicImage<C>& i)
	{
		::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		::glDrawPixels(i.size().x, i.size().y, gl::data<C>::format, gl::data<C>::type, i.data());
	}

	template<class C> inline void glReadPixels(BasicImage<C>& i, ImageRef origin=ImageRef(0,0))
	{
		::glReadPixels(origin.x, origin.y, i.size().x, i.size().y, gl::data<C>::format, gl::data<C>::type, i.data());
	}


};

#endif

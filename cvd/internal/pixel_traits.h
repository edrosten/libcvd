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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef CVD_PIXEL_TRAITS_H_
#define CVD_PIXEL_TRAITS_H_

#include <limits>

namespace CVD {
namespace Pixel {
	
	//This is required for MIPSPro, since it is able to deduce more than gcc 3.3
	//before a template is instantiated
	template<class T> struct traits_error
	{
	};
	
	template<class T> struct traits: public traits_error<T>
	{
		static const bool integral=traits_error<T>::Error_trait_not_defined_for_this_class;
	};

	template<> struct traits<unsigned char> 
	{ 
		typedef int wider_type;
		static const bool integral = true;
		static const bool is_signed = false;
		static const int bits_used = 8;
		//static const byte max_intensity=(1 << bits_used) - 1; 
		static unsigned char max_intensity() throw() { return (1 << bits_used) - 1; }
	};

	template<> struct traits<char> 
	{ 
		typedef int wider_type;
		static const bool integral = true;
		static const bool is_signed = std::numeric_limits<char>::is_signed;
		static const int bits_used = std::numeric_limits<char>::digits;
		//static const byte max_intensity=(1 << bits_used) - 1; 
		static char max_intensity() throw() { return (1 << bits_used) - 1; }
	};

	template<> struct traits<signed char> 
	{ 
		typedef int wider_type;
		static const bool integral = true;
		static const bool is_signed = false;
		static const int bits_used = 7;
		//static const byte max_intensity=(1 << bits_used) - 1; 
		static signed char max_intensity() throw() { return (1 << bits_used) - 1; }
	};

	template<> struct traits<short> 
	{ 
		typedef int wider_type;
		static const bool integral = true;
		static const bool is_signed = true;
		static const int bits_used = 15;
		static short max_intensity() throw() { return (1 << bits_used) - 1; }
		//static const short max_intensity=(1 << bits_used) - 1; 
	};

	template<> struct traits<unsigned short> 
	{ 
		typedef int wider_type;
		static const bool integral = true;
		static const bool is_signed = false;
		static const int bits_used = 16;
		//static const unsigned short max_intensity=(1 << bits_used) - 1; 
		static unsigned short max_intensity() throw() { return (1 << bits_used) - 1; }
	};

	template<> struct traits<int> 
	{ 
		typedef int wider_type; 
		static const bool integral = true;
		static const bool is_signed = true;
		static const int bits_used = 16;
		static int max_intensity() throw() { return (1 << bits_used) - 1; }
		//static const int max_intensity=(1 << bits_used) - 1; 
	};

	template<> struct traits<unsigned int> 
	{ 
		typedef unsigned int wider_type; 
		static const bool integral = true;
		static const bool is_signed = false;
		static const int bits_used = 16;
		//static const unsigned int max_intensity=(1 << bits_used) - 1; 
		static unsigned int max_intensity() throw() { return (1 << bits_used) - 1; }
	};

	template<> struct traits<long> 
	{ 
		typedef int wider_type; 
		static const bool integral = true;
		static const bool is_signed = true;
		static const int bits_used = 16;
		//static const long max_intensity=(1 << bits_used) - 1; 
		static long max_intensity() throw() { return (1 << bits_used) - 1; }
	};

	template<> struct traits<unsigned long> 
	{ 
		typedef unsigned int wider_type; 
		static const bool integral = true;
		static const bool is_signed = false;
		static const int bits_used = 16;
		//static const long max_intensity=(1 << bits_used) - 1; 
		static unsigned long max_intensity() throw() { return (1 << bits_used) - 1; }
	};

	template<> struct traits<float> 
	{ 
		typedef float wider_type; 
		static const bool integral = false;
		static const bool is_signed = true;
		//static const float max_intensity=1.0f; 
		static float max_intensity() throw() { return 1.0f;}
	};

	template<> struct traits<double> 
	{ 
		typedef double wider_type; 
		static const bool integral = false;
		static const bool is_signed = true;
		//static const double max_intensity=1.0; 
		static double max_intensity() throw() { return 1.0;}
	};

	template<> struct traits<long double> 
	{ 
		typedef long double wider_type; 
		static const bool integral = false;
		static const bool is_signed = true;
		//static const long double max_intensity=1.0; 
		static long double max_intensity() throw() { return 1.0;}
	};

}
}

#endif

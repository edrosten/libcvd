//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  rgb.h                                                               //
//                                                                      //
//  Contains definitions of Rgb template class                          //
//                                                                      //
//  derived from IPRS_* developed by Tom Drummond                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef __RGB_H
#define __RGB_H

#include <iostream>

#include "byte.h"

namespace CVD {

template <class T>
class Rgb
{
public:
   Rgb() {}
   Rgb(T r, T g, T b) : red(r),green(g),blue(b) {}

   T red;
   T green;
   T blue;
   
   T& colour(int n);
   
   Rgb<T>& operator=(const Rgb<T>& c)
      {red = c.red; green = c.green; blue = c.blue; return *this;}

	bool operator==(const Rgb<T>& c) const
      {return red == c.red && green == c.green && blue == c.blue;}

		bool operator!=(const Rgb<T>& c) const
      {return red != c.red || green != c.green || blue != c.blue;}

   template <class T2>
     Rgb<T>& operator=(const Rgb<T2>& c){
     red = static_cast<T>(c.red);
     green = static_cast<T>(c.green); 
     blue = static_cast<T>(c.blue); 
     return *this;
   }

//   T to_grey() {return 0.3*red + 0.6*green + 0.1*blue;}
};

template <class T>
std::ostream& operator <<(std::ostream& os, const Rgb<T>& x)
{
   return os << "(" << x.red << "," << x.green << ","
	     << x.blue << ")";
}

inline std::ostream& operator <<(std::ostream& os, const Rgb<char>& x)
{
   return os << "(" << (int)(unsigned char)x.red << ","
	     << (int)(unsigned char)x.green << ","
	     << (int)(unsigned char)x.blue << ")";
}

inline std::ostream& operator <<(std::ostream& os, const Rgb<byte>& x)
{
   return os << "(" << static_cast<int>(x.red) << ","
	     << static_cast<int>(x.green) << ","
	     << static_cast<int>(x.blue) << ")";
}

} // end namespace
#endif

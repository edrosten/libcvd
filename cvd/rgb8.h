#ifndef __RGB8_H
#define __RGB8_H

namespace CVD {

struct Rgb8
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   unsigned char dummy;

   Rgb8() 
   :
   red(0), green(0), blue(0), dummy(0) 
   {}
   
   Rgb8(unsigned char r, unsigned char g, unsigned char b, unsigned char a=0)
   :
   red(r), green(g), blue(b), dummy(a)  
   {}
};

} // end namespace 
#endif

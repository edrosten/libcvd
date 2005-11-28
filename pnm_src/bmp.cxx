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
#include <cvd/internal/io/bmp.h>

using namespace std;
#include <iostream>
#include <iomanip>
#include <setjmp.h>

namespace CVD
{
  namespace BMP
  {

#pragma pack(4)
    struct BMPHeader
    {
      unsigned long size;            //!< header size
      unsigned long reserved;        //!< reserved
      unsigned long dataOffset;      //!< offset to beginning of data
      unsigned long infoSize;        //!< size of info
      unsigned long width;           //!< width of image
      unsigned long height;          //!< height of image
      unsigned short planes_bpp[2];  //!< planes and color depth
      unsigned long compression;     //!< compression type
      unsigned long dataSize;        //!< size of data
      unsigned long xPelsPerMeter;   //!< x resolution in pixels/meter
      unsigned long yPelsPerMeter;   //!< y resolution in pixels/meter
      unsigned long colors;          //!< number of specified colors in the map
      unsigned long importantColors; //!< number of important colors
    };
#pragma pack()

    inline bool isBigEndian()
    {
      char buf[4]={'a','b','c','d'};
      return (*(unsigned long*)buf & 0xFF) == 'd';
    }

    //! Returns the byte reversal of a short.
    inline unsigned short rev(unsigned short s)
    {
      return ((s&0xFF)<<8)|(s>>8);
    }

    //! Returns the byte reversal of a long.
    inline unsigned long rev(unsigned long w)
    {
      return ((w&0xFF000000)>>24)|((w&0xFF0000)>>8)|((w&0xFF00)<<8)|((w&0xFF)<<24);
    }

    void correctByteOrder(BMPHeader& header)
    {
      if (!isBigEndian())
	return;
      header.size = rev(header.size);
      header.dataOffset = rev(header.dataOffset);
      header.infoSize = rev(header.infoSize);
      header.width = rev(header.width);
      header.height = rev(header.height);
      header.planes_bpp[0] = rev(header.planes_bpp[0]);
      header.planes_bpp[1] = rev(header.planes_bpp[1]);
      header.compression = rev(header.compression);
      header.dataSize = rev(header.dataSize);
      header.xPelsPerMeter = rev(header.xPelsPerMeter);
      header.yPelsPerMeter = rev(header.yPelsPerMeter);
      header.colors = rev(header.colors);
      header.importantColors = rev(header.importantColors);
    }

    void writeBMPHeader(unsigned int width, unsigned int height, unsigned int channels, ostream& out)
    {
      unsigned int rowSize = channels*width;
      if (rowSize%4)
	rowSize += 4-(rowSize%4);
      out.write("BM",2);
      struct BMPHeader header;
      memset(&header,0,sizeof(header));
      header.dataOffset=2+sizeof(header) + (channels == 1 ? 1024 : 0);
      header.dataSize=rowSize*height;
      header.size=header.dataOffset+header.dataSize;
      header.width = width;
      header.height = height;
      header.compression = 0;
      header.planes_bpp[0] = 1;
      header.planes_bpp[1] = channels*8;
      header.infoSize=40;
      correctByteOrder(header);
      out.write((const char*)&header,sizeof(header));
      if (channels == 1) {
	unsigned long val = 0;
	unsigned long step = 0x01010101;
	for (unsigned int i=0; i<256;i++, val+=step)
	  out.write((const char*)&val, 4);
      }
    }

    void readBMPHeader(unsigned int& width, unsigned int& height, unsigned int& channels, unsigned int& compression, istream& in)
    {
      char tag[3]={0};
      in.read(tag,2);
      if (strcmp(tag, "BM") != 0)
	throw CVD::Exceptions::Image_IO::MalformedImage("BMP files must start with 'BM'");
      struct BMPHeader header;
      in.read((char*)&header, sizeof(header));
      correctByteOrder(header);
      channels = (unsigned int)header.planes_bpp[1]/8;
      compression = header.compression;
      width = header.width;
      height = header.height;
    }


  }
}

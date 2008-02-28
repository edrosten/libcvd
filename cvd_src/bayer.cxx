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
#include <string.h>
#include <cvd/colourspace.h>

//Written by Ethan

namespace CVD{namespace ColourSpace{

void bayer_to_rgb(const unsigned char* bggr, unsigned char* rgb, unsigned int width, unsigned int height)
{
  const unsigned char* row = bggr;
  const unsigned char* next = bggr+width;
  unsigned char (*out)[3] = (unsigned char (*)[3])rgb;  
  out += width*(height-1);
  unsigned int midcount = (width - 4)/2;
  // First row (bgbg)
  out[0][0] = next[1];
  out[0][1] = (row[1] + next[0])/2;
  out[0][2] = row[0];
  out[1][0] = next[1];
  out[1][1] = row[1];
  out[1][2] = (row[0]+row[2])/2;
  out += 2;
  row += 2;
  next += 2;
  for (unsigned int j=0; j<midcount; j++) {
    out[0][0] = (next[-1] + next[1])/2;
    out[0][1] = (row[-1] + row[1] + next[0])/3;
    out[0][2] = row[0];
    out[1][0] = next[1];
    out[1][1] = row[1];
    out[1][2] = (row[0] + row[2])/2;
    out += 2;
    row += 2;
    next += 2;
  }
  out[0][0] = (next[-1] + next[1])/2;
  out[0][1] = (row[-1] + row[1] + next[0])/3;
  out[0][2] = row[0];
  out[1][0] = next[1];
  out[1][1] = row[1];
  out[1][2] = row[0];
  out += 2;
  row += 2;
  next += 2;

  out -= width*2;
  // Middle rows
  const unsigned char* prev = row-width;
  for (unsigned int i=1; i<height-1; i+=2) {
    //First pixels of grgr
    out[0][0] = row[1];
    out[0][1] = row[0];
    out[0][2] = (prev[0] + next[0])/2;
    out[1][0] = row[1];
    out[1][1] = (prev[1]+row[0]+row[2]+next[1])/4;
    out[1][2] = (prev[0]+prev[2]+next[0]+next[2])/4;
    out += 2;
    row += 2;
    next += 2;
    prev += 2;
    // middle pixels of grgr
    for (unsigned int j=0; j<midcount; j++) {
      out[0][0] = (row[-1] + row[1])/2;
      out[0][1] = row[0];
      out[0][2] = (prev[0] + next[0])/2;
      out[1][0] = row[1];
      out[1][1] = (prev[1]+row[0]+row[2]+next[1])/4;
      out[1][2] = (prev[0]+prev[2]+next[0]+next[2])/4;
      out += 2;
      row += 2;
      next += 2;      
      prev += 2;
    }
    // last pixels of grgr
    out[0][0] = (row[-1] + row[1])/2;
    out[0][1] = row[0];
    out[0][2] = (prev[0] + next[0])/2;
    out[1][0] = row[1];
    out[1][1] = (prev[1]+row[0]+next[1])/3;
    out[1][2] = (prev[0]+next[0])/2;
    out += 2;
    row += 2;
    next += 2;      
    prev += 2;

    out -= width*2;
    //First pixels of bgbg
    out[0][0] = (prev[1] + next[1])/2;
    out[0][1] = (prev[0] + row[1] + next[0])/3;
    out[0][2] = row[0];
    out[1][0] = out[0][0];
    out[1][1] =  row[1];
    out[1][2] = (row[0] + row[2])/2;
    out += 2;
    row += 2;
    next += 2;
    prev += 2;
    // middle pixels of bgbg
    for (unsigned int j=0; j<midcount; j++) {
      out[0][0] = (prev[-1] + prev[1] + next[-1] + next[1])/4;
      out[0][1] = (prev[0] + row[-1] + row[1] + next[0])/4;
      out[0][2] = row[0];
      out[1][0] = (prev[1] + next[1])/2;
      out[1][1] = row[1];
      out[1][2] = (row[0]+row[2])/2;
      out += 2;
      row += 2;
      next += 2;
      prev += 2;
    }
    // last pixels of bgbg
    out[0][0] = (prev[-1] + prev[1] + next[-1] + next[1])/4;
    out[0][1] = (prev[0] + row[-1] + row[1] + next[0])/4;
    out[0][2] = row[0];
    out[1][0] = (prev[1] + next[1])/2;
    out[1][1] = row[1];
    out[1][2] = row[0];
    out += 2;
    row += 2;
    next += 2;      
    prev += 2;
    out -= width*2;
  }
  // First pixels of last row: grgr
  out[0][0] = row[1];
  out[0][1] = row[0];
  out[0][2] = prev[0];
  out[1][0] = row[1];
  out[1][1] = (row[0] + prev[1] + row[2])/3;
  out[1][2] = (prev[0] + prev[2])/2;
  out += 2;
  row += 2;
  next += 2;      
  prev += 2;
  // middle pixels of last row: grgr
  for (unsigned int j=0; j<midcount; j++) {
    out[0][0] = (row[-1] + row[1])/2;
    out[0][1] = row[0];
    out[0][2] = prev[0];
    out[1][0] = row[1];
    out[1][1] = (row[0] + prev[1] + row[2])/3;
    out[1][2] = (prev[0] + prev[2])/2;
    out += 2;
    row += 2;
    next += 2;
    prev += 2;
  }
  // Last pixels of last row: grgr
  out[0][0] = (row[-1] + row[1])/2;
  out[0][1] = row[0];
  out[0][2] = prev[0];
  out[1][0] = row[1];
  out[1][1] = (row[0] + prev[1])/2;
  out[1][2] = prev[0];
}
  
  inline unsigned char cie(unsigned char r, unsigned char g, unsigned char b) { return (r*77 + g*150 + b*29)>>8; }
  
void bayer_to_grey(const unsigned char* bggr, unsigned char* grey, unsigned int width, unsigned int height)
{
  const unsigned char* row = bggr;
  const unsigned char* next = bggr+width;
  unsigned char *out = grey;  
  out += width*(height-1);
  unsigned int midcount = (width-4)/2;
  // First row (bgbg)
  out[0] = cie(next[1], (row[1] + next[0])/2, row[0]);
  out[1] = cie(next[1], row[1], (row[0]+row[2])/2);
  out += 2;
  row += 2;
  next += 2;
  for (unsigned int j=0; j<midcount; j++) {
    out[0] = cie((next[-1] + next[1])/2, (row[-1] + row[1] + next[0])/3, row[0]);
    out[1] = cie(next[1], row[1], (row[0] + row[2])/2);
    out += 2;
    row += 2;
    next += 2;
  }
  out[0] = cie((next[-1] + next[1])/2, (row[-1] + row[1] + next[0])/3, row[0]);
  out[1] = cie(next[1], row[1], row[0]);
  out += 2;
  row += 2;
  next += 2;
  out -= width*2;
  // Middle rows
  const unsigned char* prev = row-width;
  for (unsigned int i=1; i<height-1; i+=2) {
    //First pixels of grgr
    out[0] = cie(row[1], row[0], (prev[0] + next[0])/2);
    out[1] = cie(row[1], (prev[1]+row[0]+row[2]+next[1])/4, (prev[0]+prev[2]+next[0]+next[2])/4);
    out += 2;
    row += 2;
    next += 2;
    prev += 2;
    // middle pixels of grgr
    for (unsigned int j=0; j<midcount; j++) {
      out[0] = cie((row[-1] + row[1])/2, row[0], (prev[0] + next[0])/2);
      out[1] = cie(row[1], (prev[1]+row[0]+row[2]+next[1])/4, (prev[0]+prev[2]+next[0]+next[2])/4);
      out += 2;
      row += 2;
      next += 2;      
      prev += 2;
    }
    // last pixels of grgr
    out[0] = cie((row[-1] + row[1])/2, row[0], (prev[0] + next[0])/2);
    out[1] = cie(row[1], (prev[1]+row[0]+next[1])/3, (prev[0]+next[0])/2);
    out += 2;
    row += 2;
    next += 2;      
    prev += 2;
    
    out -= width*2;

    //First pixels of bgbg
    out[0] = cie((prev[1] + next[1])/2, (prev[0] + row[1] + next[0])/3, row[0]);
    out[1] = cie((prev[1] + next[1])/2, row[1], (row[0] + row[2])/2);
    out += 2;
    row += 2;
    next += 2;
    prev += 2;
    // middle pixels of bgbg
    for (unsigned int j=0; j<midcount; j++) {
      out[0] = cie((prev[-1] + prev[1] + next[-1] + next[1])/4, (prev[0] + row[-1] + row[1] + next[0])/4, row[0]);
      out[1] = cie((prev[1] + next[1])/2, row[1], (row[0]+row[2])/2);
      out += 2;
      row += 2;
      next += 2;
      prev += 2;
    }
    // last pixels of bgbg
    out[0] = cie((prev[-1] + prev[1] + next[-1] + next[1])/4, (prev[0] + row[-1] + row[1] + next[0])/4, row[0]);
    out[1] = cie((prev[0] + row[-1] + row[1] + next[0])/4, row[1], row[0]);
    out += 2;
    row += 2;
    next += 2;      
    prev += 2;
    out -= width*2;
  }
  // First pixels of last row: grgr
  out[0] = cie(row[1], row[0], prev[0]);
  out[1] = cie(row[1], (row[0] + prev[1] + row[2])/3, (prev[0] + prev[2])/2);
  out += 2;
  row += 2;
  next += 2;      
  prev += 2;
  // middle pixels of last row: grgr
  for (unsigned int j=0; j<midcount; j++) {
    out[0] = cie((row[-1] + row[1])/2, row[0], prev[0]);
    out[1] = cie(row[1], (row[0] + prev[1] + row[2])/3, (prev[0] + prev[2])/2);
    out += 2;
    row += 2;
    next += 2;
    prev += 2;
  }
  // Last pixels of last row: grgr
  out[0] = cie((row[-1] + row[1])/2, row[0], prev[0]);
  out[1] = cie(row[1], (row[0] + prev[1])/2, prev[0]);
}


}}

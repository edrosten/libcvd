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
#include <string.h>
#include <cvd/colourspace.h>

//Written by Ethan

namespace CVD{namespace ColourSpace{


void bayer_to_grey(const unsigned char* bggr, unsigned char* grey, unsigned int width, unsigned int height)
{
  unsigned int linesize = width;
  unsigned int halfwidth = width/2;
  unsigned int halfheight = height/2;
  unsigned int i, j;
  unsigned char *row;
  unsigned char bayerbuf[width*4];
  unsigned char rowbuf[width*2];
  grey += linesize*(height-1);
  row = grey;

  // first row
  row[0] = (bggr[width+1] + (bggr[1] + bggr[width])/2 + bggr[0])/3;
  row[1] = (bggr[width+1] + bggr[1] + (bggr[0]+bggr[2])/2)/3;
  bggr += 2;
  row += 2;
  for (j=halfwidth-2;j>0;j--) {
    row[0] = ((bggr[width-1]+ bggr[width+1])/2 + (bggr[-1]+bggr[width]+bggr[1])/3 + bggr[0])/3;
    row[1] = (bggr[width+1] + bggr[1] + (bggr[0]+bggr[2])/2)/3;
    row+=2;
    bggr+=2;
  }
  row[0]=((bggr[width-1]+bggr[width+1])/2 + (bggr[-1]+bggr[width]+bggr[1])/3 + bggr[0])/3;
  row[1] = (bggr[width+1] + bggr[1] + bggr[0]);
  bggr += 2;
  row += 2;
  // main image body
  for (i=halfheight-1;i>0;i--) {
    unsigned char* bayer = bayerbuf+width;
    row = rowbuf;
    memcpy(bayerbuf, bggr-width, width*4);
    // first pixels
    row[0] = (bayer[1] + bayer[0] + (bayer[-width]+bayer[width])/2)/3;
    row[1] = (bayer[1] + (bayer[-width+1]+bayer[width+1]+bayer[0]+bayer[2])/4 + (bayer[-width] + bayer[-width+2] + bayer[width] + bayer[width+2])/4)/3;
    row += 2;
    bayer += 2;
    // grgr row
    for (j=halfwidth-2;j>0;j--) {
      row[0] = ((bayer[-1]+bayer[1])/2 + bayer[0] + (bayer[-width]+bayer[width])/2)/3;
      row[1] = (bayer[1] + (bayer[-width+1]+bayer[0]+bayer[2]+bayer[width+1])/4 + (bayer[-width] + bayer[-width+2] + bayer[width] + bayer[width+2])/4)/3;
      row += 2;
      bayer += 2;
    }
    // last pixels
    row[0] = ((bayer[-1]+bayer[1])/2 + bayer[0] + (bayer[-width]+bayer[width])/2)/3;
    row[1] = (bayer[1] + (bayer[-width+1]+bayer[0]+bayer[width+1])/3 + (bayer[-width] + bayer[width])/2)/3;
    row += 2;
    bayer += 2;

    //first pixels
    row[0] = ((bayer[-width+1]+bayer[width+1])/2 + (bayer[-width]+bayer[1]+bayer[width])/3 + bayer[0])/3;
    row[1] = ((bayer[-width+1]+bayer[width+1])/2 + bayer[1] + (bayer[0]+bayer[2])/2)/3;
    row += 2;
    bayer += 2;
    // bgbg row
    for (j=halfwidth-2;j>0;j--) {
      row[0] = ((bayer[-width-1]+bayer[-width+1]+bayer[width-1]+bayer[width+1])/4 + (bayer[-width]+bayer[width]+bayer[-1]+bayer[1])/4 + bayer[0])/3;
      row[1] = ((bayer[-width+1]+bayer[width+1])/2 + bayer[1] + (bayer[0]+bayer[2])/2)/3;
      row += 2;
      bayer+=2;
    }
    // last pixels
    row[0] = ((bayer[-width-1]+bayer[-width+1]+bayer[width-1]+bayer[width+1])/4 + (bayer[-width]+bayer[width]+bayer[-1]+bayer[1])/4 + bayer[0])/3;
    row[1] = ((bayer[-width+1]+bayer[width+1])/2 + bayer[1] + bayer[0])/3;
    bayer += 2;
    row += 2;

    bggr += width*2;
    grey -= linesize*2;
    memcpy(grey+linesize, rowbuf, linesize);
    memcpy(grey, rowbuf+linesize, linesize);
  }
  grey -= linesize;
  row = grey;
  //last row first pixels
  row[0] = (bggr[1] + bggr[0] + bggr[-width])/3;
  row[1] = (bggr[1] + (bggr[0]+bggr[-width+1]+bggr[2])/3 + (bggr[-width]+bggr[-width+2])/2)/3;
  row += 2;
  bggr += 2;
  for (j=halfwidth-2;j>0;j--) {
    row[0] = ((bggr[-1]+bggr[1])/2 + bggr[0] + bggr[-width])/3;
    row[1] = (bggr[1] + (bggr[0]+bggr[2]+bggr[-width+1])/3 + (bggr[-width]+bggr[-width+2])/2)/3;
    row += 2;
    bggr += 2;
  }
  row[0] = ((bggr[-1]+bggr[1])/2 + bggr[0] + bggr[-width])/3;
  row[1] = (bggr[1] + (bggr[0]+bggr[-width+1])/2 + bggr[-width]);
}

void bayer_to_rgb(const unsigned char* bggr, unsigned char* rgb, unsigned int width, unsigned int height)
{
  unsigned int linesize = width*3;
  unsigned int halfwidth = width/2;
  unsigned int halfheight = height/2;
  unsigned int i, j;
  unsigned char *row;
  unsigned char bayerbuf[width*4];
  unsigned char rowbuf[width*6];
  rgb += linesize*(height-1);
  row = rgb;

  // first row
  row[3]=row[0]=bggr[width+1];
  row[1]=(bggr[1] + bggr[width])/2;
  row[2]=bggr[0];
  row[4]=bggr[1];
  row[5]=(bggr[0]+bggr[2])/2;
  bggr += 2;
  row += 6;
  for (j=halfwidth-2;j>0;j--) {
    row[0]=(bggr[width-1]+bggr[width+1])/2;
    row[1]=(bggr[-1]+bggr[width]+bggr[1])/3;
    row[2] = bggr[0];
    row[3] = bggr[width+1];
    row[4] = bggr[1];
    row[5] = (bggr[0]+bggr[2])/2;
    row+=6;
    bggr+=2;
  }
  row[0]=(bggr[width-1]+bggr[width+1])/2;
  row[1]=(bggr[-1]+bggr[width]+bggr[1])/3;
  row[2] = bggr[0];
  row[3] = bggr[width+1];
  row[4] = bggr[1];
  row[5] = bggr[0];
  bggr += 2;
  row += 6;
  // main image body
  for (i=halfheight-1;i>0;i--) {
    unsigned char* bayer = bayerbuf+width;
    row = rowbuf;
    memcpy(bayerbuf, bggr-width, width*4);
    // first pixels
    row[0] = bayer[1];
    row[1] = bayer[0];
    row[2] = (bayer[-width]+bayer[width])/2;
    row[3] = bayer[1];
    row[4] = (bayer[-width+1]+bayer[width+1]+bayer[0]+bayer[2])/4;
    row[5] = (bayer[-width] + bayer[-width+2] + bayer[width] + bayer[width+2])/4;
    row += 6;
    bayer += 2;
    // grgr row
    for (j=halfwidth-2;j>0;j--) {
      row[0] = (bayer[-1]+bayer[1])/2;
      row[1] = bayer[0];
      row[2] = (bayer[-width]+bayer[width])/2;
      row[3] = bayer[1];
      row[4] = (bayer[-width+1]+bayer[0]+bayer[2]+bayer[width+1])/4;
      row[5] = (bayer[-width] + bayer[-width+2] + bayer[width] + bayer[width+2])/4;
      row += 6;
      bayer += 2;
    }
    // last pixels
    row[0] = (bayer[-1]+bayer[1])/2;
    row[1] = bayer[0];
    row[2] = (bayer[-width]+bayer[width])/2;
    row[3] = bayer[1];
    row[4] = (bayer[-width+1]+bayer[0]+bayer[width+1])/3;
    row[5] = (bayer[-width] + bayer[width])/2;
    row += 6;
    bayer += 2;

    //first pixels
    row[3] = row[0] = (bayer[-width+1]+bayer[width+1])/2;
    row[1] = (bayer[-width]+bayer[1]+bayer[width])/3;
    row[2] = bayer[0];
    row[4] = bayer[1];
    row[5] = (bayer[0]+bayer[2])/2;
    row += 6;
    bayer += 2;
    // bgbg row
    for (j=halfwidth-2;j>0;j--) {
      row[0] = (bayer[-width-1]+bayer[-width+1]+bayer[width-1]+bayer[width+1])/4;
      row[1] = (bayer[-width]+bayer[width]+bayer[-1]+bayer[1])/4;
      row[2] = bayer[0];
      row[3] = (bayer[-width+1]+bayer[width+1])/2;
      row[4] = bayer[1];
      row[5] = (bayer[0]+bayer[2])/2;
      row += 6;
      bayer+=2;
    }
    // last pixels
    row[0] = (bayer[-width-1]+bayer[-width+1]+bayer[width-1]+bayer[width+1])/4;
    row[1] = (bayer[-width]+bayer[width]+bayer[-1]+bayer[1])/4;
    row[2] = bayer[0];
    row[3] = (bayer[-width+1]+bayer[width+1])/2;
    row[4] = bayer[1];
    row[5] = bayer[0];
    bayer += 2;

    bggr += width*2;
    rgb -= linesize*2;
    memcpy(rgb+linesize, rowbuf, linesize);
    memcpy(rgb, rowbuf+linesize, linesize);
  }
  rgb -= linesize;
  row = rgb;
  //last row first pixels
  row[0] = bggr[1];
  row[1] = bggr[0];
  row[2] = bggr[-width];
  row[3] = bggr[1];
  row[4] = (bggr[0]+bggr[-width+1]+bggr[2])/3;
  row[5] = (bggr[-width]+bggr[-width+2])/2;
  row += 6;
  bggr += 2;
  for (j=halfwidth-2;j>0;j--) {
    row[0] = (bggr[-1]+bggr[1])/2;
    row[1] = bggr[0];
    row[2] = bggr[-width];
    row[3] = bggr[1];
    row[4] = (bggr[0]+bggr[2]+bggr[-width+1])/3;
    row[5] = (bggr[-width]+bggr[-width+2])/2;
    row += 6;
    bggr += 2;
  }
  row[0] = (bggr[-1]+bggr[1])/2;
  row[1] = bggr[0];
  row[2] = bggr[-width];
  row[3] = bggr[1];
  row[4] = (bggr[0]+bggr[-width+1])/2;
  row[5] = bggr[-width];
}

}}

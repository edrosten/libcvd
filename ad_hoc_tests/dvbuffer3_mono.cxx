/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software, see LICENSE file for details
*/
#include <cvd/Linux/dvbuffer3.h>

using namespace CVD;

typedef unsigned char pix;
#define DATA_TYPE GL_LUMINANCE

VideoBuffer<pix>* get_vbuf()
{
	return new DVBuffer3<pix>(0, ImageRef(640, 480));
}

#include "test/videoprog.cxx"

#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/colourspacebuffer.h>
#include <cvd/Linux/dvbuffer.h>


using namespace CVD;

typedef  Rgb<byte> pix;

VideoBuffer<pix>* get_vbuf()
{
	return new ColourspaceBuffer_managed<pix,yuv411>(new DVBuffer2<yuv411>(0, 3));
}

#include "test/videoprog.cxx"

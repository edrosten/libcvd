// -*- c++ -*-
#ifndef __DVFRAME_H
#define __DVFRAME_H

#include <cvd/videoframe.h>
#include <cvd/byte.h>

namespace CVD {

class DVFrame : public VideoFrame<byte> {
public:
  DVFrame(ImageRef s, timeval t, int buff, byte* dptr):
    VideoFrame<byte>(t.tv_sec + 0.000001*t.tv_usec, dptr, s)
  {
    //my_size = s;
    my_buffer = buff;
    //my_data = dptr;
  }

  ~DVFrame(){my_data=0;}

  int my_buffer;
};

}

#endif

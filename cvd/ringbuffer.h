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
#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include <vector>

template <class T>
class RingBuffer {
 public:
  RingBuffer(int size=0){
    my_buffer.resize(size);
    my_start=0;
  }

  RingBuffer(int size, const T &t){
    my_buffer.resize(size,t);
    my_start=0;
  }

  ~RingBuffer(){}


  void resize(int size){
    my_buffer.resize(size);
  }

  int size() const {
    return my_buffer.size();
  }

  T& operator[](int i){
    return my_buffer[(my_buffer.size()+i+my_start)%my_buffer.size()];
  }

  const T& operator[](int i) const {
    return my_buffer[(my_buffer.size()+i+my_start)%my_buffer.size()];
  }

  void advance(int n=1){
    my_start = (my_start+my_buffer.size()+n)%my_buffer.size();
  }
  
 private:
  std::vector<T> my_buffer;
  int my_start;
};


#endif


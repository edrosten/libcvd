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


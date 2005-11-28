#ifndef CVD_ALIGNED_MEM_H
#define CVD_ALIGNED_MEM_H
#include <map>
#include <cassert>

namespace CVD {
  namespace Internal
  {
    
    template <class T, int N> std::map<T*,char*> aligned_mem<T,N>::buffers;

    template <class T, int N=20> struct placement_delete
    {
      enum { Size = (1<<N) };
	
      struct Array { 
	T data[Size]; 
      };

      static inline void destruct(T* buf) 
      {
	(*(Array*)buf).~Array();
      }
      
      static inline delete(T* buf, size_t M) 
      {
	if (M >= Size) {
	  placement_delete<T,N>::delete(buf+Size,M-Size);
	  placement_delete<T,N>::destruct(buf);
	} else {
	  placement_delete<T,N-1>::delete(buf, M);
	}
      }
    };

    template <class T> struct placement_delete<T,-1>
    {
      inline placement_delete(T* buf, size_t M) {}
    };

    template <class T, int N> struct aligned_mem {
      struct entry {
	char* buffer;
	size_t count;
      };
      static std::map<T*, entry> buffers;
      static T* alloc(size_t count)
      {
	char* start = new char[count*sizeof(T) + N];
	int val = (int)start;
	T* astart = (T*)(start + (N-(val % N)));
	entry e = {start, count};
	buffers[astart] = e;
	return astart;
      }
      static void release(T* ptr) 
      {
	std::map<T*,entry>::iterator it = buffers.find(ptr);
	assert(it != buffers.end());
	placement_delete<T>::delete(ptr, it->second.count);
	delete[] it->second.buffer;
	buffers.erase(it);
      }
    };


  }
}

#endif

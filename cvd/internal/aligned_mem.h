#ifndef CVD_ALIGNED_MEM_H
#define CVD_ALIGNED_MEM_H
#include <cassert>
#include <cvd/config.h>

namespace CVD {
  namespace Internal
  {
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
      
      static inline void free(T* buf, size_t M) 
      {
	if (M >= Size) {
	  placement_delete<T,N>::free(buf+Size,M-Size);
	  placement_delete<T,N>::destruct(buf);
	} else {
	  placement_delete<T,N-1>::free(buf, M);
	}
      }
    };

    template <class T> struct placement_delete<T,-1>
    {
      static inline void free(T*, size_t ) {}
    };

    void * aligned_alloc(size_t count, size_t alignment);
    void aligned_free(void * memory);

    template <class T> 
    inline T * aligned_alloc(size_t count, size_t alignment){
        void * data = aligned_alloc(sizeof(T)* count, alignment);
        return new (data) T[count];
    }

    template <class T>
    inline void aligned_free(T * memory, size_t count){
        placement_delete<T>::free(memory, count);   
        aligned_free(memory);
    }

  } // namespace Internal

    template <class T, int N> struct AlignedMem {
	T* mem;
    size_t count;
    AlignedMem(size_t c) : count(c) {
	    mem = Internal::aligned_alloc<T>(count, N);
	}
	~AlignedMem() {
	    Internal::aligned_free<T>(mem, count);
	}
	T* data() { return mem; }
	const T* data() const { return mem; }
    };    
}

#endif

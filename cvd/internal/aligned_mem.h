#ifndef CVD_ALIGNED_MEM_H
#define CVD_ALIGNED_MEM_H
#include <map>
#include <cassert>
#include <cvd/config.h>

#ifdef _REENTRANT
    #ifndef CVD_HAVE_PTHREAD
    	#error "CVD is not compiled with thread support. This code is not thread safe."
    #else 
	#include <cvd/synchronized.h>
    #endif
#endif

namespace CVD {
  namespace Internal
  {

    extern pthread_mutex_t aligned_mem_mutex;

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


    template <class T, int N> struct aligned_mem {
	struct entry {
	    char* buffer;
	    size_t count;
	};
	static std::map<T*, entry> buffers;

	#if defined(CVD_HAVE_PTHREAD) && defined(_REENTRANT)
	    static Synchronized mutex;
	#endif


	static T* alloc(size_t count)
	{
	    char* start = new char[count*sizeof(T) + N];
	    size_t val = (size_t)start;
	    T* astart = new (start + (N-(val % N))) T[count];
	    entry e = {start, count};


	    #if defined(CVD_HAVE_PTHREAD) && defined(_REENTRANT)
	    Lock lock(mutex);
	    #endif

	    buffers[astart] = e;

	    return astart;
	}

	static void release(T* ptr) 
	{
	    #if defined(CVD_HAVE_PTHREAD) && defined(_REENTRANT)
	    Lock lock(mutex);
	    #endif

	    typename std::map<T*,entry>::iterator it = buffers.find(ptr);
	    assert(it != buffers.end());
	    placement_delete<T>::free(ptr, it->second.count);
	    delete[] it->second.buffer;


	    buffers.erase(it);
	}
    };
	#if defined(CVD_HAVE_PTHREAD) && defined(_REENTRANT)
		template<class T, int N> Synchronized aligned_mem<T,N>::mutex;
	#endif

    template <class T, int N> std::map<T*,typename aligned_mem<T,N>::entry> aligned_mem<T,N>::buffers;

  }
}

#endif

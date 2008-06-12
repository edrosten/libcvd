#include <map>
#include <cassert>
#include <cvd/config.h>

using namespace std;

#ifdef _REENTRANT
    #ifndef CVD_HAVE_PTHREAD
    	#error "CVD is not compiled with thread support. This code is not thread safe."
    #else 
	#include <cvd/synchronized.h>
    #endif
#endif

namespace CVD {

namespace Internal {

static std::map<char *, char *> buffers;

#if defined(CVD_HAVE_PTHREAD) && defined(_REENTRANT)
static Synchronized mutex;
#endif

void * aligned_alloc(size_t count, size_t alignment){
    char* start = new char[count + alignment];
	size_t val = (size_t)start;
	char * astart = start + (alignment-(val % alignment));

#if defined(CVD_HAVE_PTHREAD) && defined(_REENTRANT)
    Lock lock(mutex);
#endif
    buffers[astart] = start;

    return astart;
}

void aligned_free(void * memory){
#if defined(CVD_HAVE_PTHREAD) && defined(_REENTRANT)
    Lock lock(mutex);
#endif

    std::map<char *,char *>::iterator it = buffers.find((char *)memory);
    assert(it != buffers.end());
    delete[] it->second;
    buffers.erase(it);
}

} // namespace Internal

} // namespace CVD

#include <cassert>
#include <cstdlib>

using namespace std;

namespace CVD {

namespace Internal {

void * aligned_alloc(size_t count, size_t alignment){
    void * mem = NULL;
	const int alloc_err = posix_memalign(&mem, alignment, count);
	assert(alloc_err == 0);
    return mem;
}

void aligned_free(void * memory){
    free(memory);
}

} // namespace Internal

} // namespace CVD

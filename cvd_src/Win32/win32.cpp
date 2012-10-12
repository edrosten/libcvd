#include "win32.h" 

#include <time.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <io.h>
#include <stdlib.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// implementation from http://www.openasthra.com/c-tidbits/gettimeofday-function-for-windows/
namespace CVD {

long long get_time_of_day_ns()
{
    FILETIME ft;
    long long tmpres = 0;
    static int tzflag;

	//Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
	GetSystemTimeAsFileTime(&ft);

	tmpres |= ft.dwHighDateTime;
	tmpres <<= 32;
	tmpres |= ft.dwLowDateTime;

	//tempres is in 100ns increments
	//Convert it to ns
	tmpres *= 100;

	/*converting file time to unix epoch*/
	tmpres -= DELTA_EPOCH_IN_MICROSECS * (long long)1000; 

    return tmpres;
}

namespace Internal {

void * aligned_alloc(size_t count, size_t alignment){
    return _aligned_malloc(count, alignment);
}

void aligned_free(void * memory){
    _aligned_free(memory);
}

} // namespace Internal

// returns path component from a general path string
static std::string get_path( const std::string & p){
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char out[1024];

	errno_t ret = _splitpath_s(p.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
	assert(0 == ret);
	_makepath(out, drive, dir, NULL, NULL);
	return std::string(out);
}

// simple implementation of globlist after MSDN example for _findfirst
std::vector<std::string> globlist(const std::string& gl)
{
	std::vector<std::string> ret;
	
	struct _finddatai64_t c_file;
	intptr_t hFile;

	// get the path component to stick it to the front again
	const std::string path = get_path(gl);

	// Find first file in current directory 
	if( (hFile = _findfirsti64( gl.c_str(), &c_file )) != -1L ){
		do {
			ret.push_back(path + c_file.name);
		} while( _findnexti64( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

} // namespace CVD

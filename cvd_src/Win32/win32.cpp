#include "win32.h" 

#include <time.h>
#include <string>
#include <vector>
#include <algorithm>
#include <io.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// implementation from http://www.openasthra.com/c-tidbits/gettimeofday-function-for-windows/
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS; 
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}

namespace CVD {

namespace Internal {

void * aligned_alloc(size_t count, size_t alignment){
    return _aligned_malloc(count, alignment);
}

void aligned_free(void * memory){
    _aligned_free(memory);
}

} // namespace Internal

// simple implementation of globlist after MSDN example for _findfirst
std::vector<std::string> globlist(const std::string& gl)
{
	std::vector<std::string> ret;
	
	struct _finddatai64_t c_file;
	intptr_t hFile;

	// Find first file in current directory 
	if( (hFile = _findfirsti64( gl.c_str(), &c_file )) != -1L ){
		do {
			ret.push_back(c_file.name);
		} while( _findnexti64( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

} // namespace CVD

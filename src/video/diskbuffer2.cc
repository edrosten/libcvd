#include <cvd/diskbuffer2.h>
#include <cvd/exceptions.h>

#include <sstream>
#include <string>

using namespace CVD::Exceptions;
using namespace std;

DiskBuffer2::NoFiles::NoFiles()
    : All("DiskBuffer2 has an empty list of files.")
{
}

DiskBuffer2::BadImageSize::BadImageSize(const std::string& name)
    : All("DiskBuffer2: Image \"" + name + "\" does not match diskbuffer size.")
{
}

DiskBuffer2::BadFile::BadFile(const std::string& name, int err)
    : All("DiskBuffer2: File open on \"" + name + "\" failed with " + strerror(err))
{
}

DiskBuffer2::BadImage::BadImage(const std::string& file, const std::string& error)
    : All("DiskBuffer2: File \"" + file + "\" load failed: " + error)
{
}

DiskBuffer2::EndOfBuffer::EndOfBuffer()
    : All("DiskBuffer2 has reached the end of its list of files")
{
}

DiskBuffer2::BadSeek::BadSeek(double t)
    : All([=] {
	    ostringstream ss;
	    ss << "DiskBuffer2: Seek to time " << t << "mS failed. No such time in the buffer";
	    return ss.str();
    }())
{
}

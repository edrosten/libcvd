#include <cvd/exceptions.h>
#include <cvd/diskbuffer2.h>

#include <string.h>

using namespace CVD::Exceptions;


DiskBuffer2::NoFiles::NoFiles()
{
	what = "DiskBuffer2 has an empty list of files.";
}

DiskBuffer2::BadImageSize::BadImageSize(const std::string& name)
{
	what = "DiskBuffer2: Image \"" + name + "\" does not match diskbuffer size.";
}

DiskBuffer2::BadFile::BadFile(const std::string& name, int err)
{
	what="DiskBuffer2: File open on \""+name+"\" failed with " + strerror(err);
}

DiskBuffer2::BadImage::BadImage(const std::string& file, const std::string & error)
{
	what="DiskBuffer2: File \""+file+"\" load failed: " + error;
}

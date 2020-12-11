#include <cvd/exceptions.h>
#include <cvd/videobuffer.h>

CVD::Exceptions::OutOfMemory::OutOfMemory()
    : CVD::Exceptions::All("Out of memory.")
{
}

CVD::Exceptions::VideoBuffer::BadPutFrame::BadPutFrame()
    : CVD::Exceptions::VideoBuffer::All("Attempted to put_frame() on the wrong kind of frame.")
{
}

CVD::Exceptions::VideoBuffer::BadColourSpace::BadColourSpace(const std::string& c, const std::string& buffer)
    : CVD::Exceptions::VideoBuffer::All(
        buffer + " can not grab video in the " + c + "colourspace on the specified device.")
{
}

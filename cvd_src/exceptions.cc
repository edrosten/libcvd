#include <cvd/exceptions.h>
#include <cvd/videobuffer.h>

CVD::Exceptions::OutOfMemory::OutOfMemory()
{
	what="Out of memory.";
}

CVD::Exceptions::VideoBuffer::BadPutFrame::BadPutFrame()
{	
	what="Attempted to put_frame() on the wrong kind of frame.";
}

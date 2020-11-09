#include <cvd/timeddiskbuffer.h>

CVD::Exceptions::TimedDiskBuffer::IncompatibleListLengths::IncompatibleListLengths()
    : All("TimedDiskBuffer received file and timestamp lists of different lengths.")
{
}

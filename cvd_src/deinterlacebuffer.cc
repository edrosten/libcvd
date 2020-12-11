#include <cvd/deinterlacebuffer.h>
#include <cvd/diskbuffer2.h>
#include <cvd/exceptions.h>

using namespace std;
using namespace CVD::Exceptions::DeinterlaceBuffer;

OddNumberOfLines::OddNumberOfLines()
    : CVD::Exceptions::DeinterlaceBuffer::All("VideoFrame passed to DeinterlaceBuffer has an odd number of lines!")
{
}

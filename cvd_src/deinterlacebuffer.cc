#include <cvd/exceptions.h>
#include <cvd/diskbuffer2.h>

#include <string>
#include <sstream>

using namespace CVD::Exceptions;
using namespace std;

DeinterlaceBuffer::OddNumberOfLines::OddNumberOfLines()
{
	what = "VideoFrame passed to DeinterlaceBuffer has an odd number of lines!";
}

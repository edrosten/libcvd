#include "cvd_src/fast/prototypes.h"
#include <cvd/fast_corner.h>

namespace CVD
{
void fast_corner_detect_8(const BasicImage<byte>& i, std::vector<ImageRef>& corners, int b)
{
	fast_corner_detect_plain_8(i, corners, b);
}
}

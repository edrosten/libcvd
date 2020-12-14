#if defined(__ARM_NEON)
#include "NEON/convert_rgb_to_y.cc"
#else
#include "noarch/convert_rgb_to_y.cc"
#endif

#include "cvd/vision.h"

namespace CVD{
    void median_filter_3x3(const SubImage<byte>& I, SubImage<byte> out)
	{
		median_filter_3x3<byte>(I, out);
	}

};

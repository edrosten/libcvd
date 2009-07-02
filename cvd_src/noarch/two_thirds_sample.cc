#include "cvd/vision.h"
namespace CVD{
	void twoThirdsSample(const SubImage<byte>& in, SubImage<byte>& out)
	{
		twoThirdsSample<byte>(in, out);
	}
}

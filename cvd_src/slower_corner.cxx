#include <cvd/fast_corner.h>

using namespace std;

namespace CVD
{



	void fast_corner_detect_plain_9(const SubImage<byte>& i, vector<ImageRef>& corners, int b);
    void fast_corner_detect_plain_10(const SubImage<byte>& i, vector<ImageRef>& corners, int b);
    void fast_corner_detect_plain_12(const SubImage<byte>& i, vector<ImageRef>& corners, int b);

	void fast_corner_detect_9(const SubImage<byte>& i, vector<ImageRef>& corners, int b)
	{
		fast_corner_detect_plain_9(i, corners, b);
	}

	void fast_corner_detect_10(const SubImage<byte>& i, vector<ImageRef>& corners, int b)
	{
		fast_corner_detect_plain_9(i, corners, b);
	}

	void fast_corner_detect_2(const SubImage<byte>& i, vector<ImageRef>& corners, int b)
	{
		fast_corner_detect_plain_9(i, corners, b);
	}
}

#include <cvd/fast_corner.h>
#include <cvd/image_io.h>
#include <cvd/gl_helpers.h>
#include <cvd/videodisplay.h>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>


using namespace std;
namespace CVD
{
	void fast_corner_detect_plain_9(const SubImage<byte>& i, vector<ImageRef>& corners, int b);
	void fast_corner_detect_plain_10(const SubImage<byte>& i, vector<ImageRef>& corners, int b);
	void fast_corner_detect_plain_12(const SubImage<byte>& i, vector<ImageRef>& corners, int b);
}

using namespace CVD;

template<class A, class B> void test(const Image<byte>& i, A funcf, B funcp)
{
	cout << "Size: " << i.size() << endl;

	vector<ImageRef> faster, normal;

	funcp(i, normal, 1);
	funcf(i, faster, 1);

	cout << normal.size() << " " << faster.size() << " ";

	if(normal.size() == faster.size())
		cout << "ok." << endl;
	else
		cout << "\x1b[31m BROKEN!\x1b[0m\n";


	sort(normal.begin(), normal.end());
	sort(faster.begin(), faster.end());

	VideoDisplay d(i.size(), 2);
	
	vector<ImageRef> extra_in_normal, extra_in_faster;

	set_difference(normal.begin(), normal.end(), faster.begin(), faster.end(), back_inserter(extra_in_normal));
	set_difference(faster.begin(), faster.end(), normal.begin(), normal.end(), back_inserter(extra_in_faster));

	glDrawPixels(i);
	glPointSize(3);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex(extra_in_normal);
	glColor3f(0, 1, 0);
	glVertex(extra_in_faster);
	glEnd();
	glFlush();
	cin.get();
}

template<class A, class B> void test_images(const Image<byte>& im, A funcf, B funcp)
{
	ImageRef one(1,1);
	ImageRef zero(0,0);

	for(int i=0; i < 16; i++)
	{
		ImageRef size = im.size() - i * one;

		Image<byte> part(size);
		SubImage<byte> s = im.sub_image(zero, size);
		copy(s.begin(), s.end(),part.begin());

		test(part, funcf, funcp);
	}
}


int main(int argc, char** argv)
{
	if(argc != 2)
		return 1;



	try{
		Image<byte> im= img_load(argv[1]);

		cout << "*************************FAST-9\n";
		test_images(im, fast_corner_detect_9, fast_corner_detect_plain_9);

		cout << "*************************FAST-10\n";
		test_images(im, fast_corner_detect_10, fast_corner_detect_plain_10);

		cout << "*************************FAST-12\n";
		test_images(im, fast_corner_detect_12, fast_corner_detect_plain_12);
		
	}
	catch(Exceptions::All e)
	{
		cerr << "Error: " << e.what << endl;
	}

	return 0;
}

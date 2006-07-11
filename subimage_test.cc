#include <iostream>
#include <algorithm>
#include <tag/stdpp.h>

#include <cvd/image_io.h>
#include <cvd/videodisplay.h>
#include <cvd/gl_helpers.h>
#include <cvd/timer.h>



using namespace std;
using namespace CVD;
using namespace tag;

void clear_area(BasicImage<byte> im, ImageRef start, ImageRef size)
{
	int ye = start.y + size.y;
	int xe = start.x + size.x;

	for(int y=start.y; y < ye; y++)
		for(int x=start.x; x < xe; x++)
			im[y][x] = 0;
}


template<class A, class B> void clear(A a, B b)
{
	for(; a != b; ++a)
		*a=128;
}

int main()
{
	Image<byte> in;

	img_load(in, "lab.pgm");

	VideoDisplay d(in.size());

	glDrawPixels(in);
	glFlush();


	SubImage<byte> s = in.sub_image(ImageRef(10,10), ImageRef(100,100));

	VideoDisplay e(0,0,90,90);
	glDrawPixels(s);
	cin.get();

	d.make_current();

	clear(s.begin(), s.end());

	glDrawPixels(in);
	glFlush();

	clear_area(in, ImageRef(220, 10), ImageRef(100,100));
	
	glDrawPixels(in);
	glFlush();

	cin.get();

	s = in;


	cvd_timer t;
	for(int i=0; i < 10000; i++)
		clear(in.begin(), in.end());
	cout << "Using T* as iterator: " << t.get_time() << endl;

	t.reset();
	for(int i=0; i < 10000; i++)
		clear(s.begin(), s.fastend());
	cout << "Using SubImageIterator and SubImageIteratorEnd: " << t.get_time() << endl;

	t.reset();
	for(int i=0; i < 10000; i++)
		clear(s.begin(), s.end());
	cout << "Using SubImageIterator only: " << t.get_time() << endl;


	t.reset();
	for(int i=0; i < 10000; i++)
		clear_area(in, ImageRef_zero, in.size());
	cout << "Using double for loop: " << t.get_time() << endl;

}

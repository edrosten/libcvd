#include <cvd/fast_corner.h>
#include <cvd/image_io.h>
#include <utility>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cstdlib>
#include <random>


using namespace std;
namespace CVD
{
	void fast_corner_detect_plain_9(const BasicImage<byte>& i, vector<ImageRef>& corners, int b);
	void fast_corner_detect_plain_10(const BasicImage<byte>& i, vector<ImageRef>& corners, int b);
	void fast_corner_detect_plain_12(const BasicImage<byte>& i, vector<ImageRef>& corners, int b);
}

using namespace CVD;

inline bool positive(int val, int centre, int barrier)
{
	return val > centre + barrier;
}

inline bool negative(int val, int centre, int barrier)
{
	return val < centre - barrier;
}

template<int num_for_corner, class Test>
inline int is_corner(const BasicImage<byte>& im, const ImageRef off, int barrier, const Test& test)
{
	int num_consecutive=0;
	int first_cons=0;
	const int centre = im[off];

	for(int i=0; i<16; i++)
	{
		int val = im[fast_pixel_ring[i] + off];
		if(test(val, centre, barrier))
		{
			num_consecutive++;
			
			if(num_consecutive == num_for_corner)
				return 1;
		} 
		else
		{
			if(num_consecutive == i)
				first_cons=i;

			num_consecutive=0;
		}
	}
	
	return first_cons+num_consecutive >=num_for_corner;
}

template<int Num> void segment_test(const BasicImage<byte>& im, vector<ImageRef>& v, int threshold)
{
	for(int y=3; y < im.size().y-3; y++)
		for(int x=3; x < im.size().x-3; x++)
			if(is_corner<Num>(im, ImageRef(x, y), threshold, positive) || is_corner<Num>(im, ImageRef(x, y), threshold, negative))
				v.push_back(ImageRef(x, y));	
}





template<class A, class B, class C> void test(const SubImage<byte>& i, A funcf, B funcp, C funcs, int threshold, string type)
{

	vector<ImageRef> faster, normal, simple;

	funcp(i, normal, threshold);
	funcf(i, faster, threshold);
	funcs(i, simple, threshold);


	sort(normal.begin(), normal.end());
	sort(faster.begin(), faster.end());
	sort(simple.begin(), simple.end());

	vector<ImageRef> all;
	copy(normal.begin(), normal.end(), back_inserter(all));
	copy(faster.begin(), faster.end(), back_inserter(all));
	copy(simple.begin(), simple.end(), back_inserter(all));

	sort(all.begin(), all.end());
	vector<ImageRef>::iterator new_end = unique(all.begin(), all.end());
	all.resize(new_end - all.begin());


	vector<ImageRef> bad;
	set_symmetric_difference(faster.begin(), faster.end(), simple.begin(), simple.end(), back_inserter(bad));
	set_symmetric_difference(normal.begin(), normal.end(), simple.begin(), simple.end(), back_inserter(bad));

	if(bad.empty())
		return;
	
	cout << "*********************************************" << type << endl;
	cout << "Size: " << i.size() << " threshold: " << threshold << " ";
	cout << normal.size() << " " << faster.size() << " " << simple.size() << " " << all.size() << " ";
	cout << "fail." << endl;
	
	exit(1);
}

template<class A, class B, class C> void test_images(const SubImage<byte>& im, A funcf, B funcp, C funcs, int threshold, string type)
{
	ImageRef one(1,1);
	ImageRef zero(0,0);

	ImageRef d[] = {{1,1},{1,0},{0,1}};

	for(int i=0; i < 16; i++)
	{
		for(const auto& one: d)
		{
			ImageRef size = im.size() - i * one;

			if(size.x <= 0 || size.y <= 0)
				continue;

			Image<byte> part(size);
			BasicImage<byte> s = im.sub_image(zero, size);
			//copy(s.begin(), s.end(),part.begin());

			test(s, funcf, funcp, funcs, threshold, type);
		}
	}
}


int main(int , char** )
{
	std::random_device device;
	std::ranlux48 engine(device());
	std::uniform_real_distribution<> distribution(0.0, 1.0);
	for(int n=16; n < 100; n+=1)
	{
		Image<byte> im(ImageRef(n, n));

		for(Image<byte>::iterator i = im.begin(); i != im.end(); i++)
			*i =  (distribution(engine) * 256);

		
		for(int k=0; k < 2; k++)
		{
			ImageRef start;
			start.x = distribution(engine)*(n/2);
			start.y = distribution(engine)*(n/2);

			ImageRef size = im.size() - start;
			size.x = distribution(engine)*(size.x-1) + 1;
			size.y = distribution(engine)*(size.y-1) + 1;

			SubImage<byte> ims = im.sub_image(start, size);


			int threshold = distribution(engine) * 256;
			test_images(ims, fast_corner_detect_9, fast_corner_detect_plain_9, segment_test<9>, threshold, "FAST9");
			test_images(ims, fast_corner_detect_10, fast_corner_detect_plain_10, segment_test<10>, threshold, "FAST10");
			test_images(ims, fast_corner_detect_12, fast_corner_detect_plain_12, segment_test<12>, threshold, "FAST12");
		}
	}
}

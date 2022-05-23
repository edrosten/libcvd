#include "test_utility.h"

#include <cvd/image.h>
#include <cvd/vision.h>

#include <chrono>
#include <random>

using CVD::Image;
using CVD::ImageRef;
using CVD::Testing::assert_equal;
using CVD::Testing::assert_image_equal;
using CVD::Testing::init;

int main()
{
	Image<int> a = init({ { 1, 2 }, { 3, 4 } });
	flipVertical(a);
	assert_image_equal(init({ { 3, 4 }, { 1, 2 } }), a, "Even sized flipV failed");

	////////////////////////////////////////////////////////////////////////////////
	a = init({ { 1, 2 }, { 3, 4 }, { 5, 6 } });
	flipVertical(a);
	assert_image_equal(init({ { 5, 6 }, { 3, 4 }, { 1, 2 } }), a, "Odd sized flipV failed");

	////////////////////////////////////////////////////////////////////////////////
	a = init({ { 1, 2 }, { 3, 4 }, { 5, 6 } });
	Image<int> b(a.size().transpose());
	CVD::Internal::simpleTranspose(a, b);
	assert_image_equal(init({ { 1, 3, 5 }, { 2, 4, 6 } }), b, "Simple transpose failed");

	////////////////////////////////////////////////////////////////////////////////
	b.resize(a.size().transpose());
	CVD::Internal::recursiveTranspose(a, b, 1);
	assert_image_equal(init({ { 1, 3, 5 }, { 2, 4, 6 } }), b, "Recursive transpose failed (small)");

	////////////////////////////////////////////////////////////////////////////////
	const int N = 100;
	const int R = 10;

	std::mt19937 eng;
	for(int i = 0; i < N; i++)
	{
		std::uniform_int_distribution<> rng(1, 1024);
		std::uniform_int_distribution<> byte(0, 255);

		ImageRef size;
		size.x = rng(eng);
		size.y = rng(eng);

		double s_simple = 0, s_recursive = 0;

		for(int j = 0; j < R; j++)
		{
			Image<uint8_t> im1(size), im2;
			for(auto& p : im1)
				p = static_cast<uint8_t>(byte(eng));

			auto t1 = std::chrono::steady_clock::now();
			Image<uint8_t> recurs = CVD::transpose(im1);
			auto t2 = std::chrono::steady_clock::now();

			Image<uint8_t> simple(recurs.size());
			CVD::Internal::simpleTranspose(im1, simple);
			auto t3 = std::chrono::steady_clock::now();

			CVD::Testing::assert_image_equal(simple, recurs);

			s_recursive += std::chrono::duration<double>(t2 - t1).count();
			s_simple += std::chrono::duration<double>(t3 - t2).count();
		}
		std::cout << size << " " << size.area() << " " << s_simple / R << " " << s_recursive / R << "\n";
	}
}

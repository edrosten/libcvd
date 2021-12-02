#include <chrono>
#include <cvd/image.h>
#include <cvd/vision.h>
#include <random>

using CVD::Image;
using CVD::ImageRef;
using CVD::SubImage;

Image<int> im(int x, int y, const std::initializer_list<int>& data)
{
	if(ImageRef(x, y).area() != (int)data.size())
		abort();

	return SubImage<int>(const_cast<int*>(std::data(data)), ImageRef(x, y));
}

int main()
{

	Image<int> a;

	a = im(2, 2,
	    { 1, 2,
	        3, 4 });

	flipVertical(a);

	if(!std::equal(a.begin(), a.end(), im(2, 2, { 3, 4, 1, 2 }).begin()))
		throw std::logic_error("Even sized flipV failed");

	////////////////////////////////////////////////////////////////////////////////
	a = im(2, 3,
	    { 1, 2,
	        3, 4,
	        5, 6 });

	flipVertical(a);

	if(!std::equal(a.begin(), a.end(), im(2, 3, { 5, 6, 3, 4, 1, 2 }).begin()))
		throw std::logic_error("Odd sized flipV failed");

	////////////////////////////////////////////////////////////////////////////////

	a = im(2, 3,
	    { 1, 2,
	        3, 4,
	        5, 6 });

	Image<int> b(a.size().transpose());
	CVD::Internal::simpleTranspose(a, b);

	if(!std::equal(b.begin(), b.end(), im(2, 3, { 1, 3, 5, 2, 4, 6 }).begin()))
		throw std::logic_error("Simple transpose failed");

	////////////////////////////////////////////////////////////////////////////////

	a = im(2, 3,
	    { 1, 2,
	        3, 4,
	        5, 6 });

	b.resize(a.size().transpose());
	CVD::Internal::recursiveTranspose(a, b, 1);

	if(!std::equal(b.begin(), b.end(), im(2, 3, { 1, 3, 5, 2, 4, 6 }).begin()))
		throw std::logic_error("Recursive transpose failed (small)");

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

			if(!std::equal(simple.begin(), simple.end(), recurs.begin()))
				throw std::logic_error("Recursive transpose failed");

			s_recursive += std::chrono::duration<double>(t2 - t1).count();
			s_simple += std::chrono::duration<double>(t3 - t2).count();
		}
		std::cout << size << " " << size.area() << " " << s_simple / R << " " << s_recursive / R << "\n";
	}
}

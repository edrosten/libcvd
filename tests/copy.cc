#include <cstdlib>
#include <cvd/image.h>
#include <stdexcept>
#include <vector>

using namespace CVD;

#define TEST(X)                                                            \
	if(!(X))                                                               \
	{                                                                      \
		std::cerr << "Error " << __FILE__ << " " << __LINE__ << " " << #X; \
		std::abort();                                                      \
	}

int main()
{

	Image<char> im1({ 2, 2 }, 'a');
	Image<char> im2({ 1, 1 }, 'b');

	Image<char> im3;

	im3.copy_from(im2);
	TEST(std::string(im3.data(), im3.size().area()) == "b")

	im3.copy_from(im1);
	TEST(std::string(im3.data(), im3.size().area()) == "aaaa")

	im3.copy_from(im1);
	im3.sub_image({ 0, 0 }, { 1, 1 }).copy_from(im2);
	TEST(std::string(im3.data(), im3.size().area()) == "baaa")

	Image<std::vector<int>> im4({ 1, 1 }, { 1, 2, 3, 4 }), im5;
	im5 = im4;

	TEST(im5[0][0] == std::vector<int>({ 1, 2, 3, 4 }));
}

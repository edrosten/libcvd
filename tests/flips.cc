#include <cvd/image.h>
#include <cvd/vision.h>

using CVD::Image;
using CVD::SubImage;
using CVD::ImageRef;

Image<int> im(int x, int y, const std::initializer_list<int>& data){
	if(ImageRef(x, y).area() != (int)data.size())
		abort();

	return SubImage<int>(const_cast<int*>(std::data(data)), ImageRef(x, y));
}

int main(){
	
	Image<int> a;

	a = im(2,2,
	{
		1, 2,
		3, 4
	});

	flipVertical(a);

	if(!std::equal(a.begin(), a.end(), im(2,2,
	{
		3, 4,
		1, 2
	}).begin()))
		throw std::logic_error("Even sized flipV failed");


	a = im(2,3,
	{
		1, 2,
		3, 4,
		5, 6
	});

	flipVertical(a);

	if(!std::equal(a.begin(), a.end(), im(2,3,
	{
		5, 6,
		3, 4,
		1, 2
	}).begin()))
		throw std::logic_error("Odd sized flipV failed");


	a = im(2,3,
	{
		1, 2,
		3, 4,
		5, 6
	});

	a = CVD::Internal::simpleTranspose(a);

	if(!std::equal(a.begin(), a.end(), im(2,3,
	{
		1, 3, 5,
		2, 4, 6
	}).begin()))
		throw std::logic_error("Simple transpose failed");

}

#include <cvd/image_convert.h>

#include <iostream>
using namespace std;

namespace CVD
{
namespace Pixel
{

	//This file contains instantiations of all the misc. things that need to be
	//instantiated. 

	//Some useful rgb weightings
	WeightedRGB<>		uniform(1./3, 1./3, 1./3);
	WeightedRGB<>		CIE(0.299, 0.587, 0.114);
	WeightedRGB<>		red_only(1, 0, 0);
	WeightedRGB<>		green_only(0, 1, 0);
	WeightedRGB<>		blue_only(0, 0, 1);


}
}

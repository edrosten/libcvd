#ifndef CVD_IMAGE_H_9e7447dd4a4f1d760e7bbe8bfd961bf2
#define CVD_IMAGE_H_9e7447dd4a4f1d760e7bbe8bfd961bf2

namespace CVD
{
class Default;
class Reference;

template <class Pixel, class Base = Default>
class Image : public Image<Pixel, Reference>
{
}

}

#endif

#ifndef CVD_LOAD_AND_SAVE_H
#define CVD_LOAD_AND_SAVE_H

#include <cvd/internal/disk_image.h>
namespace CVD {namespace Internal{
////////////////////////////////////////////////////////////////////////////////
//
// Loading from disk
//


//Convert incoming data (of N channels) to image data of known type
template<int Channels, class In, class Out, class Conversion> inline void load_line(In* in, Out* out, unsigned int n, Conversion& cv)
{
	unsigned int j;
	for(j=0; j < n; j++, in += Channels)
		cv.convert_pixel(*reinterpret_cast<In(*)[Channels]>(in), *out++);

		//In(*)[Channels] means a   pointer to{ an {array of length Channels} of {type In}}
}

template<class In, class Out, class Conversion> inline void load_line(In* in, Out* out, unsigned int n, Conversion& cv)
{
	unsigned int j;
	for(j=0; j < n; j++, in ++)
		cv.convert_pixel(*in, *out++);
}

//Load the data from disk, and then call the correct conversion
//function, depending on how many channels the incoming data has
template<class In, class P, class Conversion> void conv_load(CVD::Image_IO::image_in& i, P* out, Conversion& cv, int nl)
{
	//Allocate an exception-safe buffer for reading in the image.
	In* buf = new In[nl * i.elements_per_line()];
	std::auto_ptr<In> m_buff(buf);

	if(buf == NULL)
		throw CVD::Exceptions::OutOfMemory();

	int channels  = i.channels();

	//Read in the data	
	i.get_raw_pixel_lines(buf, nl);

	unsigned long max;
	max = nl * i.x_size();

	//Convert the data to the correct type

	//Switch coversion on the type of incmoing data
	if(channels == 1)
		load_line(buf, out, max, cv);
	else if(channels == 2) //grey+alpha is currently unimplemented
		load_line<2>(buf, out, max, cv);
	else if(channels == 3)
		load_line<3>(buf, out, max, cv);
	else if(channels == 4)
		load_line<4>(buf, out, max, cv);
}


//call the correct typed loader depending whether the image on disk
//is 8 bit or 16 bit
template<class PixelType, class Conversion> void load_image(CVD::Image_IO::image_in& im, PixelType* p, Conversion& cv)
{
	if(im.is_2_byte())
		conv_load<unsigned short>(im, p, cv, im.y_size());
	else
		conv_load<unsigned char>(im, p, cv, im.y_size());
}

////////////////////////////////////////////////////////////////////////////////
//
// Saving to disk
//

//Convert data to outgoing type
template<int Channels, class Disk, class Img, class Conversion> inline void conv_to_disk(Disk* dsk, Img* im, unsigned int n, Conversion& cv)
{
	unsigned int j;
	for(j=0; j < n; j++, dsk += Channels)
		cv.convert_pixel(*im++, *reinterpret_cast<Disk(*)[Channels]>(dsk));
}
template<class Disk, class Img, class Conversion> inline void conv_to_disk(Disk* dsk, Img* im, unsigned int n, Conversion& cv)
{
	unsigned int j;
	for(j=0; j < n; j++, dsk ++)
		cv.convert_pixel(*im++, *dsk);
}

template<class Out, class P, class Conversion> void conv_save(CVD::Image_IO::image_out& o, P* im, Conversion& cv, int nl)
{
	//Allocate an exception-safe buffer for reading in the image.
	Out* buf = new Out[nl * o.elements_per_line()];
	std::auto_ptr<Out> m_buff(buf);

	if(buf == NULL)
		throw CVD::Exceptions::OutOfMemory();

	int channels  = o.channels();

	unsigned long max;
	max = nl * o.x_size();

	//Convert the image data to the outgoing type
	if(channels == 1)
		conv_to_disk(buf, im, max, cv);
	else if(channels == 2) 
		conv_to_disk<2>(buf, im, max, cv);
	else if(channels == 3)
		conv_to_disk<3>(buf, im, max, cv);
	else if(channels == 4)
		conv_to_disk<4>(buf, im, max, cv);


	//Write in the data	
	o.write_raw_pixel_lines(buf, nl);
}

//call the correct typed loader depending whether the image on disk
//is 8 bit or 16 bit. Remember, the parameter passed to the factory
//class is only a request. 
template<class PixelType, class Conversion> void save_image(CVD::Image_IO::image_out& im, PixelType* p, Conversion& cv)
{
	if(im.is_2_byte())
		conv_save<unsigned short>(im, p, cv, im.y_size());
	else
		conv_save<unsigned char>(im, p, cv, im.y_size());
}



}}

#endif

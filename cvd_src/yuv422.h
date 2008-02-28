#ifndef CVD_SRC_YUV_422_H
#define CVD_SRC_YUV_422_H
namespace CVD {
namespace ColourSpace {
    
	void yuv422_to_grey(const unsigned char* yuv, unsigned char* grey, unsigned int width, unsigned int height);
	void yuv422_to_rgb(const unsigned char* yuv, unsigned char* rgb, unsigned int width, unsigned int height);

	void yuv422_to_rgb_c(const unsigned char* yuv, unsigned char* rgb, unsigned int n);
	void yuv422_to_grey_c(const unsigned char* yuv, unsigned char* grey, unsigned int n);
}
}
#endif


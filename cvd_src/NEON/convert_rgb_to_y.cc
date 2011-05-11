#include <cvd/config.h>
#include <cvd/image_convert.h>
#include <cvd/utility.h>
#include <arm_neon.h>

namespace CVD {

	namespace Internal {
		void convert_rgb_to_grey_NEON(const SubImage<Rgb<byte> >& from, SubImage<byte>& to, const unsigned red, const unsigned green, const unsigned blue) 
			const uint16x8_t red = vdupq_n_u16(red);
			const uint16x8_t green = vdupq_n_u16(green);
			const uint16x8_t blue = vdupq_n_u16(blue);
			
			for( int y = 0; y < from.size().y; ++y){
				const Rgb<byte> * in = from[y];
				byte * out = out[y];
				for( int x = 0; x < from.size().x; x+=8, in += 8, out += 8){
					uint8x8x3_t in_data  = vld3_u8((const uint8_t *)in);

					uint16x8_t sum = vmulq_u16(in_data[0], red);
					sum = vmlaq_u16(sum, in_data[1], green);
					sum = vmlaq_u16(sum, in_data[2], blue);

					uint8x8_t final_sum = vshrn_n_u16(sum, 8); // divide by 256
					vst1_u8(out_data, final_sum);
				}
			}
		}
	}

	void ConvertImage<Rgb<byte>, byte, Pixel::CIE<Rgb<byte>, byte>, 1>::convert(const SubImage<Rgb<byte> >& from, SubImage<byte>& to) 
	{

		if((from.size().x % 8) == 0){
		// red (77) + green (150) + blue (29) = 256
		Internal::convert_rgb_to_grey_NEON( from, to, 77, 150, 29);
		} else {
			const int multiple_of_8_width = (from.size().x / 8) * 8;
			const int remainder = from.size().x - multiple_of_8_width;
			const ImageRef end_fast(multiple_of_8_width, from.size().y);

			Internal::convert_rgb_to_grey_NEON( from.sub_image(ImageRef_zero, end_fast), to.sub_image(ImageRef_zero, end_fast), 77, 150, 29);
			
			for(int y = 0; y < from.size().y; ++y){
				const Rgb<byte> * in = from[y]+multiple_of_8_width;
				byte * out = to[y]+multiple_of_8_width;
				for(int x = multiple_of_8_width; x < from.size().x; ++x, ++in, ++out){
					Pixel::CIE<Rgb<byte>,byte>::convert(*in, *out);
				}
			}
		}
	}
}

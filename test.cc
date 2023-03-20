#include <cvd/image.h>
#include <cvd/opencv.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cvd/internal/rgb_components.h>

using namespace std;


template<class Pixel>
void downsampleInterArea(const CVD::BasicImage<Pixel>& src, CVD::BasicImage<Pixel>& dst){
	std::vector<Pixel> row(dst.size().x);


	double ratio_w = dst.size().x * 1.0 / (src.size().x);
	double ratio_h = dst.size().y * 1.0 / (src.size().y);
	double scale = (ratio_w * ratio_h);
	
	cerr << ratio_w << " " << ratio_h << "\n";
	CVD::Image<typename CVD::Pixel::traits<Pixel>::float_type> row1({dst.size().x+1, 1}), row2({dst.size().x+1,1});
	row1.zero();
	row2.zero();

	//Proceed row pixel at a time
	for(int r=0; r < src.size().y; r++){
		
		const int out_row1 = std::floor(r*ratio_h);
		const int out_row2 = std::floor((r+1)*ratio_h);
		const double row_frac = [&](){
			if(out_row2 == out_row1)
				return 1.;
			else
				return (out_row2 - r*ratio_h)/ratio_h;
		}();
		
		std::cerr << "j = " << r << " " << out_row1 << " " << out_row2 << " " << row_frac << endl;

		//Proceed one pixel at a time
		for(int c=0; c < src.size().x; c++){
			const int out_pixel1 = std::floor(c*ratio_w);
			const int out_pixel2 = std::floor((c+1)*ratio_w);
			const double frac = [&](){
				if(out_pixel1 == out_pixel2)
					return 1.;
				else
					return (out_pixel2 - c*ratio_w)/ratio_w;
			}();


			std::cerr << "    i=" << c << " px1=" << out_pixel1 << " px2=" << out_pixel2 << " frc=" << frac << endl;
			row1[0][out_pixel1] += row_frac *    frac  * src[r][c];
			row1[0][out_pixel2] += row_frac * (1-frac) * src[r][c];
			row2[0][out_pixel1] += (1-row_frac) *    frac  * src[r][c];
			row2[0][out_pixel2] +=
				(1-row_frac) * (1-frac) * src[r][c];

			
			cerr << "    " <<  row1[0][out_pixel1] * ratio_w << " <-- pix1\n";
			

		} 

		cerr << "row ";
		for(int i=0; i < dst.size().x; i++){
			cerr << row1[0][i]*ratio_w << " ";
			dst[out_row1][i]=row1[0][i] * scale;
		}
		cerr << endl;

		if(out_row2 != out_row1){
			row1.zero();
			swap(row2, row1);
		}
	}
}



using namespace CVD;

int main(){
	
	vector<double> pixels = {1, 2, 3, 0, 2, 3, 4, 8, 9, 1};

	BasicImage<double> in(pixels.data(), ImageRef(5,2));
	Image<double> out(ImageRef(3, 1));

	OpenCV::resize(in, out, cv::INTER_AREA);

	std::cerr << "----- OpenCV -----\n";
	for(int j=0; j < out.size().y; j++){
		for(int i=0; i < out.size().x; i++)
			cerr << out[j][i] << " ";
		cerr << endl;
	}
	
	out.zero();
	downsampleInterArea(in, out);
	
	std::cerr << "----- libcvd -----\n";
	for(int j=0; j < out.size().y; j++){
		for(int i=0; i < out.size().x; i++)
			cerr << out[j][i] << " ";
		cerr << endl;
	}

}

#include <iostream>
#include <fstream>
#include <string>



#include <cvd/image_io.h>

using namespace std;
using namespace CVD;


template<class T> string make_output_file_name(string fin, string type)
{
	std::string fn;

	//T tmp;

	//Remove the path from the filename
	for(int i=fin.size()-1; i >= 0; i--)
	{
		if(fin[i] != '/')
			fn = fin[i] + fn;
		else
			break;
	}

	fn = " from " + fn;
	fn = PNM::type_name<T>::name() + fn;

	Image<T> im;

	string fout = "testout/" + fn + "." + type;

	return fout;
}


template<class T> void loadsave(string fin)
{
	Image<T> im;
	
	//Load the image
	cout << "Reading: " << fin << endl;
	ifstream i(fin.c_str());
	img_load(im, i);
	i.close();


	string fout=make_output_file_name<T>(fin, "pnm");
	cout << "Writing: " << fout << endl << endl;

	//Save it	
	ofstream o(fout.c_str());
	img_save(im, o, ImageType::PNM);
	o.close();

	//Save it
	#ifdef CVD_IMAGE_HAS_JPEG
		fout=make_output_file_name<T>(fin, "jpg");
		cout << "Writing: " << fout << endl << endl;
		o.open(fout.c_str());
		img_save(im, o, ImageType::JPEG);
		o.close();
	#endif
}

template<class T> void loadsave_safe(const char*n)
{
	try
	{
		loadsave<T>(n);
	}
	catch(CVD::Exceptions::All b0rk3d)
	{
		cerr << "Image " << n << " error: " << b0rk3d.what << endl;
	}
}




int main(int ac, char** av)
{
	if(ac < 2)
	{
		cerr << "Error: give a filename\n";
		exit(1);
	}
	
	for(int i=1; i <ac; i++)
	{
		loadsave_safe<unsigned char>(av[i]);
		loadsave_safe<signed char>(av[i]);
		loadsave_safe<short>(av[i]);
		loadsave_safe<unsigned short>(av[i]);
		loadsave_safe<int>(av[i]);
		loadsave_safe<unsigned int>(av[i]);
		loadsave_safe<long>(av[i]);
		loadsave_safe<unsigned long>(av[i]);
		//loadsave_safe<long long>(av[i]);
		//loadsave_safe<unsigned long long>(av[i]);
		loadsave_safe<float>(av[i]);
		loadsave_safe<double>(av[i]);
		//loadsave_safe<long double>(av[i]);

	
		loadsave_safe<CVD::Rgb<unsigned char> >(av[i]);
		loadsave_safe<CVD::Rgb<signed char> >(av[i]);
		loadsave_safe<CVD::Rgb<short> >(av[i]);
		loadsave_safe<CVD::Rgb<unsigned short> >(av[i]);
		loadsave_safe<CVD::Rgb<int> >(av[i]);
		loadsave_safe<CVD::Rgb<unsigned int> >(av[i]);
		loadsave_safe<CVD::Rgb<long> >(av[i]);
		loadsave_safe<CVD::Rgb<unsigned long> >(av[i]);
		//loadsave_safe<CVD::Rgb<long long> >(av[i]);
		//loadsave_safe<CVD::Rgb<unsigned long long> >(av[i]);
		loadsave_safe<CVD::Rgb<float> >(av[i]);
		loadsave_safe<CVD::Rgb<double> >(av[i]);
		//loadsave_safe<CVD::Rgb<long double> >(av[i]);

		loadsave_safe<CVD::Rgb8>(av[i]);
		loadsave_safe<CVD::Rgba<unsigned char> >(av[i]);
		loadsave_safe<CVD::Rgba<unsigned int> >(av[i]);
	}
	
	
	exit(0);
}




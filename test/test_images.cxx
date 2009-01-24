/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <limits>
#include <algorithm>


#include <cvd/image_io.h>

using namespace std;
using namespace CVD;

using CVD::Internal::TypeList;
using CVD::Internal::Head;

template<class C> struct randpix
{
	static C r()
	{
		return rand() % (1+numeric_limits<C>::max());
	}
};

template<> struct randpix<double>
{
	static double r()
	{
		return rand() * 10./RAND_MAX - 5;
	}
};

template<> struct randpix<float>
{
	static float r()
	{
		return rand() * 10./RAND_MAX - 5;
	}
};

template<class C> struct randpix<Rgb<C> >
{
	static Rgb<C> r()
	{
		return Rgb<C>(randpix<C>::r(), randpix<C>::r(),randpix<C>::r());
	}
};

template<class C> struct randpix<Rgba<C> >
{
	static Rgba<C> r()
	{
		return Rgba<C>(randpix<C>::r(), randpix<C>::r(),randpix<C>::r(), randpix<C>::r());
	}
};

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
	ifstream i(fin.c_str(), ios::in|ios::binary);
	img_load(im, i);
 	i.close();
 

	string fout=make_output_file_name<T>(fin, "pnm");
	cout << "Writing: " << fout << endl << endl;
 
 	//Save it	
	ofstream o(fout.c_str(), ios::out|ios::binary);
	img_save(im, o, ImageType::PNM);
	o.close();

	//Save it
	#ifdef CVD_HAVE_JPEG
		fout=make_output_file_name<T>(fin, "jpg");
		cout << "Writing: " << fout << endl << endl;
		o.open(fout.c_str());
		img_save(im, o, ImageType::JPEG);
		o.close();
	#endif
	#ifdef CVD_HAVE_PNG
		fout=make_output_file_name<T>(fin, "png");
		cout << "Writing: " << fout << endl << endl;
		o.open(fout.c_str());
		img_save(im, o, ImageType::PNG);
		o.close();
	#endif
	#ifdef CVD_HAVE_TIFF
		fout=make_output_file_name<T>(fin, "tiff");
		cout << "Writing: " << fout << endl << endl;
		o.open(fout.c_str());
		img_save(im, o, ImageType::TIFF);
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

template<class T> struct randtest
{
	typedef typename T::Type Type;

	static void exec(ImageType::ImageType fmt)
	{	
		try{
			for(int i=0; i < 10; i++)
			{
				//Make a random image
				Image<Type> in(ImageRef(1000,1000)), out;

				for(int y=0; y < in.size().y; y++)
					for(int x=0; x < in.size().x; x++)
						in[y][x] = randpix<Type>::r();

				stringstream s;
				
				//Save the image
				img_save(in, s, fmt);

				s.seekg(0, ios_base::beg);
				s.seekp(0, ios_base::beg);
				
				//Load the image
				out = img_load(s);

				//Compare the results
				if(out.size() != in.size())
					cerr << "Image R/W test for type " << fmt << " " << CVD::PNM::type_name<Type>::name() << " size mismatch.\n";
				else if(!equal(in.begin(), in.end(), out.begin()))
				{
					cerr << "Image R/W test for type " << fmt << " " << CVD::PNM::type_name<Type>::name() << " data mismatch.\n";

					typedef typename Pixel::Component<Type>::type  Ct;
					double t=0, minval = HUGE_VAL, maxval=-HUGE_VAL;
					for(int y=0; y < in.size().y; y++)
						for(int x=0; x < in.size().x; x++)
							for(unsigned int c=0; c < Pixel::Component<Type>::count; c++)
							{
								double p = Pixel::Component<Type>::get(in[y][x], c);
								double p2 = Pixel::Component<Type>::get(out[y][x], c);
								t += abs(p -  p2);

								maxval = max(maxval, p);
								minval = min(minval, p);

								cerr << p << " " << p2 << endl;

							}

					cerr << "Mismatch is " << 100* t * 1.0 / in.totalsize()/(maxval - minval) << "% per pixel\n";

					cerr << "Min is: " << minval << endl;
					cerr << "Max is: " << maxval << endl;

				}
				else
					cerr << "Image R/W test for type " << fmt << " " << CVD::PNM::type_name<Type>::name() << " OK.\n";
			}
		}
		catch(Exceptions::All w)
		{
			cerr << w.what << endl;
		}

		randtest<typename T::Next>::exec(fmt);
	}
};


template<> struct randtest<Head>
{
	static void exec(ImageType::ImageType){}
};

int main(int ac, char** av)
{

	for(int i=1; i <ac; i++)
	{
		loadsave_safe<bool>(av[i]);
		loadsave_safe<unsigned char>(av[i]);
		loadsave_safe<signed char>(av[i]);
		loadsave_safe<short>(av[i]);
		loadsave_safe<unsigned short>(av[i]);
		loadsave_safe<int>(av[i]);
		loadsave_safe<unsigned int>(av[i]);
		loadsave_safe<long>(av[i]);
		loadsave_safe<unsigned long>(av[i]);
		loadsave_safe<long long>(av[i]);
		loadsave_safe<unsigned long long>(av[i]);
		loadsave_safe<float>(av[i]);
		loadsave_safe<double>(av[i]);
		loadsave_safe<long double>(av[i]);

	
		loadsave_safe<CVD::Rgb<unsigned char> >(av[i]);
		loadsave_safe<CVD::Rgb<signed char> >(av[i]);
		loadsave_safe<CVD::Rgb<short> >(av[i]);
		loadsave_safe<CVD::Rgb<unsigned short> >(av[i]);
		loadsave_safe<CVD::Rgb<int> >(av[i]);
		loadsave_safe<CVD::Rgb<unsigned int> >(av[i]);
		loadsave_safe<CVD::Rgb<long> >(av[i]);
		loadsave_safe<CVD::Rgb<unsigned long> >(av[i]);
		loadsave_safe<CVD::Rgb<long long> >(av[i]);
		loadsave_safe<CVD::Rgb<unsigned long long> >(av[i]);
		loadsave_safe<CVD::Rgb<float> >(av[i]);
		loadsave_safe<CVD::Rgb<double> >(av[i]);
		loadsave_safe<CVD::Rgb<long double> >(av[i]);

		loadsave_safe<CVD::Rgb8>(av[i]);
		loadsave_safe<CVD::Rgba<unsigned char> >(av[i]);
		loadsave_safe<CVD::Rgba<unsigned int> >(av[i]);
	}

	cerr << "Testing TEXT (type " << ImageType::BMP << ")\n";
	randtest<
			  TypeList<double,
		      TypeList<float,
			  	       Head> > >::exec(ImageType::TEXT);

	cerr << "Testing BMP (type " << ImageType::BMP << ")\n";
	randtest<
			  TypeList<byte,
			  TypeList<Rgb<byte>,
			  	       Head> > >::exec(ImageType::BMP);
	
	cerr << "Testing PNM (type " << ImageType::PNM << ")\n";
	randtest<
			  TypeList<byte,
			  TypeList<unsigned short,
			  TypeList<Rgb<byte>,
			  TypeList<Rgb<unsigned short>,
			  	       Head> > > > >::exec(ImageType::PNM);

	#ifdef CVD_HAVE_PNG
	cerr << "Testing PNG (type " << ImageType::PNG << ")\n";
	randtest<
			  TypeList<bool,
			  TypeList<byte,
			  TypeList<unsigned short,
			  TypeList<Rgb<byte>,
			  TypeList<Rgb<unsigned short>,
			  TypeList<Rgba<byte>,
			  TypeList<Rgba<unsigned short>,
			  	       Head> > > > > > > >::exec(ImageType::PNG);
	#endif
	#ifdef CVD_HAVE_TIFF
	cerr << "Testing TIFF (type " << ImageType::TIFF << ")\n";
	randtest<
			  TypeList<bool,
			  TypeList<byte,
			  TypeList<unsigned short,
			  TypeList<float,
			  TypeList<double,

			  TypeList<Rgb<byte>,
			  TypeList<Rgb<unsigned short>,
			  TypeList<Rgb<float>,
			  TypeList<Rgb<double>,

			  TypeList<Rgba<byte>,
			  TypeList<Rgba<unsigned short>,
			  TypeList<Rgba<float>,
			  TypeList<Rgba<double>,

			  	       Head> > > > > > > > > > > > > >::exec(ImageType::TIFF);
	#endif

	exit(0);
}




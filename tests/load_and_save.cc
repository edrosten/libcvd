/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software, see LICENSE file for details
*/
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>

#include <cvd/image_io.h>

using namespace CVD;
using std::cerr;
using std::clog;
using std::endl;
using std::equal;
using std::ifstream;
using std::ios;
using std::ios_base;
using std::map;
using std::max;
using std::min;
using std::ofstream;
using std::string;
using std::stringstream;

using CVD::Internal::Head;
using CVD::Internal::TypeList;

std::mt19937 rng;

template <class C>
struct randpix
{
	static C r()
	{
		union
		{
			unsigned int i;
			C c;
		} u;

		u.i = rand();
		return u.c;
	}
};

template <>
struct randpix<bool>
{
	static bool r()
	{
		return std::uniform_int_distribution<>(0, 1)(rng);
	}
};

template <>
struct randpix<bayer_bggr>
{
	static byte r()
	{
		return static_cast<byte>(std::uniform_int_distribution<>(0, 255)(rng));
	}
};
template <>
struct randpix<bayer_rggb>
{
	static byte r()
	{
		return static_cast<byte>(std::uniform_int_distribution<>(0, 255)(rng));
	}
};

template <>
struct randpix<bayer_grbg>
{
	static byte r()
	{
		return static_cast<byte>(std::uniform_int_distribution<>(0, 255)(rng));
	}
};

template <>
struct randpix<bayer_gbrg>
{
	static byte r()
	{
		return static_cast<byte>(std::uniform_int_distribution<>(0, 255)(rng));
	}
};

template <>
struct randpix<double>
{
	static double r()
	{
		return rand() * 10. / RAND_MAX - 5;
	}
};

template <>
struct randpix<float>
{
	static float r()
	{
		return std::uniform_real_distribution<float>(-5, 5)(rng);
	}
};

template <class C>
struct randpix<Rgb<C>>
{
	static Rgb<C> r()
	{
		return Rgb<C>(randpix<C>::r(), randpix<C>::r(), randpix<C>::r());
	}
};

template <class C>
struct randpix<Rgba<C>>
{
	static Rgba<C> r()
	{
		return Rgba<C>(randpix<C>::r(), randpix<C>::r(), randpix<C>::r(), randpix<C>::r());
	}
};

template <class T>
string make_output_file_name(string fin, string type)
{
	std::string fn;

	//T tmp;

	//Remove the path from the filename
	for(int i = static_cast<int>(fin.size() - 1); i >= 0; i--)
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

template <class T>
void loadsave(string fin)
{
	Image<T> im;

	//Load the image
	clog << "Reading: " << fin << endl;
	ifstream i(fin.c_str(), ios::in | ios::binary);
	img_load(im, i);
	i.close();

	string fout = make_output_file_name<T>(fin, "pnm");
	clog << "Writing: " << fout << endl
	     << endl;

	//Save it
	ofstream o(fout.c_str(), ios::out | ios::binary);
	img_save(im, o, ImageType::PNM);
	o.close();

//Save it
#ifdef CVD_HAVE_JPEG
	fout = make_output_file_name<T>(fin, "jpg");
	clog << "Writing: " << fout << endl
	     << endl;
	o.open(fout.c_str());
	img_save(im, o, ImageType::JPEG);
	o.close();
#endif
#ifdef CVD_HAVE_PNG
	fout = make_output_file_name<T>(fin, "png");
	clog << "Writing: " << fout << endl
	     << endl;
	o.open(fout.c_str());
	img_save(im, o, ImageType::PNG);
	o.close();
#endif
#ifdef CVD_HAVE_TIFF
	fout = make_output_file_name<T>(fin, "tiff");
	clog << "Writing: " << fout << endl
	     << endl;
	o.open(fout.c_str());
	img_save(im, o, ImageType::TIFF);
	o.close();
#endif
}

template <class T>
void loadsave_safe(const char* n)
{
	try
	{
		loadsave<T>(n);
	}
	catch(const CVD::Exceptions::All& b0rk3d)
	{
		cerr << "Image " << n << " error: " << b0rk3d.what() << endl;
	}
}

map<string, Parameter<>> empty()
{
	map<string, Parameter<>> nothing;
	return nothing;
}

template <class T>
struct randtest
{
	typedef typename T::Type Type;

	static void exec(ImageType::ImageType fmt, const map<string, Parameter<>>& p = empty())
	{
		for(int i = 0; i < 10; i++)
		{
			try
			{
				//Make a random image
				Image<Type> in(ImageRef(256 + i, 256 + i)), out;

				cerr << "Testing " << in.size() << " " << fmt << " " << CVD::PNM::type_name<Type>::name() << " ";

				for(int y = 0; y < in.size().y; y++)
					for(int x = 0; x < in.size().x; x++)
						in[y][x] = randpix<Type>::r();

				stringstream s;

				//Save the image
				img_save(in, s, fmt, p);

				s.seekg(0, ios_base::beg);
				s.seekp(0, ios_base::beg);

				//Load the image
				out = img_load(s);

				//Compare the results
				if(out.size() != in.size())
					cerr << " size mismatch.\n";
				else if(!equal(in.begin(), in.end(), out.begin()))
				{
					cerr << " data mismatch.\n";

					double t = 0, minval = HUGE_VAL, maxval = -HUGE_VAL;
					for(int y = 0; y < in.size().y; y++)
						for(int x = 0; x < in.size().x; x++)
							for(unsigned int c = 0; c < Pixel::Component<Type>::count; c++)
							{
								double p = Pixel::Component<Type>::get(in[y][x], c);
								double p2 = Pixel::Component<Type>::get(out[y][x], c);
								t += abs(p - p2);

								maxval = max(maxval, p);
								minval = min(minval, p);

								//cerr << p << " " << p2 << endl;
							}

					cerr << "Mismatch is " << 100 * t * 1.0 / in.size().area() / (maxval - minval) << "% per pixel\n";

					cerr << "Min is: " << minval << endl;
					cerr << "Max is: " << maxval << endl;

					exit(1);
				}
				else
					cerr << "OK.\n";
			}
			catch(const Exceptions::All& w)
			{
				cerr << w.what() << endl;
			}
		}

		randtest<typename T::Next>::exec(fmt, p);
	}
};

template <>
struct randtest<Head>
{
	static void exec(ImageType::ImageType, const map<string, Parameter<>>& = empty())
	{
	}
};

int main(int ac, char** av)
{

	for(int i = 1; i < ac; i++)
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

		loadsave_safe<CVD::Rgb<unsigned char>>(av[i]);
		loadsave_safe<CVD::Rgb<signed char>>(av[i]);
		loadsave_safe<CVD::Rgb<short>>(av[i]);
		loadsave_safe<CVD::Rgb<unsigned short>>(av[i]);
		loadsave_safe<CVD::Rgb<int>>(av[i]);
		loadsave_safe<CVD::Rgb<unsigned int>>(av[i]);
		loadsave_safe<CVD::Rgb<long>>(av[i]);
		loadsave_safe<CVD::Rgb<unsigned long>>(av[i]);
		loadsave_safe<CVD::Rgb<long long>>(av[i]);
		loadsave_safe<CVD::Rgb<unsigned long long>>(av[i]);
		loadsave_safe<CVD::Rgb<float>>(av[i]);
		loadsave_safe<CVD::Rgb<double>>(av[i]);
		loadsave_safe<CVD::Rgb<long double>>(av[i]);

		loadsave_safe<CVD::Rgb8>(av[i]);
		loadsave_safe<CVD::Rgba<unsigned char>>(av[i]);
		loadsave_safe<CVD::Rgba<unsigned int>>(av[i]);
	}

	//Test some of the variations
	{
		std::stringstream str;
		Image<unsigned char> im(ImageRef(32, 32), 0);
		img_save(im, str, ImageType::PNM);

		str.seekg(0);
		im = img_load(str);

		str.seekg(0);
		im = img_load<PNM::Reader>(str);

		str.seekg(0);
		im = img_load<PNM::Reader, FITS::Reader>(str);

		str.seekg(0);
		im = img_load<std::tuple<PNM::Reader, FITS::Reader>>(str);

		str.seekg(0);
		im = img_load<std::tuple<PNM::Reader>>(str);

		try
		{
			str.seekg(0);
			im = img_load<FITS::Reader>(str);
			throw std::logic_error("Image load of the wrong type did not succeed");
		}
		catch(const CVD::Exceptions::Image_IO::UnsupportedImageType&)
		{
		}
	}

	cerr << "Testing TEXT (type " << ImageType::TEXT << ")\n";
	randtest<
	    TypeList<double,
	        TypeList<float,
	            Head>>>::exec(ImageType::TEXT);

	cerr << "Testing BMP (type " << ImageType::BMP << ")\n";
	randtest<
	    TypeList<byte,
	        TypeList<Rgb<byte>,
	            Head>>>::exec(ImageType::BMP);

	cerr << "Testing PNM (type " << ImageType::PNM << ")\n";
	randtest<
	    TypeList<byte,
	        TypeList<unsigned short,
	            TypeList<Rgb<byte>,
	                TypeList<Rgb<unsigned short>,
	                    Head>>>>>::exec(ImageType::PNM);

	cerr << "Testing PNM (type " << ImageType::PNM << "), text I/O\n";
	map<string, Parameter<>> p;
	p["pnm.raw"] = Parameter<bool>(0);
	randtest<
	    TypeList<byte,
	        TypeList<unsigned short,
	            TypeList<Rgb<byte>,
	                TypeList<Rgb<unsigned short>,
	                    Head>>>>>::exec(ImageType::PNM, p);

	cerr << "Testing FITS (type " << ImageType::FITS << ")\n";
	randtest<
	    TypeList<byte,
	        TypeList<short,
	            TypeList<unsigned short,
	                TypeList<int,
	                    TypeList<float,
	                        TypeList<double,
	                            TypeList<Rgb<byte>,
	                                TypeList<Rgb<short>,
	                                    TypeList<Rgb<unsigned short>,
	                                        TypeList<Rgb<int>,
	                                            TypeList<Rgb<float>,
	                                                TypeList<Rgb<double>,
	                                                    TypeList<Rgba<byte>,
	                                                        TypeList<Rgba<short>,
	                                                            TypeList<Rgba<unsigned short>,
	                                                                TypeList<Rgba<int>,
	                                                                    TypeList<Rgba<float>,
	                                                                        TypeList<Rgba<double>,
	                                                                            Head>>>>>>>>>>>>>>>>>>>::exec(ImageType::FITS);

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
	                                Head>>>>>>>>::exec(ImageType::PNG);
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

	                                                        Head>>>>>>>>>>>>>>::exec(ImageType::TIFF);
#endif
	cerr << "Testing CVD (type " << ImageType::CVD << ")\n";
	randtest<
	    TypeList<byte,
	        TypeList<bayer_bggr,
	            TypeList<bayer_rggb,
	                TypeList<bayer_grbg,
	                    TypeList<bayer_gbrg,
	                        TypeList<Rgb<byte>,
	                            TypeList<Rgba<byte>,
	                                Head>>>>>>>>::exec(ImageType::CVD);

	exit(0);
}

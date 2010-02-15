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

#include <stdio.h>
#include <endian.h> // used for byte reordering
#include "cvd/internal/io/cvdimage.h"

#include "cvd/image_io.h"
#include "cvd/config.h"
#include <iostream>
#include <iomanip>
#include <setjmp.h>
#include <algorithm>
#include <set>
#include <climits>
#include <tr1/array>
using namespace std;
using namespace std::tr1;


namespace CVD
{
namespace CVDimage
{

// helper functions for prediction

// Use a simple linewise predictor and compute the residual differences for a
// single line. The predictor uses the value stored @p predictionlen bytes in
// the past to predict the current value.
void line_diff(const byte* in, int width, byte* out, int predictionlen = 1)
{
	for (int i = 0; i < predictionlen; i++)
		out[i] = in[i];

	for (int i = predictionlen; i < width; i++)
		out[i] = (256 + in[i] - in[i-predictionlen])&255;
}

void line_undiff(const byte* in, int width, byte* out, int predictionlen = 1)
{
	for (int i = 0; i < predictionlen; i++)
		out[i] = in[i];

	for (int i = predictionlen; i < width; i++)
		out[i] = (in[i] + out[i-predictionlen])&255;
}

// helper functions for huffman coding

struct Huff{
	Huff* zero, *one, *parent;
	int symbol;
	size_t count;

	bool operator()(const Huff* l, const Huff* r) const
	{
		if(l->count < r->count)
			return 1;
		else if(l->count == r->count)
			return l->symbol < r->symbol;
		else
			return 0;
	}
};


// This function creates a histogram of the element counts. The largest #
// count is always 65535 and the smallest nonzero count is at least 1. 0 means
// no counts, so it is ignored in the huffman tree. This way, all the data
// required to rebuild the tree can be stored in 512 bytes.
void create_normalized_hist(const Image<byte>& im, array<int,256>& h)
{
        fill(h.begin(), h.end(), 0);

	for(Image<byte>::const_iterator i = im.begin(); i!=im.end(); i++)
		h[*i]++;

	int hi = *max_element(h.begin(), h.end());
	for(unsigned int i=0; i < h.size(); i++)
		if(h[i])
			h[i]  = (((uint64_t)h[i])* 65534 / hi)+1;
}




// Given a histogram of for the symbols to encode, create a tree for a
// corresponding Huffman code
Huff* create_tree(const array<int,256>& h, vector<Huff*>& symbols)
{
	set<Huff*,Huff> table;

	for(unsigned int i=0; i < 256; i++)
	{
		if(h[i])
		{
			Huff s = {0,0,0,i,h[i]};
			Huff* ss = new Huff(s);
			table.insert(ss);
			symbols.push_back(ss);
		}
	}

	//Starting negative and incrementing makes equal probability
	//symbols appear in symbol order. There are at most 2x the number of 
	//junk symbols as real ones.
	int junk_symbol=INT_MIN;
	while(table.size() > 1)
	{

		Huff* smallest = *table.begin();
		table.erase(table.begin());

		Huff* next_smallest = *table.begin();
		table.erase(table.begin());

		Huff s = {smallest, next_smallest, 0, junk_symbol++, smallest->count + next_smallest->count};
		Huff*ss = new Huff(s);
		
		smallest->parent = ss;
		next_smallest->parent=ss;
		table.insert(ss);
	}
	
	return *table.begin();
}


typedef uint16_t PackType;
static const int PackBits = sizeof(PackType) * 8;
static const int Bits_48 = 48 / PackBits == 0 ? 1 : 48/PackBits;
static const int Bits_96 = 96 / PackBits == 0 ? 1 : 96/PackBits;

struct SortIndex
{
	const array<int, 256>& d;

	SortIndex(const array<int,256>& aa)
	:d(aa)
	{}

	bool operator()(int a, int b) const
	{
		return d[a] > d[b];
	}
	
};

// given a image (or "some data") and histogram of the contained symbols,
// create a Huffman tree, encode the data and return the new code
vector<PackType> huff_compress(const Image<byte>& im, const array<int,256>& h)
{
//	cvd_timer t;

//	cout << "Hist: " << t.reset() * 1000 << endl;

	vector<Huff*> terminals;
	Huff* table = create_tree(h, terminals);

//	cout << "Tree : " << t.reset() * 1000 << endl;
	vector<vector<byte> > symbols(256);

	for(unsigned int i=0; i < terminals.size(); i++)
	{
		vector<byte> bits;

		Huff* h = terminals[i];

		int symbol = h->symbol;

		while(h != table)
		{
			Huff* parent = h->parent;

			if(h == parent->one)
				bits.push_back(1);
			else
				bits.push_back(0);
			
			h = h->parent;
		}
		
		reverse(bits.begin(), bits.end());
		symbols[symbol] = bits;

	}
//	cout << "Symbols : " << t.reset() * 1000 << endl;
	//Longest symbol is 48 bits?
	
	array<array<array<PackType, 20>, PackBits>, 256> fast_symbols;
	array<array<int, PackBits>, 256> fast_symbols_num_chunks;

	for(unsigned int i=0; i < symbols.size(); i++)
		if(symbols[i].size())
		{
			for(int off=0; off < PackBits; off++)
			{
				fast_symbols_num_chunks[i][off] = 0;

				int o=off;
				PackType chunk = 0;
				for(unsigned int j=0; j < symbols[i].size(); j++)
				{
					chunk |= ((PackType)symbols[i][j]) << (o%PackBits);
					o++;
					if(o % PackBits == 0)
					{
						fast_symbols[i][off][fast_symbols_num_chunks[i][off]++] = chunk;
						chunk = 0;
					}
				}
				
				if(o % PackBits)
					fast_symbols[i][off][fast_symbols_num_chunks[i][off]++] = chunk;
			}
		}


	vector<PackType> r2;
	r2.reserve(im.size().area()/(2*PackBits));
	int bit=0;
	for(Image<byte>::const_iterator i = im.begin(); i!=im.end(); i++)
	{
		const int off = bit % PackBits;
		
		//Deal with the first (unaligned byte)
		if(off == 0)
			r2.push_back(fast_symbols[*i][0][0]);
		else
			r2.back() |= fast_symbols[*i][off][0];

		//Deal with any remaining bytes
		for(int j=1; j < fast_symbols_num_chunks[*i][off]; j++)
			r2.push_back(fast_symbols[*i][off][j]);

		bit += symbols[*i].size();
	}


//	cout << "Fast Packing : " << t.reset() * 1000 << endl;

	return r2;
}


// given an encoded data stream and a histogram of the encoded symbols, create
// a Huffman tree, decode the data and store it in the image ret.
template<class P> void huff_decompress(const vector<P>& b, const array<int,256>& h, Image<byte>& ret)
{
	vector<Huff*> terminals;
	Huff* table = create_tree(h, terminals);

	int i=0;	
	for(Image<byte>::iterator r=ret.begin(); r != ret.end(); r++)
	{
		Huff* h = table;

		while(h->one)
		{
			bool bit =  b[i/(sizeof(P) * 8)] & ((P)1 << (i%(sizeof(P) * 8)));
			i++;
			if(bit)
				h = h->one;
			else
				h = h->zero;
		}
		
		*r = h->symbol;	
	}
}




class ReadPimpl
{
	public:
		ReadPimpl(std::istream&);
		int channels(){return m_channels;}
		long  x_size() const {return xs;}
		long  y_size() const {return ys;}
		long  elements_per_line() const {return xs * m_channels;}
		void get_raw_pixel_lines(unsigned char*, unsigned long nlines);
		~ReadPimpl();
		string datatype()
		{
			return type;
		}
		
		template<class T> void get_raw_pixel_line(T* d)
		{
			if(datatype() != PNM::type_name<T>::name())
				throw CVD::Exceptions::Image_IO::ReadTypeMismatch(datatype(), PNM::type_name<T>::name());

			get_raw_pixel_lines((unsigned char*)d, 1);
		}

	private:
		void read_header(std::istream& is);
		array<int, 256> read_hist(std::istream& is);
		vector<PackType> read_data(std::istream& is);
		long	xs, ys;
		int	m_channels;
		std::istream&	i;
		string type;
		Image<byte> diff;
		int row;
};

ImageRef reader::size()
{
	return ImageRef(t->x_size(), t->y_size());
}

void reader::get_raw_pixel_line(unsigned char* d)
{
	t->get_raw_pixel_line(d);
}

void reader::get_raw_pixel_line(Rgb<byte>* d)
{
	t->get_raw_pixel_line(d);
}

void reader::get_raw_pixel_line(Rgba<byte>* d)
{
	t->get_raw_pixel_line(d);
}

string reader::datatype()
{
	return t->datatype();
}
string reader::name()
{
	return "CVD";
}

reader::~reader()
{}

reader::reader(std::istream& i)
:t(new ReadPimpl(i))
{}


ReadPimpl::ReadPimpl(istream& in)
:i(in)
{
	row = 0;
	read_header(in);
	array<int,256> h = read_hist(in);

	diff.resize(ImageRef(xs*m_channels,ys));
	vector<PackType> d = read_data(in);

	huff_decompress(d, h, diff);
}

void ReadPimpl::read_header(istream& in)
{
	string tmp;
	getline(in, tmp);
	//cout << "header-id: '" << tmp << "'" << endl;
	if (tmp != "CVD") throw CVD::Exceptions::Image_IO::MalformedImage(string("Error in CVD image: incorrect header ID"));

	getline(in, type);
	//cout << "type: '" << type << "'" << endl;
	if (type== "unsigned char") m_channels=1;
	else if (type== "CVD::Rgb<unsigned char>") m_channels=3;
	else if (type== "CVD::Rgba<unsigned char>") m_channels=4;
	else throw CVD::Exceptions::Image_IO::MalformedImage(string("Error in CVD image: unknown data type"));

	in >> xs >> ys;
	//cout << "size: " << xs << "x" << ys << endl;

	getline(in, tmp);
	//cout << "extras: '" << tmp << "'";
}

array<int, 256> ReadPimpl::read_hist(std::istream& is)
{
	array<int, 256> h;
	for (unsigned int i = 0; i < h.size(); i++) {
		h[i] = ((is.get() & 255)<<8) | (is.get()&255);
	}
	return h;
}

vector<PackType> ReadPimpl::read_data(std::istream& is)
{
	vector<PackType> data;
	while ((!is.bad())&&(!is.eof())) {
		int bits = PackBits;
		PackType tmp = 0;
		while (bits>0) {
			tmp <<= 8;
			tmp |= is.get()&255;
			bits-=8;
		}
		data.push_back(tmp);
	}
	return data;
}

void ReadPimpl::get_raw_pixel_lines(unsigned char*data, unsigned long nlines)
{
	for(unsigned int i=0; i < nlines; i++)	
	{
		line_undiff(diff[row], xs*m_channels, data, m_channels);
		data += xs;
		row++;
	}
}

ReadPimpl::~ReadPimpl()
{
}

////////////////////////////////////////////////////////////////////////////////
//
//  Compression 
//

////////////////////////////////////////////////////////////////////////////////
//
// CVD writing.
//

class WritePimpl
{
	public:
		WritePimpl(std::ostream&, int  xsize, int ysize, const string& type, const std::string& comm="");
		int channels(){return m_channels;}
		long  x_size() const {return xs;}
		long  y_size() const {return ys;}
		void 	write_raw_pixel_lines(const unsigned char*, unsigned long);
		template<class C> 	void write_raw_pixel_line(const C*);
		~WritePimpl();
		
	private:
		void write_header(std::ostream& os);
		void write_hist(std::ostream& os, const array<int, 256>& h);
		void write_data(std::ostream& os, vector<PackType>& data);

		long	xs, ys, row;
		int	m_channels;
		std::ostream& 	o;
		string type;
		Image<byte> diff;
};



WritePimpl::WritePimpl(std::ostream& out, int xsize, int ysize, const string& t, const string& comm)
:o(out)
{
	xs = xsize;
	ys = ysize;
	type = t;
	row=0;
	
	if(type == "unsigned char")
		m_channels = 1;
	else if(type == "CVD::Rgb<unsigned char>")
		m_channels = 3;
	else if(type == "CVD::Rgba<unsigned char>")
		m_channels = 4;
	else
		throw Exceptions::Image_IO::UnsupportedImageSubType("CVDimage", type);

	diff.resize(ImageRef(xs*m_channels,ys));
}

void WritePimpl::write_raw_pixel_lines(const unsigned char* data, unsigned long nlines)
{
	if(nlines + row > (unsigned long) ys)
		throw CVD::Exceptions::Image_IO::InternalLibraryError("CVD", "Write past end of image.");
	
	for(unsigned int i=0; i < nlines; i++)	
	{
		line_diff(data, xs*m_channels, diff[row], m_channels);
		data += xs;
		row++;
	}
}

template<class C> 	void WritePimpl::write_raw_pixel_line(const C*d)
{
	if(type != PNM::type_name<C>::name())
		throw CVD::Exceptions::Image_IO::WriteTypeMismatch(type, PNM::type_name<C>::name());

	write_raw_pixel_lines((const unsigned char*)d, 1); 
}

void WritePimpl::write_header(std::ostream& os)
{
	os << "CVD\n" << type << "\n" << xs << " " << ys << "\n";
}

void WritePimpl::write_hist(std::ostream& os, const array<int, 256>& h)
{
	// have to go through the data anyway, but one might want to copy the
	// two least significant bytes out of each int, store them in a new
	// array, and dump that in just one call to ostream::write()
	for (unsigned int i = 0; i < h.size(); i++) {
		os.put((h[i]>>8)&255);
		os.put(h[i]&255);
	}
}

void WritePimpl::write_data(std::ostream& os, vector<PackType>& data)
{
	// this variant should be safe, but requires one separate call to
	// ostream::put() for each byte
	/*for (vector<PackType>::const_iterator it = data.begin(); it!=data.end(); ++it) {
		int bits = PackBits;
		while (bits>0) {
			os.put(((*it)>>(bits-8))&255);
			bits-=8;
		}
	}*/

	// this variant should be faster, but relies on PackType to be 16bit
	for (vector<PackType>::iterator it = data.begin(); it!=data.end(); ++it) *it = htobe16(*it);
	os.write((const char*)(&(data[0])), data.size()*PackBits/8);
}

WritePimpl::~WritePimpl()
{
	write_header(o);

	array<int, 256> h;
        create_normalized_hist(diff, h);
	write_hist(o, h);

	vector<PackType> b = huff_compress(diff, h);
	write_data(o, b);
}


////////////////////////////////////////////////////////////////////////////////
//
// Public interfaces to image writing.
//

writer::writer(ostream& o, ImageRef size, const string& s)
:t(new WritePimpl(o, size.x, size.y, s))
{}

writer::~writer()
{}

void writer::write_raw_pixel_line(const byte* data)
{
	t->write_raw_pixel_line(data);
}

void writer::write_raw_pixel_line(const Rgb<byte>* data)
{
	t->write_raw_pixel_line(data);
}

void writer::write_raw_pixel_line(const Rgba<byte>* data)
{
	t->write_raw_pixel_line(data);
}

}
}

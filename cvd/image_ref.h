//-*- c++ -*-
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  CVD::image.h                                                        //
//                                                                      //
//  Definitions for of template classes CVD::ImageRef and CVD::Image    //
//                                                                      //
//  derived from IPRS_* developed by Tom Drummond                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef __CVD_IMAGE_REF_H__
#define __CVD_IMAGE_REF_H__

#include <iostream>

namespace CVD {

//////////////////////////////////////
// CVD::ImageRef                    //
//////////////////////////////////////
class ImageRef
{
public:

	//Construction

	inline ImageRef();
	inline ImageRef(int xp, int yp);
	inline ImageRef(std::istream& is);

	//Iteration

	inline bool next(const ImageRef& max);
	inline bool prev(const ImageRef& max);

	inline bool next(const ImageRef& min, const ImageRef& max);
	inline bool prev(const ImageRef& min, const ImageRef& max);
	
	inline void home();
	inline void end(const ImageRef& size);


	//Operators

	inline ImageRef& 	operator=(const ImageRef& ref);
	inline bool 		operator==(const ImageRef& ref) const;
	inline bool 		operator!=(const ImageRef& ref) const;
	inline ImageRef& 	operator*=(const double scale);
	inline ImageRef&	operator/=(const double scale);
	inline ImageRef& 	operator+=(const ImageRef rhs);
	inline ImageRef& 	operator-=(const ImageRef rhs);
	inline ImageRef 	operator*(const double scale) const;
	inline ImageRef 	operator/(const double scale) const;
	inline ImageRef 	operator+(const ImageRef rhs) const;
	inline ImageRef 	operator-(const ImageRef rhs) const;
	inline ImageRef& 	operator<<=(int i);
	inline ImageRef& 	operator>>=(int i);
	inline ImageRef		operator>>(int i) const;
	inline ImageRef		operator<<(int i) const;

	//Why do these exist?
	inline ImageRef shiftl(int i) const;
	inline ImageRef shiftr(int i) const;
	
	// and now the data members (which are public!)
	int x;
	int y;

};

inline ImageRef operator*(const int scale, const ImageRef&  ref);

#include <cvd/internal/image_ref_implementation.hh>

// Streams stuff for ImageRef class //

inline std::ostream& operator<<(std::ostream& os, const ImageRef& ref)
{
	return os << "(" << ref.x << "," << ref.y << ")";
}

inline std::istream& operator>>(std::istream& is, ImageRef& ref)
{
	is >> ref.x >> ref.y;
	return is;
}

const ImageRef ImageRef_zero(0, 0);

} //namespace CVD

#endif

#ifndef CVD_ABS_H
#define CVD_ABS_H

namespace CVD
{
	template <class T> inline T abs(T t) { return t<0 ? -t : t; }
	inline unsigned char abs(unsigned char b) { return b; }
	inline unsigned short abs(unsigned short u) { return u; }
	inline unsigned int abs(unsigned int u) { return u; }
	inline unsigned long abs(unsigned long u) { return u; }
}
#endif


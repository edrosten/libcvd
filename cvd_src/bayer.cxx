#include <string.h>
#include <cvd/config.h>
#include <cvd/colourspace.h>

//Written by Ethan
//Modified by Olaf :)

//It seems that the assumption is that cameras are big endian (network byte order).
//This is correct for firewire. (ER)



namespace CVD{namespace ColourSpace{

template <class T>
struct read_host_byteorder
{
  static const T & get(const T & from) { return from; }
};

template <class T>
struct read_net_byteorder;

template <>
struct read_net_byteorder<unsigned short>
{
  static unsigned short get(const unsigned short & from) 
  {
    #ifdef CVD_ARCH_LITTLE_ENDIAN
		return ((from&0xff)<<8) | ((from&0xff00) >> 8);
	#else
		return from; 
	#endif

  }
};

#if 0
#ifdef WIN32
template <>
struct read_net_byteorder<unsigned long>
{
  static unsigned short get(const unsigned short & from) { return ntohl(from); }
};
#else
template <>
struct read_net_byteorder<uint32_t>
{
  static unsigned short get(const unsigned short & from) { return ntohl(from); }
};

#endif
#endif

// RGRGRG
// GBGBGB
// RGRGRG
// GBGBGB
template <class T, class R>
struct bayer_sample_rggb {
	static inline void upper_left(T (*out)[3], const T * row, const T * next){
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = row0;
		out[0][1] = (row1 + next0)/2;
		out[0][2] = next1;
		out[1][0] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][2] = next1;
	}
	static inline void upper_row(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = row0;
		out[0][1] = (row_1+ row1 + next0)/3;
		out[0][2] = (next_1+next1)/2;
		out[1][0] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][2] = next1;
	}
	static inline void upper_right(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = row0;
		out[0][1] = (row_1+ row1 + next0)/3;
		out[0][2] = (next_1+next1)/2;
		out[1][0] = row0;
		out[1][1] = row1;
		out[1][2] = next1;
	}

	static inline void odd_left(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][0] = (prev0 + next0)/2;
		out[0][1] = row0;
		out[0][2] = row1;
		out[1][0] = (prev0 + next0 + prev2 + next2)/4;
		out[1][1] = (row0+row2+prev1+next1)/4;
		out[1][2] = row1;
	}
	static inline void odd_row(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][0] = (prev0 + next0)/2;
		out[0][1] = row0;
		out[0][2] = (row_1+row1)/2;
		out[1][0] = (prev0 + next0 + prev2 + next2)/4;
		out[1][1] = (row0+row2+prev1+next1)/4;
		out[1][2] = row1;
	}
	static inline void odd_right(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = (prev0 + next0)/2;
		out[0][1] = row0;
		out[0][2] = (row_1+row1)/2;
		out[1][0] = (prev0 + next0)/2;
		out[1][1] = (row0+prev1+next1)/3;
		out[1][2] = row1;
	}

	static inline void even_left(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = row0;
		out[0][1] = (row1+prev0+next0)/3;
		out[0][2] = (prev1+next1)/2;
		out[1][0] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][2] = out[0][2];
	}
	static inline void even_row(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = row0;
		out[0][1] = (row_1+row1+prev0+next0)/4;
		out[0][2] = (prev_1+next_1+prev1+next1)/4;
		out[1][0] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][2] = (prev1 + next1)/2;
	}
	static inline void even_right(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = row0;
		out[0][1] = (row_1+row1+prev0+next0)/4;
		out[0][2] = (prev_1+next_1+prev1+next1)/4;
		out[1][0] = row0;
		out[1][1] = row1;
		out[1][2] = (prev1 + next1)/2;
	}

	static inline void lower_left(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][0] = prev0;
		out[0][1] = row0;
		out[0][2] = row1;
		out[1][0] = (prev0+prev2)/2;
		out[1][1] = (row0+row2+prev1)/3;
		out[1][2] = row1;
	}
	static inline void lower_row(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][0] = prev0;
		out[0][1] = row0;
		out[0][2] = (row_1+row1)/2;
		out[1][0] = (prev0 + prev2)/2;
		out[1][1] = (row0+row2+prev1)/3;
		out[1][2] = row1;
	}
	static inline void lower_right(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		out[0][0] = prev0;
		out[0][1] = row0;
		out[0][2] = (row_1+row1)/2;
		out[1][0] = prev0;
		out[1][1] = (row0+prev1)/2;
		out[1][2] = row1;
	}
};


// BGBGBG
// GRGRGR
// BGBGBG
// GRGRGR
// swap red and blue
template <class T, class R>
struct bayer_sample_bggr {
	static inline void upper_left(T (*out)[3], const T * row, const T * next){
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = row0;
		out[0][1] = (row1 + next0)/2;
		out[0][0] = next1;
		out[1][2] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][0] = next1;
	}
	static inline void upper_row(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = row0;
		out[0][1] = (row_1+ row1 + next0)/3;
		out[0][0] = (next_1+next1)/2;
		out[1][2] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][0] = next1;
	}
	static inline void upper_right(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = row0;
		out[0][1] = (row_1+ row1 + next0)/3;
		out[0][0] = (next_1+next1)/2;
		out[1][2] = row0;
		out[1][1] = row1;
		out[1][0] = next1;
	}

	static inline void odd_left(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][2] = (prev0 + next0)/2;
		out[0][1] = row0;
		out[0][0] = row1;
		out[1][2] = (prev0 + next0 + prev2 + next2)/4;
		out[1][1] = (row0+row2+prev1+next1)/4;
		out[1][0] = row1;
	}
	static inline void odd_row(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][2] = (prev0 + next0)/2;
		out[0][1] = row0;
		out[0][0] = (row_1+row1)/2;
		out[1][2] = (prev0 + next0 + prev2 + next2)/4;
		out[1][1] = (row0+row2+prev1+next1)/4;
		out[1][0] = row1;
	}
	static inline void odd_right(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = (prev0 + next0)/2;
		out[0][1] = row0;
		out[0][0] = (row_1+row1)/2;
		out[1][2] = (prev0 + next0)/2;
		out[1][1] = (row0+prev1+next1)/3;
		out[1][0] = row1;
	}

	static inline void even_left(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = row0;
		out[0][1] = (row1+prev0+next0)/3;
		out[0][0] = (prev1+next1)/2;
		out[1][2] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][0] = out[0][0];
	}
	static inline void even_row(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = row0;
		out[0][1] = (row_1+row1+prev0+next0)/4;
		out[0][0] = (prev_1+next_1+prev1+next1)/4;
		out[1][2] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][0] = (prev1 + next1)/2;
	}
	static inline void even_right(T (*out)[3], const T * previous, const T * row, const T * next){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = row0;
		out[0][1] = (row_1+row1+prev0+next0)/4;
		out[0][0] = (prev_1+next_1+prev1+next1)/4;
		out[1][2] = row0;
		out[1][1] = row1;
		out[1][0] = (prev1 + next1)/2;
	}

	static inline void lower_left(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][2] = prev0;
		out[0][1] = row0;
		out[0][0] = row1;
		out[1][2] = (prev0+prev2)/2;
		out[1][1] = (row0+row2+prev1)/3;
		out[1][0] = row1;
	}
	static inline void lower_row(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][2] = prev0;
		out[0][1] = row0;
		out[0][0] = (row_1+row1)/2;
		out[1][2] = (prev0 + prev2)/2;
		out[1][1] = (row0+row2+prev1)/3;
		out[1][0] = row1;
	}
	static inline void lower_right(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		out[0][2] = prev0;
		out[0][1] = row0;
		out[0][0] = (row_1+row1)/2;
		out[1][2] = prev0;
		out[1][1] = (row0+prev1)/2;
		out[1][0] = row1;
	}
};

// GBGBGB
// RGRGRG
// GBGBGB
// RGRGRG
template <class T, class R>
struct bayer_sample_gbrg {
	static inline void upper_left(T (*out)[3], const T * row, const T * next){
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][0] = next0;
		out[0][1] = row0;
		out[0][2] = row1;
		out[1][0] = (next0+next2)/2;
		out[1][1] = (row0+row2+next1)/3;
		out[1][2] = row1;
	}
	static inline void upper_row(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][0] = next0;
		out[0][1] = row0;
		out[0][2] = (row_1+row1)/2;
		out[1][0] = (next0+next2)/2;
		out[1][1] = (row0+row2+next1)/3;
		out[1][2] = row1;
	}
	static inline void upper_right(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][0] = next0;
		out[0][1] = row0;
		out[0][2] = (row_1+row1)/2;
		out[1][0] = next0;
		out[1][1] = (row0+next1)/2;
		out[1][2] = row1;
	}

	static inline void odd_left(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_rggb<T,R>::even_left(out, previous, row, next);
	}
	static inline void odd_row(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_rggb<T,R>::even_row(out, previous, row, next);
	}
	static inline void odd_right(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_rggb<T,R>::even_right(out, previous, row, next);
	}

	static inline void even_left(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_rggb<T,R>::odd_left(out, previous, row, next);
	}
	static inline void even_row(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_rggb<T,R>::odd_row(out, previous, row, next);
	}
	static inline void even_right(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_rggb<T,R>::odd_right(out, previous, row, next);
	}

	static inline void lower_left(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][0] = row0;
		out[0][1] = (prev0 + row1)/2;
		out[0][2] = prev1;
		out[1][0] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][2] = prev1;
	}
	static inline void lower_row(T (*out)[3], const T * previous, const T * row){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][0] = row0;
		out[0][1] = (row_1+prev0 + row1)/3;
		out[0][2] = (prev_1+prev1)/2;
		out[1][0] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][2] = prev1;
	}
	static inline void lower_right(T (*out)[3], const T * previous, const T * row){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		out[0][0] = row0;
		out[0][1] = (row_1+prev0 + row1)/3;
		out[0][2] = (prev_1+prev1)/2;
		out[1][0] = row0;
		out[1][1] = row1;
		out[1][2] = prev1;
	}
};

/*
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T prev2 = R::get(previous[2]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next_1 = R::get(next[-1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
*/
// GRGRGR
// BGBGBG
// GRGRGR
// BGBGBG
template <class T, class R>
struct bayer_sample_grbg {
	static inline void upper_left(T (*out)[3], const T * row, const T * next){
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][2] = next0;
		out[0][1] = row0;
		out[0][0] = row1;
		out[1][2] = (next0+next2)/2;
		out[1][1] = (row0+row2+next1)/3;
		out[1][0] = row1;
	}
	static inline void upper_row(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		T next2 = R::get(next[2]);
		out[0][2] = next0;
		out[0][1] = row0;
		out[0][0] = (row_1+row1)/2;
		out[1][2] = (next0+next2)/2;
		out[1][1] = (row0+row2+next1)/3;
		out[1][0] = row1;
	}
	static inline void upper_right(T (*out)[3], const T * row, const T * next){
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T next0 = R::get(next[0]);
		T next1 = R::get(next[1]);
		out[0][2] = next0;
		out[0][1] = row0;
		out[0][0] = (row_1+row1)/2;
		out[1][2] = next0;
		out[1][1] = (row0+next1)/2;
		out[1][0] = row1;
	}

	static inline void odd_left(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_bggr<T,R>::even_left(out, previous, row, next);
	}
	static inline void odd_row(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_bggr<T,R>::even_row(out, previous, row, next);
	}
	static inline void odd_right(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_bggr<T,R>::even_right(out, previous, row, next);
	}

	static inline void even_left(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_bggr<T,R>::odd_left(out, previous, row, next);
	}
	static inline void even_row(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_bggr<T,R>::odd_row(out, previous, row, next);
	}
	static inline void even_right(T (*out)[3], const T * previous, const T * row, const T * next){
		bayer_sample_bggr<T,R>::odd_right(out, previous, row, next);
	}

	static inline void lower_left(T (*out)[3], const T * previous, const T * row){
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][2] = row0;
		out[0][1] = (prev0 + row1)/2;
		out[0][0] = prev1;
		out[1][2] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][0] = prev1;
	}
	static inline void lower_row(T (*out)[3], const T * previous, const T * row){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		T row2 = R::get(row[2]);
		out[0][2] = row0;
		out[0][1] = (row_1+prev0 + row1)/3;
		out[0][0] = (prev_1+prev1)/2;
		out[1][2] = (row0+row2)/2;
		out[1][1] = row1;
		out[1][0] = prev1;
	}
	static inline void lower_right(T (*out)[3], const T * previous, const T * row){
		T prev_1 = R::get(previous[-1]);
		T prev0 = R::get(previous[0]);
		T prev1 = R::get(previous[1]);
		T row_1 = R::get(row[-1]);
		T row0 = R::get(row[0]);
		T row1 = R::get(row[1]);
		out[0][2] = row0;
		out[0][1] = (row_1+prev0 + row1)/3;
		out[0][0] = (prev_1+prev1)/2;
		out[1][2] = row0;
		out[1][1] = row1;
		out[1][0] = prev1;
	}
};

// TYPE is 0, 1, 2, 3 corresponding to the layouts RGGB = 0, GBRG = 1, GRBG = 2, BGGR = 3
//template <typename T, class SAMPLER>
//void bayer_to_rgb(const T* bggr, T* rgb, unsigned int width, unsigned int height)
template <typename T, class SAMPLER>
void bayer_to_rgb(const T* bggr, T* rgb, unsigned int width, unsigned int height)
{
  const T* row = bggr;
  const T* next = bggr+width;
  const T* prev = bggr;
  const unsigned int midcount = (width - 4)/2;

  T (*out)[3] = (T (*)[3])rgb;

  SAMPLER::upper_left(out, row, next);
  out += 2; row += 2; next += 2;
  for (unsigned int j=0; j<midcount; ++j ) {
	SAMPLER::upper_row(out, row, next);
	out += 2; row += 2; next += 2;
  }
  SAMPLER::upper_right(out, row, next);
  out += 2; row += 2; next += 2;

  // Middle rows
  for (unsigned int i=1; i<height-1; i+=2) {
	// odd row
	SAMPLER::odd_left(out, prev, row, next);
	out += 2; prev+= 2; row += 2; next += 2;
	for (unsigned int j=0; j<midcount; ++j) {
		SAMPLER::odd_row(out, prev, row, next);
		out += 2; prev+= 2; row += 2; next += 2;
	}
	SAMPLER::odd_right(out, prev, row, next);
	out += 2; prev+= 2; row += 2; next += 2;
	// even row
	SAMPLER::even_left(out, prev, row, next);
	out += 2; prev+= 2; row += 2; next += 2;
	for (unsigned int j=0; j<midcount; ++j) {
		SAMPLER::even_row(out, prev, row, next);
		out += 2; prev+= 2; row += 2; next += 2;
	}
	SAMPLER::even_right(out, prev, row, next);
	out += 2; prev+= 2; row += 2; next += 2;
  }
  // last row
  SAMPLER::lower_left(out, prev, row);
  out += 2; prev+= 2; row += 2;
  for (unsigned int j=0; j<midcount; ++j) {
	SAMPLER::lower_row(out, prev, row);
	out += 2; prev+= 2; row += 2;
  }
  SAMPLER::lower_right(out, prev, row);
}

void bayer_to_rgb_rggb(const unsigned char* rggb, unsigned char* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned char, bayer_sample_rggb<unsigned char,read_host_byteorder<unsigned char> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_gbrg(const unsigned char* rggb, unsigned char* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned char, bayer_sample_gbrg<unsigned char,read_host_byteorder<unsigned char> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_grbg(const unsigned char* rggb, unsigned char* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned char, bayer_sample_grbg<unsigned char,read_host_byteorder<unsigned char> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_bggr(const unsigned char* bggr, unsigned char* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned char, bayer_sample_bggr<unsigned char,read_host_byteorder<unsigned char> > >(bggr, rgb, width, height);
}

void bayer_to_rgb_rggb(const unsigned short* rggb, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_rggb<unsigned short,read_host_byteorder<unsigned short> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_gbrg(const unsigned short* rggb, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_gbrg<unsigned short,read_host_byteorder<unsigned short> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_grbg(const unsigned short* rggb, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_grbg<unsigned short,read_host_byteorder<unsigned short> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_bggr(const unsigned short* bggr, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_bggr<unsigned short,read_host_byteorder<unsigned short> > >(bggr, rgb, width, height);
}

void bayer_to_rgb_rggb_be(const unsigned short* rggb, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_rggb<unsigned short,read_net_byteorder<unsigned short> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_gbrg_be(const unsigned short* rggb, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_gbrg<unsigned short,read_net_byteorder<unsigned short> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_grbg_be(const unsigned short* rggb, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_grbg<unsigned short,read_net_byteorder<unsigned short> > >(rggb, rgb, width, height);
}

void bayer_to_rgb_bggr_be(const unsigned short* bggr, unsigned short* rgb, unsigned int width, unsigned int height){
	bayer_to_rgb<unsigned short, bayer_sample_bggr<unsigned short,read_net_byteorder<unsigned short> > >(bggr, rgb, width, height);
}

template <class T>
inline T cie(T * c) { return (c[0]*77 + c[1]*150 + c[2]*29)>>8; }

// TYPE is 0, 1, 2, 3 corresponding to the layouts RGGB = 0, GBRG = 1, GRBG = 2, BGGR = 3
template<class T, class SAMPLER>
void bayer_to_grey(const T* bggr, T* grey, unsigned int width, unsigned int height)
{
  const T* row = bggr;
  const T* next = bggr+width;
  const T* prev = bggr;
  const unsigned int midcount = (width - 4)/2;

  T out[2][3];

  SAMPLER::upper_left(out, row, next);
  grey[0] = cie(out[0]); grey[1] = cie(out[1]);
  grey += 2; row += 2; next += 2;
  for (unsigned int j=0; j<midcount; ++j ) {
	SAMPLER::upper_row(out, row, next);
	grey[0] = cie(out[0]); grey[1] = cie(out[1]);
	grey += 2; row += 2; next += 2;
  }
  SAMPLER::upper_right(out, row, next);
  grey[0] = cie(out[0]); grey[1] = cie(out[1]);
  grey += 2; row += 2; next += 2;

  // Middle rows
  for (unsigned int i=1; i<height-1; i+=2) {
	// odd row
	SAMPLER::odd_left(out, prev, row, next);
	grey[0] = cie(out[0]); grey[1] = cie(out[1]);
	grey += 2; prev +=2; row += 2; next += 2;
	for (unsigned int j=0; j<midcount; ++j) {
		SAMPLER::odd_row(out, prev, row, next);
		grey[0] = cie(out[0]); grey[1] = cie(out[1]);
		grey += 2; prev +=2; row += 2; next += 2;
	}
	SAMPLER::odd_right(out, prev, row, next);
	grey[0] = cie(out[0]); grey[1] = cie(out[1]);
	grey += 2; prev +=2; row += 2; next += 2;
	// even row
	SAMPLER::even_left(out, prev, row, next);
	grey[0] = cie(out[0]); grey[1] = cie(out[1]);
	grey += 2; prev +=2; row += 2; next += 2;
	for (unsigned int j=0; j<midcount; ++j) {
		SAMPLER::even_row(out, prev, row, next);
		grey[0] = cie(out[0]); grey[1] = cie(out[1]);
		grey += 2; prev +=2; row += 2; next += 2;
	}
	SAMPLER::even_right(out, prev, row, next);
	grey[0] = cie(out[0]); grey[1] = cie(out[1]);
	grey += 2; prev +=2; row += 2; next += 2;
  }
  // last row
  SAMPLER::lower_left(out, prev, row);
  grey[0] = cie(out[0]); grey[1] = cie(out[1]);
  grey += 2; row += 2; prev += 2;
  for (unsigned int j=0; j<midcount; ++j) {
	grey[0] = cie(out[0]); grey[1] = cie(out[1]);
	grey += 2; row += 2; prev += 2;
  }
  SAMPLER::lower_right(out, prev, row);
}

void bayer_to_grey_rggb(const unsigned char* rggb, unsigned char* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned char, bayer_sample_rggb<unsigned char,read_host_byteorder<unsigned char> > >(rggb, grey, width, height);
}

void bayer_to_grey_gbrg(const unsigned char* rggb, unsigned char* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned char,bayer_sample_gbrg<unsigned char,read_host_byteorder<unsigned char> > >(rggb, grey, width, height);
}

void bayer_to_grey_grbg(const unsigned char* rggb, unsigned char* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned char,bayer_sample_grbg<unsigned char,read_host_byteorder<unsigned char> > >(rggb, grey, width, height);
}

void bayer_to_grey_bggr(const unsigned char* bggr, unsigned char* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned char,bayer_sample_bggr<unsigned char,read_host_byteorder<unsigned char> > >(bggr, grey, width, height);
}

void bayer_to_grey_rggb(const unsigned short* rggb, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short, bayer_sample_rggb<unsigned short,read_host_byteorder<unsigned short> > >(rggb, grey, width, height);
}

void bayer_to_grey_gbrg(const unsigned short* rggb, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short,bayer_sample_gbrg<unsigned short,read_host_byteorder<unsigned short> > >(rggb, grey, width, height);
}

void bayer_to_grey_grbg(const unsigned short* rggb, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short,bayer_sample_grbg<unsigned short,read_host_byteorder<unsigned short> > >(rggb, grey, width, height);
}

void bayer_to_grey_bggr(const unsigned short* bggr, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short,bayer_sample_bggr<unsigned short,read_host_byteorder<unsigned short> > >(bggr, grey, width, height);
}

void bayer_to_grey_rggb_be(const unsigned short* rggb, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short, bayer_sample_rggb<unsigned short,read_net_byteorder<unsigned short> > >(rggb, grey, width, height);
}

void bayer_to_grey_gbrg_be(const unsigned short* rggb, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short,bayer_sample_gbrg<unsigned short,read_net_byteorder<unsigned short> > >(rggb, grey, width, height);
}

void bayer_to_grey_grbg_be(const unsigned short* rggb, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short,bayer_sample_grbg<unsigned short,read_net_byteorder<unsigned short> > >(rggb, grey, width, height);
}

void bayer_to_grey_bggr_be(const unsigned short* bggr, unsigned short* grey, unsigned int width, unsigned int height){
	bayer_to_grey<unsigned short,bayer_sample_bggr<unsigned short,read_net_byteorder<unsigned short> > >(bggr, grey, width, height);
}

}}

#include <cvd/fast_corner.h>

using namespace CVD;
using namespace std;

namespace CVD
{

const ImageRef fast_pixel_ring[16]=
{
	ImageRef(0,3),
	ImageRef(1,3),
	ImageRef(2,2),
	ImageRef(3,1),
	ImageRef(3,0),
	ImageRef(3,-1),
	ImageRef(2,-2),
	ImageRef(1,-3),
	ImageRef(0,-3),
	ImageRef(-1,-3),
	ImageRef(-2,-2),
	ImageRef(-3,-1),
	ImageRef(-3,0),
	ImageRef(-3,1),
	ImageRef(-2,2),
	ImageRef(-1,3),
};

ImageRef dir[17]=
{
	ImageRef(0,3),
	ImageRef(1,3),
	ImageRef(2,2),
	ImageRef(3,1),
	ImageRef(3,0),
	ImageRef(3,-1),
	ImageRef(2,-2),
	ImageRef(1,-3),
	ImageRef(0,-3),
	ImageRef(-1,-3),
	ImageRef(-2,-2),
	ImageRef(-3,-1),
	ImageRef(-3,0),
	ImageRef(-3,1),
	ImageRef(-2,2),
	ImageRef(-1,3),
	ImageRef(0,0)
};

inline int is_corner_positive(const byte* imp, const int *pointer_dir, int cb)
{
	int num_for_corner = 12;

	int num_consecutive=0;
	int first_cons=0;
	int val;
	for(int i=0; i<16; i++)
	{
		val = imp[pointer_dir[i]];
		if(val > cb)
		{
			num_consecutive++;
			
			if(num_consecutive == num_for_corner)
				return 1;
		} 
		else
		{
			if(num_consecutive == i)
				first_cons=i;

			num_consecutive=0;
		}
	}
	
	return first_cons+num_consecutive >=num_for_corner;
}

inline int is_corner_negative(const byte* imp, const int *pointer_dir, int c_b)
{
	int num_for_corner = 12;
	int num_consecutive=0;
	int first_cons=0;
	int val;
	for(int i=0; i<16; i++)
	{
		val = imp[pointer_dir[i]];
		if(val < c_b)
		{
			num_consecutive++;

			if(num_consecutive == num_for_corner)
				return 2;
		} 
		else 
		{
			if(num_consecutive == i)
				first_cons=i;

			num_consecutive=0;
		}
	}

	return first_cons+num_consecutive >=num_for_corner;
}

void fast_corner_detect(const BasicImage<byte>& im, vector<ImageRef>& corners, int barrier)
{
	int boundary = 3, y;
	const byte* imp, *line_max, *line_min;
	

	//Create a list of integer pointer offstes, corresponding to the 
	//direction offsets in dir[]
	int	pointer_dir[16];
	for(int i=0; i < 16; i++)
		pointer_dir[i] = dir[i].x + dir[i].y * im.size().x;
	
	const byte* high_row, *low_row;

	int num_above;	//Number of pixels above center+barrier in value
	int num_below;
	int cb;			//center + barrier
	int c_b;		//center - barrier

	for(y = boundary ; y < im.size().y - boundary; y++)
	{
		imp = &im[ImageRef(boundary, y)];
		high_row = &im[ImageRef(boundary, y-3)];
		low_row = &im[ImageRef(boundary, y+3)];
		line_max = &im[ImageRef(im.size().x - boundary, y)];
		line_min = imp - boundary;

		for(; imp < line_max; imp++, high_row++, low_row++)
		{
			cb = *imp + barrier;
			c_b = *imp - barrier;
			
			//Look above first, then below
			num_above= (*high_row > cb);
			num_below= (*high_row < c_b);
			num_above+= (*low_row > cb);
			num_below+= (*low_row < c_b);

			if(!num_above && !num_below)
				continue;
			
			//Look left
			num_above+= (*(imp-3) > cb);
			num_below+= (*(imp-3) < c_b);

			if(num_above&2) //num_above is 2 or 3
			{	
				if(!(num_above & 1)) //num_above is 2
					num_above+= (*(imp+3) > cb);
				
				//Only do a detailed look of num_above is 3
				if((num_above &1) && is_corner_positive(imp, pointer_dir, cb))
					corners.push_back(ImageRef(imp-line_min, y));
			}
			else if(num_below & 2)
			{
				if(!(num_below & 1)) //num_above is 2
					num_below+= (*(imp+3) < c_b);

				if((num_below &1) && is_corner_negative(imp, pointer_dir, c_b))
					corners.push_back(ImageRef(imp-line_min, y));
			}
		}
	}
}



struct CornerPositive { inline static int sub(int a, int b) { return a-b; } };
struct CornerNegative { inline static int sub(int a, int b) { return b-a; } };

template<class Type, int num_for_corner> inline int is_corner_with_diff(const byte* imp, int c_and_b)
{
	//templating on num_for_corner makes a small increase in speed.

	//Return amount by which the worst good pixel passes the test
	//or 0 if its not a corner.
	int num_consecutive=0;
	int first_cons=0;
	int val;
	int min_good_diff=INT_MAX;
	
	//This is an approximation of 

	for(int i=0; i<16; i++)
	{
		val = Type::sub(imp[i], c_and_b);

		if(val > 0)
		{
			if(val < min_good_diff)
				min_good_diff = val;

			num_consecutive++;
			
			if(num_consecutive == num_for_corner)
				return min_good_diff;
		} 
		else
		{
			if(num_consecutive == i)
				first_cons=i;


			num_consecutive=0;
		}
	}
	
	if(first_cons+num_consecutive >=num_for_corner)
		return min_good_diff;
	else 
		return 0;
}


//For N <=8, features can be positive or negatve OR BOTH!
template<int Num, bool n_is_lessthan_or_equal_to_8> struct MaxThreshold
{
	static int eval(const byte* imp, const int *pointer_dir, int start)
	{	
		//Could start at t=0, but this saves 25% of the number of is_corner_with_diff calls.
		//average of 3.01 instead of 4.20 calls
		int t=start+1;
		int s;

		//Benchmarking shows that this step is worth the effort:
		//0.77 versus 0.79 us per feature on a PIII 1GHz.
		byte data[16];
		for(int i=0; i < 16; i++)
			data[i] = imp[pointer_dir[i]];

		s = is_corner_with_diff<CornerPositive,Num>(data, *imp+t);
		if(s > 0)
		{
			while(s)
			{
				t += s;
				s = is_corner_with_diff<CornerPositive,Num>(data, *imp+t);
			}
			return t-1;
		}

		s = is_corner_with_diff<CornerNegative,Num>(data, *imp-t);
		if(s > 0)
		{
			while(s)
			{
				t += s;
				s = is_corner_with_diff<CornerNegative,Num>(data, *imp-t);
			}
			return t-1;
		}
		return t-1;
	}
};


//For N <=8, features can be positive or negatve OR BOTH!
template<int Num> struct MaxThreshold<Num, true>
{
	static int eval(const byte* imp, const int *pointer_dir, int start)
	{	
		int t=start+1;
		int s;

		byte data[16];
		for(int i=0; i < 16; i++)
			data[i] = imp[pointer_dir[i]];
		

		//Test brighter corners
		s = is_corner_with_diff<CornerPositive,Num>(data, *imp+t);
		while(s)
		{
			t += s;
			s = is_corner_with_diff<CornerPositive,Num>(data, *imp+t);
		}
		
		//Test darker corners with the new threshold
		s = is_corner_with_diff<CornerNegative,Num>(data, *imp-t);
		while(s)
		{
			t += s;
			s = is_corner_with_diff<CornerNegative,Num>(data, *imp-t);
		}
		return t-1;
	}
};

template<int Num> void fast_score(const BasicImage<byte> im, const vector<ImageRef> corners, vector<int>& ret, int start)
{
	int	pointer_dir[16];
	for(int i=0; i < 16; i++)
		pointer_dir[i] = fast_pixel_ring[i].x + fast_pixel_ring[i].y * im.size().x;
	
	ret.resize(corners.size());
	
	for(unsigned int i=0; i < corners.size(); i++)
		ret[i] = MaxThreshold<Num, (Num <=8)>::eval(&im[corners[i]], pointer_dir, start);
}


void _fast_score_inst()
{	
	vector<ImageRef> c;
	vector<int> s;
	Image<byte> i;
	fast_score<7>(i,c,s,1);
	fast_score<8>(i,c,s,1);
	fast_score<9>(i,c,s,1);
	fast_score<10>(i,c,s,1);
	fast_score<11>(i,c,s,1);
	fast_score<12>(i,c,s,1);
}

int corner_score(const BasicImage<byte>& im, ImageRef c, const int *pointer_dir, int barrier)
{
	//The score for a positive feature is sum of the difference between the pixels
	//and the barrier if the difference is positive. Negative is similar.
	//The score is the max of those two.
	//
	// B = {x | x = points on the Bresenham circle around c}
	// Sp = { I(x) - t | x E B , I(x) - t > 0 }
	// Sn = { t - I(x) | x E B, t - I(x) > 0}
	//
	// Score = max sum(Sp), sum(Sn)

	const byte* imp = &im[c];
	
	int cb = *imp + barrier;
	int c_b = *imp - barrier;
	int sp=0, sn = 0;

	for(int i=0; i<16; i++)
	{
		int p = imp[pointer_dir[i]];

		if(p > cb)
			sp += p-cb;
		else if(p < c_b)
			sn += c_b-p;
	}
	
	if(sp > sn)
		return sp;
	else 
		return sn;
}

// fast_nonmax_t is templated so you can have either of:
//  1: A vector of ImageRefs of the nonmax corners
//  2: A vector of <ImageRef, int> pairs of the corners and their scores.
//  It's internal, the user-visible functions instantiate it below..
template<class ReturnType, class Collector>
inline void fast_nonmax_t(const BasicImage<byte>& im, const vector<ImageRef>& corners, int barrier, vector<ReturnType>& nonmax_corners)
{
  if(corners.size() < 5)
    return;
  
  ImageRef size = im.size();
  nonmax_corners.clear();
  
  //Create a list of integer pointer offstes, corresponding to the 
  //direction offsets in dir[]
  int	pointer_dir[16];
  for(int i=0; i < 16; i++)
    pointer_dir[i] = dir[i].x + dir[i].y * size.x;
  
  //Compute the score for each detected corner, and find where each row begins
  // (the corners are output in raster scan order). A beginning of -1 signifies
  // that there are no corners on that row.
  
  vector<int> row_start(size.y);
  vector<int> scores(corners.size());
  
  for(int i=0; i <size.y; i++)
    row_start[i] = -1;
  
  int prev_row = -1;
  
  for(unsigned int i=0; i< corners.size(); i++)
    {
      if(corners[i].y != prev_row)
	{
	  row_start[corners[i].y] = i;
	  prev_row = corners[i].y;
	}
      
      if(corners[i].y < im.size().y - 4)
	prefetch(im[ (corners[i].y+4)]);
      
      scores[i] = corner_score(im, corners[i], pointer_dir, barrier);
    }
  
  
  //Point above points (roughly) to the pixel above the one of interest, if there
  //is a feature there.
  int point_above = 0;
  int point_below = 0;
  
  
  
  for(unsigned int i=1; i < corners.size()-1; i++)
    {
      int score = scores[i];
      ImageRef pos = corners[i];
      
      //Check left
      if(corners[i-1] == pos-ImageRef(1,0) && scores[i-1] > score)
	continue;
      
      //Check right
      if(corners[i+1] == pos+ImageRef(1,0) && scores[i+1] > score)
	continue;
      
      //Check above
      if(pos.y != 0 && row_start[pos.y - 1] != -1) 
	{
	  if(corners[point_above].y < pos.y - 1)
	    point_above = row_start[pos.y-1];
	  
	  //Make point above point to the first of the pixels above the current point,
    //if it exists.
	  for(; corners[point_above].y < pos.y &&
        corners[point_above].x < pos.x - 1; point_above++);
	  
	  
	  for(int i=point_above; corners[i].y < pos.y && corners[i].x <= pos.x + 1; i++)
	    {
	      int x = corners[i].x;
	      if( (x == pos.x - 1 || x ==pos.x || x == pos.x+1) && scores[i] > score)
		{
		  goto cont;
		}
	    }
	  
	}
      
      //Check below
      if(pos.y != size.y-1 && row_start[pos.y + 1] != -1) //Nothing below
	{
	  if(corners[point_below].y < pos.y + 1)
	    point_below = row_start[pos.y+1];
	  
	  // Make point below point to one of the pixels belowthe current point, if it
    // exists.
	  for(; corners[point_below].y == pos.y+1 &&
        corners[point_below].x < pos.x - 1; point_below++);
	  
	  for(int i=point_below; corners[i].y == pos.y+1 && corners[i].x <= pos.x + 1; i++)
	    {
	      int x = corners[i].x;
	      if( (x == pos.x - 1 || x ==pos.x || x == pos.x+1) && scores[i] > score)
		{
		  goto cont;
		}
	    }
	}
      
      nonmax_corners.push_back(Collector::collect(corners[i],scores[i]));
      
    cont:
      ;
    }
}
  
// The two collectors which either return just the ImageRef or the <ImageRef,int> pair
struct collect_pos
{
  static inline ImageRef collect(const ImageRef& pos, int score){return pos;}
};

struct collect_score
{
  static inline pair<ImageRef, int> collect(const ImageRef& pos, int score){return make_pair(pos,score);}
};

// The callable functions
void fast_nonmax(const BasicImage<byte>& im, const vector<ImageRef>& corners,
		 int barrier, vector<ImageRef>& nonmax_corners)
{
  fast_nonmax_t<ImageRef, collect_pos>(im, corners, barrier, nonmax_corners);
}

void fast_nonmax_with_scores(const BasicImage<byte>& im, const vector<ImageRef>& corners, int barrier, vector<pair<ImageRef,int> >& nonmax_corners)
{
  fast_nonmax_t<pair<ImageRef,int> , collect_score>(im, corners, barrier,
						    nonmax_corners);
}


}

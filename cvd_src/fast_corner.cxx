#include <cvd/fast_corner.h>

using namespace CVD;
using namespace std;

namespace CVD
{

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
				if(!num_above & 1) //num_above is 2
					num_above+= (*(imp+3) > cb);
				
				//Only do a detailed look of num_above is 3
				if((num_above &1) && is_corner_positive(imp, pointer_dir, cb))
					corners.push_back(ImageRef(imp-line_min, y));
			}
			else if(num_below & 2)
			{
				if(!num_below & 1) //num_above is 2
					num_below+= (*(imp+3) > cb);

				if((num_below &1) && is_corner_negative(imp, pointer_dir, cb))
					corners.push_back(ImageRef(imp-line_min, y));
			}
		}
	}
}

}

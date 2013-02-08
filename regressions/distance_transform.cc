#include <cvd/distance_transform.h>
#include <cvd/image_io.h>
#include <cvd/random.h>
#include <cvd/image_convert.h>
#include <utility>
using namespace CVD;
using namespace std;

int main()
{

	
	for(int i=0; i < 1000; i++)
	{
		ImageRef s(50 + rand()%1000, 50%rand()%1000);

		Image<byte> in(s);

		Image<Rgb<byte> > o(s);

		o.zero();
		in.zero();

		for(int i=1; i < 1 + rand()%1000; i++)
			in[rand() % s.y][rand()%s.x] = 1;

		Image<float> out(in.size());
		Image<ImageRef> adt(in.size());


		euclidean_distance_transform(in, out, adt);
		
		for(ImageRef p(-1,0); p.next(in.size()); )
		{
			float d = sqrt((p - adt[p]).mag_squared());

			if(abs(d - out[p]) > 1e-3 && adt[p] != ImageRef(-1,-1))
			{
				cerr << "Error.\n";
				cerr << d << endl;
				cout << out[p] << endl;
				cout << adt[p] << endl;
				return 1;
			}	
		}
	}
}

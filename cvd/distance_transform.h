#ifndef CVD_INC_DISTANCE_TRANSFORM_HPP
#define CVD_INC_DISTANCE_TRANSFORM_HPP

#include <cmath>
#include <cvd/vision_exceptions.h>

#include <cvd/image.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>

namespace CVD {


	template <class Precision = double>
		class DistanceTransformEuclidean {
			
			// Implements
			// Distance Transforms of Sampled Functions
			//   Pedro F. Felzenszwalb Daniel P. Huttenlocher



			public:
				DistanceTransformEuclidean() 
				:sz(ImageRef(-1,-1)),
				 big_number(1e20) //Hmm, why doesn't HUGE_VAL work?
				 {}

			private:
				inline void transform_row(const int n) {
					v[0] = 0; //std::numeric_limits<Precision>::infinity();
					z[0] = -big_number; //std::numeric_limits<Precision>::infinity();
					z[1] = +big_number; // std::numeric_limits<Precision>::infinity();
					int k = 0;
					for (int q = 1; q < n; q++) {
						Precision s = ((f[q]+(q*q))-(f[v[k]]+(v[k]*v[k])))/(2*q-2*v[k]);
						while (s <= z[k]) {
							k--;
							s = ((f[q]+(q*q))-(f[v[k]]+(v[k]*v[k])))/(2*q-2*v[k]);
						}
						k++;
						v[k] = q;
						z[k] = s;
						z[k+1] = +big_number; //std::numeric_limits<Precision>::infinity();
					}
					k = 0;
					for (int q = 0; q < n; q++) {
						while (z[k+1] < q) {
							k++;
						}
						d[q] = ((q - v[k]) * (q - v[k])) + f[v[k]];
						pos[q] = q > v[k] ? (q - v[k]) : (v[k] - q);
						//cout << "n=" << n << " q=" << q << " d[q]=" << d[q] << " v[k]=" << v[k] << " f[v[k]]=" << f[v[k]] << endl;
					}
				}

				void transform_image(SubImage<Precision> &DT) {
					const ImageRef img_sz(DT.size());
					for (int x = 0; x < img_sz.x; x++) {
						for (int y = 0; y < img_sz.y; y++) {
							f[y] = DT[y][x];
						}
						transform_row(img_sz.y);
						for (int y = 0; y < img_sz.y; y++) {
							DT[y][x] = d[y];
						}
					}
					//#if 0
					for (int y = 0; y < img_sz.y; y++) {
						for (int x = 0; x < img_sz.x; x++) {
							f[x] = DT[y][x];
						}
						transform_row(img_sz.x);
						for (int x = 0; x < img_sz.x; x++) {
							DT[y][x] = d[x];
						}
					}
					//#endif
				}

				template <class Functor>
				/// Perform distance transform with reverse lookup.
				/// @param ADT which edge pixel is closest to this one
				void transform_image_with_ADT(Image <Precision> &DT, Image <ImageRef> &ADT, const Functor& f) {
					const ImageRef img_sz(DT.size());
					const double maxdist = img_sz.x * img_sz.y;
					for (int x = 0; x < img_sz.x; x++) {
						for (int y = 0; y < img_sz.y; y++) {
							f[y] = DT[y][x];
						}
						transform_row(img_sz.y);
						for (int y = 0; y < img_sz.y; y++) {
							DT[y][x] = d[y];
						}
					}
					for (int y = 0; y < img_sz.y; y++) {
						for (int x = 0; x < img_sz.x; x++) {
							f[x] = DT[y][x];
						}
						transform_row(img_sz.x);
						for (int x = 0; x < img_sz.x; x++) {
							DT[y][x] = d[x];
							const double hyp = d[x];
							if (hyp >= maxdist) {
								ADT[y][x] = ImageRef(-1, -1);
								continue;
							}
							const double dx = pos[x];
							const double dy = sqrt(hyp - dx * dx);
							const int ddy = dy;
							const ImageRef candA(x - dx, y - ddy);
							const ImageRef candB(x - dx, y + ddy);
							const ImageRef candC(x + dx, y - ddy);
							const ImageRef candD(x + dx, y + ddy);
							/** cerr << "hyp=" << hyp << " dx="<< dx << " dy=" << dy << " ddy=" << ddy
							  << " A=" << candA << " B=" << candB << " C=" << candC << " D=" << candD << endl;*/
							if (DT.in_image(candA) && f(candA)) {
								ADT[y][x] = candA;
							}
							else if (DT.in_image(candB) && f(candB)) {
								ADT[y][x] = candB;
							}
							else if (DT.in_image(candC) && f(candC)) {
								ADT[y][x] = candC;
							}
							else if (DT.in_image(candD) && f(candD)) {
								ADT[y][x] = candD;
							}
							else {
								//ADT[y][x] = ImageRef(-1,-1);
								/**cerr << hyp << " " << ImageRef(x, y);*/
								throw std::string("feature with onval set not found");
							}
						}
					}
				}

				void resize(const ImageRef &new_size) {
					if (new_size != sz) {
						sz = new_size;
						int m(std::max(new_size.x, new_size.y));
						d.resize(m);
						v.resize(m);
						f.resize(m);
						pos.resize(m);
						z.resize(m+1);
					}
				}


			public:
				template <class T, class Q, class Functor>
				void transform_ADT(const SubImage<T>& feature, SubImage<Precision> &DT, SubImage<ImageRef> &ADT) {
					if(feature.size() != DT.size())
						throw Exceptions::Vision::IncompatibleImageSizes(__FUNCTION__);
						
					if(feature.size() != ADT.size())
						throw Exceptions::Vision::IncompatibleImageSizes(__FUNCTION__);

					resize(feature.size());

					apply_functor(DT, NotZero<T>(feature));

					transform_image_with_ADT(DT, ADT, NotZero<T>(feature));
					for (int y = 0; y < DT.size().y; y++) {
						for (int x = 0; x < DT.size().x; x++) {
							DT[y][x] = sqrt(DT[y][x]);
						}
					}
				}

				void transform(SubImage<Precision> &out) {
					resize(out.size());
					//DistanceTransformPreProcess<T, Precision> preprocess;
					//preprocess(feature, onval, out);
					transform_image(out);
					for (int y = 0; y < out.size().y; y++) {
						for (int x = 0; x < out.size().x; x++) {
							out[y][x] = sqrt(out[y][x]);
							//cout << out[y][x] << ",";
						}
					}
				}


				template<class C>
				struct NotZero
				{
					NotZero(const SubImage<C>& im_)
					:im(im_)
					{}

					const SubImage<C>& im;
					bool operator()(const ImageRef& i) const
					{
						return im[i] != 0;
					}
				};
				
				template<class Out, class Functor>
				void apply_functor(SubImage<Out>& out, const Functor& f)
				{
					for (int y = 0; y < out.size().y; y++)
						for (int x = 0; x < out.size().x; x++)
							if (f(ImageRef(x, y)))
								out[y][x] = 0;
							else 
								out[y][x] = big_number;
				}

			private:
				ImageRef sz;
				double big_number;
				std::vector <Precision> d;
				std::vector <int> v;
				std::vector <Precision> z;
				std::vector <Precision> f;
				std::vector <Precision> pos;
		};

		
	
	template <class T, class Q>
	///Compute Euclidean distance transform using the Felzenszwalb & Huttenlocher algorithm.
	///@ingroup gVision
	///@param in input image: thresholded so anything &gt; 0 is on the object
	///@param out output image is euclidean distance of input image.
	void euclidean_distance_transform(const SubImage<T> &in, SubImage<Q> &out) {
		if(in.size() != out.size())
			throw Exceptions::Vision::IncompatibleImageSizes(__FUNCTION__);
		DistanceTransformEuclidean<Q> dt;
		dt.apply_functor(out,  typename DistanceTransformEuclidean<Q>::template NotZero<T>(in));
		dt.transform(out);
	}


	///Compute Euclidean distance transform using the Felzenszwalb & Huttenlocher algorithm.
	///@ingroup gVision
	///@param in input image: thresholded so anything &gt; 0 is on the object
	///@param out output image is euclidean distance of input image.
	template<class T, class Q>
	void euclidean_distance_transform(const SubImage<T> &in, SubImage<Q> &out, SubImage<ImageRef>& lookup_DT) {
		if(in.size() != out.size())
			throw Exceptions::Vision::IncompatibleImageSizes(__FUNCTION__);
		DistanceTransformEuclidean<Q> dt;
		dt.transform_ADT(in, out, lookup_DT);
	}


	#ifndef DOXYGEN_IGNORE_INTERNAL
		namespace Internal
		{
			template<class C>
			class DoDistanceTransform{};

			template<class T> struct ImagePromise<DoDistanceTransform<T> >
			{
				ImagePromise(const SubImage<T>&in_)
				:in(in_){}

				const SubImage<T>& in;

				template<class C> void execute(Image<C>& im)
				{
					im.resize(in.size());
					euclidean_distance_transform(in, im);
				}
			};	
		};	
			
		template <class T>
		Internal::ImagePromise<Internal::DoDistanceTransform<T> > euclidean_distance_transform(const SubImage<T> &in)
		{
			using namespace Internal;
			return ImagePromise<DoDistanceTransform<T> >(in);
		}
	#else

		///Compute Euclidean distance transform using the Felzenszwalb & Huttenlocher algorithm.
		///@ingroup gVision
		///@param in input image: thresholded so anything &gt; 0 is on the object
		///@returns output image is euclidean distance of input image.
		template <class T>
		Image euclidean_distance_transform(const SubImage<T> &in);
	#endif
};
#endif

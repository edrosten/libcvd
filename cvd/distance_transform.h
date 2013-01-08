#ifndef CVD_INC_DISTANCE_TRANSFORM_HPP
#define CVD_INC_DISTANCE_TRANSFORM_HPP

#include <cmath>
#include <cvd/vision_exceptions.h>

#include <cvd/image.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>

namespace Granta {

	using namespace CVD;
	using namespace std;


	template <class Precision = double>
		class DistanceTransformEuclidean {
			public:
				DistanceTransformEuclidean() : sz(ImageRef(-1,-1)) {}

			private:
				inline void transform_row(const int n) {
					v[0] = 0; //std::numeric_limits<Precision>::infinity();
					z[0] = -HUGE_VAL; //std::numeric_limits<Precision>::infinity();
					z[1] = +HUGE_VAL; // std::numeric_limits<Precision>::infinity();
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
						z[k+1] = +HUGE_VAL; //std::numeric_limits<Precision>::infinity();
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

				void transform_image(Image <Precision> &DT) {
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

				template <class T>
					void transform_image_with_ADT(const Image <T> &feature, Image <Precision> &DT, Image <ImageRef> &ADT, T onval) {
						const ImageRef img_sz(feature.size());
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
								if (DT.in_image(candA) && feature[candA] == onval) {
									ADT[y][x] = candA;
								}
								else if (DT.in_image(candB) && feature[candB] == onval) {
									ADT[y][x] = candB;
								}
								else if (DT.in_image(candC) && feature[candC] == onval) {
									ADT[y][x] = candC;
								}
								else if (DT.in_image(candD) && feature[candD] == onval) {
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

				template <class T, class Q> 
					void find_onvals(const Image<T> &feature, const Q onval, Image <Precision> &out) {
						for (int y = 0; y < out.size().y; y++) {
							for (int x = 0; x < out.size().x; x++) {
								if (feature[y][x] == onval) {
									out[y][x] = 0;
								}
								else {
									out[y][x] = HUGE_VAL;
								}
							}
						}
					}

			public:
				template <class T, class Q>
					void transform_ADT(const Image <T> &feature, Image <Precision> &DT, Image <ImageRef> &ADT, const Q onval) {
						if(feature.size() != DT.size())
							throw Exceptions::Vision::IncompatibleImageSizes(__FUNCTION__);
							
						if(feature.size() != ADT.size())
							throw Exceptions::Vision::IncompatibleImageSizes(__FUNCTION__);

						resize(feature.size());
						find_onvals(feature, onval, DT);
						transform_image_with_ADT(feature, DT, ADT, onval);
						for (int y = 0; y < DT.size().y; y++) {
							for (int x = 0; x < DT.size().x; x++) {
								DT[y][x] = sqrt(DT[y][x]);
							}
						}
					}

				template <class T, class Q>
					void transform(const Image<T> &feature, const Q onval, Image <Precision> &out) {

						if(feature.size() == out.size())
							throw Exceptions::Vision::IncompatibleImageSizes(__FUNCTION__);
							
						resize(feature.size());
						//DistanceTransformPreProcess<T, Precision> preprocess;
						//preprocess(feature, onval, out);
						find_onvals(feature, onval, out);
						transform_image(out);
						for (int y = 0; y < out.size().y; y++) {
							for (int x = 0; x < out.size().x; x++) {
								out[y][x] = sqrt(out[y][x]);
								//cout << out[y][x] << ",";
							}
						}
					}

			private:
				ImageRef sz;
				std::vector <Precision> d;
				std::vector <int> v;
				std::vector <Precision> z;
				std::vector <Precision> f;
				std::vector <Precision> pos;
		};

	template <class T, class Q>
		void euclidean_distance_transform(const Image<T> &in, const Image<Q> &out) {
			DistanceTransformEuclidean<Q> dt;
			dt.transform(in, out);
		}
};

#endif

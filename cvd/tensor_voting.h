#ifndef CVD_INC_TENSOR_VOTE_H
#define CVD_INC_TENSOR_VOTE_H

#include <cvd/image.h>
#include <TooN/TooN.h>
#include <TooN/helpers.h>
#include <vector>
#include <utility>

namespace CVD
{
	
	#ifndef DOXYGEN_IGNORE_INTERNAL
	namespace TensorVoting
	{
		std::vector<std::pair<int, TooN::Matrix<2> > > compute_a_tensor_kernel(int radius, double cutoff, double angle, double sigma, double ratio, int row_stride);
		unsigned int quantize_half_angle(double r, int num_divs);
	}

	#endif

	/**
	This function performs tensor voting on the gradients of an image. The
	voting is performed densely at each pixel, and the contribution of each
	pixel is scaled by its gradient magnitude. The kernel for voting is
	computed as follows.  Consider that there is a point at \f$(0,0)\f$, with
	gradient normal \f$(0,1)\f$. This will make a contribution to the point
	\f$(x,y)\f$.
	
	The arc-length, \f$l\f$, of the arc passing through \f$(0,0)\f$, tangent to
	the gradient at this point and also passing through \f$(x, y)\f$ is:
	\f[
		l = 2 r \theta
	\f]
	Where
	\f[
		\theta = \tan^{-1}\frac{y}{x}
	\f]
	and the radius of the arc, \f$r\f$ is:
	\f[
		r = \frac{x^2 + y^2}{2y}.
	\f]

	The scale of the contribution is:
	\f[
		s = e^{-\frac{l^2}{\sigma^2} - \kappa\frac{\sigma^2}{r^2}}.
	\f]
	Note that this is achieved by scaling \f$x\f$ and \f$y\f$ by \f$\sigma\f$, so
	\f$\kappa\f$ controls the kernel shape independent of the size.
	The complete tensor contribution is therefore:
	\f[
		e^{-\frac{l^2}{\sigma^2} - \kappa\frac{\sigma^2}{r^2}} 
							\left[
								\begin{array}{c}
									\cos 2\theta\\
									\sin 2\theta
								\end{array}
							\right]
							[ \cos 2\theta\ \ \sin 2\theta]
	\f]


	@param image    The image on which to perform tensor voting
	@param sigma    \f$ \sigma \f$
	@param ratio    \f$ \kappa \f$
	@param cutoff   When \f$s\f$ points drop below the cutoff, it is set to zero.
	@param num_divs The voting kernels are quantized by angle in to this many dicisions in the half-circle.
	@ingroup gVision
	**/
	template<class C> Image<TooN::Matrix<2> > dense_tensor_vote_gradients(const SubImage<C>& image, double sigma, double ratio, double cutoff=0.001, int num_divs = 4096)
	{
		using TooN::Matrix;
		using std::pair;
		using std::vector;

		Matrix<2> zero;
		TooN::Zero(zero);
		Image<Matrix<2> > field(image.size(), zero);


		//Kernel values go as exp(-x*x / sigma * sigma)
		//So, for cutoff = exp(-x*x / sigma * sigma)
		//ln cutoff = -x*x / sigma*sigma
		//x = sigma * sqrt(-ln cutoff)
		int kernel_radius =  (int)ceil(sigma * sqrt(-log(cutoff)));


		//First, build up the kernels
		vector<vector<pair<int, Matrix<2> > > > kernels;
		for(unsigned int i=0; i < num_divs; i++)
		{
			double angle =  M_PI * i / num_divs;
			kernels.push_back(TensorVoting::compute_a_tensor_kernel(kernel_radius, cutoff, angle, sigma, ratio, field.row_stride()));
		}
		
		
		for(int y= kernel_radius; y < field.size().y - kernel_radius; y++)
			for(int x= kernel_radius; x < field.size().x - kernel_radius; x++)
			{
				double gx = ((double)image[y][x+1] - image[y][x-1])/2.;
				double gy = ((double)image[y+1][x] - image[y-1][x])/2.;

				double scale = sqrt(gx*gx + gy*gy);
				unsigned int direction = TensorVoting::quantize_half_angle(M_PI/2 + atan(gy / gx), num_divs);

				const vector<pair<int, Matrix<2> > >& kernel = kernels[direction];

				Matrix<2>* p = &field[y][x];

				for(unsigned int i=0; i < kernel.size(); i++)
					p[kernel[i].first] += kernel[i].second * scale;
			}

		
		return field;
	}
}


#endif

#ifndef CVD_PIXEL_TRAITS_H_
#define CVD_PIXEL_TRAITS_H_

#include <limits>

#if defined(CVD_HAVE_TOON)
#include <TooN/TooN.h>
#endif

namespace CVD
{
namespace Pixel
{

	template <class T, int LIFT = 0>
	struct traits
	{
		static_assert(std::numeric_limits<T>::is_specialized == true, "Traits not defined for this type");
		using element_type = T;
		using wider_type = decltype(T {} * T {});
		typedef float float_type;
		static const bool integral = true;
		static const bool is_signed = std::numeric_limits<T>::is_signed;
		static_assert(std::numeric_limits<T>::is_integer == true, "Library bug, we shouldn't be here");
		static const int bits_used = std::numeric_limits<T>::digits;
		static const T max_intensity = std::numeric_limits<T>::max();
	};

	template <int LIFT>
	struct traits<float, LIFT>
	{
		using element_type = float;
		typedef float wider_type;
		typedef float float_type;
		static const bool integral = false;
		static const bool is_signed = true;
		static const float max_intensity;
	};

	template <int LIFT>
	const float traits<float, LIFT>::max_intensity = 1.0f;

	template <int LIFT>
	struct traits<double, LIFT>
	{
		using element_type = double;
		typedef double wider_type;
		typedef double float_type;
		static const bool integral = false;
		static const bool is_signed = true;
		static const double max_intensity;
	};

	template <int LIFT>
	const double traits<double, LIFT>::max_intensity = 1.0;

	template <int LIFT>
	struct traits<long double, LIFT>
	{
		using element_type = long double;
		typedef long double wider_type;
		typedef long double float_type;
		static const bool integral = false;
		static const bool is_signed = true;
		static const long double max_intensity;
	};

#if defined(CVD_HAVE_TOON)
	template <int N>
	struct traits<TooN::Vector<N>>
	{
		typedef TooN::Vector<N> wider_type;
		typedef TooN::Vector<N> float_type;
		static const bool integral = false;
		static const bool is_signed = true;
		static const TooN::Vector<N> max_intensity;
	};
	template <int N>
	const TooN::Vector<N> traits<TooN::Vector<N>>::max_intensity = TooN::Vector<N>(1.0);

	template <int N, int M>
	struct traits<TooN::Matrix<N, M>>
	{
		typedef TooN::Matrix<N, M> wider_type;
		typedef TooN::Matrix<N, M> float_type;
		static const bool integral = false;
		static const bool is_signed = true;
	};

#endif

	template <int LIFT>
	const long double traits<long double, LIFT>::max_intensity = 1.0;

	template <class C>
	struct indirect_type
	{
		typedef C type;
	};

	template <class C, int N, int LIFT>
	struct traits<C[N], LIFT>
	{
		typedef typename indirect_type<typename traits<C>::wider_type[N]>::type wider_type;
		typedef typename indirect_type<typename traits<C>::float_type[N]>::type float_type;
	};
}
}

#endif

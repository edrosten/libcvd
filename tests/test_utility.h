#ifndef LIBCVD_TEST_UTILITY_H
#define LIBCVD_TEST_UTILITY_H

#include <cvd/image.h>
#include <cvd/rgba.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace CVD
{
namespace Testing
{
	template <typename T>
	void dump(const CVD::BasicImage<T>& im, std::ostream& stream = std::cout)
	{
		for(int j = 0; j < im.size().y; ++j)
		{
			for(int i = 0; i < im.size().x; ++i)
			{
				stream << im[j][i] << " ";
			}
			stream << "\n";
		}
	}

	template <typename T>
	CVD::Image<T> init(std::initializer_list<std::initializer_list<T>> values)
	{
		CVD::Image<T> image({ static_cast<int>(values.begin()->size()), static_cast<int>(values.size()) });
		int j = 0;
		for(const auto& row : values)
		{
			int i = 0;
			for(const auto& value : row)
			{
				image[j][i] = value;
				++i;
			}
			++j;
		}
		return image;
	}

	template <typename T>
	void assert_equal(T expected, T actual, std::string message = "");

	template <typename T>
	void assert_image_equal(const CVD::BasicImage<T>& expected, const CVD::BasicImage<T>& actual, std::string message = "")
	{
		assert_equal(expected.size(), actual.size(), message);
		if(expected.size() != actual.size())
		{
			exit(EXIT_FAILURE);
		}
		for(int j = 0; j < expected.size().y; ++j)
		{
			for(int i = 0; i < expected.size().x; ++i)
			{
				if(expected[j][i] != actual[j][i])
				{
					if(!message.empty())
					{
						std::cerr << message << ": ";
					}
					std::cerr << "expected:\n";
					dump(expected, std::cerr);
					std::cerr << "actual:\n";
					dump(actual, std::cerr);
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	template <typename T>
	void assert_vector_equal(const std::vector<T>& expected, const std::vector<T>& actual, std::string message = "")
	{
		assert_equal(expected.size(), actual.size(), message);
		for(size_t i = 0; i < expected.size(); ++i)
		{
			if(expected[i] != actual[i])
			{
				if(!message.empty())
				{
					std::cerr << message << ": ";
				}
				std::cerr << "item " << i << ": expected " << expected[i] << ", actual " << actual[i] << "\n";
				exit(EXIT_FAILURE);
			}
		}
	}

	template <typename T>
	void assert_equal(T expected, T actual, std::string message)
	{
		if(expected != actual)
		{
			if(!message.empty())
			{
				std::cerr << message << "; ";
			}
			std::cerr << "expected " << expected << ", actual " << actual << "\n";
			exit(EXIT_FAILURE);
		}
	}

	template <typename T>
	void assert_near(Rgba<T> expected, Rgba<T> actual, std::string message = "")
	{
		int diff = std::max({
		    std::abs(static_cast<int>(expected.red) - actual.red),
		    std::abs(static_cast<int>(expected.green) - actual.green),
		    std::abs(static_cast<int>(expected.blue) - actual.blue),
		    std::abs(static_cast<int>(expected.alpha) - actual.alpha),
		});
		if(diff > 5)
		{
			if(!message.empty())
			{
				std::cerr << message << "; ";
			}
			std::cerr << "expected " << expected << ", actual " << actual << "\n";
			exit(EXIT_FAILURE);
		}
	}
}
}

#endif

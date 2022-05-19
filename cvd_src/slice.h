#include <algorithm>
#include <future>
#include <thread>
#include <vector>

namespace CVD
{
namespace internal
{

	// Executes a function on multiple threads over a vertical image range, splitting the range into a number of slices of
	// the specified height.
	template <typename F>
	void Slice(int height, int slice_height, F function)
	{
		slice_height = std::max(1, slice_height);
		std::vector<std::future<void>> futures;
		for(int i = 0, j = 0; j < height; ++i, j += slice_height)
		{
			int end = std::min(height, j + slice_height);
			futures.push_back(std::async(function, i, j, end - j));
		}
		for(auto& future : futures)
		{
			future.get();
		}
	}

}
}

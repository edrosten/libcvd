/*
	This file is part of the CVD Library.

	Copyright (C) 2022 The Authors

	This library is free software, see LICENSE file for details
*/
#include <cvd/videoreader.h>

#include <cvd/rgba.h>

#include <algorithm>
#include <cstdlib>
#include <string>

namespace
{
using CVD::Rgba;
using CVD::VideoReader;

template <typename T>
void assert_equal(T expected, T actual, std::string message)
{
	if(expected != actual)
	{
		std::cerr << message << "; expected " << expected << ", actual " << actual << "\n";
		exit(EXIT_FAILURE);
	}
}

void assert_near(Rgba<uint8_t> expected, Rgba<uint8_t> actual, std::string message)
{
	int diff = std::max({
	    std::abs(static_cast<int>(expected.red) - actual.red),
	    std::abs(static_cast<int>(expected.green) - actual.green),
	    std::abs(static_cast<int>(expected.blue) - actual.blue),
	    std::abs(static_cast<int>(expected.alpha) - actual.alpha),
	});
	if(diff > 5)
	{
		std::cerr << message << "; expected " << expected << ", actual " << actual << "\n";
		exit(EXIT_FAILURE);
	}
}
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cerr << "usage: " << argv[0] << " videoreader.mp4\n";
		exit(EXIT_FAILURE);
	}
	VideoReader reader(argv[1], 4);
	std::cout << "timebase: " << reader.timebase().num << "/" << reader.timebase().den << "\n";
	for(int i = 0; i < 20; ++i)
	{
		auto [frame, timestamp] = reader.get_frame();

		int64_t expected_timestamp_ms = 100 * i;
		int64_t expected_timestamp = (expected_timestamp_ms * reader.timebase().den) / (1000 * reader.timebase().num);
		assert_equal(expected_timestamp, timestamp, "Incorrect timestamp for frame " + std::to_string(i));

		if(i < 10)
		{
			assert_near(Rgba<uint8_t>(0, 0, 0, 255), frame[32][32], "Incorrect top left square on frame " + std::to_string(i));
			assert_near(Rgba<uint8_t>(255, 0, 0, 255), frame[32][96], "Incorrect top right square on frame " + std::to_string(i));
			assert_near(Rgba<uint8_t>(0, 255, 0, 255), frame[96][32], "Incorrect bottom left square on frame " + std::to_string(i));
			assert_near(Rgba<uint8_t>(0, 0, 255, 255), frame[96][96], "Incorrect bottom right square on frame " + std::to_string(i));
		}
		else
		{
			assert_near(Rgba<uint8_t>(0, 0, 0, 255), frame[32][32], "Incorrect top left square on frame " + std::to_string(i));
			assert_near(Rgba<uint8_t>(0, 255, 255, 255), frame[32][96], "Incorrect top right square on frame " + std::to_string(i));
			assert_near(Rgba<uint8_t>(255, 0, 255, 255), frame[96][32], "Incorrect bottom left square on frame " + std::to_string(i));
			assert_near(Rgba<uint8_t>(255, 255, 0, 255), frame[96][96], "Incorrect bottom right square on frame " + std::to_string(i));
		}
	}

	auto [frame, timestamp] = reader.get_frame();
	if(frame.size().x != 0)
	{
		std::cerr << "Expected end of stream, received frame\n";
		exit(EXIT_FAILURE);
	}
}

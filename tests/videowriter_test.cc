/*
	This file is part of the CVD Library.

	Copyright (C) 2022 The Authors

	This library is free software, see LICENSE file for details
*/
#include <cvd/videowriter.h>

#include <cvd/image.h>
#include <cvd/rgba.h>
#include <cvd/videoreader.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace
{
using CVD::Image;
using CVD::ImageRef;
using CVD::Rgba;
using CVD::VideoReader;
using CVD::VideoWriter;
using std::uint8_t;

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
		std::cerr << "usage: " << argv[0] << " videowriter.mp4\n";
		exit(EXIT_FAILURE);
	}
	{
		VideoWriter writer(argv[1], "mpeg4", "yuv420p", { 1, 10240 }, { 10, 1 }, 0, 4);

		Image<Rgba<uint8_t>> image(ImageRef { 128, 128 });
		for(int j = 0; j < image.size().y; ++j)
		{
			for(int i = 0; i < image.size().x; ++i)
			{
				if(i >= 64)
				{
					if(j >= 64)
						image[j][i] = Rgba<uint8_t>(0, 0, 255, 255);
					else
						image[j][i] = Rgba<uint8_t>(255, 0, 0, 255);
				}
				else
				{
					if(j >= 64)
						image[j][i] = Rgba<uint8_t>(0, 255, 0, 255);
					else
						image[j][i] = Rgba<uint8_t>(0, 0, 0, 255);
				}
			}
		}
		for(int i = 0; i < 10; ++i)
		{
			writer.put_frame(image, i * 1024);
		}
		for(int j = 0; j < image.size().y; ++j)
		{
			for(int i = 0; i < image.size().x; ++i)
			{
				if(i >= 64)
				{
					if(j >= 64)
						image[j][i] = Rgba<uint8_t>(255, 255, 0, 255);
					else
						image[j][i] = Rgba<uint8_t>(0, 255, 255, 255);
				}
				else
				{
					if(j >= 64)
						image[j][i] = Rgba<uint8_t>(255, 0, 255, 255);
					else
						image[j][i] = Rgba<uint8_t>(0, 0, 0, 255);
				}
			}
		}
		for(int i = 10; i < 20; ++i)
		{
			writer.put_frame(image, i * 1024);
		}
	}

	VideoReader reader(argv[1], 4);
	assert_equal(1, reader.timebase().num, "Incorrect timebase numerator");
	assert_equal(10240, reader.timebase().den, "Incorrect timebase denominator");
	for(int i = 0; i < 20; ++i)
	{
		auto [frame, timestamp] = reader.get_frame();

		int64_t expected_timestamp = i * 1024;
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

#ifndef LIBCVD_VIDEOREADERFILEBUFFER_H
#define LIBCVD_VIDEOREADERFILEBUFFER_H

#include "diskbuffer2.h"
#include "localvideobuffer.h"
#include "rgba.h"
#include "videobufferflags.h"
#include "videoreader.h"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace CVD
{

// Implements the LocalVideoBuffer interface using a VideoReader.
class VideoReaderFileBuffer : public LocalVideoBuffer<Rgba<std::uint8_t>>
{
	public:
	using pixel_t = Rgba<std::uint8_t>;

	private:
	VideoReader m_reader;
	std::string m_filename;
	std::pair<Image<pixel_t>, std::int64_t> m_next_frame;

	public:
	VideoReaderFileBuffer(const std::string& filename, int threads = 1);
	~VideoReaderFileBuffer();

	ImageRef size() override;
	bool frame_pending() override;
	void seek_to(double t) override;
	LocalVideoFrame<pixel_t>* get_frame() override;
	void put_frame(VideoFrame<pixel_t>* f) override;

	double frame_rate() const;
	std::string file_name() const;
};

}

#endif

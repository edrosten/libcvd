#include "cvd/videoreaderfilebuffer.h"

#include "cvd/videoframe.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <utility>

namespace CVD
{
namespace
{
	using std::int64_t;
	using std::uint8_t;
}

VideoReaderFileBuffer::VideoReaderFileBuffer(const std::string& filename, int threads)
    : LocalVideoBuffer(VideoBufferType::NotLive)
    , m_reader(filename, threads)
    , m_filename(filename)
{
	m_next_frame = m_reader.get_frame();
}

VideoReaderFileBuffer::~VideoReaderFileBuffer() = default;

ImageRef VideoReaderFileBuffer::size()
{
	return m_reader.size();
}

bool VideoReaderFileBuffer::frame_pending()
{
	return m_next_frame.first.size().x != 0;
}

void VideoReaderFileBuffer::seek_to(double /*t*/)
{
	throw std::runtime_error("Seek not supported");
}

LocalVideoFrame<VideoReaderFileBuffer::pixel_t>* VideoReaderFileBuffer::get_frame()
{
	if(!frame_pending())
		throw Exceptions::DiskBuffer2::EndOfBuffer();
	auto frame = std::make_unique<LocalVideoFrame<pixel_t>>(
	    m_reader.timebase_to_seconds(m_next_frame.second),
	    std::move(m_next_frame.first));
	m_next_frame = m_reader.get_frame();
	return frame.release();
}

void VideoReaderFileBuffer::put_frame(VideoFrame<VideoReaderFileBuffer::pixel_t>* f)
{
	delete static_cast<LocalVideoFrame<pixel_t>*>(f);
}

double VideoReaderFileBuffer::frame_rate() const
{
	auto frame_rate_ratio = m_reader.frame_rate();
	return static_cast<double>(frame_rate_ratio.num) / frame_rate_ratio.den;
}

std::string VideoReaderFileBuffer::file_name() const
{
	return m_filename;
}

}

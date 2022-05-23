#ifndef LIBCVD_VIDEOWRITER_H
#define LIBCVD_VIDEOWRITER_H

#include "image.h"
#include "rgba.h"
#include "videoffmpeg.h"

#include <libavutil/rational.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace CVD
{
namespace internal
{
	class Scaler;
}

// Simple video writer to write RGBA frames to a video file.
class VideoWriter
{
	public:
	VideoWriter(const std::string& filename,
	    const std::string& codec_name,
	    const std::string& pixel_format_name,
	    AVRational timebase,
	    AVRational frame_rate,
	    int rotation_degrees,
	    int threads,
	    int bitrate = -1,
	    std::int64_t duration = -1);
	~VideoWriter();

	std::int64_t seconds_to_timebase(double seconds) const
	{
		return static_cast<std::int64_t>(std::round((seconds * m_timebase.den) / m_timebase.num));
	}
	void put_frame(const BasicImage<Rgba<std::uint8_t>>& image, std::int64_t pts);

	private:
	std::string m_filename;
	AVRational m_timebase;
	int m_threads;
	const AVCodec* m_codec;
	int m_bitrate;
	std::unique_ptr<AVFormatContext, internal::AVFormatContextCloser> m_format_context;
	std::unique_ptr<AVCodecContext, internal::AVCodecContextCloser> m_codec_context;
	std::unique_ptr<internal::Scaler> m_scaler;
};

}

#endif

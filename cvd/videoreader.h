#ifndef LIBCVD_VIDEOREADER_H
#define LIBCVD_VIDEOREADER_H

#include "image.h"
#include "image_ref.h"
#include "rgba.h"
#include "videoffmpeg.h"

#include <libavutil/rational.h>

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

// Reads video files using FFmpeg as RGBA frames.
class VideoReader
{
	public:
	VideoReader(const std::string& filename, int threads = 1);
	~VideoReader();

	// Convert an FFmpeg timestamp from the video stream to seconds since the start of the video.
	double timebase_to_seconds(std::int64_t pts) const;
	// Get the timebase of the video.
	AVRational timebase() const;
	// Get the size of each video frame, in pixels.
	ImageRef size() const;
	// Get the frame rate of the video.
	AVRational frame_rate() const;
	// Get the duration of the video, in the video timebase.
	std::int64_t duration() const;
	// Read the next frame from the video as an RGBA frame and timestamp in the video timebase.
	// Returns an empty image if there are no more frames in the video.
	std::pair<Image<Rgba<std::uint8_t>>, std::int64_t> get_frame();

	private:
	std::unique_ptr<AVFormatContext, internal::AVFormatContextCloser> m_format_context;
	std::unique_ptr<AVCodecContext, internal::AVCodecContextCloser> m_codec_context;
	std::unique_ptr<internal::Scaler> m_scaler;
	std::unique_ptr<AVFrame, internal::AVFrameCloser> m_raw_frame;
	int m_stream_index = -1;
};

}

#endif

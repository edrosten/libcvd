#ifndef LIBCVD_VIDEOREADER_H
#define LIBCVD_VIDEOREADER_H

#include "image.h"
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

	// Convert an FFmpeg timestamp from the video stream to seconds since the start of the video.
	double TimebaseToSeconds(std::int64_t pts) const;
	// Get the timebase of the video.
	AVRational GetTimebase() const;
	// Get the width of each video frame, in pixels.
	int GetWidth() const;
	// Get the height of each video frame, in pixels.
	int GetHeight() const;
	// Get the frame rate of the video.
	AVRational GetFrameRate() const;
	// Get the duration of the video, in the video timebase.
	std::int64_t GetDuration() const;
	// Read the next frame from the video as an RGBA frame and timestamp.
	// Returns an empty image if there are no more frames in the video.
	std::pair<Image<Rgba<std::uint8_t>>, std::int64_t> ReadNextFrame();

	private:
	std::unique_ptr<AVFormatContext, internal::AVFormatContextCloser> m_format_context;
	std::unique_ptr<AVCodecContext, internal::AVCodecContextCloser> m_codec_context;
	std::unique_ptr<internal::Scaler> m_scaler;
	std::unique_ptr<AVFrame, internal::AVFrameCloser> m_raw_frame;
	int m_stream_index = -1;
};

}

#endif

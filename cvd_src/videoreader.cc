#include "cvd/videoreader.h"
#include "cvd/videoscaler.h"

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

VideoReader::VideoReader(const std::string& filename, int threads)
{
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 9, 100)
	// see https://github.com/FFmpeg/FFmpeg/commit/0694d8702421e7aff1340038559c438b61bb30dd
	av_register_all();
#endif
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 10, 100)
	// see https://github.com/FFmpeg/FFmpeg/commit/3f0a41367eb9180ab6d22d43ad42b9bd85a26df0
	avcodec_register_all();
#endif

	AVDictionary* opts = nullptr;
	AVFormatContext* formatContext = nullptr;
	int result = avformat_open_input(&formatContext, filename.c_str(), nullptr, &opts);
	internal::CheckAVError(result);
	m_format_context.reset(formatContext);

	result = avformat_find_stream_info(m_format_context.get(), nullptr);
	internal::CheckAVError(result);
	AVCodecParameters* parameters = nullptr;
	for(unsigned int i = 0; i != m_format_context->nb_streams; ++i)
	{
		if(m_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			parameters = m_format_context->streams[i]->codecpar;
			m_stream_index = i;
			break;
		}
	}
	if(!parameters)
	{
		throw std::runtime_error("No video stream found in video file");
	}
	auto codec = avcodec_find_decoder(parameters->codec_id);
	if(!codec)
	{
		throw std::runtime_error("No video decoder found for video codec");
	}

	m_codec_context.reset(avcodec_alloc_context3(codec));
	avcodec_parameters_to_context(m_codec_context.get(), parameters);
	m_codec_context->thread_count = threads;
	result = avcodec_open2(m_codec_context.get(), codec, 0);
	internal::CheckAVError(result);

	int colorspace = -1;
	switch(m_codec_context->color_primaries)
	{
		case AVCOL_PRI_BT709:
		{
			colorspace = SWS_CS_ITU709;
			break;
		}
		case AVCOL_PRI_BT470M:
		case AVCOL_PRI_BT470BG:
		case AVCOL_PRI_SMPTE170M:
		case AVCOL_PRI_SMPTE240M:
		case AVCOL_PRI_UNSPECIFIED:
			// Rec. 601 color conversion is the FFmpeg default.
			break;
		default:
			throw std::runtime_error("Unknown color primaries for video");
	}

	m_scaler = std::make_unique<internal::Scaler>(
	    m_codec_context->width, m_codec_context->height, m_codec_context->pix_fmt, AV_PIX_FMT_RGBA, colorspace, threads);

	m_raw_frame.reset(av_frame_alloc());
}

VideoReader::~VideoReader() = default;

AVRational VideoReader::timebase() const
{
	return m_format_context->streams[m_stream_index]->time_base;
}

double VideoReader::timebase_to_seconds(int64_t pts) const
{
	auto video_timebase = timebase();
	return static_cast<double>(pts * video_timebase.num) / video_timebase.den;
}

ImageRef VideoReader::size() const
{
	return { m_codec_context->width, m_codec_context->height };
}

AVRational VideoReader::frame_rate() const
{
	return m_format_context->streams[m_stream_index]->r_frame_rate;
}

int64_t VideoReader::duration() const
{
	return m_format_context->streams[m_stream_index]->duration;
}

std::pair<Image<Rgba<uint8_t>>, int64_t> VideoReader::get_frame()
{
	for(;;)
	{
		int r = avcodec_receive_frame(m_codec_context.get(), m_raw_frame.get());
		if(r >= 0)
		{
			break;
		}
		if(r == AVERROR_EOF)
		{
			return {};
		}
		if(r != AVERROR(EAGAIN))
		{
			internal::CheckAVError(r);
		}

		std::unique_ptr<AVPacket, internal::AVPacketDeleter> packet(av_packet_alloc());
		r = av_read_frame(m_format_context.get(), packet.get());
		// EOF packets need to be sent to flush the last frame
		if(r != AVERROR_EOF)
		{
			internal::CheckAVError(r);

			if(packet->stream_index != m_stream_index)
			{
				continue;
			}
			r = avcodec_send_packet(m_codec_context.get(), packet.get());
			if(r != AVERROR(EAGAIN) && r != AVERROR(EOF))
			{
				internal::CheckAVError(r);
			}
		}
		else
		{
			avcodec_send_packet(m_codec_context.get(), nullptr);
		}
	}

	Image<Rgba<uint8_t>> result(CVD::ImageRef(m_codec_context->width, m_codec_context->height));

	uint8_t* data[4];
	int linesize[4];
	av_image_fill_arrays(data,
	    linesize,
	    reinterpret_cast<uint8_t*>(result.data()),
	    AV_PIX_FMT_RGBA,
	    result.size().x,
	    result.size().y,
	    1);
	m_scaler->scale(m_raw_frame->data, m_raw_frame->linesize, 0, m_codec_context->height, data, linesize);
	return { std::move(result), m_raw_frame->pts };
}

}

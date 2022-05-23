#include "cvd/videowriter.h"
#include "cvd/videoscaler.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace CVD
{
namespace
{
	using std::uint8_t;

	// Calculate the greatest common divisor of two values.
	int gcd(int a, int b)
	{
		if(a < 0)
			a = -a;
		if(b < 0)
			b = -b;
		for(;;)
		{
			if(a == 0)
				return b;
			if(b == 0)
				return a;
			const int t = b;
			b = a % b;
			a = t;
		}
	}

	// Multiply one rational number by another.
	AVRational multiply_rational(const AVRational& left, const AVRational& right)
	{
		AVRational r { left.num * right.num, left.den * right.den };
		if(r.num == 0)
		{
			r.den = 1;
		}
		else
		{
			int d = gcd(r.num, r.den);
			if(d > 1)
			{
				r.num /= d;
				r.den /= d;
			}
		}
		return r;
	}

	// Divide one rational number by another.
	AVRational divide_rational(const AVRational& left, const AVRational& right)
	{
		return multiply_rational(left, AVRational { right.den, right.num });
	}

}

VideoWriter::VideoWriter(const std::string& filename,
    const std::string& codec_name,
    const std::string& pixel_format_name,
    AVRational timebase,
    AVRational frame_rate,
    int rotation_degrees,
    int threads,
    int bitrate,
    std::int64_t duration)
    : m_filename(filename)
    , m_timebase(timebase)
    , m_threads(threads)
    , m_bitrate(bitrate)
{
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 9, 100)
	// see https://github.com/FFmpeg/FFmpeg/commit/0694d8702421e7aff1340038559c438b61bb30dd
	av_register_all();
#endif
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 10, 100)
	// see https://github.com/FFmpeg/FFmpeg/commit/3f0a41367eb9180ab6d22d43ad42b9bd85a26df0
	avcodec_register_all();
#endif

	m_codec = avcodec_find_encoder_by_name(codec_name.c_str());
	if(!m_codec)
	{
		throw std::runtime_error("Unable to find codec");
	}

	AVFormatContext* format_ptr;
	internal::CheckAVError(avformat_alloc_output_context2(&format_ptr, nullptr, nullptr, m_filename.c_str()));
	m_format_context.reset(format_ptr);

	auto stream = avformat_new_stream(m_format_context.get(), nullptr);
	if(!stream)
	{
		throw std::runtime_error("avformat_new_stream");
	}
	stream->time_base = timebase;
	if(codec_name == "mpeg4")
	{
		// MPEG-4 standard codec can only handle timebases with 16-bit numerator and denominator.
		while(stream->time_base.den > 65535)
		{
			stream->time_base.den /= 2;
		}
		while(stream->time_base.num > 65535)
		{
			stream->time_base.num /= 2;
		}
	}

	stream->avg_frame_rate = frame_rate;
	stream->r_frame_rate = frame_rate;
	if(duration >= 0)
	{
		stream->duration = duration;
	}
	internal::CheckAVError(av_dict_set(&stream->metadata, "rotate", std::to_string(rotation_degrees).c_str(), 0));

	m_codec_context.reset(avcodec_alloc_context3(m_codec));
	if(!m_codec_context)
	{
		throw std::runtime_error("avcodec_alloc_context3");
	}
	m_codec_context->thread_count = threads;
	m_codec_context->codec_id = m_codec->id;
	m_codec_context->time_base = stream->time_base;
	m_codec_context->framerate = frame_rate;

	AVPixelFormat pixel_format = av_get_pix_fmt(pixel_format_name.c_str());
	if(pixel_format != AV_PIX_FMT_NONE)
	{
		m_codec_context->pix_fmt = pixel_format;
	}
	else
	{
		m_codec_context->pix_fmt = m_codec->pix_fmts ? m_codec->pix_fmts[0] : AV_PIX_FMT_YUV420P;
	}
	if(m_format_context->oformat->flags & AVFMT_GLOBALHEADER)
	{
		m_codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
	if(m_bitrate > 0)
	{
		m_codec_context->bit_rate = m_bitrate;
	}
	m_codec_context->colorspace = AVCOL_SPC_BT709;
	m_codec_context->color_primaries = AVCOL_PRI_BT709;
	m_codec_context->color_trc = AVCOL_TRC_BT709;
	m_codec_context->sample_aspect_ratio = { 1, 1 };
}

void VideoWriter::put_frame(const BasicImage<Rgba<uint8_t>>& image, std::int64_t pts)
{
	if(!m_scaler)
	{
		m_codec_context->width = image.size().x;
		m_codec_context->height = image.size().y;

		auto stream = m_format_context->streams[0];

		AVDictionary* dict = nullptr;
		internal::CheckAVError(avcodec_open2(m_codec_context.get(), m_codec, &dict));
		internal::CheckAVError(avcodec_parameters_from_context(stream->codecpar, m_codec_context.get()));
		internal::CheckAVError(avio_open2(&m_format_context->pb, m_filename.c_str(), AVIO_FLAG_WRITE, nullptr, &dict));
		internal::CheckAVError(avformat_write_header(m_format_context.get(), &dict));
		av_dict_free(&dict);

		m_scaler = std::make_unique<internal::Scaler>(
		    m_codec_context->width,
		    m_codec_context->height,
		    AV_PIX_FMT_RGBA,
		    m_codec_context->pix_fmt,
		    SWS_CS_ITU709,
		    m_threads);
	}

	const uint8_t* data[4] = { reinterpret_cast<const uint8_t*>(image.data()), nullptr, nullptr, nullptr };
	int stride[4] = { static_cast<int>(image.row_stride() * sizeof(Rgba<uint8_t>)), 0, 0, 0 };

	std::unique_ptr<AVFrame, internal::AVFrameDeleter> output_frame(av_frame_alloc());
	output_frame->width = m_codec_context->width;
	output_frame->height = m_codec_context->height;
	output_frame->format = m_codec_context->pix_fmt;
	output_frame->colorspace = m_codec_context->colorspace;
	output_frame->color_primaries = m_codec_context->color_primaries;
	output_frame->color_trc = m_codec_context->color_trc;
	output_frame->sample_aspect_ratio = m_codec_context->sample_aspect_ratio;
	auto timebase_ratio = divide_rational(m_timebase, m_format_context->streams[0]->time_base);
	output_frame->pts = (pts * timebase_ratio.num + timebase_ratio.den / 2) / timebase_ratio.den;
	internal::CheckAVError(av_frame_get_buffer(output_frame.get(), 32));

	m_scaler->scale(data, stride, 0, image.size().y, output_frame->data, output_frame->linesize);

	internal::SendFrame(m_format_context.get(), m_codec_context.get(), output_frame.get());
}

VideoWriter::~VideoWriter()
{
	if(m_scaler)
	{
		internal::SendFrame(m_format_context.get(), m_codec_context.get(), nullptr);
		internal::CheckAVError(av_write_trailer(m_format_context.get()));
	}
}

}

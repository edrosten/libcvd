#include "cvd/videoffmpeg.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <memory>
#include <stdexcept>
#include <string>

namespace CVD
{
namespace internal
{
	namespace
	{
		using namespace std::string_literals;
	}

	void AVCodecContextCloser::operator()(AVCodecContext* context) const
	{
		if(context)
		{
			av_freep(&context->extradata);
			avcodec_close(context);
			av_free(context);
		}
	}

	void AVFormatContextCloser::operator()(AVFormatContext* context) const
	{
		if(context)
		{
			avformat_close_input(&context);
			avformat_free_context(context);
		}
	}

	void AVFrameDeleter::operator()(AVFrame* p) const
	{
		if(p)
		{
			av_frame_free(&p);
		}
	}

	void AVPacketDeleter::operator()(AVPacket* p) const
	{
		if(p)
		{
			av_packet_free(&p);
		}
	}

	void SwsContextCloser::operator()(SwsContext* context) const
	{
		if(context)
		{
			sws_freeContext(context);
		}
	}

	void CheckAVError(int result)
	{
		if(result != 0)
		{
			char error[1024];
			av_strerror(result, error, sizeof(error));
			throw std::runtime_error("Video encoding/decoding error: "s + error);
		}
	}

	void SendFrame(AVFormatContext* format, AVCodecContext* codec, const AVFrame* frame)
	{
		bool sent = false;
		while(!sent)
		{
			int r = avcodec_send_frame(codec, frame);
			if(r != AVERROR(EAGAIN))
			{
				CheckAVError(r);
				sent = true;
			}

			for(;;)
			{
				std::unique_ptr<AVPacket, AVPacketDeleter> packet(av_packet_alloc());
				int r = avcodec_receive_packet(codec, packet.get());
				if(r == AVERROR(EAGAIN) || r == AVERROR_EOF)
				{
					break;
				}
				CheckAVError(r);
				packet->duration = format->streams[packet->stream_index]->r_frame_rate.den;
				CheckAVError(av_interleaved_write_frame(format, packet.get()));
			}
		}
	}

	void SetColorSpace(SwsContext* scalerContext, int colorspace)
	{
		// Newer VideoRecorder videos are encoded with Rec. 709 color conversion, so set the appropriate scaler
		// color space.
		int* invTable;
		int* table;
		int inFull, outFull, brightness, contrast, saturation;
		sws_getColorspaceDetails(scalerContext, &invTable, &inFull, &table, &outFull, &brightness, &contrast, &saturation);
		auto coefficients = sws_getCoefficients(colorspace);
		sws_setColorspaceDetails(
		    scalerContext, coefficients, inFull, coefficients, outFull, brightness, contrast, saturation);
	}

}
}

#ifndef LIBCVD_VIDEOFFMPEG_H
#define LIBCVD_VIDEOFFMPEG_H

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;

namespace CVD
{
namespace internal
{
	struct AVCodecContextCloser
	{
		void operator()(AVCodecContext* context) const;
	};

	struct AVFormatContextCloser
	{
		void operator()(AVFormatContext* context) const;
	};

	struct AVFrameCloser
	{
		void operator()(AVFrame* p) const;
	};

	struct AVFrameDeleter
	{
		void operator()(AVFrame* p) const;
	};

	struct AVPacketDeleter
	{
		void operator()(AVPacket* p) const;
	};

	struct SwsContextCloser
	{
		void operator()(SwsContext* context) const;
	};

	void CheckAVError(int result);
	void SendFrame(AVFormatContext* format, AVCodecContext* codec, const AVFrame* frame);
	void SetColorSpace(SwsContext* scalerContext, int colorspace);

}
}

#endif

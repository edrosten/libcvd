#include "cvd/videoscaler.h"

#include "slice.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}

#include <algorithm>
#include <array>
#include <stdexcept>

namespace CVD
{
namespace internal
{
	namespace
	{
		template <typename T>
		std::array<T*, 4> AdjustDataPointers(const AVPixFmtDescriptor& descriptor, T* const* data, const int* linesize, int y)
		{
			std::array<T*, 4> adjusted_data;
			adjusted_data[0] = data[0] + linesize[0] * y;
			adjusted_data[1] = data[1] + (linesize[1] >> descriptor.log2_chroma_h) * y;
			adjusted_data[2] = data[2] + (linesize[2] >> descriptor.log2_chroma_h) * y;
			adjusted_data[3] = data[3] + linesize[3] * y;
			return adjusted_data;
		}
	}

	Scaler::Scaler(int width, int height, int source_format, int target_format, int colorspace, int threads)
	    : m_source_format(source_format)
	    , m_target_format(target_format)
	{
		for(int j = 0; j < threads; ++j)
		{
			m_contexts.emplace_back(sws_getContext(width,
			    height,
			    static_cast<AVPixelFormat>(source_format),
			    width,
			    height,
			    static_cast<AVPixelFormat>(target_format),
			    SWS_POINT,
			    nullptr,
			    nullptr,
			    nullptr));
			if(!m_contexts.back())
			{
				throw std::runtime_error("Failed to create scaler context");
			}
			if(colorspace != -1)
			{
				SetColorSpace(m_contexts.back().get(), colorspace);
			}
		}
	}

	void Scaler::Scale(const std::uint8_t* const* input_data,
	    const int* input_linesize,
	    const int y,
	    const int height,
	    std::uint8_t* const* output_data,
	    const int* output_linesize)
	{
		const AVPixFmtDescriptor* source_descriptor = av_pix_fmt_desc_get(static_cast<AVPixelFormat>(m_source_format));
		const AVPixFmtDescriptor* target_descriptor = av_pix_fmt_desc_get(static_cast<AVPixelFormat>(m_target_format));

		int slice_height = std::max(1, static_cast<int>((height + m_contexts.size() - 1) / m_contexts.size()));
		while(slice_height & ((1 << std::max(source_descriptor->log2_chroma_h, target_descriptor->log2_chroma_h)) - 1))
		{
			++slice_height;
		}

		Slice(height, slice_height, [&](int i, int j, int height) {
			auto inputDataJ = AdjustDataPointers(*source_descriptor, input_data, input_linesize, j);
			auto outputDataJ = AdjustDataPointers(*target_descriptor, output_data, output_linesize, j);
			sws_scale(m_contexts[i].get(), inputDataJ.data(), input_linesize, 0, height, outputDataJ.data(), output_linesize);
		});
	}

}
}

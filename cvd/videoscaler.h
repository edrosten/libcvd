#ifndef LIBCVD_VIDEOSCALER_H
#define LIBCVD_VIDEOSCALER_H

#include "videoffmpeg.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace CVD
{
namespace internal
{

	// Abstraction of an FFMPEG video scaler.
	class Scaler
	{
		using Context = std::unique_ptr<SwsContext, SwsContextCloser>;

		public:
		Scaler(int width, int height, int source_format, int target_format, int colorspace, int threads);

		void Scale(const std::uint8_t* const* input_data,
		    const int* input_linesize,
		    int y,
		    int height,
		    std::uint8_t* const* output_data,
		    const int* output_linesize);

		private:
		int m_source_format, m_target_format;
		std::vector<Context> m_contexts;
	};
}
}

#endif

#include <cvd/videosource.h>
#include <cvd/videofilebuffer.h>

namespace CVD{

	template <> VideoBuffer<byte>* makeVideoFileBuffer(const std::string& file, VideoBufferFlags::OnEndOfBuffer eob, bool verbose)
	{
		VideoFileBuffer<byte>* vb = new VideoFileBuffer<byte>(file, verbose);
		vb->on_end_of_buffer(eob);
		return vb;
	}

	template <> VideoBuffer<Rgb<byte> >* makeVideoFileBuffer(const std::string& file, VideoBufferFlags::OnEndOfBuffer eob, bool verbose)
	{
		VideoFileBuffer<Rgb<byte> >* vb = new VideoFileBuffer<Rgb<byte> >(file, verbose);
		vb->on_end_of_buffer(eob);
		return vb;
	}
}


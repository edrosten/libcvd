#include <cvd/videosource.h>
#include <cvd/videofilebuffer.h>

namespace CVD{

	template <> VideoBuffer<byte>* makeVideoFileBuffer(const std::string& file, VideoBufferFlags::OnEndOfBuffer eob, bool verbose, const std::string& fn)
	{
		VideoFileBuffer<byte>* vb = new VideoFileBuffer<byte>(file, fn, verbose);
		vb->on_end_of_buffer(eob);
		return vb;
	}

	template <> VideoBuffer<Rgb<byte> >* makeVideoFileBuffer(const std::string& file, VideoBufferFlags::OnEndOfBuffer eob, bool verbose, const std::string& fn)
	{
		VideoFileBuffer<Rgb<byte> >* vb = new VideoFileBuffer<Rgb<byte> >(file, fn, verbose);
		vb->on_end_of_buffer(eob);
		return vb;
	}
}


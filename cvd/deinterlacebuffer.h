// PAS 17/6/04 (revised 16/2/05)

#ifndef __CVD_DEINTERLACEBUFFER_H
#define __CVD_DEINTERLACEBUFFER_H

#include <cvd/videobuffer.h>
#include <cvd/deinterlaceframe.h>

namespace CVD
{

////////////////
// DEINTERLACE BUFFER EXCEPTIONS
//
namespace Exceptions
{
	namespace DeinterlaceBuffer
	{
		struct All: public CVD::Exceptions::VideoBuffer::All { };
		struct OddNumberOfLines: public All { OddNumberOfLines(); };
	}
}

/////////////////
// DEINTERLACE BUFFER
//
template <typename T>
class DeinterlaceBuffer : public VideoBuffer<T>
{
	public:
		enum Fields{OddOnly, EvenOnly, OddEven, EvenOdd};
		
   public:
      DeinterlaceBuffer(CVD::VideoBuffer<T>& buf, Fields fields = OddEven);
 
		ImageRef size();
      CVD::VideoFrame<T>* get_frame();
      void put_frame(CVD::VideoFrame<T>* f);
      virtual bool frame_pending()
			{return m_vidbuf.frame_pending();}
      virtual void seek_to(double t)
			{return m_vidbuf.seek_to(t);}
	
      
   private:
		CVD::VideoFrame<T>* my_realframe;
		CVD::VideoBuffer<T>& m_vidbuf;
		Fields m_fields;
		bool m_loadnewframe;
		ImageRef m_size;
		unsigned int m_linebytes;
};

//
// CONSTRUCTOR
//
template <typename T>
DeinterlaceBuffer<T>::DeinterlaceBuffer(CVD::VideoBuffer<T>& vidbuf, Fields fields) :
	m_vidbuf(vidbuf),
	m_fields(fields),
	m_loadnewframe(true)
{
	// Check it has an even number of lines
	if(m_vidbuf.size().y % 2 != 0)
		throw Exceptions::DeinterlaceBuffer::OddNumberOfLines();
	
	m_size = ImageRef(m_vidbuf.size().x, m_vidbuf.size().y / 2);
	m_linebytes = sizeof(T) * m_size.x;
}

//
// GET FRAME
//
template <typename T>
VideoFrame<T>* DeinterlaceBuffer<T>::get_frame()
{
	if(m_loadnewframe)
	{
		// Get a new frame from the real videobuffer
		my_realframe = m_vidbuf.get_frame();
	}
		
	// Now return the deinterlaced image
	// First sort out the time
	double time = my_realframe->timestamp();
	
	// If we're giving the second frame of a pair, make it a fraction of time after the first
	// What happens if the frame rate is > 10kHz? This should perhaps add on half the frame rate
	// (if we knew what the frame rate was)
	if(!m_loadnewframe)
		time += 0.0001; 
	
	T* data = new T[m_size.x * m_size.y];
	DeinterlaceFrame<T>* frame = new DeinterlaceFrame<T>(time, data, m_size);
	if(m_fields == OddOnly || 
		(m_fields == OddEven && m_loadnewframe) ||
		(m_fields == EvenOdd && !m_loadnewframe))
	{
		// We want the odd field
		CVD::byte* podd = reinterpret_cast<CVD::byte*>(frame->data());
		const CVD::byte* pframe = reinterpret_cast<const CVD::byte*>(my_realframe->data());
		for(int i = 0; i < m_size.y; i++)
		{
			memcpy(podd, pframe, m_linebytes);
			pframe += 2 * m_linebytes;
			podd += m_linebytes;
		}
	}
	else
	{
		// We want the even field
		CVD::byte* peven = reinterpret_cast<CVD::byte*>(frame->data());
		const CVD::byte* pframe = reinterpret_cast<const CVD::byte*>(my_realframe->data()) + m_linebytes;
		for(int i = 0; i < m_size.y; i++)
		{
			memcpy(peven, pframe, m_linebytes);
			pframe += 2 * m_linebytes;
			peven += m_linebytes;
		}
	}
	frame->real_frame = my_realframe;
	
	if(m_fields == OddEven || m_fields == EvenOdd)
	{
		// If we're taking both fields, we only load a frame every other field
		m_loadnewframe = !m_loadnewframe;
	}

  return frame;
}

//
// SIZE
//
template <typename T>
ImageRef DeinterlaceBuffer<T>::size()
{
	ImageRef size = m_vidbuf.size();
	return ImageRef(size.x, size.y / 2);
}

//
// PUT FRAME
//
template <typename T>
void DeinterlaceBuffer<T>::put_frame(CVD::VideoFrame<T>* frame)
{
	if(m_loadnewframe)
	{
		// Next time we'll be getting a new real frame, so put back the current real frame
		m_vidbuf.put_frame(my_realframe);
	}
	
	// And delete the data for my current deinterlaced frame
	delete[] frame->data();
	delete dynamic_cast<DeinterlaceFrame<T>*>(frame);
}

} // CVD
#endif

/*                       
	This file is part of the CVD Library.

	Copyright (C) 2005 The Authors

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
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
	/// %Exceptions specific to DeinterlaceBuffer.
	/// @ingroup gException
	namespace DeinterlaceBuffer
	{	
		/// Base class for all DeinterlaceBuffer exceptions
		/// @ingroup gException
		struct All: public CVD::Exceptions::VideoBuffer::All { }; 
		
		/// The VideoBuffer that is being wrapped does not have an even number of lines (so the odd and even- fields would not be the same size)
		/// @ingroup gException
		struct OddNumberOfLines: public All { OddNumberOfLines(); }; 
	}
}


/////////////////
// DEINTERLACE BUFFER
//

/// A decorator class which wraps a VideoBuffer to return fields instead of 
/// the original frames (see also DeinterlaceFrame). The majority of commands are passed
/// straight through to the buffer that this class wraps, but get_frame() is 
/// overloaded to extract fields from the video frames.
///
/// Video intended for television use (i.e. from non-progressive scan cameras) tends
/// to be interlaced. Instead of grabbing the entire frame at one time instant, the 
/// image is grabbed in two parts (fields), made up of the odd-numbered lines and the 
/// even-numbered lines respectively (thus giving an effective <em>field-rate</em> of 
/// twice the video frame-rate. Any fast motion in frame will therefore exhibit serrated
/// distortion, with alternate lines out of step. 
///
/// This class returns individual fields from the video, which are guaranteed to 
/// represent a single time instant. The VideoFrames returned from this buffer are
/// therefore half the height of the original image, and so you might want to 
/// double the y-scale before displaying. 
///
/// Provides frames of type CVD::DeinterlaceFrame and throws exceptions of type 
/// CVD::Exceptions::DeinterlaceBuffer
/// @param T The pixel type of the original VideoBuffer
/// @ingroup gVideoBuffer
template <typename T>
class DeinterlaceBuffer : public VideoBuffer<T>
{
	public:
		/// Used to select which fields, and in which order, to extract from the frame
		enum Fields{
			OddOnly, ///< Odd fields only
			EvenOnly, ///< Even fields only
			OddEven, ///< Both fields, presenting the odd lines from each frame first 
			EvenOdd ///< Both fields, presenting the even lines from each frame first
		}; 
		
	public:
		/// Construct a DeinterlaceBuffer by wrapping it around another VideoBuffer
		/// @param buf The buffer that will provide the raw frames
		/// @param fields The fields to 
   		DeinterlaceBuffer(CVD::VideoBuffer<T>& buf, Fields fields = OddEven);
 
		/// The size of the VideoFrames returns by this buffer. This will be half the 
		/// height of the original frames.
		ImageRef size();
		
		CVD::VideoFrame<T>* get_frame();
		
		void put_frame(CVD::VideoFrame<T>* f);
		
		virtual bool frame_pending()
			{return m_vidbuf.frame_pending();}
			
		virtual void seek_to(double t)
			{return m_vidbuf.seek_to(t);}
			
		/// What is the (expected) frame rate of this video buffer, in frames per second?
		/// If OddEven or EvenOdd are selected, this will be reported as twice the original 
		/// buffer's rate.
		virtual double frame_rate()
	  	{
	  		if(m_fields == OddOnly || m_fields == EvenOnly)
	  			return m_vidbuf.frame_rate();
			else
		  		return m_vidbuf.frame_rate() * 2.0;
		}
      
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
	VideoBuffer<T>(vidbuf.type()),
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
	
	// If we're giving the second frame of a pair, make its time half-way to the next frame
	if(!m_loadnewframe)
		time += frame_rate(); 
	
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

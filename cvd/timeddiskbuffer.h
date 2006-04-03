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
#ifndef TIMEDDISKBUFFER_H
#define TIMEDDISKBUFFER_H

#include <cvd/diskbuffer2.h>

namespace CVD {

	namespace Exceptions
	{
		/// %Exceptions specific to DiskBuffer2
		/// @ingroup gException
		namespace TimedDiskBuffer
		{
			/// Base class for all DiskBuffer2 exceptions
			/// @ingroup gException
			struct All: public CVD::Exceptions::VideoBuffer::All { };
			/// list lengths for name and time lists do not agree
			/// @ingroup gException
			struct IncompatibleListLengths: public All { IncompatibleListLengths(); };
		}
	}

	/// Play a series of image files as a video stream and use a list of provided timestamps. 
	/// Provides frames of type CVD::DiskBuffer2Frame and throws exceptions of type 
	/// CVD::Exceptions::DiskBuffer2 and CVD::Exceptions::TimedDiskBuffer
	/// @param T The pixel type of the frames to provide (usually <code>CVD::Rgb<CVD::byte></code> 
	/// or <code>CVD::byte</code>. If the image files are of a different type, they will be automatically 
	/// converted (see @link gImageIO Image loading and saving, and format conversion@endlink).
	/// @ingroup gVideoBuffer
	template<class T>
	class TimedDiskBuffer: public CVD::DiskBuffer2<T>
	{
	public:
		/// Construct a TimedDiskBuffer2 from a vector of filenames and timestamps.
		/// see @ref Diskbuffer2 for details on how to use it.
		/// @param names The filenames to use (played in the order that they are in the vector)
		/// @param times The frame time stamps
		/// @param eob What should the buffer do when it reaches the end of the list of files?
		TimedDiskBuffer(const std::vector<std::string>& names, const std::vector<double> & times , CVD::VideoBufferFlags::OnEndOfBuffer eob = CVD::VideoBufferFlags::RepeatLastFrame);
	
		virtual CVD::DiskBuffer2Frame<T>* get_frame();
	protected:
		std::vector<double> file_times;
	};
	
	template<class T>
	inline TimedDiskBuffer<T>::TimedDiskBuffer(const std::vector<std::string>& names, const std::vector<double> & times, CVD::VideoBufferFlags::OnEndOfBuffer eob )
	: CVD::DiskBuffer2<T>(names, 1, eob )
	{
		if(times.size() != names.size())
			throw Exceptions::TimedDiskBuffer::IncompatibleListLengths();
		file_times = times;
	}
	
	//
	// GET FRAME
	//
	template<class T>
	inline CVD::DiskBuffer2Frame<T>* TimedDiskBuffer<T>::get_frame()
	{
		int current_frame = this->next_frame;
		if(current_frame < 0)
			current_frame = 0;
		// get a frame from the super class
		CVD::DiskBuffer2Frame<T> * vf = CVD::DiskBuffer2<T>::get_frame();
		vf->timestamp(file_times[current_frame]);
		return vf;
	}
}

#endif

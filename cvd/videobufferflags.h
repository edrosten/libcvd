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
// Paul Smith 8 March 2005
// Flags for video buffers 

#ifndef __VIDEOBUFFERFLAGS__
#define __VIDEOBUFFERFLAGS__

namespace CVD
{
	/// Flags common to several different VideoBuffer classes
	namespace VideoBufferFlags
	{
		/// If it is a finite buffer (a video file, for example), what should happen when the 
		/// end of the buffer is reached?
		enum OnEndOfBuffer{
			RepeatLastFrame, ///< Continue to return the final frame when get_frame() is called (with the same timestamp)
			UnsetPending, ///< Set the return value of frame_pending() to false and throw an EndOfBuffer exception if get_frame() is called
			Loop ///< Loop the buffer, so that the next time get_frame() is called it returns the first frame in the buffer
		};
	}
}
		
#endif

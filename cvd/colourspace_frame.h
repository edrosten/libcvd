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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef CVD_COLOURSPACE_FRAME_H
#define CVD_COLOURSPACE_FRAME_H

#include <cvd/localvideoframe.h>
#include <string>

namespace CVD
{
	//template<class To, class From> class ColourspaceBuffer;

	/// A frame from a ColourspaceBuffer. Can be treated as a VideoFrame
	template<class T> 
	class ColourspaceFrame : public CVD::LocalVideoFrame<T>
	{
		/// Allow ColourspaceBuffer to manage frames.
		public:
			~ColourspaceFrame()
			{
			}

			ColourspaceFrame(double time, CVD::Image<T>& converted)
			:LocalVideoFrame<T>(time, converted)
			{
			}	
	};
}


#endif

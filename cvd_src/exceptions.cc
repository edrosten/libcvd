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
#include <cvd/exceptions.h>
#include <cvd/videobuffer.h>

CVD::Exceptions::OutOfMemory::OutOfMemory()
{
	what="Out of memory.";
}

CVD::Exceptions::VideoBuffer::BadPutFrame::BadPutFrame()
{	
	what="Attempted to put_frame() on the wrong kind of frame.";
}

CVD::Exceptions::VideoBuffer::BadColourSpace::BadColourSpace(const std::string& c, const std::string& buffer)
{	
	what=buffer + " can not grab video in the " + c + "colourspace on the specified device.";
}

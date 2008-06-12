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
#include <cvd/config.h>
#include <cvd/exceptions.h>
#include <cvd/diskbuffer2.h>

#include <string>
#include <sstream>

using namespace CVD::Exceptions;
using namespace std;


DiskBuffer2::NoFiles::NoFiles()
{
	what = "DiskBuffer2 has an empty list of files.";
}

DiskBuffer2::BadImageSize::BadImageSize(const std::string& name)
{
	what = "DiskBuffer2: Image \"" + name + "\" does not match diskbuffer size.";
}

DiskBuffer2::BadFile::BadFile(const std::string& name, int err)
{
	what="DiskBuffer2: File open on \""+name+"\" failed with " + strerror(err);
}

DiskBuffer2::BadImage::BadImage(const std::string& file, const std::string & error)
{
	what="DiskBuffer2: File \""+file+"\" load failed: " + error;
}

DiskBuffer2::EndOfBuffer::EndOfBuffer()
{
	what="DiskBuffer2 has reached the end of its list of files";
}

DiskBuffer2::BadSeek::BadSeek(double t)
{
	ostringstream ss;
	ss << "DiskBuffer2: Seek to time " << t << "mS failed. No such time in the buffer";
	what = ss.str();
}

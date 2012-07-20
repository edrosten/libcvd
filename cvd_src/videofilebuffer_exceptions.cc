/*                       
	This file is part of the CVD Library.

	Copyright (C) 2012 E. Rosten

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

#include <cvd/videofilebuffer.h>

#include <sstream>
using namespace std;
namespace CVD
{

using namespace Exceptions::VideoFileBuffer;
	
//
// EXCEPTIONS
//

Exceptions::VideoFileBuffer::FileOpen::FileOpen(const std::string& name, const string& error)
{
	what = "RawVideoFileBuffer: Error opening file \"" + name + "\": " + error;
}

Exceptions::VideoFileBuffer::BadFrameAlloc::BadFrameAlloc()
{
	what = "RawVideoFileBuffer: Unable to allocate video frame.";
}

Exceptions::VideoFileBuffer::BadDecode::BadDecode(double t, const string& s)
{
	ostringstream os;
	os << "RawVideoFileBuffer: Error decoding video frame at time " << t;
	
	if(s == "")
		os 	<< ".";
	else
		os << ": " << s;
	what = os.str();
}

Exceptions::VideoFileBuffer::EndOfFile::EndOfFile()
{
	what =  "RawVideoFileBuffer: Tried to read off the end of the file.";
}

Exceptions::VideoFileBuffer::BadSeek::BadSeek(double t, const string& s)
{
	ostringstream os;
	os << "RawVideoFileBuffer: Seek to time " << t << "s failed";

	
	if(s == "")
		os 	<< ".";
	else
		os << ": " << s;

	what = os.str();
}

}

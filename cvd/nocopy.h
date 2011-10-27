/*
    This file is part of the CVD Library.

    Copyright (C) 2005-2011 The Authors

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

#ifndef CVD_INCLUDE_NOCOPY_H
#define CVD_INCLUDE_NOCOPY_H

namespace CVD {

/** Conveniently block the copy constructor and assignment operator of subclasses. */
class NoCopy {
protected:

    NoCopy() {}
   ~NoCopy() {}

private:

   NoCopy(const NoCopy &);
   const NoCopy & operator=(const NoCopy &);
};

}

#endif /* CVD_INCLUDE_NOCOPY_H */

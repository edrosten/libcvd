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
#include "cvd/diskbuffer2.h"
#include "cvd/config.h"
#include <glob.h>
#include <string>
#include <vector>

using namespace std;
using namespace CVD;

//
// GLOBLIST
// Makes a list of files matching a pattern
//
vector<string> CVD::globlist(const string& gl)
{
	vector<string> ret;

	glob_t g;
	unsigned int i;
	
	#ifdef CVD_INTERNAL_GLOB_IS_BAD
		glob(gl.c_str(), 0 ,  0 , &g);
	#else
		glob(gl.c_str(), GLOB_BRACE | GLOB_TILDE,  0 , &g);
	#endif

	for(i=0; i < g.gl_pathc; i++)
		ret.push_back(g.gl_pathv[i]);

	globfree(&g);

	if(ret.size() == 1 && ret[0] == "")
		ret.resize(0);

	return ret;
}

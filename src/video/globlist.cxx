#include "cvd/diskbuffer2.h"
#include <glob.h>
#include <string>
#include <vector>

using namespace std;
using namespace CVD;

//
// GLOBLIST
// Makes a list of files matching a pattern
//
namespace CVD{
	vector<string> globlist(const string& gl)
	{
		vector<string> ret;

		glob_t g;
		unsigned int i;

		glob(gl.c_str(), GLOB_BRACE | GLOB_TILDE,  0 , &g);

		for(i=0; i < g.gl_pathc; i++)
			ret.push_back(g.gl_pathv[i]);

		globfree(&g);

		if(ret.size() == 1 && ret[0] == "")
			ret.resize(0);

		return ret;
	}
}

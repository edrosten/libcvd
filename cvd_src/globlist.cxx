#include "cvd/diskbuffer2.h"
#include <glob.h>

using namespace std;
using namespace CVD;

vector<string> CVD::globlist(string gl)
{
	vector<string> ret;

	glob_t g;
	unsigned int i;
	
	glob(gl.c_str(), GLOB_BRACE | GLOB_TILDE,  0 , &g);
	
	for(i=0; i < g.gl_pathc; i++)
		ret.push_back(g.gl_pathv[i]);

	globfree(&g);

	return ret;
}


#ifndef CVD_EXCEPTIONS_H
#define CVD_EXCEPTIONS_H

#include <string>

namespace CVD
{
	namespace Exceptions
	{
		struct All
		{
			std::string what;
		};

		struct OutOfMemory: public All
		{
			OutOfMemory();	
		};
	}

}
#endif

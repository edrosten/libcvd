#ifndef CVD_EXCEPTIONS_H
#define CVD_EXCEPTIONS_H

#include <string>


namespace CVD
{
	
	/// %All exceptions thrown by CVD objects and functions are contained within this namespace.
	/// @ingroup gException
	namespace Exceptions
	{
		/// Base class for all CVD exceptions 
		struct All
		{
			std::string what; ///< The error message
		};

		/// Out of memory exception
		struct OutOfMemory: public All
		{
			OutOfMemory();	
		};
	}

}
#endif

#ifndef CVD_EXCEPTIONS_H
#define CVD_EXCEPTIONS_H

#include <stdexcept>

namespace CVD
{

/// %All exceptions thrown by CVD objects and functions are contained within this namespace.
/// @ingroup gException
namespace Exceptions
{
	/// Base class for all CVD exceptions
	/// @ingroup gException
	struct All : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
		All()
		    : std::runtime_error("No exception message provided")
		{
		}
		All(const std::string& whatarg)
		    : std::runtime_error(whatarg)
		{
		}
	};

	/// Out of memory exception
	/// @ingroup gException
	struct OutOfMemory : public All
	{
		OutOfMemory();
	};
}

}
#endif

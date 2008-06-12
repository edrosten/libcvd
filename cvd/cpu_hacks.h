#ifndef CVD_CPU_HACKS_H
#define CVD_CPU_HACKS_H
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

#if CVD_HAVE_FENV_H
	#include <fenv.h>
#endif

#if CVD_HAVE_MMXEXT
    #include <xmmintrin.h>
#endif

namespace CVD
{


	/// Enable floating point exceptions. This function may
	/// do nothing, depending on the architecture
	inline void enableFPE();

	#ifndef DOXYGEN_IGNORE_INTERNAL
		#ifdef CVD_HAVE_FENV_H
			inline void enableFPE() 
			{ 
				feclearexcept(FE_ALL_EXCEPT);
				feenableexcept(FE_DIVBYZERO|FE_INVALID); 
			}
		#else
			/// Enable floating point exceptions. This function may
			/// do nothing, depending on the architecture
			inline void enableFPE() 
			{ 
			}
		#endif
	#endif

	/// Prefetch memory. This function might do nothing, depending on the
	/// architecture, or it might prefetch memory. Either way it will have
	/// no effect on the computation except to (possibly) speed it up.
	/// @param ptr The address of the memory to prefetch.
	/// @param I   The type of prefetch. This depends on the architecture.
	///            	- x86, MMXEXT
	///					- 0: prefetcht0
	///					- 1: prefetcht1
	///					- 2: prefetcht2
	///					- *: prefetchnta  (default)
	///				- Everything else
	///					- *: nothing
	template<int I> inline void prefetch(const void* ptr);
	
	inline void prefetch(const void* ptr);


	#ifndef DOXYGEN_IGNORE_INTERNAL
		#ifdef  CVD_HAVE_MMXEXT	
			template<int I> inline void prefetch(const void* ptr)
			{
                _mm_prefetch((char *)ptr, _MM_HINT_NTA);
			}

			template<> inline void prefetch<0>(const void* ptr)
			{
				_mm_prefetch((char *)ptr, _MM_HINT_T0);
			}
			
			template<> inline void prefetch<1>(const void* ptr)
			{
				_mm_prefetch((char *)ptr, _MM_HINT_T1);
			}

			template<> inline void prefetch<2>(const void* ptr)
			{
                _mm_prefetch((char *)ptr, _MM_HINT_T2);
			}
		
			inline void prefetch(const void* ptr)
			{
				prefetch<-1>(ptr);
			}

		#else
			template<int i> inline void prefetch(const void*){}
			inline void prefetch(const void*){}

		#endif
	#endif
}
#endif



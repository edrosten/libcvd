#ifndef CVD_ALIGNED_MEM_H
#define CVD_ALIGNED_MEM_H
#include <iostream>
#include <map>

namespace CVD
{
namespace Internal
{

template <class T, int N=20> struct placement_delete
{
	enum 
	{ 
		Size = (1<<N) 
	};
	
	struct Array
	{ 
		T data[Size]; 
	};

	static inline void destruct(T* buf) 
	{
		cout << "destruct<" << N << ">(" << (void*)buf << ")" << endl;
		(*(Array*)buf).~Array();
	}

	inline placement_delete(T* buf, size_t M) 
	{
		cout << "placement_delete<" << N << ">(" << (void*)buf << ", " << M << ")" << endl;
		if (M == 0)
		  return;
		if (M >= Size) 
		{
		  placement_delete<T,N>(buf+Size,M-Size);
		  destruct(buf);
		} 
		else if (M < Size) 
		{
		  placement_delete<T,N-1>(buf, M);
		}
	}
};

template <class T> struct placement_delete<T,-1>
{
	inline placement_delete(T* buf, size_t M) {}
};


class aligned_mem
{
	static const int alignment = 0x10;
	
	struct memory_chunk
	{
		size_t n_elements;
		unsigned char*  base;
	};

	std::map<void*, memory_chunk> memory;

	void* aligned_raw_alloc(size_t n_elem, size_t elem_size)
	{
		//Allocate enough unaligned memory to support alignment
		unsigned char* memory_buf = new unsigned char[n_elem * elem_size + alignment];
		
		//Compute offset required to align memoty
		int off = (int)((size_t)memory_buf & (alignment-1));

		void * mem_aligned = memory_buf +( alignment - off);

		memory_chunk new_chunk={n_elem, memory_buf};

		memory[mem_aligned] = new_chunk;

		return mem_aligned;
	}

	template<class C> void destruct(C* mem_ptr)
	{
		size_t n = memory[mem_ptr].n_elements;
		
		//Destruct all data
		//Can we do this faster for POD?
//		do
//			mem_ptr[--n].~C();
//		while(n);

		placement_delete<C>(mem_ptr, n);

	}

	public:
	
		template<class C> C* alloc(int n)
		{
			return new(aligned_raw_alloc(n,sizeof(C))) C[n];
		}

		template<class C> void free(C* mem_ptr)
		{
			
			
			//Free memory
			delete[] memory[mem_ptr].base;

			//Erase reference to memory
			memory.erase(mem_ptr);
		}
};


}

}

using namespace CVD;

#endif

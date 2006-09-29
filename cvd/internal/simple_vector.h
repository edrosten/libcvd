#ifndef CVD_INTERNAL_SIMPLE_VECTOR_H
#define CVD_INTERNAL_SIMPLE_VECTOR_H

namespace CVD{
namespace Internal{
	

	template<class T> class simple_vector
	{
		private:
			T* dat;

		public:
			simple_vector(size_t n)
			:dat(new T[n])
			{}

			T* data()
			{
				return dat;
			}

			T& operator[](size_t n)
			{
				return dat[n];
			}

			~simple_vector()
			{
				delete[] dat;
			}
	};


}}

#endif

#ifndef DISKBUFFER2_FRAME_H
#define DISKBUFFER2_FRAME_H

#include <cvd/videoframe.h>
#include <string>

namespace CVD
{
	template<class T> class DiskBuffer2;
	
	template<class T> class DiskBuffer2Frame: public VideoFrame<T>
	{
		friend class DiskBuffer2<T>;

		public:		
			const std::string& name() {return *frame_name;};

		protected:
			~DiskBuffer2Frame()
			{
			}

			DiskBuffer2Frame(double time, T* data, const ImageRef& size)
			:VideoFrame<T>(time, data, size)
			{
			}	

			
		private:
			const std::string *frame_name;
	};
}


#endif

#ifndef CVD_DISKBUFFER2_FRAME_H
#define CVD_DISKBUFFER2_FRAME_H

#include <cvd/localvideoframe.h>
#include <string>

namespace CVD
{
	template<class T> class DiskBuffer2;

	template<class T> class DiskBuffer2Frame: public CVD::LocalVideoFrame<T>
	{
		public:		
			const std::string& name() {return frame_name;};
			friend class CVD::DiskBuffer2<T>;

		protected:
			~DiskBuffer2Frame()
			{
			}

			DiskBuffer2Frame(double time, CVD::Image<T>& from_disk, const std::string& file)
			:LocalVideoFrame<T>(time, from_disk),frame_name(file)
			{
			}	

			
		private:
			const std::string& frame_name;
	};
}


#endif

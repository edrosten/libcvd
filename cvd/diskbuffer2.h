#ifndef DISKBUFFER2_H
#define DISKBUFFER2_H

#include <vector>
#include <string>
#include <fstream>
#include <errno.h>

#include <cvd/videobuffer.h>
#include <cvd/diskbuffer2_frame.h>
#include <cvd/image_io.h>

#include <sys/time.h>

namespace CVD
{
	std::vector<std::string> globlist(const std::string& gl);
	
	namespace Exceptions
	{
		namespace DiskBuffer2
		{
			struct All: public CVD::Exceptions::VideoBuffer::All { };
			struct NoFiles: public All { NoFiles(); };
			struct BadImageSize: public All { BadImageSize(const std::string&); };
			struct BadFile: public All { BadFile(const std::string&, int); };
			struct BadImage: public All { BadImage(const std::string& file, const std::string& error); };
			struct EndOfBuffer: public All { EndOfBuffer(); };
			struct BadSeek: public All { BadSeek(unsigned long long int t); };
		}
	}

	template<typename T> class DiskBuffer2: public VideoBuffer<T>
	{
		public:
			DiskBuffer2(const std::vector<std::string>& names, double fps);

 			virtual ImageRef size()
				{return my_size;}
			virtual DiskBuffer2Frame<T>* get_frame();
			virtual void put_frame(VideoFrame<T>* f);
			virtual bool frame_pending()
				{return frame_ready;}
			virtual void seek_to(unsigned long long int t);
		
			enum OnEndOfBuffer{RepeatLastFrame, UnsetPending, Loop};

			virtual void on_end_of_buffer(OnEndOfBuffer behaviour) 
				{end_of_buffer_behaviour = behaviour;};
		
		private:
			ImageRef my_size;
			int		 next_frame;
			double   start_time;
			double	 time_per_frame;
			bool frame_ready;
			std::vector<std::string> file_names;
			OnEndOfBuffer end_of_buffer_behaviour;
	};

	//
	// CONSTRUCTOR
	//
	template<typename T>
	inline DiskBuffer2<T>::DiskBuffer2(const std::vector<std::string>& names, double fps) :
		end_of_buffer_behaviour(RepeatLastFrame)
	{
		start_time = 0;

		next_frame=0;
		
		time_per_frame = 1/fps;	

		file_names = names;

		if(file_names.size() == 0)
			throw Exceptions::DiskBuffer2::NoFiles();

		Image<T> foo;
		std::ifstream im;
		im.open(names[0].c_str());

		if(!im.good())
			throw Exceptions::DiskBuffer2::BadFile(names[0], errno);
		
		try{
			pnm_load(foo, im);
		}
		catch(Exceptions::Image_IO::All err)
		{
			throw Exceptions::DiskBuffer2::BadImage(names[0], err.what);
		}

		my_size = foo.size();
		frame_ready = true;
	}

	//
	// GET FRAME
	//
	template<typename T>
	inline DiskBuffer2Frame<T>* DiskBuffer2<T>::get_frame()
	{
		if(next_frame < 0)
			next_frame = 0;

		if(!frame_pending())
			throw Exceptions::DiskBuffer2::EndOfBuffer();

		std::ifstream im_file;	
		
		T* data = new T[my_size.x * my_size.y];
						
		DiskBuffer2Frame<T>* vf=new DiskBuffer2Frame<T>(next_frame * time_per_frame + start_time, data, my_size);

		im_file.open(file_names[next_frame].c_str());
		if(!im_file.good())
			throw Exceptions::DiskBuffer2::BadFile(file_names[next_frame], errno);

		try{
			pnm_load(*vf, im_file);
		}
		catch(CVD::Exceptions::Image_IO::All err)
		{
			throw Exceptions::DiskBuffer2::BadImage(file_names[next_frame], err.what);
		}

		vf->frame_name = &file_names[next_frame];

		next_frame++;
		
		if(next_frame > (int)file_names.size()-1)
		{
			switch(end_of_buffer_behaviour)
			{
				case RepeatLastFrame:
					next_frame = file_names.size()-1;
					break;
				
				case UnsetPending:
					frame_ready = false;
				   break;
				
				case Loop:
					next_frame = 0;
					break;
			}
		}

		return vf;	
	}

	//
	// PUT FRAME
	//
	template<typename T>
	inline void DiskBuffer2<T>::put_frame(VideoFrame<T>* f)
	{
		//Pop quiz: What if someone put_frame'e the wrong type?
		delete[] static_cast<DiskBuffer2Frame<T>*>(f)->data();
		delete static_cast<DiskBuffer2Frame<T>*>(f);
	}

	//
	// SEEK TO
	//
	template<typename T>
	inline void DiskBuffer2<T>::seek_to(unsigned long long int t)
	{
		// t is in ms, but work in seconds
		// round the answer to the nearest whole frame
	   int frameno = static_cast<int>((t*0.001 - start_time) / time_per_frame + 0.5);
		if(frameno < 0 || static_cast<unsigned int>(frameno) > (file_names.size() - 1))
			throw Exceptions::DiskBuffer2::BadSeek(t);
		next_frame = frameno;
		frame_ready = true;
	}

}


#endif

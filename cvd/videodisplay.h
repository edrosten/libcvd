//-*- c++ -*-
//////////////////////////////////////////////////////////////////////
//                                                                  //
//   VideoDisplay - Cheap and cheeful way of creating a GL X        //
//                  display                                         //
//                                                                  //
//   Tom Drummong & Paul Smith   2002                               //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifndef __VIDEODISPLAY_H
#define __VIDEODISPLAY_H

#include <string>
#include <assert.h>
#include <math.h>

#include <X11/Xlib.h>   //X11
#include <GL/glx.h>  // OpenGL (X extensions)
#include <GL/gl.h>   // OpenGL proper

#include <cvd/exceptions.h>

namespace CVD {

	namespace Exceptions
	{
		namespace VideoDisplay
		{
			struct All: public CVD::Exceptions::All{};

			struct InitialisationError: public All
			{
				InitialisationError(std::string w);
			};

			struct RuntimeError: public All
			{
				RuntimeError(std::string w);
			};
		}
	}


	extern int defAttr[];

	class VideoDisplay
	{
		public:
			VideoDisplay(double left, double top, double right, double bottom, double scale=1, int* visualAttr = defAttr);
			~VideoDisplay();

			void set_title(const std::string& s);
			int get_fd() {return ConnectionNumber(my_display);}
			void select_events(long event_mask);
			void get_event(XEvent* event);
			void locate_display_pointer(int& x, int& y);
			void locate_video_pointer(double& x, double& y);
			void flush();
			int pending();

			void set_zoom(double left, double top, double right, double bottom, double scale);
			void zoom_in(double cx, double cy, double factor = 2);
			void zoom_out(double cx, double cy, double factor = 2)
			{assert(factor != 0.0); zoom_in(cx, cy, 1/factor);}
			void zoom_reset()
			{set_zoom(my_orig_left, my_orig_top, my_orig_right, my_orig_bottom, my_orig_scale);}

			double video_width() const {return fabs(my_right - my_left);}
			double video_height() const {return fabs(my_bottom - my_top);}

			void display_to_video(int dx, int dy, double& vx, double& vy);
			void video_to_display(double dx, double dy, int& vx, int & vy);

			void make_current();

			Display* display() {return my_display;}
			Window window() {return my_window;}

		private:
			double my_left;
			double my_top;
			double my_right;
			double my_bottom;
			bool my_positive_right;
			bool my_positive_down;
			double my_scale;
			double my_orig_left;
			double my_orig_top;
			double my_orig_right;
			double my_orig_bottom;
			double my_orig_scale;
			Display *my_display;
			XVisualInfo* my_visual;
			GLXContext my_glx_context;
			Colormap my_cmap;
			Window my_window;

			VideoDisplay( VideoDisplay& copyof );
			int operator = ( VideoDisplay& copyof );
	};
   
} // CVD

#endif

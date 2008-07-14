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
// -*- c++ -*-

#ifndef __DVBUFFER_3_H
#define __DVBUFFER_3_H
#include <cvd/videobuffer.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#include <cvd/colourspaces.h>

namespace CVD
{
  namespace Exceptions
  {
    namespace DVBuffer3
    {
      /// Class for all DVBuffer3 exceptions
      /// @ingroup gException
      struct All : public CVD::Exceptions::VideoBuffer::All 
      {
	All(std::string sWhat)
	  {
	    what = "DVBuffer3: " + sWhat;
	  }
      };
    }
  }

  /// Internal DVBuffer3 helpers
  namespace DV3
  {
    /// This enumerates the list of controllable features
    /// (This isn't the full set of DC1394 ones, just a few.)
    enum DV3Feature { BRIGHTNESS,  EXPOSURE,  SHARPNESS,
		      WHITE_BALANCE,  HUE,  SATURATION,
		      GAMMA,  SHUTTER,  GAIN,  IRIS,
		      FOCUS, ZOOM,   PAN,  TILT};
    
    /// This enumerates all the colourspace types supported by DC1394
    /// N.b. only a small fraction of these map 1:1 to libCVD types
    enum DV3ColourSpace { MONO8,  MONO16,  MONO16S,
			  RGB8,	  RGB16,   RGB16S,
			  YUV411, YUV422,  YUV444,
			  RAW8,	  RAW16};
    
#ifndef DOXYGEN_IGNORE_INTERNAL
    // Translation helper classes to go from CVD-types to the above
    template<class C>  
      struct CSConvert
      {	static const DV3ColourSpace space = C::Error__type_not_valid_for_camera; };
    template<> struct CSConvert<byte>
      {	static const DV3ColourSpace space = MONO8;};
    template<> struct CSConvert<short unsigned int>
      {	static const DV3ColourSpace space = MONO16;};
    template<> struct CSConvert<yuv411>
      {	static const DV3ColourSpace space = YUV411;};
    template<> struct CSConvert<yuv422>
      {	static const DV3ColourSpace space = YUV422;};
    template<> struct CSConvert<Rgb<byte> > 
      {	static const DV3ColourSpace space = RGB8;};
    
    struct LibDCParams;
#endif
    
    /// Non-templated libDC1394 interface. This is used by DVBuffer3. If you want 
    /// typed video frames, you should use DVBuffer 3 instead..
    /// The implementation of this class depends on which version of libDC1394 is 
    /// installed on the system.
    class RawDVBuffer3
    {
    public:
      /// Mode-selecting constructor for all standard modes (not Mode7)
      /// @param colourspace Enumerated colourspace requested
      /// @param nCamNumber Which camera on the bus to use
      /// @param irSize Requested video size; if left at (-1,-1) use biggest available
      /// @param fFrameRate Requested frame-rate; if negative, use fastest available
      RawDVBuffer3(DV3ColourSpace colourspace,
		   unsigned int nCamNumber=0, 
		   ImageRef irSize = ImageRef(-1,-1),
		   float fFrameRate=-1.0);
      
      ~RawDVBuffer3();
      inline ImageRef size() {return mirSize;}
      inline double frame_rate() {return mdFramerate;}
      VideoFrame<byte>* get_frame();
      void put_frame(VideoFrame<byte>* f);
      bool frame_pending();

      void set_feature_value(DV3Feature nFeature, unsigned int nValue);
      unsigned int get_feature_value(DV3Feature nFeature);
      std::pair<unsigned int, unsigned int> get_feature_min_max(DV3Feature nFeature);
      void auto_on_off(DV3Feature nFeature, bool bValue);
      
    private:
      ImageRef mirSize;
      double mdFramerate;
      /// This encapsulates the actual libDC1394 variables
      LibDCParams *mpLDCP;
    };
    
  }

  /// A video buffer from a Firewire (IEEE 1394) camera. 
  /// This can use both v1.x and v2.0 series of libdc1394. For v2 it
  /// selects the video mode as described for RawDVBuffer3 above. For
  /// v1 this just wraps DVBuffer2.
  template <class pixel_T> 
    class DVBuffer3 : public VideoBuffer<pixel_T>, public DV3::RawDVBuffer3
    {
    public:
    DVBuffer3(unsigned int nCamNumber=0, 
	      ImageRef irSize = ImageRef(-1,-1), 
	      float fFPS = -1.0)
      : RawDVBuffer3(DV3::CSConvert<pixel_T>::space, nCamNumber, irSize, fFPS)
	{
	}
      
      virtual ~DVBuffer3()   {}
      double frame_rate()    {return RawDVBuffer3::frame_rate();  }
      ImageRef size()        {return RawDVBuffer3::size(); }
      virtual VideoFrame<pixel_T>* get_frame()
      {
	return reinterpret_cast<VideoFrame<pixel_T>*>(RawDVBuffer3::get_frame());
      }
      virtual void put_frame(VideoFrame<pixel_T>* f)
      {
	RawDVBuffer3::put_frame(reinterpret_cast<VideoFrame<byte>*>(f));
      }
      virtual bool frame_pending() {return RawDVBuffer3::frame_pending();}
      virtual void seek_to(double){}
    };
  
}

#endif







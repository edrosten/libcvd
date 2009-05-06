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

#include <cvd/Linux/dvbuffer3.h>
#include <cvd/byte.h>
#include <dc1394/dc1394.h>
#include <vector>
#include <algorithm>

using namespace CVD;
using namespace DV3;
using CVD::Exceptions::DVBuffer3::All;

namespace CVD
{

  namespace DV3
  {

    struct LibDCParams
    {
      dc1394_t *pDC1394;
      dc1394camera_t *pCamera;
      LibDCParams()   {  pDC1394 = NULL; pCamera = NULL;  }
    };
    
    struct DV3Frame : public VideoFrame<byte>
    {
      DV3Frame(dc1394video_frame_t *pDC1394Frame)
	: VideoFrame<byte>(pDC1394Frame->timestamp * 1000000.0,
			   pDC1394Frame->image,
			   ImageRef(pDC1394Frame->size[0], pDC1394Frame->size[1]))
      {	mpDC1394Frame = pDC1394Frame; }
    protected:
      dc1394video_frame_t *mpDC1394Frame;
      friend class RawDVBuffer3;
    };
    
    static dc1394color_coding_t DC_from_DV3_ColourSpace(DV3ColourSpace s)
    {
      switch(s)
	{
	case MONO8:  return DC1394_COLOR_CODING_MONO8; 
	case MONO16: return DC1394_COLOR_CODING_MONO16; 
	case MONO16S:return DC1394_COLOR_CODING_MONO16S; 
	case RGB8:   return DC1394_COLOR_CODING_RGB8; 
	case RGB16:  return DC1394_COLOR_CODING_RGB16; 
	case RGB16S: return DC1394_COLOR_CODING_RGB16S; 
	case YUV411: return DC1394_COLOR_CODING_YUV411; 
	case YUV422: return DC1394_COLOR_CODING_YUV422; 
	case YUV444: return DC1394_COLOR_CODING_YUV444; 
	case RAW8:   return DC1394_COLOR_CODING_RAW8; 
	case RAW16:  return DC1394_COLOR_CODING_RAW16; 
	}
      throw(All("DC_from_DV3: Invalid colourspace"));
    }

    static dc1394feature_t DC_from_DV3_Feature(DV3Feature f)
    {
      switch(f)
	{
	case BRIGHTNESS:    return DC1394_FEATURE_BRIGHTNESS;
	case EXPOSURE  :    return DC1394_FEATURE_EXPOSURE;
	case SHARPNESS:     return DC1394_FEATURE_SHARPNESS;
	case WHITE_BALANCE: return DC1394_FEATURE_WHITE_BALANCE;
	case HUE:           return DC1394_FEATURE_HUE;
	case SATURATION:    return DC1394_FEATURE_SATURATION;
	case GAMMA:         return DC1394_FEATURE_GAMMA;
	case SHUTTER:       return DC1394_FEATURE_SHUTTER;
	case GAIN:          return DC1394_FEATURE_GAIN;
	case IRIS:          return DC1394_FEATURE_IRIS;
	case FOCUS:         return DC1394_FEATURE_FOCUS;
	case ZOOM:          return DC1394_FEATURE_ZOOM;
	case PAN:           return DC1394_FEATURE_PAN;
	case TILT:          return DC1394_FEATURE_TILT;
	case FRAME_RATE:    return DC1394_FEATURE_FRAME_RATE;
	}
      throw(All("DC_from_DV3: Invalid feature"));
    }
    
    RawDVBuffer3::RawDVBuffer3(DV3ColourSpace colourspace,
			       unsigned int nCamNumber,
			       ImageRef irSize,
			       float fFrameRate, 
			       ImageRef irOffset)
    {
      mpLDCP = new LibDCParams;
  
      // Create a libDC1394 context.
      mpLDCP->pDC1394 = dc1394_new();
  
      // A variable to record error values..
      dc1394error_t error;

      // Enumerate the cameras connected to the system.
      dc1394camera_list_t *pCameraList = NULL;
  
      error = dc1394_camera_enumerate(mpLDCP->pDC1394, &pCameraList);
      if(error) throw(All("Camera enumerate"));
  
      if(pCameraList->num == 0)
	{
	  dc1394_camera_free_list(pCameraList);
	  throw(All("No cameras found."));
	}
  
      if(nCamNumber + 1 > pCameraList->num)
	{
	  dc1394_camera_free_list(pCameraList);
	  throw(All("Selected camera out of range"));
	}
  
      // Allocate a camera struct...
      mpLDCP->pCamera = dc1394_camera_new(mpLDCP->pDC1394, 
					  pCameraList->ids[nCamNumber].guid);
      dc1394_camera_free_list(pCameraList);
  
      if(!mpLDCP->pCamera) throw(All("Failed on dc1394_camera_new"));
  
    
      // What mode to use?
      dc1394color_coding_t nTargetColourCoding = DC_from_DV3_ColourSpace(colourspace);
      bool foundAStandardMode = false;
	dc1394video_mode_t nMode;
	mColourfilter = UNDEFINED;
	if(irOffset.x == -1){
		try {
			// First, get a list of the modes which the camera supports.
			dc1394video_modes_t modes;
			error = dc1394_video_get_supported_modes(mpLDCP->pCamera, &modes);
			if(error) throw(All("Could not get modelist"));
	
			// Second: Build up a list of the modes which are the right colour-space
			std::vector<dc1394video_mode_t> vModes;
			for(unsigned int i = 0; i < modes.num; i++)
			{
			  dc1394video_mode_t nMode = modes.modes[i];
			  // ignore format 7 for now
			  if(nMode >= DC1394_VIDEO_MODE_FORMAT7_0) continue;
			  dc1394color_coding_t nColourCoding;
			  error=dc1394_get_color_coding_from_video_mode(mpLDCP->pCamera,nMode,&nColourCoding);
			  if(error) throw(All("Error in get_color_coding_from_video_mode"));
			  if(nColourCoding == nTargetColourCoding)
				vModes.push_back(nMode);
			}
			if(vModes.size() == 0) throw(-1);
			
			// Third: Select mode according to size
			bool bModeFound = false;
			dc1394video_mode_t nMode;
			if(irSize.x != -1)	   // Has the user specified a target size? Choose mode according to that..
				for(size_t i = 0; i<vModes.size(); i++){
					uint32_t x,y;
					dc1394_get_image_size_from_video_mode(mpLDCP->pCamera, vModes[i], &x, &y);
					if(x == (uint32_t) irSize.x && y == (uint32_t) irSize.y) {
						bModeFound = true;
						nMode = vModes[i];
						break;
					}
				}
			else  // If the user didn't specify a target size, choose the one with the 
			{ // highest resolution.
				sort(vModes.begin(), vModes.end());
				bModeFound = true;
				nMode = vModes.back();
			}
			if(!bModeFound) throw(-1);
			
			// Store the size of the selected mode..
			uint32_t x,y;
			dc1394_get_image_size_from_video_mode(mpLDCP->pCamera, nMode, &x, &y);
			mirSize.x = x;
			mirSize.y = y;
			mirOffset = irOffset;

			// Got mode, now decide on frame-rate. Similar thing: first get list, then choose from list.
			dc1394framerates_t framerates;
			dc1394framerate_t nChosenFramerate = DC1394_FRAMERATE_MIN;
			mdFramerate = -1.0;
			error = dc1394_video_get_supported_framerates(mpLDCP->pCamera,nMode,&framerates);
			if(error) throw(All("Could not query supported framerates"));
			
			if(fFrameRate > 0) // User wants a specific frame-rate?
			{
				for(unsigned int i=0; i<framerates.num; i++){
					float f_rate;
					dc1394_framerate_as_float(framerates.framerates[i],&f_rate); 
					if(f_rate == fFrameRate){
						nChosenFramerate = framerates.framerates[i];
						mdFramerate = f_rate;
						break;
					}
				}
			} else { // Just pick the highest frame-rate the camera can do.
				for(unsigned int i=0; i<framerates.num; i++){
					float f_rate;
					dc1394_framerate_as_float(framerates.framerates[i],&f_rate); 
					if(f_rate > mdFramerate) {
						nChosenFramerate = framerates.framerates[i];
						mdFramerate = f_rate;
					}
				}
			}
			if(mdFramerate == -1.0) throw(-1);
		
			// Selected mode and frame-rate; Now tell the camera to use these.
			// At the moment, hard-code the channel to speed 400. This is maybe something to fix in future?
			error = dc1394_video_set_iso_speed(mpLDCP->pCamera, DC1394_ISO_SPEED_400);
			if(error) throw(All("Could not set ISO speed."));
			
			error = dc1394_video_set_iso_channel(mpLDCP->pCamera, nCamNumber);
			if(error) throw(All("Could not set ISO channel.")); 
		
			error = dc1394_video_set_mode(mpLDCP->pCamera, nMode);
			if(error) throw(All("Could not set video mode"));
			  
			error = dc1394_video_set_framerate(mpLDCP->pCamera, nChosenFramerate);
			if(error) throw(All("Could not set frame-rate"));		 
			// no need to check Format 7 modes
			foundAStandardMode = true;
		}
		catch(int e){
			//cout << "DCBuffer is checking format 7 modes as well" << endl;
			foundAStandardMode = false;
		}
	}
	if(!foundAStandardMode){
		dc1394format7modeset_t modeset;
		error = dc1394_format7_get_modeset(mpLDCP->pCamera, &modeset);
		if(error) throw(All("Could not get Format 7 modes."));
		int index = 0;
		if(irOffset.x == -1)
			mirOffset = ImageRef(0,0);
		else
			mirOffset = irOffset;
		for(; index < DC1394_VIDEO_MODE_FORMAT7_NUM; ++index){
			const dc1394format7mode_t & mode = modeset.mode[index];
			// does the mode exist ?
			if(!mode.present) continue;
			// does it support the colour format we need ?
			unsigned int i;
			for(i = 0; i < mode.color_codings.num; ++i)
				if(mode.color_codings.codings[i] == nTargetColourCoding)
					break;
			if(i == mode.color_codings.num) continue;
			if(irSize.x != -1){
				// can it support the size ?
				if((irSize.x + mirOffset.x) > (int)mode.max_size_x || (irSize.y + mirOffset.y) > (int)mode.max_size_y || irSize.x % mode.unit_size_x != 0 || irSize.y % mode.unit_size_y != 0)
					continue;
			} else {
				irSize.x = mode.max_size_x;
				irSize.y = mode.max_size_y;
			}
			// found one
			break;
		}
		if(index == DC1394_VIDEO_MODE_FORMAT7_NUM) throw(All("Could not find any usable format!"));
		const dc1394format7mode_t & mode = modeset.mode[index];
		nMode = static_cast<dc1394video_mode_t>( DC1394_VIDEO_MODE_FORMAT7_0 + index);
	
		// At the moment, hard-code the channel to speed 400. This is maybe something to fix in future?
		error = dc1394_video_set_iso_speed(mpLDCP->pCamera, DC1394_ISO_SPEED_400);
		if(error) throw(All("Could not set ISO speed."));
	
		error = dc1394_video_set_iso_channel(mpLDCP->pCamera, nCamNumber);
		if(error) throw(All("Could not set ISO channel."));
	
		error = dc1394_video_set_mode(mpLDCP->pCamera, nMode);
		if(error) throw(All("Could not set video mode."));
	
		// frame rate calculations
		int num_packets = (int)(8000.0/fFrameRate + 0.5);
		int packet_size = (irSize.x * irSize.y * 8 + num_packets * 8 - 1 ) / (num_packets * 8);
		mdFramerate = fFrameRate;
		// offset calculations
		if(irOffset.x == -1){
			mirOffset.x = (mode.max_size_x - irSize.x) / 2;
			mirOffset.y = (mode.max_size_y - irSize.y) / 2;
		} else {
			mirOffset = irOffset;
		}
		mirSize = irSize;
		dc1394_format7_set_roi( mpLDCP->pCamera, nMode,
								nTargetColourCoding,
								packet_size, 
								mirOffset.x, // left
								mirOffset.y, // top
								mirSize.x, // width
								mirSize.y);	 // height
		dc1394color_filter_t filterType;
		error = dc1394_format7_get_color_filter(mpLDCP->pCamera, nMode, &filterType);
		mColourfilter = static_cast<DV3ColourFilter>(filterType - DC1394_COLOR_FILTER_MIN);
	}
	  
	// Hack Alert: The code below sets the iso channel without this
	// having been properly allocated!	Likewise we never allocate
	// bandwidth. Both of these could be allocated if the following
	// two lines were erased, and the `0' parameter to
	// dc1394_capture_setup were changed to
	// DC1394_CAPTURE_FLAGS_DEFAULT; but this causes problems when
	// the program crashes, as the resources are not deallocated
	// properly.
	// This hack emulates what dvbuffer.cc does using libdc1394v1.
	
	// Hack to disable resource allocation -- see above
	// error = dc1394_capture_setup(mpLDCP->pCamera, 4, DC1394_CAPTURE_FLAGS_DEFAULT);
	error = dc1394_capture_setup(mpLDCP->pCamera, 4, 0);
	if(error) throw(All("Could not setup capture."));
	
	error = dc1394_video_set_transmission(mpLDCP->pCamera, DC1394_ON);					
	if(error) throw(All("Could not start ISO transmission."));			
	}

    RawDVBuffer3::~RawDVBuffer3()
    {
      if(mpLDCP->pCamera)
	{
	  dc1394_video_set_transmission(mpLDCP->pCamera, DC1394_OFF);
	  dc1394_capture_stop(mpLDCP->pCamera);
	  dc1394_camera_set_power(mpLDCP->pCamera, DC1394_OFF);
	  dc1394_camera_free(mpLDCP->pCamera);
	}
    
      if(mpLDCP->pDC1394)
	dc1394_free(mpLDCP->pDC1394);
  
      delete mpLDCP;
    }
  
  
    bool RawDVBuffer3::frame_pending()
    {
      return true;
    }

    VideoFrame<byte>* RawDVBuffer3::get_frame()
    {
      dc1394error_t error;
      dc1394video_frame_t *pDC1394Frame;
    
      error = dc1394_capture_dequeue(mpLDCP->pCamera,
				     DC1394_CAPTURE_POLICY_WAIT, 
				     &pDC1394Frame);
    
      if(error) throw(All("Failed on deque"));
      
      DV3::DV3Frame *pDV3Frame = new DV3::DV3Frame(pDC1394Frame);
      return pDV3Frame;
    
    }

    void RawDVBuffer3::put_frame(VideoFrame<byte> *pVidFrame)
    {
      dc1394error_t error;
      DV3::DV3Frame *pDV3Frame = dynamic_cast<DV3::DV3Frame*>(pVidFrame);
      if(!pVidFrame) throw(All("put_frame got passed an alien frame"));

      error = dc1394_capture_enqueue(mpLDCP->pCamera, pDV3Frame->mpDC1394Frame);
      delete pDV3Frame;
    }

    unsigned int  RawDVBuffer3::get_feature_value(DV3Feature nFeature)
    {
      if(!mpLDCP || !mpLDCP->pCamera)
	return 0;
      dc1394error_t error;
      unsigned int nValue = 0;
      error = dc1394_feature_get_value(mpLDCP->pCamera, DC_from_DV3_Feature(nFeature), &nValue);
      if(error)
	return 0;
      else
	return nValue;
    }

    void RawDVBuffer3::set_feature_value(DV3Feature nFeature, unsigned int nValue)
    {
      if(!mpLDCP || !mpLDCP->pCamera)
	return;
      dc1394_feature_set_value(mpLDCP->pCamera, DC_from_DV3_Feature(nFeature), nValue);
    }
    
    std::pair<unsigned int,unsigned int> RawDVBuffer3::get_feature_min_max(DV3Feature nFeature)
    {
      std::pair<unsigned int, unsigned int> res;
      res.first = res.second = 0;
      if(!mpLDCP || !mpLDCP->pCamera)
	return res;
      dc1394error_t error;
      unsigned int nMin;
      unsigned int nMax;
      error = dc1394_feature_get_boundaries(mpLDCP->pCamera, 
					    DC_from_DV3_Feature(nFeature), 
					    &nMin,
					    &nMax);
      if(error)
	return res;
      res.first = nMin;
      res.second = nMax;
      return res;
    }
    
    void RawDVBuffer3::auto_on_off(DV3Feature nFeature, bool bValue)
    {
      if(!mpLDCP || !mpLDCP->pCamera)
	return;
      dc1394_feature_set_mode(mpLDCP->pCamera, DC_from_DV3_Feature(nFeature), 
			      bValue? DC1394_FEATURE_MODE_AUTO : DC1394_FEATURE_MODE_MANUAL);
    }
    
    void RawDVBuffer3::power_on_off(DV3Feature nFeature, bool bValue)
    {
      if(!mpLDCP || !mpLDCP->pCamera)
        return;
      dc1394_feature_set_power(mpLDCP->pCamera, DC_from_DV3_Feature(nFeature), 
                                  bValue? DC1394_ON : DC1394_OFF);
    }
  }

}

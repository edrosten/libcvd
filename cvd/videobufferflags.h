// Paul Smith 8 March 2005
// Flags for video buffers 

#ifndef __VIDEOBUFFERFLAGS__
#define __VIDEOBUFFERFLAGS__

namespace CVD
{
	namespace VideoBufferFlags
	{
		enum OnEndOfBuffer{RepeatLastFrame, UnsetPending, Loop};
	}
}
		
#endif

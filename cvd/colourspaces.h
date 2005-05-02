#ifndef CVD_COLOURSPACES_H
#define CVD_COLOURSPACES_H

namespace CVD
{

	/// A dedicated bayer datatype to configure the V4L1 device to return bayer images.
    /// It is probably only supported in Ethan's hacked driver for USB2.0 devices.
    /// @ingroup gVideoBuffer
	struct bayer
	{
		unsigned char val;
	};

	/// A datatype to represent yuv411 (uyyvyy) data, typically from firewire
	/// cameras. It can be used to configure dvbuffer to return this format.
	/// @ingroup gVideoBuffer
	struct yuv411
	{
		unsigned char val;
	};
}

#endif

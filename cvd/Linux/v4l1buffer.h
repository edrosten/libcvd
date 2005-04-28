//-*- c++ -*-
#ifndef __CVD_V4L1BUFFER_H
#define __CVD_V4L1BUFFER_H

#include <vector>
#include <linux/videodev.h>

#include <cvd/videobuffer.h>
#include <cvd/Linux/v4l1frame.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>

namespace CVD {

    /// A dedicated bayer datatype to configure the V4L1 device to return bayer images.
    /// Does not work yet and is only supported in Ethan's hacked driver for USB2.0 devices.
    /// @ingroup gVideoBuffer
    struct bayer {
        byte val;
        operator unsigned char() { return val; }
    };

namespace Pixel {

    template<class T> struct traits;

    template<> struct traits<bayer>
    {
        typedef int wider_type;
        static const bool integral = true;
        static const bool is_signed = false;
        static const int bits_used = 8;
        //static const byte max_intensity=(1 << bits_used) - 1;
        static unsigned char max_intensity() throw() { return (1 << bits_used) - 1; }
    };

    template<class T> struct Component;

    template<> struct Component<bayer>
    {
        typedef byte type;
        static const unsigned int count = 1;

        static const byte & get(const bayer & pixel, int i)
        {
            return pixel.val;
        }

        static byte & get(bayer & pixel, int i)
        {
            return pixel.val;
        }
    };
};

namespace Exceptions
{
    /// %Exceptions specific to V4L1Buffer
    /// @ingroup gException
    namespace V4L1Buffer
    {
        /// Base class for all V4L1 exceptions
        /// @ingroup gException
        struct All: public CVD::Exceptions::VideoBuffer::All{};
        /// Error opening the device
        /// @ingroup gException
        struct DeviceOpen: public All {DeviceOpen(std::string dev); ///< Construct from the device name
        };
        /// Error setting up the device
        /// @ingroup gException
        struct DeviceSetup: public All {DeviceSetup(std::string dev, std::string action);  ///< Construct from the device string and an error string
        };
        /// Error in a put_frame() call
        /// @ingroup gException
        struct PutFrame: public All {PutFrame(std::string dev, unsigned int number); ///< Construct from the device name
        };
        /// Error in a get_frame() call
        /// @ingroup gException
        struct GetFrame: public All {GetFrame(std::string dev, unsigned int number); ///< Construct from the device name
        };
    }
}

/// Internal V4L1 helpers
namespace V4L1
{
    #ifndef DOXYGEN_IGNORE_INTERNAL
    template<class C> struct cam_type
    {
        static const unsigned int mode = Error__type_not_valid_for_camera___Use_byte_or_rgb_of_byte;
    };

    template<> struct cam_type<byte>
    {
        static const unsigned int mode = VIDEO_PALETTE_GREY;
    };

    template<> struct cam_type<bayer>
    {
        static const unsigned int mode = VIDEO_PALETTE_YUV422;
    };

    template<> struct cam_type<Rgb<byte> >
    {
        static const unsigned int mode = VIDEO_PALETTE_RGB24;
    };
    #endif

    /** Internal (non type-safe) class used by V4L1Buffer classes to access video devices with
     * v4l1 drivers. Use V4L1Buffer to get the 8-bit greyscale or 24-bit color.
     */
    class RawV4L1
    {
    public:
        /** constructor
         * @param dev file name of the device to open
         * @param mode color palette to use (see linux/video.h for possible modes)
         */
        RawV4L1(const std::string & dev, unsigned int mode);
        virtual ~RawV4L1();
        /** Get the width in pixels of the captured frames. */
        const ImageRef& get_size() const;
        /** Set the size of the captured frames. */
        void set_size(const ImageRef& size);
        /** Set the color palette to use */
        void set_palette(unsigned int palette);
        /** Set the brightness in [0,1] of the captured frames. */
        void set_brightness(double brightness);
        /** returns current brightness setting. */
        double get_brightness(void) { return myBrightness; };
        /** Set the whiteness in [0,1] of the captured frames. */
        void set_whiteness(double whiteness);
        /** returns current whiteness setting. */
        double get_whiteness(void) { return myWhiteness; };
        /** Set the hue in [0,1] of the captured frames. */
        void set_hue(double hue);
        /** returns current hue setting. */
        double get_hue(void) { return myHue; };
        /** Set the contrast in [0,1] of the captured frames. */
        void set_contrast(double contrast);
        /** returns current contrast setting. */
        double get_contrast(void) { return myContrast; };
        /** Set the saturation in [0,1] of the captured frames. */
        void set_saturation(double saturation);
        /** returns current saturation setting. */
        double get_saturation(void) { return mySaturation; };
        /** Get current settings from the camera device. */
        void retrieveSettings();
        /** Commit the settings to the camera device. */
        void commitSettings();
        /** marks a frame as ready for capturing */
        void captureFrame(unsigned int buffer);
        /** returns the data of the next captured frame */
        unsigned char* get_frame();
        /** returns the frame data to be used for further capturing */
        void put_frame( unsigned char * );
        /// Get the camera frame rate
        double frame_rate();
        /// Is there a frame waiting in the buffer? This function does not block.
        bool frame_pending();

    private:
        std::string deviceName;
        int myDevice;
        ImageRef mySize;
        unsigned int myPalette;
        double myBrightness, myWhiteness, myContrast, myHue, mySaturation;
        unsigned int myBpp;

        std::vector<unsigned char*> myFrameBuf;
        std::vector<bool> myFrameBufState;
        unsigned int myNextRetrieveBuf;
    };
};

/// A video buffer from a v4l1 video device.
/// @param T The pixel type of the frames. Currently only <code><CVD::byte></code> works
/// which returns 8-bit grey scale. <code><CVD::Rgb<CVD::Byte> ></code> should work to but
/// crashes with my current driver ?! The pixel type used will automatically configure the
/// underlying RawV4L1 object to use the right video palette.
///
/// @note The grey images are returned upside down unless the kernel module is loaded with
/// the option flipvert.
/// @note frame_rate currently returns fixed 30 fps and frame_pending is untested.
/// @ingroup gVideoBuffer
template <class T> class V4L1Buffer : public VideoBuffer<T>, public V4L1::RawV4L1
{
public:
    /// Construct a video buffer
    /// @param dev file name of the device to use
    V4L1Buffer(const std::string & dev) : RawV4L1( dev, V4L1::cam_type<T>::mode ) {}

    virtual ImageRef size()
    {
        return RawV4L1::get_size();
    }
    virtual VideoFrame<T> * get_frame()
    {
        return new V4L1Frame<T>(0, (T *)RawV4L1::get_frame(), RawV4L1::get_size());
    }
    virtual void put_frame(VideoFrame<T>* f)
    {
        RawV4L1::put_frame((unsigned char *)f->data());
        delete reinterpret_cast<V4L1Frame<T> *>(f);
    }
    virtual bool frame_pending()
    {
        return RawV4L1::frame_pending();
    }
    virtual double frame_rate()
    {
        return RawV4L1::frame_rate();
    }

private:
    V4L1Buffer( V4L1Buffer& copyof );
    int operator = ( V4L1Buffer& copyof );
};

/// An 8-bit greyscale video buffer from a V4l1 video device.
/// @ingroup gVideoBuffer
typedef V4L1Buffer<byte> V4L1BufferByte;

};
#endif

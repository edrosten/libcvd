#.rst:
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   AVCodec_INCLUDE_DIRS   - where to find the TooN includes.
#   AVCodec_FOUND          - True if TooN found.

find_path(avcodec_INCLUDE_DIR NAMES libavcodec/avcodec.h PATH_SUFFIXES include)
find_path(avformat_INCLUDE_DIR NAMES libavformat/avformat.h PATH_SUFFIXES include)
find_path(avdevice_INCLUDE_DIR NAMES libavdevice/avdevice.h PATH_SUFFIXES include)
find_path(swscale_INCLUDE_DIR NAMES libswscale/swscale.h PATH_SUFFIXES include)

find_library(avcodec_LIBRARY NAMES avcodec PATH_SUFFIXES lib)
find_library(avutil_LIBRARY NAMES avutil PATH_SUFFIXES lib)
find_library(avformat_LIBRARY NAMES avformat PATH_SUFFIXES lib)
find_library(swscale_LIBRARY NAMES swscale PATH_SUFFIXES lib)
find_library(avdevice_LIBRARY NAMES avdevice PATH_SUFFIXES lib)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(AVCodec REQUIRED_VARS avcodec_LIBRARY avutil_LIBRARY avformat_LIBRARY swscale_LIBRARY avdevice_LIBRARY avcodec_INCLUDE_DIR)

if(AVCodec_found)
    set(AVCodec_INCLUDE_DIRS ${avcodec_INCLUDE_DIR} ${avformat_INCLUDE_DIR} ${avdevice_INCLUDE_DIR} ${swscale_INCLUDE_DIR})
    set(AVCodec_LIBRARIES ${avcodec_LIBRARY} ${avutil_LIBRARY} ${avformat_LIBRARY} ${swscale_LIBRARY} ${avdevice_LIBRARY})
endif()

mark_as_advanced(AVCodec_INCLUDE_DIRS AVCodec_LIBRARIES)

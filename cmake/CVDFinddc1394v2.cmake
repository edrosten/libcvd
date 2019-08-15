# This module defines the following variables:
#
# ::
#
#   CVD_dc1394v2_INCLUDE_DIRS - where to find the dc1394v2 includes.
#   CVD_dc1394v2_LIBRARIES    - where to find the dc1394v2 libraries.
#   CVD_dc1394v2_FOUND        - True if dc1394v2 found.

find_path(CVD_dc1394_INCLUDE_DIR NAMES dc1394/control.h PATH_SUFFIXES include)

find_library(CVD_dc1394_LIBRARY NAMES dc1394 PATH_SUFFIXES lib)
find_library(CVD_raw1394_LIBRARY NAMES raw1394 PATH_SUFFIXES lib)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CVD_dc1394v2 REQUIRED_VARS CVD_dc1394_INCLUDE_DIR CVD_dc1394_LIBRARY CVD_raw1394_LIBRARY)

if(CVD_dc1394v2_FOUND)
	set(CVD_dc1394v2_INCLUDE_DIRS ${CVD_dc1394_INCLUDE_DIR})
	set(CVD_dc1394v2_LIBRARIES ${CVD_dc1394_LIBRARY} ${CVD_raw1394_LIBRARY})
endif()

mark_as_advanced(CVD_dc1394_INCLUDE_DIR CVD_dc1394_LIBRARY CVD_raw1394_LIBRARY)

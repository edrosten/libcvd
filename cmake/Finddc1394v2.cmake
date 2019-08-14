#.rst:
# Finddc1394v2
# --------
#
# Find the native dc1394 v2 includes.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   dc1394v2_INCLUDE_DIRS - where to find the dc1394v2 includes.
#   dc1394v2_LIBRARIES    - where to find the dc1394v2 libraries.
#   dc1394v2_FOUND        - True if dc1394v2 found.

find_path(dc1394_INCLUDE_DIR NAMES dc1394/control.h PATH_SUFFIXES include)

find_library(dc1394_LIBRARY NAMES dc1394 PATH_SUFFIXES lib)
find_library(raw1394_LIBRARY NAMES raw1394 PATH_SUFFIXES lib)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dc1394v2 REQUIRED_VARS dc1394_INCLUDE_DIR dc1394_LIBRARY raw1394_LIBRARY)

if(dc1394v2_FOUND)
    set(dc1394v2_INCLUDE_DIRS ${dc1394_INCLUDE_DIR})
    set(dc1394v2_LIBRARIES ${dc1394_LIBRARY} ${raw1394_LIBRARY})
endif()

mark_as_advanced(dc1394_INCLUDE_DIR dc1394_LIBRARY raw1394_LIBRARY)

#.rst:
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   V4L2_INCLUDE_DIRS   - where to find the TooN includes.
find_path(V4l2_INCLUDE_DIR NAMES linux/videodev2.h PATH_SUFFIXES include)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(V4L2 REQUIRED_VARS V4l2_INCLUDE_DIR)

mark_as_advanced(V4l2_INCLUDE_DIR)

#.rst:
# CVDFindTooN
# -----------
#
# Find the native TooN includes.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   CVD_TooN_INCLUDE_DIRS   - where to find the TooN includes.
#   CVD_TooN_FOUND          - True if TooN found.

find_path(CVD_TooN_INCLUDE_DIR NAMES TooN/TooN.h PATH_SUFFIXES include)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CVD_TooN REQUIRED_VARS CVD_TooN_INCLUDE_DIR)

if(CVD_TooN_FOUND)
	set(CVD_TooN_INCLUDE_DIRS ${CVD_TooN_INCLUDE_DIR})
endif()

mark_as_advanced(CVD_TooN_INCLUDE_DIR )

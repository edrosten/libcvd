cmake_minimum_required(VERSION 3.10)
project(CVD)
enable_testing()

option(CVD_ENABLE_TESTS "Build libCVD tests" ON)
option(CVD_ENABLE_PROGS "Build libCVD programs" ON)
option(CVD_ENABLE_EXAMPLES "Build libCVD examples" ON)

include(TestBigEndian)
include(CheckSymbolExists)

set(CMAKE_DEBUG_POSTFIX _debug)

# Dependencies that can be automatically found by CMake.
include(cmake/CVDFindAllDeps.cmake)

# Detect endianness

TEST_BIG_ENDIAN(CVD_INTERNAL_ARCH_BIG_ENDIAN)
if(CVD_INTERNAL_ARCH_BIG_ENDIAN)
	set(CVD_INTERNAL_ARCH_LITTLE_ENDIAN OFF)
else()
	set(CVD_INTERNAL_ARCH_LITTLE_ENDIAN ON)
endif()

# Basic source files and headers for all platforms and options.

set(SRCS
	cvd_src/bayer.cxx
	cvd_src/colourspace_convert.cxx
	cvd_src/connected_components.cc
	cvd_src/convolution.cc
	cvd_src/cvd_timer.cc
	cvd_src/deinterlacebuffer.cc
	cvd_src/diskbuffer2.cc
	cvd_src/draw.cc
	cvd_src/exceptions.cc
	cvd_src/faster_corner_utilities.h
	cvd_src/image_io.cc
	cvd_src/morphology.cc
	cvd_src/nonmax_suppression.cxx
	cvd_src/quartic.cpp
	cvd_src/timeddiskbuffer.cc
	cvd_src/videofilebuffer_exceptions.cc
	cvd_src/videosource.cpp
	cvd_src/yuv411_to_stuff.cxx
	cvd_src/yuv420.cpp
	cvd_src/fast_corner.cxx
	cvd_src/fast/fast_corner_9_nonmax.cxx
	cvd_src/fast/fast_10_detect.cxx
	cvd_src/fast/fast_10_score.cxx
	cvd_src/fast/fast_11_detect.cxx
	cvd_src/fast/fast_11_score.cxx
	cvd_src/fast/fast_12_detect.cxx
	cvd_src/fast/fast_12_score.cxx
	cvd_src/fast/fast_7_detect.cxx
	cvd_src/fast/fast_7_score.cxx
	cvd_src/fast/fast_8_detect.cxx
	cvd_src/fast/fast_8_score.cxx
	cvd_src/fast/fast_9_detect.cxx
	cvd_src/fast/fast_9_score.cxx
	cvd_src/image_io/bmp.cxx
	cvd_src/image_io/bmp_read.cc
	cvd_src/image_io/bmp_write.cc
	cvd_src/image_io/cvdimage.cxx
	cvd_src/image_io/fits.cc
	cvd_src/image_io/fitswrite.cc
	cvd_src/image_io/pnm_grok.cxx
	cvd_src/image_io/save_postscript.cxx
	cvd_src/image_io/text.cxx
	cvd_src/image_io/text_write.cc
	cvd_src/noarch/convert_rgb_to_y.cc
	cvd_src/noarch/convolve_gaussian.cc
	cvd_src/noarch/gradient.cc
	cvd_src/noarch/half_sample.cc
	cvd_src/noarch/median_3x3.cc
	cvd_src/noarch/two_thirds_sample.cc
	cvd_src/noarch/utility_byte_differences.cc
	cvd_src/noarch/utility_double_int.cc
	cvd_src/noarch/utility_float.cc
	cvd_src/noarch/yuv422.cpp
	cvd_src/noarch/slower_corner_10.cxx
	cvd_src/noarch/slower_corner_12.cxx
	cvd_src/noarch/slower_corner_9.cxx)
set(HEADERS
	cvd/argb.h
	cvd/bgrx.h
	cvd/bresenham.h
	cvd/byte.h
	cvd/colourmap.h
	cvd/colourspace.h
	cvd/colourspacebuffer.h
	cvd/colourspaces.h
	cvd/colourspace_convert.h
	cvd/colourspace_frame.h
	cvd/connected_components.h
	cvd/convolution.h
	cvd/deinterlacebuffer.h
	cvd/deinterlaceframe.h
	cvd/diskbuffer2.h
	cvd/diskbuffer2_frame.h
	cvd/distance_transform.h
	cvd/documentation.h
	cvd/draw.h
	cvd/exceptions.h
	cvd/fast_corner.h
	cvd/gles1_helpers.h
	cvd/glwindow.h
	cvd/gl_helpers.h
	cvd/haar.h
	cvd/harris_corner.h
	cvd/helpers.h
	cvd/image.h
	cvd/image_convert.h
	cvd/image_convert_fwd.h
	cvd/image_io.h
	cvd/image_ref.h
	cvd/integral_image.h
	cvd/interpolate.h
	cvd/la.h
	cvd/localvideobuffer.h
	cvd/localvideoframe.h
	cvd/morphology.h
	cvd/nonmax_suppression.h
	cvd/opencv.h
	cvd/readaheadvideobuffer.h
	cvd/rgb.h
	cvd/rgb8.h
	cvd/rgba.h
	cvd/serverpushjpegbuffer.h
	cvd/serverpushjpegframe.h
	cvd/timeddiskbuffer.h
	cvd/timer.h
	cvd/utility.h
	cvd/videobuffer.h
	cvd/videobufferflags.h
	cvd/videobufferwithdata.h
	cvd/videofilebuffer.h
	cvd/videofilebuffer_frame.h
	cvd/videoframe.h
	cvd/videosource.h
	cvd/vision.h
	cvd/vision_exceptions.h
	cvd/yc.h
	cvd/video/skipbuffer.h
	cvd/internal/builtin_components.h
	cvd/internal/concurrency_utilities.h
	cvd/internal/convert_pixel_types.h
	cvd/internal/gles1_types.h
	cvd/internal/gl_types.h
	cvd/internal/image_ref_implementation.hh
	cvd/internal/io/bmp.h
	cvd/internal/io/cvdimage.h
	cvd/internal/io/fits.h
	cvd/internal/io/jpeg.h
	cvd/internal/io/parameter.h
	cvd/internal/io/png.h
	cvd/internal/io/pnm_grok.h
	cvd/internal/io/save_postscript.h
	cvd/internal/io/text.h
	cvd/internal/io/tiff.h
	cvd/internal/load_and_save.h
	cvd/internal/name_builtin_types.h
	cvd/internal/name_CVD_rgb_types.h
	cvd/internal/pixel_operations.h
	cvd/internal/pixel_traits.h
	cvd/internal/rgb_components.h
	cvd/internal/scalar_convert.h
	cvd/internal/win.h)


# Library-specific source files, headers and definitions.
if(CVD_dc1394v2_FOUND)
	list(APPEND SRCS
		cvd_src/videosource_dvbuffer.cc
		cvd_src/Linux/dvbuffer3_dc1394v2.cc)
	list(APPEND HEADERS
		cvd/Linux/dvbuffer.h
		cvd/Linux/dvbuffer3.h
		cvd/Linux/dvframe.h)
else()
	list(APPEND SRCS
		cvd_src/videosource_nodvbuffer.cc)
endif()

if(OpenGL_OpenGL_FOUND)
	list(APPEND SRCS
		cvd_src/gltext.cpp)

	if(X11_FOUND)
		list(APPEND SRCS
			cvd_src/glwindow.cc
			cvd_src/videodisplay.cc)
		list(APPEND HEADERS cvd/videodisplay.h)

		set(CVD_HAVE_GLWINDOW ON)
		set(CVD_HAVE_VIDEODISPLAY ON)
	endif()
endif()

# Platform-specific source files, headers and definitions.
if(WIN32)
	list(APPEND SRCS
		cvd_src/Win32/win32.cpp
		cvd_src/Win32/glwindow.cpp)
	set(CVD_HAVE_GLWINDOW ON)
endif()

check_symbol_exists(glob glob.h CVD_HAVE_GLOB)

if(CVD_HAVE_GLOB)
	list(APPEND SRCS cvd_src/globlist.cxx)
endif()

check_symbol_exists(V4L2_PIX_FMT_GREY linux/videodev2.h CVD_HAVE_V4L)
if(CVD_HAVE_V4L)
	list(APPEND SRCS
		cvd_src/Linux/v4lbuffer.cc
		cvd_src/Linux/v4lcontrol.cc
		cvd_src/Linux/videosource_v4lbuffer.cc)
	list(APPEND HEADERS
		cvd/Linux/v4lbuffer.h
		cvd/Linux/v4lcontrol.h)
else()
	list(APPEND SRCS cvd_src/Linux/videosource_nov4lbuffer.cc)
endif()

check_symbol_exists(uvc_stream_get_frame libuvc/libuvc.h CVD_HAVE_UVC)
if(CVD_HAVE_UVC)
	list(APPEND SRCS
		cvd_src/uvcbuffer.cc
		cvd_src/videosource_uvcbuffer.cc)
	list(APPEND HEADERS cvd/video/uvcbuffer.h)
else()
	list(APPEND SRCS
		cvd_src/videosource_nouvcbuffer.cc)
endif()


if(CVD_FFMPEG_FOUND)
	set(CVD_HAVE_FFMPEG ON)
	list(APPEND SRCS
		cvd_src/videofilebuffer2.cc
		cvd_src/videosource_videofilebuffer.cc)
else()
	set(CVD_HAVE_FFMPEG OFF)
	list(APPEND SRCS
		cvd_src/videosource_novideofilebuffer.cc)
endif()

if(CVD_TooN_FOUND)
	set(CVD_HAVE_TOON ON)
	list(APPEND SRCS
		cvd_src/brezenham.cc
		cvd_src/draw_toon.cc
		cvd_src/tensor_voting.cc
		cvd_src/threepointpose.cpp)
	list(APPEND HEADERS
		cvd/brezenham.h
		cvd/camera.h
		cvd/canny.h
		cvd/esm.h
		cvd/image_interpolate.h
		cvd/tensor_voting.h
		cvd/vector_image_ref.h
		cvd/geometry/threepointpose.h)
else()
	set(CVD_HAVE_TOON OFF)
endif()

if(JPEG_FOUND)
	set(CVD_HAVE_JPEG ON)
	set(CVD_INTERNAL_JPEG_BUFFER_SIZE 1)
    list(APPEND SRCS cvd_src/image_io/jpeg.cxx)
else()
	set(CVD_HAVE_JPEG OFF)
	set(CVD_INTERNAL_JPEG_BUFFER_SIZE OFF)
endif()

if(TIFF_FOUND)
	set(CVD_HAVE_TIFF ON)
    list(APPEND SRCS
		cvd_src/image_io/tiff.cxx
		cvd_src/image_io/tiffwrite.cc)
else()
	set(CVD_HAVE_TIFF OFF)
endif()

if(PNG_FOUND)
	set(CVD_HAVE_PNG ON)
    list(APPEND SRCS cvd_src/image_io/png.cc)
else()
	set(CVD_HAVE_PNG OFF)
endif()

configure_file(cmake/config.h.in include/cvd/config.h)
configure_file(cmake/config_internal.h.in include/cvd_src/config_internal.h)

add_library(${PROJECT_NAME} ${SRCS} ${HEADERS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/include" ${CVD_DEP_INCLUDES_PUBLIC} PRIVATE ${CVD_DEP_INCLUDES_PRIVATE})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CVD_DEP_LIBS})


if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
foreach(file ${HEADERS})
	get_filename_component(dir ${file} DIRECTORY)
	install(FILES ${file} DESTINATION include/${dir})
endforeach()
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/cvd DESTINATION include)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/cmake DESTINATION . FILES_MATCHING PATTERN *.cmake)

if(CVD_ENABLE_TESTS)
	add_subdirectory(tests)
endif()
if(CVD_ENABLE_PROGS)
	add_subdirectory(progs)
endif()
if(CVD_ENABLE_EXAMPLES)
	add_subdirectory(examples)
endif()

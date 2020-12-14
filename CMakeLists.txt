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

set(BASE_SRCS
	src/base/bayer.cxx
	src/base/colourspace_convert.cxx
	src/base/connected_components.cc
	src/base/convert_rgb_to_y.cc
	src/base/convolution.cc
	src/base/convolve_gaussian.cc
	src/base/cvd_timer.cc
	src/base/draw.cc
	src/base/exceptions.cc
	src/base/fast_corner.cxx
	src/base/fast/fast_10_detect.cxx
	src/base/fast/fast_10_score.cxx
	src/base/fast/fast_11_detect.cxx
	src/base/fast/fast_11_score.cxx
	src/base/fast/fast_12_detect.cxx
	src/base/fast/fast_12_score.cxx
	src/base/fast/fast_7_detect.cxx
	src/base/fast/fast_7_score.cxx
	src/base/fast/fast_8_detect.cxx
	src/base/fast/fast_8_score.cxx
	src/base/fast/fast_9_detect.cxx
	src/base/fast/fast_9_score.cxx
	src/base/fast/fast_corner_9_nonmax.cxx
	src/base/fast/slower_corner_10.cxx
	src/base/fast/slower_corner_11.cxx
	src/base/fast/slower_corner_12.cxx
	src/base/fast/slower_corner_7.cxx
	src/base/fast/slower_corner_8.cxx
	src/base/fast/slower_corner_9.cxx
	src/base/gradient.cc
	src/base/half_sample.cc
	src/base/median_3x3.cc
	src/base/morphology.cc
	src/base/nonmax_suppression.cxx
	src/base/quartic.cpp
	src/base/two_thirds_sample.cc
	src/base/utility_byte_differences.cc
	src/base/utility_double_int.cc
	src/base/utility_float.cc
	src/base/yuv411_to_stuff.cxx
	src/base/yuv420.cpp
	src/base/yuv422.cpp)
set(VIDEO_SRCS
	src/video/deinterlacebuffer.cc
	src/video/diskbuffer2.cc
	src/video/timeddiskbuffer.cc
	src/video/video_exceptions.cc
	src/video/videofilebuffer_exceptions.cc
	src/video/videosource.cpp)
set(IMAGEIO_SRCS
	src/image_io/bmp.cxx
	src/image_io/bmp_read.cc
	src/image_io/bmp_write.cc
	src/image_io/cvdimage.cxx
	src/image_io/image_io.cc
	src/image_io/fits.cc
	src/image_io/fitswrite.cc
	src/image_io/pnm_grok.cxx
	src/image_io/save_postscript.cxx
	src/image_io/text.cxx
	src/image_io/text_write.cc)
set(BASE_HEADERS
	cvd/argb.h
	cvd/bgrx.h
	cvd/bresenham.h
	cvd/byte.h
	cvd/colourmap.h
	cvd/colourspace.h
	cvd/colourspaces.h
	cvd/colourspace_convert.h
	cvd/connected_components.h
	cvd/convolution.h
	cvd/distance_transform.h
	cvd/documentation.h
	cvd/draw.h
	cvd/exceptions.h
	cvd/fast_corner.h
	cvd/haar.h
	cvd/harris_corner.h
	cvd/helpers.h
	cvd/image.h
	cvd/image_convert.h
	cvd/image_convert_fwd.h
	cvd/image_ref.h
	cvd/integral_image.h
	cvd/interpolate.h
	cvd/la.h
	cvd/morphology.h
	cvd/nonmax_suppression.h
	cvd/opencv.h
	cvd/rgb.h
	cvd/rgb8.h
	cvd/rgba.h
	cvd/timer.h
	cvd/utility.h
	cvd/vision.h
	cvd/vision_exceptions.h
	cvd/yc.h
	cvd/internal/builtin_components.h
	cvd/internal/convert_pixel_types.h
	cvd/internal/image_ref_implementation.hh
	cvd/internal/name_builtin_types.h
	cvd/internal/name_CVD_rgb_types.h
	cvd/internal/pixel_operations.h
	cvd/internal/pixel_traits.h
	cvd/internal/rgb_components.h
	cvd/internal/scalar_convert.h
	cvd/internal/win.h)
set(GL_HEADERS
	cvd/gl_helpers.h
	cvd/gles1_helpers.h
	cvd/glwindow.h
	cvd/internal/gl_types.h
	cvd/internal/gles1_types.h)
set(IMAGEIO_HEADERS
	cvd/image_io.h
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
	cvd/internal/load_and_save.h)
set(VIDEO_HEADERS
	cvd/colourspace_frame.h
	cvd/colourspacebuffer.h
	cvd/deinterlacebuffer.h
	cvd/deinterlaceframe.h
	cvd/diskbuffer2_frame.h
	cvd/diskbuffer2.h
	cvd/localvideobuffer.h
	cvd/localvideoframe.h
	cvd/serverpushjpegbuffer.h
	cvd/serverpushjpegframe.h
	cvd/timeddiskbuffer.h
	cvd/videobuffer.h
	cvd/videobufferflags.h
	cvd/videobufferwithdata.h
	cvd/videofilebuffer_frame.h
	cvd/videofilebuffer.h
	cvd/videoframe.h
	cvd/videosource.h
	cvd/video/skipbuffer.h
	cvd/video/uvcbuffer.h)


# Library-specific source files, headers and definitions.
if(CVD_dc1394v2_FOUND)
	list(APPEND VIDEO_SRCS
		src/video/videosource_dvbuffer.cc
		src/video/Linux/dvbuffer3_dc1394v2.cc)
	list(APPEND VIDEO_HEADERS
		cvd/Linux/dvbuffer.h
		cvd/Linux/dvbuffer3.h
		cvd/Linux/dvframe.h)
else()
	list(APPEND VIDEO_SRCS
		src/video/videosource_nodvbuffer.cc)
endif()

if(OpenGL_OpenGL_FOUND)
	list(APPEND GL_SRCS
		src/gl/gltext.cpp)

	if(X11_FOUND)
		list(APPEND GL_SRCS
			src/gl/glwindow.cc
			src/gl/videodisplay.cc)
		list(APPEND GL_HEADERS cvd/videodisplay.h)

		set(CVD_HAVE_GLWINDOW ON)
		set(CVD_HAVE_VIDEODISPLAY ON)
	endif()
endif()

# Platform-specific source files, headers and definitions.
if(WIN32)
	list(APPEND VIDEO_SRCS
		src/video/Win32/win32.cpp)
	list(APPEND GL_SRCS
		src/gl/Win32/glwindow.cpp)
	set(CVD_HAVE_GLWINDOW ON)
endif()

check_symbol_exists(glob glob.h CVD_HAVE_GLOB)

if(CVD_HAVE_GLOB)
	list(APPEND VIDEO_SRCS src/video/globlist.cxx)
endif()

check_symbol_exists(V4L2_PIX_FMT_GREY linux/videodev2.h CVD_HAVE_V4L)
if(CVD_HAVE_V4L)
	list(APPEND VIDEO_SRCS
		src/video/Linux/v4lbuffer.cc
		src/video/Linux/v4lcontrol.cc
		src/video/Linux/videosource_v4lbuffer.cc)
	list(APPEND VIDEO_HEADERS
		cvd/Linux/v4lbuffer.h
		cvd/Linux/v4lcontrol.h)
else()
	list(APPEND VIDEO_SRCS src/video/Linux/videosource_nov4lbuffer.cc)
endif()

check_symbol_exists(uvc_stream_get_frame libuvc/libuvc.h CVD_HAVE_UVC)
if(CVD_HAVE_UVC)
	list(APPEND VIDEO_SRCS
		src/video/uvcbuffer.cc
		src/video/videosource_uvcbuffer.cc)
	list(APPEND VIDEO_HEADERS cvd/video/uvcbuffer.h)
else()
	list(APPEND VIDEO_SRCS
		src/video/videosource_nouvcbuffer.cc)
endif()


if(CVD_FFMPEG_FOUND)
	set(CVD_HAVE_FFMPEG ON)
	list(APPEND VIDEO_SRCS
		src/video/videofilebuffer2.cc
		src/video/videosource_videofilebuffer.cc)
else()
	set(CVD_HAVE_FFMPEG OFF)
	list(APPEND VIDEO_SRCS
		src/video/videosource_novideofilebuffer.cc)
endif()

if(CVD_TooN_FOUND)
	set(CVD_HAVE_TOON ON)
	list(APPEND SRCS
		src/base/brezenham.cc
		src/base/draw_toon.cc
		src/base/tensor_voting.cc
		src/base/threepointpose.cpp)
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
    list(APPEND IMAGEIO_SRCS src/image_io/jpeg.cxx)
else()
	set(CVD_HAVE_JPEG OFF)
	set(CVD_INTERNAL_JPEG_BUFFER_SIZE OFF)
endif()

if(TIFF_FOUND)
	set(CVD_HAVE_TIFF ON)
    list(APPEND IMAGEIO_SRCS
		src/image_io/tiff.cxx
		src/image_io/tiffwrite.cc)
else()
	set(CVD_HAVE_TIFF OFF)
endif()

if(PNG_FOUND)
	set(CVD_HAVE_PNG ON)
    list(APPEND IMAGEIO_SRCS src/image_io/png.cc)
else()
	set(CVD_HAVE_PNG OFF)
endif()

configure_file(cmake/config.h.in include/base/cvd/config.h)
configure_file(cmake/config_internal.h.in include/image_io/config_internal.h)

set(BASE_HEADERS_FULL)
foreach(file ${BASE_HEADERS})
	list(APPEND BASE_HEADERS_FULL include/base/${file})
endforeach()
add_library(CVD ${BASE_SRCS} ${BASE_HEADERS_FULL})
set_target_properties(CVD PROPERTIES OUTPUT_NAME cvd)
target_compile_features(CVD PUBLIC cxx_std_14)
target_include_directories(CVD PUBLIC "${PROJECT_SOURCE_DIR}/include/base" "${CMAKE_CURRENT_BINARY_DIR}/include/base" ${CVD_DEP_INCLUDES_PUBLIC} PRIVATE ${CVD_DEP_INCLUDES_PRIVATE})
target_link_libraries(CVD PRIVATE ${CVD_DEP_LIBS})

if(OpenGL_OpenGL_FOUND)
	set(GL_HEADERS_FULL)
	foreach(file ${GL_HEADERS})
		list(APPEND GL_HEADERS_FULL include/gl/${file})
	endforeach()
	add_library(CVD_GL ${GL_SRCS} ${GL_HEADERS_FULL})
	set_target_properties(CVD_GL PROPERTIES OUTPUT_NAME cvdgl)
	target_compile_features(CVD_GL PUBLIC cxx_std_14)
	target_include_directories(CVD_GL PUBLIC "${PROJECT_SOURCE_DIR}/include/gl" ${CVD_GL_DEP_INCLUDES_PUBLIC} PRIVATE ${CVD_GL_DEP_INCLUDES_PRIVATE})
	target_link_libraries(CVD_GL PUBLIC CVD PRIVATE ${CVD_GL_DEP_LIBS})
endif()

set(IMAGEIO_HEADERS_FULL)
foreach(file ${IMAGEIO_HEADERS})
	list(APPEND IMAGEIO_HEADERS_FULL include/image_io/${file})
endforeach()
add_library(CVD_ImageIO ${IMAGEIO_SRCS} ${IMAGEIO_HEADERS_FULL})
set_target_properties(CVD_ImageIO PROPERTIES OUTPUT_NAME cvdimageio)
target_compile_features(CVD_ImageIO PUBLIC cxx_std_14)
target_include_directories(CVD_ImageIO PUBLIC "${PROJECT_SOURCE_DIR}/include/image_io" "${CMAKE_CURRENT_BINARY_DIR}/include/image_io" ${CVD_IMAGEIO_DEP_INCLUDES_PUBLIC} PRIVATE ${CVD_IMAGEIO_DEP_INCLUDES_PRIVATE})
target_link_libraries(CVD_ImageIO PUBLIC CVD PRIVATE ${CVD_IMAGEIO_DEP_LIBS})

set(VIDEO_HEADERS_FULL)
foreach(file ${VIDEO_HEADERS})
	list(APPEND VIDEO_HEADERS_FULL include/video/${file})
endforeach()
add_library(CVD_Video ${VIDEO_SRCS} ${VIDEO_HEADERS_FULL})
set_target_properties(CVD_Video PROPERTIES OUTPUT_NAME cvdvideo)
target_compile_features(CVD_Video PUBLIC cxx_std_14)
target_include_directories(CVD_Video PUBLIC "${PROJECT_SOURCE_DIR}/include/video" ${CVD_VIDEO_DEP_INCLUDES_PUBLIC} PRIVATE ${CVD_VIDEO_DEP_INCLUDES_PRIVATE})
target_link_libraries(CVD_Video PUBLIC CVD CVD_ImageIO PRIVATE ${CVD_VIDEO_DEP_LIBS})


if(WIN32)
	target_compile_definitions(CVD PRIVATE _CRT_SECURE_NO_WARNINGS)
	if(OpenGL_OpenGL_FOUND)
		target_compile_definitions(CVD_GL PRIVATE _CRT_SECURE_NO_WARNINGS)
	endif()
	target_compile_definitions(CVD_ImageIO PRIVATE _CRT_SECURE_NO_WARNINGS)
	target_compile_definitions(CVD_Video PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

install(TARGETS CVD ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
foreach(file ${BASE_HEADERS})
	get_filename_component(dir ${file} DIRECTORY)
	install(FILES include/base/${file} DESTINATION include/${dir})
endforeach()
if(OpenGL_OpenGL_FOUND)
	install(TARGETS CVD_GL ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
	foreach(file ${GL_HEADERS})
		get_filename_component(dir ${file} DIRECTORY)
		install(FILES include/gl/${file} DESTINATION include/${dir})
	endforeach()
endif()
install(TARGETS CVD_ImageIO ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
foreach(file ${IMAGEIO_HEADERS})
	get_filename_component(dir ${file} DIRECTORY)
	install(FILES include/image_io/${file} DESTINATION include/${dir})
endforeach()
install(TARGETS CVD_Video ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
foreach(file ${VIDEO_HEADERS})
	get_filename_component(dir ${file} DIRECTORY)
	install(FILES include/video/${file} DESTINATION include/${dir})
endforeach()
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

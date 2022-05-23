project(tests)

add_executable(distance_transform_test distance_transform_test.cc)
target_link_libraries(distance_transform_test PRIVATE CVD)
add_test(NAME distance_transform_test COMMAND distance_transform_test)

add_executable(fast_corner_test fast_corner_test.cc)
target_link_libraries(fast_corner_test PRIVATE CVD)
add_test(NAME fast_corner_test COMMAND fast_corner_test)

add_executable(load_and_save load_and_save.cc)
if(WIN32)
	target_compile_options(load_and_save PRIVATE /bigobj)
endif()
target_link_libraries(load_and_save PRIVATE CVD)
add_test(NAME load_and_save COMMAND load_and_save)

add_executable(convolution convolution.cc)
target_link_libraries(convolution PRIVATE CVD)
add_test(NAME convolution COMMAND convolution)

add_executable(flips flips.cc)
target_link_libraries(flips PRIVATE CVD)
add_test(NAME flips COMMAND flips)

if(CVD_HAVE_FFMPEG)
	add_executable(videoreader_test videoreader_test.cc)
	target_link_libraries(videoreader_test PRIVATE CVD)
	add_test(NAME videoreader_test COMMAND videoreader_test ${CMAKE_CURRENT_LIST_DIR}/videoreader_test.mp4)
endif()

if(CVD_ENABLE_OPENCV_TESTS)
	# test only, carries no binary dependency
	find_package(OpenCV REQUIRED)
	find_package(Eigen3 REQUIRED)
	add_executable(opencv_test opencv_test.cc)
	target_link_libraries(opencv_test PRIVATE CVD Eigen3::Eigen ${OpenCV_LIBS})
	add_test(NAME opencv_test COMMAND opencv_test)
endif()

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

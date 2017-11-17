link_libraries(CVD)
project(tests)

link_libraries(CVD)

add_executable(distance_transform distance_transform.cc)
add_test(NAME distance_transform COMMAND distance_transform)

add_executable(fast_corner_test fast_corner_test.cc)
add_test(NAME fast_corner_test COMMAND fast_corner_test)

add_executable(load_and_save load_and_save.cc)
add_test(NAME load_and_save COMMAND load_and_save)

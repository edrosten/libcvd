project(examples)

# if(CVD_HAVE_TOON)
# 	add_executable(tensor_voting tensor_voting.cc)
#   target_link_libraries(tensor_voting PRIVATE CVD)
# endif()

add_executable(colourmaps colourmaps.cc)
target_link_libraries(colourmaps PRIVATE CVD)
add_executable(distance_transform distance_transform.cc)
target_link_libraries(distance_transform PRIVATE CVD)

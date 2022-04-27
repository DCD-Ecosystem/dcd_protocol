# ##################################################################################################
# Create the dcd-vm interpreter driver.
# ##################################################################################################
add_executable(dcd-vm-interp ${CMAKE_CURRENT_SOURCE_DIR}/interp.cpp)
target_link_libraries(dcd-vm-interp dcd-vm)

add_executable(bench-interp ${CMAKE_CURRENT_SOURCE_DIR}/bench_interp.cpp)
target_link_libraries(bench-interp dcd-vm)

add_executable(hello-driver ${CMAKE_CURRENT_SOURCE_DIR}/hello_driver.cpp)
target_link_libraries(hello-driver dcd-vm)

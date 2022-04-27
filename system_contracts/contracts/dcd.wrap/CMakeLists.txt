add_contract(dcd.wrap dcd.wrap ${CMAKE_CURRENT_SOURCE_DIR}/src/dcd.wrap.cpp)

target_include_directories(dcd.wrap
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(dcd.wrap
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/dcd.wrap.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/dcd.wrap.contracts.md @ONLY )

target_compile_options( dcd.wrap PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )

add_contract(dcd.boot dcd.boot ${CMAKE_CURRENT_SOURCE_DIR}/src/dcd.boot.cpp)

target_include_directories(dcd.boot
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(dcd.boot
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/dcd.boot.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/dcd.boot.contracts.md @ONLY )

target_compile_options( dcd.boot PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )



add_contract(dcd.token dcd.token ${CMAKE_CURRENT_SOURCE_DIR}/src/dcd.token.cpp)

target_include_directories(dcd.token
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(dcd.token
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/dcd.token.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/dcd.token.contracts.md @ONLY )

target_compile_options( dcd.token PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )

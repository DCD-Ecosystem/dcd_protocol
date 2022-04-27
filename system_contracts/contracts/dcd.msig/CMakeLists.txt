add_contract(dcd.msig dcd.msig ${CMAKE_CURRENT_SOURCE_DIR}/src/dcd.msig.cpp)

target_include_directories(dcd.msig
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(dcd.msig
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/dcd.msig.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/dcd.msig.contracts.md @ONLY )

target_compile_options( dcd.msig PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )

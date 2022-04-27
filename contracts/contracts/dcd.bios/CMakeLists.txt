if (DCD_COMPILE_TEST_CONTRACTS)
   add_contract(dcd.bios dcd.bios ${CMAKE_CURRENT_SOURCE_DIR}/src/dcd.bios.cpp)

   target_include_directories(dcd.bios
      PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include)

   set_target_properties(dcd.bios
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/dcd.bios.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/dcd.bios.contracts.md @ONLY )

   target_compile_options( dcd.bios PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
else()
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/bin/dcd.bios.abi ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY )
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/bin/dcd.bios.wasm ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY )
endif()

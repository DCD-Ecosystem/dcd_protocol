if( DCD_COMPILE_TEST_CONTRACTS )
   set(DCD_WASM_OLD_BEHAVIOR "Off")
   find_package( dcd.cdt REQUIRED )
endif()

add_subdirectory(dcd.bios)
add_subdirectory(dcd.boot)

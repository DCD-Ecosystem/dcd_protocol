if( DCD_COMPILE_CONTRACTS )

   include(ExternalProject)

   set(EOSIO_WASM_OLD_BEHAVIOR "Off")
   find_package(eosio.cdt REQUIRED)

   set(CMAKE_ARGS_VAL -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_TEST_CONTRACTS=${EOSIO_COMPILE_TEST_CONTRACTS} )
   if( USE_EOSIO_CDT_1_7_X)
      list(APPEND CMAKE_ARGS_VAL -DUSE_EOSIO_CDT_1_7_X=${USE_EOSIO_CDT_1_7_X})
   endif()
   ExternalProject_Add(
      bios_boot_contracts_project
      SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/contracts
      BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/system_contracts
      CMAKE_ARGS ${CMAKE_ARGS_VAL}
      UPDATE_COMMAND ""
      PATCH_COMMAND ""
      TEST_COMMAND ""
      INSTALL_COMMAND ""
      BUILD_ALWAYS 1
   )

endif()
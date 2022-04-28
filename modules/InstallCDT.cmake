add_custom_command( TARGET DcdClang POST_BUILD COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin )
macro( dcd_clang_install file )
   set(BINARY_DIR ${CMAKE_BINARY_DIR}/dcd_llvm/bin)
   add_custom_command( TARGET DcdClang POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file} ${CMAKE_BINARY_DIR}/bin/ )
   install(FILES ${BINARY_DIR}/${file}
      DESTINATION ${CDT_INSTALL_PREFIX}/bin
      PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endmacro( dcd_clang_install )

macro( dcd_clang_install_and_symlink file symlink )
   set(BINARY_DIR ${CMAKE_BINARY_DIR}/dcd_llvm/bin)
   add_custom_command( TARGET DcdClang POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file} ${CMAKE_BINARY_DIR}/bin/ )
   add_custom_command( TARGET DcdClang POST_BUILD COMMAND cd ${CMAKE_BINARY_DIR}/bin && ln -sf ${file} ${symlink} )
   install(FILES ${BINARY_DIR}/${file}
      DESTINATION ${CDT_INSTALL_PREFIX}/bin
      PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
   install(CODE "execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/bin)")
   install(CODE "execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink ${CDT_INSTALL_PREFIX}/bin/${file} ${CMAKE_INSTALL_PREFIX}/bin/${symlink})")
endmacro( dcd_clang_install_and_symlink )

macro( dcd_tool_install file )
   set(BINARY_DIR ${CMAKE_BINARY_DIR}/tools/bin)
   add_custom_command( TARGET DcdTools POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file} ${CMAKE_BINARY_DIR}/bin/ )
   install(FILES ${BINARY_DIR}/${file}
      DESTINATION ${CDT_INSTALL_PREFIX}/bin
      PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endmacro( dcd_tool_install )

macro( dcd_tool_install_and_symlink file symlink )
   set(BINARY_DIR ${CMAKE_BINARY_DIR}/tools/bin)
   add_custom_command( TARGET DcdTools POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/${file} ${CMAKE_BINARY_DIR}/bin/ )
   install(FILES ${BINARY_DIR}/${file}
      DESTINATION ${CDT_INSTALL_PREFIX}/bin
      PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
   install(CODE "execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/bin)")
   install(CODE "execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink ${CDT_INSTALL_PREFIX}/bin/${file} ${CMAKE_INSTALL_PREFIX}/bin/${symlink})")
endmacro( dcd_tool_install_and_symlink )

macro( dcd_cmake_install_and_symlink file symlink )
   set(BINARY_DIR ${CMAKE_BINARY_DIR}/modules)
   install(CODE "execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/lib/cmake/dcd.cdt)")
   install(CODE "execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink ${CDT_INSTALL_PREFIX}/lib/cmake/dcd.cdt/${file} ${CMAKE_INSTALL_PREFIX}/lib/cmake/dcd.cdt/${symlink})")
endmacro( dcd_cmake_install_and_symlink )

macro( dcd_libraries_install)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib)
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include)
   install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION ${CDT_INSTALL_PREFIX}/lib)
   install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION ${CDT_INSTALL_PREFIX}/include)
endmacro( dcd_libraries_install )

dcd_clang_install_and_symlink(llvm-ranlib dcd-ranlib)
dcd_clang_install_and_symlink(llvm-ar dcd-ar)
dcd_clang_install_and_symlink(llvm-nm dcd-nm)
dcd_clang_install_and_symlink(llvm-objcopy dcd-objcopy)
dcd_clang_install_and_symlink(llvm-objdump dcd-objdump)
dcd_clang_install_and_symlink(llvm-readobj dcd-readobj)
dcd_clang_install_and_symlink(llvm-readelf dcd-readelf)
dcd_clang_install_and_symlink(llvm-strip dcd-strip)

dcd_clang_install(opt)
dcd_clang_install(llc)
dcd_clang_install(lld)
dcd_clang_install(ld.lld)
dcd_clang_install(ld64.lld)
dcd_clang_install(clang-9)
dcd_clang_install(wasm-ld)

dcd_tool_install_and_symlink(dcd-pp dcd-pp)
dcd_tool_install_and_symlink(dcd-wast2wasm dcd-wast2wasm)
dcd_tool_install_and_symlink(dcd-wasm2wast dcd-wasm2wast)
dcd_tool_install_and_symlink(dcd-cc dcd-cc)
dcd_tool_install_and_symlink(dcd-cpp dcd-cpp)
dcd_tool_install_and_symlink(dcd-ld dcd-ld)
dcd_tool_install_and_symlink(dcd-abidiff dcd-abidiff)
dcd_tool_install_and_symlink(dcd-init dcd-init)

dcd_clang_install(../lib/LLVMDcdApply${CMAKE_SHARED_LIBRARY_SUFFIX})
dcd_clang_install(../lib/LLVMDcdSoftfloat${CMAKE_SHARED_LIBRARY_SUFFIX})

dcd_cmake_install_and_symlink(dcd.cdt-config.cmake dcd.cdt-config.cmake)
dcd_cmake_install_and_symlink(DcdWasmToolchain.cmake DcdWasmToolchain.cmake)
dcd_cmake_install_and_symlink(DcdCDTMacros.cmake DcdCDTMacros.cmake)

dcd_libraries_install()

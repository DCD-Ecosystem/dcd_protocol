# copyright defined in abidcd/LICENSE.txt

cmake_minimum_required (VERSION 3.8)
project(abidcd VERSION 0.1 LANGUAGES CXX C)

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

option(ABIDCD_NO_INT128 "disable use of __int128" OFF)
option(ABIDCD_ONLY_LIBRARY "define and build the ABIDCD library" OFF)

if(NOT DEFINED SKIP_SUBMODULE_CHECK)
  execute_process(COMMAND git submodule status --recursive
                  COMMAND grep -c "^[+\-]"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  OUTPUT_VARIABLE submodule_status
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(submodule_status GREATER 0)
     message(FATAL_ERROR "git submodules are not up to date.
Please run the command 'git submodule update --init --recursive'.")
  endif()
endif()

find_package(Threads)
include(GNUInstallDirs)

add_library(abidcd STATIC src/abi.cpp src/crypto.cpp include/dcd/fpconv.c)
target_include_directories(abidcd PUBLIC 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include" 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/external/rapidjson/include"
                          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(ABIDCD_NO_INT128)
target_compile_definitions(abidcd PUBLIC ABIDCD_NO_INT128)
endif()

add_library(abidcd_module MODULE src/abidcd.cpp src/abi.cpp src/crypto.cpp include/dcd/fpconv.c)
target_include_directories(abidcd_module PUBLIC 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include;" 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/external/rapidjson/include" 
                          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(abidcd_module ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(abidcd_module PROPERTIES OUTPUT_NAME "abidcd")

enable_testing()

add_executable(test_abidcd src/test.cpp src/abidcd.cpp src/ship.abi.cpp)
target_link_libraries(test_abidcd abidcd ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME test_abidcd COMMAND test_abidcd)

if(NOT ABIDCD_NO_INT128)
    add_executable(test_abidcd_template src/template_test.cpp src/abidcd.cpp)
    target_link_libraries(test_abidcd_template abidcd ${CMAKE_THREAD_LIBS_INIT})
    add_test(NAME test_abidcd_template COMMAND test_abidcd_template)
endif()

add_executable(test_abidcd_key src/key_test.cpp src/abidcd.cpp)
target_link_libraries(test_abidcd_key abidcd ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME test_abidcd_key COMMAND test_abidcd_key)

add_executable(test_abidcd_reflect src/reflect_test.cpp)
target_include_directories(test_abidcd_reflect PRIVATE include)
add_test(NAME test_abidcd_reflect COMMAND test_abidcd_reflect)

# Causes build issues on some platforms
# add_executable(test_abidcd_sanitize src/test.cpp src/abidcd.cpp src/abi.cpp src/crypto.cpp include/dcd/fpconv.c)
# target_include_directories(test_abidcd_sanitize PRIVATE include external/outcome/single-header external/rapidjson/include external/date/include)
# target_link_libraries(test_abidcd_sanitize abidcd -fno-omit-frame-pointer -fsanitize=address,undefined ${CMAKE_THREAD_LIBS_INIT})
# target_compile_options(test_abidcd_sanitize PUBLIC -fno-omit-frame-pointer -fsanitize=address,undefined)
# add_test(NAME test_abidcd_sanitize COMMAND test_abidcd_sanitize)

# add_executable(fuzzer src/fuzzer.cpp src/abidcd.cpp)
# target_include_directories(fuzzer PRIVATE external/outcome/single-header external/rapidjson/include external/date/include)
# target_link_libraries(fuzzer -fsanitize=fuzzer,address,undefined,signed-integer-overflow -fstandalone-debug ${CMAKE_THREAD_LIBS_INIT})
# target_compile_options(fuzzer PRIVATE -fsanitize=fuzzer,address,undefined,signed-integer-overflow -fstandalone-debug)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang|AppleClang)
    target_compile_options(abidcd PRIVATE -Wall -Wextra -Wno-unused-parameter -fcolor-diagnostics)
    target_compile_options(test_abidcd PRIVATE -Wall -Wextra -Wno-unused-parameter -fcolor-diagnostics)
endif()

if (NOT ABIDCD_ONLY_LIBRARY)
add_subdirectory(tools)
endif()


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/dcd DESTINATION include)

install(TARGETS abidcd
  EXPORT abidcd
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT abidcd
  DESTINATION "share/abidcd"
  FILE abidcd-targets.cmake
)

export(TARGETS abidcd 
       FILE ${CMAKE_CURRENT_BINARY_DIR}/share/abidcd/abidcd-targets.cmake)
configure_file(abidcd-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/share/abidcd/abidcd-config.cmake COPYONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/abidcd/abidcd-config.cmake
        DESTINATION "share/abidcd")
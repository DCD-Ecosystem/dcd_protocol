file(GLOB HEADERS *.hpp streams/*.hpp)
add_executable( ${RODDCD_EXECUTABLE_NAME}
  cloner_plugin.cpp
  streamer_plugin.cpp
  main.cpp
  rocksdb_plugin.cpp
  wasm_ql_http.cpp
  wasm_ql_plugin.cpp
  ${HEADERS}
)

if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../.git)
  find_package(Git)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../.."
      OUTPUT_VARIABLE "roddcd_BUILD_VERSION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Git commit revision: ${roddcd_BUILD_VERSION}")
  else()
    set(roddcd_BUILD_VERSION 0)
  endif()
else()
  set(roddcd_BUILD_VERSION 0)
endif()

configure_file(config.hpp.in config.hpp ESCAPE_QUOTES)

target_include_directories(${RODDCD_EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/abidcd/src
  ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/amqp-cpp/include)

target_link_libraries( ${RODDCD_EXECUTABLE_NAME}
        PRIVATE appbase version
        PRIVATE roddcd_lib fc amqpcpp ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS})

add_subdirectory(tests)

copy_bin( ${RODDCD_EXECUTABLE_NAME} )
install( TARGETS
   ${RODDCD_EXECUTABLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT base
)

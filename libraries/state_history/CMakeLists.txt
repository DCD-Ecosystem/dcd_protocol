file(GLOB HEADERS "include/dcd/state-history/*.hpp")

add_library( state_history
             abi.cpp
             create_deltas.cpp
             log.cpp
             transaction_trace_cache.cpp
             ${HEADERS}
           )

target_link_libraries( state_history 
                       PUBLIC dcd_chain fc chainbase softfloat
                     )

target_include_directories( state_history
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                          )

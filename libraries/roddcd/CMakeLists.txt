file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

add_library( roddcd_lib
             embedded_roddcd.cpp
             roddcd.cpp
             wasm_ql.cpp
             ${HEADERS}
           )

target_link_libraries( roddcd_lib 
                       PUBLIC abidcd chain_kv dcd_chain fc softfloat
                     )

target_include_directories( roddcd_lib
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                          )
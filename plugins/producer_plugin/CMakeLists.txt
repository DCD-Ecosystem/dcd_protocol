file(GLOB HEADERS "include/dcd/producer_plugin/*.hpp")

add_library( producer_plugin
             producer_plugin.cpp
             pending_snapshot.cpp
             ${HEADERS}
           )

target_link_libraries( producer_plugin chain_plugin signature_provider_plugin appbase dcd_chain )
target_include_directories( producer_plugin
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../chain_interface/include" )

add_subdirectory( test )

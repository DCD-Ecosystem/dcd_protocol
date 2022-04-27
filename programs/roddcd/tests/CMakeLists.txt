add_executable(test_roddcd_cli test_roddcd_cli.cpp)
target_link_libraries(test_roddcd_cli
    PRIVATE roddcd_lib fc amqpcpp ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS}
    PRIVATE Boost::unit_test_framework
)

add_test(NAME test_roddcd_cli
    COMMAND programs/roddcd/tests/test_roddcd_cli
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

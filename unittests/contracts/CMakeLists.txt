# will be implictly used for any compilation unit if not overrided by SYSTEM_INCLUDE_FOLDERS parameter
# these directories go as -isystem <dir> to avoid warnings from code of third-party libraries

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dcd.bios/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dcd.bios/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dcd.msig/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dcd.msig/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dcd.system/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dcd.system/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dcd.token/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dcd.token/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dcd.wrap/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dcd.wrap/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/old_versions/v1.6.0-rc3/dcd.bios/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/old_versions/v1.6.0-rc3/dcd.bios/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/old_versions/v1.7.0-develop-preactivate_feature/dcd.bios/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/old_versions/v1.7.0-develop-preactivate_feature/dcd.bios/)

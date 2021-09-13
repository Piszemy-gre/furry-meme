set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TESTS OFF CACHE BOOL "" FORCE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/globjects/deploy/CMakeLists.txt" "")

removeInFile(
    "${CMAKE_CURRENT_SOURCE_DIR}/globjects/source/globjects/CMakeLists.txt"
    [=[export(TARGETS ${target} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target}-export.cmake)]=]
    [=[install(EXPORT ${target}-export
    NAMESPACE   ${META_PROJECT_NAME}::
    DESTINATION ${INSTALL_CMAKE}/${target}
    COMPONENT   dev
)]=])

removeInFile(
    "${CMAKE_CURRENT_SOURCE_DIR}/globjects/cmake/CompileOptions.cmake"
    [=[/GL]=]
)

add_subdirectory(globjects)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/glbinding/deploy/CMakeLists.txt" "")

removeInFile(
    "${CMAKE_CURRENT_SOURCE_DIR}/glbinding/cmake/CompileOptions.cmake"
    [=[/GL]=]
)

add_subdirectory(glbinding)

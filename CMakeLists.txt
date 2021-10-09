cmake_minimum_required(VERSION 3.10)

# https://stackoverflow.com/a/44023223
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(my-open-world)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Since MSVC 19.29.30129 there is -std:c++20 and cmake uses it
    # However using this flag does not provide std::format implementation
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "/std:c++latest")
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "/std:c++latest")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/GS- /W4 /MT /EHa /utf-8 /MP)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>) # /MT switch
    #add_compile_options(/GL /c)
    #add_link_options(/LTCG)
    #if (PGO STREQUAL "GENPROFILE")
    #    add_link_options(/GENPROFILE)
    #elseif (PGO STREQUAL "USEPROFILE")
    #    add_link_options(/USEPROFILE:AGGRESSIVE)
    #endif()
endif ()

function(removeInFile fileToModify)
    file(READ ${fileToModify} content)
    foreach(X IN LISTS ARGN)
        string(REPLACE 
            ${X}
            "" 
            content ${content}
        )
    endforeach()
    file(WRITE ${fileToModify} ${content})
endfunction()

find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)
add_subdirectory(libs/glfw)
add_subdirectory(libs/glfwpp)
add_subdirectory(libs/glm)
add_subdirectory(libs/stb)
add_subdirectory(libs/glbinding)
add_subdirectory(libs/globjects)
add_subdirectory(libs/imgui)
add_subdirectory(libs/spdlog)

add_subdirectory(src)

#include(packages.cmake)
#include(CPack)
project(HelloWorld)
cmake_minimum_required(VERSION 3.0)

add_executable(my_open_world 
    main.cpp
    TextureArray.cpp
)

target_link_libraries(my_open_world
    OpenGL::GL
    OpenMP::OpenMP_CXX
    glfw
    glfwpp
    glm::glm
    glbinding
    glbinding-aux
    globjects
    stb
    imgui
    spdlog
)

add_custom_target(copyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:my_open_world>/shaders"
    DEPENDS my_open_world
)

add_custom_target(copyFont ALL
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/libs/imgui/imgui/misc/fonts/DroidSans.ttf" "$<TARGET_FILE_DIR:my_open_world>/fonts/DroidSans.ttf"
    DEPENDS my_open_world
)
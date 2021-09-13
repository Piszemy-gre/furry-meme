add_library(imgui INTERFACE)
set(S ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
target_sources(imgui INTERFACE 
    ${S}/imgui.cpp
    ${S}/imgui_draw.cpp
    ${S}/imgui_tables.cpp
    ${S}/imgui_widgets.cpp
    ${S}/imgui_widgets.cpp
    ${S}/imgui_demo.cpp
    ${S}/backends/imgui_impl_opengl3.cpp
    ${S}/backends/imgui_impl_glfw.cpp
)
target_include_directories(imgui INTERFACE ${S})
target_include_directories(imgui INTERFACE ${S}/backends)
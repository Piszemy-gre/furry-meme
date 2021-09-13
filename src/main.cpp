// cmake -S . -B build
// cmake --build build --config Release

#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/Meta.h>
#include <glbinding-aux/debug.h>
#include <glbinding-aux/types_to_string.h>

#include <globjects/AbstractUniform.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/globjects.h>

#define __GL_H__
#include <GLFW/glfw3.h>
#include <glfwpp/glfwpp.h>

#include <spdlog/spdlog.h>

#include "ImguiManager.h"
#include "TextureArray.h"

int main()
{
    try
    {
        auto library = glfw::init();

        glfw::WindowHints{
            .clientApi = glfw::ClientApi::OpenGl,
            .contextVersionMajor = 1,
            .contextVersionMinor = 0}
            .apply();

        glfw::Window window(800, 600, "My open world");
        glfw::makeContextCurrent(window);
        glfw::swapInterval(0);

        globjects::init(glfw::getProcAddress);

        glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue, {"glGetError"});
        glbinding::setAfterCallback([](const glbinding::FunctionCall &functionCall) {
            const auto error = gl::glGetError();
            if (error != gl::GL_NO_ERROR)
            {
                std::stringstream str;
                str << functionCall.function->name() << "(";
                for (size_t i = 0; i < functionCall.parameters.size(); i++)
                {
                    str << functionCall.parameters[i].get();
                    if (i + 1 < functionCall.parameters.size())
                        str << ", ";
                }
                str << ")";

                if (functionCall.returnValue)
                    str << " -> " << functionCall.returnValue.get();

                str << " error code: " << error;

                spdlog::error(str.str());
            }
        });

        spdlog::info("OpenGL Version:  {}", glbinding::aux::ContextInfo::version().toString());
        spdlog::info("OpenGL Vendor:   {}", glbinding::aux::ContextInfo::vendor());
        spdlog::info("OpenGL Renderer: {}", glbinding::aux::ContextInfo::renderer());

        window.keyEvent.setCallback([](glfw::Window &, glfw::KeyCode, int, glfw::KeyState, glfw::ModifierKeyBit) {
            std::cout << "key pressed";
        });

        ImguiManager imgui(window);

        TextureArray textureArray(16, 16, 3);
        textureArray.add("textures/top.png");
        textureArray.add("textures/side.jpg");
        textureArray.add("textures/bottom.jpg");
        textureArray.load();
        textureArray.bind(gl::GL_TEXTURE0);
        int textureId = 0;

        globjects::File vertexShaderSource("shaders/vert.glsl", false);
        globjects::File fragmentShaderSource("shaders/frag.glsl", false);

        auto vertexShader = globjects::Shader(gl::GL_VERTEX_SHADER, &vertexShaderSource);
        auto fragmentShader = globjects::Shader(gl::GL_FRAGMENT_SHADER, &fragmentShaderSource);

        vertexShader.compile();
        spdlog::info(vertexShader.infoLog());

        fragmentShader.compile();
        spdlog::info(fragmentShader.infoLog());

        globjects::Program program;
        program.attach(&vertexShader, &fragmentShader);
        program.use();
        spdlog::info(program.infoLog());

        std::array<float, 12> positions = {0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f};
        std::array<float, 12> texCoords = {1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

        globjects::VertexArray vao;
        globjects::Buffer positionsBuffer, texCoordsBuffer;
        positionsBuffer.setData(positions, gl::GL_STATIC_DRAW);
        texCoordsBuffer.setData(texCoords, gl::GL_STATIC_DRAW);

        auto binding0 = vao.binding(0);
        binding0->setAttribute(0);
        binding0->setBuffer(&positionsBuffer, 0, sizeof(glm::vec3));
        binding0->setFormat(3, gl::GL_FLOAT, gl::GL_FALSE, 0);
        vao.enable(0);

        auto binding1 = vao.binding(1);
        binding1->setAttribute(1);
        binding1->setBuffer(&texCoordsBuffer, 0, sizeof(glm::vec2));
        binding1->setFormat(2, gl::GL_FLOAT, gl::GL_FALSE, 0);
        vao.enable(1);

        globjects::Uniform<glm::mat4> P(&program, "P");
        auto [width, height] = window.getSize();
        P.set(glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f));
        globjects::Uniform<glm::mat4> V(&program, "V");
        V.set(glm::lookAt(glm::vec3(0, 1, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
        globjects::Uniform<int> textureIdUniform(&program, "textureId");
        textureIdUniform.set(textureId);

        window.framebufferSizeEvent.setCallback([&P](glfw::Window &window, int width, int height) {
            gl::glViewport(0, 0, width, height);
            P.set(glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f));
        });

        gl::glEnable(gl::GL_CULL_FACE);

        while (!window.shouldClose())
        {
            double time = glfw::getTime();
            gl::glClearColor((std::sin(time) + 1.0) / 2.0, (std::cos(time) + 1.0) / 2.0, (-std::sin(time) + 1.0) / 2.0, 0.0);
            gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

            vao.drawArrays(gl::GL_TRIANGLE_STRIP, 0, 4);

            imgui.render([&]() {
                ImGui::ShowDemoWindow();

                ImGuiIO &io = ImGui::GetIO();
                constexpr ImGuiWindowFlags window_flags =
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoNav |
                    ImGuiWindowFlags_NoMove;

                constexpr float padding = 10.0f;
                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImVec2 window_pos(viewport->WorkPos.x + viewport->WorkSize.x - padding, viewport->WorkPos.y + padding);
                ImVec2 window_pos_pivot(1.0f, 0.0f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                ImGui::SetNextWindowBgAlpha(0.35f);
                if (ImGui::Begin("Fps counter", nullptr, window_flags))
                    ImGui::Text("FPS: %.1f", io.Framerate);
                ImGui::End();

                if (ImGui::Begin("Texture"))
                {
                    if (ImGui::SliderInt("Texture", &textureId, 0, textureArray.size() - 1))
                        textureIdUniform.set(textureId);
                }
                ImGui::End();
            });

            glfw::pollEvents();
            window.swapBuffers();
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error(e.what());
    }
}
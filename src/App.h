#ifndef SRC_APP_H
#define SRC_APP_H

#include <array>
#include <iostream>
#include <sstream>
#include <tuple>


#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/Meta.h>
#include <glbinding-aux/debug.h>
#include <glbinding-aux/types_to_string.h>

#define __GL_H__
#include <GLFW/glfw3.h>
#include <glfwpp/glfwpp.h>

#include <spdlog/spdlog.h>

#include "ImguiManager.h"
#include "Renderer.h"

class App
{
public:
    App()
        : library(glfw::init()),
          windowHints{
              .clientApi = glfw::ClientApi::OpenGl,
              .contextVersionMajor = 1,
              .contextVersionMinor = 0},
          applyWindowHints((windowHints.apply(), 0)),
          window(defaultWidth, defaultHeight, "My open world"),
          glfwMakeContextCurrent((glfw::makeContextCurrent(window), 0)),
          globjectsInit((globjects::init(glfw::getProcAddress), 0)),
          imgui(window),
          renderer(defaultWidth, defaultHeight)
    {
        glfw::swapInterval(0);

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

        window.keyEvent.setCallback([this](glfw::Window &, glfw::KeyCode keyCode, int scanCode, glfw::KeyState keyState, glfw::ModifierKeyBit modifierKeyBit) {
            renderer.onKeyEvent(keyCode, scanCode, keyState, modifierKeyBit);
        });

        window.cursorPosEvent.setCallback([this](glfw::Window &, double xPos, double yPos) {
            static float oldXPos = xPos;
            static float oldYPos = yPos;
            renderer.onCursorMoveEvent(xPos - oldXPos, yPos - oldYPos);
            oldXPos = xPos;
            oldYPos = yPos;
        });

        window.framebufferSizeEvent.setCallback([this](glfw::Window &window, int width, int height) {
            renderer.onFramebufferSizeEvent(width, height);
        });

        gl::glEnable(gl::GL_DEPTH_TEST);
        //gl::glEnable(gl::GL_CULL_FACE); vertices need to be sorted clockwise or counter-clockwise
        gl::glLineWidth(2.f); // make wireframes easier to see
    }

    void run()
    {

        std::array positions = {
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f};
        std::array texCoords = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f};

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

        double lastTime = glfw::getTime();
        double time;
        double delta;

        while (!window.shouldClose())
        {
            lastTime = time;
            time = glfw::getTime();
            delta = time - lastTime;

            gl::glClearColor((std::sin(time) + 1.0) / 2.0, (std::cos(time) + 1.0) / 2.0, (-std::sin(time) + 1.0) / 2.0, 0.0);
            gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT | gl::ClearBufferMask::GL_DEPTH_BUFFER_BIT);

            renderer.update(delta);

            vao.drawArrays(gl::GL_TRIANGLES, 0, positions.size() / 3);

            imgui.render([this]() {
                ImGui::ShowDemoWindow();

                drawFpsWindow();
            });

            glfw::pollEvents();
            window.swapBuffers();
        }
    }

private:
    void drawFpsWindow() const
    {
        static ImGuiIO &io = ImGui::GetIO();
        constexpr ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove;

        constexpr float padding = 10.0f;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        const ImVec2 window_pos(viewport->WorkPos.x + viewport->WorkSize.x - padding, viewport->WorkPos.y + padding);
        const ImVec2 window_pos_pivot(1.0f, 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("Fps counter", nullptr, window_flags))
            ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::End();
    }

    static constexpr int defaultWidth = 800;
    static constexpr int defaultHeight = 600;

    struct helper
    {
        helper(int) {}
    };

    glfw::GlfwLibrary library;
    glfw::WindowHints windowHints;
    helper applyWindowHints;
    glfw::Window window;
    helper glfwMakeContextCurrent, globjectsInit;
    ImguiManager imgui;
    Renderer renderer;
};

#endif /* SRC_APP_H */

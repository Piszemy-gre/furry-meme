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
#include "Chunk.h"

#include <GLFW/glfw3.h>
#include <glfwpp/glfwpp.h>

#include <spdlog/spdlog.h>

#include "ImguiManager.h"
#include "Renderer.h"
#include "World.h"

struct BuffersAggregator
{
    globjects::VertexArray vao;
    globjects::Buffer positionsBuffer, texCoordsBuffer;
    size_t verticesCount{0};
};

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
        window.setInputModeCursor(glfw::CursorMode::Disabled);
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
            onKeyEvent(keyCode, scanCode, keyState, modifierKeyBit);
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

    void buildWorld()
    {
        for (int x = 0; x <= 1; x++)
            for (int y = 0; y <= 0; y++)
                for (int z = 0; z <= 0; z++)
                    world.getChunk(ChunkPosition(x, y, z)).fillAllButCorner(1);
        //world.getChunk(ChunkPosition(1, 2, 1)).fillAllButCorner(1);
    }

    void run()
    {
        buildWorld();

        constructChunksVertices();

        for (auto &[_, b] : buffers)
        {
            auto &[vao, positionsBuffer, texCoordsBuffer, verticesCount] = b;
            vao.bind();

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
        }

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

            for (auto &[_, b] : buffers)
                b.vao.drawArrays(gl::GL_TRIANGLES, 0, b.verticesCount);

            imgui.render([this]() {
                ImGui::ShowDemoWindow();

                drawFpsWindow();
            });

            glfw::pollEvents();
            window.swapBuffers();
        }
    }

private:
    void constructChunksVertices()
    {
        for (const auto &[position, chunk] : world.chunks())
            constructChunkVertices(position, *chunk);
    }
    void constructChunkVertices(ChunkPosition position, const Chunk &chunk)
    {
        positions = chunk.constructVertices(world, strategy_);
        auto &buffer = buffers[position];
        buffer.positionsBuffer.setData(positions, gl::GL_STATIC_DRAW);
        buffer.texCoordsBuffer.setData(positions, gl::GL_STATIC_DRAW);
        buffer.verticesCount = positions.size() / 3;
    }

    void onKeyEvent(glfw::KeyCode keyCode, int scanCode, glfw::KeyState keyState, glfw::ModifierKeyBit modifierKeyBit)
    {
        if (keyCode == glfw::KeyCode::K && keyState == glfw::KeyState::Press)
        {
            strategy_ = strategy_ == VerticesConstructStrategy::naive ? VerticesConstructStrategy::greedy : VerticesConstructStrategy::naive;
            constructChunksVertices();
        }

        if (keyCode == glfw::KeyCode::J && keyState == glfw::KeyState::Press)
        {
            (gl::glIsEnabled(gl::GL_CULL_FACE) == gl::GL_TRUE ? gl::glDisable : gl::glEnable)(gl::GL_CULL_FACE);
        }
    }

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

    globjects::Buffer positionsBuffer, texCoordsBuffer;

    std::unordered_map<ChunkPosition, BuffersAggregator> buffers;
    World world;
    std::vector<float> positions;
    VerticesConstructStrategy strategy_{VerticesConstructStrategy::naive};
};

#endif /* SRC_APP_H */

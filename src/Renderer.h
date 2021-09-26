#ifndef SRC_GRAPHICS_RENDERER_H
#define SRC_GRAPHICS_RENDERER_H

#include <globjects/AbstractUniform.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/File.h>
#include <globjects/globjects.h>

#include <glm/glm.hpp>

#include <glfwpp/glfwpp.h>

#include <spdlog/spdlog.h>

#include "Camera.h"

class Renderer;

class Drawable
{
public:
    virtual void draw(const Renderer &window) const = 0;
};

class Renderer
{
public:
    Renderer(float width, float height)
        : vertexShaderSource("shaders/vert.glsl", false),
          fragmentShaderSource("shaders/frag.glsl", false),
          vertexShader(gl::GL_VERTEX_SHADER, &vertexShaderSource),
          fragmentShader(gl::GL_FRAGMENT_SHADER, &fragmentShaderSource),
          P(&program, "P"),
          V(&program, "V"),
          textureIdUniform(&program, "textureId")
    {
        if (!vertexShader.compile())
            spdlog::error(vertexShader.infoLog());

        if (!fragmentShader.compile())
            spdlog::error(fragmentShader.infoLog());

        program.attach(&vertexShader, &fragmentShader);
        program.link();

        if (!program.isLinked())
            spdlog::error(program.infoLog());

        program.use();

        P.set(glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f));

        textureIdUniform.set(0);
    }

    void onKeyEvent(glfw::KeyCode keyCode, int scanCode, glfw::KeyState keyState, glfw::ModifierKeyBit modifierKeyBit)
    {
        camera.onKeyEvent(keyCode, scanCode, keyState, modifierKeyBit);

        if (keyCode == glfw::KeyCode::L && keyState == glfw::KeyState::Press)
        {
            drawWireframe = !drawWireframe;
            if (drawWireframe)
            {
                gl::glPolygonMode(gl::GL_FRONT, gl::GL_LINE);
                gl::glPolygonMode(gl::GL_BACK, gl::GL_LINE);
            }
            else
            {
                gl::glPolygonMode(gl::GL_FRONT, gl::GL_FILL);
                gl::glPolygonMode(gl::GL_BACK, gl::GL_FILL);
            }
        }
    }

    void onScrollEvent(float xOffset, float yOffset)
    {
        camera.onScrollEvent(xOffset, yOffset);
    }

    void onCursorMoveEvent(float xOffset, float yOffset)
    {
        camera.onCursorMoveEvent(xOffset, yOffset);
    }

    void update(float delta)
    {
        camera.update(delta);
        V.set(camera.getV());
    }

    void onFramebufferSizeEvent(int width, int height)
    {
        if (width == 0 || height == 0)
            return;
        gl::glViewport(0, 0, width, height);
        P.set(glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f));
    }

private:
    globjects::File vertexShaderSource;
    globjects::File fragmentShaderSource;
    globjects::Shader vertexShader;
    globjects::Shader fragmentShader;
    globjects::Program program;

    globjects::Uniform<glm::mat4> P;
    globjects::Uniform<glm::mat4> V;
    globjects::Uniform<int> textureIdUniform;

    bool drawWireframe = false;

    Camera camera;
};
#endif /* SRC_GRAPHICS_RENDERER_H */

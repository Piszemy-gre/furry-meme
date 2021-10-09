#ifndef SRC_GRAPHICS_RENDERER_H
#define SRC_GRAPHICS_RENDERER_H

#include <array>

#include <globjects/AbstractUniform.h>
#include <globjects/Renderbuffer.h>
#include <globjects/TextureHandle.h>

#include <glm/glm.hpp>

#include <glfwpp/glfwpp.h>

#include "Camera.h"
#include "Drawable.h"
#include "ShaderProgram.h"

class Renderer
{
public:
    Renderer(float width, float height)
        : windowWidth(width),
          windowHeight(height),
          deferredShader("shaders/vert.glsl", "shaders/frag.glsl"),
          mainShader("shaders/mainVert.glsl", "shaders/mainFrag.glsl"),
          aaShader("shaders/fxaaVert.glsl", "shaders/fxaaFrag.glsl"),
          P(&deferredShader.getProgram(), "P"),
          V(&deferredShader.getProgram(), "V"),
          screenSize(&aaShader.getProgram(), "screenSize"),
          gPosition(gl::GL_TEXTURE_2D),
          gNormal(gl::GL_TEXTURE_2D),
          gTexCord(gl::GL_TEXTURE_2D),
          gTexId(gl::GL_TEXTURE_2D),
            mainColor(gl::GL_TEXTURE_2D),
            aaColor(gl::GL_TEXTURE_2D)
    {
        onFramebufferSizeEvent(windowWidth, windowHeight);

        for (auto texture : {&gPosition, &gNormal, &gTexCord, &gTexId, &mainColor, &aaColor})
        {
            texture->setParameter(gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
            texture->setParameter(gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
            texture->setParameter(gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
            texture->setParameter(gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
        }

        gBuffer.attachTexture(gl::GL_COLOR_ATTACHMENT0, &gPosition);
        gBuffer.attachTexture(gl::GL_COLOR_ATTACHMENT1, &gNormal);
        gBuffer.attachTexture(gl::GL_COLOR_ATTACHMENT2, &gTexCord);
        gBuffer.attachTexture(gl::GL_COLOR_ATTACHMENT3, &gTexId);
        gBuffer.attachRenderBuffer(gl::GL_DEPTH_ATTACHMENT, &gDepth);
        gBuffer.setDrawBuffers({gl::GL_COLOR_ATTACHMENT0, gl::GL_COLOR_ATTACHMENT1, gl::GL_COLOR_ATTACHMENT2, gl::GL_COLOR_ATTACHMENT3, gl::GL_NONE});
        if (gBuffer.checkStatus() != gl::GL_FRAMEBUFFER_COMPLETE)
            spdlog::error(gBuffer.statusString());

        mainBuffer.attachTexture(gl::GL_COLOR_ATTACHMENT0, &mainColor);
        mainBuffer.setDrawBuffers({gl::GL_COLOR_ATTACHMENT0});
        if (mainBuffer.checkStatus() != gl::GL_FRAMEBUFFER_COMPLETE)
            spdlog::error(mainBuffer.statusString());

        aaBuffer.attachTexture(gl::GL_COLOR_ATTACHMENT0, &aaColor);
        aaBuffer.setDrawBuffers({gl::GL_COLOR_ATTACHMENT0});
        if (aaBuffer.checkStatus() != gl::GL_FRAMEBUFFER_COMPLETE)
            spdlog::error(aaBuffer.statusString());

        globjects::Framebuffer::defaultFBO()->bind();

        std::array quadBufferData = {
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

        quadBuffer.setData(quadBufferData, gl::GL_STATIC_DRAW);

        auto binding0 = quadVao.binding(0);
        binding0->setAttribute(0);
        binding0->setBuffer(&quadBuffer, 0, sizeof(glm::vec3) + sizeof(glm::vec2));
        binding0->setFormat(3, gl::GL_FLOAT, gl::GL_FALSE, 0);
        quadVao.enable(0);

        auto binding1 = quadVao.binding(1);
        binding1->setAttribute(1);
        binding1->setBuffer(&quadBuffer, sizeof(glm::vec3), sizeof(glm::vec3) + sizeof(glm::vec2));
        binding1->setFormat(2, gl::GL_FLOAT, gl::GL_FALSE, 0);
        quadVao.enable(1);

        mainShader.getProgram().setUniform("gPosition", 0);
        mainShader.getProgram().setUniform("gNormal", 1);
        mainShader.getProgram().setUniform("gTexCord", 2);
        mainShader.getProgram().setUniform("gTexId", 3);

        aaShader.getProgram().setUniform("aaColor", 0);
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

        windowWidth = width;
        windowHeight = height;
        glm::ivec2 windowSize = {windowWidth, windowHeight};

        gl::glViewport(0, 0, windowWidth, windowHeight);
        P.set(glm::perspective(glm::radians(45.0f), float(windowWidth) / float(windowHeight), 0.1f, 100.0f));
        screenSize.set(windowSize);

        gPosition.image2D(0, gl::GL_RGB16F, windowSize, 0, gl::GL_RGB, gl::GL_FLOAT, nullptr);
        gNormal.image2D(0, gl::GL_RGB16F, windowSize, 0, gl::GL_RGB, gl::GL_FLOAT, nullptr);
        gTexCord.image2D(0, gl::GL_RG16F, windowSize, 0, gl::GL_RG, gl::GL_FLOAT, nullptr);
        gTexId.image2D(0, gl::GL_R16UI, windowSize, 0, gl::GL_RED_INTEGER, gl::GL_UNSIGNED_SHORT, nullptr);
        gDepth.storage(gl::GL_DEPTH_COMPONENT, windowSize.x, windowSize.y);

        mainColor.image2D(0, gl::GL_RGBA16F, windowSize, 0, gl::GL_RGBA, gl::GL_FLOAT, nullptr);
        aaColor.image2D(0, gl::GL_RGBA16F, windowSize, 0, gl::GL_RGBA, gl::GL_FLOAT, nullptr);
    }

    void onGui()
    {
        const ImVec2 uv0(0, 1);
        const ImVec2 uv1(1, 0);

        if (debugTextureFullscreen)
        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
        }
        else
        {
            ImGui::SetNextWindowContentSize(ImVec2(windowWidth / 2.f, 0.0f));
        }

        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("G Buffer", nullptr, debugTextureFullscreen ? flags : ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Checkbox("Fullscreen", &debugTextureFullscreen);

        ImGui::Columns(2);
        const float imageWidth = ImGui::GetColumnWidth() - 16;
        const ImVec2 imageSize(imageWidth, windowHeight * imageWidth / windowWidth);

        ImGui::Image((void *)(intptr_t)gPosition.id(), imageSize, uv0, uv1);
        ImGui::Separator();
        ImGui::Image((void *)(intptr_t)gNormal.id(), imageSize, uv0, uv1);
        ImGui::NextColumn();
        ImGui::Image((void *)(intptr_t)gTexCord.id(), imageSize, uv0, uv1);
        ImGui::Separator();
        //ImGui::Image((void *)(intptr_t)gTexId.id(), imageSize, uv0, uv1);

        ImGui::End();
    }

    void beginDraw()
    {
        gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT | gl::ClearBufferMask::GL_DEPTH_BUFFER_BIT);
        gBuffer.clear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT | gl::ClearBufferMask::GL_DEPTH_BUFFER_BIT);
        aaBuffer.clear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT | gl::ClearBufferMask::GL_DEPTH_BUFFER_BIT);
        deferredShader.getProgram().use();
        gBuffer.bind();
    }

    void draw(const Drawable &drawable) const
    {
        drawable.draw(*this);
    }

    void endDraw()
    {
        
        //gBuffer.blit(gl::GL_COLOR_ATTACHMENT0, {{0, 0, windowWidth, windowHeight}}, globjects::Framebuffer::defaultFBO().get(),
        //             gl::GL_BACK, {{0, 0, windowWidth / 2, windowHeight / 2}}, gl::GL_COLOR_BUFFER_BIT, gl::GL_NEAREST);
        //gBuffer.blit(gl::GL_COLOR_ATTACHMENT1, {{0, 0, windowWidth, windowHeight}}, globjects::Framebuffer::defaultFBO().get(),
        //             gl::GL_BACK, {{windowWidth / 2, 0, windowWidth, windowHeight / 2}}, gl::GL_COLOR_BUFFER_BIT, gl::GL_NEAREST);
        //gBuffer.blit(gl::GL_COLOR_ATTACHMENT2, {{0, 0, windowWidth, windowHeight}}, globjects::Framebuffer::defaultFBO().get(),
        //             gl::GL_BACK, {{0, windowHeight / 2, windowWidth / 2, windowHeight}}, gl::GL_COLOR_BUFFER_BIT, gl::GL_NEAREST);
        //gBuffer.blit(gl::GL_COLOR_ATTACHMENT3, {{0, 0, windowWidth, windowHeight}}, globjects::Framebuffer::defaultFBO().get(),
        //             gl::GL_BACK_LEFT, {{windowWidth/2, windowHeight/2, windowWidth, windowHeight}}, gl::GL_COLOR_BUFFER_BIT, gl::GL_NEAREST);

        //gBuffer.blit(gl::GL_COLOR_ATTACHMENT1, {{0, 0, windowWidth, windowHeight}}, globjects::Framebuffer::defaultFBO().get(),
        //             gl::GL_BACK_LEFT, {{0, 0, windowWidth, windowHeight}}, gl::GL_COLOR_BUFFER_BIT, gl::GL_NEAREST);

        //globjects::Framebuffer::defaultFBO()->bind();

        gPosition.bindActive(gl::GL_TEXTURE0);
        gNormal.bindActive(gl::GL_TEXTURE1);
        gTexCord.bindActive(gl::GL_TEXTURE2);
        gTexId.bindActive(gl::GL_TEXTURE3);

        aaBuffer.bind();
        mainShader.getProgram().use();
        quadVao.drawArrays(gl::GL_TRIANGLE_STRIP, 0, 4);

        aaColor.bindActive(gl::GL_TEXTURE0);

        globjects::Framebuffer::defaultFBO()->bind();
        aaShader.getProgram().use();
        quadVao.drawArrays(gl::GL_TRIANGLE_STRIP, 0, 4);
    }

private:
    ShaderProgram deferredShader, mainShader, aaShader;

    globjects::Uniform<glm::mat4> P;
    globjects::Uniform<glm::mat4> V;
    globjects::Uniform<glm::ivec2> screenSize;

    globjects::Texture dummy;

    globjects::Framebuffer gBuffer;
    globjects::Texture gPosition, gNormal, gTexCord, gTexId;
    globjects::Renderbuffer gDepth;

    globjects::Framebuffer mainBuffer;
    globjects::Texture mainColor;

    globjects::Framebuffer aaBuffer;
    globjects::Texture aaColor;
 
    globjects::VertexArray quadVao;
    globjects::Buffer quadBuffer;

    Camera camera;

    int windowWidth, windowHeight;

    float debugTextureScale = 1.f;
    bool debugTextureFullscreen = false;
    bool drawWireframe = false;
};
#endif /* SRC_GRAPHICS_RENDERER_H */

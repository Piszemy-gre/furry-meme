// cmake -S . -B build
// cmake --build build --config Release

#include "TextureArray.h"

#include "App.h"

int main()
{
    try
    {
        App app;

        TextureArray textureArray(16, 16, 3);
        textureArray.add("textures/top.png");
        textureArray.add("textures/side.jpg");
        textureArray.add("textures/bottom.jpg");
        textureArray.load();
        textureArray.bind(gl::GL_TEXTURE0);

        app.run();
    }
    catch (const std::exception &e)
    {
        spdlog::error(e.what());
    }
}
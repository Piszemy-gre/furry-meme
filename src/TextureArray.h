#ifndef SRC_TEXTUREARRAY_H
#define SRC_TEXTUREARRAY_H

#include <globjects/Texture.h>
#include <memory>

class TextureArray
{
public:
    TextureArray(int width, int height, uint8_t channels);
    int add(const std::string &filename);
    int add(const char *filename);
    void load();
    void bind(gl::GLenum texture);
    int size() const { return _counter; }

private:
    const int _width;
    const int _height;
    const uint8_t _channels;
    int _counter = 0;
    std::unique_ptr<globjects::Texture> _texture;
    std::vector<unsigned char> _data;
};

#endif /* SRC_TEXTUREARRAY_H */

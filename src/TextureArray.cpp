#include "TextureArray.h"

#include <stdexcept>

#include <glbinding-aux/ContextInfo.h>
#include <glbinding/gl/gl.h>

#include <stb_image.h>
#include <stb_image_resize.h>

TextureArray::TextureArray(int width, int height, uint8_t channels)
    : _width(width), _height(height), _channels(channels)
{
    _data.reserve(_width * _height * _channels * 16);
    stbi_set_flip_vertically_on_load(true);
}

int TextureArray::add(const std::string &filename) { return add(filename.c_str()); }

int TextureArray::add(const char *filename)
{
    auto size = _data.size();
    _data.resize(size + _width * _height * _channels);

    int w;
    int h;
    int c;
    unsigned char *image = stbi_load(filename, &w, &h, &c, _channels);
    stbir_resize_uint8(image, w, h, 0, &_data[size], _width, _height, 0, _channels);
    stbi_image_free(image);

    return _counter++;
}

void TextureArray::load()
{
    _texture = globjects::Texture::create(gl::GL_TEXTURE_2D_ARRAY);

    _texture->setParameter(gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
    _texture->setParameter(gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
    _texture->setParameter(gl::GL_TEXTURE_WRAP_R, gl::GL_CLAMP_TO_EDGE);

    _texture->setParameter(gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR_MIPMAP_LINEAR);
    _texture->setParameter(gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);

    gl::GLenum internalFormat, format;
    switch (_channels)
    {
    case 1:
        internalFormat = gl::GL_R8;
        format = gl::GL_RED;
        break;
    case 2:
        internalFormat = gl::GL_RG8;
        format = gl::GL_RG;
        break;
    case 3:
        internalFormat = gl::GL_RGB8;
        format = gl::GL_RGB;
        break;
    case 4:
        internalFormat = gl::GL_RGBA8;
        format = gl::GL_RGBA;
        break;
    default:
        throw std::runtime_error("unsupported number of channels");
        break;
    }

    _texture->storage3D(1, internalFormat, _width, _height, _counter);
    _texture->subImage3D(0, 0, 0, 0, _width, _height, _counter, format, gl::GL_UNSIGNED_BYTE, &_data[0]);

    _texture->generateMipmap();

    static bool ARB_texture_filter_anisotropic = glbinding::aux::ContextInfo::supported({gl::GLextension::GL_ARB_texture_filter_anisotropic});
    static bool EXT_texture_filter_anisotropic = glbinding::aux::ContextInfo::supported({gl::GLextension::GL_EXT_texture_filter_anisotropic});

    if (ARB_texture_filter_anisotropic)
    {
        float maxAnisotropy;
        gl::glGetFloatv(gl::GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
        _texture->setParameter(gl::GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);
    }
    else if (EXT_texture_filter_anisotropic)
    {
        float maxAnisotropy;
        gl::glGetFloatv(gl::GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        _texture->setParameter(gl::GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }
}

void TextureArray::bind(gl::GLenum texture)
{
    if (_texture)
        _texture->bindActive(texture);
    else
        throw std::runtime_error("_texture was nullptr");
}
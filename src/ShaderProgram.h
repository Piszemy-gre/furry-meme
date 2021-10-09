#ifndef SRC_SHADERPROGRAM_H
#define SRC_SHADERPROGRAM_H

#include <string>

#include <glbinding/gl/gl.h>

#include <globjects/base/File.h>
#include <globjects/globjects.h>

#include <spdlog/spdlog.h>

class ShaderProgram
{

public:
    ShaderProgram(const std::string &vertexShaderSourceFilename, const std::string &fragmentShaderSourceFilename)
        : vertexShaderSource(vertexShaderSourceFilename, false),
          fragmentShaderSource(fragmentShaderSourceFilename, false),
          vertexShader(gl::GL_VERTEX_SHADER, &vertexShaderSource),
          fragmentShader(gl::GL_FRAGMENT_SHADER, &fragmentShaderSource)
    {

        if (!vertexShader.compile())
            spdlog::error(vertexShader.infoLog());

        if (!fragmentShader.compile())
            spdlog::error(fragmentShader.infoLog());

        program.attach(&vertexShader, &fragmentShader);
        program.link();

        if (!program.isLinked())
            spdlog::error(program.infoLog());
    }

    globjects::Program &getProgram() { return program; }
    const globjects::Program &getProgram() const { return program; }

private:
    globjects::File vertexShaderSource;
    globjects::File fragmentShaderSource;
    globjects::Shader vertexShader;
    globjects::Shader fragmentShader;
    globjects::Program program;
};

#endif /* SRC_SHADERPROGRAM_H */

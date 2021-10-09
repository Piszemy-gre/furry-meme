#pragma once

#include "Drawable.h"
#include "Mesh.h"
#include "glbinding/gl/boolean.h"
#include "glbinding/gl/enum.h"
#include "globjects/Buffer.h"
#include "globjects/VertexArray.h"
#include "globjects/VertexAttributeBinding.h"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


class BasicDrawable : public Drawable
{
public:
    BasicDrawable() = default;

    explicit BasicDrawable(const Mesh &mesh) : BasicDrawable(mesh.vertices, mesh.textures, mesh.texture_indexes){}

    BasicDrawable(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& texCoords, const std::vector<int>& texIds)
    {
        update(vertices, texCoords, texIds);
    }

    void update(const Mesh &mesh) { update(mesh.vertices, mesh.textures, mesh.texture_indexes); }

    void update(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2> &texCoords, const std::vector<int> &texIds)
    {
        verticesCount = static_cast<decltype(verticesCount)>(vertices.size());

        positionsBuffer.setData(vertices, gl::GL_DYNAMIC_DRAW);
        texCoordsBuffer.setData(texCoords, gl::GL_DYNAMIC_DRAW);
        texIdsBuffer.setData(texIds, gl::GL_DYNAMIC_DRAW);

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

        auto binding2 = vao.binding(2);
        binding2->setAttribute(2);
        binding2->setBuffer(&texIdsBuffer, 0, sizeof(int));
        binding2->setIFormat(1, gl::GL_INT, 0);
        vao.enable(2);
    }

    void draw(const Renderer &window) const override
    {
        vao.drawArrays(gl::GL_TRIANGLES, 0, verticesCount);
    }

private:
    globjects::VertexArray vao;
    globjects::Buffer positionsBuffer, texCoordsBuffer, texIdsBuffer;
    gl::GLsizei verticesCount{0};
};

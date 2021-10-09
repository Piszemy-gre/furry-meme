#ifndef SRC_DUMMYBLOCKS_H
#define SRC_DUMMYBLOCKS_H

#include <algorithm>
#include <vector>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <globjects/VertexAttributeBinding.h>
#include <globjects/globjects.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

#include "Drawable.h"

inline glm::vec3 ivecToVec(const glm::ivec3 &vec)
{
    return {vec.x, vec.y, vec.z};
}

class DummyBlocks : public Drawable
{
public:
    DummyBlocks()
    {

        std::vector blockVert = {
            glm::vec3(0.5f, -0.5f, -0.5),
            glm::vec3(-0.5f, -0.5f, -0.5),
            glm::vec3(0.5f, 0.5f, -0.5),
            glm::vec3(-0.5f, 0.5f, -0.5),
            glm::vec3(0.5f, 0.5f, -0.5),
            glm::vec3(-0.5f, -0.5f, -0.5),

            glm::vec3(-0.5f, -0.5f, 0.5),
            glm::vec3(0.5f, -0.5f, 0.5),
            glm::vec3(0.5f, 0.5f, 0.5),
            glm::vec3(0.5f, 0.5f, 0.5),
            glm::vec3(-0.5f, 0.5f, 0.5),
            glm::vec3(-0.5f, -0.5f, 0.5),

            glm::vec3(-0.5f, 0.5f, 0.5),
            glm::vec3(-0.5f, 0.5f, -0.5),
            glm::vec3(-0.5f, -0.5f, -0.5),
            glm::vec3(-0.5f, -0.5f, -0.5),
            glm::vec3(-0.5f, -0.5f, 0.5),
            glm::vec3(-0.5f, 0.5f, 0.5),

            glm::vec3(0.5f, 0.5f, -0.5),
            glm::vec3(0.5f, 0.5f, 0.5),
            glm::vec3(0.5f, -0.5f, -0.5),
            glm::vec3(0.5f, -0.5f, 0.5),
            glm::vec3(0.5f, -0.5f, -0.5),
            glm::vec3(0.5f, 0.5f, 0.5),

            glm::vec3(-0.5f, -0.5f, -0.5),
            glm::vec3(0.5f, -0.5f, -0.5),
            glm::vec3(0.5f, -0.5f, 0.5),
            glm::vec3(0.5f, -0.5f, 0.5),
            glm::vec3(-0.5f, -0.5f, 0.5),
            glm::vec3(-0.5f, -0.5f, -0.5),

            glm::vec3(0.5f, 0.5f, -0.5),
            glm::vec3(-0.5f, 0.5f, -0.5),
            glm::vec3(0.5f, 0.5f, 0.5),
            glm::vec3(-0.5f, 0.5f, 0.5),
            glm::vec3(0.5f, 0.5f, 0.5),
            glm::vec3(-0.5f, 0.5f, -0.5f)};

        std::vector<glm::vec2> blockTexCoords = {
            {1.0f, 0.0f},
            {0.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {1.0f, 1.0f},
            {0.0f, 0.0f},

            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},

            {0.0f, 1.0f},
            {1.0f, 1.0f},
            {1.0f, 0.0f},
            {1.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 1.0f},

            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {1.0f, 0.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {0.0f, 1.0f},

            {0.0f, 1.0f},
            {1.0f, 1.0f},
            {1.0f, 0.0f},
            {1.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 1.0f},

            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {1.0f, 0.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {0.0f, 1.0f}};

        std::vector blockTexIds = {
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, // bottom
            0, 0, 0, 0, 0, 0, // top
        };

        std::vector<glm::vec3> chunkVert;
        std::vector<glm::vec2> chunkTexCoords;
        std::vector<int> chunkTexIds;

        constexpr size_t blockCount = 10000;
        chunkVert.reserve(blockCount * blockVert.size());
        chunkTexCoords.reserve(blockCount * chunkTexCoords.size());

        for (size_t i = 0; i < blockCount; i++)
        {
            chunkVert.insert(chunkVert.end(), blockVert.begin(), blockVert.end());
            chunkTexCoords.insert(chunkTexCoords.end(), blockTexCoords.begin(), blockTexCoords.end());
            chunkTexIds.insert(chunkTexIds.end(), blockTexIds.begin(), blockTexIds.end());
            glm::vec3 V = (glm::ivec3)glm::gaussRand(glm::vec3(0, 0, 0), glm::vec3(2, 2, 2));
            for (size_t i = chunkVert.size() - blockVert.size(); i < chunkVert.size(); i++)
                chunkVert[i] += V;
        }

        verticesCount = chunkVert.size();

        positionsBuffer.setData(chunkVert, gl::GL_DYNAMIC_DRAW);
        texCoordsBuffer.setData(chunkTexCoords, gl::GL_DYNAMIC_DRAW);
        texIdsBuffer.setData(chunkTexIds, gl::GL_DYNAMIC_DRAW);

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
    size_t verticesCount;
};

#endif /* SRC_DUMMYBLOCKS_H */

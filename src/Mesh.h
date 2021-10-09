#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Mesh
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textures;
    std::vector<int> texture_indexes;
};

#include "Chunk.h"
#include "World.h"
#include "voxel_utility.h"
#include <array>
#include <format>

Chunk::Chunk(const ChunkPosition position) : position_(position)
{}


Mesh Chunk::createMesh(const World &world, VerticesConstructStrategy strategy) const
{
    switch (strategy)
    {
    case (VerticesConstructStrategy::naive):
        return createMeshNaive(world);
    case VerticesConstructStrategy::greedy:
        return createMeshGreedy(world);
    default:
        return createMeshNaive(world);
    }
}

BasicDrawable Chunk::createDrawable(const World &world, VerticesConstructStrategy strategy) const
{
    switch (strategy)
    {
    case (VerticesConstructStrategy::naive):
        return createDrawableNaive(world);
    case VerticesConstructStrategy::greedy:
        return createDrawableGreedy(world);
    default:
        return createDrawableNaive(world);
    }
}

BasicDrawable Chunk::createDrawableNaive(const World &world) const
{
    const auto da = createMeshNaive(world);
    return BasicDrawable{da.vertices, da.textures, da.texture_indexes};
}

BasicDrawable Chunk::createDrawableGreedy(const World &world) const
{
    const auto da = createMeshGreedy(world);
    return BasicDrawable{da.vertices, da.textures, da.texture_indexes};
}


Block Chunk::getBlock(size_t x, size_t y, size_t z) const
{
    return blocks_[getBlockIdx(x, y, z)];
}

Block Chunk::getBlock(glm::vec<3, size_t> position) const
{
    return getBlock(position.x, position.y, position.z);
}

void Chunk::setBlock(const size_t x, const size_t y, const size_t z, const Block b)
{
    blocks_[getBlockIdx(x, y, z)] = b;
}

void Chunk::fillWith(const Block block)
{
    std::fill(blocks_.begin(), blocks_.end(), block);
}

void Chunk::fillAllButCorner(const Block b)
{
    for (size_t x = 0; x < chunk_size; x++)
        for (size_t y = 0; y < chunk_size; y++)
            for (size_t z = 0; z < chunk_size; z++)
                setBlock(x, y, z, x * 2 < chunk_size || y * 2 < chunk_size || z * 2 < chunk_size ? b : 0);
}

void Chunk::fillAllButCorners(const Block b)
{
    for (size_t x = 0; x < chunk_size; x++)
        for (size_t y = 0; y < chunk_size; y++)
            for (size_t z = 0; z < chunk_size; z++)
                setBlock(x, y, z, (x <= chunk_size / 4 || x >= 3 * chunk_size / 4) && (y <= chunk_size / 4 || y >= 3 * chunk_size / 4) && (z <= chunk_size / 4 || z >= 3 * chunk_size / 4) ? b : 0);
}

size_t Chunk::getBlockIdx(const size_t x, const size_t y, const size_t z)
{
    return x + y * (1 << chunk_size_exp) + z * (1 << 2 * chunk_size_exp);
}


constexpr std::array faces =
    {
    //bottom
        std::array{
            glm::vec2{0.0f, 1.0f},
            glm::vec2{1.0f, 1.0f},
            glm::vec2{1.0f, 0.0f},
            glm::vec2{0.0f, 0.0f},
        },
    //top
        std::array{
            glm::vec2{1.0f, 1.0f},
            glm::vec2{0.0f, 1.0f},
            glm::vec2{0.0f, 0.0f},
            glm::vec2{1.0f, 0.0f},
        },
    //front
        std::array{
            glm::vec2{0, 0},
            glm::vec2{1, 0},
            glm::vec2{1, 1},
            glm::vec2{0, 1},
        },
    //back
        std::array{
            glm::vec2{0, 0},
            glm::vec2{1, 0},
            glm::vec2{1, 1},
            glm::vec2{0, 1},
        },
    //left
        std::array{
            glm::vec2{0, 0},
            glm::vec2{1, 0},
            glm::vec2{1, 1},
            glm::vec2{0, 1},
        },
    //right
        std::array{
            glm::vec2{0, 0},
            glm::vec2{1, 0},
            glm::vec2{1, 1},
            glm::vec2{0, 1},
        }
    };

constexpr auto &bottom_face = faces[0];
constexpr auto &top_face = faces[1];
constexpr auto &side_face = faces[2];

constexpr std::array faces_texture_idx =
    {
        std::array{2, 2, 2, 2},
        std::array{0, 0, 0, 0},
        std::array{1, 1, 1, 1},
        std::array{1, 1, 1, 1},
        std::array{1, 1, 1, 1},
        std::array{1, 1, 1, 1},
    };

constexpr auto &bottom_faces_texture_idx = faces_texture_idx[0];
constexpr auto &top_faces_texture_idx = faces_texture_idx[1];
constexpr auto &side_faces_texture_idx = faces_texture_idx[2];

Mesh Chunk::createMeshNaive(const World &world) const
{
    Mesh aggregator;
    auto &[vertices, textures, texture_indices] = aggregator;
    auto vertices_iter = std::back_inserter(vertices);
    auto textures_iter = std::back_inserter(textures);
    auto texture_indices_iter = std::back_inserter(texture_indices);

    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z)
                if (getBlock(x, y, z) != 0)
                {
                    vertices_iter = voxel_utility::insertCube(vertices_iter, glm::vec3{x, y, z}, 1);
                    textures_iter = voxel_utility::insertCubeTextures(textures_iter, faces);
                    texture_indices_iter = voxel_utility::insertCubeTextures(texture_indices_iter, faces_texture_idx);
                }

    voxel_utility::translateVertices(begin(vertices), end(vertices), position_ * static_cast<long long>(chunk_size));
    return aggregator;
}

Mesh Chunk::createMeshGreedy(const World &world) const
{
    //https://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
    //https://gist.github.com/Vercidium/a3002bd083cce2bc854c9ff8f0118d33

    Mesh aggregator;
    auto &[vertices, textures, texture_indices] = aggregator;
    auto vertices_iter = std::back_inserter(vertices);
    auto textures_iter = std::back_inserter(textures);
    auto texture_indices_iter = std::back_inserter(texture_indices);

    constexpr int ichunk_size = static_cast<int>(chunk_size);

    for (int d = 0; d < 3; d++)
    {
        //sweep across each direction X, Y, Z
        const auto u = (d + 1) % 3;
        const auto v = (d + 2) % 3;
        glm::ivec3 x{0, 0, 0};
        glm::ivec3 q{0, 0, 0};

        std::vector mask(chunk_size * chunk_size, false);
        std::vector flag(chunk_size * chunk_size, false);
        std::vector<glm::ivec3> normals(chunk_size * chunk_size);

        q[d] = 1;

        for (x[d] = -1; x[d] < ichunk_size;)
        {
            size_t n = 0;
            for (x[v] = 0; x[v] < ichunk_size; ++x[v])
            {
                for (x[u] = 0; x[u] < ichunk_size; ++x[u])
                {

                    auto bp = BlockPosition(x[0], x[1], x[2]);
                    auto wp = position_ + bp;
                    //is block on this position
                    const auto b1 = x[d] >= 0 ? world.isSolidBlockAt(wp) : false;

                    //is there a block in the direction we are searching.
                    wp.addBlockPosition({q[0], q[1], q[2]});
                    const auto b2 = x[d] < ichunk_size - 1 ? world.isSolidBlockAt(wp) : false;
                    flag[n] = b1 && !b2;
                    normals[n] = flag[n] ? q : -q;
                    mask[n++] = b1 != b2; //is there's a face between blocks. There is no face visible if both blocks are air or the two blocks are solid
                }
            }

            ++x[d];
            n = 0;
            for (int j = 0; j < ichunk_size; ++j)
            {
                for (int i = 0; i < ichunk_size;)
                {
                    if (mask[n])
                    {
                        int w;
                        for (w = 1; w + i < ichunk_size && mask[n + w]; ++w)
                        {}

                        int h;
                        [&]() {
                            for (h = 1; h + j < ichunk_size; ++h)
                            {
                                for (int k = 0; k < w; ++k)
                                {
                                    if (!mask[n + k + h * ichunk_size])
                                        return;
                                }
                            }
                        }();

                        x[u] = i;
                        x[v] = j;

                        glm::ivec3 du{0, 0, 0}, dv{0, 0, 0};
                        du[u] = w;
                        dv[v] = h;



                        const auto v1 = x, v2 = x + du, v3 = x + du + dv, v4 = x + dv;
                        if (flag[n])
                        {
                            vertices_iter = voxel_utility::insertQuad(vertices_iter,
                                                                      v1,
                                                                      v2,
                                                                      v3,
                                                                      v4);
                        }
                        else
                        {
                            vertices_iter = voxel_utility::insertQuad(vertices_iter,
                                                                      v1,
                                                                    v4,
                                                                      v3,
                                                                    v2);
                        }
                            
                        const int faces_idx = normals[n].y == 1 ? 1 : normals[n].y == -1 ? 0 : 2;
                        if (normals[n].x == 1 || normals[n].z == -1)
                            textures_iter = voxel_utility::insertQuad(textures_iter,
                                                                      faces[faces_idx][1],
                                                                      faces[faces_idx][2],
                                                                      faces[faces_idx][3],
                                                                      faces[faces_idx][0]);
                        else textures_iter = voxel_utility::insertQuad(textures_iter,
                                                                       faces[faces_idx][0],
                                                                       faces[faces_idx][1],
                                                                       faces[faces_idx][2],
                                                                       faces[faces_idx][3]);

                        texture_indices_iter = voxel_utility::insertQuad(texture_indices_iter,
                                                                 faces_texture_idx[faces_idx][0],
                                                                 faces_texture_idx[faces_idx][1],
                                                                 faces_texture_idx[faces_idx][2],
                                                                 faces_texture_idx[faces_idx][3]);
                        
                        for (int l = 0; l < h; ++l)
                            for (int k = 0; k < w; ++k)
                                mask[n + k + l * chunk_size] = false;
                        i += w;
                        n += w;
                    }
                    else
                    {
                        ++i;
                        ++n;
                    }
                }
            }
        }
    }
    voxel_utility::translateVertices(begin(vertices), end(vertices), position_ * static_cast<long long>(chunk_size));

    return aggregator;
}

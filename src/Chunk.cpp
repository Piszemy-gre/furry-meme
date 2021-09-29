#include "Chunk.h"
#include <array>
#include <format>
#include <iostream>

#include "voxel_utility.h"

std::vector<float> Chunk::constructVertices(VerticesConstructStrategy strategy) const
{
    switch (strategy)
    {
    case (VerticesConstructStrategy::naive):
        return constructVerticesNaive();
    case VerticesConstructStrategy::greedy:
        return constructVerticesGreedy();
    default:
        return constructVerticesNaive();
    }
}

Chunk::Block Chunk::getBlock(const int x, const int y, const int z) const
{
    if (x < 0 || x >= static_cast<int>(chunk_size) || y < 0 || y >= static_cast<int>(chunk_size) || z < 0 || z >= static_cast<int>(chunk_size))
        return 0;
    return blocks_[getBlockIdx(x, y, z)];
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

size_t Chunk::getBlockIdx(const size_t x, const size_t y, const size_t z)
{
    return x + y * (1 << chunk_size_exp) + z * (1 << 2 * chunk_size_exp);
}

std::vector<float> Chunk::constructVerticesNaive() const
{
    std::vector<float> vertices;
    auto iter = std::back_inserter(vertices);
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z)
                if (getBlock(x, y, z) != 0)
                    iter = voxel_utility::insertCube(iter, x, y, z, 1);
    return vertices;
}

std::vector<float> Chunk::constructVerticesGreedy() const
{
    //https://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
    //https://gist.github.com/Vercidium/a3002bd083cce2bc854c9ff8f0118d33

    std::vector<float> vertices{};
    auto vertices_iter = std::back_inserter(vertices);

    for (size_t d = 0; d < 3; d++)
    {
        //sweep across each direction X, Y, Z
        const auto u = (d + 1) % 3;
        const auto v = (d + 2) % 3;
        std::array x{0, 0, 0};
        std::array q{0, 0, 0};

        std::vector mask(chunk_size * chunk_size, false);
        q[d] = 1;

        for (x[d] = -1; x[d] < static_cast<int>(chunk_size);)
        {
            size_t n = 0;

            for (x[v] = 0; x[v] < static_cast<int>(chunk_size); ++x[v])
            {
                for (x[u] = 0; x[u] < static_cast<int>(chunk_size); ++x[u])
                {
                    //is block on this position
                    const auto b1 = x[d] >= 0 ? getBlock(x[0], x[1], x[2]) != 0 : true;
                    //is there a block in the direction we are searching. (assume there are no block outside this chunk)
                    const auto b2 = x[d] < chunk_size - 1 ? getBlock(x[0] + q[0], x[1] + q[1], x[2] + q[2]) != 0 : false;

                    mask[n++] = b1 != b2; //is there a face between blocks. There is no face visible if both blocks are air or the two blocks are solid
                }
            }

            ++x[d];
            n = 0;

            for (size_t j = 0; j < chunk_size; ++j)
            {
                for (size_t i = 0; i < chunk_size;)
                {
                    if (mask[n])
                    {
                        int w;
                        for (w = 1; w + i < chunk_size && mask[n + w]; ++w)
                        {}

                        int h;
                        [&]() {
                            for (h = 1; h + j < chunk_size; ++h)
                            {
                                for (int k = 0; k < w; ++k)
                                {
                                    if (!mask[n + k + h * chunk_size])
                                        return;
                                }
                            }
                        }();

                        x[u] = i;
                        x[v] = j;

                        std::array du{0, 0, 0}, dv{0, 0, 0};
                        du[u] = w;
                        dv[v] = h;

                        vertices_iter = voxel_utility::insertQuad(vertices_iter,
                                                                  x[0], x[1], x[2],
                                                                  x[0] + du[0], x[1] + du[1], x[2] + du[2],
                                                                  x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2],
                                                                  x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]);
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
    return vertices;
}

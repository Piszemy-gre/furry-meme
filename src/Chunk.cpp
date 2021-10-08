#include "Chunk.h"
#include "World.h"
#include "voxel_utility.h"
#include <array>
#include <format>



Chunk::Chunk(const ChunkPosition position) : position_(position)
{}

std::vector<float> Chunk::constructVertices(const World &world, VerticesConstructStrategy strategy) const
{
    switch (strategy)
    {
    case (VerticesConstructStrategy::naive):
        return constructVerticesNaive(world);
    case VerticesConstructStrategy::greedy:
        return constructVerticesGreedy(world);
    default:
        return constructVerticesNaive(world);
    }
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

std::vector<float> Chunk::constructVerticesNaive(const World &world) const
{
    std::vector<float> vertices;
    auto iter = std::back_inserter(vertices);
    for (size_t x = 0; x < chunk_size; ++x)
        for (size_t y = 0; y < chunk_size; ++y)
            for (size_t z = 0; z < chunk_size; ++z)
                if (getBlock(x, y, z) != 0)
                    iter = voxel_utility::insertCube(iter, x, y, z, 1);

    voxel_utility::translateVertices(begin(vertices), end(vertices), position_.x * chunk_size, position_.y * chunk_size, position_.z * chunk_size);
    return vertices;
}

std::vector<float> Chunk::constructVerticesGreedy(const World &world) const
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
        std::vector flag(chunk_size * chunk_size, false);

        q[d] = 1;

        for (x[d] = -1; x[d] < static_cast<int>(chunk_size);)
        {
            size_t n = 0;
            for (x[v] = 0; x[v] < static_cast<int>(chunk_size); ++x[v])
            {
                for (x[u] = 0; x[u] < static_cast<int>(chunk_size); ++x[u])
                {

                    auto bp = BlockPosition(x[0], x[1], x[2]);
                    auto wp = position_ + bp;
                    //is block on this position
                    const auto b1 = x[d] >= 0 ? world.isSolidBlockAt(wp) : false;

                    //is there a block in the direction we are searching.
                    wp.addBlockPosition({q[0], q[1], q[2]});
                    const auto b2 = x[d] < static_cast<int>(chunk_size) - 1 ? world.isSolidBlockAt(wp) : false;
                    flag[n] = b1 && !b2;
                    mask[n++] = b1 != b2; //is there's a face between blocks. There is no face visible if both blocks are air or the two blocks are solid
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

                        if (flag[n])
                            vertices_iter = voxel_utility::insertQuad(vertices_iter,
                                                                  x[0], x[1], x[2],
                                                                  x[0] + du[0], x[1] + du[1], x[2] + du[2],
                                                                  x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2],
                                                                  x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]);
                        else
                            vertices_iter = voxel_utility::insertQuad(vertices_iter,
                                                                      x[0], x[1], x[2],
                                                                      x[0] + dv[0], x[1] + dv[1], x[2] + dv[2],
                                                                      x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2],
                                                                      x[0] + du[0], x[1] + du[1], x[2] + du[2]);
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
    voxel_utility::translateVertices(begin(vertices), end(vertices), position_.x * chunk_size, position_.y * chunk_size, position_.z * chunk_size);
    return vertices;
}

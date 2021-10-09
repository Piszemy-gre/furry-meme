#pragma once

#include "BasicDrawable.h"
#include "Mesh.h"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/detail/type_vec3.hpp>

enum class VerticesConstructStrategy
{
    naive,
    greedy
};

using Block = size_t;

class World;
using BlockPosition = glm::vec<3, size_t>;
using ChunkPosition = glm::vec<3, long long>;


struct WorldPosition
{
    ChunkPosition chunk_position;
    BlockPosition block_position;

    void addChunkPosition(const ChunkPosition &chunk_position) { this->chunk_position = this->chunk_position + chunk_position; }
    void addBlockPosition(glm::vec<3, size_t> position);
};

inline WorldPosition operator+(const ChunkPosition& lhs, const BlockPosition& rhs)
{
   return {.chunk_position = lhs, .block_position = rhs};
}

namespace std
{
    template <>
    struct hash<ChunkPosition>
    {
        std::size_t operator()(const ChunkPosition &cp) const noexcept
        {
            constexpr auto hash = std::hash<decltype(cp.x)>();
            return hash(cp.x) ^ hash(cp.y) ^ hash(cp.z);
        }
    };
}

class Chunk
{
public:
    constexpr inline static size_t chunk_size_exp = 5;
    constexpr inline static size_t chunk_size = 1 << chunk_size_exp;

    explicit Chunk(ChunkPosition position);

    [[nodiscard]] BasicDrawable createDrawable(const World &, VerticesConstructStrategy = VerticesConstructStrategy::greedy) const;
    [[nodiscard]] BasicDrawable createDrawableNaive(const World &) const;
    [[nodiscard]] BasicDrawable createDrawableGreedy(const World &) const;

    [[nodiscard]] Mesh createMesh(const World &, VerticesConstructStrategy) const;
    [[nodiscard]] Mesh createMeshNaive(const World &) const;
    [[nodiscard]] Mesh createMeshGreedy(const World &) const;

    [[nodiscard]] Block getBlock(size_t x, size_t y, size_t z) const;
    [[nodiscard]] Block getBlock(glm::vec<3, size_t> position) const;

    void setBlock(size_t x, size_t y, size_t z, Block b);

    void fillWith(Block);
    void fillAllButCorner(Block);
    void fillAllButCorners(Block);

private:



    static size_t getBlockIdx(size_t x, size_t y, size_t z);
    std::vector<Block> blocks_{std::vector<Block>(1 << 3 * chunk_size_exp)};
    const ChunkPosition position_;
};


inline void WorldPosition::addBlockPosition(glm::vec<3, size_t> position)
{
    block_position += position;
    auto change_dim = [](auto &chunk, auto block) {
        if (block >= 0)
        {
            auto d = block / Chunk::chunk_size;
            block -= d * Chunk::chunk_size;
            chunk += d;
        }
        else
        {
            block = -(block - 1 - 16);
            auto d = block / Chunk::chunk_size;
            chunk -= d;
            block -= d * Chunk::chunk_size;
        }
    };
    change_dim(chunk_position.x, block_position.x);
    change_dim(chunk_position.y, block_position.y);
    change_dim(chunk_position.z, block_position.z);
}
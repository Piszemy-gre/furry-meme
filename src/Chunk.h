#pragma once
#include <vector>

enum class VerticesConstructStrategy
{
    naive,
    greedy
};

using Block = size_t;

class World;

struct ChunkPosition
{
    unsigned long long x, y, z;

    friend bool operator==(const ChunkPosition &lhs, const ChunkPosition &rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    friend bool operator!=(const ChunkPosition &lhs, const ChunkPosition &rhs)
    {
        return !(lhs == rhs);
    }
};

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
    constexpr inline static size_t chunk_size_exp = 3;
    constexpr inline static size_t chunk_size = 1 << chunk_size_exp;

    explicit Chunk(ChunkPosition position);

    [[nodiscard]] std::vector<float> constructVertices(const World &, VerticesConstructStrategy = VerticesConstructStrategy::greedy) const;
    [[nodiscard]] std::vector<float> constructVerticesNaive(const World &) const;
    [[nodiscard]] std::vector<float> constructVerticesGreedy(const World &) const;

    [[nodiscard]] Block getBlock(size_t x, size_t y, size_t z) const;

    void setBlock(size_t x, size_t y, size_t z, Block b);

    void fillWith(Block);
    void fillAllButCorner(Block);

private:
    static size_t getBlockIdx(size_t x, size_t y, size_t z);
    std::vector<Block> blocks_{std::vector<Block>(1 << 3 * chunk_size_exp)};
    const ChunkPosition position_;
};

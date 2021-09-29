#pragma once
#include <vector>

enum class VerticesConstructStrategy
{
    naive,
    greedy
};



class Chunk
{
public:
    using Block = size_t;
    constexpr inline static size_t chunk_size_exp = 6;
    constexpr inline static size_t chunk_size= 1 << chunk_size_exp;
    

    [[nodiscard]] std::vector<float> constructVertices(VerticesConstructStrategy = VerticesConstructStrategy::greedy) const;
    [[nodiscard]] std::vector<float> constructVerticesNaive() const;
    [[nodiscard]] std::vector<float> constructVerticesGreedy() const;

    [[nodiscard]] Block getBlock(int x, int y, int z) const;

    void setBlock(size_t x, size_t y, size_t z, Block b);

    void fillWith(Block);
    void fillAllButCorner(Block);

private:
    static size_t getBlockIdx(size_t x, size_t y, size_t z);
    std::vector<Block> blocks_{std::vector<Block>(1 << 3 * chunk_size_exp)};
};


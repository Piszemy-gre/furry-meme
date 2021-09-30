#pragma once
#include "Chunk.h"

#include <memory>
#include <unordered_map>

struct WorldPosition
{
    unsigned long long x, y, z;
};

class World
{
    using ChunksContainer = std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>>;
public:
    Chunk &getChunk(ChunkPosition position)
    {
        return *chunks_.try_emplace(position, new Chunk(position)).first->second;
    }

    [[nodiscard]] bool isSolidBlockAt(WorldPosition position) const
    {
        return !isAirBlockAt(position);
    }

    [[nodiscard]] bool isAirBlockAt(WorldPosition position) const
    {
        constexpr auto chunk_size = static_cast<decltype(position.x)>(Chunk::chunk_size);
        const ChunkPosition chunk_position{position.x / chunk_size, position.y / chunk_size, position.z / chunk_size};
        if (const auto chunk = chunks_.find(chunk_position); chunk != chunks_.end())
            return chunk->second->getBlock(
                       position.x - chunk_position.x * chunk_size,
                       position.y - chunk_position.y * chunk_size,
                       position.z - chunk_position.z * chunk_size) == 0;
        return true;
    }

    [[nodiscard]] ChunksContainer & chunks()
    {
        return chunks_;
    }


private:
    ChunksContainer chunks_;
};

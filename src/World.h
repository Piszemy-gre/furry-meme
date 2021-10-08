#pragma once
#include "Chunk.h"

#include <memory>
#include <unordered_map>

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
        if (const auto chunk = chunks_.find(position.chunk_position); chunk != chunks_.end())
            return chunk->second->getBlock(position.block_position) == 0;
        return true;
    }

    [[nodiscard]] ChunksContainer & chunks()
    {
        return chunks_;
    }


private:
    ChunksContainer chunks_;
};

//
// Created by lixinzhuang1 on 2024/6/5.
//

#ifndef BE_CHUNK_H
#define BE_CHUNK_H
#include <memory>

namespace starrocks {
    namespace vectorized {
        class Chunk {
        public:

            Chunk();
        };
        using ChunkPtr = std::shared_ptr<Chunk>;
    }
}

#endif//BE_CHUNK_H

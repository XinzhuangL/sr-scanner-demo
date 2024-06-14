//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_CHUNK_SOURCE_H
#define BE_CHUNK_SOURCE_H

#include "common/status.h"
#include "common/chunk.h"
#include "common/buffer.h"

namespace starrocks {
    namespace pipeline {
        class ScanOperator;

        class ChunkSource {
        public:
            ChunkSource(ScanOperator* p, Buffer& chunk_buffer);
            virtual ~ChunkSource() = default;
            virtual Status prepare();
            virtual void close() = 0;
            // Return true if eos is not reached
            // Return false if eos is reached or error occurred
            bool has_next_chunk() const { return _status.ok(); }
            Status buffer_next_batch_chunks_blocking(size_t batch_size);
        protected:
            // MUST be implemented by different ChunkSource
            virtual Status _read_chunk(vectorized::ChunkPtr* chunk) = 0;
            ScanOperator* _scan_op;
            Buffer& _chunk_buffer;
            Status _status = Status::OK();

        };
        using ChunkSourcePtr = std::shared_ptr<ChunkSource>;
    }
}

#endif//BE_CHUNK_SOURCE_H

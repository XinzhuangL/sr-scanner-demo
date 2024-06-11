//
// Created by lixinzhuang1 on 2024/6/5.
//
#include "chunk_source.h"

namespace starrocks::pipeline {

    ChunkSource::ChunkSource(starrocks::pipeline::ScanOperator *scan_op, starrocks::Buffer &chunk_buffer)
    : _scan_op(scan_op),
    _chunk_buffer(chunk_buffer)
    {}

    Status ChunkSource::prepare() {
        // do nothing
        return Status::OK();
    }

    Status ChunkSource::buffer_next_batch_chunks_blocking(size_t batch_size) {
        using namespace vectorized;
        // return if status is not ok
        if (!_status.ok()) {
            return _status;
        }

        // todo need an additional condition '!state->is_cancelled()'
        for (size_t i = 0; i < batch_size; i++) {
            ChunkPtr chunk;
            _status = _read_chunk(&chunk);
            // we always output an empty chunk instead of nullptr, because we need set tablet_id
            // and is_last_chunk flag in the chunk

            if (chunk == nullptr) {
                chunk = std::make_shared<vectorized::Chunk>();
            }

            if (!_status.ok()) {
                // todo end of file

                // todo time out

                break;
            }

            // todo set to chunk buffer

        }

    }


}

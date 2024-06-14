//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_CHUNK_H
#define BE_CHUNK_H
#include <memory>
#include <vector>
#include "column/column.h"

namespace starrocks {
    namespace vectorized {

        using ColumnPtr = std::shared_ptr<Column>;
        using Columns = std::vector<ColumnPtr>;
        class Chunk {
        public:

            Chunk();
            size_t num_rows() const { return _columns.empty() ? 0 : _columns[0]-> size(); }
        private:
            Columns _columns;
        };
        using ChunkPtr = std::shared_ptr<Chunk>;




    }
}

#endif//BE_CHUNK_H

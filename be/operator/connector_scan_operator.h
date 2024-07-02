//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_CONNECTOR_SCAN_OPERATOR_H
#define BE_CONNECTOR_SCAN_OPERATOR_H

#include "source/chunk_source.h"
#include "datasource/connector.h"
#include "operator/scan_operator.h"
#include "common/buffer.h"
#include "common/chunk.h"

// original file compose by 3 part

// ConnectorScanOperatorFactory

// ConnectorScanOperator

// ConnectorChunkSource


namespace starrocks {
    namespace pipeline {

        class ConnectorScanOperator final : public  ScanOperator {
        public:
            // OperatorFactory, id, drive_sequence, dop, ScanNode
            ConnectorScanOperator() : ScanOperator() {};
            ~ConnectorScanOperator() override = default;

            // todo  Status do_prepare() override;
            // todo void do_close() override;
            ChunkSourcePtr create_chunk_source() override;
            // connector_type

            // todo void attach_chunk_source(int32_t source_index) override;
            // todo void detach_chunk_source(int32_t source_index) override;
            // todo bool has_shared_chunk_source() const override;
            // todo vectorized::ChunkPtr get_chunk_from_buffer() override;
            // todo size_t num_buffered_chunks() const override;
            // todo size_t buffer_size() const override;
            // todo size_t buffer_capacity() const override;
            // todo size_t default_buffer_capacity() const override;
            // pin_chunk()
            // todo  bool is_buffer_full() const override;
            // todo void set_buffer_finished() override;

            // available_pickup_morsel_count
            // begin_driver_process
            // end_driver_process
            // is_running_all_io_tasks

            // mutable ConnectorScanOperatorAdaptiveProcessor
            // _enable_adaptive_io_tasks

        };

        class ConnectorChunkSource : public ChunkSource {

        public:
            ConnectorChunkSource(ScanOperator* op, Buffer& chunk_buffer);

            ~ConnectorChunkSource() override;
            Status prepare() override;
            void close() override;

        private:
            //

            Status _read_chunk(RuntimeState* state, vectorized::ChunkPtr* chunk) override;

            connector::DataSourcePtr _data_source;
            // maybe used vectorized::ConnectorScanNode* _scan_node;

            Status _status = Status::OK();
            bool _opened = false;
            bool _closed = false;
            uint64_t _rows_read = 0;
            ConnectorScanOperator* _op = nullptr;
        };
    }// namespace pipeline
}// namespace starrocks


#endif//BE_CONNECTOR_SCAN_OPERATOR_H

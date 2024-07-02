//
// Created by lixinzhuang1 on 2024/6/5.
//

#include "connector_scan_operator.h"
#include "common/chunk.h"
#include "datasource/jdbc_connector.h"

namespace starrocks {

    namespace pipeline {

        ConnectorChunkSource::~ConnectorChunkSource() noexcept {

        }

        void ConnectorChunkSource::close() {

        }

        Status ConnectorChunkSource::prepare() {
                return Status::OK();
        }

        ChunkSourcePtr ConnectorScanOperator::create_chunk_source() {
            // buffer will be destroyed when create_chunk_source finished
                Buffer buffer;
                return std::make_shared<ConnectorChunkSource>(this, buffer);
        }

        ConnectorChunkSource::ConnectorChunkSource(starrocks::pipeline::ScanOperator *op, starrocks::Buffer &chunk_buffer)
        : ChunkSource(op, chunk_buffer)
        {
            _data_source = std::make_unique<connector::JDBCDataSource>();

        }

        Status ConnectorChunkSource::_read_chunk(RuntimeState* state, vectorized::ChunkPtr *chunk) {

            if(!_opened) {
                // todo need create _data_source
                auto status = _data_source -> open(state);
                if(!status.ok()) {
                    return Status::InternalError("open data source error");
                }
                _opened = true;
            }

            // if cancelled return

            // limit cancelled

            while (_status.ok()) {
                vectorized::ChunkPtr tmp;
                _status = _data_source ->get_next(&tmp);
                // ok
                // is_end_of_file
                // is_time_out

            }
            return Status::OK();
        }



    } // pipeline

} // namespace starrocks

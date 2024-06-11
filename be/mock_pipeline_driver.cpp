//
// Created by lixinzhuang1 on 2024/6/5.
//

#include "mock_pipeline_driver.h"

namespace starrocks::pipeline {
    Status MockPipelineDriver::prepare() {
        // create operators
        return Status::OK();
    }

    StatusOr<DriverState> MockPipelineDriver::process() {

        StatusOr<vectorized::ChunkPtr> maybe_chunk;
        ScanOperator* cur_op = source_scan_operator();
        maybe_chunk = cur_op -> pull_chunk();

        Status return_status = Status::OK();

    }

    void MockPipelineDriver::finalize(DriverState state) {
        // close


    }

}

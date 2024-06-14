//
// Created by lixinzhuang1 on 2024/6/6.
//

#include "scan_operator.h"


namespace starrocks::pipeline {

    ChunkSourcePtr ScanOperator::create_chunk_source() {
            return nullptr;
    }

    Status ScanOperator::_pickup_morsel() {

        _chunk_source = create_chunk_source();
        auto status = _chunk_source -> prepare();
        if(!status.ok()) {
            set_finished();
            return status;
        }

        status = _trigger_next_scan();

        return Status::OK();
    }

    StatusOr<vectorized::ChunkPtr> ScanOperator::pull_chunk() {
        // get scan status
        if(!_get_scan_status().ok()) return StatusOr<vectorized::ChunkPtr>();

        // set peak buffer size counter

        // todo return if error
        _try_to_trigger_next_scan();


        // try to trigger next scan


    }

    Status ScanOperator::_try_to_trigger_next_scan() {
        // to sure to put it here for updating state.
        // because we want to update state based on raw data.

        if(_chunk_source != nullptr && _chunk_source -> has_next_chunk()) {
            if(!_trigger_next_scan().ok()) return Status::InternalError("trigger next scan error");
        }
        // to create _chunk_source
        _pickup_morsel();
        return Status::OK();
    }

    Status ScanOperator::_trigger_next_scan() {

        auto& chunk_source = _chunk_source;
        auto status = chunk_source ->buffer_next_batch_chunks_blocking(kIOTaskBatchSize);
        _finish_chunk_source_task();

        return status;
    }

    void ScanOperator::_finish_chunk_source_task() {
        if(!_chunk_source -> has_next_chunk() || is_finished()) {
            // close
        }
    }

    bool ScanOperator::has_output() const {
        return false;
    }

    bool ScanOperator::is_finished() const {
        return true;
    }

    ScanOperator::~ScanOperator() {

    }


}

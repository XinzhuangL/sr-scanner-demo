//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_MOCK_PIPELINE_DRIVER_H
#define BE_MOCK_PIPELINE_DRIVER_H
#include <vector>
#include "operator/source_operator.h"
#include "common/statusor.h"
#include "operator/scan_operator.h"

namespace starrocks::pipeline {

    enum DriverState : uint32_t {
        NOT_READY = 0,
        READY = 1,
        RUNNING = 2,
        INPUT_EMPTY = 3,
        OUTPUT_FULL = 4,
        PRECONDITION_BLOCK = 5,
        FINISH = 6,
        CANCELED = 7,
        INTERNAL_ERROR = 8,
        // PENDING_FINISH means a driver's SinkOperator has finished, but its SourceOperator still have a pending
        // io task executed by io threads synchronously, a driver turns to FINISH from PENDING_FINISH after the
        // pending io task's completion.
        PENDING_FINISH = 9,
    };

    using OperatorPtr = std::shared_ptr<Operator>;
    using Operators = std::vector<OperatorPtr>;

    class MockPipelineDriver {

    public:
        MockPipelineDriver(const Operators& operators)
                : _operators(operators) {}

        ~MockPipelineDriver() noexcept;
        Status prepare();
        StatusOr<DriverState> process();
        void finalize(DriverState state);

    Operators& operators() { return _operators; }
    ScanOperator* source_scan_operator() {
        return _operators.empty() ? nullptr : dynamic_cast<ScanOperator*>(_operators.front().get());
    }

    private:
        Operators _operators;
    };


}



#endif//BE_MOCK_PIPELINE_DRIVER_H

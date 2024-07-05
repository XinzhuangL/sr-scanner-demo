//
// Created by lixinzhuang1 on 2024/7/2.
//
#pragma once
#ifndef BE_RUNTIME_STATE_H
#define BE_RUNTIME_STATE_H

#include <atomic>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "common/status.h"


namespace starrocks {
    class DescriptorTbl;
    class RuntimeState {
    public:
        RuntimeState() {};
        ~RuntimeState() {};

        const DescriptorTbl& desc_tbl() const { return *_desc_tbl; }
        void set_desc_tbl(DescriptorTbl* desc_tbl) { _desc_tbl = desc_tbl; }

        // Sets _process_status with err_msg if no error has been set yet.
        void set_process_status(const std::string& err_msg) {
            std::lock_guard<std::mutex> l(_process_status_lock);
            if (!_process_status.ok()) {
                return;
            }
            _process_status = Status::InternalError(err_msg);
        }

        void set_process_status(const Status& status) {
            if (status.ok()) {
                return;
            }
            std::lock_guard<std::mutex> l(_process_status_lock);
            if (!_process_status.ok()) {
                return;
            }
            _process_status = status;
        }

    private:
        DescriptorTbl* _desc_tbl = nullptr;

        // Non-OK if an error has occurred and query execution should abort. Used only for
        // asynchronously reporting such errors (e.g., when a UDF reports an error), so this
        // will not necessarily be set in all error cases.
        std::mutex _process_status_lock;
        Status _process_status;

    };
}


#endif//BE_RUNTIME_STATE_H

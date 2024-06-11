//
// Created by lixinzhuang1 on 2024/6/11.
//

#include "udf/java_udf.h"

namespace starrocks::vectorized {

    Status detect_java_runtime() {
        const char* p = std::getenv("JAVA_HOME");
        if (p == nullptr) {
            return Status::InternalError("env 'JAVA_HOME' is not set");
        }
        return Status::OK();
    }
}

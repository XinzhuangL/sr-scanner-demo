//
// Created by lixinzhuang1 on 2024/6/11.
//

#include "udf/java_udf.h"

namespace starrocks::vectorized {

    JVMFunctionHelper& JVMFunctionHelper::getInstance() {
        if (_env == nullptr) {
            _env = getJNIEnv();
          //  CHECK(_env != nullptr) << "couldn't got a JNIEnv";
        }
        static JVMFunctionHelper helper;
        return helper;
    }

    void JVMFunctionHelper::_init() {
        // todo
    }
    Status detect_java_runtime() {
        const char* p = std::getenv("JAVA_HOME");
        if (p == nullptr) {
            return Status::InternalError("env 'JAVA_HOME' is not set");
        }
        return Status::OK();
    }
}

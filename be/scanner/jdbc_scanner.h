//
// Created by lixinzhuang1 on 2024/6/6.
//
#pragma once
#ifndef BE_JDBC_SCANNER_H
#define BE_JDBC_SCANNER_H

#include "common/chunk.h"
#include "common/status.h"
#include "jni.h"
#include "runtime/primitive_type.h"
#include "table/descriptors.h"
#include "udf/java_udf.h"
#include <map>
#include <string>
#include <vector>

namespace starrocks::vectorized {
    struct JDBCScanContext {
        std::string driver_path;
        std::string driver_class_name;
        std::string jdbc_url;
        std::string user;
        std::string passwd;
        std::string sql;
        std::map<std::string, std::string> properties;
    };

    class JDBCScanner {
    public:
        JDBCScanner(const JDBCScanContext &context, const TupleDescriptor *tuple_desc)
        //: _scan_ctx(context), _slot_descs(tuple_desc -> slots()){}
                : _scan_ctx(context), _slot_descs(tuple_desc -> slots()){}
        ~JDBCScanner() = default;

        Status open();
        Status get_next(ChunkPtr* chunk, bool* eos);
        Status close();

    private:
        // some function
        // init profile

        StatusOr<PrimitiveType> _precheck_data_type(const std::string& java_class, SlotDescriptor* slot_desc);

        Status _init_jdbc_bridge();

        Status _init_jdbc_scan_context();

        Status _init_jdbc_scanner();

        Status _init_column_class_name();

        Status _init_jdbc_util();

        Status _has_next(bool* result);

        Status _get_next_chunk(jobject* chunk, size_t* num_rows);

        Status _file_chunk(jobject jchunk, size_t nums_rows, ChunkPtr* chunk);

        Status _close_jdbc_scanner();




        JDBCScanContext _scan_ctx;
        // result column slot desc
        std::vector<SlotDescriptor *> _slot_descs;
        // java class name for each result column
        std::vector<std::string> _column_class_names;
        std::vector<PrimitiveType> _result_column_types;
        ChunkPtr _result_chunk;

        std::unique_ptr<JVMClass> _jdbc_bridge_cls;
        std::unique_ptr<JVMClass> _jdbc_scanner_cls;
        std::unique_ptr<JVMClass> _jdbc_util_cls;

        jmethodID _scanner_has_next;
        jmethodID _scanner_get_next_chunk;
        jmethodID _scanner_result_rows;
        jmethodID _scanner_close;

        // JDBCUtil method
        jmethodID _util_format_date;
        jmethodID _util_format_localdatetime;

        // _jdbc_bridge and _jdbc_scan_context are only used for cross-function passing
        JavaGlobalRef _jdbc_bridge = nullptr;
        JavaGlobalRef _jdbc_scan_context = nullptr;
        JavaGlobalRef _jdbc_scanner = nullptr;


        static constexpr const char *JDBC_BRIDGE_CLASS_NAME = "com/starrocks/jdbcbridge/JDBCBridge";
        static constexpr const char *JDBC_SCAN_CONTEXT_CLASS_NAME = "com/starrocks/jdbcbridge/JDBCScanContext";
        static constexpr const char *JDBC_SCANNER_CLASS_NAME = "com/starrocks/jdbcbridge/JDBCScanner";
        static constexpr const char *JDBC_UTIL_CLASS_NAME = "com/starrocks/jdbcbridge/JDBCUtil";


        static const int32_t DEFAULT_JDBC_CONNECTION_POOL_SIZE = 8;
        static const int32_t MINIMUM_ALLOWED_JDBC_CONNECTION_IDLE_TIMEOUT_MS = 10000;
    };
}// namespace starrocks::vectorized

#endif//BE_JDBC_SCANNER_H

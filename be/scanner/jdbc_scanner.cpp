//
// Created by lixinzhuang1 on 2024/6/6.
//

#include "jdbc_scanner.h"
#include "common/status.h"
#include "udf/java_udf.h"
#include <utility>
#include "runtime/primitive_type.h"
#include "column/column_helper.h"
#include "udf/types.h"


namespace starrocks::vectorized {

#define CHECK_JAVA_EXCEPTION(env, error_message)                                     \
    if (jthrowable thr = env->ExceptionOccurred(); thr) {                            \
        std::string err = JVMFunctionHelper::getInstance().dumpExceptionString(thr); \
        env->ExceptionClear();                                                       \
        env->DeleteLocalRef(thr);                                                    \
        return Status::InternalError(error_message);                                 \
    }

    Status JDBCScanner::open() {

        // todo all RETURN IF ERROR
        detect_java_runtime();

        _init_jdbc_bridge();

        _init_jdbc_scan_context();

        _init_jdbc_scanner();

        _init_column_class_name();

        _init_jdbc_util();

        return Status::OK();
    }


    Status JDBCScanner::get_next(starrocks::vectorized::ChunkPtr *chunk, bool *eos) {
        bool has_next = false;
        _has_next(&has_next);
        if (!has_next) {
            *eos = true;
            return Status::OK();
        }
        jobject  jchunk = nullptr;
        size_t jchunk_rows = 0;
        // LOCAL REF GUAD
        _get_next_chunk(&jchunk, &jchunk_rows);
        _file_chunk(jchunk, jchunk_rows, chunk);
        return Status::OK();

    }

    Status JDBCScanner::close() {
    }

    Status JDBCScanner::_init_jdbc_bridge() {
        // 1. construct JDBCBridge
        auto &h = JVMFunctionHelper::getInstance();
        auto *env = h.getEnv();

        auto jdbc_bridge_cls = env->FindClass(JDBC_BRIDGE_CLASS_NAME);
        // jdbc_bridge_cls != null
        _jdbc_bridge_cls = std::make_unique<JVMClass>(env->NewGlobalRef(jdbc_bridge_cls));
        // todo  LOCAL_REF_GUARD_ENV(env, jdbc_bridge_cls);
        _jdbc_bridge = _jdbc_bridge_cls->newInstance().ValueOrDie();


        // 2. set class loader
        jmethodID set_class_loader = env->GetMethodID(_jdbc_bridge_cls->clazz(), "setClassLoader", "(Ljava/lang/String;)V");
        // todo DCheck set_class_loader != null

        jstring driver_location = env->NewStringUTF(_scan_ctx.driver_path.c_str());
        // todo LOCAL_REF_GUARD_ENV(env, driver_location);
        env->CallVoidMethod(_jdbc_bridge.handle(), set_class_loader, driver_location);
        CHECK_JAVA_EXCEPTION(env, "set class loader failed")
        return Status::OK();
    }

    Status JDBCScanner::_init_jdbc_scan_context() {
        auto* env = JVMFunctionHelper::getInstance().getEnv();
        jclass scan_context_cls = env ->FindClass(JDBC_SCAN_CONTEXT_CLASS_NAME);
        // todo double check
        // todo LOCAL_REF_GUARD_ENV
        jmethodID constructor = env ->GetMethodID(
                scan_context_cls, "<init>",
                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIII)V"
                );
        jstring driver_class_name = env ->NewStringUTF(_scan_ctx.driver_class_name.c_str());
        // todo LOCAL_REF_GUARD_ENV
        jstring jdbc_url = env ->NewStringUTF(_scan_ctx.jdbc_url.c_str());
        // todo LOCAL_REF_GUARD_ENV
        jstring user = env ->NewStringUTF(_scan_ctx.user.c_str());
        // todo LOCAL_REF_GUARD_ENV
        jstring passwd = env ->NewStringUTF(_scan_ctx.passwd.c_str());
        // todo LOCAL_REF_GUARD_ENV
        jstring sql = env ->NewStringUTF(_scan_ctx.sql.c_str());
        // todo LOCAL_REF_GUARD_ENV
        // todo equal chunk size
        int statement_fetch_size =  128;
        int connection_pool_size = 8;
        int minimum_idle_connections = 1;
        int idle_timeout_ms = 60000;

        auto scan_ctx =
        env -> NewObject(scan_context_cls, constructor, driver_class_name, jdbc_url, user, passwd, sql,
                               statement_fetch_size, connection_pool_size, minimum_idle_connections, idle_timeout_ms);

        _jdbc_scan_context = env ->NewGlobalRef(scan_ctx);
        // todo LOCAL_REF_GUARD_ENV
        CHECK_JAVA_EXCEPTION(env, "construct JDBScanContext failed")
        return Status::OK();
    }

    Status JDBCScanner::_init_jdbc_scanner() {
        auto* env = JVMFunctionHelper::getInstance().getEnv();

        jmethodID  get_scanner = env ->GetMethodID(_jdbc_bridge_cls -> clazz(), "getScanner",
                                                 "(Lcom/starrocks/jdbcbridge/JDBCScanContext;)Lcom/starrocks/jdbcbridge/JDBCScanner;");
        // get_scanner != nullptr
        auto jdbc_scanner = env ->CallObjectMethod(_jdbc_bridge.handle(), get_scanner, _jdbc_scan_context.handle());
        _jdbc_scanner = env ->NewGlobalRef(jdbc_scanner);
        // todo LOCAL_REF_GUARD_ENV
        CHECK_JAVA_EXCEPTION(env, "get JDBCScanner failed")

        auto jdbc_scanner_cls = env ->FindClass(JDBC_SCANNER_CLASS_NAME);
        _jdbc_scanner_cls = std::make_unique<JVMClass>(env ->NewGlobalRef(jdbc_scanner_cls));
        // todo LOCAL_REF_GUARD_ENV

        // todo != null
        // init jmethod
        _scanner_has_next = env ->GetMethodID(_jdbc_scanner_cls -> clazz(), "hasNext", "()Z");

        _scanner_get_next_chunk = env ->GetMethodID(_jdbc_scanner_cls -> clazz(), "getNextChunk", "()Ljava/util/List;");

        _scanner_result_rows = env ->GetMethodID(_jdbc_scanner_cls -> clazz(), "getResultNumRows", "()I");

        _scanner_close = env ->GetMethodID(_jdbc_scanner_cls->clazz(), "close", "()V");

        // open scanner
        jmethodID scanner_open = env ->GetMethodID(_jdbc_scanner_cls -> clazz(), "open", "()V");

        env ->CallVoidMethod(_jdbc_scanner.handle(), scanner_open);
        CHECK_JAVA_EXCEPTION(env, "open JDBCScanner failed")
        return Status::OK();
    }

    StatusOr<PrimitiveType> JDBCScanner::_precheck_data_type(const std::string &java_class, starrocks::SlotDescriptor *slot_desc) {
        return  TYPE_VARCHAR;
    }

    Status JDBCScanner::_init_column_class_name() {
        auto* env = JVMFunctionHelper::getInstance().getEnv();
        jmethodID get_result_column_class_names =
        env ->GetMethodID(_jdbc_scanner_cls -> clazz(), "getResultColumnClassNames", "()Ljava/util/List;");
        // todo check null
        jobject column_class_names = env ->CallObjectMethod(_jdbc_scanner.handle(), get_result_column_class_names);
        CHECK_JAVA_EXCEPTION(env, "get JDBC result column class name failed")
        // todo LOCAL_REF_GUARD_ENV
        auto& helper = JVMFunctionHelper::getInstance();
        int len = helper.list_size(column_class_names);
        _result_chunk = std::make_shared<Chunk>();
        for (int i = 0; i < len; i++) {
            jobject jelement = helper.list_get(column_class_names, i);
            // LOCAL_REF_GUARD_ENV
            std::string class_name = helper.to_string((jstring)(jelement));
            // todo ASSIGN_OR_RETURN
            auto ret_type = _precheck_data_type(class_name, _slot_descs[i]);
            _column_class_names.emplace_back(class_name);
            _result_column_types.emplace_back(ret_type.ValueOrDie());

            // intermediate means the result type from JDBC Scanner
            // Some types cannot be written directly to the column,
            // sow we need to write directly to the intermediate type and then cast to the target type:
            // eg:
            // JDBC(java.sql.Date) -> SR(TYPE_VARCHAR) -> SR(cast(varchar as TYPE_DATE))
            // todo need Descriptor Column etc.
             auto intermediate = TypeDescriptor(ret_type.ValueOrDie());
             auto result_column = ColumnHelper::create_column(intermediate, true);
             // append_column
             // todo unfinished cast
             return Status::OK();
        }
    }


    Status JDBCScanner::_init_jdbc_util() {
        auto* env = JVMFunctionHelper::getInstance().getEnv();
        // init JDBCUtil method
        auto jdbc_util_cls = env ->FindClass(JDBC_UTIL_CLASS_NAME);

        _jdbc_util_cls = std::make_unique<JVMClass>(env ->NewGlobalRef(jdbc_util_cls));
        //

        _util_format_date =
        env ->GetStaticMethodID(_jdbc_util_cls -> clazz(), "formatDate", "(Ljava/sql/Date;)Ljava/lang/String;");
        _util_format_localdatetime = env ->GetStaticMethodID(_jdbc_util_cls -> clazz(), "formatLocalDatetime",
                                                            "(Ljava/time/LocalDateTime;)Ljava/lang/String;");
        return Status::OK();
    }

    Status JDBCScanner::_has_next(bool *result) {
        auto* env = JVMFunctionHelper::getInstance().getEnv();
        jboolean ret = env ->CallBooleanMethod(_jdbc_scanner.handle(), _scanner_has_next);
        CHECK_JAVA_EXCEPTION(env, "call JDBCScanner hasNext failed")
        *result = ret;
        return Status::OK();
    }

    Status JDBCScanner::_get_next_chunk(jobject *chunk, size_t *num_rows) {
        auto* env = JVMFunctionHelper::getInstance().getEnv();
        // scope time
        // count io_counter
        *chunk = env ->CallObjectMethod(_jdbc_scanner.handle(), _scanner_get_next_chunk);
        CHECK_JAVA_EXCEPTION(env, "getNextChunk failed")
        *num_rows = env ->CallIntMethod(_jdbc_scanner.handle(), _scanner_result_rows);
        CHECK_JAVA_EXCEPTION(env, "getResultNumRows failed")
        return Status::OK();
    }

    Status JDBCScanner::_close_jdbc_scanner() {
        auto* env = JVMFunctionHelper::getInstance().getEnv();
        if (_jdbc_scanner.handle() == nullptr) {
            return Status::OK();
        }
        env ->CallVoidMethod(_jdbc_scanner.handle(), _scanner_close);
        CHECK_JAVA_EXCEPTION(env, "close JDBCScanner failed")

        _jdbc_scanner.clear();
        _jdbc_scan_context.clear();
        _jdbc_bridge.clear();
        _jdbc_util_cls.reset();
        _jdbc_scanner_cls.reset();
        _jdbc_bridge_cls.reset();
        return Status::OK();
    }

    Status JDBCScanner::_file_chunk(jobject jchunk, size_t nums_rows, starrocks::vectorized::ChunkPtr *chunk) {
        // scope time
        auto& helper = JVMFunctionHelper::getInstance();
        auto* env = helper.getEnv();
        // counter
        (*chunk).reset();

        for(size_t i = 0; i < _slot_descs.size(); i++) {
            jobject jcolumn = helper.list_get(jchunk, i);
            // REF
            //  todo set result column

            // check data's length for string type

        }
    }
}// namespace starrocks::vectorized
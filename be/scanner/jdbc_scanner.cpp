//
// Created by lixinzhuang1 on 2024/6/6.
//

#include "jdbc_scanner.h"
#include "udf/java_udf.h"
#include <utility>


 namespace starrocks::vectorized {
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

     }

     Status JDBCScanner::close() {

     }

     Status JDBCScanner::_init_jdbc_bridge() {
         // 1. construct JDBCBridge
         auto& h = JVMFunctionHelper::getInstance();
         auto* env = h.getEnv();

         auto jdbc_bridge_cls = env ->FindClass(JDBC_BRIDGE_CLASS_NAME);
         // jdbc_bridge_cls != null
         _jdbc_bridge_cls = std::make_unique<JVMClass>(env ->NewGlobalRef(jdbc_bridge_cls));
         // todo  LOCAL_REF_GUARD_ENV(env, jdbc_bridge_cls);
         _jdbc_bridge = std::move(_jdbc_bridge_cls -> newInstance().value());



     }

     Status JDBCScanner::_init_jdbc_scan_context() {

     }

     Status JDBCScanner::_init_jdbc_scanner() {

     }

     StatusOr<PrimitiveType> JDBCScanner::_precheck_data_type(const std::string &java_class, starrocks::SlotDescriptor *slot_desc) {

     }

     Status JDBCScanner::_init_column_class_name() {

     }


     Status JDBCScanner::_init_jdbc_util() {

     }

     Status JDBCScanner::_has_next(bool *result) {

     }

     Status JDBCScanner::_get_next_chunk(int *chunk, size_t *num_rows) {

     }

     Status JDBCScanner::_close_jdbc_scanner() {

     }

     Status JDBCScanner::_file_chunk(int jchunk, size_t nums_rows, starrocks::vectorized::ChunkPtr *chunk) {

     }
 }
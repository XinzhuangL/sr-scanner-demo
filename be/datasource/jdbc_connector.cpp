//
// Created by lixinzhuang1 on 2024/6/6.
//

#include "jdbc_connector.h"
#include "scanner/jdbc_scanner.h"
#include "runtime/jdbc_driver_manager.h"

// simplified provider
// provider used to create JDBCConnector
namespace starrocks::connector {
    Status JDBCDataSource::open() {

    }

    void JDBCDataSource::close() {

    }

    Status JDBCDataSource::get_next(vectorized::ChunkPtr *chunk) {

    }

    Status JDBCDataSource::_create_scanner() {
        Status status;
        std::string driver_name = "";
        std::string driver_url = "";
        std::string driver_checksum = "";
        std::string driver_class = "";
        std::string driver_location = "";
        status = JDBCDriverManager::getInstance() ->get_driver_location(driver_name, driver_url, driver_checksum, &driver_location);

        if (!status.ok()) {
            return status;
        }

        vectorized::JDBCScanContext scan_ctx;
        scan_ctx.driver_path = driver_location;
        scan_ctx.driver_class_name = driver_class;
        // properties

        _scanner = new vectorized::JDBCScanner(scan_ctx, _tuple_desc);
        _scanner -> open();
        return Status::OK();



    }
}



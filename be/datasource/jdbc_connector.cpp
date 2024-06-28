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

        _create_scanner();

        return Status::OK();
    }

    void JDBCDataSource::close() {

    }

    Status JDBCDataSource::get_next(vectorized::ChunkPtr *chunk) {
        bool eos = false;
            _init_chunk(chunk, 0);
            do {
                _scanner ->get_next(chunk, &eos);

            } while (!eos && (*chunk) -> num_rows() == 0);
            if (eos) {
                // todo return end of file
                return Status::OK();
            }
            return Status::OK();

    }

    Status JDBCDataSource::_create_scanner() {
        Status status;
        std::string driver_name = "mssql-jdbc";
        std::string driver_url = "https://repo1.maven.org/maven2/com/microsoft/sqlserver/mssql-jdbc/12.4.2.jre8/mssql-jdbc-12.4.2.jre8.jar";
        std::string driver_checksum = "6c10ef9c332098b2c51f20f03eb6c294";
        std::string driver_class = "com.microsoft.sqlserver.jdbc.SQLServerDriver";
        std::string driver_location = "";
        // todo from table properties
        status = JDBCDriverManager::getInstance() -> get_driver_location(driver_name, driver_url, driver_checksum, &driver_location);

        if (!status.ok()) {
            return status;
        }

        vectorized::JDBCScanContext scan_ctx;
        scan_ctx.driver_path = driver_location;
        scan_ctx.driver_class_name = driver_class;
        // properties
        scan_ctx.jdbc_url = "jdbc:sqlserver://127.0.0.1:1433;databaseName=MyDatabase;trustServerCertificate=true";
        scan_ctx.user = "sa";
        scan_ctx.passwd = "Lxz74321.!";
        scan_ctx.sql = "select SampleID,SampleBigInt from dbo.SampleTypeTable";

        _scanner = new vectorized::JDBCScanner(scan_ctx, _tuple_desc);
        _scanner -> open();
        return Status::OK();



    }
}



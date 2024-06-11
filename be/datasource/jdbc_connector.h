//
// Created by lixinzhuang1 on 2024/6/6.
//

#ifndef BE_JDBC_CONNECTOR_H
#define BE_JDBC_CONNECTOR_H

#include "datasource/connector.h"

namespace starrocks {
    namespace vectorized {
        class JDBCScanner;
    } // namespace vectorized
    namespace connector {

        // JDBCConnector
        class JDBCDataSource;
        // JDBDataSourceProvider

class JDBCDataSource final : public DataSource {
public:
    ~JDBCDataSource() override = default;

    // todo need provider to create
    JDBCDataSource();
    Status open() override;
    void close() override;
    Status get_next(vectorized::ChunkPtr* chunk) override;

private:
    Status _create_scanner();

    // Provider
    // pool
    vectorized::JDBCScanner* _scanner = nullptr;
    // statistics

};



    } // namespace connector
}

#endif//BE_JDBC_CONNECTOR_H

//
// Created by lixinzhuang1 on 2024/7/3.
//

#pragma once
#include "common/global_types.h"
#include <string>
// 模拟TTableDescriptor

namespace starrocks {
    struct MockJDBCTable {

    public:
        // 默认构造函数
        MockJDBCTable() = default;

        // 带参数的构造函数，用于初始化所有成员变量
        MockJDBCTable(const std::string &driver_name, const std::string &driver_url,
                      const std::string &driver_checksum, const std::string &driver_class,
                      const std::string &url, const std::string &table,
                      const std::string &user, const std::string &passwd)
            : _jdbc_driver_name(driver_name),
              _jdbc_driver_url(driver_url),
              _jdbc_driver_checksum(driver_checksum),
              _jdbc_driver_class(driver_class),
              _jdbc_url(url),
              _jdbc_table(table),
              _jdbc_user(user),
              _jdbc_passwd(passwd) {}


        std::string _jdbc_driver_name;
        std::string _jdbc_driver_url;
        std::string _jdbc_driver_checksum;
        std::string _jdbc_driver_class;

        std::string _jdbc_url;
        std::string _jdbc_table;
        std::string _jdbc_user;
        std::string _jdbc_passwd;
    };

    enum TableType {
        JDBC_TABLE
    };

    class MockTTableDescriptor {

    public:
        MockTTableDescriptor() = default;
        MockTTableDescriptor(std::string &name, std::string &database, TupleId id, TableType table_type,
                             MockJDBCTable &jdbc_table) : _name(name),
                                                          _database(database),
                                                          _id(id),
                                                          _table_type(table_type),
                                                          _jdbc_table(jdbc_table) {}


        std::string _name;
        std::string _database;
        TupleId _id;
        TableType _table_type;
        MockJDBCTable _jdbc_table;
    };


}// namespace starrocks

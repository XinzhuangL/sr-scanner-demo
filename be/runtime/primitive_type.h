//
// Created by lixinzhuang1 on 2024/6/11.
//

#ifndef BE_PRIMITIVE_TYPE_H
#define BE_PRIMITIVE_TYPE_H

namespace starrocks {
    enum PrimitiveType {
        INVALID_TYPE = 0,
        TYPE_NULL,
        TYPE_BOOLEAN,
        TYPE_TINYINT,
        TYPE_SMALLINT,
        TYPE_INT,
        TYPE_BIGINT,
        TYPE_LARGEINT,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_VARCHAR,
        TYPE_DATE,
        TYPE_DATETIME,
        TYPE_BINARY, // Not impl
        TYPE_DECIMAL,
        TYPE_CHAR,

        TYPE_STRUCT,
        TYPE_ARRAY,
        TYPE_MAP,
        TYPE_HLL,
        TYPE_DECIMALV2,

        TYPE_TIME,
        TYPE_OBJECT,
        TYPE_PERCENTILE,
        TYPE_DECIMAL32,
        TYPE_DECIMAL64,
        TYPE_DECIMAL128,

        TYPE_JSON,
        TYPE_FUNCTION,

    };
}

#endif//BE_PRIMITIVE_TYPE_H

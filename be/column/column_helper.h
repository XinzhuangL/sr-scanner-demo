//
// Created by lixinzhuang1 on 2024/6/12.
//
#pragma once
#ifndef BE_COLUMN_HELPER_H
#define BE_COLUMN_HELPER_H

#include "udf/types.h"
#include <memory>
#include "common/chunk.h"

namespace starrocks {
    struct TypeDescriptor;
}

namespace starrocks::vectorized {
    class Column;


    class ColumnHelper {
    public:
        static ColumnPtr create_column(const TypeDescriptor& type_desc, bool nullable);
    };
}

#endif//BE_COLUMN_HELPER_H

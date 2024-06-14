//
// Created by lixinzhuang1 on 2024/6/12.
//
#pragma once
#ifndef BE_TYPES_H
#define BE_TYPES_H

#include "runtime/primitive_type.h"

namespace starrocks {

    struct TypeDescriptor {
        PrimitiveType type {INVALID_TYPE};

        int len{-1};

        int precision{-1};
        int scale{-1};

        TypeDescriptor() = default;

        explicit TypeDescriptor(PrimitiveType type) : type(type), len(-1), precision(-1), scale(-1) {}
    };
}

#endif//BE_TYPES_H

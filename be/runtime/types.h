//
// Created by lixinzhuang1 on 2024/6/28.
//
#pragma once
#ifndef BE_TYPES_H
#define BE_TYPES_H

#include "common/constexpr.h"
#include "runtime/primitive_type.h"
#include <glog/logging.h>
#include <string>
#include <vector>

namespace starrocks {
    struct TypeDescriptor {

        PrimitiveType type{INVALID_TYPE};

        // Only meaningful for type TYPE_CHAR/TYPE_VARCHAR/TYPE_HILL

        int len{-1};
        static constexpr int MAX_VARCHAR_LENGTH = 1048576;
        static constexpr int MAX_CHAR_LENGTH = 255;
        static constexpr int MAX_CHAR_INLINE_LENGTH = 128;
        static constexpr int DEFAULT_BITMAP_LENGTH = 128;

        // only set if type == TYPE_DECIMAL
        int precision{-1};
        int scale{-1};

        // Must be kept in sync with FE's max precision/scale.
        static const int MAX_PRECISION = 38;
        static const int MAX_SCALE = MAX_PRECISION;

        // The maximum precision representable by a 4-byte decimal (Decimal4Value)
        static const int MAX_DECIMAL4_PRECISION = 9;
        static const int MAX_DECIMAL8_PRECISION = 18;

        // Empty for scalar types
        std::vector<TypeDescriptor> children;

        // Only set if type = TYPE_STRUCT. The field name of each child.
        std::vector<std::string> field_names;

        TypeDescriptor() = default;

        explicit TypeDescriptor(PrimitiveType type) : type(type), len(-1), precision(-1), scale(-1) {}

        static TypeDescriptor create_char_type(int len) {
            TypeDescriptor ret;
            ret.type = TYPE_CHAR;
            ret.len = len;
            return ret;
        }

        static TypeDescriptor create_varchar_type(int len) {
            TypeDescriptor ret;
            ret.type = TYPE_VARCHAR;
            ret.len = len;
            return ret;
        }

        static TypeDescriptor create_json_type() {
            TypeDescriptor res;
            res.type = TYPE_JSON;
            res.len = kJsonDefaultSize;
            return res;
        }

        static TypeDescriptor create_hll_type() {
            TypeDescriptor ret;
            ret.type = TYPE_HLL;
            ret.len = HLL_COLUMN_DEFAULT_LEN;
            return ret;
        }

        static TypeDescriptor create_decimal_type(int precision, int scale) {
            DCHECK_LE(precision, MAX_PRECISION);
            DCHECK_LE(scale, MAX_SCALE);
            DCHECK_GE(precision, 0);
            DCHECK_LE(scale, precision);
            TypeDescriptor ret;
            ret.type = TYPE_DECIMAL;
            ret.precision = precision;
            ret.scale = scale;
            return ret;
        }

        static TypeDescriptor create_decimalv2_type(int precision, int scale) {
            DCHECK_LE(precision, MAX_PRECISION);
            DCHECK_LE(scale, MAX_SCALE);
            DCHECK_GE(precision, 0);
            DCHECK_LE(scale, precision);
            TypeDescriptor ret;
            ret.type = TYPE_DECIMALV2;
            ret.precision = precision;
            ret.scale = scale;
            return ret;
        }

        static TypeDescriptor create_decimalv3_type(PrimitiveType type, int precision, int scale) {
            DCHECK(type == TYPE_DECIMAL32 || type == TYPE_DECIMAL64 || type == TYPE_DECIMAL128);
            DCHECK_LE(precision, MAX_PRECISION);
            DCHECK_LE(scale, MAX_SCALE);
            DCHECK_GE(precision, 0);
            DCHECK_LE(scale, precision);
            TypeDescriptor ret;
            ret.type = type;
            ret.precision = precision;
            ret.scale = scale;
            return ret;
        }

        static TypeDescriptor create_bitmap_type() {
            TypeDescriptor ret(TYPE_OBJECT);
            ret.len = DEFAULT_BITMAP_LENGTH;
            return ret;
        }

        static TypeDescriptor from_primitive_type(PrimitiveType type,
                                                  [[maybe_unused]] int len = TypeDescriptor::MAX_VARCHAR_LENGTH,
                                                  [[maybe_unused]] int precision = 27, [[maybe_unused]] int scale = 9) {
            switch (type) {
                case TYPE_CHAR:
                    return TypeDescriptor::create_char_type(MAX_CHAR_LENGTH);
                case TYPE_VARCHAR:
                    return TypeDescriptor::create_varchar_type(MAX_VARCHAR_LENGTH);
                case TYPE_HLL:
                    return TypeDescriptor::create_hll_type();
                case TYPE_DECIMAL:
                    return TypeDescriptor::create_decimal_type(precision, scale);
                case TYPE_DECIMALV2:
                    return TypeDescriptor::create_decimalv2_type(precision, scale);
                case TYPE_DECIMAL32:
                    return TypeDescriptor::create_decimalv3_type(TYPE_DECIMAL32, precision, scale);
                case TYPE_DECIMAL64:
                    return TypeDescriptor::create_decimalv3_type(TYPE_DECIMAL64, precision, scale);
                case TYPE_DECIMAL128:
                    return TypeDescriptor::create_decimalv3_type(TYPE_DECIMAL128, precision, scale);
                case TYPE_JSON:
                    return TypeDescriptor::create_json_type();
                case TYPE_OBJECT:
                    return TypeDescriptor::create_bitmap_type();
                default:
                    return TypeDescriptor(type);
            }
        }


        std::string debug_string() const;
    };

    inline std::ostream &operator<<(std::ostream &os, const TypeDescriptor &type) {
        os << type.debug_string();
        return os;
    }
}// namespace starrocks


#endif//BE_TYPES_H

//
// Created by lixinzhuang1 on 2024/7/2.
//

#include "runtime/types.h"

namespace starrocks {
    std::string TypeDescriptor::debug_string() const {
//        switch (type) {
//            case TYPE_CHAR:
//                return strings::Substitute("CHAR($0)", len);
//            case TYPE_VARCHAR:
//                return strings::Substitute("VARCHAR($0)", len);
//            case TYPE_DECIMAL:
//                return strings::Substitute("DECIMAL($0, $1)", precision, scale);
//            case TYPE_DECIMALV2:
//                return strings::Substitute("DECIMALV2($0, $1)", precision, scale);
//            case TYPE_DECIMAL32:
//                return strings::Substitute("DECIMAL32($0, $1)", precision, scale);
//            case TYPE_DECIMAL64:
//                return strings::Substitute("DECIMAL64($0, $1)", precision, scale);
//            case TYPE_DECIMAL128:
//                return strings::Substitute("DECIMAL128($0, $1)", precision, scale);
//            case TYPE_ARRAY:
//                return strings::Substitute("ARRAY<$0>", children[0].debug_string());
//            case TYPE_MAP:
//                return strings::Substitute("MAP<$0, $1>", children[0].debug_string(), children[1].debug_string());
//            case TYPE_STRUCT: {
//                std::stringstream ss;
//                ss << "STRUCT{";
//                for (size_t i = 0; i < field_names.size(); i++) {
//                    ss << field_names[i] << " " << children[i].debug_string();
//                    if (i + 1 < field_names.size()) {
//                        ss << ", ";
//                    }
//                }
//                ss << "}";
//                return ss.str();
//            }
//            default:
//                return type_to_string(type);
//        }
    }

}

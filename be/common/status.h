//
// Created by lixinzhuang1 on 2024/6/5.
//

#ifndef BE_STATUS_H
#define BE_STATUS_H
#include <string>
#include "common/statusor.h"

namespace starrocks {
    class Status {
    public:
        Status();
        static Status OK() { return Status(); }
        static Status InternalError(const std::string& msg) {
            return Status();
        }
        bool ok() const { return _state == nullptr; }
    private:
        // OK status has a nullptr _state.  Otherwise, _state is a new[] array
        // of the following form:
        //    _state[0..1]                        == len1: length of message
        //    _state[2..3]                        == len2: length of context
        //    _state[4]                           == code
        //    _state[5.. 5 + len1]                == message
        //    _state[5 + len1 .. 5 + len1 + len2] == context
        const char* _state = nullptr;
    };
template <typename T>
inline const Status& to_status(const StatusOr<T>& st) {
    return st.status();
}

#define RETURN_IF_ERROR_INTERNAL(stmt) \
    do { \
        auto&& status__ = (stmt); \
        if (UNLIKELY(!status__.ok())) { \
            return to_status(status__).clone_and_append_context(__FILE__, __LINE__, AS_STRING(stmt)); \
        } \
    } while(false)

}

#endif//BE_STATUS_H

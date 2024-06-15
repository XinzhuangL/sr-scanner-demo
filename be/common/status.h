//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_STATUS_H
#define BE_STATUS_H
#include <string>
#include <cstring>


namespace starrocks {
    template <typename T>
    class StatusOr;

    class Status {
    public:
        // 默认构造函数，表示OK状态
        Status() : _state(nullptr) {}
        // 析构函数，负责释放_state指向的内存
        ~Status() {
            delete[] _state;
        }


        // 禁止拷贝构造函数和赋值运算符，以简化内存管理
        Status(const Status& other) = default;
        Status& operator=(const Status& other) = default;

        // 允许移动构造函数和赋值运算符
        Status(Status&& other) noexcept : _state(other._state) {
            other._state = nullptr;
        }
        Status& operator=(Status&& other) noexcept {
            if (this != &other) {
                delete[] _state;
                _state = other._state;
                other._state = nullptr;
            }
            return *this;
        }

        // 用于创建错误状态的静态方法
        static Status InternalError(const std::string& msg) {
            Status status;
            size_t msg_len = msg.length();
            status._state = new char[msg_len + 1];
            std::strcpy(status._state, msg.c_str());
            return status;
        }

        // 判断状态是否为OK
        bool ok() const { return _state == nullptr; }

        // 获取错误消息
        std::string message() const {
            if (_state) {
                return std::string(_state);
            } else {
                return "";
            }
        }

        // 创建OK状态的静态方法
        static Status OK() { return Status(); }

    private:
        // OK status has a nullptr _state.  Otherwise, _state is a new[] array
        // of the following form:
        //    _state[0..1]                        == len1: length of message
        //    _state[2..3]                        == len2: length of context
        //    _state[4]                           == code
        //    _state[5.. 5 + len1]                == message
        //    _state[5 + len1 .. 5 + len1 + len2] == context
        char* _state = nullptr;

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

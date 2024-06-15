//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_STATUSOR_H
#define BE_STATUSOR_H
#include "common/status.h"
#include <string>
#include <optional>
#include <stdexcept>
#include <iostream>

namespace starrocks {
    class Status;
    template <typename T>
    class StatusOr {
    public:
        // 构造函数用于设置错误状态
        StatusOr(const Status& status) : status_(status) {
            if (status_.ok()) {
                throw std::invalid_argument("Cannot create a StatusOr with a OK status without a value.");
            }
        }

        // 构造函数用于设置成功的值
        StatusOr(const T& value) : value_(value), status_(Status::OK()) {}

        // 构造函数用于设置成功的值（移动）
        StatusOr(T&& value) : value_(std::move(value)), status_(Status::OK()) {}

        // 判断是否成功
        bool OK() const { return status_.ok(); }

        // 拷贝构造函数
        StatusOr(const StatusOr& other) = default;

        // 移动构造函数
        StatusOr(StatusOr&& other) noexcept = default;

        // 拷贝赋值运算符
        StatusOr& operator=(const StatusOr& other) = default;

        // 移动赋值运算符
        StatusOr& operator=(StatusOr&& other) noexcept = default;

        // 获取值，如果状态不是OK，则抛出异常
        T ValueOrDie() const {
            if (!OK()) {
                throw std::runtime_error("Attempted to access the value of a StatusOr object that is not OK. Error: " + status_.message());
            }
            return *value_;
        }

        // 获取状态
        Status GetStatus() const { return status_; }

    private:
        std::optional<T> value_;
        Status status_;
    };
}

#endif//BE_STATUSOR_H

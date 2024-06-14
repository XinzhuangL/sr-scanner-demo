//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_BUFFER_H
#define BE_BUFFER_H

#include <vector>

namespace starrocks {
    class Buffer {

    public:
        void append(const char* ptr, size_t size);
        const char* read() const;
        size_t size() const;
        void clear();
    private:
        std::vector<char> data;

    };
} // starrocks



#endif//BE_BUFFER_H

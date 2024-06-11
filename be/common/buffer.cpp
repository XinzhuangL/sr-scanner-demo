//
// Created by lixinzhuang1 on 2024/6/5.
//

#include "buffer.h"

namespace starrocks {
    void Buffer::append(const char *ptr, size_t size) {
        data.insert(data.end(), ptr, ptr + size);
    }

    const char *Buffer::read() const {
        return data.data();
    }

    size_t Buffer::size() const {
        return data.size();
    }

    void Buffer::clear() {
        data.clear();
    }
}

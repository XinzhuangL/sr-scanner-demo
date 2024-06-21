//
// Created by lixinzhuang1 on 2024/6/19.
//

#include "util/md5.h"

namespace starrocks {
    Md5Digest::Md5Digest() {
        MD5_Init(&_md5_ctx);
    }

    void Md5Digest::update(const void *data, size_t length) {
        MD5_Update(&_md5_ctx, data, length);
    }

    void Md5Digest::digest() {
        unsigned char buf[MD5_DIGEST_LENGTH];
        MD5_Final(buf, &_md5_ctx);
        char hex_buf[2 * MD5_DIGEST_LENGTH];

        static char dig_vec_lower[] = "0123456789abcdef";
        char* to = hex_buf;
        for (unsigned char i : buf) {
            *to++ = dig_vec_lower[i >> 4];
            *to++ = dig_vec_lower[i & 0x0f];
        }
        _hex.assign(hex_buf, 2 * MD5_DIGEST_LENGTH);
    }
}

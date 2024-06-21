//
// Created by lixinzhuang1 on 2024/6/19.
//

#ifndef BE_MD5_H
#define BE_MD5_H


#include <openssl/md5.h>
#include <string>

namespace starrocks {
    class Md5Digest {
    public:
        Md5Digest();

        void update(const void* data, size_t length);
        void digest();

        const std::string& hex() const { return _hex; }
    private:
        MD5_CTX _md5_ctx;
        std::string _hex;
    };
}


#endif//BE_MD5_H

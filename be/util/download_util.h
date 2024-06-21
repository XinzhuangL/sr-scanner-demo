//
// Created by lixinzhuang1 on 2024/6/19.
//

#ifndef BE_DOWNLOAD_UTIL_H
#define BE_DOWNLOAD_UTIL_H

#include "common/status.h"

namespace starrocks {

    class DownloadUtil {
    public:
        static Status download(const std::string& rul, const std::string& tmp_file, const std::string& target_file,
                               const std::string& expected_checksum);
    };

}

#endif//BE_DOWNLOAD_UTIL_H

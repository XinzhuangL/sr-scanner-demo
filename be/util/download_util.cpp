//
// Created by lixinzhuang1 on 2024/6/19.
//

#include "download_util.h"
#include "util/defer_op.h"
#include "util/md5.h"
#include "http/http_client.h"
#include <boost/algorithm/string/predicate.hpp>

namespace starrocks {
    Status DownloadUtil::download(const std::string &url, const std::string &tmp_file, const std::string &target_file,
                                  const std::string &expected_checksum) {
        auto fp = fopen(tmp_file.c_str(), "w");
        DeferOp defer([&]() {
            if (fp != nullptr) {
                fclose(fp);
            }
        });
        if (fp == nullptr) {
            return Status::InternalError("fail to open tmp file when downloading file from {}");
        }

        Md5Digest digest;
        HttpClient client;
        client.init(url);
        Status status;

        auto download_cb = [&status, &tmp_file, fp, &digest, &url](const void* data, size_t length) {
            digest.update(data, length);
            auto res = fwrite(data, length, 1, fp);
            if (res != 1) {
                status = Status::InternalError("file to write data when downloading file from " + url);
                return false;
            }
            return true;
        };
        client.execute(download_cb);
        if(!status.ok()) {
            return status;
        }

        if (!boost::iequals(digest.hex(), expected_checksum)) {
            return Status::InternalError("Download file's checksum is not match");
        }

        // rename temporary file to target file
        auto ret = rename(tmp_file.c_str(), target_file.c_str());
        if (ret != 0) {
            return Status::InternalError("fail to rename file from" + tmp_file + " to " + target_file);
        }
        return Status::OK();
    }
}

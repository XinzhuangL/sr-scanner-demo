//
// Created by lixinzhuang1 on 2024/6/19.
//

#include "http_client.h"
#include <unistd.h>

// todo include config

namespace starrocks {

    static long download_low_speed_limit_kbps = 50;
    static long max_download_speed_kbps = 50000;
    static long  download_low_speed_time = 300;

    HttpClient::HttpClient() = default;

    HttpClient::~HttpClient() {
        if (_curl != nullptr) {
            curl_easy_cleanup(_curl);
            _curl = nullptr;
        }
        if (_header_list != nullptr) {
            curl_slist_free_all(_header_list);
            _header_list = nullptr;
        }
    }

    Status HttpClient::init(const std::string &url) {
        if (_curl == nullptr) {
            _curl = curl_easy_init();
            if (_curl == nullptr) {
                return Status::InternalError("fail to initialize curl");
            }
        } else {
            curl_easy_reset(_curl);
        }

        if (_header_list != nullptr) {
            curl_slist_free_all(_header_list);
            _header_list = nullptr;
        }

        // set error_buf
        _error_buf[0] = 0;
        auto code = curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, _error_buf);
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set error buffer");
        }

        // forbid signals
        code = curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set CURLOPT_NOSIGNAL");
        }

        // set fail on error
        code = curl_easy_setopt(_curl, CURLOPT_FAILONERROR, 1L);
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set CURLOPT_FAILONERROR");
        }

        // set redirect
        code = curl_easy_setopt(_curl, CURLOPT_FAILONERROR, 1L);
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set CURLOPT_FOLLOWLOCATION");
        }
        code = curl_easy_setopt(_curl, CURLOPT_MAXREDIRS, 20);
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set CURLOPT_MAXREDIRS");
        }

        // callback
        curl_write_callback callback = [](char* buffer, size_t size, size_t nmemb, void* param) {
            auto* client = (HttpClient*)param;
            return client ->on_response_data(buffer, size * nmemb);
        };

        // set callback function
        code = curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, callback);
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set CURLOPT_WRITEFUNCTION");
        }
        code = curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void*)this);
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set CURLOPT_WRITEDATA");
        }

        code = curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
        if (code != CURLE_OK) {
            return Status::InternalError("fail to set CURLOPT_RUL");
        }

        return Status::OK();
    }

    void HttpClient::set_method(starrocks::HttpMethod method) {
        switch (method) {
            case GET:
                curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
                return;
            case PUT:
                curl_easy_setopt(_curl, CURLOPT_UPLOAD, 1L);
                return;
            case POST:
                curl_easy_setopt(_curl, CURLOPT_POST, 1L);
                return;
            case DELETE:
                curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                return;
            case HEAD:
                curl_easy_setopt(_curl, CURLOPT_NOBODY, 1L);
                return;
            case OPTIONS:
                curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
                return;
            default:
                return;
        }
    }

    size_t HttpClient::on_response_data(const void *data, size_t length) {
        if (*_callback != nullptr) {
            bool is_continue = (*_callback)(data, length);
            if (!is_continue) {
                return -1;
            }
        }
        return length;
    }

    Status HttpClient::execute_post_request(const std::string &payload, std::string *response) {
        set_method(POST);
        set_payload(payload);
        return execute(response);
    }

    Status HttpClient::execute_delete_request(const std::string &payload, std::string *response) {
        set_method(DELETE);
        set_payload(payload);
        return execute(response);
    }


    Status HttpClient::execute(std::string *response) {
        auto callback = [response](const void* data, size_t length) {
            response ->append((char*)data, length);
            return true;
        };
        return execute(callback);
    }

    Status HttpClient::execute(const std::function<bool(const void *, size_t)> &callback) {
        _callback = &callback;
        CURLcode code = curl_easy_perform(_curl);
        if (code != CURLE_OK) {
            return Status::InternalError(_to_errmsg(code));
        }
        return Status::OK();
    }

    const char* HttpClient::_to_errmsg(CURLcode code) {
        if (_error_buf[0] == 0) {
            return curl_easy_strerror(code);
        }
        return _error_buf;
    }

    Status HttpClient::execute_with_retry(int retry_times, int sleep_time, const std::function<Status(HttpClient *)> &callback) {
        Status status;
        for (int i = 0; i < retry_times; ++i) {
            HttpClient client;
            status = callback(&client);
            if (status.ok()) {
                return status;
            }
            sleep(sleep_time);
        }
        return status;
    }


    Status HttpClient::download(const std::string &local_path) {

        // set method to GET
        set_method(GET);
        // limit speed
        curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_LIMIT, download_low_speed_limit_kbps * 1024);
        curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, download_low_speed_time);
        curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, max_download_speed_kbps * 1024);

        auto fp_closer = [](FILE* fp) { fclose(fp); };
        std::unique_ptr<FILE, decltype(fp_closer)> fp(fopen(local_path.c_str(), "w"), fp_closer);
        if (fp == nullptr) {
            return Status::InternalError("open file failed");
        }

        Status status;
        auto callback = [&status, &fp, &local_path] (const void* data, size_t length) {
            auto res = fwrite(data, length, 1, fp.get());
            if (res != 1) {
                status = Status::InternalError("fail to write data when download");
                return false;
            }
            return true;
        };
        execute(callback);
        return status;

    }








}

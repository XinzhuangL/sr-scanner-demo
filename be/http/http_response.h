//
// Created by lixinzhuang1 on 2024/6/19.
//

#ifndef BE_HTTP_RESPONSE_H
#define BE_HTTP_RESPONSE_H

#include <map>
#include <string>
#include <vector>

#include "http/http_status.h"

namespace starrocks {
    class HttpResponse {
    public:
        // Only have to send status line
        HttpResponse(const HttpStatus& status);

        // status and content
        HttpResponse(const HttpStatus& status, const std::string* content);

        // status and content
        HttpResponse(const HttpStatus& status, const std::string& type, const std::string* content);

        // Add one header
        void add_header(const std::string& key, const std::string& value);

        const std::map<std::string, std::vector<std::string>>& headers() const { return _custom_headers; }

        const std::string* content() const { return _content; }
        const std::string& content_type() const { return _content_type; }
        HttpStatus status() const { return _status; }


    private:
        HttpStatus _status;
        std::string _content_type;
        const std::string* _content;
        std::map<std::string, std::vector<std::string>> _custom_headers;
    };
}





#endif//BE_HTTP_RESPONSE_H

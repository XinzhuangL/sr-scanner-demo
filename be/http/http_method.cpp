//
// Created by lixinzhuang1 on 2024/6/19.
//

#include "http/http_method.h"

#include <map>
#include <string>

namespace starrocks {

    static std::map<std::string, HttpMethod> s_method_by_desc = {
            {"GET", HttpMethod::GET},   {"PUT", HttpMethod::PUT},       {"POST", HttpMethod::POST},
            {"HEAD", HttpMethod::HEAD}, {"DELETE", HttpMethod::DELETE}, {"OPTIONS", HttpMethod::OPTIONS},
    };
    static std::map<HttpMethod, std::string> s_desc_by_method = {
            {HttpMethod::GET, "GET"},   {HttpMethod::PUT, "PUT"},       {HttpMethod::POST, "POST"},
            {HttpMethod::HEAD, "HEAD"}, {HttpMethod::DELETE, "DELETE"}, {HttpMethod::OPTIONS, "OPTIONS"},
    };

    HttpMethod to_http_method(const char* desc) {
        auto iter = s_method_by_desc.find(desc);
        if (iter == s_method_by_desc.end()) {
            return HttpMethod::UNKNOWN;
        }
        return iter->second;
    }

    std::string to_method_desc(const HttpMethod& method) {
        auto iter = s_desc_by_method.find(method);
        if (iter == s_desc_by_method.end()) {
            return "UNKNOWN";
        }
        return iter->second;
    }

} // namespace starrocks

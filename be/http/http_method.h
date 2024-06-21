//
// Created by lixinzhuang1 on 2024/6/19.
//

#ifndef BE_HTTP_METHOD_H
#define BE_HTTP_METHOD_H

#include <event2/http.h>
#include <string>

namespace starrocks {
    // Http method enumerate
    enum HttpMethod { GET, PUT, POST, DELETE, HEAD, OPTIONS, UNKNOWN };

    // Convert string to HttpMethod
    HttpMethod to_http_method(const char* method);

    inline HttpMethod to_http_method(evhttp_cmd_type type) {
        switch (type) {
            case EVHTTP_REQ_GET:
                return HttpMethod::GET;
            case EVHTTP_REQ_POST:
                return HttpMethod::POST;
            case EVHTTP_REQ_HEAD:
                return HttpMethod::HEAD;
            case EVHTTP_REQ_PUT:
                return HttpMethod::PUT;
            case EVHTTP_REQ_DELETE:
                return HttpMethod::DELETE;
            case EVHTTP_REQ_OPTIONS:
                return HttpMethod::OPTIONS;
            default:
                return HttpMethod::UNKNOWN;
        }
        return HttpMethod::UNKNOWN;
    }

    std::string to_method_desc(const HttpMethod& method);
}


#endif//BE_HTTP_METHOD_H

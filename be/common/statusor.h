//
// Created by lixinzhuang1 on 2024/6/5.
//

#ifndef BE_STATUSOR_H
#define BE_STATUSOR_H

namespace starrocks {
    template <typename T>
    class StatusOr {
    public:
        StatusOr<T>() {};
        T& value();
    private:
        T data_;
    };
    template <typename T>
    T& StatusOr<T>::value() {
        return this->data_;
    }
}

#endif//BE_STATUSOR_H

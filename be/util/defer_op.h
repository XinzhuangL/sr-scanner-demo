//
// Created by lixinzhuang1 on 2024/6/19.
//

#ifndef BE_DEFER_OP_H
#define BE_DEFER_OP_H

#include <functional>
#include <utility>

namespace starrocks {
    // This class is used to defer a function when this object is deconstruct
    template <class DeferFunction>
    class DeferOp {
    public:
        explicit DeferOp(DeferFunction func) : _func(std::move(func)) {}
        ~DeferOp() {_func();}
    private:
        DeferFunction _func;
    };

}

#endif//BE_DEFER_OP_H

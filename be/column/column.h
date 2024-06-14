//
// Created by lixinzhuang1 on 2024/6/13.
//
#pragma once
#ifndef BE_COLUMN_H
#define BE_COLUMN_H
#include <memory>
class Column {
public:
    virtual size_t size() const = 0;

};

#endif//BE_COLUMN_H

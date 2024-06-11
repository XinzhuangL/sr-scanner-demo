//
// Created by lixinzhuang1 on 2024/6/7.
//

#ifndef BE_FS_UTIL_H
#define BE_FS_UTIL_H

#include "common/status.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>


namespace starrocks::fs {

    std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    void writeFile(const std::string& filePath, const std::string& content) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        file << content;
    }

    void appendToFile(const std::string& filePath, const std::string& content) {
        std::ofstream file(filePath, std::ios_base::app);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }
        file << content;
    }

    bool fileExists(const std::string& filePath) {
        return std::filesystem::exists(filePath);
    }

    bool deleteFile(const std::string& filePath) {
        if (!std::filesystem::remove(filePath)) {
            throw std::runtime_error("Could not delete files: " + filePath);
        }
    }

    std::uintmax_t getFileSize(const std::string& filePath) {
        if (!fileExists(filePath)) {
            throw std::runtime_error("File does not exist: " + filePath);
        }
        return std::filesystem::file_size(filePath);
    }

    void createDirectory(const std::string& directoryPath) {
        if (!std::filesystem::create_directories(directoryPath)) {
            throw std::runtime_error("Could not create directory: " + directoryPath);
        }
    }

    void deleteDirectory(const std::string& directoryPath) {
        if (!std::filesystem::remove_all(directoryPath)) {
            throw new std::runtime_error("Could not delete directory: " + directoryPath);
        }
    }

    bool directoryExists(const std::string& directoryPath) {
        return std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath);
    }

    std::vector<std::string> listDirectory(const std::string& directoryPath) {
        if (!directoryExists(directoryPath)) {
            throw std::runtime_error("Directory does not exist: " + directoryPath);
        }
        std::vector<std::string> entries;
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            entries.push_back(entry.path().string());
        }
        return entries;
    }

    std::uintmax_t getDirectorySize(const std::string& directoryPath) {
        if (!directoryExists(directoryPath)) {
            throw std::runtime_error("Directory does not exist: " + directoryPath);
        }
        std::uintmax_t size = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
            if (std::filesystem::is_regular_file(entry.status())) {
                size += std::filesystem::file_size(entry.path());
            }
        }
        return size;
    }


    bool ends_with(const std::string& value, const std::string& ending) {
        if (ending.size() > value.size()) {
            return false;
        }
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }





}


#endif//BE_FS_UTIL_H

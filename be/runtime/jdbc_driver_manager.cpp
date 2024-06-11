//
// Created by lixinzhuang1 on 2024/6/6.
//

#include "jdbc_driver_manager.h"
#include "fs/fs_util.h"
//#include "fmt/format.h"
#include <utility>
#include <string>
#include <string_view>


#include <atomic>
#include <cstring>

namespace starrocks {
    struct JDBCDriverEntry {
        JDBCDriverEntry(std::string name_, std::string checksum_)
            : name(std::move(name_)), checksum(std::move(checksum_)) {}
        ~JDBCDriverEntry();

        inline bool is_expected(const std::string &name, const std::string &checksum) {
            return this->name == name && this->checksum == checksum;
        }


        std::string name;
        std::string checksum;

        int64_t first_access_ts = INT64_MAX;

        // ${driver_dir}/${name}_${checksum}_${first_access_ts}.jar
        std::string location;

        std::atomic<bool> is_available{false};
        std::atomic<bool> should_delete{false};

        std::mutex download_lock;
        bool is_downloaded = false;
    };

    JDBCDriverEntry::~JDBCDriverEntry() {
        if (should_delete.load()) {
            // todo remove file in location
            // fs::remove(location)
        }
    }

    JDBCDriverManager::JDBCDriverManager() = default;

    JDBCDriverManager::~JDBCDriverManager() {
        std::unique_lock<std::mutex> l(_lock);
        _entry_map.clear();
    }

    JDBCDriverManager *JDBCDriverManager::getInstance() {
        static std::unique_ptr<JDBCDriverManager> manager;
        if (manager == nullptr) {
            manager = std::make_unique<JDBCDriverManager>();
        }
        return manager.get();
    }

    Status JDBCDriverManager::init(const std::string &driver_dir) {
        std::unique_lock<std::mutex> l(_lock);
        _driver_dir = driver_dir;
        // todo return if err
        fs::createDirectory(_driver_dir);

        // todo return if err
        std::vector<std::string> driver_files = fs::listDirectory(_driver_dir);
        // load jdbc drivers from file
        for (auto &file: driver_files) {
            std::string target_file = _driver_dir + "/" + file;
            // remove all temporary files
            if (fs::ends_with(file, TMP_FILE_SUFFIX)) {
                // todo log
                fs::deleteFile(target_file);
                continue;
            }

            // try to load drivers from jar file
            if (fs::ends_with(file, JAR_FILE_SUFFIX)) {
                std::string name;
                std::string checksum;
                int64_t first_access_ts;
                if (!_parse_from_file_name(file, &name, &checksum, &first_access_ts)) {
                    // todo log err
                    fs::deleteFile(target_file);
                    continue;
                }

                JDBCDriverEntryPtr entry;
                auto iter = _entry_map.find(name);
                // not found
                if (iter == _entry_map.end()) {
                    entry == std::make_shared<JDBCDriverEntry>(name, checksum);
                    entry->first_access_ts = first_access_ts;
                    entry->location = target_file;
                    entry->is_downloaded = true;
                    entry->is_available.store(true);
                    _entry_map[name] = entry;
                    // log info load jdbc driver from file ...
                } else {
                    entry = iter->second;
                    // replace old with new and delete the old driver
                    if (first_access_ts > entry->first_access_ts) {
                        // delete the old one
                        entry->should_delete.store(true);
                        // create the new one
                        entry = std::make_shared<JDBCDriverEntry>(name, checksum);
                        entry->first_access_ts = first_access_ts;
                        entry->location = target_file;
                        entry->is_downloaded = true;
                        entry->is_available.store(true);
                        _entry_map[name] = entry;
                        // todo log load
                    } else {
                        // this driver is old, just remove
                        // info
                        fs::deleteFile(target_file);
                    }
                }
            }
        }

        return Status::OK();
    }

    Status JDBCDriverManager::get_driver_location(const std::string &name, const std::string &url, const std::string &checksum, std::string *location) {
        JDBCDriverEntryPtr entry;
        {
            using namespace std::chrono;
            std::lock_guard<std::mutex> l(_lock);
            auto iter = _entry_map.find(name);
            if (iter == _entry_map.end()) {
                entry == std::make_shared<JDBCDriverEntry>(name, checksum);
                entry -> first_access_ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                entry -> location = _generate_driver_location(entry -> name, entry -> checksum, entry -> first_access_ts);
                _entry_map[name] = entry;
            } else {
                entry = iter -> second;
            }
            if (entry ->is_expected(name, checksum)) {
                if (entry -> is_available.load()) {
                    // log info
                    *location = entry -> location;
                    return Status::OK();
                }
            } else {
                // checksum mismatch, replace old with new
                // mark the old one for deletion
                entry -> should_delete.store(true);
                // create a new one
                entry = std::make_shared<JDBCDriverEntry>(name, checksum);
                entry -> first_access_ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                entry -> location = _generate_driver_location(entry -> name, entry -> checksum, entry -> first_access_ts);
                _entry_map[name] = entry;
            }
        }
        _download_driver(url, entry);
        *location = entry -> location;
        return Status::OK();
    }

    Status JDBCDriverManager::_download_driver(const std::string &url, starrocks::JDBCDriverManager::JDBCDriverEntryPtr &entry) {
        // todo impl
    }

    bool JDBCDriverManager::_parse_from_file_name(std::string_view file_name, std::string *name, std::string *checksum, int64_t *first_access_ts) {
        // remove '.jar' suffix
        file_name.remove_suffix(std::strlen(JAR_FILE_SUFFIX));
        // parse first_access_ts
        size_t pos = file_name.find_last_of('_');
        if (pos == std::string::npos) {
            return false;
        }
        std::string str = std::string(file_name.substr(pos + 1));
        try {
            // 将字符串转换为长整型
            *first_access_ts = std::stol(str);

        } catch (std::exception& e) {
            return false;
        }

        // parse checksum
        file_name.remove_suffix(str.size() + 1);
        pos = file_name.find_last_of('_');
        if (pos == std::string::npos) {
            return false;
        }
        str = std::string(file_name.substr(pos + 1));
        *checksum = str;

        // parse name
        file_name.remove_suffix(str.size() + 1);
        pos = file_name.find_last_of('_');
        if (pos == std::string::npos) {
            return false;
        }
        str = std::string(file_name);
        *name = str;
        return true;
    }

    std::string JDBCDriverManager::_generate_driver_location(const std::string &name, const std::string &checksum, int64_t first_access_ts) {
        return _driver_dir + '/' + name + '_' + checksum + '_' + std::to_string(first_access_ts) + ".jar";
    }

}// namespace starrocks

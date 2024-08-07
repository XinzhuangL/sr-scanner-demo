cmake_minimum_required(VERSION 3.22)
project(be)

include_directories(include ${PROJECT_SOURCE_DIR})


set(CMAKE_CXX_STANDARD 17)

set(JAVA_HOME /Users/lixinzhuang1/Library/Java/JavaVirtualMachines/corretto-1.8.0_412/Contents/Home)
#set(JAVA_HOME /var/local/thirdparty/installed/open_jdk)

message(STATUS "JAVA_HOME is set to ${JAVA_HOME}")

# GOOGLE TEST

enable_testing()
# This project is tested using GoogleTest.
add_subdirectory("third_party/googletest")
include_directories(${PROJECT_SOURCE_DIR}/third_party/googletest/googlemock/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include)

# gflag
include_directories(/opt/homebrew/Cellar/gflags/2.2.2/include)
find_library(GFLAG_LIBRARY gflags PATHS /opt/homebrew/Cellar/gflags/2.2.2/lib)
if (NOT GFLAG_LIBRARY)
    message(FATAL_ERROR "GFLAG library not found")
endif ()

# glog
include_directories(/opt/homebrew/Cellar/glog/0.6.0/include)
find_library(GLOG_LIBRARY glog PATHS /opt/homebrew/Cellar/glog/0.6.0/lib)
if (NOT GLOG_LIBRARY)
    message(FATAL_ERROR "GLOG library not found")
endif ()


# 包含 JNI 头文件目录
include_directories(${JAVA_HOME}/include)
#include_directories(${JAVA_HOME}/include/linux)
include_directories(${JAVA_HOME}/include/darwin)

# 链接到 JNI 库
# find_library(JVM_LIBRARY jvm PATHS ${JAVA_HOME}/jre/lib/amd64/server)
find_library(JVM_LIBRARY jvm PATHS ${JAVA_HOME}/jre/lib/server)
if (NOT JVM_LIBRARY)
    message(FATAL_ERROR "JVM library not found")
endif ()


#包含openssl
include_directories(/opt/homebrew/Cellar/openssl@3/3.3.1/include)
find_library(OPENSSL_SSL_LIBRARY ssl PATHS /opt/homebrew/Cellar/openssl@3/3.3.1/lib)
find_library(OPENSSL_CRYPTO_LIBRARY crypto PATHS /opt/homebrew/Cellar/openssl@3/3.3.1/lib)
if (NOT OPENSSL_SSL_LIBRARY OR NOT OPENSSL_CRYPTO_LIBRARY)
    message(FATAL_ERROR "OPEN_SSL library not found")
endif ()


#包含libevent
include_directories(/opt/homebrew/Cellar/libevent/2.1.12_1/include)
find_library(EVENT_LIBRARY event PATHS /opt/homebrew/Cellar/libevent/2.1.12_1/lib)
if (NOT EVENT_LIBRARY)
    message(FATAL_ERROR "EVENT library not found")
endif ()

#包含curl
include_directories(/opt/homebrew/Cellar/curl/8.8.0/include)
find_library(CURL_LIBRARY curl PATHS /opt/homebrew/Cellar/curl/8.8.0/lib)
if (NOT CURL_LIBRARY)
    message(FATAL_ERROR "CURL library not found")
endif ()

#包含boost
include_directories(/opt/homebrew/Cellar/boost/1.85.0/include)
find_library(BOOST_LIBRARY event PATHS /opt/homebrew/Cellar/boost/1.85.0/lib)
if (NOT BOOST_LIBRARY)
    message(FATAL_ERROR "BOOST library not found")
endif ()

#add_library(fmt STATIC IMPORTED)
#set_target_properties(fmt PROPERTIES IMPORTED_LOCATION ${THIRDPARTY_DIR}/lib64/libfmt.a)
#


add_executable(be main.cpp
        column/column_helper.cpp
        common/buffer.cpp
        datasource/jdbc_connector.cpp
        operator/connector_scan_operator.cpp
        operator/scan_operator.cpp
        runtime/jdbc_driver_manager.cpp
        scanner/jdbc_scanner.cpp
        source/chunk_source.cpp
        udf/java_udf.cpp
        mock_pipeline_driver.cpp
        util/md5.cpp
        util/download_util.cpp
        http/http_client.cpp
        http/http_headers.cpp
        http/http_method.cpp
        http/http_response.cpp
        http/http_status.cpp
runtime/runtime_state.cpp
        runtime/types.cpp
table/descriptors.cpp)


target_link_libraries(be gmock gtest gtest_main)

target_link_libraries(be ${JVM_LIBRARY})
target_link_libraries(be ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(be ${EVENT_LIBRARY})
target_link_libraries(be ${BOOST_LIBRARY})
target_link_libraries(be ${CURL_LIBRARY})
#target_link_libraries(be fmt)

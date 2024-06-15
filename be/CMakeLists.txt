cmake_minimum_required(VERSION 3.22)
project(be)

include_directories(include ${PROJECT_SOURCE_DIR})


set(CMAKE_CXX_STANDARD 17)

set(JAVA_HOME /Library/Java/JavaVirtualMachines/jdk-1.8.jdk/Contents/Home)
#set(JAVA_HOME /var/local/thirdparty/installed/open_jdk)

message(STATUS "JAVA_HOME is set to ${JAVA_HOME}")


# 包含 JNI 头文件目录
include_directories(${JAVA_HOME}/include)
#include_directories(${JAVA_HOME}/include/linux)
include_directories(${JAVA_HOME}/include/darwin)

# 链接到 JNI 库
# find_library(JVM_LIBRARY jvm PATHS ${JAVA_HOME}/jre/lib/amd64/server)
find_library(JVM_LIBRARY jvm PATHS ${JAVA_HOME}/jre/lib/server)
if(NOT JVM_LIBRARY)
    message(FATAL_ERROR "JVM library not found")
endif()



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
        mock_pipeline_driver.cpp)

target_link_libraries(be ${JVM_LIBRARY})
#target_link_libraries(be fmt)

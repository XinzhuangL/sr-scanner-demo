//
// Created by lixinzhuang1 on 2024/6/6.
//

#ifndef BE_JAVA_UDF_H
#define BE_JAVA_UDF_H

#include "common/status.h"

// implements by libhdfs
// hadoop-hdfs-native-client/src/main/native/libhdfs/jni_helper.c
// Why do we need to use this function?
// 1. a thread can not attach to more than one virtual machine
// 2. libhdfs depends on this function and does some initialization,
// if the JVM has already created it, it won't create it anymore.
// if we skip this function call will cause libhdfs to miss some initialization operations
extern "C" JNIEnv *getJNIEnv(void);

#define DEFINE_JAVA_PRIM_TYPE(TYPE) \
    jclass _class_##TYPE;           \
    jmethodID _value_of_##TYPE;     \
    jmethodID _val_##TYPE;

#define DECLARE_NEW_BOX(TYPE, CLAZZ) \
    jobject new##CLAZZ(TYPE value);  \
    TYPE val##TYPE(jobject obj);

namespace starrocks::vectorized {
    class JVMClass;
    class JavaGlobalRef;
    class DirectByteBuffer;

    // Restrictions on use:
    // can only be used in pthread, not in bthread
    // thread local helper
    class JVMFunctionHelper {
    public:
        static JVMFunctionHelper &getInstance();
        static std::pair<JNIEnv *, JVMFunctionHelper &> getInstanceWithEnv();
        JVMFunctionHelper(const JVMFunctionHelper &) = delete;

        // get env
        JNIEnv *getEnv() { return _env; }
        // Arrays.toString()
        std::string array_to_string(jobject object);
        // Object::toString()
        std::string to_string(jobject obj);
        std::string to_cxx_string(jstring str);
        std::string dumpExceptionString(jthrowable throwable);

        jmethodID getToStringMethod(jclass clazz);
        jstring to_jstring(const std::string &str);
        jmethodID getMethod(jclass clazz, const std::string &method, const std::string &sig);
        jmethodID getStaticMethod(jclass clazz, const std::string &method, const std::string &sig);
        // create a object array
        jobject create_array(int sz);


    private:
        inline static thread_local JNIEnv *_env;

        DEFINE_JAVA_PRIM_TYPE(boolean);
        DEFINE_JAVA_PRIM_TYPE(byte);
        DEFINE_JAVA_PRIM_TYPE(short);
        DEFINE_JAVA_PRIM_TYPE(int);
        DEFINE_JAVA_PRIM_TYPE(long);
        DEFINE_JAVA_PRIM_TYPE(float);
        DEFINE_JAVA_PRIM_TYPE(double);

        jclass _object_class;
        jclass _object_array_class;
        jclass _string_class;
        jclass _throwable_class;
        jclass _jarrays_class;
        jclass _list_class;

        jmethodID _string_construct_with_bytes;

        // List method
        jmethodID _list_get;
        jmethodID _list_size;

        jobject _utf8_charsets;

        jclass _udf_helper_class;
        jmethodID _create_boxed_array;
        jmethodID _batch_update;
        jmethodID _batch_update_if_not_null;
        jmethodID _batch_update_state;
        jmethodID _batch_call;
        jmethodID _batch_call_no_args;
        jmethodID _init_batch_call;
        jmethodID _get_boxed_result;
        jclass _direct_buffer_class;
        jmethodID _direct_buffer_clear;

        JVMClass *_function_states_clazz = nullptr;
    };

    // A global ref of the guard, handle can be shared across threads
    class JavaGlobalRef {
    public:
        JavaGlobalRef(jobject handle) : _handle(handle) {};
        ~JavaGlobalRef();
        JavaGlobalRef(const JavaGlobalRef&) =delete;

        JavaGlobalRef(JavaGlobalRef&& other) {
            _handle = other._handle;
            other._handle = nullptr;
        }

        JavaGlobalRef& operator=(JavaGlobalRef&& other) {
            JavaGlobalRef tmp(std::move(other));
            std::swap(this -> _handle, tmp._handle);
            return *this;
        }

        jobject handle() const { return _handle; }
        jobject& handle() { return _handle; }
        void clear();

    private:
        jobject _handle;
    };

    // A Class object created from the ClassLoader that can be accessed by multiple threads
    class JVMClass {
    public:
        JVMClass(jobject clazz) : _clazz(clazz) {}
        JVMClass(const JVMClass&) = delete;

        JVMClass& operator=(const JVMClass&&) = delete;
        JVMClass& operator=(const JVMClass& other) = delete;

        JVMClass(JVMClass&& other) : _clazz(nullptr) { _clazz = std::move(other._clazz); }

        JVMClass& operator=(JVMClass&& other) {
            JVMClass tmp(std::move(other));
            std::swap(this->_clazz, tmp._clazz);
            return *this;
        }

        jclass clazz() const { return (jclass)_clazz.handle(); }

        // Create a new instance using the default constructor
        StatusOr<JavaGlobalRef> newInstance() const;

    private:
        JavaGlobalRef _clazz;
    };



    Status detect_java_runtime();
}// namespace starrocks::vectorized

#endif//BE_JAVA_UDF_H

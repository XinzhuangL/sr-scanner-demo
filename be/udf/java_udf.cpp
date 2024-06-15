//
// Created by lixinzhuang1 on 2024/6/11.
//

#include "udf/java_udf.h"
#include <sstream>

JNIEnv* getJNIEnv() {
    JavaVM** jvm = nullptr;
    JNIEnv* env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];
    // 当前目录作为类路径
    char str[] = "-Djava.class.path=/Users/lixinzhuang1/IdeaProjects/sr-scanner-demo/fe/target/starrocks-jdbc-bridge-jar-with-dependencies.jar";
    options[0].optionString = str;
    // 设置 JNI版本
    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_FALSE;
    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
    if (ret < 0) {
        std::cerr << "Unable to launch JVM" << std::endl;
    }
    return env;
}


namespace starrocks::vectorized {

    JVMFunctionHelper& JVMFunctionHelper::getInstance() {
        if (_env == nullptr) {
            _env = getJNIEnv();
          //  CHECK(_env != nullptr) << "couldn't got a JNIEnv";
        }
        static JVMFunctionHelper helper;
        return helper;
    }


    JavaGlobalRef::~JavaGlobalRef() {
       // clear();
    }


    std::string JVMFunctionHelper::dumpExceptionString(jthrowable throwable) {
        std::stringstream ss;
        // toString
        jmethodID toString = getToStringMethod(_throwable_class);
        ss << to_string(throwable);

        // e.getStackTrace()
        jmethodID getStackTrace = _env ->GetMethodID(_throwable_class, "getStackTrace", "()[Ljava/lang/StackTraceElement;");
        jobject stack_traces = _env ->CallObjectMethod((jobject)throwable, getStackTrace);

        ss << array_to_string(stack_traces);
        return ss.str();
    }

    int JVMFunctionHelper::list_size(jobject obj) {
        return static_cast<int>(_env->CallIntMethod(obj, _list_size));
    }

    jobject JVMFunctionHelper::list_get(jobject obj, int idx) {
        return _env->CallObjectMethod(obj, _list_get, idx);
    }

    void JavaGlobalRef::clear() {
   // todo impl
    }

    std::string JVMFunctionHelper::array_to_string(jobject object) {
        _env->ExceptionClear();
        std::string value;
        jmethodID arrayToStringMethod =
                _env->GetStaticMethodID(_jarrays_class, "toString", "([Ljava/lang/Object;)Ljava/lang/String;");
        //DCHECK(arrayToStringMethod != nullptr);
        jobject jstr = _env->CallStaticObjectMethod(_jarrays_class, arrayToStringMethod, object);
       // LOCAL_REF_GUARD(jstr);
       // CHECK_FUNCTION_EXCEPTION(_env, "array_to_string")
        value = to_cxx_string((jstring)jstr);
        return value;
    }

    std::string JVMFunctionHelper::to_string(jobject obj) {
        _env->ExceptionClear();
        std::string value;
        auto method = getToStringMethod(_object_class);
        auto res = _env->CallObjectMethod(obj, method);
       // LOCAL_REF_GUARD(res);
       // CHECK_FUNCTION_EXCEPTION(_env, "to_string")
        value = to_cxx_string((jstring)res);
        return value;
    }

    jmethodID JVMFunctionHelper::getToStringMethod(jclass clazz) {
        return _env->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
    }

    std::string JVMFunctionHelper::to_cxx_string(jstring str) {
        std::string res;
        const char* charflow = _env->GetStringUTFChars((jstring)str, nullptr);
        res = charflow;
        _env->ReleaseStringUTFChars((jstring)str, charflow);
        return res;
    }

    void JVMFunctionHelper::_init() {
        // todo
    }
    Status detect_java_runtime() {
        const char* p = std::getenv("JAVA_HOME");
        if (p == nullptr) {
            return Status::InternalError("env 'JAVA_HOME' is not set");
        }
        return Status::OK();
    }

    StatusOr<JavaGlobalRef> JVMClass::newInstance() const {
        JNIEnv* env = getJNIEnv();
        // get default constructor
        jmethodID constructor = env ->GetMethodID((jclass)_clazz.handle(), "<init>", "()V");
        if (constructor == nullptr) {
            return Status::InternalError("");
        }
        auto local_ref = env ->NewObject((jclass)_clazz.handle(), constructor);
        JavaGlobalRef javaGlobalRef(env ->NewGlobalRef(local_ref));
        return javaGlobalRef; // 使用std::move以避免复制
    }
}

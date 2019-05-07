#include <jni.h>
#include <android/log.h>
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"bigman",__VA_ARGS__)
extern "C"
JNIEXPORT void JNICALL
Java_com_example_administrator_ndkstudydemo_NativeUtils_accessJavaClassProp(JNIEnv *env,
jobject instance) {


    jclass jclazz=env->GetObjectClass(instance);
    jfieldID  fid=env->GetFieldID(jclazz, "prop", "I");
    jint prop = env->GetIntField(instance, fid);
    prop+=101;
    env->SetIntField(instance,fid,prop);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_administrator_ndkstudydemo_NativeUtils_callJavaClassMethod(JNIEnv *env,
                                                                            jobject instance) {

    // TODO
    jclass  jclazz=env->GetObjectClass(instance);
    jmethodID  mid=env->GetMethodID(jclazz,"getStringFromJava","(Ljava/lang/String;)Ljava/lang/String;");
    const char* fromc="Hello Java, I am From C.";
    jobject jstr= (env->CallObjectMethod(instance, mid, env->NewStringUTF(fromc)));
    const char* cstr= (env->GetStringUTFChars(static_cast<jstring>(jstr), 0));
    LOGD("%s",cstr);
    (env)->ReleaseStringUTFChars(static_cast<jstring>(jstr), cstr);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_administrator_ndkstudydemo_NativeUtils_accessStaticJavaProp(JNIEnv *env,
                                                                             jclass jclazz) {

    jfieldID fid=env->GetStaticFieldID(jclazz,"staticProp","I");
    jint prop=env->GetStaticIntField(jclazz,fid);
    prop+=101;
    // TODO
    env->SetStaticIntField(jclazz,fid,prop);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_administrator_ndkstudydemo_NativeUtils_callStaticJavaMethod(JNIEnv *env,
                                                                             jclass jclazz) {

    // TODO
    // TODO

    jmethodID  mid=env->GetStaticMethodID(jclazz,"getStringFromJavaStatic","(Ljava/lang/String;)Ljava/lang/String;");
    const char* fromc="Hello Java, I am From C.";
    jobject jstr= (env->CallStaticObjectMethod(jclazz, mid, env->NewStringUTF(fromc)));
    const char* cstr= (env->GetStringUTFChars(static_cast<jstring>(jstr), 0));
    LOGD("%s",cstr);
    (env)->ReleaseStringUTFChars(static_cast<jstring>(jstr), cstr);

}
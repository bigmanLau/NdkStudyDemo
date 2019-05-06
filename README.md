# NdkStudyDemo
生成so和a库，然后主项目进行引用
# 在 NDK 开发中引入第三方静态库和动态库


![image](https://upload-images.jianshu.io/upload_images/3239933-deeb1fc27d6b8e3e.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/960/format/webp)


# 静态库和动态库

## 静态库

静态库文件后缀一般为 .a ，利用静态库编译成的可执行文件通常比较大，编译后的执行程序不需要外部函数库的支持。但是，如果一个程序依赖的静态库发生改变，那么这个程序就需要重新编译。

## 动态库

动态库也称为共享库，动态库文件后缀一般为 .so (Linux) 或 .dll (Windows) ，相对于静态库，动态库在编译时, 并没有被编译进目标代码中，程序执行到相关函数时才调用动态库里的相关函数，动态库编译后所产生的可执行文件通常较小。由于动态库没有被整合进程序，而是程序运行时动态地申请并调用，因此程序的运行环境中必须提供相应的库。动态库的改变并不影响程序，便于升级。

## 两种函数库特点

静态库：编译后的执行程序不需要外部的函数库支持，编译过程中已经被载入可执行程序,程序运行时将不再需要该静态库；
动态库：动态库的代码在程序运行时才载入内存，而编译过程中仅简单的引用，代码体积较小，并且动态库与程序代码独立，可复用，耦合度低；

# 生成第三方动态库和静态库

本文主要通过 CMake 工具构建，请确保 AS 版本在 2.2 以上。本节简单生成一个动态库和静态库，模仿我们要引入的第三方库文件。
新建一个工程，选择 Support C++ ，文件结构：

![image](https://upload-images.jianshu.io/upload_images/3239933-2263d53e4e335c5a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/525/format/webp)

文件 haohao/haohao.h ，定义一个简单的结构体和一个简单的类。

```
//
// Created by haohao on 2017/12/14.
//

#ifndef NDKLIB_HAOHAO_H
#define NDKLIB_HAOHAO_H
#include <stddef.h>
#include <android/log.h>
#include <jni.h>
#include <string>

#define  LOG_E(...)  __android_log_print(ANDROID_LOG_ERROR,"HaoHao",__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    std::string (*getResponse)();
} HaoHao_FUN;

class HaoHao{
public:
    HaoHao(std::string, int);
    std::string getHaoHaoWord();

private:
    std::string name;
    int age;

};

extern __attribute__ ((visibility ("default"))) HaoHao_FUN haohao_fun;

#ifdef __cplusplus
}
#endif
#endif //NDKLIB_HAOHAO_H

```

haohao/haohao.cpp ，实例化一个结构体。

```
#include "haohao.h"

HaoHao::HaoHao(std::string name, int age) {
    this->name = name;
    this->age = age;
}

std::string HaoHao::getHaoHaoWord() {
    char tem[3];
    sprintf(tem, "%d", age);
    return "Hello, My name is " + name + " I am " + tem + " years old";
}

std::string get_response(){
    HaoHao hao = HaoHao("chary", 23);
    return hao.getHaoHaoWord();
}

__attribute__ ((visibility ("default"))) HaoHao_FUN haohao_fun = {
     get_response
};

```

haohao/CMakeLists.txt

```
cmake_minimum_required(VERSION 3.4.1)
add_library(haohao STATIC haohao.cpp ) // 添加为静态库

find_library(log-lib  log )

target_link_libraries(haohao ${log-lib} )

```

nannan/nannan.h ，定义一个简单的结构体，提供给其他程序使用。

```
#ifndef NDKLIB_NANNAN_H
#define NDKLIB_NANNAN_H
#include <stddef.h>
#include <android/log.h>
#include <jni.h>
#include <string>

#define  LOG_E(...)  __android_log_print(ANDROID_LOG_ERROR,"NanNan",__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    std::string (*getResponse)();

    std::int32_t (*getNanNanKey)();
} NANNAN_FUN;

extern __attribute__ ((visibility ("default"))) NANNAN_FUN nannan_fun;

#ifdef __cplusplus
}
#endif
#endif //NDKLIB_NANNAN_H

```

nannan/nannan.cpp

```
#include "nannan.h"

std::string get_nannan_response(){
    LOG_E("Nannan get response");
    return "Hello, My name is Nannan!";
}

std::int32_t get_nannan_age(){
    LOG_E("Nannan get age");
    return 23;
}

__attribute__ ((visibility ("default"))) NANNAN_FUN nannan_fun = {
        get_nannan_response,
        get_nannan_age
};

```

nannan/CmakeLists.txt

```
cmake_minimum_required(VERSION 3.4.1)

add_library(nannan SHARED nannan.cpp ) // 添加为动态库

find_library(log-lib  log )

target_link_libraries(nannan ${log-lib} )

```

根目录下的 CMakeLists.txt

```
cmake_minimum_required(VERSION 3.4.1)
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp)
set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++14 -pthread -DMGULK_LOG_STDERR=1 -Wall -Wextra -Wnon-virtual-dtor -g")

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp/haohao)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp/nannan)

```

app/build.gradle

![image](https://upload-images.jianshu.io/upload_images/3239933-6b04eb22ad241cf1.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/820/format/webp)

make 工程，在 app/src/main/jniLibs 目录下生成了我们需要的第三方库文件。

![image](https://upload-images.jianshu.io/upload_images/3239933-0a0060da770eda3f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/524/format/webp)

# 引入第三方库

新建一个工程，将上节生成的第三方库文件拷贝到 app/src/main/jniLibs 目录下，在 cpp 目录下新建 include 目录，拷贝第三方库的头文件在 include 目录下。
工程目录

![image](https://upload-images.jianshu.io/upload_images/3239933-94353d219e381439.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/614/format/webp)

MainActivity.java

```
package com.haohao.ndklib;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}

```

native/native.cpp ，调用第三方库提供的 API 。

```
#include <jni.h>
#include <string>
#include <nannan.h>
#include <haohao.h>

extern "C"
{
JNIEXPORT jstring JNICALL
Java_com_haohao_ndklib_MainActivity_stringFromJNI(JNIEnv
                                                  *env, jobject instance) {

    char age[3];
    sprintf(age, "%d", nannan_fun.getNanNanKey());

    std::string value = nannan_fun.getResponse() + " I am " + age + " years old.\n";
    value += haohao_fun.getResponse();
    //std::string value = "Hello";
    return env->NewStringUTF(value.c_str());
}
}

```

native/CMakeLists.txt 文件。

```
add_library(native SHARED native.cpp)

# 引入 .so 文件
add_library(nannan SHARED IMPORTED )
set_target_properties(nannan PROPERTIES IMPORTED_LOCATION "${jnilibs}/${ANDROID_ABI}/libnannan.so")

# 引入 .a 文件
add_library(haohao STATIC IMPORTED )
set_target_properties(haohao PROPERTIES IMPORTED_LOCATION "${jnilibs}/${ANDROID_ABI}/libhaohao.a")

find_library( log-lib log )

target_link_libraries(native nannan haohao ${log-lib})

```

根目录下的 CMakeLists.txt 文件。

```
cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)
set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++14 -pthread -DMGULK_LOG_STDERR=1 -Wall -Wextra -Wnon-virtual-dtor -g")

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp/native)

```

app/build.gradle 文件配置。

```
apply plugin: 'com.android.application'

android {
    compileSdkVersion 25
    buildToolsVersion "25.0.3"
    defaultConfig {
        applicationId "com.haohao.ndklib"
        minSdkVersion 15
        targetSdkVersion 25
        versionCode 1
        versionName "1.0"
        testInstrumentationRunner "android.support.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                arguments '-DANDROID_PLATFORM=android-15',
                        '-DANDROID_TOOLCHAIN=clang', '-DANDROID_STL=gnustl_static'
//                cppFlags "-DANDROID_STL=c++_static"
                abiFilters 'armeabi-v7a','x86_64', 'arm64-v8a','x86'
            }
        }

    }
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }

    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"
        }
    }
}

dependencies {
    compile fileTree(dir: 'libs', include: ['*.jar'])
    androidTestCompile('com.android.support.test.espresso:espresso-core:2.2.2', {
        exclude group: 'com.android.support', module: 'support-annotations'
    })
    compile 'com.android.support:appcompat-v7:25.3.1'
    testCompile 'junit:junit:4.12'
    compile 'com.android.support.constraint:constraint-layout:1.0.2'
}

```

build 工程后，生成 libnative.so ，直接运行项目，安装 apk 。

![image](https://upload-images.jianshu.io/upload_images/3239933-2ac2685285efce1a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/720/format/webp)

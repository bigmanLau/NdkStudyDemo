NDK POSIX 多线程编程
96  字节流动 
 0.6 2019.01.27 15:42* 字数 705 阅读 4555评论 0喜欢 2
 人价值的高低取决于稀缺性，即不可代替性。
该文章首发于微信公众号“字节流动”

本博客 NDK 开发系列文章：

NDK 编译的三种方式
NDK 开发中引入第三方静态库和动态库
NDK 开发中 Native 与 Java 交互
NDK POSIX 多线程编程
NDK Android OpenSL ES 音频采集与播放
NDK FFmpeg 编译
NDK FFmpeg 音视频解码
NDK 直播流媒体服务器搭建
NDK 直播推流与引流
NDK 开发中快速定位 Crash 问题
POSIX
POSIX 全称是 Portable Operating System Interface of UNIX ，表示可移植操作系统接口，本质上是一种编程标准。它定义了操作系统应该为应用程序提供的接口标准，是 IEEE 为要在各种 UNIX 操作系统上运行的软件而定义的一系列 API 标准的总称。

Pthreads
POSIX 线程是 POSIX 的线程标准，也称为 Pthreads ，它定义了创建和管理线程的一套 API 。本文的 Pthreads 是实现 POSIX 线程标准的 c 语言编程库。在 Linux 系统中，一般多线程的实现由 POSIX 多线程编程实现，而 Android 系统基于 Linux 系统，原生便支持 POSIX 多线程编程。

POSIX Linux 编译命令：gcc hello.c -o hello -lpthread，执行命令：./hello。

由于本文讲的是 NDK 开发，代码编译基于 Android 平台实现。

POSIX 线程创建
线程创建相关 API：
pthread_t 线程 id 。
pthread_create 负责创建线程，传入 pthread_t（线程 ID ）的指针，线程的执行方法和传入线程的参数。
pthread_join 使当前线程挂起，等待指定线程执行结束，并获取线程返回值。
pthread_exit 退出当前线程，并且可以设置当前线程的返回值。
pthread_cancel 终止当前线程。

#include <stdlib.h>
#include <unistd.h>
#include "hello-thread.h"
#define NUM_THREADS 5

// 类似于 Java Runnable
void *run(void *arg){
    // 取传入当前线程的参数
    char *thread_tag = (char*)arg;

    for (int i = 0; i < 5; ++i) {
        LOGD("%s thread %d", thread_tag, i);
        if (i == 4) {
            // 结束当前线程,参数为线程结束后的返回值
            pthread_exit(thread_tag);
            //pthread_cancel(); send a cancellation request to a thread
        }
    }

    return 0; // 线程正常执行完成后的返回值
}

void create_threads(){
    LOGD("Main thread");
    char tag_arr[][5] = {"No.1","No.2","No.3","No.4","No.5"};

    //线程 id ，用于区分线程，一个线程对应一个唯一的 id
    pthread_t tids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        // 创建线程，指定 run 方法，传入参数 tags[i]
        pthread_create(&tids[i], NULL, run, (void *) tag_arr[i]);
    }

    void *return_val[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        // 阻塞当前线程，等待指定 tid 的线程结束，并获取线程返回值
        // join with a terminated thread
        pthread_join(tids[i], &return_val[i]);
        LOGD("thread %s terminated.", (char*)return_val[i]);
    }
}
运行结果：

I/hello-thread: Main thread
I/hello-thread: No.1 thread 0
I/hello-thread: No.1 thread 1
I/hello-thread: No.1 thread 2
I/hello-thread: No.1 thread 3
I/hello-thread: No.1 thread 4
I/hello-thread: No.2 thread 0
I/hello-thread: No.2 thread 1
I/hello-thread: No.2 thread 2
I/hello-thread: No.2 thread 3
I/hello-thread: No.2 thread 4
I/hello-thread: No.3 thread 0
I/hello-thread: No.3 thread 1
I/hello-thread: No.3 thread 2
I/hello-thread: No.3 thread 3
I/hello-thread: No.3 thread 4
I/hello-thread: No.4 thread 0
I/hello-thread: No.4 thread 1
I/hello-thread: No.4 thread 2
I/hello-thread: No.4 thread 3
I/hello-thread: No.4 thread 4
I/hello-thread: thread No.1 terminated.
I/hello-thread: No.5 thread 0
I/hello-thread: No.5 thread 1
I/hello-thread: No.5 thread 2
I/hello-thread: No.5 thread 3
I/hello-thread: No.5 thread 4
I/hello-thread: thread No.2 terminated.
I/hello-thread: thread No.3 terminated.
I/hello-thread: thread No.4 terminated.
I/hello-thread: thread No.5 terminated.
POSIX 线程同步
线程同步相关 API :
pthread_mutex_t 互斥锁。
pthread_mutex_init 初始化互斥锁，需传入互斥锁的指针。
pthread_mutex_destroy 销毁互斥锁，需传入互斥锁的指针。
pthread_mutex_lock 加锁，需传入互斥锁的指针。
pthread_mutex_unlock 解锁，需传入互斥锁的指针。

#include <stdlib.h>
#include <unistd.h>
#include "hello-thread.h"
int g_count = 0;

// 互斥锁
pthread_mutex_t mutex;

void *asyn_run(void *arg){
    // lock
    pthread_mutex_lock(&mutex);

    // 取传入当前线程的参数
    char *thread_tag = (char*)arg;

    for (int i = 0; i < 10; ++i) {
        // 休眠 200 ms
        usleep(200 * 1000);
        g_count ++;
        LOGD("%s thread %d, g_count = %d", thread_tag, i, g_count);
    }

    // unlock
    pthread_mutex_unlock(&mutex);

    return thread_tag; // 线程正常执行完成后的返回值
}

void syn_thread(){
    LOGD("Main thread");

    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    pthread_t t1, t2;

    // 创建 2 个线程
    pthread_create(&t1, NULL, asyn_run, "No.1");
    pthread_create(&t2, NULL, asyn_run, "No.2");

    void *rtn_val[2];
    pthread_join(t1, &rtn_val[0]);
    pthread_join(t2, &rtn_val[1]);
    LOGD("thread %s terminated.", (char*)rtn_val[0]);
    LOGD("thread %s terminated.", (char*)rtn_val[1]);

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);

}
运行结果：

I/hello-thread: No.1 thread 0, g_count = 1
I/hello-thread: No.1 thread 1, g_count = 2
I/hello-thread: No.1 thread 2, g_count = 3
I/hello-thread: No.1 thread 3, g_count = 4
I/hello-thread: No.1 thread 4, g_count = 5
I/hello-thread: No.1 thread 5, g_count = 6
I/hello-thread: No.1 thread 6, g_count = 7
I/hello-thread: No.1 thread 7, g_count = 8
I/hello-thread: No.1 thread 8, g_count = 9
I/hello-thread: No.1 thread 9, g_count = 10
I/hello-thread: No.2 thread 0, g_count = 11
I/hello-thread: No.2 thread 1, g_count = 12
I/hello-thread: No.2 thread 2, g_count = 13
I/hello-thread: No.2 thread 3, g_count = 14
I/hello-thread: No.2 thread 4, g_count = 15
I/hello-thread: No.2 thread 5, g_count = 16
I/hello-thread: No.2 thread 6, g_count = 17
I/hello-thread: No.2 thread 7, g_count = 18
I/hello-thread: No.2 thread 8, g_count = 19
I/hello-thread: No.2 thread 9, g_count = 20
I/hello-thread: thread No.1 terminated.
I/hello-thread: thread No.2 terminated.
POSIX 线程间通信
线程间通信相关 API ：
pthread_cond_t 条件变量，条件变量是线程同步的一种手段，使线程可以休眠等待某种条件出现。
pthread_cond_signal 发送一个信号给另外一个正在处于阻塞等待状态的线程，原本这两个线程竞争同一个 mutex lock 。
pthread_cond_wait 使当前线程处于阻塞状态，直到接收到其他线程发送对应的 cond_signal 。
pthread_cond_init 初始化条件变量。
pthread_cond_destroy 销毁条件变量。

#include <stdlib.h>
#include <unistd.h>
#include "hello-thread.h"
// 共享数据
volatile int shared_count = 0;
pthread_mutex_t pthread_mutex;

// 条件变量
pthread_cond_t pthread_cond;

void *producer(void *arg){
    char *tag = (char*)arg;
    for (;;) {
        pthread_mutex_lock(&pthread_mutex);

        // 生产者生产产品
        shared_count ++;
        LOGD("%s thread 生产产品, count = %d", tag, shared_count);

        // 通知消费者线程消费
        pthread_cond_signal(&pthread_cond);

        pthread_mutex_unlock(&pthread_mutex);

        // 休眠 200 ms
        usleep(200 * 1000);
    }

    return (void*)0;
}

void *consumer(void *arg){
    char* tag = (char*)arg;
    for(;;){
        pthread_mutex_lock(&pthread_mutex);

        while (shared_count == 0){
            // 当没有产品可以消费时，等待生产者生产（等待条件变量被唤醒，当前线程释放互斥锁）
            // 当被其他线程唤醒时，解除阻塞状态，重新申请获得互斥锁
            pthread_cond_wait(&pthread_cond, &pthread_mutex);

        }

        shared_count --;
        LOGD("%s thread 消费产品, count = %d", tag, shared_count);

        pthread_mutex_unlock(&pthread_mutex);

        // 休眠 500 ms
        usleep(500 * 1000);
    }

    return (void*)0;

};

// 线程间通信
void communicate_thread(){
    pthread_mutex_init(&pthread_mutex, NULL);

    // 初始化条件变量
    pthread_cond_init(&pthread_cond, NULL);

    // 线程 id
    pthread_t producer_tid, consumer_tid;

    // 创建生产者线程
    pthread_create(&producer_tid, NULL, producer, "producer");
    // 创建消费者线程
    pthread_create(&consumer_tid, NULL, consumer, "consumer");

    // 等待线程结束
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    // 销毁互斥锁
    pthread_mutex_destroy(&pthread_mutex);
    // 销毁条件变量
    pthread_cond_destroy(&pthread_cond);

}
运行结果:

I/hello-thread: producer thread 生产产品, count = 1
I/hello-thread: consumer thread 消费产品, count = 0
I/hello-thread: producer thread 生产产品, count = 1
I/hello-thread: producer thread 生产产品, count = 2
I/hello-thread: consumer thread 消费产品, count = 1
I/hello-thread: producer thread 生产产品, count = 2
I/hello-thread: producer thread 生产产品, count = 3
I/hello-thread: consumer thread 消费产品, count = 2
I/hello-thread: producer thread 生产产品, count = 3
I/hello-thread: producer thread 生产产品, count = 4
I/hello-thread: producer thread 生产产品, count = 5
I/hello-thread: consumer thread 消费产品, count = 4
I/hello-thread: producer thread 生产产品, count = 5
I/hello-thread: producer thread 生产产品, count = 6
I/hello-thread: consumer thread 消费产品, count = 5
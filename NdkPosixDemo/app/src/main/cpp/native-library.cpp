#include <jni.h>
#include <pthread.h>
#include <zconf.h>
#include "Algorithm.h"
#include "Algorithm.cpp"

#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"HaoHao",__VA_ARGS__)
#define NUM_THREADS 5

void create_threads();
void syn_thread();
void communicate_thread();

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_administrator_ndkstudydemo_MainActivity_doProcess(JNIEnv *env, jobject,
                                                                   jstring input_) {
    const char *input = env->GetStringUTFChars(input_, 0);

    LOGCATE("MainActivity_doProcess input = %s", input);
//    create_threads();
//    syn_thread();
    communicate_thread();
    (env)->ReleaseStringUTFChars(input_, input);

    return (env)->NewStringUTF(input);
}

void *run(void *arg){
    char *thread_tag= static_cast<char *>(arg);
    for (int i = 0; i < 5; ++i) {
        LOGD("%s thread %d", thread_tag, i);
        if(i==4){
            pthread_exit(thread_tag);
        }
    }
    return 0;
}

void create_threads() {
    LOGD("Main thread");
    char tag_arr[][5] = {"NO.1", "NO.2", "NO.3", "NO.4", "NO.5"};
    void *return_val[NUM_THREADS];
    pthread_t tids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        // 创建线程，指定 run 方法，传入参数 tags[i]
        pthread_create(&tids[i], NULL, run, (void *) tag_arr[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        // 阻塞当前线程，等待指定 tid 的线程结束，并获取线程返回值
        // join with a terminated thread
        pthread_join(tids[i], &return_val[i]);
        LOGD("thread %s terminated.", (char*)return_val[i]);
    }
}

int g_count=0;
pthread_mutex_t mutex;

void *asyn_run(void *arg){
    pthread_mutex_lock(&mutex);

    char *thread_tag= static_cast<char *>(arg);

    for (int i = 0; i < 10; ++i) {
        usleep(200*1000);
        g_count++;
        LOGD("%s thread %d, g_count = %d", thread_tag, i, g_count);
    }

    pthread_mutex_unlock(&mutex);
    return  thread_tag;
}

void syn_thread(){
    LOGD("Main thread");
    pthread_mutex_init(&mutex,NULL);

    pthread_t  t1,t2;

    pthread_create(&t1, NULL, asyn_run, (void *) "NO.1");
    pthread_create(&t2, NULL, asyn_run, (void *) "NO.2");

    void *rtn_val[2];
    pthread_join(t1,&rtn_val[0]);
    pthread_join(t2,&rtn_val[1]);
    LOGD("thread %s terminated.", (char*)rtn_val[0]);
    LOGD("thread %s terminated.", (char*)rtn_val[1]);

    pthread_mutex_destroy(&mutex);
}

volatile int shared_count=0;
pthread_mutex_t pthread_mutex;

pthread_cond_t pthread_cond;

void* producer(void *arg){
    char *tag= static_cast<char *>(arg);
    for (;  ; ) {
        pthread_mutex_lock(&mutex);

        shared_count++;
        LOGD("%s thread 生产产品, count = %d", tag, shared_count);

        pthread_cond_signal(&pthread_cond);

        pthread_mutex_unlock(&mutex);

        usleep(200*1000);
    }
    return 0;
}

void* consumer(void *arg){
    char* tag = (char*)arg;
    for(;;){
        pthread_mutex_lock(&mutex);
        while(shared_count==0){
            pthread_cond_wait(&pthread_cond,&mutex);
        }
        shared_count --;
        LOGD("%s thread 消费产品, count = %d", tag, shared_count);
        pthread_mutex_unlock(&mutex);
        usleep(200*1000);

    }
    return  0;
}

//线程间通信
void communicate_thread(){
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&pthread_cond,NULL);


    pthread_t p_tid,c_tid;
    pthread_create(&p_tid, NULL, producer, (void *) "producer");
    pthread_create(&c_tid, NULL, consumer, (void *) "consumer");


    pthread_join(p_tid,NULL);
    pthread_join(c_tid,NULL);

    pthread_cond_destroy(&pthread_cond);
    pthread_mutex_destroy(&mutex);
}
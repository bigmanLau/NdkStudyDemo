#ifndef NDKSAMPLE_ALGORITHM_H
#define NDKSAMPLE_ALGORITHM_H
#include<android/log.h>
#define  LOGCATE(...)  __android_log_print(ANDROID_LOG_ERROR,"HaoHao",__VA_ARGS__)

class Algorithm
{
public:
    Algorithm();
    ~Algorithm();

    int Init();
    int UnInit();

    int Process(const char* input, char* output);

};
#endif //NDKSAMPLE_ALGORITHM_H
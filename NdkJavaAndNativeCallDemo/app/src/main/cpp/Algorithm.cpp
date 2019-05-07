#include <cstring>
#include "Algorithm.h"

Algorithm::Algorithm()
{
    LOGCATE("Algorithm::Algorithm()");
}

Algorithm::~Algorithm()
{
    LOGCATE("Algorithm::~Algorithm()");
}

int Algorithm::Init()
{
    LOGCATE("Algorithm::Init()");
    return 0;
}

int Algorithm::UnInit()
{
    LOGCATE("Algorithm::Init()");
    return 0;
}

int Algorithm::Process(const char *input, char *output)
{
    LOGCATE("Algorithm::Process()");
    // 简单拼接，没有检查指针
    strcpy(output, input);
    strcat(output, "Process Done.");
    return 0;
}
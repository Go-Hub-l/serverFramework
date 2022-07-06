#ifndef __SYLAR_UTIL__
#define __SYLAR_UTIL__
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <vector>
#include <string>

namespace sylar {
    uint32_t getThreadId();
    uint64_t getFiberId();
    void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
    uint64_t GetCurrentMS();

/**
 * @brief 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
 */
    std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");
}

#endif
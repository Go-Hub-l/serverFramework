
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <execinfo.h>
#include <dirent.h>
#include <string.h>

#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <google/protobuf/unknown_field_set.h>

#include "util.h"
#include "log.h"
#include "fiber.h"
#include "sylar.h"
#include "fiber.h"



namespace sylar {

    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

    uint32_t getThreadId() {

    return syscall(SYS_gettid);
}

uint64_t getFiberId() {

    sylar::Fiber::GetFiberId();
    return 0;
}

uint64_t GetCurrentMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

void Backtrace(std::vector<std::string>& bt, int size, int skip) {
    void** array = (void**) malloc((sizeof(void*) * size));
    size_t s = ::backtrace(array, size);

    char** strings = backtrace_symbols(array, s);
    if (strings == NULL) {
        SYLAR_LOG_ERROR(g_logger) << "backtrace_synbols error";
        free(array);
        return;
    }

    for (size_t i = skip; i < s; ++i) {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

}

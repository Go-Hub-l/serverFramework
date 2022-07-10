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
#include <sys/stat.h>
#include <fcntl.h> 

namespace sylar {
    uint32_t getThreadId();
    uint64_t getFiberId();
    void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
    uint64_t GetCurrentMS();
    uint64_t GetCurrentUS();

    class FSUtil {
    public:
        // static void ListAllFile(std::vector<std::string>& files
        //                         ,const std::string& path
        //                         ,const std::string& subfix);
        // static bool Mkdir(const std::string& dirname);
        // static bool IsRunningPidfile(const std::string& pidfile);
        // static bool Rm(const std::string& path);
        // static bool Mv(const std::string& from, const std::string& to);
        // static bool Realpath(const std::string& path, std::string& rpath);
        // static bool Symlink(const std::string& frm, const std::string& to);
        // static bool Unlink(const std::string& filename, bool exist = false);
        // static std::string Dirname(const std::string& filename);
        // static std::string Basename(const std::string& filename);
        // static bool OpenForRead(std::ifstream& ifs, const std::string& filename
        //                 ,std::ios_base::openmode mode);
        // static bool OpenForWrite(std::ofstream& ofs, const std::string& filename
        //                 ,std::ios_base::openmode mode);
    };

/**
 * @brief 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
 */
    std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");
}

#endif
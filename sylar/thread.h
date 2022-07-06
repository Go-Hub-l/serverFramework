#ifndef __MYTHREAD_H__
#define __MYTHREAD_H__

#include <functional>
#include <thread>
#include "mutex.h"

namespace sylar {

    

    class Thread {
    public:
        //线程智能指针
        typedef std::shared_ptr<Thread> ptr;
        
        Thread(std::function<void()> cb, const std::string& name);
        ~Thread();
        pid_t getId() const { return m_id; }
        const std::string& getName() const { return m_name; }

        int join();
        //获取当前线程指针
        static Thread* GetThis();
        //获取当前线程名称
        static const std::string& GetName();
        //设置当前线程名称
        static void SetName(const std::string& name);
    private:
        static void* run(void*);//线程运行函数

    private:
        pid_t m_id = -1;//暂时不懂啥意思
        pthread_t m_threadId = 0;//线程id
        std::string m_name;//线程名
        std::function<void()> m_cb;

        Semaphore m_semaphore;
        Mutex m_mutex;
    };
}

#endif
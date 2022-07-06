#include "thread.h"
#include "log.h"
#include "util.h"


namespace sylar {

    static thread_local Thread* t_thread = nullptr;
    static thread_local std::string t_thread_name = "UNKNOW";

    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

    Thread::Thread(std::function<void()> cb, const std::string& name)
        :m_name(name)
        , m_cb(cb) {
        if (name.empty()) {
            m_name = "UNKNOW";
        }
        //创建线程
        int res = pthread_create(&m_threadId, nullptr, &Thread::run, this);
        if (res) {
            SYLAR_LOG_ERROR(g_logger) << "pthread create thread failed, res=" << res <<
                "   name="<< m_name;
            throw std::logic_error("pthread_create error");
        }

        m_semaphore.wait();/**创建完一个线程后
                             在线程的初始化工作未完成时先把主线程挂起
                             工作线程的初始化工作完成之后再唤醒主线程*/
    }
    Thread::~Thread() {
        //将线程设置成分离态：自动回收线程
        if (m_threadId) {
            pthread_detach(m_threadId);
        }
    }

    int Thread::join() {
        if (m_threadId) {
            int res = pthread_join(m_threadId, nullptr);
            if (res) {
                SYLAR_LOG_ERROR(g_logger) << "pthread join failed, res=" << res <<
                    "   name="<< m_name;
                throw std::logic_error("pthread_join error");
            }
            m_threadId = 0;
            return res;
        }

        return -1;
    }
    void* Thread::run(void* arg) {
        Thread* thread = (Thread*) (arg);
        //设置线程局部变量
        t_thread = thread;
        t_thread_name = thread->m_name;
        thread->m_id = sylar::getThreadId();
        //设置线程名
        pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
        
        

        std::function<void()> cb;
        //与传过来的回调函数交换
        cb.swap(thread->m_cb);
        //唤醒挂起的主线程
        thread->m_semaphore.notify();
        //当前线程执行回调任务
        cb();
        return 0;
    }

    //获取当前线程指针
    Thread* Thread::GetThis() {
        return t_thread;
    }
    //获取当前线程名称
    const std::string& Thread::GetName() {
        return t_thread_name;
    }
    //设置当前线程名称
    void Thread::SetName(const std::string& name) {
        if (name.empty()) {
            return;
        }
        if (t_thread) {
            t_thread->m_name = "UNKNOW";
        }
        t_thread_name = name;
    }


    
}

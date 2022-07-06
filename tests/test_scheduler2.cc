/**
 * @file simple_fiber_scheduler.cc
 * @brief 一个简单的协程调度器实现
 * @version 0.1
 * @date 2021-07-10
 */

#include "sylar.h"
sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
/**
 * @brief 简单协程调度类，支持添加调度任务以及运行调度任务
 */
class Scheduler {
public:
    /**
     * @brief 添加协程调度任务
     */
    void schedule(sylar::Fiber::ptr task) {
        m_tasks.push_back(task);
    }
    /**
     * @brief 执行调度任务
     */
    void run() {
        sylar::Fiber::ptr task;
        auto it = m_tasks.begin();
        while (it != m_tasks.end()) {
            task = *it;
            m_tasks.erase(it++);
            task->GetThis();
            std::cout << "Scheduler::run()::first call" << std::endl;
            task->call();
            std::cout << "Scheduler::run()::second call" << std::endl;
            task->call();
        }
    }

private:
    /// 任务队列
    std::list<sylar::Fiber::ptr> m_tasks;
};

void test_fiber(int i) {
    std::cout << "test_fiber()::hello world " << i << std::endl;
    sylar::Fiber::ptr cur = sylar::Fiber::GetThis();
    cur->back();
    std::cout << "test_fiber()::back fiber " << i << std::endl;
}

int main() {
    g_logger->setLevel(sylar::LogLevel::INFO);
    /// 初始化当前线程的主协程
    sylar::Fiber::GetThis();

    /// 创建调度器
    Scheduler sc;

    /// 添加调度任务
    for (auto i = 0; i < 10; i++) {
        sylar::Fiber::ptr fiber(new sylar::Fiber(
            std::bind(test_fiber, i), 0, true
        ));
        sc.schedule(fiber);
    }
    /// 执行调度任务
    sc.run();

    return 0;
}

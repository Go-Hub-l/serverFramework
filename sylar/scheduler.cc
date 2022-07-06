#include "scheduler.h"
#include "log.h"
#include "macro.h"
#include "hook.h"

namespace sylar {

    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

    static thread_local Scheduler* t_scheduler = nullptr;
    static thread_local Fiber* t_scheduler_fiber = nullptr;

    Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
        :m_name(name) {
        SYLAR_ASSERT(threads > 0);

        if (use_caller) {
            sylar::Fiber::GetThis();
            --threads;

            SYLAR_ASSERT(GetThis() == nullptr);
            t_scheduler = this;

            m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
            sylar::Thread::SetName(m_name);

            t_scheduler_fiber = m_rootFiber.get();
            m_rootThread = sylar::getThreadId();//注：后边统一把这个首字母改大写
            m_threadIds.push_back(m_rootThread);
        } else {
            m_rootThread = -1;
        }
        m_threadCount = threads;
    }

    Scheduler::~Scheduler() {
        SYLAR_ASSERT(m_stopping);
        if (GetThis() == this) {
            t_scheduler = nullptr;
        }
    }

    Scheduler* Scheduler::GetThis() {
        return t_scheduler;
    }

    Fiber* Scheduler::GetMainFiber() {
        return t_scheduler_fiber;
    }

    void Scheduler::start() {
        MutexType::Lock lock(m_mutex);
        //如果已经启动，直接返回
        if (!m_stopping) {
            return;
        }
        //运行状态时m_stopping标志设置为false
        m_stopping = false;
        //刚开始运行时，数组m_threads记录的线程为空：断言一下
        SYLAR_ASSERT(m_threads.empty());
        //将数组空间重置为传过来的空间
        m_threads.resize(m_threadCount);
        //以传过来的线程数为依据，创建threads个数个线程（m_threads为线程池，存放线程）
        for (size_t i = 0; i < m_threadCount; ++i) {
            m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
                , m_name + "_" + std::to_string(i)));
            //将创建好的线程的线程ID添加到线程ID中统一管理
            m_threadIds.push_back(m_threads[i]->getId());
        }
        //解锁
        lock.unlock();

        //if(m_rootFiber) {
        //    //m_rootFiber->swapIn();
        //    m_rootFiber->call();
        //    SYLAR_LOG_INFO(g_logger) << "call out " << m_rootFiber->getState();
        //}
    }

    void Scheduler::stop() {
        m_autoStop = true;
        if (m_rootFiber
            && m_threadCount == 0
            && (m_rootFiber->getState() == Fiber::TERM
                || m_rootFiber->getState() == Fiber::INIT)) {
            SYLAR_LOG_INFO(g_logger) << this << " stopped";
            m_stopping = true;

            if (stopping()) {
                return;
            }
        }

        //bool exit_on_this_fiber = false;
        if (m_rootThread != -1) {
            SYLAR_ASSERT(GetThis() == this);
        } else {
            SYLAR_ASSERT(GetThis() != this);
        }

        m_stopping = true;
        for (size_t i = 0; i < m_threadCount; ++i) {
            tickle();
        }

        if (m_rootFiber) {
            tickle();
        }

        if (m_rootFiber) {
            //while(!stopping()) {
            //    if(m_rootFiber->getState() == Fiber::TERM
            //            || m_rootFiber->getState() == Fiber::EXCEPT) {
            //        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
            //        SYLAR_LOG_INFO(g_logger) << " root fiber is term, reset";
            //        t_fiber = m_rootFiber.get();
            //    }
            //    m_rootFiber->call();
            //}
            if (!stopping()) {
                m_rootFiber->call();
            }
        }

        std::vector<Thread::ptr> thrs;
        {
            MutexType::Lock lock(m_mutex);
            thrs.swap(m_threads);
        }
        //回收线程资源
        for (auto& i : thrs) {
            i->join();
        }
        //if(exit_on_this_fiber) {
        //}
    }

    void Scheduler::setThis() {
        t_scheduler = this;
    }

    void Scheduler::run() {
        SYLAR_LOG_DEBUG(g_logger) << m_name << " run";
        //set_hook_enable(true);
        //设置当前协程为调度线程
        setThis();
        //如果当前的线程ID不和根线程相等：任意线程都是调度线程
        if (int(sylar::getThreadId()) != m_rootThread) {
            //该线程的调度协程取当前协程的主协程
            t_scheduler_fiber = Fiber::GetThis().get();
            SYLAR_LOG_DEBUG(g_logger) << "t_scheduler_fiber: " << t_scheduler << " thread [" << sylar::getThreadId() << "]";
        }
        
        //无任务时运行idle线程
        Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
        //每个协程的回调函数
        Fiber::ptr cb_fiber;
        //线程协程的无参构造
        FiberAndThread ft;
        //每个线程的运行空间
        while (true) {
            //每次执行完一次任务，就将先前执行的记录情况清空，以便下一次执行任务
            ft.reset();
            bool tickle_me = false;
            bool is_active = false;
            //大括号的作用是为了限制锁的生命周期
            {
                MutexType::Lock lock(m_mutex);
                //从协程池中取协程任务执行
                auto it = m_fibers.begin();
                while (it != m_fibers.end()) {
                    //协程指定线程执行：如果协程指定的线程不是当前线程就跳过，取下一个协程任务
                    if (it->thread != -1 && it->thread != int(sylar::getThreadId())) {
                        ++it;
                        //指出有一个协程待处理
                        tickle_me = true;
                        continue;
                    }
                    //断言取出的协程或协程执行函数有一个非空
                    SYLAR_ASSERT(it->fiber || it->cb);
                    //如果协程存在并且已经是执行状态：跳过
                    if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
                        ++it;
                        continue;
                    }
                    //当前线程执行协程任务
                    ft = *it;
                    //将取出的协程任务从协程任务队列中删除
                    m_fibers.erase(it++);
                    //待执行的协程数加一
                    ++m_activeThreadCount;
                    is_active = true;
                    break;
                }
                //如果有未执行的协程任务或者协程队列还未遍历完
                tickle_me |= it != m_fibers.end();
            }
            //取出一个协程后处理该协程
            if (tickle_me) {//scheduler那里调用了tickle()，这里也调用了，同一个协程任务好像是tickle执行了两次
                tickle();
            }
            //待处理的协程的状态不是TERM或者EXCEPT，就处理该协程
            if (ft.fiber && (ft.fiber->getState() != Fiber::TERM
                && ft.fiber->getState() != Fiber::EXCEPT)) {
                //该线程调度该协程执行
                ft.fiber->swapIn();
                //协程任务执行完毕后（或者主动让出CPU时）重新回到该位置，将待执行的协程任务减-
                --m_activeThreadCount;
                //如果协程的状态时就绪状态：重新调度执行
                if (ft.fiber->getState() == Fiber::READY) {
                    schedule(ft.fiber);
                } else if (ft.fiber->getState() != Fiber::TERM
                    && ft.fiber->getState() != Fiber::EXCEPT) {//如果协程的状态不是TERM或者EXCEPT（即是HOLD状态）
                    //将协程状态设置为HOLD状态
                    ft.fiber->m_state = Fiber::HOLD;
                }
                //当前协程调度并执行完后，在切换到下一个协程前需要将线程协程之前的数据重置
                ft.reset();
            } else if (ft.cb) {//如果协程的回调函数非空
                if (cb_fiber) {//这个是指Fiber类的成员方法
                    cb_fiber->reset(ft.cb);
                } else {//协程的回调函数：此时应该是在创建协程时  这个是指设置智能指针的指向
                    cb_fiber.reset(new Fiber(ft.cb));
                }
                //协程处理完重置
                ft.reset();
                //调度当前协程
                SYLAR_LOG_DEBUG(g_logger) << "[ft.cb] testLog m_fibers size=" << m_fibers.size();
                cb_fiber->swapIn();
                //当前协程成功调度后将可调度的协程减一
                --m_activeThreadCount;
                if (cb_fiber->getState() == Fiber::READY) {
                    schedule(cb_fiber);//调度函数  有疑问  为什么reset?
                    cb_fiber.reset();
                } else if (cb_fiber->getState() == Fiber::EXCEPT
                    || cb_fiber->getState() == Fiber::TERM) {
                    cb_fiber->reset(nullptr);//协程函数执行结束
                } else {//if(cb_fiber->getState() != Fiber::TERM) {
                    cb_fiber->m_state = Fiber::HOLD;
                    cb_fiber.reset();
                }
            } else {//说明是已经执行的协程
                if (is_active) {
                    --m_activeThreadCount;
                    continue;
                }
                //协程执行结束
                if (idle_fiber->getState() == Fiber::TERM) {
                    SYLAR_LOG_INFO(g_logger) << "idle fiber term";
                    break;
                }
                //空闲的协程数
                ++m_idleThreadCount;
                //SYLAR_LOG_DEBUG(g_logger) << "[Null] testLog m_fibers size=" << m_fibers.size();
                //空闲的协程调度
                idle_fiber->swapIn();
                //空闲的协程数
                --m_idleThreadCount;
                //idle协程执行结束
                if (idle_fiber->getState() != Fiber::TERM
                    && idle_fiber->getState() != Fiber::EXCEPT) {
                    idle_fiber->m_state = Fiber::HOLD;
                }
            }
        }
    }
    //协程任务的处理函数
    void Scheduler::tickle() {
        SYLAR_LOG_INFO(g_logger) << "tickle";
    }

    bool Scheduler::stopping() {
        MutexType::Lock lock(m_mutex);
        return m_autoStop && m_stopping
            && m_fibers.empty() && m_activeThreadCount == 0;
    }

    void Scheduler::idle() {
        SYLAR_LOG_INFO(g_logger) << "idle";
        while (!stopping()) {
            sylar::Fiber::YieldToHold();
        }
    }

    // void Scheduler::switchTo(int thread) {
    //     SYLAR_ASSERT(Scheduler::GetThis() != nullptr);
    //     if (Scheduler::GetThis() == this) {
    //         if (thread == -1 || thread == sylar::GetThreadId()) {
    //             return;
    //         }
    //     }
    //     schedule(Fiber::GetThis(), thread);
    //     Fiber::YieldToHold();
    // }

    // std::ostream& Scheduler::dump(std::ostream& os) {
    //     os << "[Scheduler name=" << m_name
    //         << " size=" << m_threadCount
    //         << " active_count=" << m_activeThreadCount
    //         << " idle_count=" << m_idleThreadCount
    //         << " stopping=" << m_stopping
    //         << " ]" << std::endl << "    ";
    //     for (size_t i = 0; i < m_threadIds.size(); ++i) {
    //         if (i) {
    //             os << ", ";
    //         }
    //         os << m_threadIds[i];
    //     }
    //     return os;
    // }

    // SchedulerSwitcher::SchedulerSwitcher(Scheduler* target) {
    //     m_caller = Scheduler::GetThis();
    //     if (target) {
    //         target->switchTo();
    //     }
    // }

    // SchedulerSwitcher::~SchedulerSwitcher() {
    //     if (m_caller) {
    //         m_caller->switchTo();
    //     }
    // }

}

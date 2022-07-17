#ifndef __SYLAR_MUTEX_H__
#define __SYLAR_MUTEX_H__

#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <stdexcept>
#include "noncopyable.h"
#include "fiber.h"
#include <list>
//#include <thread>

namespace sylar {
    class Semaphore {
    public:
        Semaphore(uint32_t count = 0);
        ~Semaphore();
        void wait();
        void notify();
    private:
        Semaphore(Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore operator=(Semaphore&) = delete;
    private:
        sem_t m_semaphore;
    };


    template<class T>
    class ReadScopedLockImpl {
    public:
        ReadScopedLockImpl(T& mutex)
            :m_mutex(mutex) {
            m_mutex.rdlock();
            m_bLock = true;
        }
        ~ReadScopedLockImpl() {
            unlock();
        }
        void lock() {
            if (!m_bLock) {
                m_mutex.rdlock();
                m_bLock = true;
            }
        }

        void unlock() {
            if (m_bLock) {
                m_mutex.unlock();
                m_bLock = false;
            }
        }
    private:
        T& m_mutex;
        bool m_bLock;

    };

    template<class T>
    class WriteScopedLockImpl {
    public:
        WriteScopedLockImpl(T& mutex)
            :m_mutex(mutex) {
            m_mutex.wrlock();
            m_bLock = true;
        }
        ~WriteScopedLockImpl() {
            unlock();
        }
        void lock() {
            if (!m_bLock) {
                m_mutex.wrlock();
                m_bLock = true;
            }
        }

        void unlock() {
            if (m_bLock) {
                m_mutex.unlock();
                m_bLock = false;
            }
        }
    private:
        T& m_mutex;
        bool m_bLock;

    };

    template<class T>
    class ScopedLockImpl {
    public:
        ScopedLockImpl(T& mutex)
            :m_mutex(mutex) {
            m_mutex.lock();
            m_bLock = true;
        }
        ~ScopedLockImpl() {
            unlock();
        }
        void lock() {
            if (!m_bLock) {
                m_mutex.lock();
                m_bLock = true;
            }
        }

        void unlock() {
            if (m_bLock) {
                m_mutex.unlock();
                m_bLock = false;
            }
        }
    private:
        T& m_mutex;
        bool m_bLock;
    };

    class Mutex {
    public:
        typedef ScopedLockImpl<Mutex> Lock;
        Mutex() {
            pthread_mutex_init(&m_mutex, nullptr);
        }
        ~Mutex() {
            pthread_mutex_destroy(&m_mutex);
        }
        void lock() {
            pthread_mutex_lock(&m_mutex);
        }
        void unlock() {
            pthread_mutex_unlock(&m_mutex);
        }
    private:
        pthread_mutex_t m_mutex;
    };

    class NullMutex {
    public:
        typedef ScopedLockImpl<NullMutex> Lock;
        NullMutex() {}
        ~NullMutex() {}
        void lock() {}
        void unlock() {}
    };

    class RWMutex {
    public:
        typedef ReadScopedLockImpl<RWMutex> ReadLock;
        typedef WriteScopedLockImpl<RWMutex> WriteLock;
        RWMutex() {
            if (pthread_rwlock_init(&m_lock, nullptr)) {
                throw std::logic_error("pthread rwlock init error");
            }
        }

        ~RWMutex() {
            pthread_rwlock_destroy(&m_lock);
        }
        void rdlock() {
            if (pthread_rwlock_rdlock(&m_lock)) {
                throw std::logic_error("pthread rwlock rdlock error");
            }
        }

        void wrlock() {
            if (pthread_rwlock_wrlock(&m_lock)) {
                throw std::logic_error("pthread rwlock wrlock error");
            }
            //pthread_rwlock_wrlock(&m_lock);
        }

        void unlock() {
            pthread_rwlock_unlock(&m_lock);
        }
    private:
        pthread_rwlock_t m_lock;
    };

    class NullRWMutex {
    public:
        typedef ScopedLockImpl<NullRWMutex> Lock;
        NullRWMutex() {}
        ~NullRWMutex() {}
        void rdlock() {}
        void wrlock() {}
        void unlock() {}
    };

    class SpinLock {
    public:
        typedef ScopedLockImpl<SpinLock> Lock;
        SpinLock() {
            pthread_spin_init(&m_mutex, 0);
        }
        ~SpinLock() {
            pthread_spin_destroy(&m_mutex);
        }
        void lock() {
            pthread_spin_lock(&m_mutex);
        }
        void unlock() {
            pthread_spin_unlock(&m_mutex);
        }
    private:
        pthread_spinlock_t m_mutex;
    };

    class Scheduler;
    class FiberSemaphore : Noncopyable {
    public:
        typedef SpinLock MutexType;

        FiberSemaphore(size_t initial_concurrency = 0);
        ~FiberSemaphore();

        bool tryWait();
        void wait();
        void notify();

        size_t getConcurrency() const { return m_concurrency; }
        void reset() { m_concurrency = 0; }
    private:
        MutexType m_mutex;
        std::list<std::pair<Scheduler*, Fiber::ptr> > m_waiters;
        size_t m_concurrency;
    };
}
#endif
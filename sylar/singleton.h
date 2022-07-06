#ifndef __SYLAR_SINGLETON_H__
#define __SYLAR_SINGLETON_H__

namespace sylar {

template <class T, class X = void, int N = 0>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

template <class T, class X = void, int N = 0>
class Singletonptr {
public:
    static T* GetInstance() {
        std::shared_ptr<T> l(new T);
        return l;
    }
};

}
#endif
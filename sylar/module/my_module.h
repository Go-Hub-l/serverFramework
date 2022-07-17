#ifndef __MY_MODULE_H__
#define __MY_MODULE_H__

//#include <memory>
#include "module.h"

namespace chat {
    class MyModule : public sylar::Module {
    public:
        typedef std::shared_ptr<MyModule> ptr;

        MyModule();
        bool onLoad() override;
        bool onUnload() override;
        bool onServerReady() override;
        bool onServerUp() override;
    //     static void main();
    // private:
    //     static void run();
    };
}


#endif
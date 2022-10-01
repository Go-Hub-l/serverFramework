#include "http/http_server.h"
#include "my_module.h"
#include "log.h"
#include "resource_servlet.h"
#include "env.h"
#include "http/ws_server.h"
#include "chat_servlet.h"
#include "application.h"

namespace chat {

    static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

    MyModule::MyModule()
        :sylar::Module("chat_room", "1.0", "") {

    }

// #define XX(...) #__VA_ARGS__

//     sylar::IOManager::ptr worker;
//     //调度的主运行函数
//     void MyModule::run() {
//         // g_logger->setLevel(sylar::LogLevel::INFO);
//         //         //sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(true, worker.get(), sylar::IOManager::GetThis()));
//         // sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(true));
//         // sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("172.26.85.62:8088");
//         // while (!server->bind(addr)) {
//         //     sleep(2);
//         // }
//         // auto sd = server->getServletDispatch();
//         // sylar::http::ResourceServlet::ptr slt(new sylar::http::ResourceServlet(
//         //     sylar::EnvMgr::GetInstance()->getCwd()
//         // ));
//         // //设置访问/sylar/*路径，就执行后面跟的回调函数或者servlet类的handle函数
//         // SYLAR_LOG_INFO(g_logger) << "addServlet";
//         // sd->addGlobServlet("/html/*", slt);

//         // std::vector<sylar::TcpServer::ptr> srvs;
//         // sylar::TcpServer::ptr server;
//         // server.reset(new sylar::http::WSServer(sylar::IOManager::GetThis(),
//         //     sylar::IOManager::GetThis(),
//         //     sylar::IOManager::GetThis()));

//         //服务器开始监听
//         //server->start();



//         std::vector<sylar::TcpServer::ptr> svrs;
//         if (!sylar::Application::GetInstance()->getServer("http", svrs)) {
//             SYLAR_LOG_INFO(g_logger) << "no httpserver alive";
//             return;
//         }

//         // for (auto& i : svrs) {
//         //     sylar::http::HttpServer::ptr http_server =
//         //         std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
//         //     if (!i) {
//         //         continue;
//         //     }
//         //     auto slt_dispatch = http_server->getServletDispatch();

//         //     sylar::http::ResourceServlet::ptr slt(new sylar::http::ResourceServlet(
//         //         sylar::EnvMgr::GetInstance()->getCwd()
//         //     ));
//         //     slt_dispatch->addGlobServlet("/html/*", slt);
//         //     SYLAR_LOG_INFO(g_logger) << "addServlet";
//         // }

//         // svrs.clear();
//         // if (!sylar::Application::GetInstance()->getServer("ws", svrs)) {
//         //     SYLAR_LOG_INFO(g_logger) << "no ws alive";
//         //     return;
//         // }

//         // for (auto& i : svrs) {
//         //     sylar::http::WSServer::ptr ws_server =
//         //         std::dynamic_pointer_cast<sylar::http::WSServer>(i);

//         //     sylar::http::ServletDispatch::ptr slt_dispatch = ws_server->getWSServletDispatch();
//         //     ChatWSServlet::ptr slt(new ChatWSServlet);
//         //     slt_dispatch->addServlet("/sylar/chat", slt);
//         // }

//     }

    // void MyModule::main() {
    //     sylar::IOManager iom(2, true, "main");
    //     //worker.reset(new sylar::IOManager(3, false, "worker"));
    //     iom.schedule(run);
    // }

    bool MyModule::onLoad() {
        SYLAR_LOG_INFO(g_logger) << "onLoad";
        return true;
    }

    bool MyModule::onUnload() {
        SYLAR_LOG_INFO(g_logger) << "onUnLoad";
        return true;
    }

    bool MyModule::onServerReady() {
        SYLAR_LOG_INFO(g_logger) << "onServerReady";
        std::vector<sylar::TcpServer::ptr> svrs;
        if (!sylar::Application::GetInstance()->getServer("http", svrs)) {
            SYLAR_LOG_INFO(g_logger) << "no httpserver alive";
            return false;
        }

        for (auto& i : svrs) {
            sylar::http::HttpServer::ptr http_server =
                std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
            if (!i) {
                continue;
            }
            auto slt_dispatch = http_server->getServletDispatch();

            sylar::http::ResourceServlet::ptr slt(new sylar::http::ResourceServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            sylar::http::LoginServlet::ptr sltLogin(new sylar::http::LoginServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            slt_dispatch->addGlobServlet("/test", slt);
            slt_dispatch->addGlobServlet("/api/login", sltLogin);
            SYLAR_LOG_INFO(g_logger) << "addServlet";
        }
        return true;
    }

    bool MyModule::onServerUp() {
        SYLAR_LOG_INFO(g_logger) << "onServerUp";
        return true;
    }

    // extern "C" {//hook了，加载的时候到这里执行

        sylar::Module* MyModule::CreateModule() {
            sylar::Module* module = new chat::MyModule;
            SYLAR_LOG_INFO(chat::g_logger) << "CreateModule " << module;
            return module;
        }

        void MyModule::DestoryModule(sylar::Module* module) {
            SYLAR_LOG_INFO(chat::g_logger) << "CreateModule " << module;
            delete module;
        }
    // }
}
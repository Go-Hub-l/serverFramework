#include "http/http_server.h"
#include "my_module.h"
#include "log.h"
#include "resource_servlet.h"
#include "env.h"
#include "http/ws_server.h"
#include "chat_servlet.h"
#include "application.h"
#include "login_servlet.h"
#include "myfiles_servlet.h"
#include "reg_servlet.h"
#include "upload_servlet.h"
#include "md5_servlet.h"
#include "dealfile_servlet.h"
#include "sharefiles_servlet.h"
#include "dealsharefile_servlet.h"

namespace chat {

    static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

    MyModule::MyModule()
        :sylar::Module("chat_room", "1.0", "") {

    }

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
            sylar::http::RegServlet::ptr sltReg(new sylar::http::RegServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            sylar::http::MyfilesServlet::ptr sltMyfiles(new sylar::http::MyfilesServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            sylar::http::UploadServlet::ptr sltUpload(new sylar::http::UploadServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            sylar::http::MD5Servlet::ptr sltMD5(new sylar::http::MD5Servlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            sylar::http::DealfileServlet::ptr sltDealfile(new sylar::http::DealfileServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            sylar::http::ShareFilesServlet::ptr sltSharefiles(new sylar::http::ShareFilesServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            sylar::http::DealShareFileServlet::ptr sltDealSharefile(new sylar::http::DealShareFileServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
            ));
            slt_dispatch->addGlobServlet("/test", slt);
            slt_dispatch->addGlobServlet("/api/login", sltLogin);
            slt_dispatch->addGlobServlet("/api/reg", sltReg);
            slt_dispatch->addGlobServlet("/api/myfiles", sltMyfiles);
            slt_dispatch->addGlobServlet("/api/upload", sltUpload);
            slt_dispatch->addGlobServlet("/api/md5", sltMD5);
            slt_dispatch->addGlobServlet("/api/dealfile", sltDealfile);
            slt_dispatch->addGlobServlet("/api/sharefiles", sltSharefiles);
            slt_dispatch->addGlobServlet("/api/dealsharefile", sltDealSharefile);
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
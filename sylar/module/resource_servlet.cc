#include "resource_servlet.h"
#include "log.h"
#include "json/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "common/make_log.h" //日志头文件
#include "common/util_cgi.h"
#include "common/deal_mysql.h"
#include "common/redis_op.h"
#include "common/cfg.h"
#include "common/cJSON.h"
#include "common/des.h"    //加密
#include "common/base64.h" //base64
#include "common/md5.h"    //md5
#include <time.h>

#define MYFILES_LOG_MODULE "cgi"
#define MYFILES_LOG_PROC "myfiles"


namespace sylar {
    namespace http {

        static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
        void Servlet::read_cfg() {
//读取mysql数据库配置信息
            get_cfg_value(CFG_PATH, "mysql", "user", mysql_user);
            get_cfg_value(CFG_PATH, "mysql", "password", mysql_pwd);
            get_cfg_value(CFG_PATH, "mysql", "database", mysql_db);
            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "mysql:[user=%s,pwd=%s,database=%s]", mysql_user, mysql_pwd, mysql_db);
        }

        ResourceServlet::ResourceServlet(const std::string& path)
            :Servlet("ResourceServlet")
            , m_path(path) {}

        int32_t ResourceServlet::handle(sylar::http::HttpRequest::ptr request
            , sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {
            // auto tmppath = m_path + "/" + request->getPath();
            // SYLAR_LOG_INFO(g_logger) << "handle path=" << request->getPath();
            // auto path = "/root/CPP/sylar/workspace/serverFramework/bin/" + request->getPath();
            std::string path = "/root/CPP/sylar/workspace/serverFramework/bin/html/index.html";
            //std::string path = "/root/projects/0voice_tuchuang/tc-front/index.html";
            SYLAR_LOG_INFO(g_logger) << "handle path=" << path;

            if (path.find("..") != std::string::npos) {
                response->setBody("invalid path");
                response->setStatus(sylar::http::HttpStatus::NOT_FOUND);
                return 0;
            }

            std::ifstream ifs(path);
            if (!ifs) {
                response->setBody("invalid file");
                response->setStatus(sylar::http::HttpStatus::NOT_FOUND);
                return 0;
            }

            std::stringstream ss;
            std::string line;
            while (std::getline(ifs, line)) {
                ss << line << std::endl;
            }

            response->setBody(ss.str());
            response->setHeader("content-type", "text/html;charset=utf-8");
            return 0;
        }



    }
}
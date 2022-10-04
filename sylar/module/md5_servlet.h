#ifndef __MD5_SERVLET_H__
#define __MD5_SERVLET_H__

#include "http/servlet.h"
// #include "mysql.h"
#include <memory>

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

namespace sylar {
    namespace http {
        class MD5Servlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<MD5Servlet> ptr;

            MD5Servlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;
            int get_md5_info(char* buf, char* user, char* token, char* md5, char* filename);
            int deal_md5(char* user, char* md5, char* filename
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int close(int ret, char* out, MYSQL* conn
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);

        private:
            std::string m_path;
        };

    }
}


#endif
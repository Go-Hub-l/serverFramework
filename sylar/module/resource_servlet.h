#ifndef __RESOURCE_SERVLET_H__
#define __RESOURCE_SERVLET_H__

#include "http/servlet.h"
#include <memory>


// extern "C" {
// #include "common/make_log.h" //日志头文件
// #include "common/util_cgi.h"
// #include "common/deal_mysql.h"
// #include "common/redis_op.h"
// #include "common/cfg.h"
// #include "common/cJSON.h"
// #include "common/des.h"    //加密
// #include "common/base64.h" //base64
// #include "common/md5.h"    //md5
// #include <time.h>
// }


namespace sylar {
    namespace http {
        class ResourceServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<ResourceServlet> ptr;

            ResourceServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;

        private:
            std::string m_path;
        };
        class LoginServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<LoginServlet> ptr;

            LoginServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;

            //解析用户登陆信息的json包login_buf
            //用户名保存在username，密码保存在pwd
            bool get_login_info(const std::string& login_buf, std::string& usrname, std::string& pwd);
            int check_user_pwd(const std::string& username, const std::string& pwd);
            int set_token(char* username, char* token);
            void return_login_status(int ret_code, const char* token, sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);

        private:
            std::string m_path;
        };

    }
}


#endif
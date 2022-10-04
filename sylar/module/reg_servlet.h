#ifndef __REG_SERVLET_H__
#define __REG_SERVLET_H__

#include "http/servlet.h"
#include <memory>


namespace sylar {
    namespace http {

        class RegServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<RegServlet> ptr;

            RegServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;
            int get_reg_info(char* reg_buf, char* user, char* nick_name, char* pwd, char* tel, char* email);
            int user_register(char* reg_buf);
            //int close(cJSON* root);

        private:
            std::string m_path;
        };

    }
}


#endif
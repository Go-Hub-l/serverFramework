#ifndef __DEALSHAREFILE_SERVLET_H__
#define __DEALSHAREFILE_SERVLET_H__

#include "http/servlet.h"
#include <memory>

namespace sylar {
    namespace http {
        class DealShareFileServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<DealShareFileServlet> ptr;

            DealShareFileServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;
            void read_cfg();
            int get_json_info(char* buf, char* user, char* md5, char* filename);
            int pv_file(char* md5, char* filename
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int cancel_share_file(char* user, char* md5, char* filename
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int save_file(char* user, char* md5, char* filename
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);

        private:
            std::string m_path;
            char redis_ip[30] = { 0 };
            char redis_port[10] = { 0 };
        };

    }
}


#endif
#ifndef __DEALFILE_SERVLET_H__
#define __DEALFILE_SERVLET_H__

#include "http/servlet.h"
#include <memory>

namespace sylar {
    namespace http {
        class DealfileServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<DealfileServlet> ptr;

            DealfileServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;
            void read_cfg();
            int get_json_info(char* buf, char* user, char* token, char* md5, char* filename);
            int share_file(char* user, char* md5, char* filename
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int remove_file_from_storage(char* fileid);
            int del_file(char* user, char* md5, char* filename
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int pv_file(char* user, char* md5, char* filename
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
#ifndef __SHAREFILES_SERVLET_H__
#define __SHAREFILES_SERVLET_H__

#include "http/servlet.h"
#include <memory>

namespace sylar {
    namespace http {
        class ShareFilesServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<ShareFilesServlet> ptr;

            ShareFilesServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;

            void read_cfg();
            int get_share_files_count(long* pcout);
            void handle_get_share_files_count(sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int get_fileslist_json_info(char* buf, int* p_start, int* p_count);
            int get_share_filelist(int start, int count
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int get_ranking_filelist(int start, int count
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
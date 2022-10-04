#ifndef __UPLOAD_SERVLET_H__
#define __UPLOAD_SERVLET_H__

#include "http/servlet.h"
#include <memory>


namespace sylar {
    namespace http {

        class UploadServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<UploadServlet> ptr;

            UploadServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;

            char* trim_space_and_around(char* begin, char* end);
            char* buffer_search(char* buf, int total_len, const char* sep, const int seplen);
            int recv_save_file(long len, char* user, char* filename, char* md5, long* p_size);
            int recv_save_file1(long len, char* user, char* filename, char* md5, long* p_size);
            int upload_to_dstorage_1(char* filename, char* confpath, char* fileid);
            int upload_to_dstorage(char* filename, char* fileid);
            int make_file_url(char* fileid, char* fdfs_file_url);
            int store_fileinfo_to_mysql(char* user, char* filename, char* md5, long size, char* fileid, char* fdfs_file_url);

        private:
            std::string m_path;
            std::string body;
        };

    }
}


#endif
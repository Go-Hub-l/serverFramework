#ifndef __MYFILES_SERVLET_H__
#define __MYFILES_SERVLET_H__

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

        class MyfilesServlet : public sylar::http::Servlet {
        public:
            typedef std::shared_ptr<MyfilesServlet> ptr;

            MyfilesServlet(const std::string& path);
            virtual int32_t handle(sylar::http::HttpRequest::ptr request
                , sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session) override;

            //解析的json包, 登陆token
            int get_count_json_info(char* buf, char* user, char* token);
            void return_myfiles_status(long total, int ret_code,
                sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int get_user_files_count(char* user, long* count);
            void handle_user_files_count(char* user, sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);
            int get_fileslist_json_info(char* buf,
                char* user, char* token, int* p_start, int* p_count);
            int get_user_filelist(char* cmd, char* user, int start, int count, sylar::http::HttpResponse::ptr response
                , sylar::http::HttpSession::ptr session);

        private:
            std::string m_path;
        };

    }
}


#endif
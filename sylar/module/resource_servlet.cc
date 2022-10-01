#include "resource_servlet.h"
#include "log.h"
#include "json/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "common/deal_mysql.h"

// extern "C" {
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
// }

#define LOGIN_LOG_MODULE "cgi"
#define LOGIN_LOG_PROC   "login"

#define LOGIN_RET_OK          0  // 成功
#define LOGIN_RET_USER_EXIST  1  // 用户存在
#define LOGIN_RET_FAIL        2  // 失败

namespace sylar {
    namespace http {

        static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
        static sylar::Logger::ptr g_logLogin = SYLAR_LOG_NAME("login");

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


        LoginServlet::LoginServlet(const std::string& path)
            :Servlet("LoginServlet")
            , m_path(path) {}

        int32_t LoginServlet::handle(sylar::http::HttpRequest::ptr request
            , sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {

            std::cout << request->getBody() << std::endl;

            std::string username;
            std::string pwd;
            char token[128] = { 0 };
            // username.resize(1024);
            // pwd.resize(1024);
            get_login_info(request->getBody(), username, pwd);
            LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "username = %s, pwd = %s\n", username, pwd);

                        //登陆判断，成功返回0，失败返回-1
            bool ret = check_user_pwd(username, pwd);
            if (ret == 0) //登陆成功
            {
                //生成token字符串
                memset(token, 0, sizeof(token));
                ret = set_token((char*) username.c_str(), token);
                LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "token = %s\n", token);

            }

            if (ret == 0) {
                //返回前端登陆情况， 000代表成功
                return_login_status(HTTP_RESP_OK, token, response, session);
            } else {
                //返回前端登陆情况， 001代表失败
                return_login_status(HTTP_RESP_FAIL, "fail", response, session);
            }
            return 0;
        }
        bool LoginServlet::get_login_info(const std::string& login_buf, std::string& usrname, std::string& pwd) {
            //解析json包
            //解析一个json字符串为cJSON对象
            Json::CharReaderBuilder ReaderBuilder;
            ReaderBuilder["emitUTF8"] = true;
            Json::Value root;
            std::unique_ptr<Json::CharReader> charread(ReaderBuilder.newCharReader());
            std::string strerr;

            bool isok = charread->parse(login_buf.c_str(), login_buf.c_str() + login_buf.size(), &root, &strerr);
            // root.append(login_buf);
            if (!isok || strerr.size() != 0) {
                SYLAR_LOG_ERROR(g_logLogin) << "json parse err";
                return false;
            }
            //返回指定字符串对应的json对象
            //用户
            Json::Value child1 = root["user"];
            if (child1.empty()) {
                SYLAR_LOG_ERROR(g_logLogin) << "get user err";
                return false;
            }
            SYLAR_LOG_INFO(g_logLogin) << child1.asString();
            usrname = child1.asString(); //拷贝内容

            Json::Value child2 = root["pwd"];
            if (child2.empty()) {
                SYLAR_LOG_ERROR(g_logLogin) << "get pwd err";
                return false;
            }
            SYLAR_LOG_INFO(g_logLogin) << child2.asString();
            pwd = child2.asString(); //拷贝内容

            return true;
        }

        int LoginServlet::check_user_pwd(const std::string& username, const std::string& pwd) {
            char sql_cmd[SQL_MAX_LEN] = { 0 };
            int ret = 0;

            //获取数据库用户名、用户密码、数据库标示等信息
            char mysql_user[256] = { 0 };
            char mysql_pwd[256] = { 0 };
            char mysql_db[256] = { 0 };
            get_mysql_info(mysql_user, mysql_pwd, mysql_db);
            LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "mysql_user = %s, mysql_pwd = %s, mysql_db = %s\n", mysql_user, mysql_pwd, mysql_db);

            //connect the database
            MYSQL* conn = msql_conn(mysql_user, mysql_pwd, mysql_db);
            if (conn == NULL) {
                LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "msql_conn err\n");
                return -1;
            }

            //设置数据库编码，主要处理中文编码问题
            mysql_query(conn, "set names utf8");

            //sql语句，查找某个用户对应的密码
            sprintf(sql_cmd, "select password from user_info where user_name='%s'", username.c_str());

            //deal result
            char tmp[PWD_LEN] = { 0 };

            //返回值： 0成功并保存记录集，1没有记录集，2有记录集但是没有保存，-1失败
            process_result_one(conn, sql_cmd, tmp); //执行sql语句，结果集保存在tmp
            if (strcmp(tmp, pwd.c_str()) == 0) {
                ret = 0;
            } else {
                ret = -1;
            }

            mysql_close(conn);


            return ret;
        }

        int LoginServlet::set_token(char* username, char* token) {
            int ret = 0;
            redisContext* redis_conn = NULL;

            //redis 服务器ip、端口
            char redis_ip[30] = { 0 };
            char redis_port[10] = { 0 };

            //读取redis配置信息
            get_cfg_value(CFG_PATH, "redis", "ip", redis_ip);
            get_cfg_value(CFG_PATH, "redis", "port", redis_port);
            LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "redis:[ip=%s,port=%s]\n", redis_ip, redis_port);

            //连接redis数据库
            redis_conn = rop_connectdb_nopwd(redis_ip, redis_port);
            if (redis_conn == NULL) {
                LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "redis connected error\n");
                ret = -1;
                //goto END;
            }

            //产生4个1000以内的随机数
            int rand_num[4] = { 0 };
            int i = 0;

            //设置随机种子
            srand((unsigned int) time(NULL));
            for (i = 0; i < 4; ++i) {
                rand_num[i] = rand() % 1000;//随机数
            }

            char tmp[1024] = { 0 };
            sprintf(tmp, "%s%d%d%d%d", username, rand_num[0], rand_num[1], rand_num[2], rand_num[3]);   // token唯一性
            LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "tmp = %s\n", tmp);

            //加密
            char enc_tmp[1024 * 2] = { 0 };
            int enc_len = 0;
            ret = DesEnc((unsigned char*) tmp, strlen(tmp), (unsigned char*) enc_tmp, &enc_len);        // 加密
            if (ret != 0) {
                LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "DesEnc error\n");
                ret = -1;
                //goto END;
            }

            //to base64
            char base64[1024 * 3] = { 0 };
            base64_encode((const unsigned char*) enc_tmp, enc_len, base64);  //base64编码 防止不可见字符
            LOG(LOGIN_LOG_MODULE, LOGIN_LOG_PROC, "base64 = %s\n", base64);

            //to md5
            MD5_CTX md5;
            MD5Init(&md5);
            unsigned char decrypt[16];
            MD5Update(&md5, (unsigned char*) base64, strlen(base64));
            MD5Final(&md5, decrypt);


            char str[100] = { 0 };
            for (i = 0; i < 16; i++) {
                sprintf(str, "%02x", decrypt[i]);                       // 最后token转成md5值，定长
                strcat(token, str);
            }

            // redis保存此字符串，用户名：token, 有效时间为24小时
            ret = rop_setex_string(redis_conn, username, 86400, token);
            //ret = rop_setex_string(redis_conn, user, 30, token); //30秒


        //END:
            if (redis_conn != NULL) {
                rop_disconnect(redis_conn);
            }

            return ret;

        }

        void LoginServlet::return_login_status(int ret_code, const char* token, sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {
            char* out = NULL;
            cJSON* root = cJSON_CreateObject();  //创建json项目
            cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(ret_code));// 
            cJSON_AddStringToObject(root, "token", token);// {"token":"token"}
            out = cJSON_Print(root);//cJSON to string(char *)

            cJSON_Delete(root);

            if (out != NULL) {
                response->setBody(out);
                session->sendResponse(response);
                // printf(out); //给前端反馈信息
                free(out); //记得释放
            }
        }
    }
}
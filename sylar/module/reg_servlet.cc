#include "reg_servlet.h"
#include "log.h"

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

#define REG_LOG_MODULE       "cgi"
#define REG_LOG_PROC         "reg"
#define REG_DEBUG_LOG_PROC   "reg_debug"

namespace sylar {
    namespace http {
        static sylar::Logger::ptr g_logReg = SYLAR_LOG_NAME("reg");

        RegServlet::RegServlet(const std::string& path) :
            Servlet("RegServlet"), m_path(path) {}

        int32_t RegServlet::handle(sylar::http::HttpRequest::ptr request
            , sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {

            std::string body = request->getBody();
            int len = body.size();

            if (len <= 0)//没有登陆用户信息
            {
                SYLAR_LOG_INFO(g_logReg) << "No data from standard input.<p>\n";
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "len = 0, No data from standard input\n");
            } else //获取登陆用户信息
            {
                char buf[4 * 1024] = { 0 };
                strcpy(buf, body.c_str());
                int ret = 0;
                char* out = NULL;

                LOG(REG_LOG_MODULE, REG_LOG_PROC, "buf = %s\n", body.c_str());

                //注册用户，成功返回0，失败返回-1, 该用户已存在返回-2
                /*
                注册：
                    成功：{"code": 0}
                    失败：{"code":1}
                    该用户已存在：{"code":2}
                */
                ret = user_register(buf);
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "ret = %d\n", ret);
                if (ret == 0) //登陆成功
                {
                    out = return_status(HTTP_RESP_OK);//util_cgi.h
                } else if (ret == -1) {
                    out = return_status(HTTP_RESP_FAIL);//util_cgi.h
                } else if (ret == -2) // 用户存在
                {
                    out = return_status(HTTP_RESP_USER_EXIST);//util_cgi.h
                }

                if (out != NULL) {
                    response->setBody(out);
                    session->sendResponse(response);
                    free(out); //记得释放
                }
            }
            return 0;
        }

        //解析用户注册信息的json包
        int RegServlet::get_reg_info(char* reg_buf, char* user, char* nick_name, char* pwd, char* tel, char* email) {
            int ret = 0;

            /*json数据如下
            {
                userName:xxxx,
                nickName:xxx,
                firstPwd:xxx,
                phone:xxx,
                email:xxx
            }
            */

            //解析json包
            //解析一个json字符串为cJSON对象
            cJSON* root = cJSON_Parse(reg_buf);
            if (NULL == root) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "cJSON_Parse err\n");
                ret = -1;
                //goto END;

            }

            //返回指定字符串对应的json对象
            //用户
            cJSON* child1 = cJSON_GetObjectItem(root, "userName");
            if (NULL == child1) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                //goto END;
            }

            //LOG(REG_LOG_MODULE, REG_LOG_PROC, "child1->valuestring = %s\n", child1->valuestring);
            strcpy(user, child1->valuestring); //拷贝内容

            //昵称
            cJSON* child2 = cJSON_GetObjectItem(root, "nickName");
            if (NULL == child2) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                //goto END;
            }
            strcpy(nick_name, child2->valuestring); //拷贝内容

            //密码
            cJSON* child3 = cJSON_GetObjectItem(root, "firstPwd");
            if (NULL == child3) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                //goto END;
            }
            strcpy(pwd, child3->valuestring); //拷贝内容

            //电话
            cJSON* child4 = cJSON_GetObjectItem(root, "phone");
            if (NULL == child4) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                //goto END;
            }
            strcpy(tel, child4->valuestring); //拷贝内容

            //邮箱
            cJSON* child5 = cJSON_GetObjectItem(root, "email");
            if (NULL == child5) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                //goto END;
            }
            strcpy(email, child5->valuestring); //拷贝内容

        //END:
            if (root != NULL) {
                cJSON_Delete(root);//删除json对象
                root = NULL;
            }

            return ret;
        }

//注册用户，成功返回0，失败返回-1, 该用户已存在返回-2
        int RegServlet::user_register(char* reg_buf) {
            LOG(REG_LOG_MODULE, REG_DEBUG_LOG_PROC, "reg_buf = %s\n", reg_buf);
            int ret = 0;
            //LOG(REG_LOG_MODULE, REG_LOG_PROC, "ret = %d\n", ret);
            MYSQL* conn = NULL;

            //获取数据库用户名、用户密码、数据库标示等信息
            char mysql_user[256] = { 0 };
            char mysql_pwd[256] = { 0 };
            char mysql_db[256] = { 0 };
            ret = get_mysql_info(mysql_user, mysql_pwd, mysql_db);
            if (ret != 0) {
                //goto END;
                if (conn != NULL) {
                    mysql_close(conn); //断开数据库连接
                }

                return ret;
            }
            LOG(REG_LOG_MODULE, REG_LOG_PROC, "mysql_user = %s, mysql_pwd = %s, mysql_db = %s\n", mysql_user, mysql_pwd, mysql_db);

            //获取注册用户的信息
            char user_name[128];
            char nick_name[128];
            char pwd[128];
            char tel[128];
            char email[128];
            ret = get_reg_info(reg_buf, user_name, nick_name, pwd, tel, email);
            if (ret != 0) {
                //goto END;
                if (conn != NULL) {
                    mysql_close(conn); //断开数据库连接
                }

                return ret;
            }
            LOG(REG_LOG_MODULE, REG_LOG_PROC, "user_name = %s, nick_name = %s, pwd = %s, tel = %s, email = %s\n", user_name, nick_name, pwd, tel, email);
            LOG(REG_LOG_MODULE, REG_LOG_PROC, "%s %s %s\n", mysql_user, mysql_pwd, mysql_db);
            //connect the database
            conn = msql_conn(mysql_user, mysql_pwd, mysql_db);
            if (conn == NULL) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "msql_conn err\n");
                ret = -1;
                //goto END;
                if (conn != NULL) {
                    mysql_close(conn); //断开数据库连接
                }

                return ret;
            }

            //设置数据库编码，主要处理中文编码问题
            mysql_query(conn, "set names utf8");

            char sql_cmd[1024] = { 0 };

            sprintf(sql_cmd, "select * from user_info where user_name = '%s'", user_name);

            //查看该用户是否存在
            int ret2 = 0;
            //返回值： 0成功并保存记录集，1没有记录集，2有记录集但是没有保存，-1失败
            ret2 = process_result_one(conn, sql_cmd, NULL); //指向sql查询语句
            if (ret2 == 2) //如果存在
            {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "【%s】该用户已存在\n");
                ret = -2;
                //goto END;
                if (conn != NULL) {
                    mysql_close(conn); //断开数据库连接
                }

                return ret;
            }

            //当前时间戳
            struct timeval tv;
            struct tm* ptm;
            char time_str[128];

            //使用函数gettimeofday()函数来得到时间。它的精度可以达到微妙
            gettimeofday(&tv, NULL);
            ptm = localtime(&tv.tv_sec);//把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为本地时间
            //strftime() 函数根据区域设置格式化本地时间/日期，函数的功能将时间格式化，或者说格式化一个时间字符串
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", ptm);

            //sql 语句, 插入注册信息
            sprintf(sql_cmd, "insert into user_info (user_name, nick_name, password, phone, email, create_time) values ('%s', '%s', '%s', '%s', '%s', '%s')", user_name, nick_name, pwd, tel, email, time_str);

            if (mysql_query(conn, sql_cmd) != 0) {
                LOG(REG_LOG_MODULE, REG_LOG_PROC, "%s 插入失败：%s\n", sql_cmd, mysql_error(conn));
                ret = -1;
                //goto END;
                if (conn != NULL) {
                    mysql_close(conn); //断开数据库连接
                }

                return ret;
            }

        //END:
            if (conn != NULL) {
                mysql_close(conn); //断开数据库连接
            }

            return ret;
        }



        // int RegServlet::close(cJSON* root) {
        //     if (root != NULL) {
        //         cJSON_Delete(root);//删除json对象
        //         root = NULL;
        //     }
        // }

    }
}
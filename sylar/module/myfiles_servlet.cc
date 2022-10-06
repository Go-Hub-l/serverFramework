#include "myfiles_servlet.h"
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

#define MYFILES_LOG_MODULE "cgi"
#define MYFILES_LOG_PROC "myfiles"

namespace sylar {
    namespace http {
        static sylar::Logger::ptr g_logMyfile = SYLAR_LOG_NAME("myfiles");
        MyfilesServlet::MyfilesServlet(const std::string& path)
            :Servlet("MyfilesServlet"), m_path(path) {}

        int32_t MyfilesServlet::handle(sylar::http::HttpRequest::ptr request
            , sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {
            read_cfg();
            std::string tmpcmd = request->getParam("cmd");
            char cmd[1024] = { 0 };
            strcpy(cmd, tmpcmd.c_str());
            std::string body = request->getBody();
            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cmd = %s\n", cmd);
            SYLAR_LOG_INFO(g_logMyfile) << cmd;
            int len = body.size();
            char user[1024] = { 0 };
            char token[1024] = { 0 };
            char buf[4096] = { 0 };
            int ret = 0;
            if (len <= 0) {
                SYLAR_LOG_INFO(g_logMyfile) << "no bady data";
                return 0;
            } else {
                strcpy(buf, body.c_str());
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "buf = %s\n", buf);
                if (strcmp(cmd, "count") == 0) {
                    get_count_json_info(buf, user, token); //通过json包获取用户名, token

                    //验证登陆token，成功返回0，失败-1
                    ret = verify_token(user, token); // util_cgi.h

                    if (ret == 0) {
                        handle_user_files_count(user, response, session); //获取用户文件个数
                    } else {
                        char* out = return_status(HTTP_RESP_FAIL); // token验证失败错误码
                        if (out != NULL) {
                            // printf(out); //给前端反馈错误码
                            response->setBody(out);
                            session->sendResponse(response);
                            free(out);
                        }
                    }
                } else {
                    //获取用户文件信息 127.0.0.1:80/myfiles&cmd=normal
                    //按下载量升序 127.0.0.1:80/myfiles?cmd=pvasc
                    //按下载量降序127.0.0.1:80/myfiles?cmd=pvdesc
                    int start = 0;                                                 //文件起点
                    int count = 0;                                                 //文件个数
                    ret = get_fileslist_json_info(buf, user, token, &start, &count); //通过json包获取信息
                    LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "user = %s, token = %s, start = %d, count = %d\n", user, token, start, count);
                    if (ret == 0) {
                        //验证登陆token，成功返回0，失败-1
                        ret = verify_token(user, token); // util_cgi.h
                        if (ret == 0) {
                            get_user_filelist(cmd, user, start, count, response, session); //获取用户文件列表
                        } else {
                            char* out = return_status(HTTP_RESP_FAIL); // token验证失败错误码
                            if (out != NULL) {
                                response->setBody(out);
                                session->sendResponse(response);
                                free(out);
                            }
                        }
                    } else {
                        char* out = return_status(HTTP_RESP_FAIL); // token验证失败错误码
                        if (out != NULL) {
                            response->setBody(out);
                            session->sendResponse(response);
                            free(out);
                        }
                    }
                }
            }

            return 0;
        }

        //解析的json包, 登陆token
        int MyfilesServlet::get_count_json_info(char* buf, char* user, char* token) {
            int ret = 0;

                /*json数据如下
                {
                    "token": "9e894efc0b2a898a82765d0a7f2c94cb",
                    user:xxxx
                }
                */

                //解析json包
                //解析一个json字符串为cJSON对象
            cJSON* root = cJSON_Parse(buf);
            if (NULL == root) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_Parse err\n");
                ret = -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }

            //返回指定字符串对应的json对象
            //用户
            cJSON* child1 = cJSON_GetObjectItem(root, "user");
            if (NULL == child1) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }

            // LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "child1->valuestring = %s\n", child1->valuestring);
            strcpy(user, child1->valuestring); //拷贝内容

            //登陆token
            cJSON* child2 = cJSON_GetObjectItem(root, "token");
            if (NULL == child2) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                return -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }

            // LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "child2->valuestring = %s\n", child2->valuestring);
            strcpy(token, child2->valuestring); //拷贝内容

        // END:
            if (root != NULL) {
                cJSON_Delete(root); //删除json对象
                root = NULL;
            }

            return ret;
        }

        //返回前端情况
        void MyfilesServlet::return_myfiles_status(long total, int ret_code, sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {
            char* out = NULL;

            cJSON* root = cJSON_CreateObject(); //创建json项目
            cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(ret_code));
            cJSON_AddItemToObject(root, "total", cJSON_CreateNumber(total));

            out = cJSON_Print(root); // cJSON to string(char *)

            cJSON_Delete(root);

            if (out != NULL) {
                response->setBody(out);
                session->sendResponse(response);
                free(out);   //记得释放
            }
        }

        //获取用户文件个数
        int MyfilesServlet::get_user_files_count(char* user, long* count) {
            char sql_cmd[SQL_MAX_LEN] = { 0 };
            MYSQL* conn = NULL;
            long line = 0;
            int ret = 0;

            // connect the database
            conn = msql_conn(mysql_user, mysql_pwd, mysql_db);
            if (conn == NULL) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "msql_conn err\n");
                // goto END;
                if (conn != NULL) {
                    mysql_close(conn);
                }
                *count = line;
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "ret = %d, line = %ld\n", ret, line);
                return ret;
            }

            //设置数据库编码，主要处理中文编码问题
            mysql_query(conn, "set names utf8");

            sprintf(sql_cmd, "select count from user_file_count where user='%s'", user);
            char tmp[512] = { 0 };
            //返回值： 0成功并保存记录集，1没有记录集，2有记录集但是没有保存，-1失败
            ret = process_result_one(conn, sql_cmd, tmp); //指向sql语句
            if (ret != 0) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s 操作失败\n", sql_cmd);
                // goto END;
                if (conn != NULL) {
                    mysql_close(conn);
                }
                ret = 0;
                *count = line;
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "ret = %d, line = %ld\n", ret, line);
                return ret;
            }

            line = atol(tmp); //字符串转长整形

        // END:
            if (conn != NULL) {
                mysql_close(conn);
            }
            *count = line;
            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "ret = %d, line = %ld\n", ret, line);
            return ret;
        }

        void MyfilesServlet::handle_user_files_count(char* user, sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {
            long line = 0;
            int ret = 0;

            ret = get_user_files_count(user, &line);

            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "ret = %d, line = %ld\n", ret, line);
            //给前端反馈的信息
            if (ret == 0) {
                return_myfiles_status(line, HTTP_RESP_OK, response, session);
            } else {
                return_myfiles_status(0, HTTP_RESP_FAIL, response, session);
            }
        }

        //解析的json包
        int MyfilesServlet::get_fileslist_json_info(char* buf,
            char* user, char* token, int* p_start, int* p_count) {
            int ret = 0;

            /*json数据如下
            {
                "user": "milo"
                "token": xxxx
                "start": 0
                "count": 10
            }
            */

            //解析json包
            //解析一个json字符串为cJSON对象
            cJSON* root = cJSON_Parse(buf);
            if (NULL == root) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_Parse err\n");
                ret = -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }

            //返回指定字符串对应的json对象
            //用户
            cJSON* child1 = cJSON_GetObjectItem(root, "user");
            if (NULL == child1) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }

            // LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "child1->valuestring = %s\n", child1->valuestring);
            strcpy(user, child1->valuestring); //拷贝内容

            // token
            cJSON* child2 = cJSON_GetObjectItem(root, "token");
            if (NULL == child2) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }

            strcpy(token, child2->valuestring); //拷贝内容

            //文件起点
            cJSON* child3 = cJSON_GetObjectItem(root, "start");
            if (NULL == child3) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }

            *p_start = child3->valueint;

            //文件请求个数
            cJSON* child4 = cJSON_GetObjectItem(root, "count");
            if (NULL == child4) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_GetObjectItem err\n");
                ret = -1;
                // goto END;
                if (root != NULL) {
                    cJSON_Delete(root); //删除json对象
                    root = NULL;
                }

                return ret;
            }
            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "count:%d\n", *p_count);

            *p_count = child4->valueint;

        // END:
            if (root != NULL) {
                cJSON_Delete(root); //删除json对象
                root = NULL;
            }

            return ret;
        }

        //获取用户文件列表
        //获取用户文件信息 127.0.0.1:80/myfiles&cmd=normal
        //按下载量升序 127.0.0.1:80/myfiles?cmd=pvasc
        //按下载量降序127.0.0.1:80/myfiles?cmd=pvdesc
        int MyfilesServlet::get_user_filelist(char* cmd, char* user, int start, int count
            , sylar::http::HttpResponse::ptr response
            , sylar::http::HttpSession::ptr session) {
            int ret = 0;
            char sql_cmd[SQL_MAX_LEN] = { 0 };
            MYSQL* conn = NULL;
            cJSON* root = NULL;
            cJSON* array = NULL;
            char* out = NULL;
            MYSQL_RES* res_set = NULL;
            long total = 0;
            int line = 0;

            root = cJSON_CreateObject();
            ret = get_user_files_count(user, &total);
            if (ret != 0) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "get_user_files_count err\n");
                // goto END;
                if (ret == 0) {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_OK)); //
                } else {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_FAIL)); //
                }
                cJSON_AddItemToObject(root, "count", cJSON_CreateNumber(line));
                cJSON_AddItemToObject(root, "total", cJSON_CreateNumber(total));
                out = cJSON_Print(root);

                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s\n", out);

                if (out != NULL) {
                    response->setBody(out);
                    session->sendResponse(response);
                    free(out);
                }

                if (res_set != NULL) {
                    //完成所有对数据的操作后，调用mysql_free_result来善后处理
                    mysql_free_result(res_set);
                }

                if (conn != NULL) {
                    mysql_close(conn);
                }

                if (root != NULL) {
                    cJSON_Delete(root);
                }

                return ret;
            }
            // connect the database
            conn = msql_conn(mysql_user, mysql_pwd, mysql_db);
            if (conn == NULL) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "msql_conn err\n");
                // goto END;
                if (ret == 0) {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_OK)); //
                } else {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_FAIL)); //
                }
                cJSON_AddItemToObject(root, "count", cJSON_CreateNumber(line));
                cJSON_AddItemToObject(root, "total", cJSON_CreateNumber(total));
                out = cJSON_Print(root);

                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s\n", out);

                if (out != NULL) {
                    response->setBody(out);
                    session->sendResponse(response);
                    free(out);
                }

                if (res_set != NULL) {
                    //完成所有对数据的操作后，调用mysql_free_result来善后处理
                    mysql_free_result(res_set);
                }

                if (conn != NULL) {
                    mysql_close(conn);
                }

                if (root != NULL) {
                    cJSON_Delete(root);
                }

                return ret;
            }

            //设置数据库编码，主要处理中文编码问题
            mysql_query(conn, "set names utf8");

            //多表指定行范围查询
            if (strcmp(cmd, "normal") == 0) //获取用户文件信息
            {
                // sql语句
                sprintf(sql_cmd, "select user_file_list.*, file_info.url, file_info.size, file_info.type from file_info, user_file_list where user = '%s' and file_info.md5 = user_file_list.md5 limit %d, %d", user, start, count);
            } else if (strcmp(cmd, "pvasc") == 0) //按下载量升序
            {
                // sql语句
                sprintf(sql_cmd, "select user_file_list.*, file_info.url, file_info.size, file_info.type from file_info, user_file_list where user = '%s' and file_info.md5 = user_file_list.md5  order by pv asc limit %d, %d", user, start, count);
            } else if (strcmp(cmd, "pvdesc") == 0) //按下载量降序
            {
                // sql语句
                sprintf(sql_cmd, "select user_file_list.*, file_info.url, file_info.size, file_info.type from file_info, user_file_list where user = '%s' and file_info.md5 = user_file_list.md5 order by pv desc limit %d, %d", user, start, count);
            }

            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s 在操作\n", sql_cmd);

            if (mysql_query(conn, sql_cmd) != 0) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s 操作失败：%s\n", sql_cmd, mysql_error(conn));
                ret = -1;
                // goto END;
                if (ret == 0) {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_OK)); //
                } else {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_FAIL)); //
                }
                cJSON_AddItemToObject(root, "count", cJSON_CreateNumber(line));
                cJSON_AddItemToObject(root, "total", cJSON_CreateNumber(total));
                out = cJSON_Print(root);

                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s\n", out);

                if (out != NULL) {
                    response->setBody(out);
                    session->sendResponse(response);
                    free(out);
                }

                if (res_set != NULL) {
                    //完成所有对数据的操作后，调用mysql_free_result来善后处理
                    mysql_free_result(res_set);
                }

                if (conn != NULL) {
                    mysql_close(conn);
                }

                if (root != NULL) {
                    cJSON_Delete(root);
                }

                return ret;
            }

            res_set = mysql_store_result(conn); /*生成结果集*/
            if (res_set == NULL) {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "smysql_store_result error: %s!\n", mysql_error(conn));
                ret = -1;
                // goto END;
                if (ret == 0) {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_OK)); //
                } else {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_FAIL)); //
                }
                cJSON_AddItemToObject(root, "count", cJSON_CreateNumber(line));
                cJSON_AddItemToObject(root, "total", cJSON_CreateNumber(total));
                out = cJSON_Print(root);

                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s\n", out);

                if (out != NULL) {
                    response->setBody(out);
                    session->sendResponse(response);
                    free(out);
                }

                if (res_set != NULL) {
                    //完成所有对数据的操作后，调用mysql_free_result来善后处理
                    mysql_free_result(res_set);
                }

                if (conn != NULL) {
                    mysql_close(conn);
                }

                if (root != NULL) {
                    cJSON_Delete(root);
                }

                return ret;
            }


            // mysql_num_rows接受由mysql_store_result返回的结果结构集，并返回结构集中的行数
            line = mysql_num_rows(res_set);
            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "mysql_num_rows(res_set) = %d\n", line);
            if (line == 0) //没有结果
            {
                ret = 0;
                // goto END;
                if (ret == 0) {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_OK)); //
                } else {
                    cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_FAIL)); //
                }
                cJSON_AddItemToObject(root, "count", cJSON_CreateNumber(line));
                cJSON_AddItemToObject(root, "total", cJSON_CreateNumber(total));
                out = cJSON_Print(root);

                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s\n", out);

                if (out != NULL) {
                    response->setBody(out);
                    session->sendResponse(response);
                    free(out);
                }

                if (res_set != NULL) {
                    //完成所有对数据的操作后，调用mysql_free_result来善后处理
                    mysql_free_result(res_set);
                }

                if (conn != NULL) {
                    mysql_close(conn);
                }

                if (root != NULL) {
                    cJSON_Delete(root);
                }

                return ret;
            }

            MYSQL_ROW row;

            array = cJSON_CreateArray();
            // mysql_fetch_row从使用mysql_store_result得到的结果结构中提取一行，并把它放到一个行结构中。
            // 当数据用完或发生错误时返回NULL.
            while ((row = mysql_fetch_row(res_set)) != NULL) {
                // array[i]:
                cJSON* item = cJSON_CreateObject();

                // mysql_num_fields获取结果中列的个数
                /*for(i = 0; i < mysql_num_fields(res_set); i++)
                {
                    if(row[i] != NULL)
                    {

                    }
                }*/

                /*
                {
                "user": "milo",
                "md5": "e8ea6031b779ac26c319ddf949ad9d8d",
                "create_time": "2020-06-21 21:35:25",
                "file_name": "test.mp4",
                "share_status": 0,
                "pv": 0,
                "url": "http://192.168.31.109:80/group1/M00/00/00/wKgfbViy2Z2AJ-FTAaM3As-g3Z0782.mp4",
                "size": 27473666,
                 "type": "mp4"
                }

                */
                int column_index = 1;
                //-- user	文件所属用户
                if (row[column_index] != NULL) {
                    cJSON_AddStringToObject(item, "user", row[column_index]);
                }

                column_index++;
                //-- md5 文件md5
                if (row[column_index] != NULL) {
                    cJSON_AddStringToObject(item, "md5", row[column_index]);
                }

                column_index++;
                //-- createtime 文件创建时间
                if (row[column_index] != NULL) {
                    cJSON_AddStringToObject(item, "create_time", row[column_index]);
                }

                column_index++;
                //-- filename 文件名字
                if (row[column_index] != NULL) {
                    cJSON_AddStringToObject(item, "file_name", row[column_index]);
                }

                column_index++;
                //-- shared_status 共享状态, 0为没有共享， 1为共享
                if (row[column_index] != NULL) {
                    cJSON_AddNumberToObject(item, "share_status", atoi(row[column_index]));
                }

                column_index++;
                //-- pv 文件下载量，默认值为0，下载一次加1
                if (row[column_index] != NULL) {
                    cJSON_AddNumberToObject(item, "pv", atol(row[column_index]));
                }

                column_index++;
                //-- url 文件url
                if (row[column_index] != NULL) {
                    cJSON_AddStringToObject(item, "url", row[column_index]);
                }

                column_index++;
                //-- size 文件大小, 以字节为单位
                if (row[column_index] != NULL) {
                    cJSON_AddNumberToObject(item, "size", atol(row[column_index]));
                }

                column_index++;
                //-- type 文件类型： png, zip, mp4……
                if (row[column_index] != NULL) {
                    cJSON_AddStringToObject(item, "type", row[column_index]);
                }

                cJSON_AddItemToArray(array, item);
            }

            cJSON_AddItemToObject(root, "files", array);

        // END:
            if (ret == 0) {
                cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_OK)); //
            } else {
                cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(HTTP_RESP_FAIL)); //
            }
            cJSON_AddItemToObject(root, "count", cJSON_CreateNumber(line));
            cJSON_AddItemToObject(root, "total", cJSON_CreateNumber(total));
            out = cJSON_Print(root);

            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s\n", out);

            if (out != NULL) {
                response->setBody(out);
                session->sendResponse(response);
                free(out);
            }

            if (res_set != NULL) {
                //完成所有对数据的操作后，调用mysql_free_result来善后处理
                mysql_free_result(res_set);
            }

            if (conn != NULL) {
                mysql_close(conn);
            }

            if (root != NULL) {
                cJSON_Delete(root);
            }

            return ret;
        }

    }
}
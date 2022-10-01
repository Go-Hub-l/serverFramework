#include "myMysql.h"
#include "log.h"



/*
    定义通用的数据库接口
*/

//后期做：创建数据库，查找所有表格
namespace db {

    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
    Mysql::Mysql() {
        m_pConn = mysql_init(nullptr);
        if (!connect("172.26.85.62", "root", "XIAObing321.", "chat")) {
            SYLAR_LOG_ERROR(g_logger) << "connect chat failed";
        } else {
            SYLAR_LOG_INFO(g_logger) << "connect chat success";
        }
    }
    Mysql::~Mysql() {
        if (m_pConn) {
            mysql_close(m_pConn);
        }
        m_pConn = nullptr;
    }
    //连接数据库
    bool Mysql::connect(const std::string server, const std::string user,
        const std::string password, const std::string dbName) {
        MYSQL* p = mysql_real_connect(m_pConn, server.c_str(), user.c_str(), password.c_str(),
            dbName.c_str(), 3306, nullptr, 0);
        if (!p) {
            char buf[1024] = "";
            sprintf(buf, "Failed to connect to database: Error: %s\n",
                mysql_error(m_pConn));
            SYLAR_LOG_ERROR(g_logger) << buf;
            return false;
        }
        return true;
    }
    //断开连接数据库
    void Mysql::disConnect() {
        mysql_close(m_pConn);
    }
    // //创建表格
    // bool Mysql::createTable(std::string sql, std::string tableName) {
    //     if (mysql_query(m_pConn, sql.c_str())) {
    //         char buf[1024] = "";
    //         sprintf(buf, "Failed to createTable: Error: %s\n",
    //             mysql_error(m_pConn));
    //         SYLAR_LOG_ERROR(g_logger) << buf;
    //         return false;
    //     }
    //     return true;
    // }
    //更新数据库
    bool Mysql::update(const Json::Value& sqlType) {
        std::string sql = "update " + sqlType["name"].asString() + " set ";
        for (Json::Value column : sqlType["column"]) {
            sql += column["name"].asString() + "=" + "\"" + column["value"].asString() + "\"" + ",";
        }
        //去除最后一个逗号
        sql.pop_back();
        for (Json::Value column : sqlType["filter"]) {
            if (column["type"] == db::Mysql::EQUAL) {
                sql += " where " + column["name"].asString() + "=" + column["value"].asString() + ";";
            }


        }

        return update(sql);
    }
    bool Mysql::update(const std::string& sql) {
        if (mysql_query(m_pConn, sql.c_str())) {
            char buf[1024] = "";
            sprintf(buf, "Failed to update: Error: %s\n",
                mysql_error(m_pConn));
            SYLAR_LOG_ERROR(g_logger) << buf;
            return false;
        }
        return true;
    }
    //从数据库中查询信息
    bool Mysql::select(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result,
        bool all, const std::vector<std::string>& col) {
        std::string sql = "select ";
        //构造sql语句
        if (sqlType["all"] == "true") {
            sql += " *from " + sqlType["name"].asString();
        } else {
            for (auto column : sqlType["column"]) {
                sql += column.asString() + ", ";
            }
            //删除最后一个逗号
            sql = sql.substr(0, sql.find_last_of(','));
            sql += " from " + sqlType["name"].asString();
        }
        return select(sql, result, all, col);
    }
    bool Mysql::select(const std::string& sql,
        std::vector<std::unordered_map<std::string, std::string> >& result,
        bool all, const std::vector<std::string>& col) {
        if (mysql_query(m_pConn, sql.c_str())) {
            char buf[1024] = "";
            sprintf(buf, "Failed to createTable: Error: %s\n",
                mysql_error(m_pConn));
            SYLAR_LOG_ERROR(g_logger) << buf;
            return false;
        }
        //初始化结果集检索
        MYSQL_RES* res = mysql_use_result(m_pConn);
        if (!res) {
            char buf[1024] = "";
            sprintf(buf, "Failed to createTable: Error: %s\n",
                mysql_error(m_pConn));
            SYLAR_LOG_ERROR(g_logger) << buf;
            return false;
        }
        //获取所有行到结果集中
        if (res) {
            MYSQL_ROW row;
            std::unordered_map < std::string, std::string> um;
            while ((row = mysql_fetch_row(res))) {
                if (all) {
                    um["id"] = row[0];
                    um["nickname"] = row[1];
                    um["passward"] = row[2];
                    um["email"] = row[3];
                    um["gender"] = row[4];
                    um["phone"] = row[5];
                    um["state"] = row[6];
                } else {
                    for (unsigned int i = 0; i < col.size(); ++i) {
                        um[col[i]] = row[i];
                    }
                }
                result.push_back(um);
                um.clear();
            }
        }
        return true;
    }
    bool Mysql::deleteInfo(const Json::Value& sqlType) {
        std::string sql = "delete from ";
        sql += sqlType["name"].asString() + " where ";
        for (Json::Value fil : sqlType["filter"]) {
            //目前只支持整数删除
            sql += fil["name"].asString() + "=" + fil["value"].asString();
        }
        return deleteInfo(sql);
    }
    bool Mysql::deleteInfo(const std::string& sql) {
        if (mysql_query(m_pConn, sql.c_str())) {
            char buf[1024] = "";
            sprintf(buf, "Failed to delete info -sql[%s]: Error: %s\n",
                sql.c_str(), mysql_error(m_pConn));
            SYLAR_LOG_ERROR(g_logger) << buf;
            return false;
        }
        return true;
    }
        //从数据库中删除所有信息
    bool Mysql::deleteAll(const std::string sql) {
        // std::string sqlStr = "delete from " + sql;
        if (mysql_query(m_pConn, sql.c_str())) {
            char buf[1024] = "";
            sprintf(buf, "Failed to delete all info(table[%s]): Error: %s\n",
                sql.c_str(), mysql_error(m_pConn));
            SYLAR_LOG_ERROR(g_logger) << buf;
            return false;
        }
        return true;
    }

    bool Mysql::insert(const std::string sql) {
        if (mysql_query(m_pConn, sql.c_str())) {
            char buf[1024] = "";
            sprintf(buf, "Failed to insert info[%s]: Error: %s\n",
                sql.c_str(), mysql_error(m_pConn));
            SYLAR_LOG_ERROR(g_logger) << buf;
            return false;
        }

        return true;
    }

    bool Mysql::join(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result,
        Table::ptr left, Table::ptr right, std::string str, bool all,
        const std::vector<std::string>& col) {
        std::string sql;/* select * from
                                left->getTableName()
                            inner join
                                right->getTableName()
                             on
                                sqlType["key"][0]["name"].asString() == sqlType["key"][1]["name"];*/
        //内联
        if (str == "inner") {
            sql = "select * from " + left->getTableName()
                + " inner join " + right->getTableName()
                + " on " + sqlType["key"][0]["name"].asString() + "=" + sqlType["key"][1]["name"].asString();
        } else {//左联
            sql = "select * from " + left->getTableName()
                + " left join " + right->getTableName()
                + " on " + sqlType["key"][0]["name"].asString() + "=" + sqlType["key"][1]["name"].asString();
        }

        //查询
        return select(sql, result, all, col);
    }
}

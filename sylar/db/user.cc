#include "user.h"
#include "log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

namespace db {
    User::User(const std::vector<std::string>& cols, IDB::ptr db)
        : m_db(db),
        m_cols(cols) {
        //默认是mysql数据库
    }

    User::~User() {}

    std::string User::getTableName() {
        //user表
        return "user";
    }
    //增加一条消息
    bool User::insertOne(const Json::Value& sqlType) {
        std::string sql;
        //构造sql语句
        if (sqlType["data"].isNull()) {
            SYLAR_LOG_ERROR(g_logger) << "no data input, please input least one data";
            return false;
        } else {
            sql = "insert into user(";
            for (auto col : sqlType["data"]["column"]) {
                sql += col.asString() + ",";
            }
            //取出最后一个逗号
            sql.pop_back();
            sql += ") values(";
            int i = 0;
            for (auto val : sqlType["data"]["value"]) {
                if (sqlType["data"]["type"][i] == "int") {
                    sql += val.asString() + ",";
                } else {
                    sql += "'" + val.asString() + "'" + ",";
                }
                ++i;
            }
            sql.pop_back();
            sql += ");";
            // remove(sql.begin(), sql.end(), '\\');
        }
        m_db->insert(sql);
        return true;
    }
    //增加多条消息
    bool User::insertMany(const Json::Value& sqlType) {
        std::string sql;
        //构造sql语句
        if (sqlType["data"].isNull()) {
            SYLAR_LOG_ERROR(g_logger) << "no data input, please input least one data";
            return false;
        } else {
            sql = "insert into user(";

            for (auto col : sqlType["data"]["column"]) {
                sql += col.asString() + ",";
            }
            //取出最后一个逗号
            sql.pop_back();
            sql += ") values";
            for (auto values : sqlType["data"]["value"]) {
                //插入一行数据
                sql += "(";
                int i = 0;
                for (auto val : values) {
                    if (sqlType["data"]["type"][i] == "int") {
                        sql += val.asString() + ",";
                    } else {
                        sql += "'" + val.asString() + "'" + ",";
                    }
                    ++i;
                }
                sql.pop_back();
                sql += "),";
            }
            sql.pop_back();
            sql += ";";
        }
        m_db->insert(sql);
        return true;
    }
    //删除key消息
    bool User::deleteInfo(const Json::Value& sqlType) {
        std::string sql = "delete from user where " + sqlType["key"].asString() + "=" + sqlType["value"].asString() + ";";
        return m_db->deleteInfo(sql);
    }
    //删除所有消息
    bool User::deleteAll() {
        std::string sql = "delete from `user`;";
        return m_db->deleteAll(sql);
    }
    //查找指定字段消息不带过滤条件
    bool User::selectColNoFilter(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result) {
        std::string sql;
        if (sqlType["column"].isNull()) {
            SYLAR_LOG_ERROR(g_logger) << "no column input, json format error";
            return false;
        } else {
            //需判断是否分组和排序
            sql = "select ";
            for (auto col : sqlType["column"]) {
                sql += col.asString() + ",";
            }
            //去除最后一个逗号
            sql.pop_back();
            sql += " from user ";
            //分组
            if (sqlType["group"]["flag"] == "true") {
                sql += " group by ";
                for (auto name : sqlType["group"]["name"]) {
                    sql += name.asString() + ",";
                }
                sql.pop_back();
                //sql += ";";
            }
            //排序
            if (sqlType["order"]["flag"] == "true") {
                sql += " order by ";
                for (auto order : sqlType["order"]["orderValue"]) {
                    sql += order["name"].asString() + " " + order["type"].asString() + ",";
                }
                sql.pop_back();
                sql += ";";
            }
        }
        // std::vector<std::unordered_map<std::string, std::string> > result;
        std::vector<std::string> cols;
        for (auto col : sqlType["column"]) {
            cols.push_back(col.asString());
        }
        m_db->select(sql, result, false, cols);
        return true;
    }
    //查找所有字段消息不带过滤条件
    bool User::selectAllNoFilter(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result) {
        std::string sql;
        //需判断是否分组和排序
        sql = "select *from user ";
        //分组
        if (sqlType["group"]["flag"] == "true") {
            sql += " group by ";
            for (auto name : sqlType["group"]["name"]) {
                sql += name.asString() + ",";
            }
            sql.pop_back();
            sql += ";";
        }
        //排序
        if (sqlType["order"]["flag"] == "true") {
            sql += " order by ";
            for (auto order : sqlType["order"]["orderValue"]) {
                sql += order["name"].asString() + " " + order["type"].asString() + ",";
            }
            sql.pop_back();
            sql += ";";
        }
        // std::vector<std::unordered_map<std::string, std::string> > result;
        m_db->select(sql, result, true);
        return true;
    }
    //查找指定字段消息带过滤条件
    bool User::selectClo(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result) {
        std::string sql;
        if (sqlType["column"].isNull()) {
            SYLAR_LOG_ERROR(g_logger) << "no column input, json format error";
            return false;
        } else {
            //需判断是否分组和排序
            sql = "select ";
            for (auto col : sqlType["column"]) {
                sql += col.asString() + ",";
            }
            //去除最后一个逗号
            sql.pop_back();
            //过滤条件
            sql += " from user where ";
            for (auto fil : sqlType["filter"]) {
                switch (fil["type"].asInt()) {
                case db::Mysql::EQUAL:
                {
                    sql += fil["name"].asString() + "=";
                    if (fil["value"]["type"] == "int") {
                        sql += fil["value"]["val"].asString();
                    } else {
                        sql += "'" + fil["value"]["val"].asString() + "'";
                    }
                    break;
                }
                case db::Mysql::NOEQUAL:
                {
                    sql += fil["name"].asString() + "<>";
                    if (fil["value"]["type"] == "int") {
                        sql += fil["value"]["val"].asString();
                    } else {
                        sql += "'" + fil["value"]["val"].asString() + "'";
                    }
                    break;
                }
                case db::Mysql::GREATER:
                {
                    sql += fil["name"].asString() + ">";
                    if (fil["value"]["type"] == "int") {
                        sql += fil["value"]["val"].asString();
                    } else {
                        sql += "'" + fil["value"]["val"].asString() + "'";
                    }
                    break;
                }
                case db::Mysql::LESS:
                {
                    sql += fil["name"].asString() + "<";
                    if (fil["value"]["type"] == "int") {
                        sql += fil["value"]["val"].asString();
                    } else {
                        sql += "'" + fil["value"]["val"].asString() + "'";
                    }
                    break;
                }
                case db::Mysql::IN:
                {
                    sql += fil["name"].asString() + "IN(" + fil["value"].asString();
                    if (fil["value"]["type"] == "int") {
                        for (auto val : fil["value"]["val"]) {
                            sql += val.asString() + ",";
                        }
                    } else {
                        for (auto val : fil["value"]["val"]) {
                            sql += "'" + val.asString() + "'" + ",";
                        }
                    }
                    sql.pop_back();
                    sql += ")";

                    break;
                }


                case db::Mysql::BETWEEN_AND:
                {
                    sql += fil["name"].asString() + " between ";
                    if (fil["value"]["val"].size() != 2) {
                        SYLAR_LOG_ERROR(g_logger) << "filter [between...and...] format error.";
                        return false;
                    }
                    if (fil["value"]["type"] == "int") {
                        sql += fil["value"]["val"][0].asString() + " and " + fil["value"]["val"][1].asString();
                    } else {
                        sql += "'" + fil["value"]["val"][0].asString() + "'" + " and " + "'" + fil["value"]["val"][1].asString() + "'";
                    }
                    break;
                }
                }
            }

            //分组
            if (sqlType["group"]["flag"] == "true") {
                sql += " group by ";
                for (auto name : sqlType["group"]["name"]) {
                    sql += name.asString() + ",";
                }
                sql.pop_back();
                // sql += ";";
            }
            //排序
            if (sqlType["order"]["flag"] == "true") {
                sql += " order by ";
                for (auto order : sqlType["order"]["orderValue"]) {
                    sql += order["name"].asString() + " " + order["type"].asString() + ",";
                }
                sql.pop_back();
                sql += ";";
            }
        }
        // std::vector<std::unordered_map<std::string, std::string> > result;
        std::vector<std::string> cols;
        for (auto col : sqlType["column"]) {
            cols.push_back(col.asString());
        }
        m_db->select(sql, result, false, cols);
        return true;
    }
    //查找所有字段消息带过滤条件
    bool User::selectAll(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result) {
        std::string sql;

            //需判断是否分组和排序
        sql = "select *from user where ";
        for (auto fil : sqlType["filter"]) {
            switch (fil["type"].asInt()) {
            case db::Mysql::EQUAL:
            {
                sql += fil["name"].asString() + "=";
                if (fil["value"]["type"] == "int") {
                    sql += fil["value"]["val"].asString();
                } else {
                    sql += "'" + fil["value"]["val"].asString() + "'";
                }
                break;
            }
            case db::Mysql::NOEQUAL:
            {
                sql += fil["name"].asString() + "<>";
                if (fil["value"]["type"] == "int") {
                    sql += fil["value"]["val"].asString();
                } else {
                    sql += "'" + fil["value"]["val"].asString() + "'";
                }
                break;
            }
            case db::Mysql::GREATER:
            {
                sql += fil["name"].asString() + ">";
                if (fil["value"]["type"] == "int") {
                    sql += fil["value"]["val"].asString();
                } else {
                    sql += "'" + fil["value"]["val"].asString() + "'";
                }
                break;
            }
            case db::Mysql::LESS:
            {
                sql += fil["name"].asString() + "<";
                if (fil["value"]["type"] == "int") {
                    sql += fil["value"]["val"].asString();
                } else {
                    sql += "'" + fil["value"]["val"].asString() + "'";
                }
                break;
            }
            case db::Mysql::IN:
            {
                sql += fil["name"].asString() + "IN(" + fil["value"].asString();
                if (fil["value"]["type"] == "int") {
                    for (auto val : fil["value"]["val"]) {
                        sql += val.asString() + ",";
                    }
                } else {
                    for (auto val : fil["value"]["val"]) {
                        sql += "'" + val.asString() + "'" + ",";
                    }
                }
                sql.pop_back();
                sql += ")";

                break;
            }

            case db::Mysql::BETWEEN_AND:
            {
                sql += fil["name"].asString() + " between ";
                if (fil["value"]["val"].size() != 2) {
                    SYLAR_LOG_ERROR(g_logger) << "filter [between...and...] format error.";
                    return false;
                }
                if (fil["value"]["type"] == "int") {
                    sql += fil["value"]["val"][0].asString() + " and " + fil["value"]["val"][1].asString();
                } else {
                    sql += "'" + fil["value"]["val"][0].asString() + "'" + " and " + "'" + fil["value"]["val"][1].asString() + "'";
                }
                break;
            }
            }
        }

        //分组
        if (sqlType["group"]["flag"] == "true") {
            sql += " group by ";
            for (auto name : sqlType["group"]["name"]) {
                sql += name.asString() + ",";
            }
            sql.pop_back();
            sql += ";";
        }
        //排序
        if (sqlType["order"]["flag"] == "true") {
            sql += " order by ";
            for (auto order : sqlType["order"]["orderValue"]) {
                sql += order["name"].asString() + " " + order["type"].asString() + ",";
            }
            sql.pop_back();
            sql += ";";
        }

        // std::vector<std::unordered_map<std::string, std::string> > result;
        m_db->select(sql, result, true);
        return true;
    }
    bool User::update(const Json::Value& sqlType) {
        std::string sql = "update user set ";
        for (auto data : sqlType["data"]) {
            if (data["type"] == "int") {
                sql += data["name"].asString() + "=" + data["value"].asString() + ",";
            } else {
                sql += data["name"].asString() + "=" + "'" + data["value"].asString() + "'" + ",";
            }
        }
        sql.pop_back();
        sql += " where " + sqlType["key"]["name"].asString() + " = ";
        if (sqlType["key"]["type"] == "int") {
            sql += sqlType["key"]["value"].asString();
        } else {
            sql += "'" + sqlType["key"]["value"].asString() + "';";
        }
        return m_db->update(sql);
    }
    //内联表
    bool User::innerJoinClo(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) {
        std::string sql;
        if (sqlType["column"].isNull()) {
            SYLAR_LOG_ERROR(g_logger) << "no column input, json format error";
            return false;
        }
        // } else {
        //     //需判断是否分组和排序
        //     sql = "select ";
        //     for (auto col : sqlType["column"]) {
        //         sql += col.asString() + ",";
        //     }
        //     //去除最后一个逗号
        //     sql.pop_back();
        //     sql += " from user inner join " + right->getTableName() + " on " +
        //         sqlType["key"][0]["name"].asString() + "=" + sqlType["key"][1]["name"].asString();
        // }
        // std::vector<std::unordered_map<std::string, std::string> > result;
        std::vector<std::string> cols;
        for (auto col : sqlType["column"]) {
            cols.push_back(col.asString());
        }
        // m_db->select(sql, result, false, cols);
        // return true;

        User::ptr left(this);
        return m_db->join(sql, result, left, right, "inner", false, cols);
    }
    //左联表
    bool User::leftJoinClo(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) {
        User::ptr left(this);
        std::vector<std::string> cols;
        for (auto col : sqlType["column"]) {
            cols.push_back(col.asString());
        }

        return m_db->join(sqlType, result, left, right, "left", false, cols);
    }
    bool User::innerJoinAll(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) {
        User::ptr left(this);

        return m_db->join(sqlType, result, left, right, "inner", true);
    }
    //左联表
    bool User::leftJoinAll(const Json::Value& sqlType,
        std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) {
        User::ptr left(this);

        return m_db->join(sqlType, result, left, right, "left", true);
    }
}
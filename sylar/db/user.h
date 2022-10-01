#ifndef __USER_H__
#define __USER_H__

#include "table.h"
#include "myMysql.h"
// #include "db.h"

namespace db {
    class User : public Table {
    public:
        typedef std::shared_ptr<User> ptr;
    public:
        User(const std::vector<std::string>& cols = std::vector<std::string>({
             "id", "nickname","passward","email","gender","phone", "state" }),
             db::IDB::ptr db = std::make_shared<Mysql>());
        ~User();

        virtual std::string getTableName();
        //增加一条消息
        virtual bool insertOne(const Json::Value& sqlType);
        //增加多条消息
        virtual bool insertMany(const Json::Value& sqlType);
        //删除一条消息
        virtual bool deleteInfo(const Json::Value& sqlType);
        //删除所有消息
        virtual bool deleteAll();
        //查找指定字段消息不带过滤条件
        virtual bool selectColNoFilter(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result);
            //查找所有字段消息不带过滤条件
        virtual bool selectAllNoFilter(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result);
            //查找指定字段消息带过滤条件
        virtual bool selectClo(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result);
            //查找所有字段消息带过滤条件
        virtual bool selectAll(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result);
        virtual bool update(const Json::Value& sqlType);
        //内联表
        virtual bool innerJoinClo(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right);
            //左联表
        virtual bool leftJoinClo(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right);
        virtual bool innerJoinAll(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right);
            //左联表
        virtual bool leftJoinAll(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right);
    private:
        IDB::ptr m_db;
        std::vector<std::string> m_cols;
    };
}


#endif
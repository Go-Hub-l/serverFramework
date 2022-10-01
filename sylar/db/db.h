#ifndef __DB_H__
#define __DB_H__

#include <string>
#include <vector>
#include "json/json.h"
#include <unordered_map>
#include <memory>
#include "table.h"


/*
    定义通用的数据库接口
*/
namespace db {

    class Table;

    class IDB {
    public:
        typedef std::shared_ptr<IDB> ptr;
    public:
        IDB() {}
        virtual ~IDB() {}
        //连接数据库
        // virtual bool connect(const std::string server, const std::string user,
        //     const std::string password, const std::string dbName) = 0;
        // //断开连接数据库
        // virtual void disConnect() = 0;
        // //创建表格
        // // virtual bool createTable(std::string sql, std::string tableName) = 0;
        // //更新数据库
        // virtual bool update(const Json::Value& sql) = 0;
        // virtual bool update(const std::string& sql) = 0;
        // //从数据库中查询信息
        // virtual bool select(const Json::Value& sql, std::vector<std::unordered_map<std::string, std::string> >& result) = 0;
        // virtual bool select(const std::string& sql,
        //     std::vector<std::unordered_map<std::string, std::string> >& result,
        //     const std::vector<std::string>& col = std::vector<std::string>(),
        //     bool all = false) = 0;
        // //从数据库中删除指定信息
        // virtual bool deleteInfo(const Json::Value& sql) = 0;
        // virtual bool deleteInfo(const std::string& sql) = 0;
        // //从数据库中删除所有信息
        // virtual bool deleteAll(const std::string sql) = 0;
        // //插入数据
        // virtual bool insert(const std::string sql) = 0;
        // //联表查询:默认左联
        // virtual bool join(const Json::Value& sql,
        //     std::vector<std::unordered_map<std::string, std::string> >& result,
        //     std::string str = "inner") = 0;

        virtual bool connect(const std::string server, const std::string user,
            const std::string password, const std::string dbName) = 0;
        //断开连接数据库
        virtual void disConnect() = 0;
        //创建表格
        // bool createTable(std::string sql, std::string tableName);
        //更新数据库
        virtual bool update(const Json::Value& sql) = 0;
        virtual bool update(const std::string& sql) = 0;
        //从数据库中查询信息
        virtual bool select(const Json::Value& sql,
            std::vector<std::unordered_map<std::string, std::string> >& result,
            bool all = false, const std::vector<std::string>& col = std::vector<std::string>()) = 0;
        virtual bool select(const std::string& sql,
            std::vector<std::unordered_map<std::string, std::string> >& result,
            bool all = false, const std::vector<std::string>& col = std::vector<std::string>()) = 0;
        //从数据库中删除指定信息
        virtual bool deleteInfo(const Json::Value& sql) = 0;
        virtual bool deleteInfo(const std::string& sql) = 0;
        //从数据库中删除所有信息
        virtual bool deleteAll(const std::string sql) = 0;
        virtual bool insert(const std::string sql) = 0;
        //联表查询:默认左联
        virtual bool join(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result,
            db::Table::ptr left, db::Table::ptr right, std::string str = "inner", bool all = false,
            const std::vector<std::string>& col = std::vector<std::string>()) = 0;
    };

}


#endif
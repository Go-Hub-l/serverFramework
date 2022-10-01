#ifndef __MYMYSQL_H__
#define __MYMYSQL_H__

#include <string>
#include <mysql/mysql.h>
#include <vector>
#include <unordered_map>

#include "db.h"
// #include "table.h"


/*
    定义通用的数据库接口
1 查找接口：需要传入一个json对象，来决定查询的字段值
{
    "name":要查找的表名
    "all": true,//查找所有字段，设置该字段为true时，直接select *from tableName
    "column":[...]//列数组,
    "where":true,//过滤条件
    "filter":{
        "name":[
            "type": "",
            "value": ""
        ]
    }
    "order":"ASC", //排序  ASC:升序， DESC:降序

}

2 更新接口
{
    "name":要更新的表名
    "column":{
        "name": "",
        "value":""
    }//要更新的列,
    "where":true,//过滤条件
    "filter":{
        "name":[
            "type": "",
            "value": ""
        ]
    }

    }
}

3 删除接口
{
    "name":要更新的表名
    "where":true,//过滤条件
    "valueType": "string",//值的格式【int】【string】
    "filter":{
        [
            "name": ""
            "type": "",
            "value": ""
            "valueType": "string", //值的格式
        ]
    }

    }
}
*/

namespace db {
    class Table;
    class Mysql : public db::IDB {
    public:
        typedef std::shared_ptr<Mysql> ptr;
    public:
        enum filterType {//后续需要再加
            EQUAL,//等于
            NOEQUAL,//不等于
            GREATER,//大于
            LESS,//小于
            IN,//IN
            BETWEEN_AND,//between_and
        };
    public:
        Mysql();
        ~Mysql();
        //连接数据库
        bool connect(const std::string server, const std::string user,
            const std::string password, const std::string dbName);
        //断开连接数据库
        void disConnect();
        //创建表格
        // bool createTable(std::string sql, std::string tableName);
        //更新数据库
        bool update(const Json::Value& sql);
        bool update(const std::string& sql);
        //从数据库中查询信息
        bool select(const Json::Value& sql,
            std::vector<std::unordered_map<std::string, std::string> >& result,
            bool all, const std::vector<std::string>& col);
        bool select(const std::string& sql,
            std::vector<std::unordered_map<std::string, std::string> >& result,
            bool all = false, const std::vector<std::string>& col = std::vector<std::string>());
           //从数据库中删除指定信息
        virtual bool deleteInfo(const Json::Value& sql);
        virtual bool deleteInfo(const std::string& sql);
        //从数据库中删除所有信息
        virtual bool deleteAll(const std::string sql);
        virtual bool insert(const std::string sql);
        //联表查询:默认左联
        virtual bool join(const Json::Value& sql,
            std::vector<std::unordered_map<std::string, std::string> >& result,
            Table::ptr left, Table::ptr right, std::string str = "inner", bool all = false,
            const std::vector<std::string>& col = std::vector<std::string>());
    private:
        MYSQL* m_pConn;
    };
}


#endif
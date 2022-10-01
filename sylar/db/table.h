#ifndef __TABLE_H__
#define __TABLE_H__

// #include "myMysql.h"
#include "json/json.h"
#include <vector>
#include <unordered_map>
#include <string>

/*
    过滤条件：EQUAL  NOEQUAL  LESS GREATE BETWEEN_AND IN NOT_IN
1 查找指定字段不带过滤条件
{
    "column":[...],
    "group":{
        "flag": "false",
        "name": [...]//分组依据的字段：flag为true才有效
    },//是否分组
    "order":{
        "flag": "true",
        orderValue:[
            {"name": "","type":"asc/desc"},//不区分大小写
            ...
            ]
    }
}
2 查找所有字段不带过滤条件
{
    "group":{
        "flag": "false",
        "name": [...]//分组依据的字段：flag为true才有效
    },//是否分组
    "order":{
        "flag": "true",
        orderValue:[
            {"name": "","type":"asc/desc"},//不区分大小写
            ...
            ]
    }
}
3 查找指定字段带过滤条件
{
    "column":[...],
    "group":{
        "flag": "false",
        "name": [...]//分组依据的字段：flag为true才有效
    },//是否分组
    "filter":[
        {"name": "", "type": "EQUAL/NOEQUAL", "value": {"type": "int/string", "val": ""}},
        ...
    ]
    "order":{
        "flag": "true",
        orderValue:[
            {"name": "","type":"asc/desc"},//不区分大小写
            ...
            ]
    }
}
4 查找所有字段带过滤条件
{
    "group":{
        "flag": "false",
        "name": [...]//分组依据的字段：flag为true才有效
    },//是否分组
    "filter":[
        {"name": "", "type": "", "value": {"type":"int/string", "val": ""}},
        ...
    ]
    "order":{
        "flag": "false",
        orderValue:[
            {"name": "","type":"asc/desc"},//不区分大小写
            ...
            ]//排序依据：flag为true才有效
    }
}
5 插入一条数据
{
    "data": {
        "column":[...],
        "type":["int/string", ...]
        "value":[...]
        }
}
6 插入多条数据
{
    "data": {
        "column":[...],
        "type":["int/string", ...]
        "value":[
            [....],
            ....
        ]
    }
}
7 删除关键字key对应的数据
{
    "key": "",//要删除信息的关键字字段
    "value": ""//要删除信息的关键字值
}
8 删除所有数据
{
    无
}
9 更改关键字key对应的数据
{
    "key":[
        {"name":"", "type": "int/string", "value": "",//关键字对应的值},
        ...
    ],
    "data":[//要更新的数据
        {"name": "", "type": "int/string", "value": ""},
        ...
    ]
}
10 内联
{
    "column": [...],
    "key":[
        {"name": ""},//左表 下标0
        {"name": ""} //右表 下标1
    ]
}
11 左联
{
    "column": [...],
    "key":[
        {"name": ""},//左表 下标0
        {"name": ""}//右表  下标1
    ]
}
*/

namespace db {
    class Table {
    public:
        typedef std::shared_ptr<Table> ptr;
    public:
        virtual ~Table() {};
        //获取表名
        virtual std::string getTableName() = 0;
        //增加一条消息
        virtual bool insertOne(const Json::Value& sqlType) = 0;
        //增加多条消息
        virtual bool insertMany(const Json::Value& sqlType) = 0;
        //删除一条消息
        virtual bool deleteInfo(const Json::Value& sqlType) = 0;
        //删除所有消息
        virtual bool deleteAll() = 0;
        //查找指定字段消息不带过滤条件
        virtual bool selectColNoFilter(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result) = 0;
            //查找所有字段消息不带过滤条件
        virtual bool selectAllNoFilter(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result) = 0;
            //查找指定字段消息带过滤条件
        virtual bool selectClo(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result) = 0;
            //查找所有字段消息带过滤条件
        virtual bool selectAll(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result) = 0;
        virtual bool update(const Json::Value& sqlType) = 0;
        //内联表
        virtual bool innerJoinClo(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) = 0;
            //左联表
        virtual bool leftJoinClo(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) = 0;
        virtual bool innerJoinAll(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) = 0;
            //左联表
        virtual bool leftJoinAll(const Json::Value& sqlType,
            std::vector<std::unordered_map<std::string, std::string> >& result, Table::ptr right) = 0;
    };
}


#endif
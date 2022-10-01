#include "db/user.h"
#include "json/json.h"
#include <vector>
#include <unordered_map>
#include "log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_select() {
    Json::Value sqlType;
    Json::Value order;
    Json::Value val;
    Json::Value fil;

    sqlType["column"].append("id");
    sqlType["column"].append("nickname");
    sqlType["column"].append("passward");
    sqlType["column"].append("email");
    sqlType["column"].append("phone");
    val["name"] = "id";
    val["type"] = "asc";
    order["flag"] = "true";
    order["orderValue"].append(val);
    sqlType["order"] = order;
    fil["name"] = "state";
    fil["type"] = db::Mysql::EQUAL;
    fil["value"]["type"] = "int";
    fil["value"]["val"] = 0;

    sqlType["filter"].append(fil);


    std::vector<std::unordered_map<std::string, std::string> > res;

    db::User user;

    if (!user.selectClo(sqlType, res)) {
        SYLAR_LOG_ERROR(g_logger) << "select failed";
    }

    //输出查询到的结果
    for (std::unordered_map<std::string, std::string> val : res) {
        SYLAR_LOG_INFO(g_logger) << val["id"] << " "
            << val["nickname"] << " "
            << val["passward"] << " "
            << val["email"] << " "
            // << val["gender"] << " "
            << val["phone"] << " ";
            // << val["state"] << " ";
    }
}

void test_selectAll() {
    Json::Value sqlType;
    Json::Value group;
    Json::Value fil;
    Json::Value val;
    std::vector<std::unordered_map<std::string, std::string> > res;

    fil["name"] = "passward";
    fil["type"] = db::Mysql::EQUAL;
    fil["value"]["type"] = "string";
    fil["value"]["val"] = "55555";

    sqlType["filter"].append(fil);

    db::User user;

    if (!user.selectAllNoFilter(sqlType, res)) {
        SYLAR_LOG_ERROR(g_logger) << "select failed";
    }

    //输出查询到的结果
    for (std::unordered_map<std::string, std::string> val : res) {
        SYLAR_LOG_INFO(g_logger) << val["id"] << " "
            << val["nickname"] << " "
            << val["passward"] << " "
            << val["email"] << " "
            // << val["gender"] << " "
            << val["phone"] << " ";
            // << val["state"] << " ";
    }
}

void test_insert() {
    Json::Value sqlType;
    Json::Value group;
    Json::Value order;
    Json::Value data;

    // sqlType["data"]["column"].append("nickname");
    // sqlType["data"]["column"].append("passward");
    // sqlType["data"]["column"].append("email");
    // sqlType["data"]["column"].append("gender");
    // sqlType["data"]["column"].append("phone");
    // sqlType["data"]["column"].append("state");

    // sqlType["data"]["value"].append("jingjing3");
    // sqlType["data"]["value"].append("58555");
    // sqlType["data"]["value"].append("1080156146@qq.com");
    // sqlType["data"]["value"].append(1);
    // sqlType["data"]["value"].append("18886784567");
    // sqlType["data"]["value"].append(1);

    // sqlType["data"]["type"].append("string");
    // sqlType["data"]["type"].append("string");
    // sqlType["data"]["type"].append("string");
    // sqlType["data"]["type"].append("int");
    // sqlType["data"]["type"].append("string");
    // sqlType["data"]["type"].append("int");

    //插入多行
    sqlType["data"]["column"].append("nickname");
    sqlType["data"]["column"].append("passward");
    sqlType["data"]["column"].append("email");
    sqlType["data"]["column"].append("gender");
    sqlType["data"]["column"].append("phone");
    sqlType["data"]["column"].append("state");



    sqlType["data"]["type"].append("string");
    sqlType["data"]["type"].append("string");
    sqlType["data"]["type"].append("string");
    sqlType["data"]["type"].append("int");
    sqlType["data"]["type"].append("string");
    sqlType["data"]["type"].append("int");


    data.append("jingjing7");
    data.append("55555");
    data.append("1030146146@qq.com");
    data.append(1);
    data.append("19886484567");
    data.append(1);
    sqlType["data"]["value"].append(data);
    data.clear();

    // data.append("jingjing5");
    // data.append("585555");
    // data.append("1080155146@qq.com");
    // data.append(1);
    // data.append("18886754567");
    // data.append(1);
    // sqlType["data"]["value"].append(data);
    // data.clear();

    // data.append("jingjing6");
    // data.append("58556");
    // data.append("1080656146@qq.com");
    // data.append(1);
    // data.append("18866784567");
    // data.append(1);
    // sqlType["data"]["value"].append(data);
    // data.clear();


    std::vector<std::unordered_map<std::string, std::string> > res;
    db::User user;
    user.insertMany(sqlType);

    test_select();
}


void test_delete() {
    Json::Value sqlType;

    sqlType["key"] = "id";
    sqlType["value"] = 4;
    db::User user;
    // user.deleteInfo(sqlType);

    user.deleteAll();

    test_selectAll();
}


int main() {
    // db::Mysql mysql;

    // if (!mysql.connect("172.26.85.62", "root", "XIAObing321.", "chat")) {
    //     SYLAR_LOG_ERROR(g_logger) << "connect chat failed";
    // } else {
    //     SYLAR_LOG_INFO(g_logger) << "connect chat success";
    // }

    test_delete();

    return 0;
}
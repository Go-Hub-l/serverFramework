#include "log.h"
#include "config.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

#if 0
sylar::ConfigVar<int>::ptr g_int_value_config =
    sylar::Config::Lookup("system.port", (int)8080, "system port");

sylar::ConfigVar<float>::ptr g_float_value_config =
    sylar::Config::Lookup("system.value", (float)10.2f, "system value");

sylar::ConfigVar<std::vector<int>>::ptr g_vec_int_value_config =
    sylar::Config::Lookup("system.vec_int", std::vector<int>{10, 20}, "system vec value");

sylar::ConfigVar<std::list<int>>::ptr g_list_int_value_config =
    sylar::Config::Lookup("system.list_int", std::list<int>{100, 200}, "system list value");

sylar::ConfigVar<std::set<int>>::ptr g_set_int_value_config =
    sylar::Config::Lookup("system.set_int", std::set<int>{50, 100}, "system set value");

sylar::ConfigVar<std::unordered_set<int> >::ptr g_uset_int_value_config =
    sylar::Config::Lookup("system.uset_int", std::unordered_set<int>{150, 100}, "system uset value");

sylar::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_config =
    sylar::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"k", 8}}, "system str_int_map value");

sylar::ConfigVar<std::unordered_map<std::string, int> >::ptr g_str_int_umap_config =
    sylar::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"a", 1}}, "system str_int_unordered_map value");

void print_yaml(const YAML::Node &node, int level)
{
    if (node.IsScalar())
    {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                         << node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if (node.IsNull())
    {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                         << "NULL - " << node.Type() << " - " << level;
    }
    else if (node.IsMap())
    {
        for (auto it = node.begin();
             it != node.end(); ++it)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                             << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    }
    else if (node.IsSequence())
    {
        for (size_t i = 0; i < node.size(); ++i)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                                             << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml()
{
    YAML::Node root = YAML::LoadFile("/root/CPP/sylar/workspace/sylar/bin/conf/test.yml");

    print_yaml(root, 0);
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root;
}

void test_config() 
{
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before:" << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before:" << g_float_value_config->toString();

#define XX(g_value, name, prefix)                                                                     \
    {                                                                                                 \
        auto v = g_value->getValue();                                                                 \
        for (auto &i : v)                                                                             \
        {                                                                                             \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix "   " #name << ": " << i;                     \
        }                                                                                             \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix "   " #name << "  yaml: " << g_value->toString(); \
    }

#define XX_M(g_value, name, prefix)                                                                   \
    {                                                                                                 \
        auto v = g_value->getValue();                                                                 \
        for (auto &i : v)                                                                             \
        {                                                                                             \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix "   " #name << "{" << i.first                 \
                                             << "-" << i.second << "}";                               \
        }                                                                                             \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix "   " #name << "  yaml: " << g_value->toString(); \
    }

    XX(g_vec_int_value_config, vec_int, before);
    XX(g_list_int_value_config, list_int, before);
    XX(g_set_int_value_config, set_int, before);
    XX(g_uset_int_value_config, uset_int, before);
    XX_M(g_str_int_map_config, str_int_map, before);
    XX_M(g_str_int_umap_config, str_int_umap, before);

    YAML::Node root = YAML::LoadFile("/root/CPP/sylar/workspace/sylar/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);

    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after:" << g_int_value_config->getValue();
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after:" << g_float_value_config->toString();
    XX(g_vec_int_value_config, vec_int, after);
    XX(g_list_int_value_config, list_int, after);
    XX(g_set_int_value_config, set_int, after);
    XX(g_uset_int_value_config, uset_int, after);
    XX_M(g_str_int_map_config, str_int_map, after);
    XX_M(g_str_int_umap_config, str_int_umap, after);
}

class Person {
public:
    std::string m_name = "";
    int m_age = 18;
    bool m_sex = 0;

    std::string toString() const{
        std::stringstream ss;

        ss << "name:" << m_name << "age:" << m_age << "sex:" << m_sex;
        return ss.str();
    }

    bool operator== (const Person& p) const{
        return m_age == p.m_age
            && m_name == p.m_name
            && m_sex == p.m_sex;
    }
};

namespace sylar {
    // 自定义person类: FromStr
    template <>
    class LexicalCast<std::string, Person> {
    public:
        Person operator()(const std::string& val) {
            YAML::Node node = YAML::Load(val);
            Person p;

            p.m_name = node["name"].as<std::string>();
            p.m_age = node["age"].as<int>();
            p.m_sex = node["sex"].as<bool>();

            return p;
        }
    };
    // 自定义person类: ToStr
    template <>
    class LexicalCast<Person, std::string> {
    public:
        std::string operator()(const Person& val) {
            YAML::Node node;
            std::stringstream ss;
            
            node["name"] = val.m_name;
            node["age"] = val.m_age;
            node["sex"] = val.m_sex;

            ss << node;
            return ss.str();
        }
    };
}

sylar::ConfigVar<Person>::ptr g_person =
    sylar::Config::Lookup("class.person", Person(), "system Person");

sylar::ConfigVar<std::map<std::string, Person>>::ptr g_person_map =
sylar::Config::Lookup("class.map", std::map<std::string, Person>(), "system map");

sylar::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map =
sylar::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person> >(), "system vec map");

void test_class() {
#define XX_PM(g_val, perfix) \
    { auto v = g_val->getValue(); \
        for (auto& i : v) {\
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #perfix "   "<< "{"\
                << i.first << "-" << i.second.toString() << "}";\
        }\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "size:" << v.size();\
    }

#define XX(g_value, name, prefix)                                                                     \
    {                                                                                                 \
        auto v = g_value->getValue();                                                                 \
        for (auto &i : v)                                                                             \
        {                                                                                             \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix "   " #name << ": " << i;                     \
        }                                                                                             \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix "   " #name << "  yaml: " << g_value->toString(); \
    }

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_person->toString();
    g_person->addListener([](const Person& old_value, const Person& new_value) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "old_value=" << old_value.toString()
            << " new_value=" << new_value.toString();
        });
    XX_PM(g_person_map, before);
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before:" << g_person_vec_map->toString();
    YAML::Node root = YAML::LoadFile("/root/CPP/sylar/workspace/sylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_person->toString();
    XX_PM(g_person_map, after);
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after:" << g_person_vec_map->toString();


}
#endif

void test_log() {
    static sylar::Logger::ptr system_log = SYLAR_LOG_NAME("system");
    SYLAR_LOG_INFO(system_log) << "hello system" << std::endl;
    std::cout << sylar::loggerMgr::GetInstance()->toYamlString() << std::endl;
    YAML::Node root = YAML::LoadFile("/root/CPP/sylar/workspace/sylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);
    std::cout << "=============" << std::endl;
    std::cout << sylar::loggerMgr::GetInstance()->toYamlString() << std::endl;
    std::cout << "=============" << std::endl;
    std::cout << root << std::endl;
    SYLAR_LOG_INFO(system_log) << "hello system" << std::endl;

    system_log->setFormatter("%d - %m%n");
    SYLAR_LOG_INFO(system_log) << "hello system" << std::endl;
}

int main(int argc, char** argv)
{
    // test_yaml();
    //test_config();
    //test_class();
    test_log();
    return 0;
}
// #include "config.h"
// #include "config.h"
// #include "env.h"
// #include "util.h"
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>

// namespace sylar
// {

//     static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
//     Config::ConfigVarMap Config::s_datas;

//     ConfigVarBase::ptr Config::LookupBase(const std::string &name)
//     {
//         RWMutexType::ReadLock lock(GetMutex());
//         auto it = GetDatas().find(name);
//         return it == GetDatas().end() ? nullptr : it->second;
//     }

//     static void ListAllMember(const std::string &prefix,
//                               const YAML::Node &node,
//                               std::list<std::pair<std::string, const YAML::Node>> &output)
//     {
//         if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
//         {
//             SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Config invalid name:" << prefix << " : " << node;
//             return;
//         }
//         output.push_back(std::make_pair(prefix, node));
//         if (node.IsMap())
//         {
//             for (auto it = node.begin();
//                  it != node.end(); ++it)
//             {
//                 ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
//             }
//         }
//     }

//     void Config::LoadFromYaml(const YAML::Node &root)
//     {
//         std::list<std::pair<std::string, const YAML::Node>> all_nodes;
//         ListAllMember("", root, all_nodes);

//         for (auto &i : all_nodes)
//         {
//             std::string key = i.first;
//             if (key.empty())
//             {
//                 continue;
//             }

//             std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//             ConfigVarBase::ptr var = LookupBase(key);

//             if (var)
//             {
//                 if (i.second.IsScalar())
//                 {
//                     var->fromString(i.second.Scalar());
//                 }
//                 else
//                 {
//                     std::stringstream ss;
//                     ss << i.second;
//                     var->fromString(ss.str()); //因为已经通过偏特化将vector等其他容器序列化了，过可以设置
//                 }
//             }
//         }
//     }

//     static std::map<std::string, uint64_t> s_file2modifytime;
//     static sylar::Mutex s_mutex;

//     void Config::LoadFromConfDir(const std::string& path, bool force) {
//         std::string absoulte_path = sylar::EnvMgr::GetInstance()->getAbsolutePath(path);
//         std::vector<std::string> files;
//         FSUtil::ListAllFile(files, absoulte_path, ".yml");

//         for (auto& i : files) {
//             {
//                 struct stat st;
//                 lstat(i.c_str(), &st);
//                 sylar::Mutex::Lock lock(s_mutex);
//                 if (!force && s_file2modifytime[i] == (uint64_t) st.st_mtime) {
//                     continue;
//                 }
//                 s_file2modifytime[i] = st.st_mtime;
//             }
//             try {
//                 YAML::Node root = YAML::LoadFile(i);
//                 LoadFromYaml(root);
//                 SYLAR_LOG_INFO(g_logger) << "LoadConfFile file="
//                     << i << " ok";
//             } catch (...) {
//                 SYLAR_LOG_ERROR(g_logger) << "LoadConfFile file="
//                     << i << " failed";
//             }
//         }
//     }

//     void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
//         RWMutexType::ReadLock lock(GetMutex());
//         ConfigVarMap& m = GetDatas();
//         for (auto it = m.begin();
//             it != m.end(); ++it) {
//             cb(it->second);
//         }

//     }
// }




#include "config.h"
#include "env.h"
#include "util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace sylar {

    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

    ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
        RWMutexType::ReadLock lock(GetMutex());
        auto it = GetDatas().find(name);
        return it == GetDatas().end() ? nullptr : it->second;
    }

    //"A.B", 10
    //A:
    //  B: 10
    //  C: str

    static void ListAllMember(const std::string& prefix,
        const YAML::Node& node,
        std::list<std::pair<std::string, const YAML::Node> >& output) {
        if (prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
            != std::string::npos) {
            SYLAR_LOG_ERROR(g_logger) << "Config invalid name: " << prefix << " : " << node;
            return;
        }
        output.push_back(std::make_pair(prefix, node));
        if (node.IsMap()) {
            for (auto it = node.begin();
                it != node.end(); ++it) {
                ListAllMember(prefix.empty() ? it->first.Scalar()
                    : prefix + "." + it->first.Scalar(), it->second, output);
            }
        }
    }

    void Config::LoadFromYaml(const YAML::Node& root) {
        std::list<std::pair<std::string, const YAML::Node> > all_nodes;
        ListAllMember("", root, all_nodes);

        for (auto& i : all_nodes) {
            std::string key = i.first;
            if (key.empty()) {
                continue;
            }

            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            ConfigVarBase::ptr var = LookupBase(key);

            if (var) {
                if (i.second.IsScalar()) {
                    var->fromString(i.second.Scalar());
                } else {
                    std::stringstream ss;
                    ss << i.second;
                    var->fromString(ss.str());
                }
            }
        }
    }

    static std::map<std::string, uint64_t> s_file2modifytime;
    static sylar::Mutex s_mutex;

    void Config::LoadFromConfDir(const std::string& path, bool force) {
        //配置文件所在文件夹的绝对路径
        std::string absoulte_path = sylar::EnvMgr::GetInstance()->getAbsolutePath(path);
        std::vector<std::string> files;
        //将所有的配置文件的绝对路径都保存到files数组中
        FSUtil::ListAllFile(files, absoulte_path, ".yml");

        for (auto& i : files) {
            {
                struct stat st;
                lstat(i.c_str(), &st);
                sylar::Mutex::Lock lock(s_mutex);
                //记录每个配置文件的修改时间
                if (!force && s_file2modifytime[i] == (uint64_t) st.st_mtime) {
                    continue;
                }
                s_file2modifytime[i] = st.st_mtime;
            }
            try {
                //加载该配置文件
                YAML::Node root = YAML::LoadFile(i);
                LoadFromYaml(root);
                SYLAR_LOG_INFO(g_logger) << "LoadConfFile file="
                    << i << " ok";
            } catch (...) {
                SYLAR_LOG_ERROR(g_logger) << "LoadConfFile file="
                    << i << " failed";
            }
        }
    }

    void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
        RWMutexType::ReadLock lock(GetMutex());
        ConfigVarMap& m = GetDatas();
        for (auto it = m.begin();
            it != m.end(); ++it) {
            cb(it->second);
        }

    }

}

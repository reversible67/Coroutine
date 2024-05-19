/*
实现配置模块
2024/5/18 11:50
by 六七
*/
#include "config.h"

namespace duan{

// 静态数据成员 需要初始化
Config::ConfigVarMap Config::s_datas;


//"A.B", 10
//A:
//  B: 10
//  C: str

static void ListAllMember(const std::string& prefix,
                          const YAML::Node node,
                          std::list<std::pair<std::string, const YAML::Node> >& output){
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos){
        DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
    }
}

void Config::LoadFromYaml(const YAML::Node& root){
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    ListAllMember("", root, all_nodes);
}

} // end of namespace 
/*
配置模块测试
2024/5/18 11:48
by 六七
*/
#include "../duan_src/config.h"
#include "../duan_src/log.h"
#include <yaml-cpp/yaml.h>

duan::ConfigVar<int>::ptr g_int_value_config = 
    duan::Config::Lookup("system.port", (int)8080, "system port");

duan::ConfigVar<float>::ptr g_float_value_config = 
    duan::Config::Lookup("system.port", (float)10.2f, "system port");

void print_yaml(const YAML::Node& node, int level) {
    if(node.IsScalar()){
        DUAN_LOG_INFO(DUAN_LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if(node.IsNull()){
       DUAN_LOG_INFO(DUAN_LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - " << node.Type() << " - " << level; 
    }
    else if(node.IsMap()){
        for(auto it = node.begin(); it != node.end(); ++it){
           DUAN_LOG_INFO(DUAN_LOG_ROOT()) << std::string(level * 4, ' ') 
                << it->first << " - " << it->second.Type() << " - " << level; 
           print_yaml(it->second, level + 1);
        }
    }
    else if(node.IsSequence()){
        for(size_t i = 0; i < node.size(); ++i){
            DUAN_LOG_INFO(DUAN_LOG_ROOT()) << std::string(level * 4, ' ') 
                << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml(){
    YAML::Node root  = YAML::LoadFile("../bin/conf/log.yml");
    // 解析
    print_yaml(root, 0);

    // DUAN_LOG_INFO(DUAN_LOG_ROOT()) << root;
}

int main(int argc, char** argv){
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << g_int_value_config->getValue();
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << g_int_value_config->toString();

    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << g_float_value_config->getValue();
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << g_float_value_config->toString();

    test_yaml();
    return 0;
}
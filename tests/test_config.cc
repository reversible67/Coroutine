/*
配置模块测试
2024/5/18 11:48
by 六七
*/
#include "../duan_src/config.h"
#include "../duan_src/log.h"
#include <yaml-cpp/yaml.h>

// 把配置写死   约定
/*
duan::ConfigVar<int>::ptr g_int_value_config = 
    duan::Config::Lookup("system.port", (int)8080, "system port");

duan::ConfigVar<float>::ptr g_float_value_config = 
    duan::Config::Lookup("system.port", (float)10.2f, "system port");

duan::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config = 
    duan::Config::Lookup("system.int_vec", std::vector<int>{1, 2}, "system int vec");

duan::ConfigVar<std::list<int> >::ptr g_int_list_value_config = 
    duan::Config::Lookup("system.int_list", std::list<int>{1, 2}, "system int list");

duan::ConfigVar<std::set<int> >::ptr g_int_set_value_config = 
    duan::Config::Lookup("system.int_set", std::set<int>{1, 2}, "system int set");

duan::ConfigVar<std::unordered_set<int> >::ptr g_int_uset_value_config = 
    duan::Config::Lookup("system.int_uset", std::unordered_set<int>{1, 2}, "system int uset");

duan::ConfigVar<std::map<std::string, int> >::ptr g_str_int_map_value_config = 
    duan::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"k", 2}}, "system str int map");

duan::ConfigVar<std::unordered_map<std::string, int> >::ptr g_str_int_umap_value_config = 
    duan::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"k", 2}}, "system str int umap");
*/

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
    YAML::Node root  = YAML::LoadFile("../bin/conf/test.yml");
    // 解析
    print_yaml(root, 0);

    // DUAN_LOG_INFO(DUAN_LOG_ROOT()) << root;
}

// 测试一下config   用yaml进行配置
/*
void test_config(){
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "before: " << g_float_value_config->toString();
// 写个宏  更方便测试容器
#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v){ \
            DUAN_LOG_INFO(DUAN_LOG_ROOT()) << #prefix " " #name ": " << i;} \
        DUAN_LOG_INFO(DUAN_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
    }

#define XX_M(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v){ \
            DUAN_LOG_INFO(DUAN_LOG_ROOT()) << #prefix " " #name ": {" \
                    << i.first << " - " << i.second << "}";} \
        DUAN_LOG_INFO(DUAN_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
    }

    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_uset_value_config, int_uset, before);
    // 针对map需要重新写一个宏 因为它不是顺序容器
    XX_M(g_str_int_map_value_config, str_int_map, before);
    XX_M(g_str_int_umap_value_config, str_int_umap, before);

    YAML::Node root  = YAML::LoadFile("../bin/conf/test.yml");
    duan::Config::LoadFromYaml(root);

    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "after: " << g_float_value_config->toString();

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uset, after);
    XX_M(g_str_int_map_value_config, str_int_map, after);
    XX_M(g_str_int_umap_value_config, str_int_umap, after);
}
*/

class Person{
public:
    Person() {};
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const{
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << " age=" << m_age
           << " sex=" << m_sex
           << "]";
        return ss.str();
    }

    bool operator== (const Person& oth) const {
        return m_name == oth.m_name
            && m_age == oth.m_age
            && m_sex == oth.m_sex;
    }
};

namespace duan{

template<>
class LexicalCast<std::string, Person>{
public:
    Person operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        Person p;                            
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string>{
public:
    std::string operator()(const Person& p){
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}

duan::ConfigVar<Person>::ptr g_person = 
    duan::Config::Lookup("class.person", Person(), "system person");

duan::ConfigVar<std::map<std::string, Person> >::ptr g_person_map = 
    duan::Config::Lookup("class.map", std::map<std::string, Person>(), "system map");

duan::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map = 
    duan::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person> >(), "system map");

void test_class(){
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "before: " << g_person->getValue().toString() << " - " << g_person->toString();

#define XX_PM(g_var, prefix) \
    { \
        auto m = g_person_map->getValue(); \
        for(auto& i : m){ \
            DUAN_LOG_INFO(DUAN_LOG_ROOT()) << prefix << " : " <<  i.first << " - " << i.second.toString(); \
        } \
        DUAN_LOG_INFO(DUAN_LOG_ROOT()) << prefix << " : size=" << m.size(); \
    }

    g_person->addListener(10, [](const Person& old_value, const Person& new_value){
        DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "old_value=" << old_value.toString()
                << "new_value=" << new_value.toString();
    });

    XX_PM(g_person_map, "class.before");
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "before: " << g_person_vec_map->toString();

    YAML::Node root  = YAML::LoadFile("../bin/conf/test.yml");
    duan::Config::LoadFromYaml(root);
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "after: " << g_person->getValue().toString() << " - " << g_person->toString();
    XX_PM(g_person_map, "class.after");
    DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "after: " << g_person_vec_map->toString();
}

int main(int argc, char** argv){
    // DUAN_LOG_INFO(DUAN_LOG_ROOT()) << g_int_value_config->getValue();
    // DUAN_LOG_INFO(DUAN_LOG_ROOT()) << g_int_value_config->toString();

    // test_yaml();
    // test_config();
    test_class();
    return 0;
}
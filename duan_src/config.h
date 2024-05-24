/*
实现配置系统
2024/5/17 15:10
by 六七
*/
#ifndef __DUAN_CONFIG_H__
#define __DUAN_CONFIG_H__

#include <memory>                   // 智能指针
#include <sstream>                  // 用于后续序列化
#include <string>
#include <boost/lexical_cast.hpp>   // 类型转换
#include "log.h"
#include <yaml-cpp/yaml.h>

// 配置系统支持更多类型的容器
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>

namespace duan{

// 配置变量的基类
// 把一些公有属性放在这里
class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name)
        , m_description(description){
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    virtual ~ConfigVarBase() {}

    const std::string& getName() const {return m_name;}
    const std::string& getDescription() const { return m_description;}

    virtual std::string toString() = 0;                               // 转成字符串
    virtual bool fromString(const std::string& val) = 0;              // 从字符串初始化值
    virtual std::string getTypeName() const = 0;
private:
    std::string m_name;                     // 配置参数的名称
    std::string m_description;              // 配置参数的描述
};

// F from_type    T to_type
template<class F, class T>
class LexicalCast{
public:
    T operator()(const F& v){
        return boost::lexical_cast<T>(v);
    }
};

// 偏特化  string -> vector
template<class T>
class LexicalCast<std::string, std::vector<T> >{
public:
    std::vector<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        // typename 关键字用于引入一个模板参数，这个关键字用于指出模板声明（或定义）中的非独立名称（dependent names）是类型名，而非变量名
        typename std::vector<T> vec;                             // 返回的是一个数组
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return std::move(vec);        // C++11新特性
    }
};

// 偏特化 vector -> string
template<class T>
class LexicalCast<std::vector<T>, std::string>{
public:
    std::string operator()(const std::vector<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// list
template<class T>
class LexicalCast<std::string, std::list<T> >{
public:
    std::list<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;                             
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return std::move(vec);      
    }
};

template<class T>
class LexicalCast<std::list<T>, std::string>{
public:
    std::string operator()(const std::list<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// set
template<class T>
class LexicalCast<std::string, std::set<T> >{
public:
    std::set<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;                             
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return std::move(vec);      
    }
};

template<class T>
class LexicalCast<std::set<T>, std::string>{
public:
    std::string operator()(const std::set<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// unordered_set
template<class T>
class LexicalCast<std::string, std::unordered_set<T> >{
public:
    std::unordered_set<T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> vec;                             
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return std::move(vec);      
    }
};

template<class T>
class LexicalCast<std::unordered_set<T>, std::string>{
public:
    std::string operator()(const std::unordered_set<T>& v){
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// map
template<class T>
class LexicalCast<std::string, std::map<std::string, T> >{
public:
    std::map<std::string, T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;                             
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), 
                        LexicalCast<std::string, T>()(ss.str())));
        }
        return std::move(vec);      
    }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string>{
public:
    std::string operator()(const std::map<std::string, T>& v){
        YAML::Node node;
        for(auto& i : v){
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second)); 
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// unordered_map
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> >{
public:
    std::unordered_map<std::string, T> operator()(const std::string& v){
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;                             
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), 
                        LexicalCast<std::string, T>()(ss.str())));
        }
        return std::move(vec);      
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string>{
public:
    std::string operator()(const std::unordered_map<std::string, T>& v){
        YAML::Node node;
        for(auto& i : v){
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second)); 
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
}; 

// FromStr T operator()(const std::string&)
// ToStr std::string operator()(const T&)
// 模板特化
template<class T, class FromStr = LexicalCast<std::string, T>
                , class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name
            ,const T& default_value
            ,const std::string& description = "")
        : ConfigVarBase(name, description)
        , m_val(default_value){

    }

    // 注意类型转换是有异常安全问题的
    std::string toString() override {
        try{
            // return boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        }
        catch(std::exception& e){
            DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << "convert: " << typeid(m_val).name() << "to string";
        }
        return "";
    }                               
    // 转成字符串
    bool fromString(const std::string& val) override {
        try{
            // m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
        }
        catch(std::exception& e){
           DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << "convert: string to " << typeid(m_val).name()
                << " - " << val; 
        }
        return false;
    }

    const T getValue() const{ return m_val;}
    void setValue(const T& v) { m_val = v;}
    std::string getTypeName() const override { return typeid(T).name();}
private:
    T m_val;
};

class Config{
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    template<class T>
    // typename告诉编译器 T是类型 而不是变量名
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = ""){
        auto it = s_datas.find(name);
        if(it != s_datas.end()){
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp){
                DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "Lookup name=" << name << "exists";
                return tmp;
            }
            else{
                DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << "Lookup name=" << name << "exists but type not " 
                    << typeid(T).name() << " real_type=" << it->second->getTypeName()
                    << " " << it->second->toString();
                return nullptr;
            }
        }

        if(name.find_first_not_of("abcdedghigklmnopqrstuvwxyz._012345678")
                != std::string::npos){
            DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << "Lookup name invalid " << name;    // 用日志记录错误
            throw std::invalid_argument(name);                                    // 抛出异常  名字有问题
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = v;
        return v;
    }

    // 查找
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        auto it = s_datas.find(name);
        if(it == s_datas.end()){    // 未找到
            return nullptr;
        }
        // 找到了  强制父转子类，只能用在shared_ptr下  dynamic_pointer_cast是转为智能指针
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::ptr LookupBase(const std::string& name);
private:
    static ConfigVarMap s_datas;
};

} // end of namespace

#endif
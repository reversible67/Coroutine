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
private:
    std::string m_name;                     // 配置参数的名称
    std::string m_description;              // 配置参数的描述
};

template<class T>
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
            return boost::lexical_cast<std::string>(m_val);
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
            m_val = boost::lexical_cast<T>(val);
        }
        catch(std::exception& e){
           DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << "convert: string to " << typeid(m_val).name(); 
        }
        return false;
    }

    const T getValue() const{ return m_val;}
    void setValue(const T& v) { m_val = v;}
private:
    T m_val;
};

class Config{
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    template<class T>
    // typename告诉编译器 T是类型 而不是变量名
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = ""){
        auto tmp = Lookup<T>(name);
        if(tmp){
            DUAN_LOG_INFO(DUAN_LOG_ROOT()) << "Lookup name=" << name << "exists";
            return tmp;
        }

        if(name.find_first_not_of("abcdedghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ._012345678")
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
private:
    static ConfigVarMap s_datas;
};

} // end of namespace

#endif
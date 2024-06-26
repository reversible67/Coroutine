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
#include <functional>              // 函数对象
#include "thread.h"

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
    typedef RWMutex RWMutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

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
            RWMutexType::ReadLock lock(m_mutex);
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

    const T getValue(){ 
        RWMutexType::ReadLock lock(m_mutex);
        return m_val;
    }

    // 要通知变化
    void setValue(const T& v) {
        // 加{}的作用  就是形成一个局部作用域  出了这个作用域 锁就自己失效了！
        // 性能方面考虑： 释放锁其实也会消耗时间  不如只用一个写锁
        {
            // 这一段用读锁
            RWMutexType::ReadLock lock(m_mutex);
            if(v == m_val){
                return;
            }
            for(auto& i : m_cbs){
                i.second(m_val, v);
            }
        }
        // 这一段用写锁
        RWMutexType::WriteLock lock(m_mutex);
        m_val = v;
    }
    std::string getTypeName() const override { return typeid(T).name();}

    // 因为要变更 所以要增加监听
    uint64_t addListener(on_change_cb cb){
        // 可以添加的时候 每一个都有一个返回值 删除的时候 删掉这个返回值
        static uint64_t s_fun_id = 0;
        RWMutexType::WriteLock lock(m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }
    // 删除
    void delListener(uint64_t key){
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.erase(key);
    }
    // 返回
    on_change_cb getListener(uint64_t key){
        RWMutexType::ReadLock lock(m_mutex);
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }
    // 清空
    void clearListener(){
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.clear();
    }
private:
    RWMutexType m_mutex;      // 写操作比较多 所以这里使用读写锁
    T m_val;
    // 变更回调函数组， 为什么用map？ 因为变更后可以通过key进行删除   unit64_t key, 要求唯一， 一般可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config{
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex RWMutexType;    // 读多写少 就用读写锁

    template<class T>
    // typename告诉编译器 T是类型 而不是变量名
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = ""){
        RWMutexType::WriteLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if(it != GetDatas().end()){
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

        if(name.find_first_not_of("abcdefghigklmnopqrstuvwxyz._012345678")
                != std::string::npos){
            DUAN_LOG_ERROR(DUAN_LOG_ROOT()) << "Lookup name invalid " << name;    // 用日志记录错误
            throw std::invalid_argument(name);                                    // 抛出异常  名字有问题
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas() [name] = v;
        return v;
    }

    // 查找
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        RWMutexType::ReadLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()){    // 未找到
            return nullptr;
        }
        // 找到了  强制父转子类，只能用在shared_ptr下  dynamic_pointer_cast是转为智能指针
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::ptr LookupBase(const std::string& name);

    static void Visit(std::function<void(ConfigVarBase::ptr)> cb); 
private:
    // static ConfigVarMap s_datas;     // 有初始化顺序的问题  有可能还没有初始化 其他地方就被使用了 会core错误
    static ConfigVarMap& GetDatas(){
        static ConfigVarMap s_datas;
        return s_datas;
    }

    // 读写锁也是需要静态方法返回的 因为我们这个类基本上都是创建一个全局变量
    // 这个全局变量是一个配置 全局变量的初始化没有严格的顺序 如果不是这种方式的话 有可能会出现问题（内存错误）
    static RWMutexType& GetMutex(){
        static RWMutexType s_mutex;
        return s_mutex;
    }
};

} // end of namespace

#endif
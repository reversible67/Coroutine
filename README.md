# duan__README



## 开发环境

Centos 7

gcc 9.3

cmake 

## 项目路径

bin  -- 二进制

build -- 中间文件路径

cmake -- cmake函数文件夹

CMakeLists.txt -- cmake的定义文件

lib -- 库的输出路径

Makefile

duan_src -- 源代码路径

tests -- 测试代码

## 日志系统

> 1. 参考Log4
> 2. Logger定义日志类别  Appender定义日志输出的地方  Formatter 日志格式



## 配置系统

>Config --> Yaml
>
>## Yaml安装
>
>Yaml-cpp: [jbeder/yaml-cpp: A YAML parser and emitter in C++ (github.com)](https://github.com/jbeder/yaml-cpp)
>
>mkdir build && cd build && cmake .. && make install
>
>## Yaml使用
>
>```C++
>// 加载文件
>YAML::Node node = YAML::LoadFile(filename)
>// 通过node进行遍历
>node.IsMap()时
>for(auto it = node.begin(); it != node.end(); ++it) {
>it->first, it ->second
>}
>node.IsSequence()时
>for(size_t i = 0; i < node.size(); ++i){
>
>}
>node.IsScalar()时
>```

> 配置系统的原则，约定优于配置
>
> ```C++
> template<T, FromStr, ToStr>
> class ConfigVar;
> 
> F from_typr    T to_type
> template<F, T>
> LexicalCast;
> 
> // 容器偏特化  目前支持vector list map set unordered_map unordered_set
> // map 和 unodered_map 支持 key = std::string
> // Config::Lookup(key), key相同,
> // 类型不同的, 不会有报错， 这个需要处理一下
> string -> vector
> vector -> string
> ```

自定义类型，需要实现duan::LexicalCast,偏特化

实现后，就可以支持Config解析自定义类型，自定义类型可以和常规stl容器一起使用。(可以嵌套的很深  --复杂类型)

配置的事件机制

当一个配置项发生修改的时候，可以反向通知对应的代码，回调

## 日志系统整合配置系统

```C++
logs:
	- name: root
    - level: (debug, info, warn, error, fatal)
	  formatter: '%d%T%p%T%t%m%n'
      appender:
		- type: StdoytLogAppender, FileLogAppender
          level:(debug, ...)
		  file: /logs/xxx.log
```

```C++
	duan::Logger g_logger = duan::LoggerMgr::GetInstance()->getLogger(name);
	DUAN_LOG_INFO(g_logger) << "xxx log";
```



```C++
static Logger::ptr g_log = DUAN::LOG_NAME("system");    
// m_root, m_system-> m_root
// 当logger的appenders为空，使用root的写logger
```

```C++
// 定义LogDefine LogAppenderDefine， 偏特化 LexicalCast
// 实现日志配置解析
```

```C++
// 遗留问题
// 1.appender定义的formatter读取yaml的时候，没有被初始化
// 2.去掉额外的调试日志
```

总结：

ProcessOn

日志模块类图

![duan - 服务器框架类图 - 日志模块](C:\Users\wd\Desktop\Cpp项目\duan - 服务器框架类图 - 日志模块.png)

配置模块类图

![duan - 服务器框架类图 - 配置模块](C:\Users\wd\Desktop\Cpp项目\duan - 服务器框架类图 - 配置模块.png)

## 线程库

Thread，Mutex，Pthread

pthread pthread_create

互斥量 mutex 信号量 semaphore

和log来整合

Logger，LogAppender，LogFormatter

SpinLock替换Mutex

写文件，周期性reopen，防止文件被删除时没有感知文件的变化

##  协程库封装

协程是更轻量级的线程，是用户态的，是我们可以操控的

定义协程接口、ucontext_t、macro(assert + 栈信息)

```C++
Fiber::GetThis()  // 如果当前没有协程  调用之后 会创建一个主协程

Thread->main_fiber  <-----> sub_fiber
```

```C++
        1 - N       1 - M
scheduler --> thread --> fiber
1.线程池，分配一组线程
2.协程调度器，将协程，指定到相应的线程上去执行
// 最终就是N ： M

m_threads
// m_fibers里面可以放 function 也可以放 fiber
<function<void()>, fiber, thread_id> m_fibers
schedule(func/fiber)     // 最终可以调度func 也可以调度fiber
          
start()
stop()
run()
       
1.设置当前线程的scheduler
2.设置当前线程的run，fiber
3.协程调度循环while(true)
    1.协程消息队列里面是否有任务
    2.无任务执行，执行idle
```

```C++
// 做了这么久铺垫~ 正片开始!
IOManager(epoll)   ---> Scheduler
      |
      |
      |
   idle(epoll_wait)
    
    信号量
PutMessage(msg,)  ++信号量
message_queue
    |
    |------- Thread
    |------- Thread
    		wait() --信号量 RecvMessage(msg,)  没有信号量的时候  会阻塞
    
    
// 异步IO，等待数据返回   epoll_wait
    
创建：epoll_create, 修改：epoll_ctl, 等待：epoll_wait
```

```C++
Timer -> addTimer --> cancel()
获取当前的定时器触发离现在的时间差
返回当前需要触发的定时器
```

## 2024/6/24 接下来写hook功能  hook还未实现
hook已实现


## socket函数库



## http协议开发



## 分布式协议库
分布式 写了一个KVstorageBaseRaft
https://github.com/reversible67/KVstorageBaseRaft-cpp


## 推荐系统



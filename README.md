# 高性能服务器框架

## 开发环境

Centos8

gcc 8.4.1

## 1 项目路径

bin --二进制

build -- 中间文件路径

cmake -- cmake函数文件夹

CMakeLists.txt -- cmake的定义文件

lib -- 库的输出路径

Makefile

sylar -- 源代码路径

tests -- 测试代码

## 2 日志系统

**前言**：为了保证所有的变量工作的空间，日志的所有类都定义在`sylar`命名空间内部。

### 2.1 日志格式

```tex
* %m -- 消息体
* %p -- level   
* %r -- 启动后的时间
* %c -- 日志名称
* %t -- 线程id   
* %n -- 回车换行
* %d -- 时间戳   
* %f -- 文件名   
* %l -- 行号
```

### 2.2 日志类

#### 2.2.1 一级类（或者是父类）

**Logger日志事件类**

```tex
1 该类为日志的输出类，供给上层去调用，提供给上层调用的接口包括几种要输出的日志级别：	debug,info,warn,error,fatal。
2 由于要控制日志的输出,因此需要在日志里需要能控制日志输出地（增删）
3 在日志事件里需要能够设置日志级别和获得日志级别
4 提供给上层调用的debug,info,warn,error,fatal接口，其内部都是调用log函数，来实际输出日志信息。

**方法分析**
1 构造函数
	该类的构造函数中，首先设置了日志名为root，然后设置日志级别为DEBUG，设置默认的日志输出格式为：%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n。
2 void Logger::clearAppenders()方法
	该方法即清空所有的输出器。
3 void Logger::addAppender(LogAppender::ptr appender)方法
	该方法将给定的输出器添加到输出器链表中，如果appender中的输出格式为空则将当前logger的输出格式赋值给appender。
4 void Logger::delAppender(LogAppender::ptr appender)方法
	删除指定的appender输出器。
5 void Logger::setFormatter(LogFormatter::ptr val)方法
	使用LogFormatter::ptr类来设置输出格式，logger类的设置日志输出格式的方法，如果appender没有输出格式，则同样设置appender的输出格式，如果有则不设置。
6 void Logger::setFormatter(const std::string& val)方法
	使用字符串来设置formatter，实际上是调用了上面的setFormatter.
7 LogFormatter::ptr Logger::getFormatter()方法
	直接返回m_formatter.
8 std::string Logger::toYamlString()方法
	该方法转换成YamlString，具体的YAML中logger对象包含以下几部分内容：name,level,formatter,appender;另外，appender又包括file和type，且appender是一个数组。因此，在转换时，需要对日志级别，格式串，日志名，输出器等进行转换；由于日志级别本身是整形，故日志级别里需要有个将整形日志级别转换成字符串类型的函数；同理，格式化字符串有个获得格式化字符串类的函数。
9 void Logger::log(LogLevel::Level level, LogEvent::ptr event)方法
	该方法就是实际写日志的入口，可以供给自己的类方法debug,info,warn,error,fatal等方法调用。该方法内部调用了appender的log方法，即实际是appender的方法进行写的。
	10 void Logger::debug(LogEvent::ptr event)【注：info,warn,error,fatal同，区别就是在调用log时传入的日志级别不同，即传入对应的日志级别】。
```

<img src="C:\Users\King\AppData\Roaming\Typora\typora-user-images\1653925999916.png" alt="1653925999916" style="zoom:50%;" />

**LogEvent日志事件类**

```tex
**成员方法**
1 构造函数：LogEvent::LogEvent(std::shared_ptr<Logger> logger,
        LogLevel::Level level, const char* file,
        int32_t line, uint32_t elapse,
        uint32_t thread_id, uint32_t fiber_id,
        uint64_t time)
        该构造函数中初始化了如上这么多属性。
2 void LogEvent::format(const char* fmt, ...)
	该方法是LogEvent提供的格式化可变参数列表，该可变参数列表调用另一个实际的format来进行实际的输出。
3 void LogEvent::format(const char* fmt, va_list al)方法
	该方法是实际的日志事件格式输出。
4 std::stringstream& LogEventWrap::getSS()方法
	该方法就是返回一个stringstream输入流的引用对象，使得将输入流写入m_ss对象。
5 该日志事件类提供了如下的内联方法
	const char* getFile()，int32_t getElapse()，uint32_t getThreadId()，  
    uint32_t getFiberId()，    uint32_t getLine()，    uint64_t getTime()，   
    std::string getContent()，    std::stringstream& getSS()，   
    std::shared_ptr<Logger> getLogger()，    LogLevel::Level getLevel()，
    
    注：可变参数列表语法：
```

**LogEventWrap日志事件包裹类**

```tex
该类的析构函数中调用日志，进行写入日志。

**成员方法**
1 构造函数：LogEventWrap(LogEvent::ptr e)
	该构造函数里即通过传入一个LogEvent的指针，来初始化该类。
2 析构函数：LogEventWrap::~LogEventWrap()
	同过利用该类的析构函数来执行写入日志的操作。
3 std::stringstream& LogEventWrap::getSS()方法
	该方法即获得输入流，实际是调用LogEvent类的getSS来实现的。
```



**LogLevel日志级别类**

```tex
1 该类主要的功能就是定义日志级别

**方法分析**
1 const char* LogLevel::ToString(LogLevel::Level level)方法
	该方法就是将日志级别转换成字符串格式
2 LogLevel::Level LogLevel::FromString(const std::string& str)方法
	该方法将日志级别从string转换成LogLevel类型
```

**LogFormatter日志格式器类**

```c++
1 该类最主要的功能就是格式化数据格式，返回一个string类型供给输出。
2 在该函数内部定义了一个格式化项目的基类，供给具体的项目来实际调用
3 该类内部实现了format，然而这个format的实际工作是调用具体的子项的format，该类内部存放了子项的数组，在调用时，遍历数组来调用具体项的format
```

```tex
在该函数的init方法中，使用了tuple，该元组包含三个参数，第一个表示键key（第三个参数为1时，如果第三个参数为0则表示这个只是一个普通字符），第二个表示传递给通过第一个键值找到的类方法的实参（多态：父类指针指向子类对象），这里的值是对应一个类，使用了多态，第三个表示该元组的第一个元组是键还是普通字符。

**方法分析**
1 构造函数LogFormatter::LogFormatter(const std::string& pattern)
	该构造函数中需要传入一个期望输出的日志格式字符串，然后在其初始化列表中初始化该字符串。之后在该函数体内部，调用了该类的初始化函数（init()方法）。
2 init()方法【如格式字符串：%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n】
	该方法主要是用来解析传入的格式字符串，传入的格式化字符串格式可以为如下类型：[%x] %x %xxx{xxx} %xx %x。
	2.1 该init()方法中，首先定义了一个包含三个元素的元组列表（std::vector<std::tuple<std::string, std::string, int>> vec;），接着，便开始从头开始解析格式化字符串。
	2.2 如果当前字符不是'%'，说明当前字符是普通字符，则将普通字符加入定义的普通字符串变量中（nstr）,然后遍历下一个字符。
	2.3 如果当前字符是'%'，则需判断%字符的下一个元素，如果下一个元素还是%，（即%%），此种情况表示该格式字符串的当前字符是普通字符%，加入普通字符变量（nstr）中，然后继续遍历下一个字符。
	2.4 如果当前字符的下一个字符也不是'%'，并且当前字符是'%'，那么就开始分析'%'后面的格式字符，判断是以何种格式输出。
	2.5 定义了'%'字符后的字符的位置遍历n，格式字符串的状态变量，格式字符串的开始变量。每次只分析一组格式。其中，fmt_status是为了标记{}类型的格式串的。
	2.6 每次跳出循环的条件：①非普通字符 ②非{} ③fmt_status == 0 或者 fmt_status == 2；第③个条件设置的意义是必须取得完整的{}，即要么没有，有就必须得有一对，该{}内可能有其他特殊字符，故需要用fmt_status来进一步判断。
	2.7 如果fmt_status == 0说明此时可能是{或者没有{}，此时再进一步判断该字符是不是{，
		2.7.1 如果当前字符不是{，则会提取分析遍历下一个字符；
		2.7.2 如果是的话，则需将fmt_status状态+1，并且取出%和{字符之间的字符，保存在变量str中。然后设置{里的格式化字符的开始位置，即fmt_begin，然后继续下一个字符的提取。
	2.8 如果fmt_status == 1说明前面已经解析了{字符，那么此时就要解析{}里的字符，故需要保存{}里的字符，该字符通过fmt字符串进行保存。
		2.8.1 如果当前字符不是}，则说明{}里的字符并未提取完成，故接着遍历提取。
		2.8.2 如果当前字符是}，则说明{}里的字符已经提取完成，则取出{}里的字符，并把fmt_status的值+1，说明状态提取完成，然后跳出当前格式的提取。
	2.9 这一种格式提取完成之后，接下来就是判断当前提取的格式
		2.9.1 如果fmt_status == 0，则说明当前提取的格式化字符串中不包括{}，是一个合法的字符串；因此，需要将该合法的格式字符串加入格式化字符串的元组列表中；不过，在加入该格式化字符串之前，因为格式化字符串之前可能有普通字符串，故在开始处如果普通字符串不为空则需要先将普通字符串添加到元组列表，然后清空普通字符串，等待接收下一个普通字符串。接着，我们就需要将格式化成功的字符串加入元组列表。因为当前的字符串m_pattern[n] == '%'，并且for循环中有一个++i,故需要先后退一个，不然就会跳过一个字符，导致解析出错。
		2.9.2 如果fmt_status == 1，则说明解析字符串时有{无}，故此时解析出错，报错提示。
		2.9.3 如果fmt_status == 2，则说明此时解析的字符串有{}，故先将普通字符串加入，然后将格式字符串加入，并把{}里的字符串当做是传入函数的参数。由于此时m_pattern[n] == '}'，故i 不需要减了。
	2.10 格式化字符串成功后，下面就是准备如何处理格式化成功的字符串，此处处理格式化成功的字符串是通过map来进行映射的。即通过格式化字符映射到处理类，此外添加元组的列表时，如果元组的第三个数是0表示当前是普通字符串，直接输出即可，如果是1则说明是格式化字符串。
	2.11 格式化字符串类
	//格式字符串映射一个格式类映射关系
        static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
    #define XX(str, C) \
            {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

            XX(m, MessageFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElapseFormatItem),
            XX(c, NameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(n, NewLineFormatItem),
            XX(d, DateTimeFormatItem),
            XX(f, FilenameFormatItem),
            XX(l, LineFormatItem),
            XX(T, TabFormatItem),
            XX(F, FiberFormatItem),
    #undef XX
        };
	2.12 该映射关系是通过一个格式化字符来映射到一个lamada函数，该lamada函数的传的实参就是{}里的字符串。该lamada函数的返回值就是一个FormatItem类型的指针（父类指针），并且在lamada函数内部是new了一个C（C表示XX里自己传入的一个对象）。故通过s_format_items[key]可以得到一个父类对象（多态：父类指针指向子类对象）
	2.13 接着，通过遍历元组列表，将所有的父类对象添加到格式化项目中。其中，对于元组来说，std::get<2>(i)表示元组i的第2个元素（从0开始算）。
	2.14 由于元组中的格式字符串有的是普通字符串，有的是格式字符串，是否是普通字符串即通过元组的第三个元素的值来判断（0：普通字符串，1：格式字符串），即std::get<2>(i)来判断。
		2.14.1 如果是普通字符串，则需要处理普通字符串：单独的StringFormatItem类
		2.14.2 如果是格式字符串，则需要处理格式字符串
			2.14.2.1 在处理格式化字符串中，首先将提取到的格式化字符串与自己定义的格式字符串与类的映射关系表比较，如果映射关系表中没有提取的格式字符串，则输出该格式字符串为错误格式，报错提示。
			2.14.2.2 如果在映射关系表中找到了，则将该格式化字符串类添加到格式化字符串项目指针列表中，等待后续格式化字符串。
3 Logformatter内部提供了FormatItem虚接口
	注：每个继承自FormatItem都有一个format方法，即重写父类的该方法，重写的format方法类型如下
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;(关键字override表明重写该函数，在编译器进行辅助性检查)
	3.1 MessageFormatItem消息格式化类
		3.1.1 该类的构造函数中默认是空字符
		3.1.2 format方法
			该类的format通过调用os << event->getContent()来实现消息体的输出，getContent()的函数处理在LogEvent类中进行分析。
	3.2 LevelFormatItem日志级别格式化类
		3.2.1 该类的构造函数中默认是空字符
		3.2.2 format方法
			该类的format通过调用os << LogLevel::ToString(level)来实现日志级别的输出，ToString的函数处理在LogLevel类中进行分析。
	3.3 ElapseFormatItem日志启动的毫秒数类
		3.3.1 该类的构造函数中默认是空字符
		3.3.2 format方法
			该类的format通过调用os << event->getElapse()来实现启动时间的输出，getElapse 的函数处理在 LogEvent 类中进行分析。
	3.4 NameFormatItem 日志名类
		3.4.1 该类的构造函数中默认是空字符
		3.4.2 format方法
			该类的format通过调用os << logger->getName()来实现日志名的输出，getName 的函数处理在 Logger 类中进行分析。
	3.5 ThreadIdFormatItem 线程id类
		3.5.1 该类的构造函数中默认是空字符
		3.5.2 format方法
			该类的format通过调用 os << event->getThreadId()来实现线程id的输出，getThreadId 的函数处理在 LogEvent 类中进行分析。
		注：该类获得的线程id是通过系统调用获得的真实的线程id。
	3.6 FiberFormatItem 协程id类
		3.6.1 该类的构造函数中默认是空字符
		3.6.2 format方法
			该类的format通过调用 os << event->getFiberId()来实现协程id的输出，getFiberId 的函数处理在 LogEvent 类中进行分析。
	3.7 DateTimeFormatItem 时间戳类
		3.7.1 该类的构造函数中默认有特定的时间戳格式，如果不传参则使用默认的时间戳格式，否则使用传入的时间戳格式。
		3.7.2 format方法
			该类的format首先通过调用event->getTime()来获得时间戳，之后又调用了localtime_r,strftime等函数来获得当前的时间戳，将时间戳字符串写入字符流。
			注：其中strftime为将特定格式的时间类型转换为char*类型。localtime_r函数即将time_t类型的时间变量存储到struct tm类型的变量中，strftime要转换的就是struct tm类型的时间变量。
	3.8 FilenameFormatItem 文件类
		3.8.1 该类的构造函数中默认是空字符
		3.8.2 format方法
			该类的format通过调用 os << event->getFile()来实现文件名的输出，getFile 的函数处理在 LogEvent 类中进行分析。
	3.9 LineFormatItem 行号类
		3.9.1 该类的构造函数中默认是空字符
		3.9.2 format方法
			该类的format通过调用 os << event->getLine()来实现行号的输出，getLine 的函数处理在 LogEvent 类中进行分析。
	3.10 NewLineFormatItem 换行符类
		3.10.1 该类的构造函数中默认是空字符
		3.10.2 format方法
			该类的format通过调用 os << std::endl来实现行号的输出。
	3.11 StringFormatItem 普通字符串类
		3.11.1 该类的构造函数中默认是空字符
		3.11.2 format方法
			该类的format通过调用 os << m_string 来实现字符串的输出。
	3.12 TabFormatItem 制表符类
		3.12.1 该类的构造函数中默认是空字符
		3.12.2 format方法
			该类的format通过调用 os << "\t" 来实现制表符的输出。
			
	注：以上输出都是指输出到流中，然后从流中再统一格式化输出！！！
4 LogFormatter::LogFormatter(const std::string& pattern) 接口
	该接口遍历格式化列表项目接口，然后依次将格式化后的字符写入字符流。
	注：stringstream继承的关系图如下。
```

<img src="C:\Users\King\AppData\Roaming\Typora\typora-user-images\1653991372407.png" alt="1653991372407" style="zoom:80%;" />

**LogAppender日志输出地类**

```cpp
1 该类是一个虚基类，只是提供一个共同的接口，供给StdoutLogAppender和FileLogAppender去调用
2 由于该函数是一个虚基类，故该函数的析构函数应该是虚析构
3 上层的Logger调用该函数来实际输出日志
4 由于在输出日志的时候需要控制输出日志的格式，故该函数需要能够设置日志格式
5 并且每个日志都有日志级别，故需要一个日志级别变量
    
**成员方法**
1 LogFormatter::ptr LogAppender::getFormatter()
    该类直接返回格式化的字符串格式。
```

#### 2.2.2 次级类

##### 2.2.2.1 定义在LogFormatter内部的类：  LogFormatter::FormatItem

```c++
1 该类是一个虚基类，主要是为了提供给下面的类提供一个统一的虚接口
```

##### 2.2.2.3 子类

**LogAppender的子类：**

**StdoutLogAppender输出到控制台的Appender**

```c++
1 实际输出，定义输出到控制台
2 该类内部重写log接口来实现输出位置控制
    
**成员方法**
只有重写父类的log和toYamlString方法
1 void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override方法
    该方法实际上是直接调用Logformatter的format方法，然后Logformatter中再遍历所有的格式项输出。
2 std::string StdoutLogAppender::toYamlString()方法
    该方法就是将自己的type，level，formatter转换成YAML的string格式（如果不为空）。
```

**FileLogAppender输出到文件的Appender**

```c++
1 实际输出，定义输出到文件
2 该类内部重写log接口，来时输出位置控制
3 由于要输出到文件，故在构造函数中初始化要输出到文件的文件名
4 如果输出的文件已经是打开的，则需要重新关闭再打开。
    
    
**成员方法**
1 构造函数
    该构造函数中对m_filename进行了初始化，并且在构造函数中调用了打开文件的reopen方法。
2 bool FileLogAppender::reopen()方法
    该方法就是以追加写的方式打开文件，在打开文件之前先判断文件是否已经打开。如果已经打开了，则先关闭该文件，然后再打开。
3 void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)重写方法
    该方法同StdoutAppender的log方法，都是调用Logformatter的format方法。
4 std::string FileLogAppender::toYamlString()方法
    该方法同样是将类属性转换成YAML字符串格式，与StdoutAppender不同的是该方法多了一个file文件名。
```

### 2.3 日志工作流程

```tex
**注：日志的类只能有一个，即是一个单例模式**

调用类别：假如是调用debug级别的日志
1 首先new一个Logger对象XXX（单例模式）
2 调用XXX.addAppender()【Logger类】（添加debug级别的输出器，注：这里用到了多态，即父类指针指向子类对象，故这里传入的应该是一个子类对象，例如这里传入StdoutLogAppender,将日志输出到控制台上）
3 调用XXX.debug()【Logger类】(调用的时候需要传一个LogEvent类的指针)
4 XXX.debug()【Logger类】内部调用了XXX.log()【Logger类】（注：这里依据第一个日志级别来判断后续调用的是哪个级别的操作日志）
5 XXX.log（）【Logger类内部根据日志级别调用了具体的输出器（StdoutLogAppender）】
6 遍历所有的输出器找到对应的输出器后通过对应的输出器来调用该输出器的log方法【StdoutLogAppender】，写操作日志
7 StdoutLogAppender内部调用了LogFormatter中的format方法【LogFormatter】后，将数据格式化成字符串后输出
8 LogFormatter内部的format【LogFormatter类】在进行格式化字符串时，需要对每一种具体的格式化数据单独格式化后，格式化成一种字符流后以字符串的形式返回输出，该格式化内部具体又调用了具体项目的format方法【比如ElapseFormatItem等】（具体需要格式化的类包括：LevelFormatItem，ElapseFormatItem，NameFormatItem，ThreadIdFormatItem，FiberIdFormatItem，DateTimeFormatItem，FilenameFormatItem，LineFormatItem，NewLineFormatItem，StringFormatItem等，具体需要格式化的类有哪些，是依据自己想要的输出日志的格式定义的，该日志格式为log4j）
9 遍历所有的日志格式类，一个个的格式化后，进行输出，该输出的内部调用了最终LogEvent中的函数，ElapseFormatItem，则在ElapseFormatItem内部的函数调用format方法，该方法内部调用了通过LogEvent类穿过来event指针，来调用具体的事件，即event->getElapse().

注：1 在调用具体项目的format方法之前，必须得对LogFormatter进行初始化，即将需要格式的项目添加进格式化数组，故需要在LogFormatter的构造函数内部调用init方法【LogFormatter类】
    2 在LogFormatter中的format方法是将需要格式化的项先统一写入stringstream内部，之后返回这个字符流转成的字符串输出。
    3 因为日志级别是一个整数，不过输出时日志级别应该是一个字符串，故应该有个将日志级别转换成字符串的函数

```

<img src="C:\Users\King\AppData\Roaming\Typora\typora-user-images\1652164197351.png" alt="1652164197351" style="zoom:50%;" />

1）

​	Logger(定义日志级别)

​		|

​		|--------------Formatter(日志格式)

​		|

​	Appender(日志输出地方)

2）配置系统 ：yaml库

```tex
定义节点：
YAML::Node root = LoadFile(filepath);
节点类型：YAML::NodeType   enum {NULL,Scalar,Sequence,Map}
判断函数：
IsNull(),IsScalar(),IsSequence(),IsMap()
```



**工作流程分析2**

`宏定义`

```cpp
#define SYLAR_LOG_NAME(name) sylar::loggerMgr::GetInstance()->getLogger(name)
```

```tex
该宏定义是通过宏SYLAR_LOG_NAME来获取一个Logger对象。其中loggerMgr是一个单例模式对象的别名，GetInstance是该单例模式的一个方法，用来获得一个LogManager对象，该对象统一管理日志系统。方法定义如下：
```

```cpp
class LogManager {
public:
    LogManager();
    Logger::ptr getLogger(std::string name);
    Logger::ptr getRoot() const { return m_root; }
    void init();
    std::string toYamlString();
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};
typedef Singleton<LogManager> loggerMgr;
}
```

```tex
在调用SYLAR_LOG_NAME时需要传入一个name，该name的含义就是获得名字为name的操作日志对象（即调用getLogger(std::string name)获得）。
该日志管理类中通过map保存了所有的日志对象（key为日志对象名，值为对应的日志对象指针）。在获取日志对象时，首先从m_loggers的map中查找指定名字的日志对象，如果找到了则直接返回，如果没找到则新创建一个日志对象，该新创建的日志对象的各种属性被设置成与m_root的日志相同。
```

## 3 协程库封装

### 3.1 线程

`类Thread`

```cpp
属性
    private:
        pid_t m_id = -1;//暂时不懂啥意思
        pthread_t m_threadId = 0;//线程id
        std::string m_name;//线程名
        std::function<void()> m_cb;
方法：
    public:
        //线程智能指针
        typedef std::shared_ptr<Thread> ptr;
        
        Thread(std::function<void()> cb, const std::string& name);
        ~Thread();
        pid_t getId() const { return m_id; }
        const std::string& getName() const { return m_name; }

        void join();
        //获取当前线程指针
        static Thread* GetThis();
        //获取当前线程名称
        static const std::string& GetName();
        //设置当前线程名称
        static void SetName(const std::string& name);
    private:
        static void* run(void*);//线程运行函数
```

`分析`

```cpp
该类中设置了私有变量：m_id,m_threadId,m_name, m_cb;其中m_id暂时不知道作用是什么，m_threadId用来记录创建线程时id传参，m_name用来记录线程名称，m_cb是用来记录线程的回调函数，即创建线程时的一些初始化操作在类中的线程成员函数做，其余的工作处理在其他地方做（回调函数）。其中的getId函数暂时不懂啥意思；getName函数返回的是当前线程传入的线程名；getThis为当前运行的线程的线程类；GetName为当前运行的线程名称；SetName为设置当前运行的线程的名称。
```



## 4 socket函数库



## 5 http协议




# 6 全局宏定义

**宏定义**

```cpp
1    #define SYLAR_LOG_LEVEL(logger, level) \
            if(logger->getLevel() <= level) \
            sylar::LogEventWrap (sylar::LogEvent::ptr (new sylar::LogEvent(\
            logger, level, __FILE__, __LINE__, 0,\
            sylar::getThreadId(), sylar::getFiberId(), time(0)))).getSS()


2	#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
3	#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
4	#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
5	#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
6	#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

7    #define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
        if(logger->getLevel() <= level) \
            sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
            __FILE__, __LINE__, 0, sylar::getThreadId(),\
            sylar::getFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

8    #define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
    logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
9    #define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
    logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
10    #define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
    logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
11    #define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
    logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
12    #define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(\
    logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)


13	#define SYLAR_LOG_ROOT() sylar::loggerMgr::GetInstance()->getRoot()
14	#define SYLAR_LOG_NAME(name) sylar::loggerMgr::GetInstance()->getLogger(name)
```

**解析**

```tex
* 宏定义1：
	对于该宏定义，其目的是通过该宏
```

# 7 协程

**Fiber类**

```tex
1 构造函数
	Fiber::Fiber()；
	解析：该构造函数的目的是为了创建一个主协程，负责统一的调度；构造函数内部首先更新该主协程的状态为EXEC；接着，设置当前运行的协程为该协程；紧接着，保存当前主协程的运行上下文，然后将协程数量加1，并输出日志。
	Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)；
	解析：构造函数的目的就是为了真实的创建一个运行协程（非主协程），第一个参数cb是指协程运行时处理具体任务的回调函数（不过与具体的协程绑定的函数时MainFunc函数，在MainFunc函数中调用了cb回调函数）；第二个参数是指要构造的协程的栈的大小；第三个指出当前协程是否是调度协程；所有的参数在初始化列表中初始化；在进入该构造函数后，首先对协程数加1，紧接着初始化协程的栈大小，然后给协程分配协程栈；保存当前协程的上下文；接着，将该协程与一个特定的回调函数进行绑定，即当切换协程时就运行该回调函数；最后输出日志。
2 析构函数
	Fiber::~Fiber()
	解析：当协程运行结束时，就会调用该析构函数将协程空间释放；进入析构函数后，首先将协程数减1；接着，判断执行析构函数的是主协程还是普通协程；主协程未主动分配栈空间，故可以通过栈空间是否为空来判断；如果是普通协程析构，则需首先对普通协程的执行状态进行断言：TERM,EXCEPT,INIT，然后再回收内存；如果是主协程，需断言该主协程的回调函数为空，并且是EXEC状态；紧接着，如果当前的协程就是主协程，将指向运行协程的指针置空；最后，输出日志。
3 void Fiber::reset(std::function<void()> cb) 函数
	解析：该函数重新将一个协程切换到另一个回调函数；
4 void Fiber::call() 函数
	解析：call函数的作用是切换运行协程，即切换当前的协程为运行协程；通过具体的类调用，一个特定的类就是一个具体的协程；
5 void Fiber::swapIn()
	解析：首先设置当前运行的协程；并且断言切入的协程不是EXEC状态；
6 void Fiber::swapOut()
	解析：将协程切换到后台；即切换到调度协程。
7 void Fiber::SetThis(Fiber* f)
	解析：设置线程全局变量t_fiber指向传的形参
8 Fiber::ptr Fiber::GetThis()
	解析：返回当前线程的工作协程；即t_fiber的指向，如果t_fiber为空，则创建一个主协程然后让t_fiber指向该主协程；否则直接返回。
9 void Fiber::YieldToReady()
	解析：将当前协程切换到后台，并把当前状态设置为Ready状态。
10 void Fiber::YieldToHold()
	解析：将当前协程切换到后台，并把当前状态设置为Hold状态。
11 uint64_t Fiber::TotalFibers()
	解析：返回总协程数
12 void Fiber::MainFunc()
	解析：与协程绑定的回调函数
```





# 8 协程调度器

##  8.1 协程调度模块

**Scheduler类**

```tex
1 构造函数
	Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
	解析：该构造函数首先在其初始化列表中对m_name（调度器名称）进行了初始化；在进入构造函数时，首先断言线程数量:threads > 0（默认是1）；其次，如果当前线程是调度线程，设置主协程。然后断言调度线程为空，赋值调度线程为当前线程。
```



# 9 执行顺序

## 9.1 main函数开始执行之前：执行全局变量的初始化操作

**全局变量的初始化执行顺序如下（依据头文件导入的顺序）：**

**log.cc头文件**

```cpp
**log.cc头文件**
	* 1 sylar::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
        	sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");
    * 2 static LogIniter __log_init;
```

```tex
分析上述执行的语句：
	对于上面log.cc的语句1，定义了一个日志配置变量的指针，该指针调用了Lookup函数，对日志进行了注册，即注册的日志名为logs，并且传入的第二个参数是一个LogDefine集合；下面深入看一下Lookup函数具体做了什么工作：
	**Lookup函数**
	1 该函数首先在开始处调用了该类私有的GetDatas()静态函数，即所有对象共享的一个函数，并且范围值为引用，目的是获得一个所有对象共享的 ConfigVarMap 类型的s_datas对象，ConfigVarMap类型为std::map<std::string, ConfigVarBase::ptr>的别名，即可以通过一个string类型来找到对应的配置变量指针，这里运用了多态，因为ConfigVarBase是一个纯虚接口类型。
	2 调用GetDatas()后取得了s_datas对象，然后从对象中查找是否已经存在名为name的配置变量（name是Lookup传过来的第一个形参）。
	3 如果查找到了名字为name的键，说明之前已经使用该名字注册过，那么就使用动态类型强制转换（dynamic_pointer_cast），来将该类型强转为ConfigVar<T>类型；（注：这里之所以要进行强转的原因是在定义时，键定义的是父类指针，实际对象是子类对象，即应用了多态，然而返回值的子类对象，故需要进行强转）。
	4 在进行强转时，如果强转成功，则会调用：SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << "   exists";进行输出并返回，否则调用：SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << "exists but type error:" << typeid(T).name()
                                                     << "   real-type:" << s_datas[name]->getTypeName();来说明强转失败的原因。（注：只有当重复注册时，才会输出，注册已存在，如果重复注册的类型两次不一致，则会报错提示，然后以此次的值进行更新。）
     5 如果没有找到名字为name的键，说明name未被注册过，那么在使用name注册前，需要先对name的名称进行合法性校验，如果校验成功则继续接下来的工作，否则报错。
     6 名字校验成功后，就会创建一个ConfigVar<T>(name, default_value, description)类型的对象，此时会调用ConfigVar的构造函数，来初始化该对象；由于ConfigVar对象继承自ConfigVarBase,故实际初始化类时先初始化基类，然后再初始化子类；在初始化基类的时候，对名字进行了调整，即将所有的大写字符都转为小写字符。
     7 之后，会再次通过调用GetDatas()函数来将已经注册的键和值关联起来，然后返回值。
     
     
	对于上面log.cc的语句2，即在注册好以后，通过语句static LogIniter __log_init 创建了__log_init对象，利用了LogIniter的构造函数，在该构造函数中执行了部分操作，下面我们看一下LogIniter类的构造函数：
	1 在该构造函数内部，首先使用了语句1创建的ConfigVar<std::set<LogDefine> >::ptr g_log_defines日志配置变量。
	2 通过g_log_defines指针调用了ConfigVar类中的添加监听者的方法（实际上该类的作用就是设置监听者：在main函数执行之前）；该方法是通过一个键来绑定一个函数（注：这个键不一定有用，后面好像是删除了），对应的绑定的函数此处使用的是lamada表达式。
	3 lamada表达式的作用：
		3.1 首先，该lamada表达式需要传入两个参数，第一个是旧值，第二个是新值；
		3.2 其次，只要调用该lamada表达式，就是首先调用：SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed"; 输出日志配置更新了，表示该lamada表达式的回调函数被触发了。
		3.3 接着，就是判断是删除logger，还是更新logger，还是添加了logger;对于删除logger，说明在新的new_value中不包含该删除的logger，但是在旧的old_value中包含；对于更新了logger，说明new_value和old_value中都包含，但是两者的数据不一样；对于新增logger，说明在新的new_value中包含logger，但是在旧的old_value中不包含。接下来就是针对这几种情况进行不同的处理。
		3.4 如果是新增logger或者是更新logger，都会首先取得对应的该logger对象，取得该logger对象是依据logger的名字获得的，其中通过宏定义（SYLAR_LOG_NAME(i.name)）来获得对象，具体宏定义是如何做的，在后续进行介绍。
		3.5 如果该logger没有发生变化，则不需要更改（continue）。
		3.6 接着，对于这个发生变化的日志，首先设置其日志级别为新值的日志级别，然后如果新值的日志格式不为空的话，同样设置日志格式。之后，重新设置该日志的输出器（Appenders）为新值的输出器，设置前先将旧值的Appenders清空。
			3.6.1 设置输出器时，如果输出器的类型字段type == 1，说明是输出到文件，如果是type == 2，则说明是输出到控制台（注：这里目前疑问，何处设置的type，具体的old_value和new_value在哪传入的需要关注下）
			3.6.2 接着，设置输出器的日志级别，日志格式
        3.7 最后，调用该日志的addAppender来添加该日志的输出器。
        3.8 如果是删除logger，则不实际删除，只是调用SYLAR_LOG_NAME(i.name);并将日志基表设置成最低，最后清空下该日志的输出器。
    4 lamada表达式的函数此时不调用，此处只是添加了一个监听者，具体调用是在有日志发生改变时才调用。
    
    
    
    **解析SYLAR_LOG_NAME(i.name)宏定义的作用**
    宏定义：#define SYLAR_LOG_NAME(name) sylar::loggerMgr::GetInstance()->getLogger(name)
    通过上面可以看出，该宏定义相当于是调用了函数，首先我们需要知道loggerMgr是typedef Singleton<LogManager> loggerMgr的别名，GetInstance（）是Singleton<LogManager>的一个静态方法，该方法的返回值就是一个LogManager实例对象；在创建LogManager实例对象时，调用了LogManager的构造函数，在该构造函数内部，为m_root指向了一个Logger对象，并且为该对象添加了输出到控制台的输出器。
    接着，该LogManager实例对象调用了他的类方法getLogger(name)；对于该类方法，首先他会查找是否存在名为name的日志（map存储的，name为键），如果找到了直接返回该日志对象。否则，就新创建一个名为name的日志对象，并且该对象的m_root设置成管理类的m_root，然后添加到map。最后返回。
```


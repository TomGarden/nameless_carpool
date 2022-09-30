#pragma once

/* 用于记录一些常量的值 */

#include <string>
#include <chrono>
#include <random>

namespace nameless_carpool {


  namespace Common {
    using string = std::string;

    /*************___ nameless_carpool::Common::Config  全局配置___**************
     ***************************************************************************/
    namespace Config {
      /* 验证码失效时间 */
      const int VERTIFY_CODE_LIFE_TIME = 5 * 60 ;/* 五分钟 */
    }

    /*************___ nameless_carpool::Common::Regex  正则___***************** 
     ***************************************************************************/
    struct RegexVals {
      /* 其他国家手机号字符串匹配规则 : https://cloud.tencent.com/developer/article/1140913 */
      /* 中国 手机号正则字符串  */
      const string phone_zh_CN = "^(\\+?0?86\\-?)?1[345789]\\d{9}$";

      const string verifyCode = "^\\d{6}$";

      /* 校验合法性 , 
       @return true, 合法 */
      bool isLegal(const string& regexStr, const string& str);
      bool isLegalPhone(const string& phoneNumber);
      bool isLegalVC(const string& vertifyCodeStr);
    };
    extern RegexVals regexVals;


    /*************___ nameless_carpool::Common   文件读取操作___*************** 
     ***************************************************************************/
    /* 从文件(path)中读取文本信息(content) 
          如果 srcPath 为空 , 会用到  可执行程序所在目录 + defSrcFileName  文件作为 srcPath 
          如果读取失败 , defContent 会用于填充 content
       @return true, 读取成功 , defContent 未使用
               false,读取事变 , 用 defContent 填充 content   */ 
    extern bool getContent(string& content, const string& srcPath, const string& defSrcFileName, const string& defContent);
    extern bool getContent(string& content, const string& defSrcFileName, const string& defContent);


    /*************___ nameless_carpool::Common::Date___************************ 
     ***************************************************************************/
    /* 封装 C / C++ 时间调用到单一对象中 
       默认的对这几种格式支持较好其他的测试力度不够需要注意
          - 纳秒 : 1970-1-1 00:00:00.000000000 
          - 微秒 : 1970-1-1 00:00:00.000000 
          - 毫秒 : 1970-1-1 00:00:00.000 
          -   秒 : 1970-1-1 00:00:00
       需要留意的是 system_clock::now() 默认获取的是 0 时区的时间
       */
    class Date {
      using time_point = std::chrono::system_clock::time_point;
      using system_clock = std::chrono::system_clock;
      template <typename _Rep, typename _Period>
      using Duration = std::chrono::duration<_Rep, _Period>;

      private :
        int8_t    tz;
        time_point timePoint;
        static string defFormatStr; 
        

      public :
        /* 获取一个新的 Date 对象 */
        static Date   newInstance(const int8_t& _tz = timeZone(), const time_point& _timePoint = system_clock::now());
        /* 判断时区是否合法 */
        static bool tzLegal(const int8_t& tz);
        /* 获取当前设备时区 */
        static int8_t timeZone();
        /* 根据 curTz 时区下的时间 curTime 计算目标时区 targetTz 的时间*/
        static timespec fixSecByTz (timespec& curTime, const int8_t& curTz, const int8_t& targetTz);
        /* 将格式化字符串转换为时间戳 */
        static timespec toTimespec(const string& str, const string& format = defFormatStr);
        /* timePoint 对象转换为时间戳对象 */
        static timespec toTimespec (const time_point& tp);
        /* 将以 X秒(比 纳秒 更精确的数据被抹除) 为单位的时间戳转换为时间戳对象 */
        static timespec toTimespec (const uint64_t& xSeconds);
        /* 将 uint64_t 时间戳格式化为 format 的格式 . 
            默认的 format = defFormatStr 被转换为 : 1970-01-01 00:00:00
            默认的 suffixWidth = 6       倍转换为 : 000000
              - 后缀最长为 9 , 大于 9 也按九 计算   1970-01-01 00:00:00.000000000
              - 后缀最小为 0 , 也就是没有后缀 ,     1970-01-01 00:00:00
              - 后缀宽度 只接受 0/3/6/9 其中一个值 , 默认 6 
            默认的转换结果 为 1970-01-01 00:00:00.000000 
            整个 结果 就是将 前缀和后缀做了字符串拼接                               */
        static string formatDef(const uint64_t& timeT, 
                                const string& format = defFormatStr, 
                                const uint8_t& suffixWidth = 6);
        static string formatDef(const timespec& timeT, 
                                const string& format = defFormatStr, 
                                const uint8_t& suffixWidth = 6);
      public :
        Date(const int8_t& _tz = timeZone(), const time_point&  _timePoint = system_clock::now());
      
      
      public : 

        /* #region 转换为指定单位的 duration 对象 */
          /* 将 timePoint 对应的 duration 根据模板变量做出相应的转换并 返回结果 
            可以通过 duration_cast<duration>().count() 得到相应单位的值 
            
            以上操作可以直接通过 durationCount() 完成 .
            >*/
          template <typename _Tp>
          struct __is_duration : std::false_type {};

          template <typename _Rep, typename _Period>
          struct __is_duration<Duration<_Rep, _Period>> : std::true_type {};

          template <typename _Tp>
          using __enable_if_is_duration = typename std::enable_if<__is_duration<_Tp>::value, _Tp>::type;
          /* 包装 std::chrono::duration_cast 
            模板参数类型限定为 std::chrono::duration
            返回值类型与模板参数类型一致  */
          template<typename _ToDur>
          constexpr __enable_if_is_duration<_ToDur> /* 此编译时语句用于限定模板入参类型 */
          durationCast() {
            return duration_cast<_ToDur>(timePoint.time_since_epoch());
          }

          template <typename _Tp>
          using __get_int_if_is_duration = typename std::enable_if<__is_duration<_Tp>::value, uint64_t>::type;

          /* 转换未包含时区修正动作 */
          template<typename _ToDur>
          constexpr __get_int_if_is_duration<_ToDur>
          durationCount() {
            return durationCast<_ToDur>().count();
          }
        /* #endregion */

        /* #region 转换为指定的秒数 */
          /*************************************************************************
           * 度量 Unix 时间（从协调世界时 (UTC) 1970-1-1 00:00:00 开始的时间，不计闰秒）
           ************************************************************************/
          /*  秒表示  : 1970-01-01 00:00:00   ,  withTzInfo= true, 按时区( tz )修正*/
          uint64_t toSec(bool withTzInfo = true);
          /* 毫秒表示 : 1970-01-01 00:00:00.000   ,  withTzInfo= true, 按时区( tz )修正*/
          uint64_t toMilliSec(bool withTzInfo = true);
          /* 微秒表示 : 1970-01-01 00:00:00.000000   ,  withTzInfo= true, 按时区( tz )修正*/
          uint64_t toMicroSec(bool withTzInfo = true);
          /* 纳秒表示 : 1970-01-01 00:00:00.000000000   ,  withTzInfo= true, 按时区( tz )修正*/
          uint64_t toNanoSec(bool withTzInfo = true);
        /* #endregion */
        
        /* #region 格式化到字符串 , 3/6/9 表示后缀的毫秒/微秒/纳秒*/
          /* 默认的 1970-01-01 00:00:00*/     
          string   formatStr(bool withTzInfo = true, const string& format = defFormatStr) ;
          /* 1970-01-01 00:00:00.000*/    
          string   formatStr3(bool withTzInfo = true) ;
          /* 1970-01-01 00:00:00.000000*/    
          string   formatStr6(bool withTzInfo = true) ;
          /* 1970-01-01 00:00:00.000000000*/    
          string   formatStr9(bool withTzInfo = true) ;
        /* #endregion */

        /* #region 根据时区计算修正秒数 */
          int64_t fixTzSecond() ;
        /* #endregion */

      //public :

    };
  

    /*************___ nameless_carpool::Common::Number___********************** 
     ***************************************************************************/
    struct Number {
      /* 获取 6 未随机数 */
      static int randomInt(const int &min = 100000, const int &max = 999999) ;
    };
  
    /*************___ nameless_carpool::Common::GetErr 获取异常信息___***********
     ***************************************************************************/
    struct GetErr {
      static string getErr(const std::ios_base::iostate& state) ;
    };
  }
}
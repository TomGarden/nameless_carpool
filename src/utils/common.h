#pragma once

/* 用于记录一些常量的值 */

#include <chrono>
#include <random>
#include <string>

#include "../../libs/date_3.0.1/include/date/date.h"
#include "../../libs/date_3.0.1/include/date/tz.h"

namespace nameless_carpool {

  namespace Common {
    using namespace std;
    using String = std::string;

    /*************___ nameless_carpool::Common::Config  全局配置___**************
     ***************************************************************************/
    namespace Config {
      /* 验证码失效时间 */
      const int VERTIFY_CODE_LIFE_TIME = 5 * 60; /* 五分钟 */
    }                                            // namespace Config

    /*************___ nameless_carpool::Common::Regex  正则___*****************
     ***************************************************************************/
    struct RegexVals {
      /* 其他国家手机号字符串匹配规则 : https://cloud.tencent.com/developer/article/1140913 */
      /* 中国 手机号正则字符串  */
      const String phone_zh_CN = "^(\\+?0?86\\-?)?1[345789]\\d{9}$";

      const String verifyCode = "^\\d{6}$";

      /* 校验合法性 ,
       @return true, 合法 */
      bool isLegal(const String& regexStr, const String& str);
      bool isLegalPhone(const String& phoneNumber);
      bool isLegalVC(const String& vertifyCodeStr);
    };
    extern RegexVals regexVals;

    /*************___ nameless_carpool::Common   文件读取操作___***************
     ***************************************************************************/
    /* 从文件(path)中读取文本信息(content)
          如果 srcPath 为空 , 会用到  可执行程序所在目录 + defSrcFileName  文件作为 srcPath
          如果读取失败 , defContent 会用于填充 content
       @return true, 读取成功 , defContent 未使用
               false,读取事变 , 用 defContent 填充 content   */
    extern bool getContent(String& content, const String& srcPath, const String& defSrcFileName, const String& defContent);
    extern bool getContent(String& content, const String& defSrcFileName, const String& defContent);

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

      using SystemClock       = std::chrono::system_clock;
      using SysClockTimePoint = std::chrono::system_clock::time_point;
      template <typename _Rep, typename _Period>
      using Duration = std::chrono::duration<_Rep, _Period>;
      using TimeZone = date::time_zone;
      //using ZonedTime = date::zoned_time;

      template <typename _Tp>
      struct __is_duration : std::false_type {};
      template <typename _Rep, typename _Period>
      struct __is_duration<Duration<_Rep, _Period>> : std::true_type {};
      template <typename _Tp>
      using __enable_if_is_duration = typename std::enable_if<__is_duration<_Tp>::value, _Tp>::type;
      template <typename _Tp>
      using __get_int_if_is_duration = typename std::enable_if<__is_duration<_Tp>::value, uint64_t>::type;
      template <typename _Tp>
      using __get_string_if_is_duration = typename std::enable_if<__is_duration<_Tp>::value, std::string>::type;


     private:
      SysClockTimePoint timePoint;
      const TimeZone*   tzPtr;
      static String     defFormatStr;

     public:
      /* 获取一个新的 Date 对象 */
      static Date newInstance(const String&    tzName,
                              const SysClockTimePoint& _timePoint = SystemClock::now());
      static Date newInstance(const TimeZone*  _tzPtr     = date::current_zone(),
                              const SysClockTimePoint& _timePoint = SystemClock::now());
      /* 判断时区是否合法 */
      static bool tzLegal(const String& tzName);
      /* 根据 curTz 时区下的时间 curTime 计算目标时区 targetTz 的时间*/
      static timespec fixSecByTz(timespec& curTime, const String& curTz, const String& targetTz);
      /* 将格式化字符串转换为时间戳 */
      static timespec toTimespec(const String& str, const String& format = defFormatStr);
      /* timePoint 对象转换为时间戳对象 */
      static timespec toTimespec(const SysClockTimePoint& tp);
      /* 将以 X秒(比 纳秒 更精确的数据被抹除) 为单位的时间戳转换为时间戳对象 */
      static timespec toTimespec(const uint64_t& xSeconds);
      /* 将 uint64_t 时间戳格式化为 format 的格式 .
          默认的 format = defFormatStr 被转换为 : 1970-01-01 00:00:00
          默认的 suffixWidth = 6       倍转换为 : 000000
            - 后缀最长为 9 , 大于 9 也按九 计算   1970-01-01 00:00:00.000000000
            - 后缀最小为 0 , 也就是没有后缀 ,     1970-01-01 00:00:00
            - 后缀宽度 只接受 0/3/6/9 其中一个值 , 默认 6
          默认的转换结果 为 1970-01-01 00:00:00.000000
          整个 结果 就是将 前缀和后缀做了字符串拼接                              
           */
          // using nameless_carpool::Common::Date::TimePoint = std::chrono::_V2::system_clock::time_point 
          // using std::chrono::nanoseconds = std::chrono::duration<int64_t, std::nano> 

      template <typename _Duration, typename _Period>
      static constexpr __get_string_if_is_duration<_Duration> 
      formatDef(const date::local_time<_Period>& timeT, const String& format = defFormatStr) {
        return date::format(format, date::floor<_Duration>(timeT));
      }
      template <typename _Duration>
      static constexpr __get_string_if_is_duration<_Duration> 
      formatDef(const uint64_t& timeT, const String& format = defFormatStr) {
        /* 1666686660000000000 */
        String inputStrTime = to_string(timeT);
        
        date::local_time<std::chrono::nanoseconds> localTime ; {
          unsigned long long       nanoSecond;
          int    size = 19;
          if (inputStrTime.size() < size) {
            String outputStrTime(size, '0');  //= "0000000000000000000";
            outputStrTime.replace(0, inputStrTime.size(), inputStrTime);
            nanoSecond = stoull(outputStrTime);
          } else {
            nanoSecond = timeT;
          }
          std::chrono::nanoseconds durNano     {nanoSecond};
          localTime = date::local_time(durNano);
        } /* localTime 初始化完毕 */

        return formatDef<_Duration>(localTime, format);
      }
      template <typename _Duration>
      static constexpr __get_string_if_is_duration<_Duration> 
      formatDef(const timespec& timeT, const String& format = defFormatStr) {
        std::chrono::nanoseconds durNano = std::chrono::seconds{timeT.tv_sec} + std::chrono::nanoseconds{timeT.tv_sec};
        date::local_time localTime        {durNano};
        return formatDef<_Duration>(durNano, format);
      }


     public:
      Date(const TimeZone* _tzPtr = date::current_zone(), const SysClockTimePoint& _timePoint = SystemClock::now());

     public:
      /* #region 转换为指定单位的 duration 对象 */
        /* 将 timePoint 对应的 duration 根据模板变量做出相应的转换并 返回结果
          可以通过 duration_cast<duration>().count() 得到相应单位的值

          以上操作可以直接通过 durationCount() 完成 .
          >*/

        /* 包装 std::chrono::duration_cast
          模板参数类型限定为 std::chrono::duration
          返回值类型与模板参数类型一致  */
        template <typename _ToDur>
        constexpr __enable_if_is_duration<_ToDur> /* 此编译时语句用于限定模板入参类型 */
        durationCast(const TimeZone* _tzPtr) {

          std::chrono::nanoseconds duration;

          if(nullptr == _tzPtr){
            duration = timePoint.time_since_epoch();
          } else {
            duration = date::make_zoned(_tzPtr, timePoint).get_local_time().time_since_epoch();
          }

          return duration_cast<_ToDur>(duration);
        }
        template <typename _ToDur>
        constexpr __enable_if_is_duration<_ToDur> /* 此编译时语句用于限定模板入参类型 */
        durationCast() { return durationCast<_ToDur>(tzPtr); }

        /* 转换未包含时区修正动作 */
        template <typename _ToDur>
        constexpr __get_int_if_is_duration<_ToDur>
        durationCount(const TimeZone* _tzPtr) { return durationCast<_ToDur>(_tzPtr).count(); }
        template <typename _ToDur>
        constexpr __get_int_if_is_duration<_ToDur>
        durationCount() { return durationCount<_ToDur>(tzPtr).count(); }

      /* #endregion */

      /* #region 转换为指定的秒数 */
        /*************************************************************************
         * 度量 Unix 时间（从协调世界时 (UTC) 1970-1-1 00:00:00 开始的时间，不计闰秒）
         ************************************************************************/
        /*  秒表示  : 1970-01-01 00:00:00   ,  withTzInfo= true, 按时区( tz )修正*/
        uint64_t toSec();
        uint64_t toSec(const TimeZone*  _tzPtr);
        /* 毫秒表示 : 1970-01-01 00:00:00.000   ,  withTzInfo= true, 按时区( tz )修正*/
        uint64_t toMilliSec();
        uint64_t toMilliSec(const TimeZone*  _tzPtr);
        /* 微秒表示 : 1970-01-01 00:00:00.000000   ,  withTzInfo= true, 按时区( tz )修正*/
        uint64_t toMicroSec();
        uint64_t toMicroSec(const TimeZone*  _tzPtr);
        /* 纳秒表示 : 1970-01-01 00:00:00.000000000   ,  withTzInfo= true, 按时区( tz )修正*/
        uint64_t toNanoSec();
        uint64_t toNanoSec(const TimeZone*  _tzPtr);
      /* #endregion */

      /* #region 格式化到字符串 , 3/6/9 表示后缀的毫秒/微秒/纳秒*/
        /** 
            1970-01-01 00:00:00             std::chrono::seconds
            1970-01-01 00:00:00.000         std::chrono::milliseconds
            1970-01-01 00:00:00.000000      std::chrono::microseconds
            1970-01-01 00:00:00.000000000   std::chrono::nanoseconds
          */

        template <typename duration>
        constexpr __get_string_if_is_duration<duration> 
        formatStr() { return formatStr<duration>(tzPtr); }
        template <typename duration>
        constexpr __get_string_if_is_duration<duration> 
        formatStr(const TimeZone* tzP) {
          uint64_t sec;
          if(nullptr == tzP) {
            sec = timePoint.time_since_epoch().count();
          } else {
            date::zoned_time zonedTime = date::make_zoned(tzP, timePoint);
            sec = zonedTime.get_local_time().time_since_epoch().count();
          }
          return formatDef<duration>(sec);
        }
      /* #endregion */

      // public :

      /************** 预计用 HowardHinnant/date 重构 date 类 ********************/
      /************** 预计用 HowardHinnant/date 重构 date 类 ********************/

     public:
    };

    /*************___ nameless_carpool::Common::Number___**********************
     ***************************************************************************/
    struct Number {
      /* 获取 6 未随机数 */
      static int randomInt(const int& min = 100000, const int& max = 999999);
    };

    /*************___ nameless_carpool::Common::GetErr 获取异常信息___***********
     ***************************************************************************/
    struct GetErr {
      static String getErr(const std::ios_base::iostate& state);
    };
  }  // namespace Common
}  // namespace nameless_carpool
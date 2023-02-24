#pragma once

/* 用于记录一些常量的值 */

#include <chrono>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>

#include "libs/date_3.0.1/include/date/date.h"
#include "libs/date_3.0.1/include/date/tz.h"

namespace nameless_carpool {
  namespace Common {
    namespace Config {};

    struct RegexVals;
    extern RegexVals regexVals;

    class Date;
    struct Number;
    struct GetErr;
  }
}


namespace nameless_carpool::Common::Config { /* 全局配置 */
  /* 验证码失效时间 */
  const uint64_t VERTIFY_CODE_LIFE_TIME_SECOND       = 5 * 60; /* 五分钟 */
  const uint64_t VERTIFY_CODE_LIFE_TIME_MILLISECONDS = VERTIFY_CODE_LIFE_TIME_SECOND * 1000;
  const uint64_t VERTIFY_CODE_LIFE_TIME_MICROSECONDS = VERTIFY_CODE_LIFE_TIME_SECOND * 1000000;
  const uint64_t VERTIFY_CODE_LIFE_TIME_NANOSECONDS  = VERTIFY_CODE_LIFE_TIME_SECOND * 1000000000;
}

struct nameless_carpool::Common::RegexVals { /* 正则 */
  /* 其他国家手机号字符串匹配规则 : https://cloud.tencent.com/developer/article/1140913 */
  /* 中国 手机号正则字符串  */
  const std::string phone_zh_CN = "^(\\+?0?86\\-?)?1[345789]\\d{9}$";

  const std::string verifyCode = "^\\d{6}$";

  /* 校验合法性 ,
   @return true, 合法 */
  bool isLegal(const std::string& regexStr, const std::string& str);
  bool isLegalPhone(const std::string& phoneNumber);
  bool isLegalVC(const std::string& vertifyCodeStr);
};

namespace nameless_carpool::Common {/* 文件读取操作 */
  /* 从文件(path)中读取文本信息(content)
        如果 srcPath 为空 , 会用到  可执行程序所在目录 + defSrcFileName  文件作为 srcPath
        如果读取失败 , defContent 会用于填充 content
     @return true, 读取成功 , defContent 未使用
             false,读取事变 , 用 defContent 填充 content   */

  /** @description: 从文件(path)中读取文本信息(content)
   *               如果 srcPath 为空 , 会用到  可执行程序所在目录 + defSrcFileName  文件作为 srcPath
   *               如果读取失败 , defContent 会用于填充 content
   * @param {std::string&} content         文件内容会存储到这个变量中
   * @param {std::string&} srcPath         文件父文件夹路径
   * @param {std::string&} defSrcFileName  文件名
   * @param {std::string&} defContent      读取文件失败 content 填充 这个值
   * @return {*}                      true, 读取成功 , defContent 未使用
   *                                  false,读取失败 , 用 defContent 填充 content
   */
  extern bool getContent(std::string& content, const std::string& srcPath, const std::string& defSrcFileName, const std::string& defContent);
  extern bool getContent(std::string& content, const std::string& fileFullPath, const std::string& defContent);

}  // namespace nameless_carpool::Common

/* 封装 C / C++ 时间调用到单一对象中
   默认的对这几种格式支持较好其他的测试力度不够需要注意
      - 纳秒 : 1970-1-1 00:00:00.000000000
      - 微秒 : 1970-1-1 00:00:00.000000
      - 毫秒 : 1970-1-1 00:00:00.000
      -   秒 : 1970-1-1 00:00:00
   需要留意的是 system_clock::now() 默认获取的是 0 时区的时间 , 它不会随着操作系统时区的变化而变化
   */
class nameless_carpool::Common::Date {

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
      /* 对于类似 tzPtr 和 timePoint , 定义变量的顺序和 构造函数初始化变量的顺序如果不同 , 编译器会抱怨的 */
      const TimeZone*   tzPtr;
      SysClockTimePoint timePoint;
      static std::string     defFormatStr;

     public: /* static member */
      /* 获取一个新的 Date 对象 */
      static Date newInstance(const std::string&    tzName,
                              const SysClockTimePoint& _timePoint = SystemClock::now());
      static Date newInstance(const TimeZone*  _tzPtr     = date::current_zone(),
                              const SysClockTimePoint& _timePoint = SystemClock::now());
      /* 判断时区是否合法 */
      static bool tzLegal(const std::string& tzName);
      /* 根据 curTz 时区下的时间 curTime 计算目标时区 targetTz 的时间*/
      static timespec fixSecByTz(timespec& curTime, const std::string& curTz, const std::string& targetTz);

      /** @description:       将格式化字符串 转换为 local_time 
       * @param {std::string&} str
       * @param {std::string&} format
       * @return {*}
       */
      static date::local_time<std::chrono::nanoseconds> toLocalTime(const std::string& str, const std::string& format = defFormatStr);
      /** @description:  将格式化字符串转换为时间戳 , 这个字符串将被转换为不带有时区信息的时间戳 (即 时区 为 0 的时间戳)
       * @param {std::string&} str     格式化字符串
       * @param {std::string&} format  预期格式化字符串和 这个 格式相匹配
       * @return {*}
       */
      static timespec toTimespec(const std::string& str, const std::string& format = defFormatStr);
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
      formatDef(const date::local_time<_Period>& timeT, const std::string& format = defFormatStr) {
        return date::format(format, date::floor<_Duration>(timeT));
      }
      template <typename _Duration>
      static constexpr __get_string_if_is_duration<_Duration> 
      formatDef(const uint64_t& timeT, const std::string& format = defFormatStr) {
        /* 1666686660000000000 */
        const std::string& inputStrTime = std::to_string(timeT);
        
        date::local_time<std::chrono::nanoseconds> localTime ; {
          unsigned long long       nanoSecond;
          uint8_t    size = 19;
          if (inputStrTime.size() < size) {
            const std::string& outputStrTime = std::string(size, '0') /* = "0000000000000000000"; */
                                                   .replace(0, inputStrTime.size(), inputStrTime);

            // std::string outputStrTime(size, '0');  
            // outputStrTime.replace(0, inputStrTime.size(), inputStrTime);
            nanoSecond = stoull(outputStrTime);
          } else {
            nanoSecond = timeT;
          }
          std::chrono::nanoseconds durNano{nanoSecond};
          localTime = date::local_time<std::chrono::nanoseconds>(durNano);
        } /* localTime 初始化完毕 */

        return formatDef<_Duration>(localTime, format);
      }
      template <typename _Duration>
      static constexpr __get_string_if_is_duration<_Duration> 
      formatDef(const timespec& timeT, const std::string& format = defFormatStr) {
        std::chrono::nanoseconds durNano = std::chrono::seconds{timeT.tv_sec} + std::chrono::nanoseconds{timeT.tv_sec};
        date::local_time<std::chrono::nanoseconds> localTime        {durNano};
        return formatDef<_Duration>(durNano, format);
      }


     public:
      Date(const TimeZone*          _tzPtr     = date::current_zone(),
           const SysClockTimePoint& _timePoint = SystemClock::now())
          : tzPtr(_tzPtr), timePoint(_timePoint){};

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
        /* constexpr  */__get_string_if_is_duration<duration> 
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

};

struct nameless_carpool::Common::Number {
  /* 获取 6 未随机数 */
  static int randomInt(const int& min = 100000, const int& max = 999999);
};

struct nameless_carpool::Common::GetErr { /* 获取异常信息 */
  static std::string getErr(const std::ios_base::iostate& state);
};

date 的使用过程中需要注意的一个问题是 
- 当前时区 : Asia/Shanghai
- zonedTime.get_sys_time() 
   - 时间戳        : 1667264810618250550             -> 问题是这个时间戳实际上是 2022-11-01 09:06:50.618250550
   - 转换为字符串   : 2022-11-01 01:06:50.618250550
   - 字符串转时间戳 : 1667264810618250550             -> 转回来竟然没问题 , 很诧异
- zonedTime.get_local_time();
   - 时间戳        : 1667293610618250550             -> 问题是这个时间戳实际上是 2022-11-01 17:06:50.618250550
   - 转换为字符串   : 2022-11-01 09:06:50.618250550
   - 字符串转时间戳 : 1667293610618250550             -> 转回来竟然没问题 , 很诧异



  ```c++
  #include <chrono>
  #include <iostream>

  #include "../../libs/date_3.0.1/include/date/date.h"
  #include "../../libs/date_3.0.1/include/date/tz.h"

  using namespace std;
  std::string dateFormatStr = "%Y-%m-%d %H:%M:%S";
  using locale_time_nano    = date::local_time<std::chrono::nanoseconds>;


  locale_time_nano getTimestemp(const std::string& formatTimestemp) {

    std::istringstream inputSs {formatTimestemp};
    locale_time_nano   resultLocalTime;
    inputSs >> date::parse(dateFormatStr, resultLocalTime);

    return resultLocalTime;
  }


  template <typename T>
  void test(std::chrono::system_clock::time_point& timePoint) {
    static_assert(chrono::__is_duration<T>::value, "阿斯卡打法;啥快递附件;啊山东科技发;");

    const date::time_zone* timeZone = date::current_zone();
    date::zoned_time zonedTime = date::make_zoned(timeZone, timePoint);
    std::chrono::time_point sysTP = zonedTime.get_sys_time();
    // date::local_time localTP = zonedTime.get_local_time();
    locale_time_nano localTP = zonedTime.get_local_time();

    std::string sysStrFormat = date::format(dateFormatStr, date::floor<T>(sysTP));
    std::string localStrFormat = date::format(dateFormatStr, date::floor<T>(localTP));

    cout << std::to_string(sysTP.time_since_epoch().count())   << "\t"
        << std::to_string(date::floor<T>(sysTP).time_since_epoch().count()) << "\t"
        << sysStrFormat << "\t"
        << getTimestemp(sysStrFormat).time_since_epoch().count() << endl;
    cout << std::to_string(localTP.time_since_epoch().count()) << "\t"
        << std::to_string(date::floor<T>(localTP).time_since_epoch().count()) << "\t"
        << localStrFormat << "\t"
        << getTimestemp(localStrFormat).time_since_epoch().count() << endl;

  }




  int main() {
    
    // using namespace date;
    std::string str = date::current_zone()->name();
    std::cout << str.size() << std::endl
              << str << std::endl;

    /* 我们说我们想要通过 时区 字符串 获取 时区 , 实际上 , 我们也想通过 时区字符串 对给定的时间点做出针对时区的修正  */

    auto                   flag     = date::current_zone();
    const date::time_zone *timeZone = date::current_zone();
    date::floor<date::days>(std::chrono::system_clock::now());

    date::zoned_time        testFlag2    = date::make_zoned(date::current_zone(), std::chrono::system_clock::now());


    std::chrono::time_point testSysTime  = testFlag2.get_sys_time();
    locale_time_nano       testLocalTime = testFlag2.get_local_time();
    date::sys_info         testInfo      = testFlag2.get_info();
    const date::time_zone *testTimeZone  = testFlag2.get_time_zone();

    std::string sysFormatTime = date::format(dateFormatStr, date::floor<std::chrono::nanoseconds>(testSysTime)) ;
    std::string localeFormatTime = date::format(dateFormatStr, date::floor<std::chrono::nanoseconds>(testLocalTime)) ;
    cout << testSysTime.time_since_epoch().count() << "\t" << sysFormatTime << endl;
    cout << testLocalTime.time_since_epoch().count() << "\t" << localeFormatTime << endl;
    cout << testInfo.abbrev << endl;

    std::chrono::time_point timePoint     = std::chrono::system_clock::now();
    cout << timePoint.time_since_epoch().count() << endl;

    test<std::chrono::nanoseconds>(timePoint);
    test<std::chrono::microseconds>(timePoint);
    test<std::chrono::milliseconds>(timePoint);
    test<std::chrono::seconds>(timePoint);

    std::string format = "%Y-%m-%d %H:%M:%S";
  }
  ```
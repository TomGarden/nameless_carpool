/* 测试 https://github.com/HowardHinnant/date */

#include <chrono>
#include <iostream>

#include "../../libs/date_3.0.1/include/date/date.h"
#include "../../libs/date_3.0.1/include/date/tz.h"
#include <stdio.h>
#include <time.h>
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

  // const date::time_zone* timeZone = date::current_zone();
  // const date::time_zone* timeZone = date::time_zone();
  date::zoned_time zonedTime = date::make_zoned("Asia/Shanghai", timePoint);
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


void testUtcTime() {

  date::utc_clock::time_point utcTimePoint     = date::utc_clock::now();
  cout << "utcTimePoint\t" << utcTimePoint.time_since_epoch().count() << endl;

  date::local_time<std::chrono::nanoseconds> localTimePiont = date::utc_clock::to_local(utcTimePoint);
  cout << "localTimePiont\t" << localTimePiont.time_since_epoch().count() << endl;

  std::chrono::system_clock::time_point sysTimePoint = date::utc_clock::to_sys(utcTimePoint);
  cout << "sysTimePoint\t" << sysTimePoint.time_since_epoch().count() << endl;

  date::utc_clock::time_point utcTimePoint2 = date::to_utc_time(std::chrono::system_clock::now());
  cout << "utcTimePoint2\t" << utcTimePoint2.time_since_epoch().count() << endl;


  test<std::chrono::nanoseconds>(sysTimePoint);
  test<std::chrono::microseconds>(sysTimePoint);
  test<std::chrono::milliseconds>(sysTimePoint);
  test<std::chrono::seconds>(sysTimePoint);

}

void testLocaleTime() {

  date::utc_clock::time_point utcTimePoint     = date::utc_clock::now();
  cout << "utcTimePoint\t" << utcTimePoint.time_since_epoch().count() << endl;

  date::local_time<std::chrono::nanoseconds> localTimePiont = date::utc_clock::to_local(utcTimePoint);
  cout << "localTimePiont\t" << localTimePiont.time_since_epoch().count() << endl;
  
  std::chrono::system_clock::time_point sysTimePoint = date::utc_clock::to_sys(utcTimePoint);
  cout << "sysTimePoint\t" << sysTimePoint.time_since_epoch().count() << endl;


  test<std::chrono::nanoseconds>(sysTimePoint);
  test<std::chrono::microseconds>(sysTimePoint);
  test<std::chrono::milliseconds>(sysTimePoint);
  test<std::chrono::seconds>(sysTimePoint);

}


void ask() {
  const date::time_zone* timeZone = date::current_zone();
  std::chrono::system_clock::time_point timePoint =  std::chrono::system_clock::now();

  date::zoned_time zonedTime = date::make_zoned(timeZone, timePoint);
  std::chrono::time_point sysTP = zonedTime.get_sys_time();
  date::local_time<std::chrono::nanoseconds> localTP = zonedTime.get_local_time();
  uint64_t sysTimestemp = sysTP.time_since_epoch().count();
  uint64_t localeTimestemp = localTP.time_since_epoch().count();

  std::string dateFormatStr = "%Y-%m-%d %H:%M:%S";
  std::string sysStrFormat = date::format(dateFormatStr, sysTP);
  std::string localStrFormat = date::format(dateFormatStr, localTP);

  std::cout << "-------------------------------" << std::endl;
  std::cout << std::to_string(sysTimestemp)   << "\t"
            << sysStrFormat << "\t" << std::endl;
  std::cout << std::to_string(localeTimestemp) << "\t"
            << localStrFormat << "\t" << std::endl;
}



int offset ()
{
	// 获取系统时间
	time_t _rt = time(NULL);
	// 系统时间转换为GMT时间
	tm _gtm = *gmtime(&_rt);
	// 系统时间转换为本地时间
	tm _ltm = *localtime(&_rt);
	printf("UTC:       %s", asctime(&_gtm));
	printf("local:     %s", asctime(&_ltm));
	// 再将GMT时间重新转换为系统时间
	time_t _gt = mktime(&_gtm);
	tm _gtm2 = *localtime(&_gt);
	// 这时的_gt已经与实际的系统时间_rt有时区偏移了,计算两个值的之差就是时区偏的秒数,除60就是分钟
	int offset = ((_rt - _gt ) + (_gtm2.tm_isdst ? 3600 : 0)) / 60;
	printf(" offset (minutes) %d", offset);
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

  // std::chrono::time_point timePoint     = std::chrono::system_clock::now();
  // std::chrono::time_point timePoint     = date::utc_clock::now();

  testUtcTime();

  ask();

  offset ();

  std::string format = "%Y-%m-%d %H:%M:%S";
  // std::chrono::nanoseconds timeT {1667124554089197048};
  // date::local_time localTime  {timeT};
  // std::string formatDate = date::format(format, date::floor<std::chrono::nanoseconds>(localTime));

  // date::make_zoned(tzP, timePoint)


  // std::cout << "======>>>>" << formatDate << std::endl;




}

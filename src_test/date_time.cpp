

#include "date_time.h"

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include <chrono>


using namespace std::chrono;

/** 类和函数 学习 自 《C程序设计语言》/https://zh.cppreference.com/w/首页 */
void src_test::DateTime::test_time_t()
{
  /* 当地当前时间戳 单位 秒 */
  time_t oneTimeT = time(NULL);
  /* 将时间戳转换为结构体 仍然表示当地当前时间*/
  tm *localTime = localtime(&oneTimeT);
  int arySize = 30;
  char localTimeStr[arySize];
  char fmtStr[] = "%Y-%m-%d %H:%M:%S %Z";
  /* 将当地当前时间格式化为字符串 方便输出 */
  strftime(localTimeStr, arySize, fmtStr, localTime);
  printf("本地时间戳(%d)\n本地时间字符串(%s)\n", oneTimeT, localTimeStr);

  /* 本地时间时间戳 单位 秒 */
  time_t curTime = time(NULL);
  /* 与本地时间的相对应的  0 时区的当前时间戳 */
  tm *zeroTzTm = gmtime(&curTime);
  /* 抹除时区时间差后的 UTC(0) 时间戳 ， 这个抹除的时间差就是时区时间差。
     维基百科记录了，0时区以东的时区用正数，0时区以西的时区用负数；
     此处是在抹除时区差 */
  time_t tz = (curTime - mktime(zeroTzTm)) / (60 * 60);
  printf("当前时区为  >> %d\n", tz);
}

void src_test::DateTime::test_cpp_time()
{
  system_clock::time_point timePoint = system_clock::now();
  auto timeStemp = timePoint.time_since_epoch().count();
  /* system_clock 设计用来表示时钟 */
  std::cout << "C++ 获取时间戳 [精度(纳秒) > 秒 > 微秒 > 毫秒]>> " << timeStemp << std::endl
            /* system_clock 设计用来表示计时器，处理计时任务，时钟由于时间需要与网络同步并不一定总是在增加 */
            << "steady_clock : " << steady_clock::now().time_since_epoch().count() << std::endl
            /* 以 libstdc++-10-dev 库来看， high_resolution_clock 是 的别名*/
            << "high_resolution_clock : " << high_resolution_clock::now().time_since_epoch().count() << std::endl;

  system_clock::time_point ts = system_clock::now();
  system_clock::to_time_t(ts); //转换为 time_t 再作格式化输出

  std::cout << "时区着一块， c++ 20 的标准 在 GNU GCC 还没有完全实现， 所以也是向 C 语言靠拢了" << std::endl;

  // timezone
}



#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <chrono>

#include "date_time.h"

using namespace std::chrono;
using namespace std;

/** 类和函数 学习 自 《C程序设计语言》/https://zh.cppreference.com/w/首页 */
void src_test::DateTime::test_time_t() {
  /* 当地当前时间戳 单位 秒 */
  time_t oneTimeT = time(NULL);
  /* 将时间戳转换为结构体 仍然表示当地当前时间*/
  tm *localTime = localtime(&oneTimeT);
  int arySize = 30;
  char localTimeStr[arySize];
  char fmtStr[] = "%Y-%m-%d %H:%M:%S %Z";
  /* 将当地当前时间格式化为字符串 方便输出 */
  strftime(localTimeStr, arySize, fmtStr, localTime);
  printf("本地时间戳(%ld)\n本地时间字符串(%s)\n", oneTimeT, localTimeStr);

  /* 本地时间时间戳 单位 秒 */
  time_t curTime = time(NULL);
  /* 与本地时间的相对应的  0 时区的当前时间戳 */
  tm *zeroTzTm = gmtime(&curTime);
  /* 抹除时区时间差后的 UTC(0) 时间戳 ， 这个抹除的时间差就是时区时间差。
     维基百科记录了，0时区以东的时区用正数，0时区以西的时区用负数；
     此处是在抹除时区差 */
  time_t tz = (curTime - mktime(zeroTzTm)) / (60 * 60);
  printf("当前时区为  >> %ld\n", tz);
}

void src_test::DateTime::test_cpp_time() {
  system_clock::time_point timePoint = system_clock::now();
  auto timeStemp = timePoint.time_since_epoch().count();
  /* system_clock 设计用来表示时钟 */
  std::cout << "C++ 获取时间戳 [精度(纳秒) > 秒 > 微秒 > 毫秒]>> " << timeStemp << std::endl
            /* system_clock 设计用来表示计时器，处理计时任务，时钟由于时间需要与网络同步并不一定总是在增加 */
            << "steady_clock : " << steady_clock::now().time_since_epoch().count() << std::endl
            /* 以 libstdc++-10-dev 库来看， high_resolution_clock 是 system_clock 的别名*/
            << "high_resolution_clock : " << high_resolution_clock::now().time_since_epoch().count() << std::endl;
//1663579465 367267 675
  system_clock::time_point ts = system_clock::now();
  system_clock::to_time_t(ts); //转换为 time_t 再作格式化输出

  std::cout << "时区着一块， c++ 20 的标准 在 GNU GCC 还没有完全实现， 所以也是向 C 语言靠拢了" << std::endl;

  // timezone
}

void src_test::DateTime::test_cpp_time2() {
  // template< typename _ToDur, typename _Rep, typename _Period >
  // using duration_cast = std::chrono::duration_cast<_ToDur, _Rep, _Period>;
  //template<class T>
  // using duration_cast = std::chrono::duration_cast;
  // using duration_cast_microseconds = std::chrono::duration_cast<std::chrono::microseconds>;

  // system_clock::time_point timePoint = system_clock::now();
  // auto durationVal = timePoint.time_since_epoch();
  // cout << durationVal.count() << endl;
  // cout << duration_cast<nanoseconds>(durationVal).count() << endl;
  // cout << duration_cast<microseconds>(durationVal).count() << endl;
  // cout << duration_cast<milliseconds>(durationVal).count() << endl;
  // cout << duration_cast<seconds>(durationVal).count() << endl;

  // uint64_t second = duration_cast<seconds>(durationVal).count();
  // gmtime(second)


}

string defFormatStr = "%Y-%m-%d %H:%M:%S";

      string formatDef(const timespec& timeT, 
                             const string& format /* = defFormatStr */, 
                             const uint8_t& suffixWidth /* = 6 */) {
        tm* tmPtr = gmtime(&(timeT.tv_sec));
        
        int arySize = 1024;
        char localTimeStr[arySize];
        size_t count = strftime(localTimeStr, arySize, format.c_str(), tmPtr);
        if(count == 0) {
          throw logic_error("在能存储整个字符串前抵达 arySize ，则返回 0 ，写入内容是未定义的。");
        }

        string prefix = string(localTimeStr, count);
        string suffix = "000000000"; {
          /* 将毫秒/微秒/纳秒部分转换为后缀 */
          if(timeT.tv_nsec < 1) {
            /* empty */
          } else {
            string nsecStr = to_string(timeT.tv_nsec);
            if(nsecStr.size() > 9) {
              nsecStr = nsecStr.substr(0, 9);
            }
            suffix.replace(0, nsecStr.size(), nsecStr);
          }

          /* 将后缀剪裁为 suffixWidth 指定长度 */
          uint8_t suffixW ; {
            if(suffixWidth > 9) {
              suffixW = 9;
            } else if(suffixWidth < 1) {
              suffixW = 0;
            } else {
              suffixW = suffixWidth;
            }
          }
          if(suffixW == 0) {
            suffix.clear();
          } else if(suffix.size() > suffixW) {
            suffix = suffix.substr(0, suffixW);
          }

          suffix = "." + suffix;
        }

        return (prefix + suffix);
      }
      string formatDef(const uint64_t& timeStemp, 
                             const string& format/*  = defFormatStr */,
                             const uint8_t& suffixWidth/*  = 6 */) {

        //{10'000'000'000'000'000'000ull}; /* 比 纳秒   更精细 , 忽略更精细的部分*/
        //{    10'000'000'000'000'000ull}; /* 比 微秒   更精细 */
        //{        10'000'000'000'000ull}; /* 比 毫秒   更精细 */
        //{            10'000'000'000ull}; /* 比 秒(S) 更精细 */
        //              0'000'000'000      /* 以 秒 为单位需要站 十个 整数位 */  

        timespec timeSpec {0, 0};

        string tmpStr = to_string(timeStemp);
        
        if(int stringLen = tmpStr.size(); stringLen <= 10) {
          timeSpec.tv_sec = static_cast<time_t>(timeStemp);
        } else {
          string strSecond = tmpStr.substr(0, 10);
          string strOther; {
            if (stringLen <= 19) {
              strOther = tmpStr.substr(10);
            } else {
              strOther = tmpStr.substr(10, 9);
            }
          }
          timeSpec.tv_sec = stoull(strSecond);
          timeSpec.tv_nsec = stoull(strOther);
        }

        return formatDef(timeSpec, format, suffixWidth);
      }

void printFormatDate() {
  time_point tp = system_clock::now();
  cout << formatDef(tp.time_since_epoch().count(), defFormatStr, 9) << endl;


}

// int main() {
//   printFormatDate();

//   return 0 ;
// }
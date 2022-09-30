

#include <regex>
#include <ctime>
#include <exception>
#include  <iomanip>

#include "common.h"
#include "linux_os.h"
#include "input_check.h"



namespace nameless_carpool::Common {
  using namespace std;
    

  /**********___nameless_carpool::Common::Regex  正则___***********************
   ***************************************************************************/  
  RegexVals regexVals;
  bool RegexVals::isLegal(const string& checkStr, const string& regexStr) {
      std::regex aRegex(regexStr);
      return regex_match(checkStr, aRegex);
    }
  bool RegexVals::isLegalPhone(const string& phoneNumber) {
      return isLegal(phoneNumber, this->phone_zh_CN);
    }

  bool RegexVals::isLegalVC(const string& vertifyCodeStr) {
      return isLegal(vertifyCodeStr, this->verifyCode);
    }
    
    
  /**********___nameless_carpool::Common   文件读取操作___*********************
   ***************************************************************************/
  bool getContent(string& content, const string& srcPath, const string& defSrcFileName, const string& defContent) {
      string defFilePath;
      if(srcPath.empty()) {
        if( getCurExeFd(defFilePath) ) {
          defFilePath.append("/").append(defSrcFileName);
        } else {
          content = defContent;
          return false;
        }
      }
      if( readStrFromFile(defFilePath, content) ) {
        return true;
      } else {
        content = defContent;
        return false;
      }
    }
  bool getContent(string& content, const string& defSrcFileName, const string& defContent) {
      const string path;
      return getContent(content, path, defSrcFileName, defContent);
    }
  

  /**********___nameless_carpool::Common::Date___******************************
   ***************************************************************************/
  /* #region class Date */
    using namespace std::chrono;
    using time_point = std::chrono::system_clock::time_point;

    /* #region 静态函数 & 静态变量 */
      string Date::defFormatStr = "%Y-%m-%d %H:%M:%S";

      Date     Date::newInstance(const int8_t& _tz, const time_point& _timePoint) {
        return Date(_tz, _timePoint);
      }
      
      bool Date::tzLegal(const int8_t& tz) {
        return (-12 <= tz && tz <= 12);
      }

      int8_t   Date::timeZone() {
        /* 本地时间时间戳 单位 秒 */
        time_t curTime = time(NULL);
        /* 与本地时间的相对应的  0 时区的当前时间戳 */
        tm *zeroTzTm = gmtime(&curTime);
        /* 抹除时区时间差后的 UTC(0) 时间戳 ， 这个抹除的时间差就是时区时间差。
          维基百科记录了，0时区以东的时区用正数，0时区以西的时区用负数；
          此处是在抹除时区差 */
        int8_t tz = (curTime - mktime(zeroTzTm)) / (60 * 60);
        return tz;
      }
      
      timespec Date::fixSecByTz (timespec& curTime, const int8_t& curTz, const int8_t& targetTz) {
        if(!tzLegal(curTz) || !tzLegal(targetTz)) {
          throw logic_error("入参非法时区");
        }

        time_t offsetSecond = (targetTz - curTz) * 60 * 60;
        timespec targetTime = curTime;
        targetTime.tv_sec = curTime.tv_sec + offsetSecond;

        return targetTime;
      }

      timespec Date::toTimespec(const string& str, const string& format) {
        if(defFormatStr.compare(format) != 0) {
          throw logic_error("未知格式需要关注逻辑细节, 只有 [不限内容.000000000] 格式的数据才能被合理部署");
        }

        timespec result {0, 0} ;
        size_t index = str.find_last_of('.');
        if(index == string::npos) {
          /* empty */
        } else {
          if(index >= str.size() - 1) {
            /* empty */
          } else {
            result.tv_nsec = stol(str.substr(index+2));
          }

          string formatDate = str.substr(0, index) ;
          std::tm tmObj;
          istringstream ss(formatDate);
          ss >> std::get_time(&tmObj, format.c_str()); /* 将 时间字符串 转换为 当前时区对应的秒数 , 还需要根据目标时区进行修正 */
          result.tv_sec = mktime(&tmObj);
          if(result.tv_sec < 0) {
            string errMsg = std::strerror(errno);
            throw logic_error(errMsg);
          }
        }

        return result;
      }

      timespec Date::toTimespec(const time_point& tp) {
        uint64_t nanoSec = tp.time_since_epoch().count();
        return toTimespec(nanoSec);
      }
      
      timespec Date::toTimespec(const uint64_t& xSeconds) {
        //{10'000'000'000'000'000'000ull}; /* 比 纳秒   更精细 , 忽略更精细的部分*/
        //{    10'000'000'000'000'000ull}; /* 比 微秒   更精细 */
        //{        10'000'000'000'000ull}; /* 比 毫秒   更精细 */
        //{            10'000'000'000ull}; /* 比 秒(S) 更精细 */
        //              0'000'000'000      /* 以 秒 为单位需要站 十个 整数位 */  
        
        timespec timeSpec {0, 0};

        string tmpStr = to_string(xSeconds);
        
        if(int stringLen = tmpStr.size(); stringLen <= 10) {
          timeSpec.tv_sec = static_cast<time_t>(xSeconds);
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

        return timeSpec;
      }
      
      string   Date::formatDef(const uint64_t& timeStemp, 
                             const string& format/*  = defFormatStr */,
                             const uint8_t& suffixWidth/*  = 6 */) {

        //{10'000'000'000'000'000'000ull}; /* 比 纳秒   更精细 , 忽略更精细的部分*/
        //{    10'000'000'000'000'000ull}; /* 比 微秒   更精细 */
        //{        10'000'000'000'000ull}; /* 比 毫秒   更精细 */
        //{            10'000'000'000ull}; /* 比 秒(S) 更精细 */
        //              0'000'000'000      /* 以 秒 为单位需要站 十个 整数位 */  

        const timespec& timeSpec = toTimespec(timeStemp);

        return formatDef(timeSpec, format, suffixWidth);
      }
      
      string   Date::formatDef(const timespec& timeT, 
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
            suffix = "." + suffix.substr(0, suffixW);
          }
        }

        return (prefix + suffix);
      }
    /* #endregion */

    /* #region 构造函数 */
      Date::Date(const int8_t& _tz, const time_point& _timePoint):tz(_tz), timePoint(_timePoint) {} ;

    /* #endregion */

    /* #region 转换为指定的秒数 */
      uint64_t Date::toSec(bool withTzInfo) {
        uint64_t result = Date::durationCount<seconds>();
        if(withTzInfo) {
          result += (fixTzSecond());
        }
        return result;
      }
      uint64_t Date::toMilliSec(bool withTzInfo) {
        uint64_t result = Date::durationCount<milliseconds>();
        if(withTzInfo) {
          result += (fixTzSecond() * 1'000);
        }
        return result;
      }
      uint64_t Date::toMicroSec(bool withTzInfo) {
        uint64_t result = Date::durationCount<microseconds>();
        if(withTzInfo) {
          result += (fixTzSecond() * 1'000'000);
        }
        return result;
      }
      uint64_t Date::toNanoSec(bool withTzInfo) {
        uint64_t result = Date::durationCount<nanoseconds>();
        if(withTzInfo) {
          result += (fixTzSecond() * 1'000'000'000);
        }
        return result;
      }
    /* #endregion */

    /* #region 转换为格式化字符串 */
      string Date::formatStr(bool withTzInfo, const string& format /* = defFormatStr */) {
        return formatDef(toNanoSec(withTzInfo), defFormatStr, 0);
      }
      string   Date::formatStr3(bool withTzInfo ) {
        return formatDef(toNanoSec(withTzInfo), defFormatStr, 3);
      }
      string   Date::formatStr6(bool withTzInfo ) {
        return formatDef(toNanoSec(withTzInfo), defFormatStr, 6);
      }
      string   Date::formatStr9(bool withTzInfo ) {
        return formatDef(toNanoSec(withTzInfo), defFormatStr, 9);
      }
    /* #endregion */
    
    /* #region 根据时区计算修正秒数 */
      int64_t Date::fixTzSecond() {
        return (60 * 60 * tz);
      }
    /* #endregion */
  /* #endregion */

  /**********___nameless_carpool::Common::Number___****************************
   ***************************************************************************/
  /* #region class Number */ 
    int Number::randomInt(const int &min /* = 100000 */, const int &max /* = 999999 */) {
      std::random_device rd;
      std::uniform_int_distribution<int> dist(min, max);
      return dist(rd);
    }
  /* #endregion */

  /*************___ nameless_carpool::Common::GetErr 获取异常信息___***********
   ***************************************************************************/
  /* #region  */
    string GetErr::getErr(const std::ios_base::iostate& state) {
      switch(state) {
        case _Ios_Iostate::_S_goodbit           : return "_S_goodbit"         ;         
        case _Ios_Iostate::_S_badbit            : return "_S_badbit"          ;       
        case _Ios_Iostate::_S_eofbit            : return "_S_eofbit"          ;       
        case _Ios_Iostate::_S_failbit           : return "_S_failbit"         ;         
        case _Ios_Iostate::_S_ios_iostate_end   : return "_S_ios_iostate_end" ;                         
        case _Ios_Iostate::_S_ios_iostate_max   : return "_S_ios_iostate_max" ;                         
        case _Ios_Iostate::_S_ios_iostate_min   : return "_S_ios_iostate_min" ;                         
        default                                 : return "unknow_states"      ;
      }
    }
  /* #endregion */
}
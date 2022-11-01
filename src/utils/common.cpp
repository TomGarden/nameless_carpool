

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
  bool RegexVals::isLegal(const String& checkStr, const String& regexStr) {
      std::regex aRegex(regexStr);
      return regex_match(checkStr, aRegex);
    }
  bool RegexVals::isLegalPhone(const String& phoneNumber) {
      return isLegal(phoneNumber, this->phone_zh_CN);
    }

  bool RegexVals::isLegalVC(const String& vertifyCodeStr) {
      return isLegal(vertifyCodeStr, this->verifyCode);
    }
    
    
  /**********___nameless_carpool::Common   文件读取操作___*********************
   ***************************************************************************/
  bool getContent(String& content, const String& srcPath, const String& defSrcFileName, const String& defContent) {
      String defFilePath;
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
  bool getContent(String& content, const String& defSrcFileName, const String& defContent) {
      const String path;
      return getContent(content, path, defSrcFileName, defContent);
    }
  

  /**********___nameless_carpool::Common::Date___******************************
   ***************************************************************************/
  /* #region class Date */
    using namespace std::chrono;
    using time_point = std::chrono::system_clock::time_point;

    /* #region 静态函数 & 静态变量 */
      String Date::defFormatStr = "%Y-%m-%d %H:%M:%S";

      Date Date::newInstance(const String& tzName, const SysClockTimePoint& _timePoint) {
        return Date::newInstance(date::locate_zone(tzName));
      }

      Date Date::newInstance(const TimeZone* _tzPtr, const SysClockTimePoint& _timePoint) {
        return Date(_tzPtr, _timePoint);
      }

      bool Date::tzLegal(const String& tzName) {
        try {
          const TimeZone* timeZone = date::locate_zone(tzName);
          return true;
        } catch (std::exception except) {
          return false;
        }
      }

      timespec Date::toTimespec(const String& str, const String& format) {
        using LocaleTimeNano = date::local_time<std::chrono::nanoseconds>;
        std::istringstream inputSs{str};
        LocaleTimeNano     localeTimeNano;
        inputSs >> date::parse(format, localeTimeNano);

        return toTimespec(localeTimeNano.time_since_epoch().count());
      }

      timespec Date::toTimespec(const SysClockTimePoint& tp) {
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

        String tmpStr = to_string(xSeconds);
        
        if(int stringLen = tmpStr.size(); stringLen <= 10) {
          timeSpec.tv_sec = static_cast<time_t>(xSeconds);
        } else {
          String strSecond = tmpStr.substr(0, 10);
          String strOther; {
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


    /* #endregion */

    /* #region 构造函数 */
      Date::Date(const TimeZone*  _tzPtr, const SysClockTimePoint& _timePoint) : tzPtr(_tzPtr), timePoint(_timePoint) {} ;

    /* #endregion */

    /* #region 转换为指定的秒数 */
      uint64_t Date::toSec() { return toSec(tzPtr) ; }
      uint64_t Date::toSec(const TimeZone*  _tzPtr) {
        uint64_t result = Date::durationCount<seconds>(tzPtr);
        return result;
      }
      uint64_t Date::toMilliSec() { return toMilliSec(tzPtr) ; }
      uint64_t Date::toMilliSec(const TimeZone*  _tzPtr) {
        uint64_t result = Date::durationCount<milliseconds>(tzPtr);
        return result;
      }
      uint64_t Date::toMicroSec() { return toMicroSec(tzPtr) ; }
      uint64_t Date::toMicroSec(const TimeZone*  _tzPtr) {
        uint64_t result = Date::durationCount<microseconds>(tzPtr);
        return result;
      }
      uint64_t Date::toNanoSec() { return toNanoSec(tzPtr) ; }
      uint64_t Date::toNanoSec(const TimeZone*  _tzPtr) {
        uint64_t result = Date::durationCount<nanoseconds>(tzPtr);
        return result;
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
    String GetErr::getErr(const std::ios_base::iostate& state) {
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
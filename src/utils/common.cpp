

#include <optional>
#include <regex>
#include <ctime>
#include <exception>
#include  <iomanip>

#include "src/utils/common.h"
#include "libs/date_3.0.1/include/date/date.h"
#include "linux_os.h"
#include "input_check.h"


namespace nameless_carpool::Common::Config { /* 全局配置 */
  /* 验证码失效时间 */
  const uint64_t VERIFY_CODE_LIFE_TIME_SECOND       = 5 * 60; /* 五分钟 */
  const uint64_t VERIFY_CODE_LIFE_TIME_MILLISECONDS = VERIFY_CODE_LIFE_TIME_SECOND * 1'000;
  const uint64_t VERIFY_CODE_LIFE_TIME_MICROSECONDS = VERIFY_CODE_LIFE_TIME_SECOND * 1'000'000;
  const uint64_t VERIFY_CODE_LIFE_TIME_NANOSECONDS  = VERIFY_CODE_LIFE_TIME_SECOND * 1'000'000'000;
}


namespace nameless_carpool::Common {
  using namespace std;


  /**********___nameless_carpool::Common::Regex  正则___***********************
   ***************************************************************************/
  RegexVals regexVals;

  bool RegexVals::isLegal(const std::string& checkStr, const std::string& regexStr) {
    std::regex aRegex(regexStr);
    return regex_match(checkStr, aRegex);
  }
  bool RegexVals::isLegalPhone(const std::string& phoneNumber) {
    return isLegal(phoneNumber, this->phone_zh_CN);
  }

  bool RegexVals::isLegalVC(const std::string& verifyCodeStr) {
    return isLegal(verifyCodeStr, this->verifyCode);
  }


  /**  @description: nameless_carpool::Common 读取文件内容
   * @param {std::string&} content       最终输出的文件内容
   * @param {std::string&} fileFullPath  文件路径
   * @param {std::string&} defContent    如果读取有问题 content , 会持有这个值
   * @return {*}                    true , 成功;
   */
  bool getContent(std::string& content, const std::string& fileFullPath, const std::string& defContent) {
    if (readStrFromFile(fileFullPath, content)) {
      return true;
    } else {
      content = defContent;
      return false;
    }
  }

  /**
   * @param content         , 读取到的文件内的内容
   * @param srcPath         , 文件父目录
   * @param defSrcFileName  , 文件名称
   * @param defContent      , 如果读取文件内容失败 , 默认文件内容
  */
  bool getContent(std::string& content, const std::string& srcPath, const std::string& defSrcFileName, const std::string& defContent) {
    std::string defFilePath;
    if(srcPath.empty()) {
      if(getCurExeFileDir(defFilePath) ) {
        defFilePath.append("/").append(defSrcFileName);
      } else {
        content = defContent;
        return false;
      }
    }
    
    return getContent(content, defFilePath, defContent);
  }
  
}  // namespace nameless_carpool::Common

namespace nameless_carpool::Common/* date */ {
  using namespace std::chrono;
  using time_point = std::chrono::system_clock::time_point;


  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │   静态函数 & 静态变量
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */


  std::string Date::defFormatStr = "%Y-%m-%d %H:%M:%S";
  
  Date Date::newInstance(const std::string& tzName, const SysClockTimePoint& _timePoint) {
    return Date::newInstance(date::locate_zone(tzName));
  }
  
  Date Date::newInstance(const TimeZone* _tzPtr, const SysClockTimePoint& _timePoint) {
        return Date(_tzPtr, _timePoint);
  }
  
  bool Date::tzLegal(const std::string& tzName) {
        try {
          date::locate_zone(tzName);
          return true;
        } catch (std::exception& except) {
          return false;
        }
  }

  date::local_time<std::chrono::nanoseconds> Date::toLocalTime(const std::string& str, const std::string& format) {
    using LocaleTimeNano = date::local_time<std::chrono::nanoseconds>;
    std::istringstream inputSs{str};
    LocaleTimeNano     localeTimeNano;
    inputSs >> date::parse(format, localeTimeNano);
    return localeTimeNano;
  }

  timespec Date::toTimespec(const std::string& str, const std::string& format) {
    const date::local_time<std::chrono::nanoseconds>& localeTimeNano = toLocalTime(str, format);
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

        std::string tmpStr = to_string(xSeconds);
        
        if(int stringLen = tmpStr.size(); stringLen <= 10) {
          timeSpec.tv_sec = static_cast<time_t>(xSeconds);
        } else {
          std::string strSecond = tmpStr.substr(0, 10);
          std::string strOther; {
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

  int64_t Date::passedNanoseconds(const std::string& formatDate, const std::string& timeZone) {
    using nanoDuration                     = std::chrono::nanoseconds;
    using localNanoTime                    = date::local_time<nanoDuration>;
    const localNanoTime&   localTimeNano   = Common::Date::toLocalTime(formatDate);
    const date::zoned_time zonedTime       = date::zoned_time(timeZone, localTimeNano);
    const nanoDuration&    recordTimePoint = zonedTime.get_sys_time().time_since_epoch(); /* get_sys_time 用于获取 utc0 时区的时间 */
    const nanoDuration&    nowTimePoint    = std::chrono::system_clock::now().time_since_epoch();
    int64_t                offset          = (nowTimePoint - recordTimePoint).count();
    return offset;
  }
  std::optional<int64_t> Date::passedNanoseconds(const std::optional<std::string>& formatDate, const std::optional<std::string>& timeZone) {
    if ((!formatDate.has_value() || formatDate->empty()) &&
        (!timeZone.has_value() || timeZone->empty())) return std::nullopt;
    return passedNanoseconds(formatDate.value(), timeZone.value());
  }

  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │   转换为指定的秒数
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */
  inline uint64_t Date::toSec() { return toSec(tzPtr) ; }
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


}

namespace nameless_carpool::Common/* Number */ {
  /* #region class Number */ 
    int Number::randomInt(const int &min /* = 100000 */, const int &max /* = 999999 */) {
      std::random_device rd;
      std::uniform_int_distribution<int> dist(min, max);
      return dist(rd);
    }
  /* #endregion */
}

namespace nameless_carpool::Common /* GetErr */ {
  std::string GetErr::getErr(const std::ios_base::iostate& state) {
    switch (state) {
      case _Ios_Iostate::_S_goodbit /*         */: return "_S_goodbit";
      case _Ios_Iostate::_S_badbit /*          */: return "_S_badbit";
      case _Ios_Iostate::_S_eofbit /*          */: return "_S_eofbit";
      case _Ios_Iostate::_S_failbit /*         */: return "_S_failbit";
      case _Ios_Iostate::_S_ios_iostate_end /* */: return "_S_ios_iostate_end";
      case _Ios_Iostate::_S_ios_iostate_max /* */: return "_S_ios_iostate_max";
      case _Ios_Iostate::_S_ios_iostate_min /* */: return "_S_ios_iostate_min";
      default /*                               */: return "unknow_states";
    }
  }
}  // namespace nameless_carpool::Common
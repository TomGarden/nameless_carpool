#pragma once

#include "sql_util.h"
#include <boost/format.hpp>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>

#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/document.h"
#include "log_utils.h"
#include "tom_string_utils.h"

namespace nameless_carpool {
  namespace SqlUtil {

    const std::string dbName = "nameless_carpool";

    /*┌─────────────────────────────────────────────────────────────────────────────────────┐
    * │                对 sql 字符串拼装过程中的 字段修饰
    * └─────────────────────────────────────────────────────────────────────────────────────┘ */

    /** @return 'str' */
    std::string apostrophe(const std::string& str) {
      return ("'" + str + "'");
    }

    std::string getDbAndTablename(const std::string& tableName, const std::string& dbName) {
      return backticks(dbName) + "." + backticks(tableName);
    }


    /*┌─────────────────────────────────────────────────────────────────────────────────────┐
    * │                从数据库检索结果中获取值
    * └─────────────────────────────────────────────────────────────────────────────────────┘ */
    std::string valueToStr(const mysqlx::Value& value) {
      using CommonType = mysqlx::common::Value::Type;
      using ValueType  = mysqlx::Value::Type;

      auto throwLogicError = []()->std::string { 
        throw std::logic_error("暂未兼容的类型"); 
        return "";
      };

      
      int type = static_cast<int>(value.getType());;
      switch (type) {
        case ValueType::ARRAY:      return throwLogicError();
        case ValueType::DOCUMENT:   return throwLogicError();
        case ValueType::VNULL:      return throwLogicError();
        
        default: switch (type) {
          case CommonType::VNULL:    return throwLogicError();
          case CommonType::UINT64:   return tom_utils::numToStr(value.get<uint64_t>());
          case CommonType::INT64:    return tom_utils::numToStr(value.get<int64_t>());
          case CommonType::FLOAT:    return tom_utils::numToStr(value.get<float>());
          case CommonType::DOUBLE:   return tom_utils::numToStr(value.get<double>());
          case CommonType::BOOL:     return tom_utils::numToStr(value.get<bool>());
          case CommonType::STRING:   return value.get<std::string>();
          case CommonType::USTRING:  return value.get<mysqlx::string>();
          case CommonType::RAW:      return throwLogicError();
          case CommonType::EXPR:     return throwLogicError();
          case CommonType::JSON:     return throwLogicError();
          
          default:                   return throwLogicError();
        }/* end default switch */
      }/* end switch */
    }

    std::string tableColumn(const std::string& tableName, const std::string& columnName) {
      return backticks(tableName) + "." + backticks(columnName);
    }
    
    std::string strOrDef(const mysqlx::Value& value, const std::string& defStr){
      using CommonType = mysqlx::common::Value::Type;
      if(value.isNull()) {
        return defStr;
      } else switch (value.getType()){
        case CommonType::STRING   : return value.get<std::string>();
        case CommonType::USTRING  : return value.get<mysqlx::string>();
        default                   : return defStr;
      }
    }
    std::string strOrEmpty(const mysqlx::Value& value){
      return strOrDef(value, "");
    }


    std::string rawDateParse(const std::vector<uint8_t>& inByteVector) {

      class Result {
      private:
        uint16_t year        = 0;
        uint8_t  month       = 0;
        uint8_t  day         = 0;
        uint8_t  hour        = 0; 
        uint8_t  minute      = 0;
        uint8_t  second      = 0;
        uint32_t microsecond = 0; /* 微秒 , 1秒 = 1000毫秒 ; 1毫秒 = 1000微秒 */
      public:
        const uint8_t fieldCount = 7;
        std::deque<uint32_t> dequeResult;

      private:
        uint8_t getValue8(const uint32_t inVal) {
          if (inVal > std::numeric_limits<uint8_t>::max()) throw std::logic_error(getHumenReadableInfo());
          return static_cast<uint8_t>(inVal);
        }
        uint16_t getValue16(const uint32_t inVal) {
          if (inVal > std::numeric_limits<uint16_t>::max()) throw std::logic_error(getHumenReadableInfo());
          return static_cast<uint16_t>(inVal);
        }

      public:
        std::string getHumenReadableInfo() {
          std::stringstream strStream;
          for (int index = 0; index < dequeResult.size(); index++) {
            switch (index) {
              case 0: strStream << dequeResult[index]; break;
              case 1: strStream << "-" << dequeResult[index]; break;
              case 2: strStream << "-" << dequeResult[index]; break;
              case 3: strStream << " " << dequeResult[index]; break;
              case 4: strStream << ":" << dequeResult[index]; break;
              case 5: strStream << ":" << dequeResult[index]; break;
              case 6: strStream << "." << dequeResult[index]; break;
              default : strStream << "~" << dequeResult[index]; break;
            }
          }
          return strStream.str();
        }

        void setYear /*       */ (const uint32_t inVal) { year = getValue16(inVal); }
        void setMonth /*      */ (const uint32_t inVal) { month = getValue8(inVal); }
        void setDay /*        */ (const uint32_t inVal) { day = getValue8(inVal); }
        void setHour /*       */ (const uint32_t inVal) { hour = getValue8(inVal); }
        void setMinute /*     */ (const uint32_t inVal) { minute = getValue8(inVal); }
        void setSecond /*     */ (const uint32_t inVal) { second = getValue8(inVal); }
        void setMicrosecond /**/ (const uint32_t inVal) { microsecond = inVal; }

        std::string dateFormatStr() {
          return boost::str(boost::format("%d-%02d-%02d %02d:%02d:%02d.%d") %
                            static_cast<uint32_t>(year) %
                            static_cast<uint32_t>(month) %
                            static_cast<uint32_t>(day) %
                            static_cast<uint32_t>(hour) %
                            static_cast<uint32_t>(minute) %
                            static_cast<uint32_t>(second) %
                            static_cast<uint32_t>(microsecond));
        }
      } objResult;
      std::deque<uint32_t>& dequeResult = objResult.dequeResult;

      auto funWithNextByte = [](uint8_t inOneByte) -> bool { return (inOneByte & 0b1000'0000) != 0; }; /* true, 下一个字节是当前字节合并使用 */
      auto funGetPayload   = [](uint8_t inOneByte) -> uint8_t { return (inOneByte & 0b0111'1111); };   /* 获取 每个字节的 荷载 */

      bool     withNextByte   = false; /* true , 下一个字节中的数据与当前字节应该组合 */
      uint8_t  leftShiftBits  = 0;     /* 记录应当左移的位数, 因为是小端字节序所以需要它 */
      for (uint8_t oneByte : inByteVector) {
        uint32_t tmp = {0};

        if(withNextByte) {
          tmp = dequeResult.back();
          dequeResult.pop_back();
        } else {
          leftShiftBits  = 0;
        }
        
        withNextByte = funWithNextByte(oneByte);
        uint8_t payload = funGetPayload(oneByte);
        if (leftShiftBits > (32 - 7)) /* 对于我们所获取的信息 , 我们认为 超过 4 字节即为非法 */ {
          throw std::logic_error(boost::str(boost::format("超过预期的范围 : 左移[ %1% ]会生成大于 4 字节的整数") % leftShiftBits));
        }
        tmp |= (payload << leftShiftBits);
        leftShiftBits += 7;

        dequeResult.push_back(tmp);
      }

      if (dequeResult.size() > objResult.fieldCount) {
        logWarning << boost::str(boost::format(
                                    "预期外的队列长度为 : %d \n"
                                    "逐个输出结果为 : %s \n"
                                    "请做进一步逻辑校验 , 避免未知异常发生 \n") %
                                dequeResult.size() %
                                objResult.getHumenReadableInfo());
      } else if (dequeResult.size() < 3) {
        throw std::logic_error(boost::str(boost::format("要解析的日期是非法的 , 需要进一步校验 ; 当前 dequeResult.size()=%1%") % dequeResult.size()));
      }

      for(int index = 0; index < dequeResult.size(); index++) {
        switch(index) {
          case 0: objResult.setYear(dequeResult.at(index)); break;
          case 1: objResult.setMonth(dequeResult.at(index)); break;
          case 2: objResult.setDay(dequeResult.at(index)); break;
          case 3: objResult.setHour(dequeResult.at(index)); break;
          case 4: objResult.setMinute(dequeResult.at(index)); break;
          case 5: objResult.setSecond(dequeResult.at(index)); break;
          case 6: objResult.setMicrosecond(dequeResult.at(index)); break;
        }
      }

      return objResult.dateFormatStr();
    }
    std::string rawDateParse(const mysqlx::bytes& inByteVector) {
      // logDebug << "!!!!> " << "inByteVector.second = " << inByteVector.second << std::endl;
      const mysqlx::byte*  srcBegin         = inByteVector.first;
      const mysqlx::byte*  srcEnd           = inByteVector.first + inByteVector.second;
      const uint8_t*       destinationBegin = reinterpret_cast<const uint8_t*>(srcBegin);
      const uint8_t*       destinationEnd   = reinterpret_cast<const uint8_t*>(srcEnd);
      std::vector<uint8_t> objVector;
      objVector.insert(objVector.begin(), destinationBegin, destinationEnd);
      return rawDateParse(objVector);
    }


    std:: optional<std::string> getOptionalDate(const mysqlx::Value& value) {
      if(value.isNull()) {
        return std::nullopt;
      } else {
        return rawDateParse(value.getRawBytes());
      }
    }

  };  // namespace SqlUtil
}  // namespace nameless_carpool

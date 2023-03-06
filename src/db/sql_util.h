#pragma once

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

    extern const std::string dbName;
  
    /*┌─────────────────────────────────────────────────────────────────────────────────────┐
    * │                方便 vector 操作
    * └─────────────────────────────────────────────────────────────────────────────────────┘ */
    /* 将给定的 vector 们 按照给定顺序首位相接变成一个 vector */
    template <typename T>
    inline std::vector<T> merge(const std::initializer_list<const std::vector<T>> & inFrom) {
      std::vector<T> result;
      for(const std::vector<T>& fromItem :inFrom) {
        result.insert(result.end(), fromItem.begin(), fromItem.end());
      }
      return result;
    }

    /* 将 inFrom 插入 inTarget 并返回 inTarget , 从尾部插入 */
    template <typename T>
    inline std::vector<T> insertBackAll(std::vector<T>& inTarget, const std::vector<T>& inFrom) {
      inTarget.insert(inTarget.end(), inFrom.begin(), inFrom.end());
      return inTarget;
    }

    /* 将 inFrom 插入 inTarget 并返回 inTarget , 从头部插入 */
    template <typename T>
    inline std::vector<T> insertFrontAll(std::vector<T>& inTarget, const std::vector<T>& inFrom) {
      inTarget.insert(inTarget.begin(), inFrom.begin(), inFrom.end());
      return inTarget;
    }
    

    /*┌─────────────────────────────────────────────────────────────────────────────────────┐
    * │                对 sql 字符串拼装过程中的 字段修饰
    * └─────────────────────────────────────────────────────────────────────────────────────┘ */
    /** @return `str` */
    inline std::string backticks(const std::string& str) { return ("`" + str + "`"); }

    /** @return NULL or 'str' */
    inline std::string nullOrApostrophe(const std::string& str) {
      std::string result;
      if (str.empty()) {
        result = "NULL";
      } else {
        result = ("'" + str + "'");
      }
      return result;
    }

    /** @return NULL or 'str' */
    inline std::string nullOrApostrophe(const std::optional<std::string>& optStr) {
      return nullOrApostrophe(optStr.value_or(""));
      std::string result = optStr.value_or("");
      if (result.empty()) {
        result = "NULL";
      } else {
        result = ("'" + result + "'");
      }
      return result;
    }

    inline std::vector<std::string> nullOrApostrophe(const std::vector<std::optional<std::string>>& optVector) {
      std::vector<std::string> result;
      for(const std::optional<std::string>& item : optVector) result.push_back(nullOrApostrophe(item));
      return result;
    }

    /* 针对数字 NULL 或 数字字符串 */
    template <typename _Type>
    inline std::string nullOrApostrophe(const std::optional<_Type>& optObj) {
      static_assert(std::is_arithmetic<_Type>::value == true, "只接受算数类型的 optional 参数");
      if (optObj.has_value()) {
        return "'" + tom_utils::numToStr(optObj.value()) + "'";
      } else {
        return "NULL";
      }
    }

    inline std::string allFieldSql(const std::vector<std::string> inStrVector) {
      if (inStrVector.empty()) return "";

      std::stringstream sqlTmp;
      for (const std::string& tempStr : inStrVector) {
        sqlTmp << " \t  " << SqlUtil::backticks(tempStr) << " ,\n";
      }
      std::string result = sqlTmp.str();
      return result.erase(result.size() - 2, 1);
    }

    /** @return 'str' */
    std::string apostrophe(const std::string& str);

    std::string getDbAndTablename(const std::string& tableName, const std::string& dbName = SqlUtil::dbName);


    /*┌─────────────────────────────────────────────────────────────────────────────────────┐
    * │                从数据库检索结果中获取值
    * └─────────────────────────────────────────────────────────────────────────────────────┘ */
    std::string valueToStr(const mysqlx::Value& value);

    std::string tableColumn(const std::string& tableName, const std::string& columnName);
    
    std::string strOrDef(const mysqlx::Value& value, const std::string& defStr);
    std::string strOrEmpty(const mysqlx::Value& value);


    std::string rawDateParse(const std::vector<uint8_t>& inByteVector) ;
    std::string rawDateParse(const mysqlx::bytes& inByteVector) ;

    /* 模板函数 定义 范围 : https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl*/
    template<typename Type>
    std:: optional<Type> getOptional(const mysqlx::Value& value) {
      if(value.isNull()) {
        return std::nullopt;
      } else {
        return value.get<Type>();
      }
    }
    std:: optional<std::string> getOptionalDate(const mysqlx::Value& value);
    /* 当需要到其他类型的时候在做新增, 这么做的原因 : https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl */
    template std:: optional<std::string> getOptional(const mysqlx::Value& value); 
    template std:: optional<int64_t> getOptional(const mysqlx::Value& value); 

  };  // namespace SqlUtil
}  // namespace nameless_carpool

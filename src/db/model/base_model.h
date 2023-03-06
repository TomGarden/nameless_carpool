#pragma once

#include <algorithm>
#include <boost/format.hpp>
#include <initializer_list>
#include <stdexcept>

#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/result.h"
#include "sql_util.h"


/*

  > BaseMode , BaseModelNames , 的 getColumnXxxVector 获取的值应该是一一对应的

*/


namespace nameless_carpool {
  struct BaseModel;
  struct BaseNames;
}  // namespace nameless_carpool

/* 数据库 数据模型 基类 */
struct nameless_carpool::BaseModel {
  // BaseModel()                      = default;
  // BaseModel(const BaseModel& obj)  = default;
  // BaseModel(const BaseModel&& obj) = default;
  // ~BaseModel()                     = default;

  static inline std::optional<std::string> numOptionToStrOption(const std::optional<std::string>& inStrOptional) { return inStrOptional; }
  template <typename T>
  static inline std::optional<std::string> numOptionToStrOption(const std::optional<T>& inNumOptional) {
    if (!inNumOptional.has_value()) return "";
    return tom_utils::numToStr(inNumOptional.value());
  }
  template <typename T>
  static inline std::vector<std::optional<std::string>> numOptionToStrOption(const std::initializer_list<std::optional<T>>& inNumList) {
    std::vector<std::optional<std::string>> result = {};
    for(auto inNumItr = inNumList.begin(); inNumItr!= inNumList.end(); inNumItr++) {
      result.push_back(numOptionToStrOption(*inNumItr));
    }
    return result;
  }

  /** ANCHOR - @description: 填充对象自身内容 需要每个类 各自去实现它
  virtual const bool inflate(const ModeNames& names, const std::string& name, const mysqlx::Value& value);
  */

  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>> getPrimaryKeyValVector() const = 0;
  /* ANCHOR - 获取非主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>> getUnPrimaryKeyValVector() const = 0;
  
  /*        - 获取主键值 , 只有主键唯一的时候有效 , 主键非唯一的情况下 crash */
  virtual const std::optional<std::string> getPrimaryKeyVal() const {
    const std::vector<std::optional<std::string>>& pkValVector = getPrimaryKeyValVector();
    int size = pkValVector.size();
    if (pkValVector.size() != 1) throw std::logic_error(
        boost::str(boost::format("预期 size 为 1 , 实际为 %1% ") % size));
    return pkValVector[0];
  };
  /*        -> 获取所有列值组成的列表 */
  virtual const std::vector<std::optional<std::string>> getColumnValVector() const {
    return SqlUtil::merge({getPrimaryKeyValVector(), getUnPrimaryKeyValVector()});
  }

  /* 构造插入 sql 子串 : 获取所有成员 '值' , 或者 NULL , 用于 insert 语句 ; 获取的值被单引号包围 */
  inline const std::string insertColunmNamesSql(const std::vector<std::optional<std::string>> inStrVector) const {
    std::stringstream sqlTmp;
    for (const std::optional<std::string>& optionalStr : inStrVector) {
      sqlTmp << " \t \t " << SqlUtil::nullOrApostrophe(optionalStr) << " ,\n";
    }
    std::string result = sqlTmp.str();
    return result.erase(result.size() - 2, 1);
  }
};

/* ANCHOR - 表描述结构体作为内部类定义 , 子类名称限定为 Names */
struct nameless_carpool::BaseNames {

  /* ANCHOR -> 返回表名称字段 */
  virtual const std::string getTableName() const = 0;

  /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
  virtual const std::vector<std::string> getPrimaryKeyNameVector() const = 0;

  /* ANCHOR -> 获取非主键列名称组成的列表 */
  virtual const std::vector<std::string> getUnPrimaryKeyNameVector() const = 0;


    /*        - 获取主键名 , 只有主键唯一的时候有效 , 主键非唯一的情况下 crash */
  virtual const std::string getPrimaryKeyName() const {
    const std::vector<std::string>& pkNameVector = getPrimaryKeyNameVector();
    int size = pkNameVector.size();
    if (pkNameVector.size() != 1) throw std::logic_error(
        boost::str(boost::format("预期 size 为 1 , 实际为 %1% ") % size));
    return pkNameVector[0];
  };

  /*        -> 获取所有列名称组成的列表 , 
     一般来说是 getPrimaryKeyNameVector / getUnPrimaryKeyNameVector 合体 , 
     特殊情况 , 覆写即可 
  */
  virtual inline const std::vector<std::string> getColumnNameVector() const {
    return SqlUtil::merge({getPrimaryKeyNameVector(), getUnPrimaryKeyNameVector()});
  }

  /* allColumnNameStr , 便利函数 , 用于 select 语句选择所成员 ; 意在取代 allFieldSql */
  inline const std::string selectColunmNamesSql(const std::vector<std::string> nameVector) const {
    if (nameVector.empty()) return "";

    std::stringstream sqlTmp;
    for (const std::string& tempStr : nameVector) {
      sqlTmp << " \t  " << SqlUtil::backticks(tempStr) << " ,\n";
    }
    std::string result = sqlTmp.str();
    return result.erase(result.size() - 2, 1);
  }
};
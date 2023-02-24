#pragma once

#include <time.h>
#include <string>
#include <optional> 

#include "base_model.h"
#include "sql_util.h"




namespace nameless_carpool {
  using namespace std;
  //using string = std::string;
  
  struct BaseTime;
  struct BaseTimeNames;
}

struct nameless_carpool::BaseTimeNames : virtual BaseNames {
  using const_string = const std::string;

  // [[deprecated("ANCHOR - 目前看这个变量并非必须, 待到认识到必须时删除此弃用标识")]]   
  // const std::string className        =  "BaseTime"         ; 

  const std::string create_time      =  "create_time"      ;
  const std::string create_time_tz   =  "create_time_tz"   ;
  const std::string update_time      =  "update_time"      ;
  const std::string update_time_tz   =  "update_time_tz"   ;
  const std::string del_time         =  "del_time"         ;
  const std::string del_time_tz      =  "del_time_tz"      ;

  /** 获取所有成员名 */
  const std::string allFieldSql(const std::vector<std::string> inStrVector) const;

  // virtual const std::string getTableName() const override;
  // virtual const std::vector<std::string> getPrimaryKeyNameVector() const override;



  /*┌──────────────────────────────────────────────────────────────────────────────────────
  * │                以上的内容需要重新考虑
  * └────────────────────────────────────────────────────────────────────────────────────── */

  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
  virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const { return {}; }

  virtual const std::vector<std::string> getUnPrimaryKeyNameVector() const override {
    std::vector<std::string> subUnPrimaryKeyNameVector = getSubUnPrimaryKeyNameVector();
    return SqlUtil::insertBackAll(subUnPrimaryKeyNameVector,
                                  {create_time,
                                   create_time_tz,
                                   update_time,
                                   update_time_tz,
                                   del_time,
                                   del_time_tz});
  }
};

/** 基本时间信息 */
struct nameless_carpool::BaseTime : virtual BaseModel {
  BaseTime();
  BaseTime(const BaseTime& obj);
  BaseTime(const BaseTime&& obj);
  std::optional<std::string>    create_time      ;        /*  */
  std::optional<std::string>    create_time_tz   ;        /*  */
  std::optional<std::string>    update_time      ;        /*  */
  std::optional<std::string>    update_time_tz   ;        /*  */
  std::optional<std::string>    del_time         ;        /*  */
  std::optional<std::string>    del_time_tz      ;        /*  */


  /* 构造插入 sql 子串 : 获取所有成员 '值' , 或者 NULL , 用于 insert 语句 ; 获取的值被单引号包围 */
  const std::string insertAllFieldSql(const std::vector<optional<std::string>> inStrVector) const ;

  /** 把 value 中的值 , 通过名称筛选后 设置到成员变量
   * @param {const ModelNames&} names  表头名称列表 , 用于从 value 中判断当前是那一列
   * @param {string&} name    明确制定了要被填充的对象的列名
   * @param {Value&} value    保存了从数据库中取出的列值
   * @return {*}  true , 填充成功 ; false 填充失败有异常
   */
  const bool inflate(const BaseTimeNames& names, const std::string& name, const mysqlx::Value& value) {
    if /*  */ (name.compare(names.BaseTimeNames::create_time) == 0) {
      this->BaseTime::create_time = SqlUtil::getOptionalDate(value);
    } else if (name.compare(names.BaseTimeNames::create_time_tz) == 0) {
      this->BaseTime::create_time_tz = value.get<std::string>();
    } else if (name.compare(names.BaseTimeNames::update_time) == 0) {
      this->BaseTime::update_time = SqlUtil::getOptionalDate(value);
    } else if (name.compare(names.BaseTimeNames::update_time_tz) == 0) {
      this->BaseTime::update_time_tz = value.get<std::string>();
    } else if (name.compare(names.BaseTimeNames::del_time) == 0) {
      this->BaseTime::del_time = SqlUtil::getOptionalDate(value);
    } else if (name.compare(names.BaseTimeNames::del_time_tz) == 0) {
      this->BaseTime::del_time_tz = SqlUtil::getOptional<std::string>(value);
    } else {
      return false;
    }

    return true;
  }


  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>> getSubUnPrimaryKeyValVector() const { return {}; }

  virtual const std::vector<std::optional<std::string>> getUnPrimaryKeyValVector() const override {
    std::vector<std::optional<std::string>> subUnPrimaryKeyValVector = getSubUnPrimaryKeyValVector();
    return SqlUtil::insertBackAll(subUnPrimaryKeyValVector,
                                  {create_time,
                                   create_time_tz,
                                   update_time,
                                   update_time_tz,
                                   del_time,
                                   del_time_tz});
  }
};

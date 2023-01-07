#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "../model/user_info.h"
#include "db_include.h"

namespace nameless_carpool {

  using string = std::string;
  template <typename T>
  using vector = std::vector<T>;

  class DbManager {
   private:
    Client client = Client(SessionOption::USER, "root",
                           SessionOption::PWD, "root",
                           SessionOption::HOST, "172.17.0.4",
                           SessionOption::PORT, 33060,
                           SessionOption::DB, dbName);

   private:
    DbManager(); /* 默认构造 */

   public:
    DbManager(const DbManager& dbManager) = delete;             /* 拷贝构造 */
    DbManager& operator=(const DbManager& dbManager) = delete;  /* 拷贝赋值 */
    DbManager(const DbManager&& dbManager)           = delete;  /* 移动构造 */
    DbManager& operator=(const DbManager&& dbManager) = delete; /* 移动赋值 */
    //~DbManager();                                                /* 析   构 */

   public:
    static const string dbName;

    static DbManager& getInstance();
    static Client&    getClient();

   public: /* 方便数据库对象操作的函数集 */
    /* NULL or 反引号 `str` */
    static string nullOrBackticks(const string& str);
    /* return `str` */
    static string backticks(const string& str);
    /* NULL or 单引号 'str' */
    static string nullOrApostrophe(const string& str);
    static string nullOrApostrophe(const optional<string>& optStr);
    static string apostrophe(const string& str);
    /* 针对数字 NULL 或 数字字符串 */
    template <typename _Type>
    static string nullOrApostrophe(const optional<_Type>& optObj) {
      static_assert(std::is_arithmetic<_Type>::value == true, "只接受算数类型的 optional 参数");
      if (optObj.has_value()) {
        return "'" + std::to_string(optObj.value()) + "'";
      } else {
        return "NULL";
      }
    }

    static string dateSelectStatements(const string& dateStr);
    static string valueToStr(const Value& value);
    static string getDbAndTablename(const string& tableName, const string& dbName = DbManager::dbName);
    static string tableColumn(const string& tableName, const string& columnName);

    static string strOrDef(const Value& value, const string& defStr);
    static string strOrEmpty(const Value& value);
    template <typename Type>
    static optional<Type> getOptional(const Value& value);

   public:
    SqlResult executeSql(const string& sqlTmp);
    SqlResult executeSql(const stringstream& sqlTmp);

   public:
    string            queryTelephoneSql(const string& telStr);
    vector<Telephone> queryTelephone(const string& telStr);

    string insertTelephoneSql(const vector<Telephone>& telList);
    void   insertTelephone(const vector<Telephone>& telList);
    void   insertTelephone(const Telephone& telObj);

    string updateTelephoneSql(const vector<Telephone>& telList);
    void   updateTelephone(const vector<Telephone>& telList);
    void   updateTelephone(const Telephone& telObj);

    string delTelephoneSql(const vector<uint64_t>& telIds);
    void   delTelephone(const vector<uint64_t>& telIds);
    void   delTelephone(const vector<Telephone>& telList);
    void   delTelephone(const uint64_t& telId);
    void   delTelephone(const Telephone& telObj);

   public:
    string          queryUserTelSql(const uint64_t& telId);
    vector<UserTel> queryUserTel(const uint64_t& telId);
    string          insertUserTelSql(const vector<UserTel> userTelVector);
    void            insertUserTel(const vector<UserTel>& telList);
    void            insertUserTel(const UserTel& telObj);
    string          updateUserTelSql(const vector<UserTel>& telList);
    void            updateUserTel(const vector<UserTel>& telList);
    void            updateUserTel(const UserTel& telObj);
    string          delUserTelSql(const vector<uint64_t>& telIds);
    void            delUserTel(const vector<uint64_t>& telIds);
    void            delUserTel(const vector<UserTel>& telList);
    void            delUserTel(const uint64_t& telId);
    void            delUserTel(const UserTel& telObj);

   public:
    string       queryUserSql(const string& telStr);
    vector<User> queryUser(const string& telStr);
    string       insertUserSql(const vector<User> userVector);
    void         insertUser(const vector<User>& telList);
    void         insertUser(const User& telObj);
    string       updateUserSql(const vector<User>& telList);
    void         updateUser(const vector<User>& telList);
    void         updateUser(const User& telObj);
    string       delUserSql(const vector<uint64_t>& telIds);
    void         delUser(const vector<uint64_t>& telIds);
    void         delUser(const vector<User>& telList);
    void         delUser(const uint64_t& telId);
    void         delUser(const User& telObj);
  };

}  // namespace nameless_carpool
#include <set>
#include <string>
#include <vector>
#include <iterator>
#include <deque>
#include <map>
#include <cstdarg>
#include <utility>

#include "../../utils/log_utils.h"
#include "db_manager.h"
#include "../../utils/common.h"
#include "src/db/model/user_info.h"

namespace nameless_carpool {

  /*************************___私有构造___********************************
   **************************************************************************/
  DbManager::DbManager() = default;

  /*************************___静态函数___********************************
   **************************************************************************/
  const string DbManager::dbName = "nameless_carpool";
  
  DbManager& DbManager::getInstance() {
    static DbManager dbManagerInstance;
    return dbManagerInstance;
  }

  Client& DbManager::getClient()  {
    return getInstance().client;
  }

  /* #region ___静态函数 : 方便数据库对象操作的函数集 ____________________________*/
  using CommonType = mysqlx::common::Value::Type ;
  using ValueType       = mysqlx::Value::Type;
  string DbManager::nullOrBackticks(const string& str){
    string result;
    if(str.empty()) {
      result = "NULL";
    } else {
      result = ("`" + str + "`");
    }
    return result;
  }
  string DbManager::backticks(const string& str) {
    return ("`" + str + "`");
  }
  string DbManager::nullOrApostrophe(const string& str){
    string result;
    if(str.empty()) {
      result = "NULL";
    } else {
      result = ("'" + str + "'");
    }
    return result;
  }
  string DbManager::nullOrApostrophe(const optional<string>& optStr) {
    string result = optStr.value_or("");
    if(result.empty()) {
      result = "NULL";
    } else {
      result = ("'" + result + "'");
    }
    return result;
  }
  string DbManager::apostrophe(const string& str) {
    return ("'" + str + "'");
  }
  
  string DbManager::dateSelectStatements(const string& dateStr) {
    /* DATE_FORMAT(`vc_update_time`,'%Y-%m-%d %H:%i:%S.%f') AS `vc_update_time` */
    return "DATE_FORMAT(" + backticks(dateStr) + ",'%Y-%m-%d %H:%i:%S.%f') AS " + backticks(dateStr);
  }
  string DbManager::valueToStr(const Value& value) {
    auto throwLogicError = []()->string { 
      throw logic_error("暂未兼容的类型"); 
      return "";
    };

    ValueType type = value.getType();
    switch (type) {
      case ValueType::ARRAY:      return throwLogicError();
      case ValueType::DOCUMENT:   return throwLogicError();
      case ValueType::VNULL:      return throwLogicError();
      
      default: switch (type) {
        case CommonType::VNULL:    return throwLogicError();
        case CommonType::UINT64:   return to_string(value.get<uint64_t>());
        case CommonType::INT64:    return to_string(value.get<int64_t>());
        case CommonType::FLOAT:    return to_string(value.get<float>());
        case CommonType::DOUBLE:   return to_string(value.get<double>());
        case CommonType::BOOL:     return to_string(value.get<bool>());
        case CommonType::STRING:   return value.get<std::string>();
        case CommonType::USTRING:  return value.get<mysqlx::string>();
        case CommonType::RAW:      return throwLogicError();
        case CommonType::EXPR:     return throwLogicError();
        case CommonType::JSON:     return throwLogicError();
        
        default:                   return throwLogicError();
      }/* end default switch */
    }/* end switch */
  }
  string DbManager::getDbAndTablename(const string& tableName, const string& dbName) {
    return backticks(dbName) + "." + backticks(tableName);
  }
  string DbManager::tableColumn(const string& tableName, const string& columnName) {
    return backticks(tableName) + "." + backticks(columnName);
  }
  
  string DbManager::strOrDef(const Value& value, const string& defStr){
    if(value.isNull()) {
      return defStr;
    } else switch (value.getType()){
      case CommonType::STRING   : return value.get<std::string>();
      case CommonType::USTRING  : return value.get<mysqlx::string>();
      default                   : return defStr;
    }
  }
  string DbManager::strOrEmpty(const Value& value){
    return strOrDef(value, "");
  }
  
  /* #region 模板函数 定义 范围 : https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl*/
  template<typename Type>
  optional<Type> DbManager::getOptional(const Value& value) {
    if(value.isNull()) {
      return nullopt;
    } else {
      return value.get<Type>();
    }
  }
  /* 当需要到其他类型的时候在做新增, 这么做的原因 : https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl */
  template optional<string> DbManager::getOptional(const Value& value); 
  template optional<int64_t> DbManager::getOptional(const Value& value); 
  /* #endregion */
  /* #endregion */


  /*************************___Sql 封装___********************************
   **************************************************************************/
  SqlResult DbManager::executeSql(const string& sqlTmp) {
    logInfo << sqlTmp << endl;

    /* 分开函数逐个调用 , 不会导致中间对象被回收 */
    Client& client = DbManager::getClient();
    Session session = client.getSession();
    SqlStatement sqlStatement = session.sql(sqlTmp);
    SqlResult sqlResult = sqlStatement.execute();

    // SqlStatement sqlStatement = DbManager::getClient().getSession().sql(sqlTmp);
    // SqlResult sqlResult = sqlStatement.execute();

    stringstream errorSs; {
      vector<Warning> warns = sqlResult.getWarnings();

      for(auto warnIter = warns.begin(); warnIter != warns.end(); warnIter++) {
        Warning& warning = *warnIter;
        errorSs << '(' << warning.getCode() << ')'
                << ':' << warning.getMessage() << endl;
        switch(warning.getLevel()){
          case Warning::Level::LEVEL_INFO: {
            logInfo << "LEVEL_INFO" << errorSs.str();
            break;
          }
          case Warning::Level::LEVEL_WARNING: {
            logWarning << "LEVEL_WARNING" << errorSs.str();
            break;
          }
          case Warning::Level::LEVEL_ERROR: {
            logError << "LEVEL_ERROR" << errorSs.str();
            logDebug << "LEVEL_ERROR" << errorSs.str();
            break;
          }
        }/* switch end */
      }/* for end */
    }/* errorSs 操作完毕 */

    return sqlResult;
  } 
  SqlResult DbManager::executeSql(const stringstream& sqlTmp) {
    string sqlStr = sqlTmp.str();

    SqlResult sqlResult = executeSql(sqlStr);

    return sqlResult;
  }

  /*************************___操作电话表-Telephone-___********************************
   **************************************************************************/
  string DbManager::queryTelephoneSql(const string& telStr) {
    auto& objNames = telephoneNames;
    stringstream sqlTmp;
    sqlTmp << " SELECT "
           << objNames.queryAllFieldSql()
           << " FROM   " << getDbAndTablename(objNames.tableName) << " "
           << " WHERE  " << tableColumn(objNames.tableName, objNames.number) << " = " << apostrophe(telStr) << " ; ";
    return sqlTmp.str();
  }
  vector<Telephone> DbManager::queryTelephone(const string& telStr) {
    const string& sqlStr = queryTelephoneSql(telStr);

    SqlResult sqlResult = executeSql(sqlStr);

    vector<Telephone> result;

    if (!sqlResult.hasData()) {
      return result;
    }

    return Telephone::getTelVector(sqlResult);
  }

  string DbManager::insertTelephoneSql(const vector<Telephone>& telList){
    stringstream sqlTmp; {
      sqlTmp
        << " INSERT LOW_PRIORITY INTO " << getDbAndTablename(telephoneNames.tableName) <<" ( \n"
        << telephoneNames.insertAllFieldSql()        << " )\n"
        << " VALUES                         \n";


      for(auto telIter = telList.cbegin(); telIter != telList.cend(); telIter++) {
        Telephone tel = *telIter;
        sqlTmp << " \t(" << tel.insertAllFieldSql() << " \t) "; 

        if(telIter+1 == telList.cend()) {
          sqlTmp << ';';
        } else {
          sqlTmp << ",\n";
        }
      }/* for 循环结束 */
    }/* sqlTmp 初始化结束 */
    return sqlTmp.str();
  }
  void DbManager::insertTelephone(const vector<Telephone>& telList) {
    if(telList.empty()) {
      return;
    }

    const string& sqlTmp = insertTelephoneSql(telList);
    SqlResult sqlResult = executeSql(sqlTmp);

    return;
  }
  void DbManager::insertTelephone(const Telephone& telObj) {
    insertTelephone(vector<Telephone>{telObj});
  }

  string DbManager::updateTelephoneSql(const vector<Telephone>& telList) {
  
    string updatePrefix = " UPDATE " + getDbAndTablename(telephoneNames.tableName) + " \n SET \n";

    /* 预计存储 
        `字段名`                   = CASE `id`                                                            
                                  WHEN '1' THEN '15111111111'                                                            
                                  WHEN '2' THEN '15122222222'
     */
    using Pair = pair<const string, stringstream>;
    Pair pair01(backticks(telephoneNames.number                       )   , "");
    Pair pair02(backticks(telephoneNames.vertify_code                 )   , "");
    Pair pair03(backticks(telephoneNames.vc_update_time               )   , "");
    Pair pair04(backticks(telephoneNames.vc_update_time_tz            )   , "");
    Pair pair05(backticks(telephoneNames.BaseTimeNames::create_time   )   , "");
    Pair pair06(backticks(telephoneNames.BaseTimeNames::create_time_tz)   , "");
    Pair pair07(backticks(telephoneNames.BaseTimeNames::update_time   )   , "");
    Pair pair08(backticks(telephoneNames.BaseTimeNames::update_time_tz)   , "");
    Pair pair09(backticks(telephoneNames.BaseTimeNames::del_time      )   , "");
    Pair pair10(backticks(telephoneNames.BaseTimeNames::del_time_tz   )   , "");
    vector< pair<const string, stringstream>* > fieldPairVector { 
      &pair01,
      &pair02,
      &pair03,
      &pair04,
      &pair05,
      &pair06,
      &pair07,
      &pair08,
      &pair09,
      &pair10,
    };
    
    /* sql 后半段 */
    string updateSuffix = "\n WHERE " + backticks(telephoneNames.id) + " IN ();";

    /* 遍历所有要更新的 手机号 , 用这些数据填充 fieldPairVector*/
    for(auto telIter = telList.cbegin(); telIter != telList.cend(); telIter++) {
      
      /* telephone 的 所有变量 按顺序 放到 队列中 */
      vector<string> fieldValVector {
        nullOrApostrophe(telIter->number)                          ,
        nullOrApostrophe(telIter->vertify_code)                    ,
        nullOrApostrophe(telIter->vc_update_time)                  ,
        nullOrApostrophe(telIter->vc_update_time_tz)               ,

        nullOrApostrophe(telIter->BaseTime::create_time)           ,
        nullOrApostrophe(telIter->BaseTime::create_time_tz)        ,
        nullOrApostrophe(telIter->BaseTime::update_time)           ,
        nullOrApostrophe(telIter->BaseTime::update_time_tz)        ,
        nullOrApostrophe(telIter->BaseTime::del_time)              ,
        nullOrApostrophe(telIter->BaseTime::del_time_tz)           
      };

      if(fieldPairVector.size() != fieldValVector.size()) {
        throw logic_error("逻辑错误 , 需要跟进");
      }

      vector<pair<const string, stringstream>*>::iterator  fieldPair = fieldPairVector.begin();
      vector<string>::const_iterator                       fieldVal  = fieldValVector.cbegin();

      while(fieldPair != fieldPairVector.end() && fieldVal != fieldValVector.cend()) {

        const string& first  = (**fieldPair).first;
        stringstream& second = (**fieldPair).second;
        int outputPosition = second.tellg(); /* 输出到 stringstream 中的当前位置索引 */
        if(outputPosition == -1) {
          string errState = Common::GetErr::getErr(second.rdstate());
          throw runtime_error("字符流状态异常:" + errState);
        }
        if(outputPosition <= 0) {
          second << "\n\t" << first << " = CASE " << backticks(telephoneNames.id) << "\n";
        }
          
        second << "\t\t\t WHEN " << nullOrApostrophe(telIter->id) << " THEN " << *fieldVal << '\n';

        if(telIter+1 == telList.cend()){
          second << "\t\t\t END ";

          if(fieldPair+1 != fieldPairVector.end() && fieldVal+1 != fieldValVector.cend()) {
            second << " , ";
          }
        }
        
        ++fieldPair;
        ++fieldVal;
      }/* while end 为 每条 set 语句 添加一种情况*/
    
      updateSuffix.insert(updateSuffix.size() - 2, nullOrApostrophe(telIter->id));
      if(telIter+1 != telList.end()) {
        updateSuffix.insert(updateSuffix.size() - 2, ", ");
      }

    }/* for end */

    stringstream sqlTmp; {
      sqlTmp << updatePrefix;
      for(auto fieldPair = fieldPairVector.cbegin(); fieldPair != fieldPairVector.cend(); fieldPair++) {
        sqlTmp << (**fieldPair).second.str();
      }
      sqlTmp << updateSuffix;
    }
  
    return sqlTmp.str();
  }
  void DbManager::updateTelephone(const vector<Telephone>& telList) {

    if(telList.empty()) {
      return;
    }

    const string& sqlTmp = updateTelephoneSql(telList);
    SqlResult sqlResult = executeSql(sqlTmp);

    return;
  }
  void DbManager::updateTelephone(const Telephone& telObj) {
    updateTelephone(vector<Telephone>{telObj});
  }

  string DbManager::delTelephoneSql(const vector<uint64_t>& telIds) {
    stringstream sqlTmp; {
      sqlTmp << " DELETE FROM " << getDbAndTablename(telephoneNames.tableName) << " "
             << " WHERE ID IN (";
      
      for(auto telIdItr = telIds.cbegin(); telIdItr != telIds.cend(); telIdItr++) {
        sqlTmp << *telIdItr;
        if(telIdItr+1 != telIds.cend()) {
          sqlTmp << ", ";
        }
      }

      sqlTmp << " ) ; ";
    }

    return sqlTmp.str();
  }
  void DbManager::delTelephone(const vector<uint64_t>& telIds) {
    const string& sqlTmp = delTelephoneSql(telIds);
    executeSql(sqlTmp);
  }
  void DbManager::delTelephone(const vector<Telephone>& telList) {
    vector<uint64_t> telIds;
    auto lambdaFun = [](const Telephone& tel) -> const uint64_t {  return tel.id.value();  };
    transform(telList.cbegin(), telList.cend(), telIds.begin(), lambdaFun);
    delTelephone(telIds);
  }
  void DbManager::delTelephone(const uint64_t& telId) {
    delTelephone(vector<uint64_t>{telId});
  }
  void DbManager::delTelephone(const Telephone& telObj) {
    delTelephone(vector<uint64_t>{telObj.id.value()});
  }

  /*************************___UserTel___********************************
   **************************************************************************/
  string          DbManager::queryUserTelSql(const uint64_t& telId) {
    auto& objNames = userTelNames;
    stringstream sqlTmp;
    sqlTmp << " SELECT "
           << objNames.queryAllFieldSql()
           << " FROM   " << getDbAndTablename(objNames.tableName) <<" "
           << " WHERE  " << tableColumn(objNames.tableName, objNames.number) << " = " << apostrophe(std::to_string(telId)) << " ; ";
    return sqlTmp.str();
  }
  vector<UserTel> DbManager::queryUserTel(const uint64_t& telId) {
    const string& sqlStr = queryUserTelSql(telId);

    SqlResult sqlResult = executeSql(sqlStr);

    vector<UserTel> result;

    if (!sqlResult.hasData()) {
      return result;
    }

    return UserTel::getUserTelVector(sqlResult);
  }
  string          DbManager::insertUserTelSql(const vector<UserTel> userTelVector) {}
  void            DbManager::insertUserTel(const vector<UserTel>& telList) {}
  void            DbManager::insertUserTel(const UserTel& telObj) {}
  string          DbManager::updateUserTelSql(const vector<UserTel>& telList) {}
  void            DbManager::updateUserTel(const vector<UserTel>& telList) {}
  void            DbManager::updateUserTel(const UserTel& telObj) {}
  string          DbManager::delUserTelSql(const vector<uint64_t>& telIds) {}
  void            DbManager::delUserTel(const vector<uint64_t>& telIds) {}
  void            DbManager::delUserTel(const vector<UserTel>& telList) {}
  void            DbManager::delUserTel(const uint64_t& telId) {}
  void            DbManager::delUserTel(const UserTel& telObj) {}

  /*************************___User___********************************
   **************************************************************************/
  string       DbManager::queryUserSql(const string& telStr) {}
  vector<User> DbManager::queryUser(const string& telStr) {}
  string       DbManager::insertUserSql(const vector<User> userVector) {}
  void         DbManager::insertUser(const vector<User>& telList) {}
  void         DbManager::insertUser(const User& telObj) {}
  string       DbManager::updateUserSql(const vector<User>& telList) {}
  void         DbManager::updateUser(const vector<User>& telList) {}
  void         DbManager::updateUser(const User& telObj) {}
  string       DbManager::delUserSql(const vector<uint64_t>& telIds) {}
  void         DbManager::delUser(const vector<uint64_t>& telIds) {}
  void         DbManager::delUser(const vector<User>& telList) {}
  void         DbManager::delUser(const uint64_t& telId) {}
  void         DbManager::delUser(const User& telObj) {}
}
#include "db_manager.h"

#include <bitset>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format/format_fwd.hpp>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <boost/format.hpp>
#include <thread>

#include "common.h"
#include "container.h"
#include "log_utils.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/common.h"
#include "user_info.h"

namespace nameless_carpool {

  /*************************___私有构造___********************************
   **************************************************************************/

  //┌─────────────────────────────────────────────────────────────────────────────────────┐*/
  //│              私有构造
  //└─────────────────────────────────────────────────────────────────────────────────────┘ */
  DbManager::DbManager() = default;
  //┌─────────────────────────────────────────────────────────────────────────────────────┐
  //│              静态函数
  //└─────────────────────────────────────────────────────────────────────────────────────┘
  DbManager& DbManager::getInstance() {
    static DbManager dbManagerInstance;
    return dbManagerInstance;
  }

  mysqlx::Client& DbManager::getClient()  {
    return getInstance().client;
  }


  //┌─────────────────────────────────────────────────────────────────────────────────────┐
  //│              Sql 封装
  //└─────────────────────────────────────────────────────────────────────────────────────┘
  std::string DbManager::getSqlResultWarning(mysqlx::SqlResult* sqlResult) {
    std::stringstream errorSs;
    {
      std::vector<mysqlx::Warning> warns = sqlResult->getWarnings();

      for (auto warnIter = warns.begin(); warnIter != warns.end(); warnIter++) {
        mysqlx::Warning& warning = *warnIter;

        std::string level;
        switch (warning.getLevel()) {
          case mysqlx::Warning::Level::LEVEL_INFO: level = "LEVEL_INFO"; break;
          case mysqlx::Warning::Level::LEVEL_WARNING: level = "LEVEL_WARNING"; break;
          case mysqlx::Warning::Level::LEVEL_ERROR: level = "LEVEL_ERROR"; break;
          default: level = "unknow"; break;
        } /* switch end */

        const std::string& mysqlWarning = boost::str(boost::format("[%1%](%2%):%3%\n") % level % warning.getCode() % warning.getMessage());
        errorSs << mysqlWarning;

      } /* switch end */
    }   /* for end */

    return errorSs.str();
  }

  mysqlx::SqlResult DbManager::executeSql(const std::string& sqlTmp) {
    logInfoTrace << sqlTmp << std::endl;

    /* 分开函数逐个调用 , 不会导致中间对象被回收 */
    mysqlx::Client&      client       = DbManager::getClient();
    mysqlx::Session      session      = client.getSession(); /* 目前的逻辑每一个 sql 语句的执行都会创建一个会话 */
    mysqlx::SqlStatement sqlStatement = session.sql(sqlTmp);
    mysqlx::SqlResult    sqlResult    = sqlStatement.execute();
    session.close();

    const std::string& sqlResultWarning = getSqlResultWarning(&sqlResult);
    if (!sqlResultWarning.empty()) logWarning << sqlResultWarning << std::endl;

    return sqlResult;
  }
  mysqlx::SqlResult DbManager::executeSql(const std::stringstream& sqlTmp) { return executeSql(sqlTmp.str()); }
  mysqlx::SqlResult DbManager::executeTransactionSql(const std::vector<std::string>& sqlVector) {
    /* 分开函数逐个调用 , 不会导致中间对象被回收 */
    mysqlx::Client&      client       = DbManager::getClient();
    mysqlx::Session      session      = client.getSession(); /* 目前的逻辑每一个 sql 语句的执行都会创建一个会话 */
    mysqlx::SqlResult    sqlResult;
    try {
      logInfo << "session.startTransaction();***********************************" << std::endl;
      session.startTransaction();
      for (const std::string& sqlItem : sqlVector) {
        if (!sqlItem.empty()) {
          logInfo << sqlItem << std::endl;
          sqlResult                           = session.sql(sqlItem).execute();
          const std::string& sqlResultWarning = getSqlResultWarning(&sqlResult);
          if (!sqlResultWarning.empty()) logWarning << sqlResultWarning << std::endl;
        } else {
          logInfo << " jump one empty sql " << std::endl;
        }
      }
      session.commit();
      logInfo << "session.commit();*********************************************" << std::endl;
    } catch (mysqlx::Error& mysqlError) {
      logError << mysqlError.what() << std::endl;
      session.rollback();
    }
    session.close();
    return sqlResult;
  }


  //┌─────────────────────────────────────────────────────────────────────────────────────┐
  //│                测试函数
  //└─────────────────────────────────────────────────────────────────────────────────────┘
  void DbManager::testSqlResult(mysqlx::SqlResult& sqlResult)  {
    // bool 	hasData () const  // Tell if the current result contains row data. More...

    logError << boost::format("hasData() = %b \n") % sqlResult.hasData();
 
    // bool 	nextResult ()  // Move to the next result, if there is one. More...
 
    // uint64_t 	getAutoIncrementValue ()  // Get the auto-increment value if one was generated by a table insert statement.
    logError << boost::format("getAutoIncrementValue() = %d \n") % sqlResult.getAutoIncrementValue();
 
    // col_count_t 	getColumnCount () const  // Return the number of fields in each row.
    // logError << boost::format("getColumnCount() = %d \n") % sqlResult.getColumnCount();
 
    // const Column & 	getColumn (col_count_t pos) const  // Return Column object describing the given column of the result.
 
    // const Columns & 	getColumns () const  // Return meta-data for all result columns. More...
 
    // Row 	fetchOne ()  // Return the current row and move to the next one in the sequence. More...
 
    // RowList 	fetchAll ()  // Return all remaining rows. More...
 
    // row_count_t 	count ()  // Returns the number of rows contained in the result. More...
    // logError << boost::format("count() = %d \n") % sqlResult.count();
 
    // unsigned 	getWarningsCount () const  // Get the number of warnings stored in the result.
 
    // WarningList 	getWarnings ()  // Get a list of warnings stored in the result.
 
    // Warning 	getWarning (unsigned pos)  // Get the warning at the given, 0-based position.
 
    // uint64_t 	getAffectedItemsCount () const  // Get the count of affected items (rows or doucuments) from manipulation statements.
    logError << boost::format("getAffectedItemsCount() = %d \n") % sqlResult.getAffectedItemsCount();
  }

  void DbManager::testMultipleThread() {

    /* 分开函数逐个调用 , 不会导致中间对象被回收 */
    mysqlx::Client& client = DbManager::getClient();
    /* 目前的逻辑每一个 sql 语句的执行都会创建一个会话 */
    mysqlx::Session session = client.getSession();


    auto oneCall = [&](const std::string flag, const bool toSleepOne, const bool toSleepTwo) {
      
      if (toSleepOne) {
        logInfo << boost::format("%1% >> thread [%2%] sleep start") % flag % std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(6000ms);
        logInfo << boost::format("%1% >> thread [%2%] sleep end") % flag % std::this_thread::get_id() << std::endl;
      }


      std::string sqlStr =
          // "BEGIN;"
          "INSERT INTO `nameless_carpool`.`user` ( id_card_num, `name`, gender, birth_date, birth_date_tz, create_time, create_time_tz, update_time, update_time_tz)"
          "VALUES('id_card_name', 'name', '0', NULL, Null , '2023-02-27 06:52:24', 'create_time_tz', '2023-02-27 06:52:24', 'update_time_tz'),"
          "			('id_card_name', 'name', '0', NULL, Null , '2023-02-27 06:52:24', 'create_time_tz', '2023-02-27 06:52:24', 'update_time_tz'),"
          "			('id_card_name', 'name', '0', NULL, Null , '2023-02-27 06:52:24', 'create_time_tz', '2023-02-27 06:52:24', 'update_time_tz');"
          // "SELECT LAST_INSERT_ID();"
          // "COMMIT;"
          ;


      // mysqlx::SqlResult sqlResult = executeSql(sqlStr);
      mysqlx::SqlStatement sqlStatement       = session.sql(sqlStr);
      mysqlx::SqlResult    sqlResult          = sqlStatement.execute();
      std::string autoIncrementValue = boost::str(boost::format("%1% >> threadId = %2% ; AutoIncrementValue = %3%") % flag % std::this_thread::get_id() % sqlResult.getAutoIncrementValue());
      logInfo << autoIncrementValue << std::endl;


      if (toSleepTwo) {
        logInfo << boost::format("%1% >> thread [%2%] sleep start") % flag % std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(6000ms);
        logInfo << boost::format("%1% >> thread [%2%] sleep end") % flag % std::this_thread::get_id() << std::endl;
      }


      std::string sqlQuery = "SELECT LAST_INSERT_ID()";
      // mysqlx::SqlResult sqlQueryResult = executeSql(sqlQuery);
      mysqlx::SqlStatement sqlStatement2  = session.sql(sqlQuery);
      mysqlx::SqlResult    sqlQueryResult = sqlStatement2.execute();
      if(!sqlQueryResult.hasData()) {
        logInfo << boost::format("%1% >> has't data") % flag << std::endl;
        return;
      }
      auto rawCount = sqlQueryResult.count();
      mysqlx::Row row = *sqlQueryResult.fetchAll().begin();
      uint64_t lastInsertId = row[0].get<uint64_t>();
      std::string lastInsertIdStr = boost::str(boost::format("%1% >> threadId = %2% ; lastInsertId = %3% ; rawCount = %4%") % flag % std::this_thread::get_id() % lastInsertId % rawCount);
      logInfo << lastInsertIdStr << std::endl;
    };

    // std::thread threadOne = std::thread(oneCall, "one", false , true);
    std::thread threadTwo = std::thread(oneCall, "two", false , false);

    // threadOne.detach();
    threadTwo.detach();

    std::this_thread::sleep_for(60000ms);
  }

  //┌─────────────────────────────────────────────────────────────────────────────────────┐
  //│                other insert opt 手写的 sql 语句
  //└─────────────────────────────────────────────────────────────────────────────────────┘
  void DbManager::insert(const Telephone& telephone, User& user, const UserTel& userTel) {
    int index = Containter::indexOf(userTelNames.getColumnNameVector(), userTelNames.user_id);
    if (index < 0) throw std::logic_error("BaseMode , BaseModelNames , 的 getColumnXxxVector 获取的值应该是一一对应的 ; 需要进一步校验此处逻辑");

    const std::string insertUserSql    = insertModelSql<User>({user});
    const std::string lastInsertUserId = "@last_insert_user_id";
    std::string       insertUserTelSql;  {
      std::vector<std::string> columnValVector = SqlUtil::nullOrApostrophe(userTel.getColumnValVector());
      columnValVector[index] = lastInsertUserId;
      std::stringstream sqlTmp;
      sqlTmp /* INSERT INTO user_tel */
          << " INSERT INTO " << SqlUtil::getDbAndTablename(userTelNames.tableName) << " ( \n"
          << SqlUtil::allFieldSql(userTelNames.getColumnNameVector()) << " )\n"
          << " VALUES \n"
          << " \t(\n" << userTel.BaseTime::insertAllFieldSqlWithoutApostrophe(columnValVector) << " \t) "
          << ';';
      insertUserTelSql = sqlTmp.str();
    }

    mysqlx::SqlResult sqlResult = executeTransactionSql(
        {insertUserSql,
         boost::str(boost::format(" SET %1% = (SELECT LAST_INSERT_ID());") % lastInsertUserId),
         insertUserTelSql,
         boost::str(boost::format(" SELECT %1% ;") % lastInsertUserId)});

    user.id = sqlResult.getAutoIncrementValue();
  }

  void DbManager::insert(Session& session, UserSession& userSession) {

    int index = Containter::indexOf(userSessionNames.getColumnNameVector(), userSessionNames.session_id);
    if (index < 0) throw std::logic_error("BaseMode , BaseModelNames , 的 getColumnXxxVector 获取的值应该是一一对应的 ; 需要进一步校验此处逻辑");


    const std::string insertSessionSql    = insertModelSql<Session>({session});
    const std::string lastInsertSessionId = "@last_insert_session_id";
    std::string       insertUserSessionSql;  {
      const auto& modelName = userSessionNames;
      const auto& model = userSession;
      std::vector<std::string> columnValVector = SqlUtil::nullOrApostrophe(model.getColumnValVector());
      columnValVector[index] = lastInsertSessionId;
      std::stringstream sqlTmp;
      sqlTmp /* INSERT INTO user_session */
          << " INSERT INTO " << SqlUtil::getDbAndTablename(modelName.tableName) << " ( \n"
          << SqlUtil::allFieldSql(modelName.getColumnNameVector()) << " )\n"
          << " VALUES \n"
          << " \t(\n" << model.BaseTime::insertAllFieldSqlWithoutApostrophe(columnValVector) << " \t) "
          << ';';
      insertUserSessionSql = sqlTmp.str();
    }


    mysqlx::SqlResult sqlResult = executeTransactionSql(
        {insertSessionSql,
         boost::str(boost::format("SET %1% = (SELECT LAST_INSERT_ID())") % lastInsertSessionId),
         insertUserSessionSql,
         boost::str(boost::format("SELECT %1% ") % lastInsertSessionId)});

    session.id             = sqlResult.getAutoIncrementValue();
    userSession.session_id = session.id;
  }
}
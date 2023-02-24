#include "db_manager.h"

#include <bitset>
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

#include "common.h"
#include "log_utils.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/common.h"
#include "user_info.h"

namespace nameless_carpool {

  /*************************___私有构造___********************************
   **************************************************************************/
  DbManager::DbManager() = default;

  /*************************___静态函数___********************************
   **************************************************************************/
  
  DbManager& DbManager::getInstance() {
    static DbManager dbManagerInstance;
    return dbManagerInstance;
  }

  Client& DbManager::getClient()  {
    return getInstance().client;
  }

 

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
      std::vector<Warning> warns = sqlResult.getWarnings();

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

 /*┌─────────────────────────────────────────────────────────────────────────────────────┐
 * │                操作电话表-Telephone                                                   │
 * └─────────────────────────────────────────────────────────────────────────────────────┘ */

  const std::string DbManager::whereTelByNum(const std::string telNum, const std::optional<bool> isDel) {
    const Telephone::Names& modelNames = getModelNames<Telephone>();
    std::string result = boost::str(boost::format("%1%=%2%") % modelNames.number % telNum);
    return result.append(andDelFilter<Telephone>(isDel));
  }
}
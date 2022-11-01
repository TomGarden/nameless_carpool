
#include "db_proxy.h"
#include "../utils/log_utils.h"
#include "../db/sql/db_manager.h"
#include "../utils/common.h"

namespace nameless_carpool {

  DbProxy::DbProxy() = default;

  DbProxy& DbProxy::getInstance() {
    static DbProxy dbProxy;
    return dbProxy;
  }

  bool DbProxy::requestVertifyCode(const string& phoneNumber, const string& timeZone,
                                   string& internalMsg, string& externalMsg) {

    const vector<Telephone>& telVector = DbManager::getInstance().queryTelephone(phoneNumber);

    if(telVector.size() > 1) {
      internalMsg = "逻辑意外 , 预期外情况 , 暂时无法处理";
      externalMsg = "服务器错误";
      logDebug << internalMsg << "; " << externalMsg << endl; 
      return false;

    } else if(telVector.size() < 1) {

      Telephone telObj; {
        string timeStemp = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();

        telObj.number = phoneNumber;
        telObj.vertify_code = to_string(Common::Number::randomInt(100000,999999));
        telObj.vc_update_time = timeStemp;
        telObj.vc_update_time_tz = timeZone;

        telObj.create_time = timeStemp;
        telObj.create_time_tz = timeZone;
        telObj.update_time = timeStemp;
        telObj.update_time_tz = timeZone;
      }

      DbManager::getInstance().insertTelephone(telObj);

    } else /* telVector.size() == 1 */ {

      Telephone curTel = telVector[0];

      bool updateVC; {
        if(!curTel.vc_update_time.has_value() || curTel.vc_update_time->empty()) {
          updateVC = true;
        } else {
          timespec timespecObj = Common::Date::toTimespec(curTel.vc_update_time.value());
          uint64_t now = Common::Date::newInstance(timeZone).toSec();
          uint64_t offsetTime = now - timespecObj.tv_sec;
          if(offsetTime > Common::Config::VERTIFY_CODE_LIFE_TIME) {
            updateVC = true;
          }
        }
      }

      if(updateVC) {
        curTel.vertify_code = to_string(Common::Number::randomInt(100000,999999));
      }
      curTel.update_time    = curTel.vc_update_time    = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();
      curTel.update_time_tz = curTel.vc_update_time_tz = timeZone;
      
      DbManager::getInstance().updateTelephone(curTel);
      
    }/* telVector.zise() 不同情况下的不同处理完成 */

    return true;

  }
}
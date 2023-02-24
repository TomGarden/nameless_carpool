
#include "db_proxy.h"

#include "db_manager.h"
#include "common.h"
#include "log_utils.h"
#include "constant.h"
#include "user_info.h"
#include "http_util.h"
#include "user_info.h"

namespace nameless_carpool {

  DbProxy::DbProxy() = default;

  DbProxy& DbProxy::getInstance() {
    static DbProxy dbProxy;
    return dbProxy;
  }

  HttpStatusEnum DbProxy::requestVertifyCode(const string& phoneNumber, const string& timeZone,
                                             string& internalMsg, string& externalMsg) {
    /* telephone 表内一个手机号可以查到几行 */
    // const vector<Telephone>& telVector = DbManager::getInstance().queryTelephone(phoneNumber);

    DbManager& db = DbManager::getInstance();
    const vector<Telephone>& telVector = db.query<Telephone>(db.whereTelByNum(phoneNumber, false));

    Json telVectorJson = Json(telVector);
    logInfo << telVectorJson.dump(2) << std::endl;

    if (telVector.size() > 1) {
      internalMsg = constantStr.logicException;
      externalMsg = constantStr.serverErr;
      logDebug << internalMsg << "; " << externalMsg << endl;

      return HttpStatusEnum::unknowErr;

    } else if (telVector.size() < 1) {
      Telephone telObj;
      {
        string timeStemp = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();

        telObj.number            = phoneNumber;
        telObj.vertify_code      = to_string(Common::Number::randomInt(100000, 999999));
        telObj.vc_update_time    = timeStemp;
        telObj.vc_update_time_tz = timeZone;

        telObj.create_time    = timeStemp;
        telObj.create_time_tz = timeZone;
        telObj.update_time    = timeStemp;
        telObj.update_time_tz = timeZone;
      }

      // DbManager::getInstance().insertTelephone(telObj);
      db.insert<Telephone>(telObj);

    } else /* telVector.size() == 1 */ {
      Telephone curTel = telVector[0];

      bool updateVC;
      {
        if (!curTel.vc_update_time.has_value() || curTel.vc_update_time->empty()) {
          updateVC = true;
        } else {
          updateVC = curTel.vcIsExpired();/* 验证码过期就意味着需要更新验证码 */
        }
      }

      if (updateVC) {
        curTel.vertify_code = to_string(Common::Number::randomInt(100000, 999999));
      }
      curTel.update_time = curTel.vc_update_time = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();
      curTel.update_time_tz = curTel.vc_update_time_tz = timeZone;

      // DbManager::getInstance().updateTelephone(curTel);
      db.update<Telephone>(curTel);

    } /* telVector.zise() 不同情况下的不同处理完成 */

    return HttpStatusEnum::success;
  }

  bool DbProxy::login(const HttpRequest& requestInput, const Login::RequestBody& body, HttpResponse& responseOutput) {

    DbManager& db = DbManager::getInstance();

    /* 查手机号 , 验证登录信息 */
    // const vector<Telephone>& telVector = DbManager::getInstance().queryTelephone(body.phone.value());
    const vector<Telephone>& telVector = db.query<Telephone>(db.whereTelByNum(body.phone.value(), false));

    /* 异常处理 */ {
      if (telVector.size() > 1) {
        responseOutput.inflateResponse(HttpStatusEnum::unknowErr, constantStr.logicException, constantStr.serverErr);
        return false;

      } else if (telVector.size() < 1) {
        /* 没找到当前手机号 , 手机号未注册 */
        responseOutput.inflateResponse(HttpStatusEnum::badRequest, constantStr.telNotRegister, constantStr.pleaseRetry);
        return false;

      } else /* telVector.size() == 1 */ {
        // 查看验证码是否过期 , 如果没有过期检查验证码是否匹配
        const Telephone& curTel = telVector.at(0);
        if (curTel.vcIsExpired()) { /* 验证码已过期 */
          responseOutput.inflateResponse(HttpStatusEnum::badRequest, constantStr.vertifyCodeExpired, constantStr.pleaseReapplyVc);
          return false;
        }
        if (curTel.vertify_code->compare(body.verify_code.value()) != 0) {
          responseOutput.inflateResponse(HttpStatusEnum::badRequest, constantStr.vertifyCodeErr, constantStr.pleaseRetry);
          return false;
        }

      } /* telVector.zise() 不同情况下的不同处理完成 */
    }

    // responseOutput.inflateResponse(HttpStatusEnum::success, "应该编写生成 token 并返回 token 的代码了");
    const Telephone& tel = telVector.front();

    std::string whereStr; {
      const UserTel::Names& modelNames = getModelNames<UserTel>();
      auto boostFormat = boost::format("%1%=%2% AND %3%=%4%") %modelNames.telephone_id %tel.id.value() %modelNames.flag % userTelFlagConstant.forLogIn;
      whereStr = boost::str(boostFormat);
      whereStr.append(db.andDelFilter<UserTel::Names>(modelNames, false));
    }
    // const vector<UserTel>& userTelVector = DbManager::getInstance().queryUserTel(tel.id.value(), userTelFlagConstant.forLogIn);
    const vector<UserTel>& userTelVector = db.query<UserTel>(whereStr);

    if(userTelVector.size() < 1) {
      // DbManager::getInstance().insertUser()
    } else if (userTelVector.size() > 1) {

    } else /* (userTelVector.size() == 1) */ {
      /* empty */
    }


    /*

    1. 根据手机号 tel id 获取 user id
    2. 根据 user id 查找 user telephone session id
    3. 根据 user telephone session id  获取 telephone_session id 和 session token

    userId_telId_sessionId_token


     */


    return true;
  }
}  // namespace nameless_carpool
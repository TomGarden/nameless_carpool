
#include "db_proxy.h"
#include <boost/algorithm/string/predicate.hpp>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

#include "authenticate.h"
#include "base_model.h"
#include "db_manager.h"
#include "common.h"
#include "log_utils.h"
#include "constant.h"
#include "user_info.h"
#include "http_util.h"
#include "sql_util.h"
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
    const std::string& whereStr = db.where(SqlUtil::backticks(telephoneNames.number), SqlUtil::nullOrApostrophe(phoneNumber), false);
    const vector<Telephone>& telVector = db.query<Telephone>(whereStr);

    Json telVectorJson = Json(telVector);
    logInfo << telVectorJson.dump(2) << std::endl;

    if (telVector.size() > 1) {
      internalMsg = constantStr.logicException;
      externalMsg = constantStr.serverErr;
      logInfo << boost::str(boost::format("tel 数量异常 , 预计有一条数据 , 实际有 %1%") % telVector.size()) << std::endl;

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

      db.insert<Telephone>(telObj);
      return HttpStatusEnum::success;
    }

    Telephone curTel = telVector[0];

    bool updateVC = curTel.vcIsExpired();

    if (updateVC) {
      curTel.vertify_code      = to_string(Common::Number::randomInt(100000, 999999));
      curTel.vc_update_time    = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();
      curTel.vc_update_time_tz = timeZone;
      curTel.update_time       = curTel.vc_update_time;
      curTel.update_time_tz    = curTel.vc_update_time_tz;
      db.update<Telephone>(curTel);
    }

    return HttpStatusEnum::success;
  }

  bool DbProxy::login(const HttpRequest& inRequest, const Login::RequestBody& inBody, HttpResponse& outResponse) {
    auto serverLogicError = [&](const std::string& input) {
      const std::string& errMsg = input;
      logInfo << errMsg << std::endl;
      outResponse.inflateResponse(HttpStatusEnum::serverLogicError, errMsg, constantStr.serverErr);
    };
    // auto serverLogicError = [&](const boost::basic_format<char>& input) {
    //   const std::string& errMsg = boost::str(input);
    //   logInfo << errMsg << std::endl;
    //   outResponse.inflateResponse(HttpStatusEnum::serverLogicError, errMsg, constantStr.serverErr);
    // };

    DbManager& db = DbManager::getInstance();

    /* ① 查手机号 */
    std::shared_ptr<Telephone> telPtr = nullptr; {
      const std::string&       whereTelStr = db.where(SqlUtil::backticks(telephoneNames.number), SqlUtil::nullOrApostrophe(inBody.phone), false);
      const vector<Telephone>& telVector   = db.query<Telephone>(whereTelStr);

      /* 异常处理 */ {
        if (telVector.size() > 1) {
          outResponse.inflateResponse(HttpStatusEnum::unknowErr, WITH_LINE_INFO(constantStr.logicException), constantStr.serverErr);
          return false;

        } else if (telVector.size() < 1) {
          /* 没找到当前手机号 , 手机号未注册 */
          outResponse.inflateResponse(HttpStatusEnum::badRequest, WITH_LINE_INFO(constantStr.telNotRegister), constantStr.pleaseRetry);
          return false;

        } else /* telVector.size() == 1 */ {
          // 查看验证码是否过期 , 如果没有过期检查验证码是否匹配
          telPtr = std::make_shared<Telephone>(telVector[0]);

          if (telPtr->vcIsExpired()) { /* 验证码已过期 */
            outResponse.inflateResponse(HttpStatusEnum::badRequest, WITH_LINE_INFO(constantStr.vertifyCodeExpired), constantStr.pleaseReapplyVc);
            return false;
          }

          if (!telPtr->vertify_code.has_value() || !inBody.verify_code.has_value() ||
              !boost::algorithm::equals(telPtr->vertify_code.value(), inBody.verify_code.value())) {
            outResponse.inflateResponse(HttpStatusEnum::badRequest, WITH_LINE_INFO(constantStr.vertifyCodeErr), constantStr.pleaseRetry);
            return false;
          }

        } /* telVector.zise() 不同情况下的不同处理完成 */
      }   
      /* 异常处理通过 , 即说明没有检查到异常 */
    }

    /* ② 查找手机号对应的用户信息 */
    std::shared_ptr<User> userPtr = nullptr; {
      const std::string& whereUserTelStr = db.whereWithAnd(
          {db.where(SqlUtil::backticks(userTelNames.telephone_id), SqlUtil::nullOrApostrophe(telPtr->id), std::nullopt),
          db.where(SqlUtil::backticks(userTelNames.flag), SqlUtil::nullOrApostrophe(userTelFlagConstant.forLogIn), std::nullopt)},
          false);
      const vector<UserTel>& userTelVector = db.query<UserTel>(whereUserTelStr);

      if(userTelVector.size() < 1) /* 此手机号没有登录过 , 需要插入用户信息和登录信息 */ {
        userPtr = std::make_shared<User>();  {
          Common::Date       commonDate = Common::Date::newInstance();
          const std::string& timeStemp  = commonDate.formatStr<std::chrono::microseconds>();
          const std::string& tzName     = commonDate.getTzName();

          userPtr->create_time    = timeStemp;
          userPtr->create_time_tz = tzName;
          userPtr->update_time    = timeStemp;
          userPtr->update_time_tz = tzName;
        }
        
        UserTel userTel; {
          // userTel.user_id = user.id;
          userTel.telephone_id = telPtr->id;
          userTel.flag = userTelFlagConstant.forLogIn;
          userTel.update_time = userTel.create_time = userPtr->create_time;
          userTel.update_time_tz = userTel.create_time_tz = userPtr->create_time_tz;
        }
        // db.insert<UserTel>(userTel);

        db.insert(*telPtr, *userPtr, userTel);
      } else if (userTelVector.size() > 1) /* 此手机号作为账号有多个登录用户信息 */ {
        serverLogicError(WITH_LINE_INFO(boost::format("逻辑不完整造成的异常 : 此手机号作为账号有多个(%1%个)登录用户信息") % userTelVector.size()));
        return false;
      } else /* (userTelVector.size() == 1) */ {
        // curUserPtr = make_shared<User>(userTelVector[0]);
        const std::string& whereUserStr = db.where(SqlUtil::backticks(userNames.id), SqlUtil::nullOrApostrophe(userTelVector[0].user_id), false);
        std::vector<User> userVector = db.query<User>(whereUserStr);
        if(userVector.size() == 1) {
          userPtr = make_shared<User>(userVector[0]);
        } else {
          serverLogicError(WITH_LINE_INFO(boost::format("预期 user Vector 长度为 1  , 实际长度为 %1%") % userVector.size()));
          return false;
        }
      } 
      
      /* 运行至此 , 说明 : 我们有 一个 userTel , 一个 User */
    }

    /* ③ 用户是否存在登录信息 
    -----> 根据 User 获取 UserSession
    -----> 根据 UserSession 获取 Session
    -----> 判断 Session 是否有效 ; 无效的情况下需要清除 Session 和 UserSession ; 有效的情况直接转发 token 并更新时间
    SELECT *  From session WHERE id in (SELECT UserSession.session_id FROM UserSession Where user_id == xxx)
    */
    std::shared_ptr<Session> sessionPtr = nullptr;  {
      std::string whereSessionSql ; {
        const std::string& whereUserSession       = db.whereWithPack(userSessionNames.user_id, userPtr->id, false);
        std::string        queryUserSessionSubSql = db.queryModelSql({userSessionNames.session_id}, userSessionNames.tableName, whereUserSession);
        auto index = queryUserSessionSubSql.find_last_of(";");
        if(index != std::string::npos) queryUserSessionSubSql = queryUserSessionSubSql.substr(0, index);

        whereSessionSql = boost::str(boost::format("( %1% IN (%2%) )") % sessionNames.id % queryUserSessionSubSql);
        whereSessionSql.append(db.andDelFilter(false));
      }
      std::vector<Session> sessionVector = db.query<Session>(whereSessionSql);

      /* 同类型的设备只能登录一个 , 我们来完成这个逻辑需求  */
      std::vector<Session> expiredSession;               /* 过期 session */
      std::vector<Session*> duringSamePlateformSession;   /* 在有效期内 & 同平台 session */
      // std::vector<Session*> duringUnSamePlateformSession; /* 在有效期内 & 不同平台 session */
      for(Session& sessionItem: sessionVector) {
        if (sessionItem.tokenIsExpired()) expiredSession.push_back(sessionItem);
        if (boost::algorithm::equals(sessionItem.client_type.value_or("null"), inBody.client_type.value_or("null")))
          duringSamePlateformSession.push_back(&sessionItem);
        // else duringUnSamePlateformSession.push_back(&sessionItem);
      }

      if (duringSamePlateformSession.size() > 1) {
        serverLogicError(WITH_LINE_INFO(boost::format("预期 duringSamePlateformSession 长度 ≤ 1  , 实际长度为 : %1%") % duringSamePlateformSession.size()));
        return false;
      } else if (duringSamePlateformSession.size() == 1) {
        /* update token and return */
        sessionPtr = std::make_shared<Session>(*duringSamePlateformSession[0]);
        const std::string& timeZone   = inRequest.headers[httpHeaderNames.timeZone].get<std::string>();
        sessionPtr->resetToken(timeZone);
        db.update(*sessionPtr);
      } else /* if (duringSamePlateformSession.size() < 1) */ {
        /* insert token + userSession and return */
        const std::string& timeZone   = inRequest.headers[httpHeaderNames.timeZone].get<std::string>();
        sessionPtr = std::make_shared<Session>(Session::newObj(timeZone));  {
          sessionPtr->client_type = Session::ClientType().getClientType(inBody.client_type);
        }
        UserSession userSession; {
          userSession.user_id        = userPtr->id;
          userSession.create_time    = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();
          userSession.create_time_tz = timeZone;
          userSession.update_time    = userSession.create_time;
          userSession.update_time_tz = userSession.create_time_tz;
        }
        db.insert(*sessionPtr, userSession);
      }

      /* 删除 过期的 session */ {
        std::vector<std::vector<std::optional<std::string>>> pkOptDoubleVector;
        for (const Session& expiredSessionItem : expiredSession) {
          pkOptDoubleVector.push_back(BaseModel::numOptionToStrOption({userPtr->id, expiredSessionItem.id}));
        }
        const std::string& delUserSessionSql = db.delSqlByPkDoubleVector<UserSession>(pkOptDoubleVector);
        const std::string& delSessionSql = db.delSqlByModelVector(expiredSession);
        db.executeTransactionSql({delUserSessionSql, delSessionSql});
      }
    }
    
    Login::Response response; {
      /* 从数据库数据类型简化为需要的返回值类型由两种方式 , 
         一是通过先序列化后反序列化的方式 
         二是 : https://stackoverflow.com/a/35778400/7707781
      */
      response.user = Json(*userPtr).get<Login::Response::User_>();
      response.session = Json(*sessionPtr).get<Login::Response::Session_>();
    }

    outResponse.inflateResponse(HttpStatusEnum::success).inflateBodyData(response);
    return true;
  }
}  // namespace nameless_carpool
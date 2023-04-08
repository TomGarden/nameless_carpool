
#include "db_proxy.h"

#include <boost/algorithm/string/predicate.hpp>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

#include "src/db/model/base_model.h"
#include "src/db/model/include_db_model.h"
#include "src/db/sql/db_manager.h"
#include "src/db/sql_util.h"
#include "src/net/api/authenticate.h"
#include "src/net/model/authenticate.h"
#include "src/utils/common.h"
#include "src/utils/constant.h"
#include "src/utils/json/include_json.h"
#include "src/utils/log_utils.h"
#include "src/net/http_util.h"
#include "container.h"

namespace nameless_carpool {

  DbProxy& DbProxy::getInstance() {
    static DbProxy dbProxy;
    return dbProxy;
  }

  HttpStatus::Enum DbProxy::requestVerifyCode(
      const std::string& phoneNumber, const std::string& timeZone,
      std::string& internalMsg, std::string& externalMsg) {
    /* telephone 表内一个手机号可以查到几行 */
    // const vector<Telephone>& telVector = DbManager::getInstance().queryTelephone(phoneNumber);

    DbManager& db = DbManager::getInstance();
    const std::string& whereStr = db.where(SqlUtil::backticks(Telephone::names().number), SqlUtil::nullOrApostrophe(phoneNumber), false);
    const vector<Telephone>& telVector = db.query<Telephone>(whereStr);

    nlohmann::json telVectorJson = nlohmann::json(telVector);
    logInfo << telVectorJson.dump(2) << std::endl;

    if (telVector.size() > 1) {
      internalMsg = constantStr.logicException;
      externalMsg = constantStr.serverErr;
      logInfo << boost::str(boost::format("tel 数量异常 , 预计有一条数据 , 实际有 %1%") % telVector.size()) << std::endl;

      return HttpStatus::Enum::unknowErr;

    } else if (telVector.size() < 1) {
      Telephone telObj;
      {
        std::string timeStemp = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();

        telObj.number            = phoneNumber;
        telObj.verify_code      = to_string(Common::Number::randomInt(100000, 999999));
        telObj.vc_update_time    = timeStemp;
        telObj.vc_update_time_tz = timeZone;

        telObj.create_time    = timeStemp;
        telObj.create_time_tz = timeZone;
        telObj.update_time    = timeStemp;
        telObj.update_time_tz = timeZone;
      }

      db.insert<Telephone>(telObj);
      return HttpStatus::Enum::success;
    }

    Telephone curTel = telVector[0];

    bool updateVC = curTel.vcIsExpired();

    if (updateVC) {
      curTel.verify_code      = to_string(Common::Number::randomInt(100000, 999999));
      curTel.vc_update_time    = Common::Date::newInstance(timeZone).formatStr<std::chrono::microseconds>();
      curTel.vc_update_time_tz = timeZone;
      curTel.update_time       = curTel.vc_update_time;
      curTel.update_time_tz    = curTel.vc_update_time_tz;
      db.update<Telephone>(curTel);
    }

    return HttpStatus::Enum::success;
  }

  bool DbProxy::login(const HttpRequest& inRequest, const Login::RequestBody& inBody, HttpResponse& outResponse) {
    auto serverLogicError = [&](const std::string& input) {
      const std::string& errMsg = input;
      logInfo << errMsg << std::endl;
      outResponse.inflateResponse(HttpStatus::Enum::serverLogicError, errMsg, constantStr.serverErr);
    };
    // auto serverLogicError = [&](const boost::basic_format<char>& input) {
    //   const std::string& errMsg = boost::str(input);
    //   logInfo << errMsg << std::endl;
    //   outResponse.inflateResponse(HttpStatus::Enum::serverLogicError, errMsg, constantStr.serverErr);
    // };

    DbManager& db = DbManager::getInstance();

    /* ① 查手机号 */
    std::shared_ptr<Telephone> telPtr = nullptr; {
      const std::string&       whereTelStr = db.where(SqlUtil::backticks(Telephone::names().number), SqlUtil::nullOrApostrophe(inBody.phone), false);
      const vector<Telephone>& telVector   = db.query<Telephone>(whereTelStr);

      /* 异常处理 */ {
        if (telVector.size() > 1) {
          outResponse.inflateResponse(HttpStatus::Enum::unknowErr, WITH_LINE_INFO(constantStr.logicException), constantStr.serverErr);
          return false;

        } else if (telVector.size() < 1) {
          /* 没找到当前手机号 , 手机号未注册 */
          outResponse.inflateResponse(HttpStatus::Enum::badRequest, WITH_LINE_INFO(constantStr.telNotRegister), constantStr.pleaseRetry);
          return false;

        } else /* telVector.size() == 1 */ {
          // 查看验证码是否过期 , 如果没有过期检查验证码是否匹配
          telPtr = std::make_shared<Telephone>(telVector[0]);

          if (telPtr->vcIsExpired()) { /* 验证码已过期 */
            outResponse.inflateResponse(HttpStatus::Enum::badRequest, WITH_LINE_INFO(constantStr.verifyCodeExpired), constantStr.pleaseReapplyVc);
            return false;
          }

          if (!telPtr->verify_code.has_value() || !inBody.verify_code.has_value() ||
              !boost::algorithm::equals(telPtr->verify_code.value(), inBody.verify_code.value())) {
            outResponse.inflateResponse(HttpStatus::Enum::badRequest, WITH_LINE_INFO(constantStr.verifyCodeErr), constantStr.pleaseRetry);
            return false;
          }

        } /* telVector.zise() 不同情况下的不同处理完成 */
      }   
      /* 异常处理通过 , 即说明没有检查到异常 */
    }

    /* ② 查找手机号对应的用户信息 */
    std::shared_ptr<User> userPtr = nullptr; {
      const std::string& whereUserTelStr = db.whereWithAnd(
          {db.where(SqlUtil::backticks(UserTel::names().telephone_id), SqlUtil::nullOrApostrophe(telPtr->id), std::nullopt),
          db.where(SqlUtil::backticks(UserTel::names().flag), SqlUtil::nullOrApostrophe(UserTel::Flag::toString(UserTel::Flag::login)), std::nullopt)},
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
          userTel.flag = UserTel::Flag::toString(UserTel::Flag::login);
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
        const std::string& whereUserStr = db.where(SqlUtil::backticks(User::names().id), SqlUtil::nullOrApostrophe(userTelVector[0].user_id), false);
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
        const std::string& whereUserSession       = db.whereWithPack(UserSession::names().user_id, userPtr->id, false);
        std::string        queryUserSessionSubSql = db.queryModelSql({UserSession::names().session_id}, UserSession::names().tableName, whereUserSession);
        auto index = queryUserSessionSubSql.find_last_of(";");
        if(index != std::string::npos) queryUserSessionSubSql = queryUserSessionSubSql.substr(0, index);

        whereSessionSql = boost::str(boost::format("( %1% IN (%2%) )") % Session::names().id % queryUserSessionSubSql);
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
        const std::string& timeZone   = inRequest.headers[HttpHeaderNames::instance().timeZone].get<std::string>();
        sessionPtr->resetToken(timeZone);
        db.update(*sessionPtr);
      } else /* if (duringSamePlateformSession.size() < 1) */ {
        /* insert token + userSession and return */
        const std::string& timeZone   = inRequest.headers[HttpHeaderNames::instance().timeZone].get<std::string>();
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
      response.user = nlohmann::json(*userPtr).get<Login::Response::User_>();
      response.session = nlohmann::json(*sessionPtr).get<Login::Response::Session_>();
    }

    outResponse.inflateResponse(HttpStatus::Enum::success).inflateBodyData(response);
    return true;
  }

  bool DbProxy::tokenIsLegal(const string& inToken, string& outErrMsg) {
    const std::string& whereSessionStr = db().where(
        SqlUtil::backticks(Session::names().token), SqlUtil::nullOrApostrophe(inToken), false);
    std::vector<Session>            sessionVector = db().query<Session>(whereSessionStr);
    std::vector<Session>::size_type size          = sessionVector.size();

    bool result = false;
    if(size > 1) {
      outErrMsg = boost::str(boost::format("token 校验异常 ; 服务端异常 ; 预期通过 token 只能得到一条 session , 实际得到了 : %1% 条") % size);
      result    = false;
    } else if (size < 1) {
      outErrMsg = "非法 token";
      result    = false;
    } else if (sessionVector[0].tokenIsExpired()) {
      outErrMsg = "token 已过期";
      result    = false;
    } else {
      result = true;
    }

    return result;
  }

  bool DbProxy::tokenIsLegal(const std::string& inToken, std::string& outErrMsg, std::shared_ptr<RequestBasicInfo> outRequestBasicPtr) {
    /* 通过 token 获取 session ; 如果如果合法会跳出逻辑块 */ {
      const std::string& whereSessionStr = db().where(
          SqlUtil::backticks(Session::names().token), SqlUtil::nullOrApostrophe(inToken), false);
      std::vector<Session>            sessionVector = db().query<Session>(whereSessionStr);
      std::vector<Session>::size_type size          = sessionVector.size();


      if (size > 1) {
        outErrMsg = boost::str(boost::format("token 校验异常 ; 服务端异常 ; 预期通过 token 只能得到一条 session , 实际得到了 : %1% 条") % size);
        return false;
      } else if (size < 1) {
        outErrMsg = "非法 token";
        return false;
      } else if (sessionVector[0].tokenIsExpired()) {
        outErrMsg = "token 已过期";
        return false;
      } else {
        outRequestBasicPtr->session = sessionVector[0];
      }
    }

    /* 通过合法的 session 获取 user */ {
      const std::string& queryUserSessionSql = db().queryModelSql(
          {UserSession::names().user_id},
          UserSession::names().tableName,
          db().where(
              SqlUtil::backticks(UserSession::names().session_id),
              SqlUtil::nullOrApostrophe(outRequestBasicPtr->session->id),
              false),
          false);
      std::stringstream strStream;
      strStream << SqlUtil::backticks(User::names().id) << " IN ( " << queryUserSessionSql << " ) " << db().andDelFilter(false);

      std::vector<User> userVector = db().query<User>(strStream.str());
      std::vector<User>::size_type size = userVector.size();

      if (size != 1) {
        outErrMsg = boost::str(boost::format("token 校验异常 ; 服务端异常 ; 预期通过 token 只能得到一条 user , 实际得到了 : %1% 条") % size);
        return false;
      } else {
        outRequestBasicPtr->user = userVector[0];
      }
    }

    return true;
  }

  /**
   * 目前所有的 return 都是 true
   * @param requestInput
   * @param requestBasicPtr
   * @param findCarBody
   * @param outResponse
   * @return
   */
  bool DbProxy::postPeopleFindCar(const HttpRequest&                       requestInput,
                                  const std::shared_ptr<RequestBasicInfo>& requestBasicPtr,
                                  body::FindCarBody&                       findCarBody,
                                  HttpResponse&                            outResponse) {

    nlohmann::json responseBodyData;

    std::vector<std::string> sqlVector;

    auto getLastInsertId = [](mysqlx::Session& session) -> uint64_t {
      mysqlx::SqlResult sqlResult = db().executeSqlUnCloseSession(session, " SELECT LAST_INSERT_ID() ; ");
      debugAssertTrue(sqlResult.count() == 1);
      return sqlResult.fetchOne().get(0).get<uint64_t>();
    };

    auto optGoodsInfo = [&](mysqlx::Session& session) {
      if(findCarBody.goodsInfo->id.has_value()) {
        if(!findCarBody.goodsInfo->isPresetItem()) {
          /* 判断用户名下是否存在该货物 */ {
            const UserGoodsInfo::Names& userGoodsInfoNames = UserGoodsInfo::names();
            const std::string&        queryUserGoodsInfoWhere = boost::str(
                boost::format(" %1% = %2% AND %3% = %4% %5% ") %
                SqlUtil::backticks(userGoodsInfoNames.user_id) % SqlUtil::nullOrApostrophe(requestBasicPtr->user->id) %
                SqlUtil::backticks(userGoodsInfoNames.goods_info_id) % SqlUtil::nullOrApostrophe(findCarBody.goodsInfo->id) %
                db().andDelFilter(false));

            const std::vector<UserGoodsInfo>& userGoodsInfoVector = db().query<UserGoodsInfo>(session, queryUserGoodsInfoWhere, false);
            if (userGoodsInfoVector.empty()) {
              outResponse.inflateResponse(HttpStatus::forbidden, " GoodsInfo id 不在用户名下 ");
              return;
            } else if (userGoodsInfoVector.size() != 1) {
              outResponse.inflateResponse(HttpStatus::serverLogicError, " UserGoodsInfo 个数大于 1 ");
              return;
            }
          }
          /* 更新货物信息 */ {
            findCarBody.goodsInfo->renewUpdateTime(requestInput.getTimeZone());

            const std::string& updateGoodsInfo = db().updateSingleModelSql<GoodsInfo>(findCarBody.goodsInfo.value(), GoodsInfo::names().getIgnoreCDColumn());
            db().executeSqlUnCloseSession(session, updateGoodsInfo);

            findCarBody.findCar->goods_info_id = findCarBody.goodsInfo->id;
          }
        }
      } else /* if(!findCarBody.goodsInfo->id.has_value()) */ {
        /* insert goods info */ {
          findCarBody.goodsInfo->initCreateAndUpdateTime(requestInput.getTimeZone());
          const std::string& insertGoodsInfo = db().insertModelSql<GoodsInfo>({*findCarBody.goodsInfo});
          db().executeSqlUnCloseSession(session, insertGoodsInfo);
          findCarBody.findCar->goods_info_id = findCarBody.goodsInfo->id = getLastInsertId(session);
        }
        /* insert user goods info */ {
          UserGoodsInfo userGoodsInfo;{
            userGoodsInfo.user_id = requestBasicPtr->user->id;
            userGoodsInfo.goods_info_id = findCarBody.goodsInfo->id;
            userGoodsInfo.initCreateAndUpdateTime(requestInput.getTimeZone());
          }
          const std::string& insertUserGoodsInfo = db().insertModelSql<UserGoodsInfo>({userGoodsInfo});
          db().executeSqlUnCloseSession(session,insertUserGoodsInfo);
        }
      }
    };

    if(findCarBody.findCar->id.has_value() ) {
      /*
          if(findCarBody.goodsInfo->id.has_value()) {
            1. 用户应该存在 这个 userFindCar 记录 , 否则可以算作越权
            2. update goodsInfo
            3. update findCar

          } else if(!findCarBody.goodsInfo->id.has_value()) {
            1. 用户应该存在 这个 userFindCar 记录 , 否则可以算作越权
            2. insert goodsInfo
            3. update findCar
          }
       * */
      db().executeTransactionSql([&](mysqlx::Session& session) -> void {
        /* 判断 findCar 是否在用户名 */ {
          const UserFindCar::Names& userFindCarNames      = UserFindCar::names();
          const std::string&        queryUserFindCarWhere = boost::str(
              boost::format(" %1% = %2% AND %3% = %4% %5% ") %
              SqlUtil::backticks(userFindCarNames.user_id) % SqlUtil::nullOrApostrophe(requestBasicPtr->user->id) %
              SqlUtil::backticks(userFindCarNames.find_car_id) % SqlUtil::nullOrApostrophe(findCarBody.findCar->id) %
              db().andDelFilter(false));

          const std::vector<UserFindCar>& userFindCarVector = db().query<UserFindCar>(session, queryUserFindCarWhere, false);
          if (userFindCarVector.empty()) {
            outResponse.inflateResponse(HttpStatus::forbidden, " findCar id 不在用户名下 ");
            return;
          } else if (userFindCarVector.size() != 1) {
            outResponse.inflateResponse(HttpStatus::serverLogicError, " UserFindCar 个数大于 1 ");
            return;
          }
        }
        /* insert or update GoodsInfo */ {
          optGoodsInfo(session);
          if (!outResponse.isEmpty()) return;
        }
        /* update FindCar */ {
          findCarBody.findCar->renewUpdateTime(requestInput.getTimeZone());
          const std::string& insertOrUpdateFindCar = db().updateSingleModelSql<FindCar>(findCarBody.findCar.value(), FindCar::names().getIgnoreCDColumn());
          db().executeSqlUnCloseSession(session, insertOrUpdateFindCar);
        }
      });

    } else /* ( !findCarBody.findCar->id.has_value() ) */ {

      /*
          if (findCarBody.goodsInfo->id.has_value()) {
              1. update GoodsInfo
              2. insert findCar
              3. insert UserFindCar

          } else if (!findCarBody.goodsInfo->id.has_value()) {
              1. insert GoodsInfo
              2. insert findCar
              3. insert UserFindCar
          }
      */
      db().executeTransactionSql([&](mysqlx::Session& session) -> void {
        /* insert or update GoodsInfo */ {
          optGoodsInfo(session);
          if (!outResponse.isEmpty()) return;
        }
        /* insert FindCar */ {
          findCarBody.findCar->initCreateAndUpdateTime(requestInput.getTimeZone());
          const std::string& insertFindCar = db().insertModelSql<FindCar>({*findCarBody.findCar});
          db().executeSqlUnCloseSession(session, insertFindCar);

          findCarBody.findCar->id = getLastInsertId(session);
        }
        /* insert UserFindCar */ {
          UserFindCar userFindCar; {
            userFindCar.user_id = requestBasicPtr->user->id;
            userFindCar.find_car_id = findCarBody.findCar->id;
            userFindCar.initCreateAndUpdateTime(requestInput.getTimeZone());
          }
          const std::string& insertUserFindCar = db().insertModelSql<UserFindCar>({userFindCar});
          db().executeSqlUnCloseSession(session,insertUserFindCar);
        }
      });
    }

    if(!outResponse.isEmpty()) return false;

    responseBodyData[GoodsInfo::names().tableName] = findCarBody.goodsInfo;
    responseBodyData[FindCar::names().tableName]   = findCarBody.findCar;
    outResponse.inflateResponse(HttpStatus::success);
    outResponse.inflateBodyData(responseBodyData);
    return true;

  }

}  // namespace nameless_carpool
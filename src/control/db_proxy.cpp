
#include "db_proxy.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
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

  bool DbProxy::tokenIsLegal(const std::string& inToken, std::string& outErrMsg, RequestBasicInfo& outRequestBasicPtr) {
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
        outRequestBasicPtr.session = sessionVector[0];
      }
    }

    /* 通过合法的 session 获取 user */ {
      const std::string& queryUserSessionSql = db().queryModelSql(
          {UserSession::names().user_id},
          UserSession::names().tableName,
          db().where(
              SqlUtil::backticks(UserSession::names().session_id),
              SqlUtil::nullOrApostrophe(outRequestBasicPtr.session->id),
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
        outRequestBasicPtr.user = userVector[0];
      }
    }

    return true;
  }


  void optGoodsInfo(const HttpRequest&      inRequestInput,
                    const RequestBasicInfo& inRequestBasicPtr,
                    mysqlx::Session&        inSession,
                    GoodsInfo&              goodsInfo,
                    HttpResponse&           outResponse
                    ) {
    if (goodsInfo.id.has_value()) {
      if (!goodsInfo.isPresetItem()) {
        /* 判断用户名下是否存在该货物 */ {
          const UserGoodsInfo::Names& userGoodsInfoNames      = UserGoodsInfo::names();
          const std::string&          queryUserGoodsInfoWhere = boost::str(
              boost::format(" %1% = %2% AND %3% = %4% %5% ") %
              SqlUtil::backticks(userGoodsInfoNames.user_id) % SqlUtil::nullOrApostrophe(inRequestBasicPtr.user->id) %
              SqlUtil::backticks(userGoodsInfoNames.goods_info_id) % SqlUtil::nullOrApostrophe(goodsInfo.id) %
              db().andDelFilter(false));

          const std::vector<UserGoodsInfo>& userGoodsInfoVector = db().query<UserGoodsInfo>(inSession, queryUserGoodsInfoWhere, false);
          if (userGoodsInfoVector.empty()) {
            outResponse.inflateResponse(HttpStatus::forbidden, " GoodsInfo id 不在用户名下 ");
            return;
          } else if (userGoodsInfoVector.size() != 1) {
            outResponse.inflateResponse(HttpStatus::serverLogicError, " UserGoodsInfo 个数大于 1 ");
            return;
          }
        }
        /* 更新货物信息 */ {
          goodsInfo.renewUpdateTime(inRequestInput.getTimeZone());

          const std::string& updateGoodsInfo = db().updateSingleModelSql<GoodsInfo>(goodsInfo, GoodsInfo::names().getIgnoreCDColumn());
          db().executeSqlUnCloseSession(inSession, updateGoodsInfo);
        }
      }
    } else /* if(!findCarBody.goodsInfo->id.has_value()) */ {
      /* insert goods info */ {
        goodsInfo.initCreateAndUpdateTime(inRequestInput.getTimeZone());
        const std::string& insertGoodsInfo = db().insertModelSql<GoodsInfo>({goodsInfo});
        db().executeSqlUnCloseSession(inSession, insertGoodsInfo);
        goodsInfo.id = db().getLastInsertIdNoCloseSession(inSession);
      }
      /* insert user goods info */ {
        UserGoodsInfo userGoodsInfo;
        {
          userGoodsInfo.user_id       = inRequestBasicPtr.user->id;
          userGoodsInfo.goods_info_id = goodsInfo.id;
          userGoodsInfo.initCreateAndUpdateTime(inRequestInput.getTimeZone());
        }
        const std::string& insertUserGoodsInfo = db().insertModelSql<UserGoodsInfo>({userGoodsInfo});
        db().executeSqlUnCloseSession(inSession, insertUserGoodsInfo);
      }
    }
  }

  /**
   * 目前所有的 return 都是 true
   * @param requestInput
   * @param requestBasic
   * @param findCarBody
   * @param outResponse
   * @return
   */
  bool DbProxy::postFindCar(const HttpRequest&      requestInput,
                            const RequestBasicInfo& requestBasic,
                            body::FindCarBody&      findCarBody,
                            HttpResponse&           outResponse) {
    nlohmann::json responseBodyData;

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
        /* 判断 findCar 是否在用户名下 */ {
          const UserFindCar::Names& userFindCarNames      = UserFindCar::names();
          const std::string&        queryUserFindCarWhere = boost::str(
              boost::format(" %1% = %2% AND %3% = %4% %5% ") %
              SqlUtil::backticks(userFindCarNames.user_id) % SqlUtil::nullOrApostrophe(requestBasic.user->id) %
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
          optGoodsInfo(requestInput, requestBasic, session, *findCarBody.goodsInfo, outResponse);
          findCarBody.findCar->goods_info_id = findCarBody.goodsInfo->id;
          if (!outResponse.isEmpty()) return;
        }
        /* update FindCar */ {
          findCarBody.findCar->renewUpdateTime(requestInput.getTimeZone());
          std::string outMsg = {};
          if (findCarBody.findCar->dbUpdateFieldContainIllegalNull(outMsg)) {
            outResponse.inflateResponse(HttpStatus::badRequest, outMsg);
            return;
          }
          db().insert(session,*findCarBody.findCar,false);
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
          optGoodsInfo(requestInput, requestBasic, session, *findCarBody.goodsInfo, outResponse);
          findCarBody.findCar->goods_info_id = findCarBody.goodsInfo->id;
          if (!outResponse.isEmpty()) return;
        }
        /* insert FindCar */ {
          findCarBody.findCar->initCreateAndUpdateTime(requestInput.getTimeZone());
          std::string outMsg = {};
          if(findCarBody.findCar->dbInsertFieldContainIllegalNull(outMsg)) {
            outResponse.inflateResponse(HttpStatus::badRequest, outMsg);
            return;
          }
          findCarBody.findCar->id = db().insert<FindCar>(session, *findCarBody.findCar, false)
                                        .getAutoIncrementValue();
        }
        /* insert UserFindCar */ {
          UserFindCar userFindCar; {
            userFindCar.user_id     = requestBasic.user->id;
            userFindCar.find_car_id = findCarBody.findCar->id;
            userFindCar.initCreateAndUpdateTime(requestInput.getTimeZone());
          }
          db().insert(session, userFindCar, false);
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

  bool DbProxy::postFindCustomers(const HttpRequest&       requestInput,
                                  const RequestBasicInfo&  requestBasic,
                                  body::FindCustomersBody& findBody,
                                  HttpResponse&            outResponse) {
    nlohmann::json responseBodyData;

    auto optCar = [&](mysqlx::Session& session) {
      if(findBody.car->id.has_value()) {
        /* 判断该车是否在当前用户名下 */ {
          const UserCar::Names& modelNames              = UserCar::names();
          const std::string&    queryUserCarWhere = boost::str(
              boost::format(" %1% = %2% AND %3% = %4% %5% ") %
              SqlUtil::backticks(modelNames.user_id) % SqlUtil::nullOrApostrophe(requestBasic.user->id) %
              SqlUtil::backticks(modelNames.car_id) % SqlUtil::nullOrApostrophe(findBody.car->id) %
              db().andDelFilter(false));

          const std::vector<UserCar>& userCarVector = db().query<UserCar>(session, queryUserCarWhere, false);
          if (userCarVector.empty()) {
            outResponse.inflateResponse(HttpStatus::forbidden, " Car 不在用户名下 ");
            return;
          } else if (userCarVector.size() != 1) {
            outResponse.inflateResponse(
                HttpStatus::serverLogicError,
                boost::str(boost::format(" UserCar 个数为(%1%)非法值 ") % userCarVector.size()));
            return;
          }
        }
        /* 更新车辆信息 */ {
          findBody.car->renewUpdateTime(requestInput.getTimeZone());
          /* car 本身数据合法性校验 */ {
            std::string carFieldCheck;
            if (!findBody.car->dbUpdateFieldUnContainIllegalNull(carFieldCheck)) {
              outResponse.inflateResponse(HttpStatus::badRequest, carFieldCheck);
              return;
            }
          }
          const std::string& updateCar = db().updateSingleModelSql<Car>(*findBody.car, GoodsInfo::names().getIgnoreCDColumn());
          db().executeSqlUnCloseSession(session, updateCar);
          findBody.findCustomers->car_id = findBody.car->id;
        }
      } else /* if(!findBody.car->id.has_value()) */ {
        /* insert car */ {
          findBody.car->initCreateAndUpdateTime(requestInput.getTimeZone());
          std::string insertCheckMsg;
          if(!findBody.car->dbInsertFieldUnContainIllegalNull(insertCheckMsg)) { /* 数据插入前的合法性校验 */
            outResponse.inflateResponse(HttpStatus::badRequest, insertCheckMsg);
            return;
          }
          findBody.findCustomers->car_id = findBody.car->id =
              db().insert(session, *findBody.car, false)
                  .getAutoIncrementValue();
        }
        /* insert user car */ {
          UserCar userCar ;{
            userCar.user_id = requestBasic.user->id;
            userCar.car_id = findBody.car->id;
            userCar.initCreateAndUpdateTime(requestInput.getTimeZone());
          }
          db().insert<UserCar>(session, userCar, false);
        }
      }
    };

    if (findBody.findCustomers->id.has_value()) {
      db().executeTransactionSql([&](mysqlx::Session& session) -> void {
        /* 判断 findCustomer 是否在用户名下 */ {
          const UserFindCustomers::Names& userUserFindCustomersNames      = UserFindCustomers::names();
          const std::string&              queryUserFindCustomersWhere     = boost::str(
              boost::format(" %1% = %2% AND %3% = %4% %5% ") %
              SqlUtil::backticks(userUserFindCustomersNames.user_id) % SqlUtil::nullOrApostrophe(requestBasic.user->id) %
              SqlUtil::backticks(userUserFindCustomersNames.find_customers_id) % SqlUtil::nullOrApostrophe(findBody.findCustomers->id) %
              db().andDelFilter(false));

          const std::vector<UserFindCustomers>& userFindCustomersVector = db().query<UserFindCustomers>(session, queryUserFindCustomersWhere, false);
          if (userFindCustomersVector.empty()) {
            outResponse.inflateResponse(HttpStatus::forbidden, " userFindCustomer 不在用户名下 ");
            return;
          } else if (userFindCustomersVector.size() != 1) {
            outResponse.inflateResponse(HttpStatus::serverLogicError, " userFindCustomer 个数大于 1 ");
            return;
          }
        }
        /* insert or update car */ {
          optCar(session);
          if (!outResponse.isEmpty()) return;
        }
        /* insert or update GoodsInfo */ {
          optGoodsInfo(requestInput, requestBasic, session, *findBody.goodsInfo, outResponse);
          if (!outResponse.isEmpty()) return;
          findBody.findCustomers->goods_info_id = findBody.goodsInfo->id;
        }
        /* update findCustomer */ {
          findBody.findCustomers->renewUpdateTime(requestInput.getTimeZone());
          std::string outMsg = {};
          if(findBody.findCustomers->dbUpdateFieldContainIllegalNull(outMsg)){
            outResponse.inflateResponse(HttpStatus::badRequest, outMsg);
            return;
          }
          const std::string& updateFindCustomer = db().updateSingleModelSql<FindCustomers>(
              findBody.findCustomers.value(), FindCar::names().getIgnoreCDColumn());
          db().executeSqlUnCloseSession(session, updateFindCustomer);
        }
      });
    } else /* if (!findBody.findCustomers->id.has_value()) */ {
      db().executeTransactionSql([&](mysqlx::Session& session) -> void {
        /* insert or update car */ {
          optCar(session);
          if (!outResponse.isEmpty()) return;
        }
        /* insert or update GoodsInfo */ {
          optGoodsInfo(requestInput, requestBasic, session, *findBody.goodsInfo, outResponse);
          if (!outResponse.isEmpty()) return;
          findBody.findCustomers->goods_info_id = findBody.goodsInfo->id;
        }
        /* insert findCustomer */ {
          findBody.findCustomers->initCreateAndUpdateTime(requestInput.getTimeZone());
          std::string outMsg = {};
          if(!findBody.findCustomers->dbInsertFieldUnContainIllegalNull(outMsg)) {
            outResponse.inflateResponse(HttpStatus::badRequest, outMsg);
            return;
          }
          findBody.findCustomers->id =
              db().insert(session, *findBody.findCustomers, false)
                  .getAutoIncrementValue();
        }
        /* insert user find customer */ {
          UserFindCustomers userFindCustomers; {
            userFindCustomers.user_id = requestBasic.user->id;
            userFindCustomers.find_customers_id = findBody.findCustomers->id;
            userFindCustomers.initCreateAndUpdateTime(requestInput.getTimeZone());
          }
          db().insert(session, userFindCustomers, false);
        }
      });
    }


    if(!outResponse.isEmpty()) return false;

    responseBodyData[GoodsInfo::names().tableName]     = findBody.goodsInfo;
    responseBodyData[FindCustomers::names().tableName] = findBody.findCustomers;
    responseBodyData[Car::names().tableName]           = findBody.car;
    outResponse.inflateResponse(HttpStatus::success);
    outResponse.inflateBodyData(responseBodyData);
    return true;
  }

  /* X找车信息提交 */
  bool DbProxy::postSearchXxFindCar(const HttpRequest&      requestInput,
                                    const RequestBasicInfo& requestBasic,
                                    body::FindCarBody&      findCarBody,
                                    HttpResponse&           outResponse) {
    /* 1. 这会是多条件搜索
     * 2. 排序方式是也是多条件的排序
     *    > 这一步多个条件无法合理排序 , 我们需要将每个条件都抽象成具有一致评价方式的数字
     *    > 比如所有条件都想办法转换成得到的数字越大 , 结果越优
     * 3. 我们需要先把需要的数据搜索出来然后分页返回给客户端
     *
     *
     * 1. 我们搜索 FindCustomers 需要增加额外字段
     *    > 与搜索入参的直线距离参数  计算出数字  距离越小越优 , 如果为结果取反 , 则评价方式就是值越大结果越优
     *    > 出发时间距离            计算出数字  这个数字本身计算的是合作双方的重叠时间 , 数字本身值越大结果越优
     *    > 用车形式               匹配出结果  我们可以按逻辑, 匹配的给大值 , 不匹配的给小值 , 结论也是值越大越优
     * 2. 上述的诸多条件我们确实还不会计算 , 我们试着将问题简化为我们有能力计算的问题
     *    > 根据入参坐标和行内坐标计算坐标之间的距离 , 并按距离排序 .
     *    > 出发时间重合秒数
     *    > 用车形式是否匹配
     * 3. 我们的搜索需要分页
     * */

    if(findCarBody.findCar.has_value()) {

      if(!findCarBody.findCar->existTwoPoint()) {
        /* 请求非法 , 查询请求必须携带起始点坐标才被认为有效 */
        return false;
      }

      /* 现在我们假设所有条件都是完备的先写一个 sql 试试 , 后面再做拆分 */

      /* 出发地经纬度 计算合作双方的出发点距离 distance_of_two_start */
      if (findCarBody.findCar->start_point_longitude.has_value() &&
          findCarBody.findCar->start_point_latitude.has_value()) {
      }

      /* 到达地经纬度  计算合作双方的到达点距离 distance_of_two_end */
      if (findCarBody.findCar->end_point_longitude.has_value() &&
          findCarBody.findCar->end_point_latitude.has_value()) {
      }

      /* 出发时间范围 计算出发时间是否有重合 */
      if(findCarBody.findCar->departure_time_range_start.has_value() &&
          findCarBody.findCar->departure_time_range_end.has_value()){
      }

      /* 用车形式 计算合作双方的用车供求形式是否匹配 */
      if(findCarBody.findCar->service_form.has_value()) {

      }

    }

    std::string tableCountSql = boost::str(boost::format("SELECT COUNT(*) FROM %1% ;") %
                                           SqlUtil::getDbAndTableName(FindCustomers::names().tableName));

    std::string querySql;
    querySql =
        " SELECT "
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().id) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().start_point_json) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().end_point_json) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().service_form) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().departure_time_range_start) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().departure_time_range_end) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().pick_up_point) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().people_number) +
        "\n\t" + SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().append_info) +
        boost::str(boost::format("\n\tCOORDINATE_DISTANCE_METER(%1%, %2%, %3%, %4%) AS distance_of_two_start") % /* 起点距离 */
                   SqlUtil::nullOrApostrophe(findCarBody.findCar->start_point_longitude) %
                   SqlUtil::nullOrApostrophe(findCarBody.findCar->start_point_latitude) %
                   SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().start_point_longitude) %
                   SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().start_point_latitude)) +
        boost::str(boost::format("\n\tCOORDINATE_DISTANCE_METER(%1%, %2%, %3%, %4%) AS distance_of_two_end") % /* 终点距离 */
                   SqlUtil::nullOrApostrophe(findCarBody.findCar->end_point_longitude) %
                   SqlUtil::nullOrApostrophe(findCarBody.findCar->end_point_latitude) %
                   SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().end_point_longitude) %
                   SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().end_point_latitude)) +
        boost::str(boost::format("\n\tCALCULATE_TIME_PERIOD_OVERLAP(%1%, %2%, %3%, %4%, %5%, %6%) AS time_period_overlap_second") % /* 出发时间重合秒数 */
                   SqlUtil::nullOrApostrophe(findCarBody.findCar->departure_time_range_start) %
                   SqlUtil::nullOrApostrophe(findCarBody.findCar->departure_time_range_end) %
                   SqlUtil::nullOrApostrophe(findCarBody.findCar->departure_time_range_tz) %
                   SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().departure_time_range_start) %
                   SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().departure_time_range_end) %
                   SqlUtil::tableColumn(FindCustomers::names().tableName, FindCustomers::names().departure_time_range_tz)) +
        "\n FROM " + SqlUtil::getDbAndTableName(FindCustomers::names().tableName) +
        "\n ORDER BY \n\t"
        "CASE WHEN "



        ;
  }

  /* 车找X信息提交 */
  bool DbProxy::postSearchCarFindXx(const HttpRequest&       requestInput,
                                    const RequestBasicInfo&  requestBasic,
                                    body::FindCustomersBody& findBody,
                                    HttpResponse&            outResponse) {
  }

}  // namespace nameless_carpool

#pragma once

#include <optional>
#include <sstream>
#include <string>

#include "src/utils/common.h"
#include "src/utils/json/include_json.h"
#include "src/db/model/include_db_model.h"

namespace nameless_carpool {

  /* 按照每一个细分业务进行更进一步的作用域划分 */

  /* 登录相关 */
  namespace Login {

    struct RequestBody;
    struct Response;

  }  // namespace Login

  /* 请求验证码 */
  namespace RequestVerifyCode {

    struct RequestBody;

  }
}  // namespace nameless_carpool


struct nameless_carpool::Login::RequestBody {

  struct Names{
    /* 仅仅用于命名属性  */
    const std::string className   = "RequestBody";
    const std::string phone       = "phone";
    const std::string verify_code = "verify_code";
    const std::string client_type = "client_type";
  };


  std::optional<std::string> phone;
  std::optional<std::string> verify_code;
  std::optional<std::string> client_type;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RequestBody, phone, verify_code, client_type)

  /* 参数合法性校验
     @return true, 合法 ;
             否则 非法 ;    非法情况下 , ssMsg 含有非法信息*/
  bool legalityCheck(std::string& msg) {
    std::stringstream ssMsg;
    bool         result = true;

    { /* 手机号合法性校验 */
      bool isLegalPhone = Common::regexVals.isLegalPhone(phone.value_or(""));
      if (!isLegalPhone) {
        result = false;
        ssMsg << "phone inlegal :" << phone.value_or("not found field") << std::endl;
      }
    }

    { /* 验证码合法性校验 */
      bool isLegal = Common::regexVals.isLegalVC(verify_code.value_or(""));
      if (!isLegal) {
        result = false;
        ssMsg << "verify_code inlegal : " << verify_code.value_or("not found field") << std::endl;
      }
    }

    msg = ssMsg.str();
    return result;
  }

 
};

struct nameless_carpool::Login::Response {
  struct User_ : public User {
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(User_,

                                   id,
                                   id_card_num,
                                   name,
                                   gender,
                                   birth_date,
                                   birth_date_tz,

                                   create_time,
                                   create_time_tz,
                                   update_time,
                                   update_time_tz,
                                   del_time,
                                   del_time_tz)
  };
  struct Session_ :public Session {
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Session_,

                                   id,
                                   client_more_info,
                                   client_type,
                                   token,
                                   max_age,
                                   from_where,
                                   auth_method,
                                   token_update_time,
                                   token_update_time_tz,

                                   create_time,
                                   create_time_tz,
                                   update_time,
                                   update_time_tz,
                                   del_time,
                                   del_time_tz)
  };

  std::optional<User_>    user    = std::nullopt;
  std::optional<Session_> session = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Response, user, session)
};



struct nameless_carpool::RequestVerifyCode::RequestBody {
  std::optional<std::string> phone;

  /** 参数合法性校验
      @return true, 合法 ;
             否则 非法 ;    非法情况下 , ssMsg 含有非法信息
  */
  bool legalityCheck(std::string& msg) {
    std::stringstream ssMsg;
    bool              result = true;

    { /* 手机号合法性校验 */
      bool isLegalPhone = Common::regexVals.isLegalPhone(phone.value_or(""));
      if (!isLegalPhone) {
        result = false;
        ssMsg << "phone inlegal :" << phone.value_or("not found field") << std::endl;
      }
    }

    msg = ssMsg.str();
    return result;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RequestBody, phone)
};
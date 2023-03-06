
#pragma once

#include <string>
#include <sstream>
#include <optional>
#include "common.h"
#include "include_json.h"

namespace nameless_carpool {

  // using namespace std;

  /* 按照每一个细分业务进行更进一步的作用域划分 */

  /* 登录下相关 */
  namespace Login {

    struct RequestBody {
      std::optional<std::string> phone;
      std::optional<std::string> verify_code;
      std::optional<std::string> client_type;

      /* 参数合法性校验 
         @return true, 合法 ; 
                 否则 非法 ;    非法情况下 , ssMsg 含有非法信息*/
      bool legalityCheck(std::string& msg) {
        stringstream ssMsg;
        bool result = true;

        { /* 手机号合法性校验 */
          bool isLegalPhone = Common::regexVals.isLegalPhone(phone.value_or(""));
          if(!isLegalPhone) {
              result = false;
              ssMsg << "phone inlegal :" << phone.value_or("not found field") << endl;
          }
        }

        { /* 验证码合法性校验 */
          bool isLegal = Common::regexVals.isLegalVC(verify_code.value_or(""));
          if(!isLegal) {
            result = false;
            ssMsg << "verify_code inlegal : " << verify_code.value_or("not found field") << endl;
          }
        }

        msg = ssMsg.str();
        return result;
      }

      NLOHMANN_DEFINE_TYPE_INTRUSIVE(RequestBody, phone, verify_code)
      // NLOHMANN_DEFINE_OPTIONAL_TYPE_INTRUSIVE(RequestBody, phone, verify_code)
    };

  }

  /* 请求验证码 */
  namespace RequestVertifyCode {

    struct RequestBody {
      std::optional<std::string> phone;

      /** 参数合法性校验 
          @return true, 合法 ; 
                 否则 非法 ;    非法情况下 , ssMsg 含有非法信息
      */
      bool legalityCheck(string& msg) {
        std::stringstream ssMsg;
        bool result = true;

        { /* 手机号合法性校验 */
          bool isLegalPhone = Common::regexVals.isLegalPhone(phone.value_or(""));
          if(!isLegalPhone) {
              result = false;
              ssMsg << "phone inlegal :" << phone.value_or("not found field") << endl;
          }
        }

        msg = ssMsg.str();
        return result;
      }

      NLOHMANN_DEFINE_TYPE_INTRUSIVE(RequestBody, phone)
      
    };

    
  }
}
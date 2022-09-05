
#pragma once

#include <string>
#include <sstream>
#include <regex>
// #include "../../utils/all_include.h"
#include "../../utils/common.h"

namespace nameless_carpool {

  using namespace std;

  namespace Body {

    struct Login {
      string phone;
      string verify_code;

      /* 仅仅用于命名属性  */
      const string name_phone = "phone";
      const string name_verify_code = "verify_code";

      /* 参数合法性校验 
         @return true, 合法 ; 
                 否则 非法 ;    非法情况下 , ssMsg 含有非法信息*/
      bool legalityCheck(string& msg) {
        stringstream ssMsg;
        bool result = true;

        { /* 手机号合法性校验 */
          regex phoneRegex(Common::regex.phone_zh_CN);
          bool isLegalPhone = regex_match(phone, phoneRegex);
          if(!isLegalPhone) {
              result = false;
              ssMsg << "phone inlegal" << endl;
          }
        }

        { /* 验证码合法性校验 */
          regex vcRegex(Common::regex.verifyCode);
          bool isLegal = regex_match(verify_code, vcRegex);
          if(!isLegal) {
            result = false;
            ssMsg << "verify_code inlegal" << endl;
          }
        }

        msg = ssMsg.str();
        return result;
      }

      NLOHMANN_DEFINE_TYPE_INTRUSIVE(Login, phone, verify_code)
    };

    
  }
}
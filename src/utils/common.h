#pragma once

/* 用于记录一些常量的值 */

#include <string>

namespace nameless_carpool {

  using namespace std;

  namespace Common {
    struct Regex {
      /* 其他国家手机号字符串匹配规则 : https://cloud.tencent.com/developer/article/1140913 */
      /* 中国 手机号正则字符串  */
      const string phone_zh_CN = "^(\\+?0?86\\-?)?1[345789]\\d{9}$";

      const string verifyCode = "^\\d{6}$";
    };

    extern Regex regex;
  }
}
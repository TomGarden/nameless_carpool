#pragma once

#include <string>

namespace nameless_carpool {

  class DbProxy {

    using string = std::string;

    private:
      DbProxy();                                                 /* 默认构造 */

    public:
      DbProxy(const DbProxy&) = delete;              /* 拷贝构造 */
      DbProxy& operator=(const DbProxy&) = delete;   /* 拷贝赋值 */
      DbProxy(const DbProxy&&) = delete;             /* 移动构造 */
      DbProxy& operator=(const DbProxy&&) = delete;  /* 移动赋值 */

    public:
      static DbProxy& getInstance();


    public:
      /* 针对手机号请求验证码 , 并不一定会产生新验证码 , 可能是延长旧验证码的时效性 
         @return true, 操作成功 
                 false,查看 errmsg 了解详情*/
     bool requestVertifyCode(const string& phoneNumber, const string& timeZone,
                             string& internalMsg, string& externalMsg);
  };
}
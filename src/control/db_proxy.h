#pragma once

#include <string>
#include "http_util.h"
#include "authenticate_m.h"

namespace nameless_carpool {

  class DbProxy {
    using string = std::string;

   private:
    DbProxy(); /* 默认构造 */

   public:
    DbProxy(const DbProxy&) = delete;             /* 拷贝构造 */
    DbProxy& operator=(const DbProxy&) = delete;  /* 拷贝赋值 */
    DbProxy(const DbProxy&&)           = delete;  /* 移动构造 */
    DbProxy& operator=(const DbProxy&&) = delete; /* 移动赋值 */

   public:
    static DbProxy& getInstance();

   public:
    /** @description:  针对手机号请求验证码 , 并不一定会产生新验证码 , 可能是延长旧验证码的时效性
     *  @param {string&} phoneNumber
     *  @param {string&} timeZone
     *  @param {string&} internalMsg    用于开发者内部分享的异常信息
     *  @param {string&} externalMsg    用于面向用户提醒的异常信息
     *  @return {*} HttpStatusEnum::success, 操作成功
     */
    HttpStatusEnum requestVertifyCode(const string& phoneNumber, const string& timeZone,
                            string& internalMsg, string& externalMsg);

    
    /** @description: 登录动作
     * @param {string&} phoneNumber
     * @param {string&} timeZone
     * @param {string&} vertifyCode
     * @param {string&} internalMsg
     * @param {string&} externalMsg
     * @return {*}  true, 操作成功, 结果有效
     */
    bool login(const HttpRequest& requestInput, const Login::RequestBody& body, HttpResponse& responseOutput);
  };
}  // namespace nameless_carpool
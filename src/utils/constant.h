
#pragma once

#include <string>

/* 设计用于管理常量们 */

namespace nameless_carpool {

  struct ConstantStr {
    const std::string flag           = "我是一个字符串呀";
    const std::string bodyFormatErr  = "请求体格式 校验 失败;";
    const std::string headerMissErr  = "请求头丢失 : ";
    const std::string logicException = "逻辑意外 , 预期外情况 , 暂时无法处理";
    const std::string serverErr      = "服务器错误";

    const std::string pleaseRetry = "请重试";

    const std::string vertifyCode        = "验证码";
    const std::string pleaseReapplyVc    = "请重新申请" + vertifyCode;
    const std::string vertifyCodeErr     = vertifyCode + "错误";
    const std::string vertifyCodeExpired = vertifyCode + "已到期";

    const std::string telNumber      = "手机号";
    const std::string telNotRegister = "未注册" + telNumber;

    const std::string tzUTC          = "UTC";
    const std::string tzAsiaShanghai = "Asia/Shanghai";
  };

  extern const ConstantStr constantStr;
}  // namespace nameless_carpool
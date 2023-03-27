//
// Created by Tom on 2023/3/22.
//

#pragma once

#include <optional>
#include <string>
#include <utility>

#include "include_json.h"

/* 数据模型 : https://lbs.amap.com/api/webservice/guide/api/inputtips */
namespace nameless_carpool {

  struct InputTipRequest;
  struct InputTipResponse;
  struct InputTipItem;

}  // namespace nameless_carpool

struct nameless_carpool::InputTipRequest {
  /*必填*/ std::optional<std::string> key      = std::nullopt; /* 请求服务权限标识 必填 */
  /*必填*/ std::optional<std::string> keywords = std::nullopt; /* 查询关键词 */
  std::optional<std::string>          type     = std::nullopt; /* POI分类
                                                                  服务可支持传入多个分类，多个类型剑用“|”分隔
                                                                  可选值：POI分类名称、分类代码
                                                                  此处强烈建议使用分类代码，否则可能会得到不符合预期的结果 */
  std::optional<std::string>          location  = std::nullopt;
  std::optional<std::string>          city      = std::nullopt;
  std::optional<bool>                 citylimit = std::nullopt; /* 仅返回指定城市数据 ; 可选值：true/false */
  std::optional<std::string>          datatype  = std::nullopt; /*返回的数据类型 ; 多种数据类型用“|”分隔，可选值：all-返回所有数据类型、poi-返回POI数据类型、bus-返回公交站点数据类型、busline-返回公交线路数据类型*/
  std::optional<std::string>          sig       = std::nullopt;
  /*必填*/ std::optional<std::string> output    = "JSON";
  std::optional<std::string>          callback  = std::nullopt;

  /**
   * 校验必填属性是否存在值
   * @return  true 合法; false 非法;
   */
  inline bool isLegal() const {
    if (key.value_or("").empty()) return false;
    if (keywords.value_or("").empty()) return false;
    if (output.value_or("").empty()) return false;
    return true;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(InputTipRequest,
                                              key,
                                              keywords,
                                              type,
                                              location,
                                              city,
                                              citylimit,
                                              datatype,
                                              sig,
                                              output,
                                              callback)
};

struct nameless_carpool::InputTipItem {
  std::optional<std::string> id       = std::nullopt;
  std::optional<std::string> name     = std::nullopt;
  std::optional<std::string> district = std::nullopt;
  std::optional<std::string> adcode   = std::nullopt;
  std::optional<std::string> location = std::nullopt;
  std::optional<std::string> address  = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(InputTipItem,
                                              id,
                                              name,
                                              district,
                                              adcode,
                                              location,
                                              address)
};

struct nameless_carpool::InputTipResponse {
  std::optional<std::string> status = std::nullopt;
  std::optional<std::string> info   = std::nullopt;
  std::optional<std::string> count  = std::nullopt;
  std::vector<InputTipItem>  tips;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(InputTipResponse,
                                              status,
                                              info,
                                              count,
                                              tips)
};

//
// Created by Tom on 2023/3/28.
//

#pragma once

#include "travel_info.h"
#include "include_json.h"
#include "struct_field_name.h"

namespace nameless_carpool::body {

  struct FindCarBody;
  struct FindCustomersBody;

}  // namespace nameless_carpool::body

struct nameless_carpool::body::FindCarBody{
  std::optional<FindCar>   findCar   = std::nullopt;
  std::optional<GoodsInfo> goodsInfo = std::nullopt;

  STRUCT_FIELD_NAME_DEFINE_NAMES_WITH_GET(findCar ,goodsInfo)

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FindCarBody, findCar ,goodsInfo)

  /**
   * 校验入参是否合法
   * @param outErrMsg
   * @return false, 非法 , outErrMsg 有意义
   */
  bool isLegal(std::string& outErrMsg) const {
    if (!findCar.has_value()) {
      outErrMsg = "缺少参数 : findCar";
      return false;
    }
    if (!findCar->goods_info_id.has_value() && !goodsInfo.has_value()) {
      outErrMsg = "findCar.goods_info_id / goodsInfo , 至少存在一个";
      return false;
    }

    return true;
  }

};

struct nameless_carpool::body::FindCustomersBody{
  std::optional<FindCustomers> findCustomers = std::nullopt;
  std::optional<GoodsInfo>     goodsInfo     = std::nullopt;
  std::optional<Car>           car           = std::nullopt;

  STRUCT_FIELD_NAME_DEFINE_NAMES_WITH_GET(findCustomers, goodsInfo, car)

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FindCustomersBody, findCustomers, goodsInfo, car)

  /**
   * 校验入参是否合法
   * @param outErrMsg
   * @return false, 非法 , outErrMsg 有意义
   */
  bool isLegal(std::string& outErrMsg) const {
    if (!findCustomers.has_value()) {
      outErrMsg = "缺少参数 : findCar";
      return false;
    }
    if (!findCustomers->goods_info_id.has_value() && !goodsInfo.has_value()) {
      outErrMsg = "findCar.goods_info_id 和 goodsInfo , 至少存在一个";
      return false;
    }
    if (!findCustomers->car_id.has_value() && !car.has_value()) {
      outErrMsg = "findCustomers->car_id 和 car , 至少存在一个";
      return false;
    }

    return true;
  }

};

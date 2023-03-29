//
// Created by Tom on 2023/3/27.
//

#pragma once

#include "macro_base.h"



namespace nameless_carpool {

  /* 用作某个类的内部类 , 用于生成列的成员名称相关字符串 */
  /*

      struct nameless_carpool::body::FindCarBody{
        std::optional<FindCar>   findCar   = std::nullopt;
        std::optional<GoodsInfo> goodsInfo = std::nullopt;

        STRUCT_FIELD_NAME_DEFINE_NAMES_WITH_GET(findCar ,goodsInfo)
      };

   -------------------------------------------------------------

    struct nameless_carpool::body::FindCarBody{

      std::optional<FindCar>   findCar   = std::nullopt;
      std::optional<GoodsInfo> goodsInfo = std::nullopt;

      struct Names {
        const std::string findCar   = "findCar";
        const std::string goodsInfo = "goodsInfo";
      };
      static Names& names() {
        static Names names;
        return names;
      }
    };

   */


#define STRUCT_FIELD_NAME_DEFINE_FIELD(field) const std::string field = #field;

#define STRUCT_FIELD_NAME_DEFINE_NAMES(...)                                              \
  struct Names {                                                                         \
    MACRO_BASE__EXPAND(MACRO_BASE__0_PASTE(STRUCT_FIELD_NAME_DEFINE_FIELD, __VA_ARGS__)) \
  };

#define STRUCT_FIELD_NAME_GET_NAMES(structName) \
  static Names& names() {     \
    static Names names;       \
    return names;             \
  }

#define STRUCT_FIELD_NAME_DEFINE_NAMES_WITH_GET(...) \
  STRUCT_FIELD_NAME_DEFINE_NAMES(__VA_ARGS__)        \
  STRUCT_FIELD_NAME_GET_NAMES(structName)
}  // namespace nameless_carpool
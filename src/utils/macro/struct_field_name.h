//
// Created by Tom on 2023/3/27.
//

#pragma once

#include "macro_base.h"
#include "vector"
#include "include_json.h"


/* 生成内部结构 struct Names{}  */
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


/* 针对 model 的某些成员变量生成空检测函数 */
namespace nameless_carpool {

#define DB_UPDATE_UN_NULL_COLUMN_CHECK_FUNC(field) \
  if (!field.has_value()) nullItemsName.push_back(#field);


#define DB_XXX_UN_NULL_COLUMN_CHECK_BODY(...)                                               \
  std::vector<std::string> nullItemsName = {};                                              \
  MACRO_BASE__EXPAND(MACRO_BASE__0_PASTE(DB_UPDATE_UN_NULL_COLUMN_CHECK_FUNC, __VA_ARGS__)) \
  if (nullItemsName.empty()) return true;                                                   \
  nlohmann::json jObj = nullItemsName;                                                      \
  outMsg.append("缺少参数:").append(jObj.dump());                                           \
  return false;

#define DB_UPDATE_UN_NULL_COLUMN_CHECK(...)                                                                \
  /** 在进行 update 动作的时候, 不包含空值                                                    \
   * @param outMsg                                                                                         \
   * @return true, 在进行 update 动作的时候, 不包含空值, outMsg 无意义 ;                   \
   *         false , 包含非法空值 outMsg 有意义                                                   \
   */                                                                                                      \
  inline bool dbUpdateFieldUnContainIllegalNull(std::string& outMsg) {                                             \
    DB_XXX_UN_NULL_COLUMN_CHECK_BODY(__VA_ARGS__)                                                          \
  }                                                                                                        \
  /** 在进行 update 动作的时候 , 必须非空的列有空值出现                                 \
   * @param outMsg                                                                                         \
   * @return true, 在进行 update 动作的时候 , 必须非空的列有空值出现 ; outMsg 有意义 \
   */                                                                                                      \
  inline bool dbUpdateFieldContainIllegalNull(std::string& outMsg) {                                       \
    return !dbUpdateFieldUnContainIllegalNull(outMsg);                                                     \
  }
#define DB_INSERT_UN_NULL_COLUMN_CHECK(...)                                                                \
  /** 在进行 insert 动作的时候, 不包含空值                                                    \
   * @param outMsg                                                                                         \
   * @return true, 在进行 insert 动作的时候, 不包含空值, outMsg 无意义 ;                    \
   *         false , 包含非法空值 outMsg 有意义                                                   \
   */                                                                                                      \
  inline bool dbInsertFieldUnContainIllegalNull(std::string& outMsg) {                                     \
    DB_XXX_UN_NULL_COLUMN_CHECK_BODY(__VA_ARGS__)                                                          \
  }                                                                                                        \
  /** 在进行 insert 动作的时候 , 必须非空的列有空值出现                                 \
   * @param outMsg                                                                                         \
   * @return true, 在进行 insert 动作的时候 , 必须非空的列有空值出现 ; outMsg 有意义 \
   */                                                                                                      \
  inline bool dbInsertFieldContainIllegalNull(std::string& outMsg) {                                       \
    return !dbInsertFieldUnContainIllegalNull(outMsg);                                                     \
  }

}  // namespace nameless_carpool
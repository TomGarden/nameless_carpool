//
// Created by Tom on 2023/3/27.
//

#pragma once

#include "exception"
#include "log_utils.h"
#include "macro_base.h"
#include "string"

namespace nameless_carpool {

  /*

    ENUM_UTIL_DEFINE_ENUM(Flag,
             login,
             connect
    )

    struct Flag {
      enum Enum { login,
                  connect };
      inline static std::string toString(const Enum& input) {
        if (input == Enum::login) return "login";
        if (input == Enum::connect) return "connect";
        throw std::logic_error("un know exception ~~~~~ 1");
      }
      inline static Enum toEnum(const std::string& str) {
        if (str == "login") return Enum::login;
        if (str == "connect") return Enum::connect;
        return std::nullopt;
      }

    };

   */


#define ENUM_UTIL_CHECK_ITEM_AND_RETURN_STR(Type, input, field) \
  if (input == Type::field) return #field;
#define ENUM_UTIL_CHECK_ITEM_AND_RETURN_VAL(Type, str, field) \
  if (str == #field) return Type::field;


#define ENUM_UTIL_VAL_TO_STRING(Type, ...)                                                                 \
  static std::string toString(const Type& input) {                                                         \
    MACRO_BASE__EXPAND(MACRO_BASE__2_PASTE(ENUM_UTIL_CHECK_ITEM_AND_RETURN_STR, Type, input, __VA_ARGS__)) \
    throw std::logic_error(WITH_LINE_INFO("un know exception ~~~~~ 1"));                                   \
  }

#define ENUM_UTIL_VAL_TO_STRING_VIEW(Type, ...)                                                            \
  static std::string_view toStrView(const Type& input) {                                                   \
    MACRO_BASE__EXPAND(MACRO_BASE__2_PASTE(ENUM_UTIL_CHECK_ITEM_AND_RETURN_STR, Type, input, __VA_ARGS__)) \
    throw std::logic_error(WITH_LINE_INFO("un know exception ~~~~~ 1"));                                   \
  }

#define ENUM_UTIL_STRING_TO_VAL(Type, ...)                                                               \
  static std::optional<Type> toEnum(const std::string& str) {                                            \
    MACRO_BASE__EXPAND(MACRO_BASE__2_PASTE(ENUM_UTIL_CHECK_ITEM_AND_RETURN_VAL, Type, str, __VA_ARGS__)) \
    return std::nullopt;                                                                                 \
  }

#define ENUM_UTIL_ENUM_CONVERT(Type, ...)         \
  ENUM_UTIL_VAL_TO_STRING(Type, __VA_ARGS__)      \
  ENUM_UTIL_VAL_TO_STRING_VIEW(Type, __VA_ARGS__) \
  ENUM_UTIL_STRING_TO_VAL(Type, __VA_ARGS__)

/**
 *  @code RAW
 *   ENUM_UTIL_DEFINE_ENUM(PickUpPoint,
 *                         nearby,
 *                         door_to_door
 *                        )
 *  @endcode
 *
 *  @code CONVERT RESULT
 *   struct PickUpPoint {
 *     enum Enum { nearby,
 *                 door_to_door };
 *     static std::string toString(const Enum& input) {
 *       if (input == Enum::nearby) return "nearby";
 *       if (input == Enum::door_to_door) return "door_to_door";
 *       throw std::logic_error(
 *          WITH_LINE_INFO("un know exception ~~~~~ 1"));
 *     }
 *     static std::string_view toStrView(const Enum& input) {
 *       if (input == Enum::nearby) return "nearby";
 *       if (input == Enum::door_to_door) return "door_to_door";
 *       throw std::logic_error(
 *          WITH_LINE_INFO("un know exception ~~~~~ 1"));
 *     }
 *     static std::optional<Enum> toEnum(const std::string& str) {
 *       if (str == "nearby") return Enum::nearby;
 *       if (str == "door_to_door") return Enum::door_to_door;
 *       return std::nullopt;
 *     }
 *    };
 *  @endcode
 * */
#define ENUM_UTIL_DEFINE_ENUM(Type, ...)      \
  struct Type {                               \
    enum Enum {                               \
      __VA_ARGS__                             \
    };                                        \
    ENUM_UTIL_ENUM_CONVERT(Enum, __VA_ARGS__) \
  };

}  // namespace nameless_carpool

/* enum 单参数 默认值 */
/**
 * @brief 用于生成枚举类型 , 后缀 [__WITH_DEF_VAL] 表示生成的枚举不指定值
 *
 * @code RAW
 *   ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
 *       AnimalWithDefVal,
 *       std::string, name,
 *       DOG, "dog",
 *       CAT, "cat")
 * @endcode
 *
 * @code CONVERT RESULT
 *   struct AnimalWithDefVal {
 *     enum Enum { DOG,
 *                 CAT,
 *     };
 *     struct Val {
 *       const std::string name;
 *     };
 *     static const Val& val_DOG() {
 *       static const Val val{"dog"};
 *       return val;
 *     }
 *     static const Val& val_CAT() {
 *       static const Val val{"cat"};
 *       return val;
 *     }
 *     static std::string enum_to_name(Enum input) {
 *       if (input == DOG) return val_DOG().name;
 *       if (input == CAT) return val_CAT().name;
 *       throw std::logic_error(
 *          WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)"));
 *     }
 *     static std::optional<Enum> name_to_enum(std::string& input) {
 *       if (input == val_DOG().name) return DOG;
 *       if (input == val_CAT().name) return CAT;
 *       return std::nullopt;
 *     }
 *   }
 * @endcode
 * */
namespace nameless_carpool {

/* region sub */
#define ENUM_UTIL_1_FIELD_DEF_GENERATE_ENUM_ITEM(TypeDeclare, field1type, field1declare, enumKey, type1val) enumKey,
#define ENUM_UTIL_1_FIELD_DEF_GET_VAL_OBJ(TypeDeclare, field1type, field1declare, enumKey, type1val) \
  static const TypeDeclare##Val& val_##enumKey() {                                                   \
    static const TypeDeclare##Val val{type1val};                                                     \
    return val;                                                                                      \
  }
#define ENUM_UTIL_1_FIELD_DEF_ENUM_TO_DECLARE(TypeDeclare, field1type, field1declare, enumKey, type1val) \
  if (input == enumKey) return val_##enumKey().field1declare;
#define ENUM_UTIL_1_FIELD_DEF_DECLARE_TO_ENUM(TypeDeclare, field1type, field1declare, enumKey, type1val) \
  if (input == val_##enumKey().field1declare) return enumKey;
/* endregion sub */

/* region ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE */
/**
 * @brief 用于生成枚举类型 , 后缀 [__WITH_DEF_VAL] 表示生成的枚举不指定值
 *
 * @code RAW
 *   ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE(
 *       AnimalWithDefVal__,
 *       std::string, name,
 *       DOG, "dog",
 *       CAT, "cat")
 * @endcode
 *
 * @code CONVERT RESULT
 *   enum AnimalWithDefVal__ { DOG,
 *                             CAT,
 *   };
 *   struct Val {
 *     const std::string name;
 *   };
 *   static const Val& val_DOG() {
 *     static const Val val{"dog"};
 *     return val;
 *   }
 *   static const Val& val_CAT() {
 *     static const Val val{"cat"};
 *     return val;
 *   }
 *   static std::string enum_to_name(AnimalWithDefVal__ input) {
 *     if (input == DOG) return val_DOG().name;
 *     if (input == CAT) return val_CAT().name;
 *     throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)"));
 *   }
 *   static std::optional<AnimalWithDefVal__> name_to_enum(std::string& input) {
 *     if (input == val_DOG().name) return DOG;
 *     if (input == val_CAT().name) return CAT;
 *     return std::nullopt;
 *   }
 * @endcode
 * */
#define ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE(                                                                                                   \
    TypeDeclare,                                                                                                                               \
    field1type, field1declare,                                                                                                                 \
    /*enumKey, type1val*/...)                                                                                                                  \
                                                                                                                                               \
  enum TypeDeclare {                                                                                                                           \
    MACRO_BASE__EXPAND(MACRO_BASE__S3_D2_PASTE(ENUM_UTIL_1_FIELD_DEF_GENERATE_ENUM_ITEM, TypeDeclare, field1type, field1declare, __VA_ARGS__)) \
  };                                                                                                                                           \
  struct TypeDeclare##Val {                                                                                                                    \
    const field1type field1declare;                                                                                                            \
  };                                                                                                                                           \
  MACRO_BASE__EXPAND(MACRO_BASE__S3_D2_PASTE(ENUM_UTIL_1_FIELD_DEF_GET_VAL_OBJ, TypeDeclare, field1type, field1declare, __VA_ARGS__))          \
                                                                                                                                               \
  static field1type enum_to_##field1declare(TypeDeclare input) {                                                                               \
    MACRO_BASE__EXPAND(MACRO_BASE__S3_D2_PASTE(ENUM_UTIL_1_FIELD_DEF_ENUM_TO_DECLARE, TypeDeclare, field1type, field1declare, __VA_ARGS__))    \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)"));                                                              \
  }                                                                                                                                            \
  static std::optional<TypeDeclare> field1declare##_to_enum(field1type& input) {                                                               \
    MACRO_BASE__EXPAND(MACRO_BASE__S3_D2_PASTE(ENUM_UTIL_1_FIELD_DEF_DECLARE_TO_ENUM, TypeDeclare, field1type, field1declare, __VA_ARGS__))    \
    return std::nullopt;                                                                                                                       \
  }
/* endregion ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT */

/* region ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT */
/**
 * @brief 用于生成枚举类型 , 后缀 [__WITH_DEF_VAL] 表示生成的枚举不指定值
 *
 * @code RAW
 *   ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
 *       AnimalWithDefVal,
 *       std::string, name,
 *       DOG, "dog",
 *       CAT, "cat")
 * @endcode
 *
 * @code CONVERT RESULT
 *   struct AnimalWithDefVal {
 *     enum Enum { DOG,
 *                 CAT,
 *     };
 *     struct Val {
 *       const std::string name;
 *     };
 *     static const Val& val_DOG() {
 *       static const Val val{"dog"};
 *       return val;
 *     }
 *     static const Val& val_CAT() {
 *       static const Val val{"cat"};
 *       return val;
 *     }
 *     static std::string enum_to_name(Enum input) {
 *       if (input == DOG) return val_DOG().name;
 *       if (input == CAT) return val_CAT().name;
 *       throw std::logic_error(
 *          WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)"));
 *     }
 *     static std::optional<Enum> name_to_enum(std::string& input) {
 *       if (input == val_DOG().name) return DOG;
 *       if (input == val_CAT().name) return CAT;
 *       return std::nullopt;
 *     }
 *   }
 * @endcode
 * */
#define ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT( \
    TypeDeclare,                                       \
    field1type, field1declare,                         \
    /*enumKey, type1val*/...)                          \
                                                       \
  struct TypeDeclare {                                 \
    ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE(               \
        Enum,                                          \
        field1type, field1declare, __VA_ARGS__)        \
  };
  /* endregion ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT */

}  // namespace nameless_carpool

/* enum 单参数 指定值 */
/**
 * @code RAW
 *   ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
 *       Animal, std::string, name,
 *       DOG, 1, "dog",
 *       CAT, 2, "cat")
 * @endcode
 *
 * @code CONVERT RESULT
 *   struct Animal {
 *     enum Enum { DOG = 1,
 *                 CAT = 2,
 *     };
 *     struct Val {
 *       const std::string name;
 *     };
 *     static const Val& val_DOG() {
 *       static const Val val{"dog"};
 *       return val;
 *     }
 *     static const Val& val_CAT() {
 *       static const Val val{"cat"};
 *       return val;
 *     }
 *     static std::string enum_to_name(Enum input) {
 *       if (input == DOG) return val_DOG().name;
 *       if (input == CAT) return val_CAT().name;
 *           throw std::logic_error(
 *             WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)")) );
 *     }
 *     static std::optional<Enum> name_to_enum(std::string& input) {
 *           if (input == val_DOG().name) return DOG;
 *           if (input == val_CAT().name) return CAT;
 *           return std::nullopt;
 *     }
 *   };
 * @endcode
 */
namespace nameless_carpool {

/* region sub */
#define ENUM_UTIL_1_FIELD_SPECIFY_GENERATE_ENUM_ITEM(TypeDeclare, field1type, field1declare, enumKey, enumVal, type1val) enumKey = enumVal,
#define ENUM_UTIL_1_FIELD_SPECIFY_GET_VAL_OBJ(TypeDeclare, field1type, field1declare, enumKey, enumVal, type1val) \
  static const TypeDeclare##Val& val_##enumKey() {                                                                \
    static const TypeDeclare##Val val{type1val};                                                                  \
    return val;                                                                                                   \
  }
#define ENUM_UTIL_1_FIELD_SPECIFY_ENUM_TO_DECLARE(TypeDeclare, field1type, field1declare, enumKey, enumVal, type1val) \
  if (input == enumKey) return val_##enumKey().field1declare;
#define ENUM_UTIL_1_FIELD_SPECIFY_DECLARE_TO_ENUM(TypeDeclare, field1type, field1declare, enumKey, enumVal, type1val) \
  if (input == val_##enumKey().field1declare) return enumKey;
/* endregion sub */

/* region ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE  */
/**
 * @code RAW
 *    ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE(
 *        Animal,
 *        std::string, name,
 *        DOG, 1, "dog",
 *        CAT, 2, "cat")
 * @endcode
 *
 * @code CONVERT RESULT
 *    enum Animal { DOG = 1,
 *                CAT = 2,
 *    };
 *    struct Val {
 *      const std::string name;
 *    };
 *    static const Val& val_DOG() {
 *      static const Val val{"dog"};
 *      return val;
 *    }
 *    static const Val& val_CAT() {
 *      static const Val val{"cat"};
 *      return val;
 *    }
 *    static std::string enum_to_name(Animal input) {
 *      if (input == DOG) return val_DOG().name;
 *      if (input == CAT) return val_CAT().name;
 *      throw std::logic_error(
 *          WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)"));
 *    }
 *    static std::optional<Animal> name_to_enum(std::string& input) {
 *      if (input == val_DOG().name) return DOG;
 *      if (input == val_CAT().name) return CAT;
 *      return std::nullopt;
 *    }
 * @endcode
 * */
#define ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE(TypeDeclare, field1type, field1declare, ...)                                                       \
  enum TypeDeclare {                                                                                                                               \
    MACRO_BASE__EXPAND(MACRO_BASE__S3_D3_PASTE(ENUM_UTIL_1_FIELD_SPECIFY_GENERATE_ENUM_ITEM, TypeDeclare, field1type, field1declare, __VA_ARGS__)) \
  };                                                                                                                                               \
  struct TypeDeclare##Val {                                                                                                                        \
    const field1type field1declare;                                                                                                                \
  };                                                                                                                                               \
  MACRO_BASE__EXPAND(MACRO_BASE__S3_D3_PASTE(ENUM_UTIL_1_FIELD_SPECIFY_GET_VAL_OBJ, TypeDeclare, field1type, field1declare, __VA_ARGS__))          \
                                                                                                                                                   \
  static field1type enum_to_##field1declare(TypeDeclare input) {                                                                                   \
    MACRO_BASE__EXPAND(MACRO_BASE__S3_D3_PASTE(ENUM_UTIL_1_FIELD_SPECIFY_ENUM_TO_DECLARE, TypeDeclare, field1type, field1declare, __VA_ARGS__))    \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)"));                                                                  \
  }                                                                                                                                                \
  static std::optional<TypeDeclare> field1declare##_to_enum(field1type& input) {                                                                   \
    MACRO_BASE__EXPAND(MACRO_BASE__S3_D3_PASTE(ENUM_UTIL_1_FIELD_SPECIFY_DECLARE_TO_ENUM, TypeDeclare, field1type, field1declare, __VA_ARGS__))    \
    return std::nullopt;                                                                                                                           \
  }
/* endregion ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE  */

/* region ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT  */
/**
 * @code RAW
 *   ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
 *       Animal, std::string, name,
 *       DOG, 1, "dog",
 *       CAT, 2, "cat")
 * @endcode
 *
 * @code CONVERT RESULT
 *   struct Animal {
 *     enum Enum { DOG = 1,
 *                 CAT = 2,
 *     };
 *     struct Val {
 *       const std::string name;
 *     };
 *     static const Val& val_DOG() {
 *       static const Val val{"dog"};
 *       return val;
 *     }
 *     static const Val& val_CAT() {
 *       static const Val val{"cat"};
 *       return val;
 *     }
 *     static std::string enum_to_name(Enum input) {
 *       if (input == DOG) return val_DOG().name;
 *       if (input == CAT) return val_CAT().name;
 *           throw std::logic_error(
 *             WITH_LINE_INFO("unknow error (2023-03-28 21:01:38)")) );
 *     }
 *     static std::optional<Enum> name_to_enum(std::string& input) {
 *           if (input == val_DOG().name) return DOG;
 *           if (input == val_CAT().name) return CAT;
 *           return std::nullopt;
 *     }
 *   };
 * @endcode
 */
#define ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(TypeDeclare, field1type, field1declare, ...) \
  struct TypeDeclare {                                                                                 \
    ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE(Enum, field1type, field1declare, __VA_ARGS__)              \
  };
  /* endregion ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT  */
}  // namespace nameless_carpool

/* enmu 双参数 默认值 */
/**
 *  @code RAW
 *    ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
 *        Animal2WithDefVal,
 *        std::string, name, std::string, desc,
 *        DOG, "dog", "这表示一条狗",
 *        CAT, "cat", "这表示一只猫")
 *  @endcode
 *
 *  @code CONVERT RESULT
 *    struct Animal2WithDefVal {
 *      enum Enum { DOG,
 *                  CAT,
 *      };
 *      struct Val {
 *        const std::string name;
 *        const std::string desc;
 *      };
 *      static const Val& val_DOG() {
 *        static const Val val{"dog", "这表示一条狗"};
 *        return val;
 *      }
 *      static const Val& val_CAT() {
 *        static const Val val{"cat", "这表示一只猫"};
 *        return val;
 *      }
 *      static std::string enum_to_name(Enum input) {
 *        if (input == DOG) return val_DOG().name;
 *        if (input == CAT) return val_CAT().name;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));
 *      }
 *      static std::string enum_to_desc(Enum input) {
 *        if (input == DOG) return val_DOG().desc;
 *        if (input == CAT) return val_CAT().desc;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *      }
 *      static std::optional<Enum> name_to_enum(std::string& input) {
 *        if (input == val_DOG().name) return DOG;
 *        if (input == val_CAT().name) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> desc_to_enum(std::string& input) {
 *        if (input == val_DOG().desc) return DOG;
 *        if (input == val_CAT().desc) return CAT;
 *        return std::nullopt;
 *      }
 *    };
 *  @endcode
 */
namespace nameless_carpool {

/* region sub */
#define ENUM_UTIL_2_FIELD_DEF_GENERATE_ENUM_ITEM(         \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, field1val, field2val) enumKey,

#define ENUM_UTIL_2_FIELD_DEF_GET_VAL_OBJ(                   \
    TypeDeclare,                                             \
    field1type, field1declare, field2type, field2declare,    \
    enumKey, field1val, field2val)                           \
  static const TypeDeclare##Val& val_##enumKey() {           \
    static const TypeDeclare##Val val{field1val, field2val}; \
    return val;                                              \
  }

#define ENUM_UTIL_2_FIELD_DEF_ENUM_TO_DECLARE_1(          \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, field1val, field2val)                        \
  if (input == enumKey) return val_##enumKey().field1declare;
#define ENUM_UTIL_2_FIELD_DEF_ENUM_TO_DECLARE_2(          \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, field1val, field2val)                        \
  if (input == enumKey) return val_##enumKey().field2declare;

#define ENUM_UTIL_2_FIELD_DEF_DECLARE_1_TO_ENUM(          \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, field1val, field2val)                        \
  if (input == val_##enumKey().field1declare) return enumKey;
#define ENUM_UTIL_2_FIELD_DEF_DECLARE_2_TO_ENUM(          \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, field1val, field2val)                        \
  if (input == val_##enumKey().field2declare) return enumKey;
/* endregion sub */

/* region ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE */
/**
 * @code RAW
 *  ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE(
 *      Animal2WithDefVal_,
 *      std::string, name, bool, desc,
 *      DOG, "dog", true,
 *      CAT, "cat", false)
 * @endcode
 *
 * @code CONVERT RESULT
 *  enum Animal2WithDefVal_ { DOG,
 *                            CAT,
 *  };
 *  struct Val {
 *    const std::string name;
 *    const bool        desc;
 *  };
 *  static const Val& val_DOG() {
 *    static const Val val{"dog", true};
 *    return val;
 *  }
 *  static const Val& val_CAT() {
 *    static const Val val{"cat", false};
 *    return val;
 *  }
 *  static std::string enum_to_name(Animal2WithDefVal_ input) {
 *    if (input == DOG) return val_DOG().name;
 *    if (input == CAT) return val_CAT().name;
 *    throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));
 *  }
 *  static bool enum_to_desc(Animal2WithDefVal_ input) {
 *    if (input == DOG) return val_DOG().desc;
 *    if (input == CAT) return val_CAT().desc;
 *    throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *  }
 *  static std::optional<Animal2WithDefVal_> name_to_enum(std::string& input) {
 *    if (input == val_DOG().name) return DOG;
 *    if (input == val_CAT().name) return CAT;
 *    return std::nullopt;
 *  }
 *  static std::optional<Animal2WithDefVal_> desc_to_enum(bool& input) {
 *    if (input == val_DOG().desc) return DOG;
 *    if (input == val_CAT().desc) return CAT;
 *    return std::nullopt;
 *  }
 *
 *@endcode
 * */
#define ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE(                                                  \
    TypeDeclare,                                                                              \
    field1type, field1declare,                                                                \
    field2type, field2declare, /* enumKey, enumVal, field1val, field2val*/                    \
    ...)                                                                                      \
  enum TypeDeclare {                                                                          \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D3_PASTE(                                               \
        ENUM_UTIL_2_FIELD_DEF_GENERATE_ENUM_ITEM,                                             \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__)) /**/ \
  };                                                                                          \
  struct TypeDeclare##Val {                                                                   \
    const field1type field1declare;                                                           \
    const field2type field2declare;                                                           \
  };                                                                                          \
                                                                                              \
  MACRO_BASE__EXPAND(MACRO_BASE__S5_D3_PASTE(                                                 \
      ENUM_UTIL_2_FIELD_DEF_GET_VAL_OBJ,                                                      \
      TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))        \
  static field1type enum_to_##field1declare(TypeDeclare input) {                              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D3_PASTE(                                               \
        ENUM_UTIL_2_FIELD_DEF_ENUM_TO_DECLARE_1,                                              \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));             \
  }                                                                                           \
  static field2type enum_to_##field2declare(TypeDeclare input) {                              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D3_PASTE(                                               \
        ENUM_UTIL_2_FIELD_DEF_ENUM_TO_DECLARE_2,                                              \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));             \
  }                                                                                           \
                                                                                              \
  static std::optional<TypeDeclare> field1declare##_to_enum(field1type& input) {              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D3_PASTE(                                               \
        ENUM_UTIL_2_FIELD_DEF_DECLARE_1_TO_ENUM,                                              \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    return std::nullopt;                                                                      \
  }                                                                                           \
  static std::optional<TypeDeclare> field2declare##_to_enum(field2type& input) {              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D3_PASTE(                                               \
        ENUM_UTIL_2_FIELD_DEF_DECLARE_2_TO_ENUM,                                              \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    return std::nullopt;                                                                      \
  }
/* endregion ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE */

/* region ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE_IN_STRUCT */
/**
 *  @code RAW
 *    ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
 *        Animal2WithDefVal,
 *        std::string, name, std::string, desc,
 *        DOG, "dog", "这表示一条狗",
 *        CAT, "cat", "这表示一只猫")
 *  @endcode
 *
 *  @code CONVERT RESULT
 *    struct Animal2WithDefVal {
 *      enum Enum { DOG,
 *                  CAT,
 *      };
 *      struct Val {
 *        const std::string name;
 *        const std::string desc;
 *      };
 *      static const Val& val_DOG() {
 *        static const Val val{"dog", "这表示一条狗"};
 *        return val;
 *      }
 *      static const Val& val_CAT() {
 *        static const Val val{"cat", "这表示一只猫"};
 *        return val;
 *      }
 *      static std::string enum_to_name(Enum input) {
 *        if (input == DOG) return val_DOG().name;
 *        if (input == CAT) return val_CAT().name;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));
 *      }
 *      static std::string enum_to_desc(Enum input) {
 *        if (input == DOG) return val_DOG().desc;
 *        if (input == CAT) return val_CAT().desc;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *      }
 *      static std::optional<Enum> name_to_enum(std::string& input) {
 *        if (input == val_DOG().name) return DOG;
 *        if (input == val_CAT().name) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> desc_to_enum(std::string& input) {
 *        if (input == val_DOG().desc) return DOG;
 *        if (input == val_CAT().desc) return CAT;
 *        return std::nullopt;
 *      }
 *    };
 *  @endcode
 */
#define ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(                         \
    TypeDeclare,                                                               \
    field1type, field1declare,                                                 \
    field2type, field2declare, /* enumKey, enumVal, field1val, field2val*/     \
    ...)                                                                       \
  struct TypeDeclare {                                                         \
    ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE(                                       \
        Enum,                                                                  \
        field1type, field1declare,                                             \
        field2type, field2declare, /* enumKey, enumVal, field1val, field2val*/ \
        __VA_ARGS__)                                                           \
  };
  /* endregion ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE_IN_STRUCT */

}  // namespace nameless_carpool

/* enmu 双参数 指定值 */
/**
  @code RAW
    ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
        Animal2,
        std::string, name, std::string, desc,
        DOG, 1, "dog", "这表示一条狗",
        CAT, 2, "cat", "这表示一只猫")
  @endcode

@code CONVERT RESULT
    struct Animal2 {
  enum Enum { DOG = 1,
              CAT = 2,
  };
  struct Val {
    const std::string name;
    const std::string desc;
  };
  static const Val& val_DOG() {
    static const Val val{"dog", "这表示一条狗"};
    return val;
  }
  static const Val& val_CAT() {
    static const Val val{"cat", "这表示一只猫"};
    return val;
  }
  static std::string enum_to_name(Enum input) {
    if (input == DOG) return val_DOG().name;
    if (input == CAT) return val_CAT().name;
    throw std::logic_error(
        WITH_LINE_INFO( "unknow error (2023-03-28 22:18:38)"));
  }
  static std::string enum_to_desc(Enum input) {
    if (input == DOG) return val_DOG().desc;
    if (input == CAT) return val_CAT().desc;
    throw std::logic_error(
        WITH_LINE_INFO( "unknow error (2023-03-28 22:18:47)"));
  }
  static std::optional<Enum> name_to_enum(std::string& input) {
    if (input == val_DOG().name) return DOG;
    if (input == val_CAT().name) return CAT;
    return std::nullopt;
  }
  static std::optional<Enum> desc_to_enum(std::string& input) {
    if (input == val_DOG().desc) return DOG;
    if (input == val_CAT().desc) return CAT;
    return std::nullopt;
  }
};
@endcode





        */
namespace nameless_carpool {
/* region sub */
#define ENUM_UTIL_2_FIELD_SPECIFY_GENERATE_ENUM_ITEM(     \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, enumVal, field1val, field2val) enumKey = enumVal,

#define ENUM_UTIL_2_FIELD_SPECIFY_GET_VAL_OBJ(               \
    TypeDeclare,                                             \
    field1type, field1declare, field2type, field2declare,    \
    enumKey, enumVal, field1val, field2val)                  \
  static const TypeDeclare##Val& val_##enumKey() {           \
    static const TypeDeclare##Val val{field1val, field2val}; \
    return val;                                              \
  }

#define ENUM_UTIL_2_FIELD_SPECIFY_ENUM_TO_DECLARE_1(      \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, enumVal, field1val, field2val)               \
  if (input == enumKey) return val_##enumKey().field1declare;
#define ENUM_UTIL_2_FIELD_SPECIFY_ENUM_TO_DECLARE_2(      \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, enumVal, field1val, field2val)               \
  if (input == enumKey) return val_##enumKey().field2declare;

#define ENUM_UTIL_2_FIELD_SPECIFY_DECLARE_1_TO_ENUM(      \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, enumVal, field1val, field2val)               \
  if (input == val_##enumKey().field1declare) return enumKey;
#define ENUM_UTIL_2_FIELD_SPECIFY_DECLARE_2_TO_ENUM(      \
    TypeDeclare,                                          \
    field1type, field1declare, field2type, field2declare, \
    enumKey, enumVal, field1val, field2val)               \
  if (input == val_##enumKey().field2declare) return enumKey;
/* endregion sub */

/* region ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE  */
/**
 *@code raw
 *  ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE(
 *      Animal2__,
 *      std::string, name, std::string, desc,
 *      DOG, 1, "dog", "这表示一条狗",
 *      CAT, 2, "cat", "这表示一只猫")
 *@endcode
 *
 *@code convert result
 *  enum Animal2__ { DOG = 1,
 *                   CAT = 2,
 *  };
 *  struct Val {
 *    const std::string name;
 *    const std::string desc;
 *  };
 *  static const Val& val_DOG() {
 *    static const Val val{"dog", "这表示一条狗"};
 *    return val;
 *  }
 *  static const Val& val_CAT() {
 *    static const Val val{"cat", "这表示一只猫"};
 *    return val;
 *  }
 *  static std::string enum_to_name(Animal2__ input) {
 *    if (input == DOG) return val_DOG().name;
 *    if (input == CAT) return val_CAT().name;
 *    throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));
 *  }
 *  static std::string enum_to_desc(Animal2__ input) {
 *    if (input == DOG) return val_DOG().desc;
 *    if (input == CAT) return val_CAT().desc;
 *    throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *  }
 *  static std::optional<Animal2__> name_to_enum(std::string& input) {
 *    if (input == val_DOG().name) return DOG;
 *    if (input == val_CAT().name) return CAT;
 *    return std::nullopt;
 *  }
 *  static std::optional<Animal2__> desc_to_enum(std::string& input) {
 *    if (input == val_DOG().desc) return DOG;
 *    if (input == val_CAT().desc) return CAT;
 *    return std::nullopt;
 *  }
 *@endcode
 * */
#define ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE(                                              \
    TypeDeclare,                                                                              \
    field1type, field1declare,                                                                \
    field2type, field2declare, /* enumKey, enumVal, field1val, field2val*/                    \
    ...)                                                                                      \
  enum TypeDeclare {                                                                          \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D4_PASTE(                                               \
        ENUM_UTIL_2_FIELD_SPECIFY_GENERATE_ENUM_ITEM,                                         \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__)) /**/ \
  };                                                                                          \
                                                                                              \
  struct TypeDeclare##Val {                                                                   \
    const field1type field1declare;                                                           \
    const field2type field2declare;                                                           \
  };                                                                                          \
                                                                                              \
  MACRO_BASE__EXPAND(MACRO_BASE__S5_D4_PASTE(                                                 \
      ENUM_UTIL_2_FIELD_SPECIFY_GET_VAL_OBJ,                                                  \
      TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))        \
  static field1type enum_to_##field1declare(TypeDeclare input) {                              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D4_PASTE(                                               \
        ENUM_UTIL_2_FIELD_SPECIFY_ENUM_TO_DECLARE_1,                                          \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));             \
  }                                                                                           \
  static field2type enum_to_##field2declare(TypeDeclare input) {                              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D4_PASTE(                                               \
        ENUM_UTIL_2_FIELD_SPECIFY_ENUM_TO_DECLARE_2,                                          \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));             \
  }                                                                                           \
                                                                                              \
  static std::optional<TypeDeclare> field1declare##_to_enum(field1type& input) {              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D4_PASTE(                                               \
        ENUM_UTIL_2_FIELD_SPECIFY_DECLARE_1_TO_ENUM,                                          \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    return std::nullopt;                                                                      \
  }                                                                                           \
  static std::optional<TypeDeclare> field2declare##_to_enum(field2type& input) {              \
    MACRO_BASE__EXPAND(MACRO_BASE__S5_D4_PASTE(                                               \
        ENUM_UTIL_2_FIELD_SPECIFY_DECLARE_2_TO_ENUM,                                          \
        TypeDeclare, field1type, field1declare, field2type, field2declare, __VA_ARGS__))      \
    return std::nullopt;                                                                      \
  }
/* endregion ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE*/

/* region ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT*/
/**
 *  @code RAW
 *    ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
 *        Animal2,
 *        std::string, name, std::string, desc,
 *        DOG, 1, "dog", "这表示一条狗",
 *        CAT, 2, "cat", "这表示一只猫")
 *  @endcode
 *
 *  @code CONVERT RESULT
 *    struct Animal2 {
 *      enum Enum { DOG = 1,
 *                  CAT = 2,
 *      };
 *      struct Val {
 *        const std::string name;
 *        const std::string desc;
 *      };
 *      static const Val& val_DOG() {
 *        static const Val val{"dog", "这表示一条狗"};
 *        return val;
 *      }
 *      static const Val& val_CAT() {
 *        static const Val val{"cat", "这表示一只猫"};
 *        return val;
 *      }
 *      static std::string enum_to_name(Enum input) {
 *        if (input == DOG) return val_DOG().name;
 *        if (input == CAT) return val_CAT().name;
 *        throw std::logic_error(
 *            WITH_LINE_INFO( "unknow error (2023-03-28 22:18:38)"));
 *      }
 *      static std::string enum_to_desc(Enum input) {
 *        if (input == DOG) return val_DOG().desc;
 *        if (input == CAT) return val_CAT().desc;
 *        throw std::logic_error(
 *            WITH_LINE_INFO( "unknow error (2023-03-28 22:18:47)"));
 *      }
 *      static std::optional<Enum> name_to_enum(std::string& input) {
 *        if (input == val_DOG().name) return DOG;
 *        if (input == val_CAT().name) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> desc_to_enum(std::string& input) {
 *        if (input == val_DOG().desc) return DOG;
 *        if (input == val_CAT().desc) return CAT;
 *        return std::nullopt;
 *      }
 *    };
 *  @endcode
 */
#define ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(                 \
    TypeDeclare,                                                           \
    field1type, field1declare,                                             \
    field2type, field2declare, /* enumKey, enumVal, field1val, field2val*/ \
    ...)                                                                   \
  struct TypeDeclare {                                                     \
    ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE(                               \
        Enum,                                                              \
        field1type, field1declare,                                         \
        field2type, field2declare, __VA_ARGS__)                            \
  };
  /* endregion ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT*/

}  // namespace nameless_carpool

/* enmu 三参数 默认值 */
/**
 *  @code RAW
 *   ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
 *       Animal3,
 *       std::string, name, double, desc, int, other,
 *       DOG, "dog", "这表示一条狗", 123,
 *       CAT, "cat", "这表示一只猫", 456)
 *  @endcode
 *
 *  @code CONVERT RESULT
 *    struct Animal3 {
 *      enum Enum { DOG,
 *                  CAT,
 *      };
 *      struct Val {
 *        const std::string name;
 *        const double      desc;
 *        const int         other;
 *      };
 *      static const Val& val_DOG() {
 *        static const Val val{"dog", "这表示一条狗", 123};
 *        return val;
 *      }
 *      static const Val& val_CAT() {
 *        static const Val val{"cat", "这表示一只猫", 456};
 *        return val;
 *      }
 *      static std::string enum_to_name(Enum input) {
 *        if (input == DOG) return val_DOG().name;
 *        if (input == CAT) return val_CAT().name;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:44)"));
 *      }
 *      static double enum_to_desc(Enum input) {
 *        if (input == DOG) return val_DOG().desc;
 *        if (input == CAT) return val_CAT().desc;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:50)"));
 *      }
 *      static int enum_to_other(Enum input) {
 *        if (input == DOG) return val_DOG().other;
 *        if (input == CAT) return val_CAT().other;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:55)"));
 *      }
 *      static std::optional<Enum> name_to_enum(std::string& input) {
 *        if (input == val_DOG().name) return DOG;
 *        if (input == val_CAT().name) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> desc_to_enum(double& input) {
 *        if (input == val_DOG().desc) return DOG;
 *        if (input == val_CAT().desc) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> other_to_enum(int& input) {
 *        if (input == val_DOG().other) return DOG;
 *        if (input == val_CAT().other) return CAT;
 *        return std::nullopt;
 *      }
 *    };
 *  @endcode
 */
namespace nameless_carpool {
/* region sub */
#define ENUM_UTIL_3_FIELD_DEF_GENERATE_ENUM_ITEM( \
    TypeDeclare,                                  \
    field1type, field1declare,                    \
    field2type, field2declare,                    \
    field3type, field3declare,                    \
    enumKey, field1val, field2val, field3val) enumKey,

#define ENUM_UTIL_3_FIELD_DEF_GET_VAL_OBJ(                              \
    TypeDeclare,                                                        \
    field1type, field1declare,                                          \
    field2type, field2declare,                                          \
    field3type, field3declare,                                          \
    enumKey, field1val, field2val, field3val)                           \
  static const TypeDeclare##Val& val_##enumKey() {                      \
    static const TypeDeclare##Val val{field1val, field2val, field3val}; \
    return val;                                                         \
  }

#define ENUM_UTIL_3_FIELD_DEF_ENUM_TO_DECLARE_1( \
    TypeDeclare,                                 \
    field1type, field1declare,                   \
    field2type, field2declare,                   \
    field3type, field3declare,                   \
    enumKey, field1val, field2val, field3val)    \
  if (input == enumKey) return val_##enumKey().field1declare;
#define ENUM_UTIL_3_FIELD_DEF_ENUM_TO_DECLARE_2( \
    TypeDeclare,                                 \
    field1type, field1declare,                   \
    field2type, field2declare,                   \
    field3type, field3declare,                   \
    enumKey, field1val, field2val, field3val)    \
  if (input == enumKey) return val_##enumKey().field2declare;
#define ENUM_UTIL_3_FIELD_DEF_ENUM_TO_DECLARE_3( \
    TypeDeclare,                                 \
    field1type, field1declare,                   \
    field2type, field2declare,                   \
    field3type, field3declare,                   \
    enumKey, field1val, field2val, field3val)    \
  if (input == enumKey) return val_##enumKey().field3declare;

#define ENUM_UTIL_3_FIELD_DEF_DECLARE_1_TO_ENUM( \
    TypeDeclare,                                 \
    field1type, field1declare,                   \
    field2type, field2declare,                   \
    field3type, field3declare,                   \
    enumKey, field1val, field2val, field3val)    \
  if (input == val_##enumKey().field1declare) return enumKey;
#define ENUM_UTIL_3_FIELD_DEF_DECLARE_2_TO_ENUM( \
    TypeDeclare,                                 \
    field1type, field1declare,                   \
    field2type, field2declare,                   \
    field3type, field3declare,                   \
    enumKey, field1val, field2val, field3val)    \
  if (input == val_##enumKey().field2declare) return enumKey;
#define ENUM_UTIL_3_FIELD_DEF_DECLARE_3_TO_ENUM( \
    TypeDeclare,                                 \
    field1type, field1declare,                   \
    field2type, field2declare,                   \
    field3type, field3declare,                   \
    enumKey, field1val, field2val, field3val)    \
  if (input == val_##enumKey().field3declare) return enumKey;
/* endregion sub */

/* region ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE  */

/**
 * @code RAW
 *  ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE(
 *      Animal3Def_,
 *      std::string, name, double, desc, int, other,
 *      DOG, "dog", 1.123, 123,
 *      CAT, "cat", 5.234, 456)
 * @endcode
 *
 *
 * @code CONVERT RESULT
 *  enum Animal3Def_ { DOG,
 *                     CAT,
 *  };
 *  struct Val {
 *    const std::string name;
 *    const double      desc;
 *    const int         other;
 *  };
 *  static const Val& val_DOG() {
 *    static const Val val{"dog", 1.123, 123};
 *    return val;
 *  }
 *  static const Val& val_CAT() {
 *    static const Val val{"cat", 5.234, 456};
 *    return val;
 *  }
 *  static std::string enum_to_name(Animal3Def_ input) {
 *    if (input == DOG) return val_DOG().name;
 *    if (input == CAT) return val_CAT().name;
 *    throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-29 09:01:44)"));
 *  }
 *  static double enum_to_desc(Animal3Def_ input) {
 *    if (input == DOG) return val_DOG().desc;
 *    if (input == CAT) return val_CAT().desc;
 *    throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-29 09:01:50)"));
 *  }
 *  static int enum_to_other(Animal3Def_ input) {
 *    if (input == DOG) return val_DOG().other;
 *    if (input == CAT) return val_CAT().other;
 *    throw std::logic_error(
 *      WITH_LINE_INFO("unknow error (2023-03-29 09:01:55)"));
 *  }
 *  static std::optional<Animal3Def_> name_to_enum(std::string& input) {
 *    if (input == val_DOG().name) return DOG;
 *    if (input == val_CAT().name) return CAT;
 *    return std::nullopt;
 *  }
 *  static std::optional<Animal3Def_> desc_to_enum(double& input) {
 *    if (input == val_DOG().desc) return DOG;
 *    if (input == val_CAT().desc) return CAT;
 *    return std::nullopt;
 *  }
 *  static std::optional<Animal3Def_> other_to_enum(int& input) {
 *    if (input == val_DOG().other) return DOG;
 *    if (input == val_CAT().other) return CAT;
 *    return std::nullopt;
 *  }
 * @endcode
 */
#define ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE(                                      \
    TypeDeclare,                                                                  \
    field1type, field1declare,                                                    \
    field2type, field2declare,                                                    \
    field3type, field3declare, ...)                                               \
                                                                                  \
  enum TypeDeclare {                                                              \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                   \
        ENUM_UTIL_3_FIELD_DEF_GENERATE_ENUM_ITEM,                                 \
        TypeDeclare,                                                              \
        field1type, field1declare,                                                \
        field2type, field2declare,                                                \
        field3type, field3declare, __VA_ARGS__)) /**/                             \
  };                                                                              \
                                                                                  \
  struct TypeDeclare##Val {                                                       \
    const field1type field1declare;                                               \
    const field2type field2declare;                                               \
    const field3type field3declare;                                               \
  };                                                                              \
                                                                                  \
  MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                     \
      ENUM_UTIL_3_FIELD_DEF_GET_VAL_OBJ,                                          \
      TypeDeclare,                                                                \
      field1type, field1declare,                                                  \
      field2type, field2declare,                                                  \
      field3type, field3declare, __VA_ARGS__))                                    \
                                                                                  \
  static field1type enum_to_##field1declare(TypeDeclare input) {                  \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                   \
        ENUM_UTIL_3_FIELD_DEF_ENUM_TO_DECLARE_1,                                  \
        TypeDeclare,                                                              \
        field1type, field1declare,                                                \
        field2type, field2declare,                                                \
        field3type, field3declare, __VA_ARGS__))                                  \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:44)")); \
  }                                                                               \
  static field2type enum_to_##field2declare(TypeDeclare input) {                  \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                   \
        ENUM_UTIL_3_FIELD_DEF_ENUM_TO_DECLARE_2,                                  \
        TypeDeclare,                                                              \
        field1type, field1declare,                                                \
        field2type, field2declare,                                                \
        field3type, field3declare, __VA_ARGS__))                                  \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:50)")); \
  }                                                                               \
  static field3type enum_to_##field3declare(TypeDeclare input) {                  \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                   \
        ENUM_UTIL_3_FIELD_DEF_ENUM_TO_DECLARE_3,                                  \
        TypeDeclare,                                                              \
        field1type, field1declare,                                                \
        field2type, field2declare,                                                \
        field3type, field3declare, __VA_ARGS__))                                  \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:55)")); \
  }                                                                               \
                                                                                  \
  static std::optional<TypeDeclare> field1declare##_to_enum(field1type& input) {  \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                   \
        ENUM_UTIL_3_FIELD_DEF_DECLARE_1_TO_ENUM,                                  \
        TypeDeclare,                                                              \
        field1type, field1declare,                                                \
        field2type, field2declare,                                                \
        field3type, field3declare, __VA_ARGS__))                                  \
    return std::nullopt;                                                          \
  }                                                                               \
  static std::optional<TypeDeclare> field2declare##_to_enum(field2type& input) {  \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                   \
        ENUM_UTIL_3_FIELD_DEF_DECLARE_2_TO_ENUM,                                  \
        TypeDeclare,                                                              \
        field1type, field1declare,                                                \
        field2type, field2declare,                                                \
        field3type, field3declare, __VA_ARGS__))                                  \
    return std::nullopt;                                                          \
  }                                                                               \
  static std::optional<TypeDeclare> field3declare##_to_enum(field3type& input) {  \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D4_PASTE(                                   \
        ENUM_UTIL_3_FIELD_DEF_DECLARE_3_TO_ENUM,                                  \
        TypeDeclare,                                                              \
        field1type, field1declare,                                                \
        field2type, field2declare,                                                \
        field3type, field3declare, __VA_ARGS__))                                  \
    return std::nullopt;                                                          \
  }
/* endregion ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE  */

/* region ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE_IN_STRUCT  */
/**
 *  @code RAW
 *   ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
 *       Animal3,
 *       std::string, name, double, desc, int, other,
 *       DOG, "dog", "这表示一条狗", 123,
 *       CAT, "cat", "这表示一只猫", 456)
 *  @endcode
 *
 *  @code CONVERT RESULT
 *    struct Animal3 {
 *      enum Enum { DOG,
 *                  CAT,
 *      };
 *      struct Val {
 *        const std::string name;
 *        const double      desc;
 *        const int         other;
 *      };
 *      static const Val& val_DOG() {
 *        static const Val val{"dog", "这表示一条狗", 123};
 *        return val;
 *      }
 *      static const Val& val_CAT() {
 *        static const Val val{"cat", "这表示一只猫", 456};
 *        return val;
 *      }
 *      static std::string enum_to_name(Enum input) {
 *        if (input == DOG) return val_DOG().name;
 *        if (input == CAT) return val_CAT().name;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:44)"));
 *      }
 *      static double enum_to_desc(Enum input) {
 *        if (input == DOG) return val_DOG().desc;
 *        if (input == CAT) return val_CAT().desc;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:50)"));
 *      }
 *      static int enum_to_other(Enum input) {
 *        if (input == DOG) return val_DOG().other;
 *        if (input == CAT) return val_CAT().other;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:55)"));
 *      }
 *      static std::optional<Enum> name_to_enum(std::string& input) {
 *        if (input == val_DOG().name) return DOG;
 *        if (input == val_CAT().name) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> desc_to_enum(double& input) {
 *        if (input == val_DOG().desc) return DOG;
 *        if (input == val_CAT().desc) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> other_to_enum(int& input) {
 *        if (input == val_DOG().other) return DOG;
 *        if (input == val_CAT().other) return CAT;
 *        return std::nullopt;
 *      }
 *    };
 *  @endcode
 */
#define ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(                         \
    TypeDeclare,                                                               \
    field1type, field1declare,                                                 \
    field2type, field2declare,                                                 \
    field3type, field3declare, /* enumKey,  field1val, field2val, field3val */ \
    ...)                                                                       \
  struct TypeDeclare {                                                         \
    ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE(                                       \
        Enum,                                                                  \
        field1type, field1declare,                                             \
        field2type, field2declare,                                             \
        field3type, field3declare, __VA_ARGS__)                                \
  };
  /* endregion ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE_IN_STRUCT  */

}  // namespace nameless_carpool

/* enmu 三参数 指定值 */
/**
 *  @code RAW
 *    ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
 *        Animal3,
 *        std::string, name, std::string, desc, int, other,
 *        DOG, 1, "dog", "这表示一条狗", 123,
 *        CAT, 2, "cat", "这表示一只猫", 456)
 *  @endcode
 *
 *  @code CONVERT RESULT
 *    struct Animal3 {
 *      enum Enum { DOG = 1,
 *                  CAT = 2,
 *      };
 *      struct Val {
 *        const std::string name;
 *        const std::string desc;
 *        const int         other;
 *      };
 *      static const Val& val_DOG() {
 *        static const Val val{"dog", "这表示一条狗", 123};
 *        return val;
 *      }
 *      static const Val& val_CAT() {
 *        static const Val val{"cat", "这表示一只猫", 456};
 *        return val;
 *      }
 *      static std::string enum_to_name(Enum input) {
 *        if (input == DOG) return val_DOG().name;
 *        if (input == CAT) return val_CAT().name;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));
 *      }
 *      static std::string enum_to_desc(Enum input) {
 *        if (input == DOG) return val_DOG().desc;
 *        if (input == CAT) return val_CAT().desc;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *      }
 *      static int enum_to_other(Enum input) {
 *        if (input == DOG) return val_DOG().other;
 *        if (input == CAT) return val_CAT().other;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *      }
 *      static std::optional<Enum> name_to_enum(std::string& input) {
 *        if (input == val_DOG().name) return DOG;
 *        if (input == val_CAT().name) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> desc_to_enum(std::string& input) {
 *        if (input == val_DOG().desc) return DOG;
 *        if (input == val_CAT().desc) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> other_to_enum(std::string& input) {
 *        if (input == val_DOG().other) return DOG;
 *        if (input == val_CAT().other) return CAT;
 *        return std::nullopt;
 *      }
 *    };
 *  @endcode
 */
namespace nameless_carpool {

/* region sub */
#define ENUM_UTIL_3_FIELD_SPECIFY_GENERATE_ENUM_ITEM( \
    TypeDeclare,                                      \
    field1type, field1declare,                        \
    field2type, field2declare,                        \
    field3type, field3declare,                        \
    enumKey, enumVal, field1val, field2val, field3val) enumKey = enumVal,

#define ENUM_UTIL_3_FIELD_SPECIFY_GET_VAL_OBJ(                          \
    TypeDeclare,                                                        \
    field1type, field1declare,                                          \
    field2type, field2declare,                                          \
    field3type, field3declare,                                          \
    enumKey, enumVal, field1val, field2val, field3val)                  \
  static const TypeDeclare##Val& val_##enumKey() {                      \
    static const TypeDeclare##Val val{field1val, field2val, field3val}; \
    return val;                                                         \
  }

#define ENUM_UTIL_3_FIELD_SPECIFY_ENUM_TO_DECLARE_1(   \
    TypeDeclare,                                       \
    field1type, field1declare,                         \
    field2type, field2declare,                         \
    field3type, field3declare,                         \
    enumKey, enumVal, field1val, field2val, field3val) \
  if (input == enumKey) return val_##enumKey().field1declare;
#define ENUM_UTIL_3_FIELD_SPECIFY_ENUM_TO_DECLARE_2(   \
    TypeDeclare,                                       \
    field1type, field1declare,                         \
    field2type, field2declare,                         \
    field3type, field3declare,                         \
    enumKey, enumVal, field1val, field2val, field3val) \
  if (input == enumKey) return val_##enumKey().field2declare;
#define ENUM_UTIL_3_FIELD_SPECIFY_ENUM_TO_DECLARE_3(   \
    TypeDeclare,                                       \
    field1type, field1declare,                         \
    field2type, field2declare,                         \
    field3type, field3declare,                         \
    enumKey, enumVal, field1val, field2val, field3val) \
  if (input == enumKey) return val_##enumKey().field3declare;

#define ENUM_UTIL_3_FIELD_SPECIFY_DECLARE_1_TO_ENUM(   \
    TypeDeclare,                                       \
    field1type, field1declare,                         \
    field2type, field2declare,                         \
    field3type, field3declare,                         \
    enumKey, enumVal, field1val, field2val, field3val) \
  if (input == val_##enumKey().field1declare) return enumKey;
#define ENUM_UTIL_3_FIELD_SPECIFY_DECLARE_2_TO_ENUM(   \
    TypeDeclare,                                       \
    field1type, field1declare,                         \
    field2type, field2declare,                         \
    field3type, field3declare,                         \
    enumKey, enumVal, field1val, field2val, field3val) \
  if (input == val_##enumKey().field2declare) return enumKey;
#define ENUM_UTIL_3_FIELD_SPECIFY_DECLARE_3_TO_ENUM(   \
    TypeDeclare,                                       \
    field1type, field1declare,                         \
    field2type, field2declare,                         \
    field3type, field3declare,                         \
    enumKey, enumVal, field1val, field2val, field3val) \
  if (input == val_##enumKey().field3declare) return enumKey;
/* endregion sub */

/* region ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE*/
/**
 * @code RAW
 *  ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE(
 *     Animal3_,
 *     std::string, name, std::string, desc, int, other,
 *     DOG, 1, "dog", "这表示一条狗", 123,
 *     CAT, 2, "cat", "这表示一只猫", 456)
 * @endcode
 *
 * @code CONVERT RESULT
 *   enum Animal3_ { DOG = 1,
 *                   CAT = 2,
 *   };
 *   struct Val {
 *     const std::string name;
 *     const std::string desc;
 *     const int         other;
 *   };
 *   static const Val& val_DOG() {
 *     static const Val val{"dog", "这表示一条狗", 123};
 *     return val;
 *   }
 *   static const Val& val_CAT() {
 *     static const Val val{"cat", "这表示一只猫", 456};
 *     return val;
 *   }
 *   static std::string enum_to_name(Animal3_ input) {
 *     if (input == DOG) return val_DOG().name;
 *     if (input == CAT) return val_CAT().name;
 *     throw std::logic_error(
 *       WITH_LINE_INFO( "unknow error (2023-03-29 09:01:44)"));
 *   }
 *   static std::string enum_to_desc(Animal3_ input) {
 *     if (input == DOG) return val_DOG().desc;
 *     if (input == CAT) return val_CAT().desc;
 *     throw std::logic_error(
 *       WITH_LINE_INFO( "unknow error (2023-03-29 09:01:50)"));
 *   }
 *   static int enum_to_other(Animal3_ input) {
 *     if (input == DOG) return val_DOG().other;
 *     if (input == CAT) return val_CAT().other;
 *     throw std::logic_error(
 *       WITH_LINE_INFO( "unknow error (2023-03-29 09:01:55)"));
 *   }
 *   static std::optional<Animal3_> name_to_enum(std::string& input) {
 *     if (input == val_DOG().name) return DOG;
 *     if (input == val_CAT().name) return CAT;
 *     return std::nullopt;
 *   }
 *   static std::optional<Animal3_> desc_to_enum(std::string& input) {
 *     if (input == val_DOG().desc) return DOG;
 *     if (input == val_CAT().desc) return CAT;
 *     return std::nullopt;
 *   }
 *   static std::optional<Animal3_> other_to_enum(int& input) {
 *     if (input == val_DOG().other) return DOG;
 *     if (input == val_CAT().other) return CAT;
 *     return std::nullopt;
 *   }
 * */
#define ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE(                                       \
    TypeDeclare,                                                                       \
    field1type, field1declare,                                                         \
    field2type, field2declare,                                                         \
    field3type, field3declare, /* enumKey, enumVal, field1val, field2val, field3val */ \
    ...)                                                                               \
  enum TypeDeclare {                                                                   \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                        \
        ENUM_UTIL_3_FIELD_SPECIFY_GENERATE_ENUM_ITEM,                                  \
        TypeDeclare,                                                                   \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__)) /**/                                  \
  };                                                                                   \
                                                                                       \
  struct TypeDeclare##Val {                                                            \
    const field1type field1declare;                                                    \
    const field2type field2declare;                                                    \
    const field3type field3declare;                                                    \
  };                                                                                   \
                                                                                       \
  MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                          \
      ENUM_UTIL_3_FIELD_SPECIFY_GET_VAL_OBJ,                                           \
      TypeDeclare,                                                                     \
      field1type, field1declare,                                                       \
      field2type, field2declare,                                                       \
      field3type, field3declare, __VA_ARGS__))                                         \
                                                                                       \
  static field1type enum_to_##field1declare(TypeDeclare input) {                       \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                        \
        ENUM_UTIL_3_FIELD_SPECIFY_ENUM_TO_DECLARE_1,                                   \
        TypeDeclare,                                                                   \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__))                                       \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:44)"));      \
  }                                                                                    \
  static field2type enum_to_##field2declare(TypeDeclare input) {                       \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                        \
        ENUM_UTIL_3_FIELD_SPECIFY_ENUM_TO_DECLARE_2,                                   \
        TypeDeclare,                                                                   \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__))                                       \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:50)"));      \
  }                                                                                    \
  static field3type enum_to_##field3declare(TypeDeclare input) {                       \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                        \
        ENUM_UTIL_3_FIELD_SPECIFY_ENUM_TO_DECLARE_3,                                   \
        TypeDeclare,                                                                   \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__))                                       \
    throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-29 09:01:55)"));      \
  }                                                                                    \
                                                                                       \
  static std::optional<TypeDeclare> field1declare##_to_enum(field1type& input) {       \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                        \
        ENUM_UTIL_3_FIELD_SPECIFY_DECLARE_1_TO_ENUM,                                   \
        TypeDeclare,                                                                   \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__))                                       \
    return std::nullopt;                                                               \
  }                                                                                    \
  static std::optional<TypeDeclare> field2declare##_to_enum(field2type& input) {       \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                        \
        ENUM_UTIL_3_FIELD_SPECIFY_DECLARE_2_TO_ENUM,                                   \
        TypeDeclare,                                                                   \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__))                                       \
    return std::nullopt;                                                               \
  }                                                                                    \
  static std::optional<TypeDeclare> field3declare##_to_enum(field3type& input) {       \
    MACRO_BASE__EXPAND(MACRO_BASE__S7_D5_PASTE(                                        \
        ENUM_UTIL_3_FIELD_SPECIFY_DECLARE_3_TO_ENUM,                                   \
        TypeDeclare,                                                                   \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__))                                       \
    return std::nullopt;                                                               \
  }
/* endregion ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE*/

/* region ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT*/
/**
 *  @code RAW
 *    ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
 *        Animal3,
 *        std::string, name, std::string, desc, int, other,
 *        DOG, 1, "dog", "这表示一条狗", 123,
 *        CAT, 2, "cat", "这表示一只猫", 456)
 *  @endcode
 *
 *  @code CONVERT RESULT
 *    struct Animal3 {
 *      enum Enum { DOG = 1,
 *                  CAT = 2,
 *      };
 *      struct Val {
 *        const std::string name;
 *        const std::string desc;
 *        const int         other;
 *      };
 *      static const Val& val_DOG() {
 *        static const Val val{"dog", "这表示一条狗", 123};
 *        return val;
 *      }
 *      static const Val& val_CAT() {
 *        static const Val val{"cat", "这表示一只猫", 456};
 *        return val;
 *      }
 *      static std::string enum_to_name(Enum input) {
 *        if (input == DOG) return val_DOG().name;
 *        if (input == CAT) return val_CAT().name;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:38)"));
 *      }
 *      static std::string enum_to_desc(Enum input) {
 *        if (input == DOG) return val_DOG().desc;
 *        if (input == CAT) return val_CAT().desc;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *      }
 *      static int enum_to_other(Enum input) {
 *        if (input == DOG) return val_DOG().other;
 *        if (input == CAT) return val_CAT().other;
 *        throw std::logic_error(WITH_LINE_INFO("unknow error (2023-03-28 22:18:47)"));
 *      }
 *      static std::optional<Enum> name_to_enum(std::string& input) {
 *        if (input == val_DOG().name) return DOG;
 *        if (input == val_CAT().name) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> desc_to_enum(std::string& input) {
 *        if (input == val_DOG().desc) return DOG;
 *        if (input == val_CAT().desc) return CAT;
 *        return std::nullopt;
 *      }
 *      static std::optional<Enum> other_to_enum(std::string& input) {
 *        if (input == val_DOG().other) return DOG;
 *        if (input == val_CAT().other) return CAT;
 *        return std::nullopt;
 *      }
 *    };
 *  @endcode
 */
#define ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(                             \
    TypeDeclare,                                                                       \
    field1type, field1declare,                                                         \
    field2type, field2declare,                                                         \
    field3type, field3declare, /* enumKey, enumVal, field1val, field2val, field3val */ \
    ...)                                                                               \
  struct TypeDeclare {                                                                 \
    ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE(                                           \
        Enum,                                                                          \
        field1type, field1declare,                                                     \
        field2type, field2declare,                                                     \
        field3type, field3declare, __VA_ARGS__)                                        \
  };
  // endregion ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT
}  // namespace nameless_carpool



namespace nameless_carpool {
  ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE(
      AnimalWithDefVal__,
      std::string, name,
      DOG, "dog",
      CAT, "cat")
  ENUM_UTIL_1_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
      AnimalWithDefVal,
      std::string, name,
      DOG, "dog",
      CAT, "cat")

  //  ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE(
  //      Animal,
  //      std::string, name,
  //      DOG, 1, "dog",
  //      CAT, 2, "cat")
  ENUM_UTIL_1_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
      Animal,
      std::string, name,
      DOG, 1, "dog",
      CAT, 2, "cat")

  //  ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE(
  //      Animal2WithDefVal_,
  //      std::string, name, bool, desc,
  //      DOG, "dog", true,
  //      CAT, "cat", false)
  ENUM_UTIL_2_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
      Animal2WithDefVal,
      std::string, name, bool, desc,
      DOG, "dog", true,
      CAT, "cat", false)



  //  ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE(
  //      Animal2__,
  //      std::string, name, std::string, desc,
  //      DOG, 1, "dog", "这表示一条狗",
  //      CAT, 2, "cat", "这表示一只猫")
  ENUM_UTIL_2_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
      Animal2_,
      std::string, name, std::string, desc,
      DOG, 1, "dog", "这表示一条狗",
      CAT, 2, "cat", "这表示一只猫")


  //  ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE(
  //      Animal3Def_,
  //      std::string, name, double, desc, int, other,
  //      DOG, "dog", 1.123, 123,
  //      CAT, "cat", 5.234, 456)
  ENUM_UTIL_3_FIELD_DEF_ENUM_GENERATE_IN_STRUCT(
      Animal3Def,
      std::string, name, double, desc, int, other,
      DOG, "dog", 1.123, 123,
      CAT, "cat", 5.234, 456)



  //  ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE(
  //      Animal3_,
  //      std::string, name3, std::string, desc3, int, other3,
  //      DOG3, 1, "dog", "这表示一条狗", 123,
  //      CAT3, 2, "cat", "这表示一只猫", 456)
  ENUM_UTIL_3_FIELD_SPECIFY_ENUM_GENERATE_IN_STRUCT(
      Animal3,
      std::string, name, std::string, desc, int, other,
      DOG, 1, "dog", "这表示一条狗", 123,
      CAT, 2, "cat", "这表示一只猫", 456)
}  // namespace nameless_carpool

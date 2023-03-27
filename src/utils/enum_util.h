//
// Created by Tom on 2023/3/27.
//

#pragma once

#include "exception"
#include "string"
#include "log_utils.h"

namespace nameless_carpool {

  /*

    DEFINE_ENUM(Flag,
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
        throw std::logic_error("un know exception ~~~~~ 2");
      }

    };

   */


#define TOM_EXPAND(x) x

/* 获取带有两个固定入参的宏 */
#define TOM_2_GET_MACRO(func, param1, param2, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, NAME, ...) NAME
/* TOM_2_PASTE 函数带有两个固定入参 , 第三个参数被遍历 */
#define TOM_2_PASTE(...) TOM_EXPAND(TOM_2_GET_MACRO(__VA_ARGS__,   \
                                                    TOM_2_PASTE64, \
                                                    TOM_2_PASTE63, \
                                                    TOM_2_PASTE62, \
                                                    TOM_2_PASTE61, \
                                                    TOM_2_PASTE60, \
                                                    TOM_2_PASTE59, \
                                                    TOM_2_PASTE58, \
                                                    TOM_2_PASTE57, \
                                                    TOM_2_PASTE56, \
                                                    TOM_2_PASTE55, \
                                                    TOM_2_PASTE54, \
                                                    TOM_2_PASTE53, \
                                                    TOM_2_PASTE52, \
                                                    TOM_2_PASTE51, \
                                                    TOM_2_PASTE50, \
                                                    TOM_2_PASTE49, \
                                                    TOM_2_PASTE48, \
                                                    TOM_2_PASTE47, \
                                                    TOM_2_PASTE46, \
                                                    TOM_2_PASTE45, \
                                                    TOM_2_PASTE44, \
                                                    TOM_2_PASTE43, \
                                                    TOM_2_PASTE42, \
                                                    TOM_2_PASTE41, \
                                                    TOM_2_PASTE40, \
                                                    TOM_2_PASTE39, \
                                                    TOM_2_PASTE38, \
                                                    TOM_2_PASTE37, \
                                                    TOM_2_PASTE36, \
                                                    TOM_2_PASTE35, \
                                                    TOM_2_PASTE34, \
                                                    TOM_2_PASTE33, \
                                                    TOM_2_PASTE32, \
                                                    TOM_2_PASTE31, \
                                                    TOM_2_PASTE30, \
                                                    TOM_2_PASTE29, \
                                                    TOM_2_PASTE28, \
                                                    TOM_2_PASTE27, \
                                                    TOM_2_PASTE26, \
                                                    TOM_2_PASTE25, \
                                                    TOM_2_PASTE24, \
                                                    TOM_2_PASTE23, \
                                                    TOM_2_PASTE22, \
                                                    TOM_2_PASTE21, \
                                                    TOM_2_PASTE20, \
                                                    TOM_2_PASTE19, \
                                                    TOM_2_PASTE18, \
                                                    TOM_2_PASTE17, \
                                                    TOM_2_PASTE16, \
                                                    TOM_2_PASTE15, \
                                                    TOM_2_PASTE14, \
                                                    TOM_2_PASTE13, \
                                                    TOM_2_PASTE12, \
                                                    TOM_2_PASTE11, \
                                                    TOM_2_PASTE10, \
                                                    TOM_2_PASTE9,  \
                                                    TOM_2_PASTE8,  \
                                                    TOM_2_PASTE7,  \
                                                    TOM_2_PASTE6,  \
                                                    TOM_2_PASTE5,  \
                                                    TOM_2_PASTE4,  \
                                                    TOM_2_PASTE3,  \
                                                    TOM_2_PASTE2,  \
                                                    TOM_2_PASTE1)(__VA_ARGS__))

#define TOM_2_PASTE1(func, param1, param2, v1) func(param1, param2, v1)
#define TOM_2_PASTE2(func, param1, param2, v1, v2) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE1(func, param1, param2, v2)
#define TOM_2_PASTE3(func, param1, param2, v1, v2, v3) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE2(func, param1, param2, v2, v3)
#define TOM_2_PASTE4(func, param1, param2, v1, v2, v3, v4) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE3(func, param1, param2, v2, v3, v4)
#define TOM_2_PASTE5(func, param1, param2, v1, v2, v3, v4, v5) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE4(func, param1, param2, v2, v3, v4, v5)
#define TOM_2_PASTE6(func, param1, param2, v1, v2, v3, v4, v5, v6) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE5(func, param1, param2, v2, v3, v4, v5, v6)
#define TOM_2_PASTE7(func, param1, param2, v1, v2, v3, v4, v5, v6, v7) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE6(func, param1, param2, v2, v3, v4, v5, v6, v7)
#define TOM_2_PASTE8(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE7(func, param1, param2, v2, v3, v4, v5, v6, v7, v8)
#define TOM_2_PASTE9(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE8(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9)
#define TOM_2_PASTE10(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE9(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10)
#define TOM_2_PASTE11(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE10(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define TOM_2_PASTE12(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE11(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)
#define TOM_2_PASTE13(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE12(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
#define TOM_2_PASTE14(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE13(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)
#define TOM_2_PASTE15(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE14(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)
#define TOM_2_PASTE16(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE15(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)
#define TOM_2_PASTE17(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE16(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)
#define TOM_2_PASTE18(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE17(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)
#define TOM_2_PASTE19(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE18(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)
#define TOM_2_PASTE20(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE19(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)
#define TOM_2_PASTE21(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE20(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21)
#define TOM_2_PASTE22(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE21(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22)
#define TOM_2_PASTE23(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE22(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23)
#define TOM_2_PASTE24(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE23(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24)
#define TOM_2_PASTE25(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE24(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25)
#define TOM_2_PASTE26(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE25(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26)
#define TOM_2_PASTE27(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE26(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27)
#define TOM_2_PASTE28(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE27(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28)
#define TOM_2_PASTE29(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE28(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29)
#define TOM_2_PASTE30(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE29(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30)
#define TOM_2_PASTE31(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE30(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31)
#define TOM_2_PASTE32(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE31(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32)
#define TOM_2_PASTE33(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE32(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33)
#define TOM_2_PASTE34(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE33(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34)
#define TOM_2_PASTE35(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE34(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35)
#define TOM_2_PASTE36(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE35(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36)
#define TOM_2_PASTE37(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE36(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37)
#define TOM_2_PASTE38(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE37(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38)
#define TOM_2_PASTE39(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE38(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39)
#define TOM_2_PASTE40(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE39(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40)
#define TOM_2_PASTE41(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE40(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41)
#define TOM_2_PASTE42(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE41(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42)
#define TOM_2_PASTE43(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE42(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43)
#define TOM_2_PASTE44(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE43(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44)
#define TOM_2_PASTE45(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE44(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45)
#define TOM_2_PASTE46(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE45(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46)
#define TOM_2_PASTE47(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE46(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47)
#define TOM_2_PASTE48(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE47(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48)
#define TOM_2_PASTE49(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE48(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49)
#define TOM_2_PASTE50(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE49(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50)
#define TOM_2_PASTE51(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE50(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51)
#define TOM_2_PASTE52(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE51(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52)
#define TOM_2_PASTE53(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE52(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53)
#define TOM_2_PASTE54(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE53(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54)
#define TOM_2_PASTE55(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE54(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55)
#define TOM_2_PASTE56(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE55(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56)
#define TOM_2_PASTE57(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE56(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57)
#define TOM_2_PASTE58(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE57(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58)
#define TOM_2_PASTE59(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE58(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59)
#define TOM_2_PASTE60(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE59(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60)
#define TOM_2_PASTE61(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE60(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61)
#define TOM_2_PASTE62(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE61(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62)
#define TOM_2_PASTE63(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE62(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63)
#define TOM_2_PASTE64(func, param1, param2, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63, v64) TOM_2_PASTE1(func, param1, param2, v1) TOM_2_PASTE63(func, param1, param2, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63, v64)



#define ENUM__CHECK_ITEM_AND_RETURN_STR(Type, input, field) \
  if (input == Type::field) return #field;
#define ENUM__CHECK_ITEM_AND_RETURN_VAL(Type, str, field) \
  if (str == #field) return Type::field;


#define ENUM__VAL_TO_STRING(Type, ...)                                                 \
  inline static std::string toString(const Type& input) {                              \
    TOM_EXPAND(TOM_2_PASTE(ENUM__CHECK_ITEM_AND_RETURN_STR, Type, input, __VA_ARGS__)) \
    throw std::logic_error(WITH_LINE_INFO("un know exception ~~~~~ 1"));                               \
  }

#define ENUM__STRING_TO_VAL(Type, ...)                                               \
  inline static Type toEnum(const std::string& str) {                                \
    TOM_EXPAND(TOM_2_PASTE(ENUM__CHECK_ITEM_AND_RETURN_VAL, Type, str, __VA_ARGS__)) \
    throw std::logic_error(WITH_LINE_INFO("un know exception ~~~~~ 2"));                             \
  }

#define ENUM_CONVERT(Type, ...)          \
  ENUM__VAL_TO_STRING(Type, __VA_ARGS__) \
  ENUM__STRING_TO_VAL(Type, __VA_ARGS__)

#define DEFINE_ENUM(Type, ...)      \
  struct Type {                     \
    enum Enum {                     \
      __VA_ARGS__                   \
    };                              \
    ENUM_CONVERT(Enum, __VA_ARGS__) \
  };

}  // namespace nameless_carpool
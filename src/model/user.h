#pragma once

#include <string>
// #include "../utils/all_include.h"
#include "../utils/json/include_json.h"

namespace nameless_carpool {
  using namespace std;

  /** 用户基本信息 */
  struct User {
    int       id;                  /*  */
    string    id_card_num;         /* 身份证 */
    string    name;                /*  */
    int       gender;              /* 性别 */
    time_t    birth_date;          /*  */
    int       birth_date_tz;       /*  */
    time_t    register_time;       /*  */
    int       register_time_tz;    /*  */
    time_t    update_time;         /*  */
    int       update_time_tz;      /*  */
    time_t    unregister_time;     /*  */
    int       unregister_time_tz;  /*  */

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, 
                                   id,
                                   name,
                                   gender,
                                   birth_date,
                                   birth_date_tz,
                                   register_time,
                                   register_time_tz,
                                   update_time,
                                   update_time_tz,
                                   unregister_time,
                                   unregister_time_tz
                                   )
  };

  /** 微信用户信息 */
  struct WcUser {
    int     user_id;
    string  wc_uid;
    string  wc_number;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WcUser, 
                                   user_id,
                                   wc_uid,
                                   wc_number
                                   )
  };

}

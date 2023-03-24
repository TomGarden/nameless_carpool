
#ifndef nameless_carpool_user_cpp
#define nameless_carpool_user_cpp

#include <string>
#include "include_json.h"


namespace src_test
{

  /** 用户基本信息 */
  struct User
  {
    int id;
    std::string name;
    int gender;
    time_t birth_date;
    int birth_date_tz;
    time_t register_time;
    int register_time_tz;
    time_t update_time;
    int update_time_tz;
    time_t unregister_time;
    int unregister_time_tz;
  };

  /** 微信用户信息 */
  struct WcUser
  {
    int user_id;
    std::string wc_uid;
    std::string wc_number;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WcUser          ,
                                   user_id,
                                   wc_uid,
                                   wc_number
    );
  };

}

#endif
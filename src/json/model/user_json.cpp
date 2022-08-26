#pragma once

#include "../../../libs/nlohmann_json_3.10.5/json.hpp"
#include "../../model/user.h"

using Json = nlohmann::json;

namespace nameless_carpool
{

  void to_json(Json &json, const WcUser &obj)
  {
    json = Json{
        {"user_id", obj.user_id},
        {"wc_uid", obj.wc_uid},
        {"wc_number", obj.wc_number}
      };
  }

  void from_json(const Json &json, WcUser &obj)
  {
    json.at("user_id").get_to(obj.user_id);
    json.at("wc_uid").get_to(obj.wc_uid);
    json.at("wc_number").get_to(obj.wc_number);
  }
}

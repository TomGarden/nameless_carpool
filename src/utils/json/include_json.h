#pragma once


#include <optional>
#include <exception>
#include "src/utils/linux_os.h"
#include "src/utils/log_utils.h"

/* 提供更详细的异常信息 https://json.nlohmann.me/api/macros/json_diagnostics/#examples  */
# define JSON_DIAGNOSTICS 1           
#include "libs/nlohmann_json_3.11.2/single_include/nlohmann/json.hpp"



namespace nlohmann {
  /* https://github.com/nlohmann/json#how-do-i-convert-third-party-types */
  template <typename T>
  struct adl_serializer<std::optional<T>> {
    static void to_json(json& jObj, const std::optional<T>& opt) {
      if (opt.has_value()) {
        // this will call adl_serializer<T>::to_json which will
        // find the free function to_json in T's namespace!
        jObj = opt.value();
      } else {
        jObj = nullptr;
      }
    }

    static void from_json(const json& jObj, std::optional<T>& opt) {
      if (jObj.is_null()) {
        opt = std::nullopt;
      } else {
        opt = jObj.get<T>();  // same as above, but with
                              // adl_serializer<T>::from_json
      }
    }
  };
}  // namespace nlohmann

namespace nameless_carpool {
  nlohmann::json& clearNull(nlohmann::json& jObj);

  template<typename T>
  nlohmann::json& getJsonWithoutNull(T& obj) {
    nlohmann::json jObj = obj;
    return clearNull(jObj);
  }
}
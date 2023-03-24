//
// Created by Tom on 2023/3/22.
//

#include "application.h"

#include <boost/algorithm/string.hpp>
#include <execution>

#include "linux_os.h"

namespace nameless_carpool {
  Application& Application::instance() {
    static Application application;
    return application;
  }

  bool Application::isDebugModel() {
#ifdef DEBUG_MODEL
    return true;
#else
    return localConfig.isDebugRunModel();
#endif
  }
}  // namespace nameless_carpool

namespace nameless_carpool{
  LocalConfig LocalConfig::parseInstance(const std::string& path) {
    logInfo << "local config path : " << path << logEndl;

    auto        flag   = nlohmann::json::parse(std::ifstream(path), nullptr, true, true);
    LocalConfig result = flag.get<LocalConfig>();

    if (result.amap_key.value_or("").empty()) logError << "amap_key is empty" << logEndl;

    return result;
  }

  bool LocalConfig::isDebugRunModel() const { return boost::iequals(run_model.value_or(""), "debug"); }
}
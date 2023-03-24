//
// Created by Tom on 2023/3/22.
//

#pragma once

#include <string>
#include <optional>
#include <fstream>

#include "linux_os.h"
#include "log_utils.h"
#include "include_json.h"

namespace nameless_carpool {

  struct LocalConfig;
  class Application;

}  // namespace nameless_carpool

struct nameless_carpool::LocalConfig {
  std::optional<std::string> run_model = std::nullopt;
  std::optional<std::string> amap_key  = std::nullopt;

  /* 从 path 给定的 配置文件 路径 获取并解析配置文件 */
  static LocalConfig parseInstance(const std::string& path) ;

  bool isDebugRunModel() const;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LocalConfig,
                                 run_model,
                                 amap_key)
};

class nameless_carpool::Application {
 private:
  const std::string executablePath          = getCurExeFileDirOrThrow();
  const std::string tomManualDir            = executablePath + "/tom_manual_dir";
  const std::string tomManualDocDir         = tomManualDir + "/doc";
  const std::string tomManualLocalConfigDir = tomManualDir + "/local_config";

 public:
  const std::string  appDebugInputPath   = tomManualDocDir + "/debugInput.json";
  const std::string  appCustomConfigPath = tomManualLocalConfigDir + "/nameless_carpool_config.json";
  const LocalConfig  localConfig         = LocalConfig::parseInstance(appCustomConfigPath);

 private:
  Application() = default;

 public:
  static Application& instance();
  bool isDebugModel();
};

namespace nameless_carpool{
  inline Application& app() { return Application::instance(); }
}


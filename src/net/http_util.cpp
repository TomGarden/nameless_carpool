
#include "src/net/http_util.h"
#include "application.h"

namespace nameless_carpool {

  extern std::string getHttpScheme() {
    if (app().isDebugModel()) return "http://";
    else return "https://";
  }
}  // namespace nameless_carpool

#include "src/net/http_util.h"
#include "application.h"

namespace nameless_carpool {
  HttpStatus      httpStatus;
  HttpMethodUtil  httpMethodUtil;
  MediaType       mediaType;
  HttpHeaderNames httpHeaderNames;

  extern std::string getHttpScheme() {
    if (app().isDebugModel()) return "http://";
    else return "https://";
  }
}  // namespace nameless_carpool
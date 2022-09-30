
#include "http_util.h"

namespace nameless_carpool {
  HttpStatus httpStatus;
  HttpMethodUtil httpMethodUtil;
  MediaType mediaType;
  HttpHeaderNames httpHeaderNames;

  const string HttpResponse::BODY_FORMAT_ERR = "请求体格式 校验 失败;"  ;
  const string HttpResponse::HEADER_MISS_ERR = "请求头丢失 : "           ;
}
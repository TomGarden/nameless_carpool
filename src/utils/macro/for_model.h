#pragma once

namespace nameless_carpool {
  #define GET_NAMES() \
      static Names& names() { static Names names; return names; }
}
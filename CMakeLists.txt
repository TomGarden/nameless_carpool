cmake_minimum_required(VERSION 3.0.0)
project(NamelessCarpool VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(NamelessCarpool 
               src   
               src/control   
               src/control/db_proxy.cpp   
               src/control/db_proxy.h   
               src/db   
               src/db/exten   
               src/db/exten/README.MD   
               src/db/exten.h   
               src/db/model   
               src/db/model/base_time.cpp   
               src/db/model/base_time.h   
               src/db/model/user_info.cpp   
               src/db/model/user_info.h   
               src/db/sql   
               src/db/sql/db_include.h   
               src/db/sql/db_manager.cpp   
               src/db/sql/db_manager.h   
               src/main.cpp   
               src/net   
               src/net/api   
               src/net/api/authenticate.cpp   
               src/net/api/authenticate.h   
               src/net/api/authenticate_m.h   
               src/net/api/readme.md   
               src/net/api.cpp   
               src/net/api.h   
               src/net/model   
               src/net/model/response_body.h   
               src/utils   
               src/utils/common.cpp       
               src/utils/common.h       
               src/utils/custom       
               src/utils/custom/optional.h       
               src/utils/fcgi_util.h       
               src/utils/http_util.cpp       
               src/utils/http_util.h       
               src/utils/input_check.cpp       
               src/utils/input_check.h       
               src/utils/json       
               src/utils/json/include_json.h       
               src/utils/json/json_util.cpp       
               src/utils/json/json_util.h       
               src/utils/linux_os.cpp       
               src/utils/linux_os.h       
               src/utils/log_utils.cpp       
               src/utils/log_utils.h       
               src/utils/tom_string_utils.cpp       
               src/utils/tom_string_utils.h       
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

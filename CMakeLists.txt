cmake_minimum_required(VERSION 3.0.0)
project(NamelessCarpool.run VERSION 0.1.0)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 17)
endif()

# Default to C++20
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  # set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)     # 包含当前路径

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-g -Wall -ggdb -rdynamic -no-pie -fno-pie -fPIC) 
endif()

set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set(TOM_MANUAL_DIR ${CMAKE_CURRENT_BINARY_DIR}/tom_manual_dir)

# 是否到处编译命令到 "build/compile_commands.json" 文件  
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)



# ┌──────────────────────────────────────────────────────────────────────────┐
# │    重点关注逻辑开始                                                         │
# │                         路径获取逻辑                                       │
# │                                                                          │
# └──────────────────────────────────────────────────────────────────────────┘                

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeUtils.cmake)   

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(LIB_JSON_DIR          ${LIB_DIR}/nlohmann_json_3.11.2/include/single_include)
set(LIB_DATE_INCLUDE_DIR  ${LIB_DIR}/date_3.0.1/include)
set(LIB_MYSQL_INCLUDE_DIR ${LIB_DIR}/mysql_connector_arm_static/include/mysqlx)
#set(LIB_OATPP_INCLUDE_DIR /usr/local/include/oatpp-1.3.0/oatpp) # 必须精确到这个目录之下 , oatapp 头文件中的引用才能正常解析
set(LIB_RXCPP_INCLUDE_DIR          /usr/local/include/rxcpp)
set(LIB_DRAGON_INCLUDE_DIR         /usr/local/include/drogon)
set(LIB_DRAGON_TRANTOR_INCLUDE_DIR /usr/local/include/trantor) # drogon 依赖这个库
set(LIB_DRAGON_JSON_INCLUDE_DIR    /usr/include/jsoncpp) # drogon 依赖这个库


# 构建一个可供后续 检索的 根目录列表
list(APPEND CODE_ROOT_DIR_LIST 
            ${SRC_DIR} 
            ${LIB_JSON_DIR}
            ${LIB_DATE_INCLUDE_DIR}
            ${LIB_RXCPP_INCLUDE_DIR}
            ${LIB_DRAGON_INCLUDE_DIR}
            ${LIB_DRAGON_TRANTOR_INCLUDE_DIR}
            ${LIB_DRAGON_JSON_INCLUDE_DIR}
    )

all_get_dir_list_from_root_list("${CODE_ROOT_DIR_LIST}" CODE_DIR)
all_get_file_list_from_root_list("${CODE_ROOT_DIR_LIST}" CODE_FILE) # 不含根目录 , 文件没有区分类型 , 包含所有文件
all_get_cpp_file_list("${CODE_DIR}" SOURCE_CODE_LIST)

# 设置包含目录
include_directories(
        ${CODE_DIR}
        )
#include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

message(↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓\n
        ↓"                           运行异常可以取消注释  , 观察详细信息                              "↓\n
        ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓)
get_property(RESULT_INCLUDE_DIR_LIST DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# message_list("${RESULT_INCLUDE_DIR_LIST}"   "##-include-dir->")     # 当前的 include 信息
# message_list("${CODE_DIR}"                  "()-code-dir->")        # 给定根目录以及根目录下的所有 子目录们
 message_list("${CODE_FILE}"                 "++-all-file->")        # 给定根目录以及根目录下的所有子 文件 , 不区分文件类型 , 没有目录
message_list("${SOURCE_CODE_LIST}"          "&&>-source-file-")     # 给定根目录以及根目录下的所有 源文件 们 , 通过 `aux_source_directory` 指令获取的

if (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})
  set(EXECUTABLE_ARG_LIST ${SOURCE_CODE_LIST})
elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_MAC}) #[[MacOs]] 
  set(EXECUTABLE_ARG_LIST ${SOURCE_CODE_LIST})  
elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_WINDOWS})
  set(EXECUTABLE_ARG_LIST ${CODE_FILE})
else ()
  message(>>>>>>>>>>>>>>>>>>>>  "Not setting Cmake logic for unkonw plateform 22222222 ")
endif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})

## 有些文件中包含了过多的源文件不适合对文件夹进行检索 , 我们单独摘出来逐个添加
list(APPEND EXECUTABLE_ARG_LIST ${LIB_DIR}/date_3.0.1/src/tz.cpp)

# ┌──────────────────────────────────────────────────────────────────────────┐
# │    重点关注逻辑结束                                                         │
# └──────────────────────────────────────────────────────────────────────────┘

# include(CTest)
# enable_testing()
# include(CPack)


add_executable(${CMAKE_PROJECT_NAME}  ${EXECUTABLE_ARG_LIST})


find_package(rxcpp CONFIG)
#find_package(Drogon)
#find_package(Jsoncpp)
#find_package(Trantor)

# 动态库 在 g++ 使用 -lLibName 指令
target_link_libraries(${CMAKE_PROJECT_NAME}
    # mysql 数据库 静态库
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/mysql_connector_arm_static/lib64/libmysqlcppconn8-static.a
    # 其他动态库
    glog fcgi fcgi++ cgicc ssl crypto pthread resolv
    # boost 相关 动态库
    boost_stacktrace_backtrace dl boost_date_time
    # HowardHinnant_date 相关 动态库
    pthread curl
    #dragon 依赖 trantor / jsoncpp
    drogon trantor jsoncpp z uuid
    #    /usr/local/lib/libdrogon.a /usr/local/lib/libtrantor.a jsoncpp
  )

# 拷贝文档到编译目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/doc          DESTINATION ${TOM_MANUAL_DIR} FOLLOW_SYMLINK_CHAIN)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/local_config DESTINATION ${TOM_MANUAL_DIR} FOLLOW_SYMLINK_CHAIN)
execute_process(COMMAND  chown --recursive tom:TOM ${CMAKE_CURRENT_SOURCE_DIR})


message(STATUS "CMake 指令全部执行完毕")  

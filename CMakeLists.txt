cmake_minimum_required(VERSION 3.0.0)
project(NamelessCarpool.run VERSION 0.1.0)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 17)
endif()

# Default to C++20
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-g -Wall -ggdb -rdynamic -no-pie -fno-pie -fPIC) 
endif()

set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})

# 设置包含目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(
                    ./src             
                    ./src/control     
                    ./src/db          
                    ./src/db/exten    
                    ./src/db/model    
                    ./src/db/sql      
                    ./src/net         
                    ./src/net/api     
                    ./src/net/model   
                    ./src/utils       
                    ./src/utils/custom
                    ./src/utils/json  
                  )



# 是否到处编译命令到 "build/compile_commands.json" 文件  
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

# 如果有必要后续这里的添加文件的指令可以通过 shell script 生成单独文件 , 然后包含到这里即可
aux_source_directory(./src                   DIR_SRC               )
aux_source_directory(./src/control           DIR_SRC_CONTROL       )
aux_source_directory(./src/db                DIR_SRC_DB            )
aux_source_directory(./src/db/exten          DIR_SRC_DB_EXTEN      )
aux_source_directory(./src/db/model          DIR_SRC_DB_MODEL      )
aux_source_directory(./src/db/sql            DIR_SRC_DB_SQL        )
aux_source_directory(./src/net               DIR_SRC_NET           )
aux_source_directory(./src/net/api           DIR_SRC_NET_API       )
aux_source_directory(./src/net/model         DIR_SRC_NET_MODEL     )
aux_source_directory(./src/utils             DIR_SRC_UTILS         )
aux_source_directory(./src/utils/custom      DIR_SRC_UTILS_CUSTOM  )
aux_source_directory(./src/utils/json        DIR_SRC_UTILS_JSON    )

message(ERROR "尝试下 , 文件路径有没有可能用 for 循环完成递归") 

set(CPP_FILES)
list(APPEND CPP_FILES ${DIR_SRC}
                      ${DIR_SRC_CONTROL}
                      ${DIR_SRC_DB}
                      ${DIR_SRC_DB_EXTEN}
                      ${DIR_SRC_DB_MODEL}
                      ${DIR_SRC_DB_SQL}
                      ${DIR_SRC_NET}
                      ${DIR_SRC_NET_API}
                      ${DIR_SRC_NET_MODEL}
                      ${DIR_SRC_UTILS}
                      ${DIR_SRC_UTILS_CUSTOM}
                      ${DIR_SRC_UTILS_JSON}      )

list(APPEND CPP_FILES libs/date_3.0.1/src/tz.cpp )


# message(STATUS "所有文件 + " ${CPP_FILES}) 
# message(STATUS "RUNTIME_OUTPUT_DIRECTORY + " ${RUNTIME_OUTPUT_DIRECTORY})  
# message(STATUS "CMAKE_CURRENT_BINARY_DIR + " ${CMAKE_CURRENT_BINARY_DIR})  
message(STATUS "CMAKE_INSTALL_PREFIX + " ${CMAKE_INSTALL_PREFIX})  

# 通过文件指令 遍历 cmakefile 目录下匹配 正则的所有文件 , 作为 list 放到 RECURSION_XXX_FILES 目录中
# file(GLOB_RECURSE RECURSION_SRC_FILES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# file(GLOB_RECURSE RECURSION_HEADER_FILES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)


add_executable(${CMAKE_PROJECT_NAME}  ${CPP_FILES})

# add_executable(${CMAKE_PROJECT_NAME}  
# /mount_point/data/_document/c_cpp_program/nameless_carpool/src_test/2022-10-05/main.cpp
# libs/date_3.0.1/src/tz.cpp
# )

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
  )
  
include(CPack)


message(STATUS "CMake 指令全部执行完毕") 

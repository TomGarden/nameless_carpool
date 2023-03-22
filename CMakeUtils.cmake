
# TIPS : 列表作为参数入参的时候 , 需要作为一个整体入参 , 像这样(双引号也是代码) : "${LIST_VAR}"


set(PLATEFROM_LINUX   "Linux")
set(PLATEFROM_MAC     "Darwin")
set(PLATEFROM_WINDOWS "Windows")



# ┌──────────────────────────────────────────────────────────────────────┐
# │                                                                      │
# │               UU   UU    NN   NN    IIIII    XX    XX                │
# │               UU   UU    NNN  NN     III      XX  XX                 │
# │               UU   UU    NN N NN     III       XXXX                  │
# │               UU   UU    NN  NNN     III      XX  XX                 │
# │                UUUUU     NN   NN    IIIII    XX    XX                │
# │                                                                      │
# └──────────────────────────────────────────────────────────────────────┘

### unix 通过给定的根目录递归获取根目录下的路径们 , 具体值取决于 ARG_COMMAND , 并输出到 RESULT_PATH_LIST 列表中 (含根目录)
function(unix_get_path_list ARG_CMD ARG_CMD_ARG RESULT_PATH_LIST )
  ###使用 tree 命令获取文件夹路径们
    execute_process(COMMAND  ${ARG_CMD} ${ARG_CMD_ARG} OUTPUT_VARIABLE OUTPUT_STRING)
  ###替换换行 , 将文件夹们放到 列表中
    #message(===> ${OUTPUT_STRING})
    string(REPLACE "\n" ";" SOURCE_DIR_LIST ${OUTPUT_STRING})
  ###列表末尾有 tree 命令的统计信息 , 删除他们
    list(LENGTH SOURCE_DIR_LIST LIST_LENGTH)
    #message(list_length ===> ${LIST_LENGTH})
    math(EXPR LIST_LENGTH "${LIST_LENGTH} - 3" OUTPUT_FORMAT DECIMAL)
    list(SUBLIST SOURCE_DIR_LIST 0 ${LIST_LENGTH} SOURCE_DIR_LIST)
    set(${RESULT_PATH_LIST} ${SOURCE_DIR_LIST} PARENT_SCOPE)
endfunction(unix_get_path_list ARG_CMD ARG_CMD_ARG RESULT_PATH_LIST )

### unix 通过给定的根目录递归获取根目录下的所有目录 , 并输出到 RESULT_DIR_LIST 列表中 (含根目录)
function(unix_get_dir_list ARG_ROOT_DIR RESULT_DIR_LIST)
  IF(NOT EXISTS ${ARG_ROOT_DIR})
    MESSAGE("Path not exist ===>" ${ARG_ROOT_DIR})
    RETURN()
  ENDIF()
  set(ARG_CMD tree )
  set(ARG_CMD_ARG "${ARG_ROOT_DIR} -d -f -i")
  string(REPLACE " " ";" ARG_CMD_ARG ${ARG_CMD_ARG})    # 把字符串转换为列表 , 并转义 列表分隔符
  unix_get_path_list(${ARG_CMD} "${ARG_CMD_ARG}" RESULT_PATH_LIST)
  set(${RESULT_DIR_LIST} ${RESULT_PATH_LIST} PARENT_SCOPE)
endfunction(unix_get_dir_list ARG_ROOT_DIR RESULT_DIR_LIST)

### unix 通过给定的根目录递归获取根目录下的所有目录和文件 , 并输出到 RESULT_FILE_AND_DIR_LIST 列表中 (含根目录)
function(unix_get_file_and_dir_list ARG_ROOT_DIR RESULT_FILE_AND_DIR_LIST)
  set(ARG_CMD tree )
  set(ARG_CMD_ARG "${ARG_ROOT_DIR} -f -i")
  string(REPLACE " " ";" ARG_CMD_ARG ${ARG_CMD_ARG})    # 把字符串转换为列表 , 列表作为入参的时候要用双引号 , 保证整个列表作为一个参数 , 入参
  unix_get_path_list(${ARG_CMD} "${ARG_CMD_ARG}" RESULT_PATH_LIST)
  set(${RESULT_FILE_AND_DIR_LIST} ${RESULT_PATH_LIST} PARENT_SCOPE)
endfunction(unix_get_file_and_dir_list ARG_ROOT_DIR RESULT_FILE_AND_DIR_LIST)


# ┌──────────────────────────────────────────────────────────────────────────┐
# │                                                                          │
# │                       WW      WW    IIIII    NN   NN                     │
# │                       WW      WW     III     NNN  NN                     │
# │                       WW   W  WW     III     NN N NN                     │
# │                        WW WWW WW     III     NN  NNN                     │
# │                         WW   WW     IIIII    NN   NN                     │
# │                                                                          │
# └──────────────────────────────────────────────────────────────────────────┘

### unix 通过给定的根目录递归获取根目录下的路径们 , 具体值取决于 ARG_COMMAND , 并输出到 RESULT_PATH_LIST 列表中 (含根目录)
function(windows_get_path_list ARG_CMD ARG_CMD_ARG ARG_ROOT_DIR RESULT_PATH_LIST )
  ###使用 tree 命令获取文件夹路径们
  execute_process(COMMAND powershell ${ARG_CMD} ${ARG_ROOT_DIR} ${ARG_CMD_ARG} OUTPUT_VARIABLE SOURCE_DIR_STRING)

  ### 字符串操作 , 将文件路径符号替换 ; 将文件路径字符串转换为列表
    if("${SOURCE_DIR_STRING}" STREQUAL "")
      # [[ 啥也不做就合理 ]]
    else()
      string(REPLACE "\\" "/" SOURCE_DIR_STRING ${SOURCE_DIR_STRING})
      string(REPLACE "\n" ";" SOURCE_DIR_LIST ${SOURCE_DIR_STRING})
    endif("${SOURCE_DIR_STRING}" STREQUAL "")

  ### windows 环境下 , 文件路径不是完整的需要一个补全动作
    set(SOURCE_DIR_FULL_PATH_LIST)
    foreach(SRC_DIR ${SOURCE_DIR_LIST})
      list(APPEND SOURCE_DIR_FULL_PATH_LIST ${ARG_ROOT_DIR}/${SRC_DIR})
    endforeach()
    list(APPEND SOURCE_DIR_FULL_PATH_LIST ${ARG_ROOT_DIR})              # windows 下 tree 命令不包含入参路径

  ### 将所有文件添加到结果变量中
    set(${RESULT_PATH_LIST} ${SOURCE_DIR_FULL_PATH_LIST} PARENT_SCOPE)
endfunction(windows_get_path_list ARG_CMD ARG_CMD_ARG ARG_ROOT_DIR RESULT_PATH_LIST )

### windows 通过给定的根目录递归获取根目录下的所有目录 , 并输出到 RESULT_DIR_LIST 列表中 (含根目录)
function(windows_get_dir_list ARG_ROOT_DIR RESULT_DIR_LIST)

  set(ARG_CMD dir)
  set(ARG_CMD_ARG "-directory -name -recurse")
  string(REPLACE " " ";" ARG_CMD_ARG ${ARG_CMD_ARG})    # 把字符串转换为列表 , 并转义 列表分隔符
  windows_get_path_list(${ARG_CMD} "${ARG_CMD_ARG}" ${ARG_ROOT_DIR} RESULT_PATH_LIST)
  set(${RESULT_DIR_LIST} ${RESULT_PATH_LIST} PARENT_SCOPE)

endfunction(windows_get_dir_list ARG_ROOT_DIR RESULT_DIR_LIST)

### windows 通过给定的根目录递归获取根目录下的所有目录和文件 , 并输出到 RESULT_FILE_AND_DIR_LIST 列表中 (含根目录)
function(windows_get_file_and_dir_list ARG_ROOT_DIR RESULT_FILE_AND_DIR_LIST)

  set(ARG_CMD dir)
  set(ARG_CMD_ARG "-name -recurse")
  string(REPLACE " " ";" ARG_CMD_ARG ${ARG_CMD_ARG})    # 把字符串转换为列表 , 并转义 列表分隔符
  windows_get_path_list(${ARG_CMD} "${ARG_CMD_ARG}" ${ARG_ROOT_DIR} RESULT_PATH_LIST)
  set(${RESULT_FILE_AND_DIR_LIST} ${RESULT_PATH_LIST} PARENT_SCOPE)

endfunction(windows_get_file_and_dir_list ARG_ROOT_DIR RESULT_DIR_LIST)


# ┌──────────────────────────────────────────────────────────────────────────┐
# │                                                                          │
# │                         AAA      LL         LL                           │
# │                        AAAAA     LL         LL                           │
# │                       AA   AA    LL         LL                           │
# │                       AAAAAAA    LL         LL                           │
# │                       AA   AA    LLLLLLL    LLLLLLL                      │
# │                                                                          │
# └──────────────────────────────────────────────────────────────────────────┘


### all 通过给定的根目录递归获取根目录下的路径们 , 具体值取决于 ARG_COMMAND , 并输出到 RESULT_PATH_LIST 列表中 (含根目录)
function(all_get_path_list_from_root ARG_ROOT_DIR RESULT_PATH_LIST )

  if (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})
    unix_get_path_list(${ARG_ROOT_DIR} LOCALE_RESULT_PATH_LIST)
  elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_MAC}) #[[MacOs]]
    unix_get_path_list(${ARG_ROOT_DIR} LOCALE_RESULT_PATH_LIST)
  elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_WINDOWS})
    windows_get_path_list(${ARG_ROOT_DIR} LOCALE_RESULT_PATH_LIST)
  else ()
    message(>>>[${CMAKE_CURRENT_FUNCTION}]>>>  "Not setting Cmake logic for unkonw plateform")
  endif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})

  set(${RESULT_PATH_LIST} ${LOCALE_RESULT_PATH_LIST} PARENT_SCOPE)
endfunction(all_get_path_list_from_root ARG_ROOT_DIR RESULT_PATH_LIST )
#   all 入参为 列表 , 循环调用 [all_get_path_list_from_root]
function(all_get_path_list_from_root_list ARG_ROOT_DIR_LIST RESULT_PATH_LIST )
  set(LOCALE_RESULT_PATH_LIST)
  foreach(ROOT_DIR_ITEM ${ARG_ROOT_DIR_LIST})
    set(SUB_PATH_LIST "")
    all_get_path_list_from_root(${ROOT_DIR_ITEM} SUB_PATH_LIST)
    list(APPEND LOCALE_RESULT_PATH_LIST ${SUB_PATH_LIST})
  endforeach(ROOT_DIR_ITEM ${ARG_ROOT_DIR_LIST})

  set(${RESULT_PATH_LIST} ${LOCALE_RESULT_PATH_LIST} PARENT_SCOPE)
endfunction(all_get_path_list_from_root_list ARG_ROOT_DIR_LIST RESULT_PATH_LIST )


### all 通过给定的根目录递归获取根目录下的所有目录 , 并输出到 RESULT_DIR_LIST 列表中 (含根目录)
function(all_get_dir_list_from_root ARG_ROOT_DIR RESULT_DIR_LIST)

  if (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})
    unix_get_dir_list(${ARG_ROOT_DIR} LOCALE_RESULT_PATH_LIST)
  elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_MAC}) #[[MacOs]]
    unix_get_dir_list(${ARG_ROOT_DIR} LOCALE_RESULT_PATH_LIST)
  elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_WINDOWS})
    windows_get_dir_list(${ARG_ROOT_DIR} LOCALE_RESULT_PATH_LIST)
  else ()
    message(>>>[${CMAKE_CURRENT_FUNCTION}]>>>  "Not setting Cmake logic for unkonw plateform")
  endif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})

  set(${RESULT_DIR_LIST} ${LOCALE_RESULT_PATH_LIST} PARENT_SCOPE)
endfunction(all_get_dir_list_from_root ARG_ROOT_DIR RESULT_DIR_LIST)
#   all 入参为列表参数  , 循环调用   [all_get_dir_list_from_root]
function(all_get_dir_list_from_root_list ARG_ROOT_DIR_LIST RESULT_DIR_LIST )
  set(LOCALE_RESULT_DIR_LIST)
  foreach(ROOT_DIR_ITEM ${ARG_ROOT_DIR_LIST})
    set(SUB_DIR_LIST "")
    all_get_dir_list_from_root(${ROOT_DIR_ITEM} SUB_DIR_LIST)
    list(APPEND LOCALE_RESULT_DIR_LIST ${SUB_DIR_LIST})
  endforeach(ROOT_DIR_ITEM ${ARG_ROOT_DIR_LIST})

  set(${RESULT_DIR_LIST} ${LOCALE_RESULT_DIR_LIST} PARENT_SCOPE)
endfunction(all_get_dir_list_from_root_list ARG_ROOT_DIR_LIST RESULT_DIR_LIST )


### all plateform 通过给定的根目录递归获取根目录下的所有文件 , 并输出到 RESULT_FILE_LIST 列表中 (不含根目录 , 文件没有区分类型 , 包含所有文件)
function(all_get_file_list_from_root ARG_ROOT_DIR RESULT_FILE_LIST)

  if (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})
    unix_get_file_and_dir_list(${ARG_ROOT_DIR} RESULT_PATH_LIST)
  elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_MAC}) #[[MacOs]]
    unix_get_file_and_dir_list(${ARG_ROOT_DIR} RESULT_PATH_LIST)
  elseif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_WINDOWS})
    windows_get_file_and_dir_list(${ARG_ROOT_DIR} RESULT_PATH_LIST)
  else ()
    message(>>>[${CMAKE_CURRENT_FUNCTION}]>>>  "Not setting Cmake logic for unkonw plateform")
  endif (CMAKE_SYSTEM_NAME MATCHES ${PLATEFROM_LINUX})

  set(FILE_LIST)
  foreach(path_str ${RESULT_PATH_LIST}) # 筛选出所有路径中的文件路径
    if(IS_DIRECTORY ${path_str})
      #[[ empty ]]
    else()
      list(APPEND FILE_LIST ${path_str})
    endif()
  endforeach()

  set(${RESULT_FILE_LIST} ${FILE_LIST} PARENT_SCOPE)
endfunction(all_get_file_list_from_root ARG_ROOT_DIR RESULT_FILE_LIST)
#   all 入参为列表参数  , 循环调用   [all_get_file_list_from_root]
function(all_get_file_list_from_root_list ARG_ROOT_DIR_LIST RESULT_FILE_LIST )
  set(LOCALE_RESULT_FILE_LIST)
  foreach(ROOT_DIR_ITEM ${ARG_ROOT_DIR_LIST})
    set(SUB_FILE_LIST "")
    all_get_file_list_from_root(${ROOT_DIR_ITEM} SUB_FILE_LIST)
    list(APPEND LOCALE_RESULT_FILE_LIST ${SUB_FILE_LIST})
  endforeach(ROOT_DIR_ITEM ${ARG_ROOT_DIR_LIST})

  set(${RESULT_FILE_LIST} ${LOCALE_RESULT_FILE_LIST} PARENT_SCOPE)
endfunction(all_get_file_list_from_root_list ARG_ROOT_DIR_LIST RESULT_DIR_LIST )



### all plateform 获取给定 目录列表中的 所有 源文件 (不含头文件) 文件路径
function(all_get_cpp_file_list ARG_DIR_LIST RESULT_CPP_FILE_LIST)
  set(CPP_FILE_LIST)
  foreach(DIR_PATH_ITEM ${ARG_DIR_LIST}) # 筛选出所有路径中的文件路径
    set(SUB_CPP_LIST "")
    aux_source_directory(${DIR_PATH_ITEM} SUB_CPP_LIST)
    list(APPEND CPP_FILE_LIST ${SUB_CPP_LIST})
  endforeach()

  set(${RESULT_CPP_FILE_LIST} ${CPP_FILE_LIST} PARENT_SCOPE)
endfunction(all_get_cpp_file_list ARG_DIR_LIST RESULT_CPP_FILE_LIST)



# ┌──────────────────────────────────────────────────────────────────────────┐
# │                                                                          │
# │                         平台逻辑结束                                       │
# │                以下属于业务逻辑 , 不需要时可以注释掉                           │
# │                                                                          │
# └──────────────────────────────────────────────────────────────────────────┘

function(message_list ARG_LIST ARG_PREFIX)
  foreach(ITEM ${ARG_LIST})
    message(${ARG_PREFIX}${ITEM})
  endforeach(ITEM ${ARG_LIST})
endfunction(message_list ARG_LIST ARG_PREFIX)

# 我们的编译预期 : 编译动作会新创建一个文件夹`build` , 
#	然后将编译的中间产物按照现有项目文件分布在 build 中做一个相同的分布 ; 
#	我期望的 make file 文件结构的样子:
#		1. 所有的编译产物归总到 build 文件夹中
#		2. 每个文件夹都有一个 makefile 文件
#		3. 通过最外层 makefile 调用内层 makefile 的形式最终组织起整个 makefile 结构
# 	4. 最终一个 make 命令完成代码编译动作


.RECIPEPREFIX = ># 标定命令行前缀,默认命令行前缀是 tab 字符，更改为 ‘>’
stdV = c++20


make_file_path = $(dir $(word 1,$(MAKEFILE_LIST)))
pro_dir = $(realpath $(make_file_path)..)



################################################################################
# 													源文件 & 目标文件声明																 #
################################################################################
# 源代码文件夹
# src_dir = $(pro_dir)/src
# src 目录下 所有的编译产物都放到这个目录中 , 按照代码目录结构组织这个文件夹
protram_output_dir = $(pro_dir)/build_src
build_dir = $(protram_output_dir)/build
dynamic_lib_dir = $(pro_dir)/libs/dynamic

libs = libs
	libs_date_src = date_3.0.1/src
		tz_in = $(pro_dir)/$(libs)/$(libs_date_src)/tz.cpp
		tz_o  = $(build_dir)/$(libs)/$(libs_date_src)/tz.o

src = src
	main_in = $(pro_dir)/$(src)/main.cpp
	main_o = $(build_dir)/$(src)/main.o

src_control = src/control
	db_proxy_in = $(pro_dir)/$(src_control)/db_proxy.cpp
	db_proxy_o = $(build_dir)/$(src_control)/db_proxy.o


src_db = src/db

	src_db_sql = $(src_db)/sql
		db_manager_in = $(pro_dir)/$(src_db_sql)/db_manager.cpp 
		db_manager_o 	= $(build_dir)/$(src_db_sql)/db_manager.o 

	src_db_model = $(src_db)/model
		base_time_in = $(pro_dir)/$(src_db_model)/base_time.cpp 
		base_time_o 	= $(build_dir)/$(src_db_model)/base_time.o
		user_info_in = $(pro_dir)/$(src_db_model)/user_info.cpp 
		user_info_o 	= $(build_dir)/$(src_db_model)/user_info.o  


src_utils = src/utils
	tom_string_utils_in = $(pro_dir)/$(src_utils)/tom_string_utils.cpp 
	tom_string_utils_o 	= $(build_dir)/$(src_utils)/tom_string_utils.o 
	input_check_in 		 	= $(pro_dir)/$(src_utils)/input_check.cpp
	input_check_o 			= $(build_dir)/$(src_utils)/input_check.o 
	http_util_in 				= $(pro_dir)/$(src_utils)/http_util.cpp
	http_util_o 				= $(build_dir)/$(src_utils)/http_util.o
	linux_os_in 				= $(pro_dir)/$(src_utils)/linux_os.cpp
	linux_os_o 					= $(build_dir)/$(src_utils)/linux_os.o
	common_in 					= $(pro_dir)/$(src_utils)/common.cpp
	common_o 						= $(build_dir)/$(src_utils)/common.o
	log_utils_in				= $(pro_dir)/$(src_utils)/log_utils.cpp
	log_utils_o 				= $(build_dir)/$(src_utils)/log_utils.o

src_net = src/net
	api_in = $(pro_dir)/$(src_net)/api.cpp
	api_o = $(build_dir)/$(src_net)/api.o
	src_net_api = $(src_net)/api
		authenticate_in = $(pro_dir)/$(src_net_api)/authenticate.cpp
		authenticate_o = $(build_dir)/$(src_net_api)/authenticate.o

objects = $(tom_string_utils_o) $(http_util_o)	$(input_check_o) $(fcgio_o) $(common_o) $(linux_os_o) $(log_utils_o)\
					$(db_manager_o) $(base_time_o) $(user_info_o) \
					$(db_proxy_o) \
					$(api_o) $(authenticate_o) $(tz_o) \
					$(main_o)

static_lib = $(mysql)
	mysql = $(pro_dir)/libs/mysql_connector_arm_static/lib64/libmysqlcppconn8-static.a


# -I 用于指定搜索头文件的路径
# -L 用于指定动态库名称 , 默认的动态库搜索路径包含了 `/usr/local/lib` , 如果要自行添加使用 -L 
dynamic_libs = $(links) $(boost) $(HowardHinnant_date)
	links = -lglog \
					-lfcgi -lfcgi++ -lcgicc \
					-lssl -lcrypto -lpthread -lresolv 
	boost = $(stackTrace) $(date_time)
		stackTrace = -lboost_stacktrace_backtrace -ldl
		date_time	 = -lboost_date_time 
	HowardHinnant_date = -lpthread  -lcurl

#				-lboost_stacktrace_basic -ldl -DBOOST_STACKTRACE_LINK
# -lboost_stacktrace_backtrace  -lboost_stacktrace_addr2line -lboost_stacktrace_basic -lboost_stacktrace_noop -ldl -DBOOST_STACKTRACE_LINK
# https://stackoverflow.com/questions/52583544/boost-stack-trace-not-showing-function-names-and-line-numbers

# g++ = g++ -g -c -std=c++11 -Wall
gpp = g++ -g -std=${stdV} -Wall -ggdb -rdynamic -no-pie -fno-pie -fPIC
g++ = $(gpp) -c 

# region 定义几个便利函数

# 如果文件夹不存在,创建一个文件夹
create_dir = 								\
	if [ ! -d "$(1)" ]; then 	\
		mkdir -p "$(1)"; 				\
	fi												\
# endregion 

all : try_create_all_dependent_dir					\
			$(protram_output_dir)/execute.run
.PHONY : all


################################################################################
# 																创建新文件夹																	 #
################################################################################
try_create_all_dependent_dir :      
>	$(call create_dir,$(build_dir)/$(src))
>		$(call create_dir,$(build_dir)/$(libs)/$(libs_date_src))
>		$(call create_dir,$(build_dir)/$(src_control))
>		$(call create_dir,$(build_dir)/$(src_utils))
>		$(call create_dir,$(build_dir)/$(src_net))
>			$(call create_dir,$(build_dir)/$(src_net_api))
>		$(call create_dir,$(build_dir)/$(src_db))
>			$(call create_dir,$(build_dir)/$(src_db_sql))
>			$(call create_dir,$(build_dir)/$(src_db_model))

################################################################################
# 																创建可执行文件																 #
################################################################################
$(protram_output_dir)/execute.run : $(objects)
>   $(gpp) $(objects) $(static_lib) $(dynamic_libs) -o $(protram_output_dir)/execute.run 
>		echo "build success ..."
# > 	echo " start run ... \n"
# >		$(protram_output_dir)/execute.run


################################################################################
# 																	libs																			 #
################################################################################
$(tz_o) : $(tz_in)
>		$(g++) $(tz_in) -o $(tz_o) 

################################################################################
# 																	src																				 #
################################################################################
$(main_o) : $(main_in)
>		$(g++) $(main_in) -o $(main_o) 

################################################################################
# 																	src/control 															 #
################################################################################
$(db_proxy_o) : $(db_proxy_in)
>		$(g++) $(db_proxy_in) -o $(db_proxy_o) 

################################################################################
# 																	src/utils																	 #
################################################################################
$(tom_string_utils_o) : $(tom_string_utils_in)
>		$(g++) $(tom_string_utils_in) -o $(tom_string_utils_o)

$(input_check_o) : $(input_check_in)
>		$(g++) $(input_check_in) -o $(input_check_o)

$(http_util_o) : $(http_util_in)
>		$(g++) $(http_util_in) -o $(http_util_o)

$(common_o) : $(common_in)
>		$(g++) $(common_in) -o $(common_o)

$(linux_os_o) : $(linux_os_in)
>		$(g++) $(linux_os_in) -o $(linux_os_o)

$(log_utils_o) : $(log_utils_in)
>		$(g++) $(log_utils_in) -o $(log_utils_o)

################################################################################
# 																	src/net																	 #
################################################################################
$(api_o) : $(api_in)
>		$(g++) $(api_in) -o $(api_o)
################################################################################
# 																	src/net/api																 #
################################################################################

$(authenticate_o) : $(authenticate_in)
>		$(g++) $(authenticate_in) -o $(authenticate_o)

################################################################################
# 																	src/db																		 #
################################################################################



################################################################################
# 																	src/db/sql															 #
################################################################################

$(db_manager_o) : $(db_manager_in)
>		$(g++) $(db_manager_in) -o $(db_manager_o)


################################################################################
# 																	src/db/model															 #
################################################################################

$(base_time_o) : $(base_time_in)
>		$(g++) $(base_time_in) -o $(base_time_o)
$(user_info_o) : $(user_info_in)
>		$(g++) $(user_info_in) -o $(user_info_o)




################################################################################
# 															其他命令选项																		 #
################################################################################

# 清空编译产物
.PHONY : clean
clean :
>   rm -rf $(protram_output_dir)

clean_log:
> 	rm -rf $(pro_dir)/tmp_dir/run_log/*

copy_doc:
>		cp $(pro_dir)/doc/nameless_carpool.software.usage.txt  $(protram_output_dir)
>		cp $(pro_dir)/doc/debugInput.json  			 						   $(protram_output_dir)

# 执行编译结果
run :
>		$(protram_output_dir)/execute.run  --input_file


reload_cgi :
> 	ssh -i /mount_point/data/_beyourself/_global_config_file/ssh/login_remote_ssh  \
				tom@172.17.0.3 /mount_point/data/_document/nginx_web_server/fcgi_reload.sh 	


#		clear && make --file=makefiles/src.mk clean all copy_doc clean_log reload_cgi run       
#		clear && make --file=makefiles/src.mk clean all copy_doc clean_log            run
#		clear && make --file=makefiles/src.mk       all copy_doc clean_log            run       
#		clear && make --file=makefiles/src.mk       all copy_doc clean_log reload_cgi run       
#		clear && make --file=makefiles/src.mk                              reload_cgi run       
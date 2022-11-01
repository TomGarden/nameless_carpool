# 我们的编译预期 : 编译动作会新创建一个文件夹`build` , 
#	然后将编译的中间产物按照现有项目文件分布在 build 中做一个相同的分布 ; 
#	我期望的 make file 文件结构的样子:
#		1. 所有的编译产物归总到 build 文件夹中
#		2. 每个文件夹都有一个 makefile 文件
#		3. 通过最外层 makefile 调用内层 makefile 的形式最终组织起整个 makefile 结构
# 	4. 最终一个 make 命令完成代码编译动作


.RECIPEPREFIX = ># 标定命令行前缀,默认命令行前缀是 tab 字符，更改为 ‘>’
g++ = g++ -g -std=c++20
g++Unlink = $(g++) -c

make_file_path = $(dir $(word 1,$(MAKEFILE_LIST)))
pro_dir = $(realpath $(make_file_path)..)

# 源代码文件夹
src_dir = $(pro_dir)/src_test
# src 目录下 所有的编译产物都放到这个目录中 , 按照代码目录结构组织这个文件夹
protram_output_dir = $(pro_dir)/build_src_test
build_dir = $(protram_output_dir)/build

objects = $(build_dir)/date_time.o 																\
					$(build_dir)/mysql_db.o   															\
					$(build_dir)/tz.o			    															\
					$(build_dir)/main.o


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


try_create_all_dependent_dir :      
>		$(call create_dir,$(protram_output_dir))
>		$(call create_dir,$(build_dir))

# linkMysqlLib=-L$(pro_dir)/libs/mysql_connector/lib64 -lmysqlcppconn8
# linkMysqlLib=-L$(pro_dir)/libs/mysql_connector_tom/lib64 -lmysqlcppconn8
# linkMysqlLib=-L$(pro_dir)/libs/mysql_connector_static_tom/lib64 -lmysqlcppconn8-static
# 使用 mysql 静态库之后链接异常的解答 : https://forums.mysql.com/read.php?167,686070,686070
# staticLib=$(pro_dir)/libs/mysql_connector_static_tom/lib64/libmysqlcppconn8-static.a
staticLib=$(pro_dir)/libs/mysql_connector_arm_static/lib64/libmysqlcppconn8-static.a
dynamicLibs = -lssl -lcrypto -lpthread -lresolv -lcurl
$(protram_output_dir)/execute.run : $(objects)
>   $(g++) $(objects) \
		$(staticLib) \
		$(dynamicLibs) \
		-o $(protram_output_dir)/execute.run 
>		echo "build success ..."
# > 	echo " start run ... \n"
# >		$(protram_output_dir)/execute.run


date_time_o_out = $(build_dir)/date_time.o
date_time_o_in = $(src_dir)/date_time.cpp
$(date_time_o_out) : $(date_time_o_in)
>		$(g++Unlink) $(date_time_o_in) -o $(date_time_o_out)

mysql_db_o_out = $(build_dir)/mysql_db.o
mysql_db_o_in = $(src_dir)/mysql_db.cpp
$(mysql_db_o_out) : $(mysql_db_o_in)
>		$(g++Unlink) $(mysql_db_o_in) -o $(mysql_db_o_out)

tz_o_out = $(build_dir)/tz.o
tz_o_in = $(pro_dir)/libs/date_3.0.1/src/tz.cpp
$(tz_o_out) : $(tz_o_in)
>		$(g++Unlink) $(tz_o_in) -o $(tz_o_out)

main_o_dependent = $(src_dir)/main.cpp
main_o_out = $(build_dir)/main.o
$(main_o_out) : $(main_o_dependent)
>		$(g++Unlink) $(main_o_dependent) -o $(main_o_out)

# 清空编译产物
.PHONY : clean
clean :
>   rm -rf $(protram_output_dir)

.PHONY : clear
clear :
> 	echo $(pro_dir)
> 	echo $(objects)
>   rm -rf $(protram_output_dir)

# 执行编译结果
run :
>		$(protram_output_dir)/execute.run

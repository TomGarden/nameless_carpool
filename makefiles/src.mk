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


# 源代码文件夹
# src_dir = $(pro_dir)/src
# src 目录下 所有的编译产物都放到这个目录中 , 按照代码目录结构组织这个文件夹
protram_output_dir = $(pro_dir)/build_src
build_dir = $(protram_output_dir)/build
dynamic_lib_dir = $(pro_dir)/libs/dynamic

src = src
	main_in = $(pro_dir)/$(src)/main.cpp
	main_o = $(build_dir)/$(src)/main.o

src_model = src/model
	# user_in = $(pro_dir)/$(src_model)/user.cpp
	# user_o = $(build_dir)/$(src_model)/user.o

src_utils = src/utils
	tom_string_utils_in = $(pro_dir)/$(src_utils)/tom_string_utils.cpp 
	tom_string_utils_o = $(build_dir)/$(src_utils)/tom_string_utils.o 
	input_check_in = $(pro_dir)/$(src_utils)/input_check.cpp
	input_check_o = $(build_dir)/$(src_utils)/input_check.o 


objects = $(tom_string_utils_o)	$(input_check_o) $(fcgio_o)\
					$(main_o)                                

# -I 用于指定搜索头文件的路径
links = -lglog \
				-lfcgi -lfcgi++ -lcgicc \

stackTrace = -lboost_stacktrace_backtrace -ldl

#				-lboost_stacktrace_basic -ldl -DBOOST_STACKTRACE_LINK
# -lboost_stacktrace_backtrace  -lboost_stacktrace_addr2line -lboost_stacktrace_basic -lboost_stacktrace_noop -ldl -DBOOST_STACKTRACE_LINK
# https://stackoverflow.com/questions/52583544/boost-stack-trace-not-showing-function-names-and-line-numbers

# g++ = g++ -g -c -std=c++11 -Wall
gpp = g++ -g -std=${stdV} -Wall -ggdb -rdynamic -no-pie -fno-pie 
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


try_create_all_dependent_dir :      
>		$(call create_dir,$(build_dir)/$(src))
>		$(call create_dir,$(build_dir)/$(src_model))
>		$(call create_dir,$(build_dir)/$(src_utils))


$(protram_output_dir)/execute.run : $(objects)
>   $(gpp) $(objects) $(links) $(stackTrace) -o $(protram_output_dir)/execute.run 
>		echo "build success ..."
# > 	echo " start run ... \n"
# >		$(protram_output_dir)/execute.run

$(tom_string_utils_o) : $(tom_string_utils_in)
>		$(g++) $(tom_string_utils_in) -o $(tom_string_utils_o)

$(input_check_o) : $(input_check_in)
>		$(g++) $(input_check_in) -o $(input_check_o)

# $(user_o) : $(user_in)
# >		$(g++) $(user_in) -o $(user_o)

$(main_o) : $(main_in)
>		$(g++) $(main_in) -o $(main_o) 


# 清空编译产物
.PHONY : clean
clean :
>   rm -rf $(protram_output_dir)

# 执行编译结果
run :
>		$(protram_output_dir)/execute.run \
			--debug													\
			--header={one:1,two:"2"} 				\
			--method=POST										\
			--function=/api/功能细节				 \
			--body={msg:"消息内容"}

